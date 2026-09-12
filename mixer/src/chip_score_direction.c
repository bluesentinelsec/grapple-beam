/* Original Grapple code (zlib). Scoped MusicXML directions and source-time curves. */
#include "chip_score.h"

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static bool SameScope(const ScoreControl *a, const ScoreControl *b)
{
    return a->event.track == b->event.track && a->staff == b->staff &&
           SDL_strcmp(a->voice ? a->voice : "", b->voice ? b->voice : "") == 0;
}

static bool AppliesToScope(const ScoreControl *a, const ScoreControl *b)
{
    return a->event.track == b->event.track && (a->staff < 0 || a->staff == b->staff) &&
           (!a->voice || !*a->voice || (b->voice && SDL_strcmp(a->voice, b->voice) == 0));
}

static bool Append(ScoreReader *r, ScoreControl c)
{
    if (!Chip_ScoreGrow((void **)&r->controls, &r->control_capacity, r->control_count, sizeof(c)))
        return false;
    c.event.order = (Uint32)r->control_count;
    r->controls[r->control_count++] = c;
    return true;
}

static ScoreControl Direction(ScoreReader *r, const ChipXmlNode *node, Sint64 cursor)
{
    ScoreControl c = {0};
    c.measure = r->measure;
    c.event.track = (Uint16)r->part;
    c.start = cursor;
    c.node = node;
    const char *staff = Chip_XmlText(node, "staff");
    c.staff = *staff ? Chip_ScoreInteger(r, node, staff, 1, 1, SCORE_MAX_STAVES) - 1 : -1;
    c.voice = Chip_XmlText(node, "voice");
    return c;
}

static int PedalValue(ScoreReader *r, const ChipXmlNode *node, const char *value)
{
    if (SDL_strcmp(value, "yes") == 0)
        return 127;
    if (SDL_strcmp(value, "no") == 0)
        return 0;
    return (int)SDL_round(Chip_ScoreDecimal(r, node, value, 0, 0, 100) * 1.27);
}

bool Chip_ReadDirections(ScoreReader *r, const ChipXmlNode *node, const ChipXmlNode *sound,
                         Sint64 cursor)
{
    ScoreControl base = Direction(r, node, cursor);
    const char *dynamics = Chip_XmlAttribute(sound, "dynamics");
    if (*dynamics)
    {
        ScoreControl c = base;
        c.kind = SCORE_DYNAMIC;
        c.value = c.target =
            SDL_min(127, Chip_ScoreDecimal(r, sound, dynamics, 100, 0, 1000) * 0.9);
        if (!Append(r, c))
            return false;
    }
    const char *const pedals[] = {"damper-pedal", "sostenuto-pedal", "soft-pedal"};
    const int controllers[] = {64, 66, 67};
    for (size_t i = 0; i < SDL_arraysize(pedals); ++i)
        if (*Chip_XmlAttribute(sound, pedals[i]))
        {
            ScoreControl c = base;
            c.kind = SCORE_PEDAL;
            c.number = controllers[i];
            c.value = c.target = PedalValue(r, sound, Chip_XmlAttribute(sound, pedals[i]));
            if (!Append(r, c))
                return false;
        }
    const ChipXmlNode *swing = Chip_XmlChild(sound, "swing");
    if (swing)
    {
        ScoreControl c = base;
        c.kind = SCORE_SWING;
        c.value = 0.5;
        c.target = 0.5;
        if (!Chip_XmlChild(swing, "straight"))
        {
            const int first = Chip_ScoreInteger(r, swing, Chip_XmlText(swing, "first"), 2, 1, 1000);
            const int second =
                Chip_ScoreInteger(r, swing, Chip_XmlText(swing, "second"), 1, 1, 1000);
            c.value = (double)first / (first + second);
            const char *type = Chip_XmlText(swing, "swing-type");
            if (*type && SDL_strcmp(type, "eighth") != 0 && SDL_strcmp(type, "16th") != 0)
                return Chip_ScoreError(r, swing, "swing type must be eighth or 16th");
            c.target = SDL_strcmp(type, "16th") == 0 ? 0.25 : 0.5;
        }
        if (!Append(r, c))
            return false;
    }
    for (const ChipXmlNode *type = node->children; type; type = type->next)
    {
        if (!Named(type, "direction-type"))
            continue;
        for (const ChipXmlNode *mark = type->children; mark; mark = mark->next)
        {
            ScoreControl c = base;
            c.node = mark;
            if (Named(mark, "dynamics") && mark->children && !*dynamics)
            {
                c.kind = SCORE_DYNAMIC;
                const char *name = mark->children->name;
                c.value = c.target = Chip_ScoreDynamic(name);
                if (SDL_strcmp(name, "fp") == 0 || SDL_strcmp(name, "sfp") == 0 ||
                    SDL_strcmp(name, "sfz") == 0 || SDL_strcmp(name, "sf") == 0 ||
                    SDL_strcmp(name, "sffz") == 0 || SDL_strcmp(name, "rfz") == 0)
                {
                    c.value = SDL_strcmp(name, "fp") == 0 ? 96 : 120;
                    c.target = SDL_strstr(name, "p") ? 48 : -1;
                    c.duration = SDL_max(1, r->song->info.ticks_per_quarter / 8);
                }
            }
            else if (Named(mark, "wedge"))
            {
                c.kind = SCORE_WEDGE;
                c.number = Chip_ScoreInteger(r, mark, Chip_XmlAttribute(mark, "number"), 1, 1, 16);
                const char *name = Chip_XmlAttribute(mark, "type");
                if (SDL_strcmp(name, "continue") == 0)
                    continue;
                if (SDL_strcmp(name, "crescendo") == 0)
                    c.value = 1;
                else if (SDL_strcmp(name, "diminuendo") == 0)
                    c.value = -1;
                else if (SDL_strcmp(name, "stop") != 0)
                    return Chip_ScoreError(r, mark, "invalid wedge type");
                c.target = SDL_strcmp(Chip_XmlAttribute(mark, "niente"), "yes") == 0;
            }
            else if (Named(mark, "pedal"))
            {
                const char *name = Chip_XmlAttribute(mark, "type");
                c.kind = SCORE_PEDAL;
                c.number = SDL_strcmp(name, "sostenuto") == 0 ? 66 : 64;
                c.event.a =
                    (Uint8)Chip_ScoreInteger(r, mark, Chip_XmlAttribute(mark, "number"), 1, 1, 16);
                if (SDL_strcmp(name, "stop") == 0 || SDL_strcmp(name, "discontinue") == 0 ||
                    SDL_strcmp(name, "change") == 0)
                {
                    for (size_t i = r->control_count; i > 0;)
                    {
                        const ScoreControl *prior = &r->controls[--i];
                        if (prior->kind == SCORE_PEDAL && prior->event.a == c.event.a &&
                            SameScope(prior, &c))
                        {
                            c.number = prior->number;
                            break;
                        }
                    }
                    if (SDL_strcmp(name, "change") == 0)
                    {
                        if (!Append(r, c))
                            return false;
                        c.value = c.target = 127;
                    }
                }
                else
                    c.value = c.target = 127;
                const char *attribute = c.number == 66 ? "sostenuto-pedal" : "damper-pedal";
                if (*Chip_XmlAttribute(sound, attribute))
                    continue;
            }
            else if (Named(mark, "words") && mark->text)
            {
                char word[64];
                size_t length = 0;
                for (const char *text = mark->text; *text && length + 1 < sizeof(word); ++text)
                    if (SDL_isalpha((unsigned char)*text))
                        word[length++] = (char)SDL_tolower((unsigned char)*text);
                word[length] = 0;
                if (SDL_strcmp(word, "rit") == 0 || SDL_strcmp(word, "ritardando") == 0 ||
                    SDL_strcmp(word, "rall") == 0 || SDL_strcmp(word, "rallentando") == 0)
                {
                    c.kind = SCORE_TEMPO_RAMP;
                    c.value = 0.75;
                }
                else if (SDL_strcmp(word, "accel") == 0 || SDL_strcmp(word, "accelerando") == 0)
                {
                    c.kind = SCORE_TEMPO_RAMP;
                    c.value = 1.25;
                }
                else if (!swing &&
                         (SDL_strcmp(word, "swing") == 0 || SDL_strcmp(word, "shuffle") == 0 ||
                          SDL_strcmp(word, "straight") == 0))
                {
                    c.kind = SCORE_SWING;
                    c.value = SDL_strcmp(word, "straight") == 0
                                  ? 0.5
                                  : r->options->swing_ratio / (r->options->swing_ratio + 1);
                    c.target = 0.5;
                }
                else
                    continue;
            }
            else
                continue;
            if (!Append(r, c))
                return false;
        }
    }
    return !r->failed;
}

static int SDLCALL Compare(const void *left, const void *right)
{
    const ScoreControl *a = left, *b = right;
    if (a->event.tick != b->event.tick)
        return a->event.tick < b->event.tick ? -1 : 1;
    return (a->event.order > b->event.order) - (a->event.order < b->event.order);
}

static double Value(const ScoreControl *c, Sint64 at)
{
    const double fraction =
        c->duration > 0
            ? SDL_clamp((double)(at - (Sint64)c->event.tick) / (double)c->duration, 0, 1)
            : 1;
    return c->value + (c->target - c->value) * fraction;
}

static double PreviousDynamic(const ScoreControl *controls, size_t count, const ScoreControl *scope,
                              Sint64 at)
{
    double value = 80;
    for (size_t i = 0; i < count; ++i)
        if (controls[i].kind == SCORE_DYNAMIC && AppliesToScope(&controls[i], scope) &&
            controls[i].event.tick <= (Uint64)at)
            value = Value(&controls[i], at);
    return value;
}

static bool TempoRamp(ScoreReader *r, size_t index, size_t source_count, const Sint64 *positions)
{
    ScoreControl c = r->controls[index];
    const Sint64 start = (Sint64)c.event.tick;
    Sint64 end =
        SDL_min(positions[r->measures], start + (Sint64)r->song->info.ticks_per_quarter * 4);
    double bpm = 120;
    for (size_t i = 0; i < index; ++i)
        if (r->controls[i].event.tempo)
            bpm = 60000000.0 / r->controls[i].event.tempo;
    double target = SDL_clamp(bpm * c.value, 20, 600);
    for (size_t i = index + 1; i < source_count; ++i)
        if (r->controls[i].event.tempo && r->controls[i].event.tick > c.event.tick)
        {
            end = (Sint64)r->controls[i].event.tick;
            target = 60000000.0 / r->controls[i].event.tempo;
            break;
        }
    r->controls[index].kind = SCORE_CONTROL;
    if (end <= start)
        return Chip_ScoreError(r, c.node, "tempo ramp has no following duration");
    const Sint64 steps =
        SDL_min(4096, SDL_max(1, (end - start) * 32 / r->song->info.ticks_per_quarter));
    for (Sint64 step = 0; step <= steps; ++step)
    {
        const Sint64 tick = start + (end - start) * step / steps;
        const double value = bpm + (target - bpm) * (double)step / (double)steps;
        ScoreControl point = {0};
        point.event.track = c.event.track;
        point.event.tempo = (Uint32)SDL_round(60000000 / value);
        int low = 0, high = r->measures;
        while (low + 1 < high)
        {
            const int mid = low + (high - low) / 2;
            if (positions[mid] <= tick)
                low = mid;
            else
                high = mid;
        }
        point.measure = low;
        point.start = tick - positions[low];
        point.event.tick = (Uint64)tick;
        if (!Append(r, point))
            return false;
    }
    return true;
}

bool Chip_PrepareDirections(ScoreReader *r)
{
    Sint64 *positions = SDL_calloc((size_t)r->measures + 1, sizeof(*positions));
    if (!positions)
        return false;
    for (int m = 0; m < r->measures; ++m)
        positions[m + 1] = positions[m] + r->lengths[m];
    for (size_t i = 0; i < r->control_count; ++i)
        r->controls[i].event.tick =
            (Uint64)(positions[r->controls[i].measure] + r->controls[i].start);
    SDL_qsort(r->controls, r->control_count, sizeof(*r->controls), Compare);
    const size_t count = r->control_count;
    bool ok = true;
    for (size_t i = 0; ok && i < count; ++i)
    {
        ScoreControl *c = &r->controls[i];
        r->part = c->event.track;
        r->measure = c->measure;
        if (c->kind == SCORE_DYNAMIC && c->target < 0)
            c->target = PreviousDynamic(r->controls, i, c, (Sint64)c->event.tick);
        if (c->kind == SCORE_TEMPO_RAMP)
        {
            ok = TempoRamp(r, i, count, positions);
            continue;
        }
        if (c->kind != SCORE_WEDGE)
            continue;
        if (c->value == 0)
        {
            ok = Chip_ScoreError(r, c->node, "wedge stop without matching start");
            break;
        }
        size_t stop = i + 1;
        for (; stop < count; ++stop)
            if (r->controls[stop].kind == SCORE_WEDGE && r->controls[stop].number == c->number &&
                SameScope(c, &r->controls[stop]))
                break;
        if (stop == count || r->controls[stop].value != 0 ||
            r->controls[stop].event.tick <= c->event.tick)
        {
            ok = Chip_ScoreError(r, c->node, "wedge needs a later matching stop");
            break;
        }
        ScoreControl *last = &r->controls[stop];
        const double sign = c->value;
        const double initial = c->target != 0 && sign > 0
                                   ? 0
                                   : PreviousDynamic(r->controls, i, c, (Sint64)c->event.tick);
        double target = SDL_clamp(initial + sign * 32, 0, 127);
        for (size_t d = i + 1; d < count && r->controls[d].event.tick <= last->event.tick; ++d)
            if (r->controls[d].kind == SCORE_DYNAMIC && AppliesToScope(&r->controls[d], c) &&
                r->controls[d].event.tick == last->event.tick)
                target = r->controls[d].value;
        if (last->target != 0 && sign < 0)
            target = 0;
        c->kind = SCORE_DYNAMIC;
        c->value = initial;
        c->target = target;
        c->duration = (Sint64)(last->event.tick - c->event.tick);
        last->kind = SCORE_DYNAMIC;
        last->value = last->target = target;
    }
    SDL_free(positions);
    return ok;
}

bool Chip_RestoreDirections(ScoreReader *r, const ScoreControl *controls, size_t count,
                            const Sint64 *positions, int source)
{
    const Sint64 at = positions[source];
    for (size_t i = 0; i < count; ++i)
    {
        if (controls[i].kind == SCORE_CONTROL)
            continue;
        bool seen = false;
        for (size_t prior = 0; prior < i; ++prior)
            if (controls[prior].kind == controls[i].kind &&
                controls[prior].number == controls[i].number &&
                SameScope(&controls[prior], &controls[i]))
            {
                seen = true;
                break;
            }
        if (seen)
            continue;
        ScoreControl c = controls[i];
        c.start = 0;
        c.measure = r->measure;
        c.duration = 0;
        c.value = c.target = c.kind == SCORE_DYNAMIC ? 80 : c.kind == SCORE_SWING ? 0.5 : 0;
        if (!Append(r, c))
            return false;
    }
    /* Replay scoped source controls in order; later instructions replace earlier state. */
    for (size_t i = 0; i < count; ++i)
    {
        if (controls[i].kind == SCORE_CONTROL)
            continue;
        ScoreControl c = controls[i];
        const Sint64 tick = positions[c.measure] + c.start;
        if (tick >= at)
            continue;
        c.measure = r->measure;
        c.start = 0;
        c.event.tick = (Uint64)tick;
        if (c.kind == SCORE_DYNAMIC)
        {
            c.value = Value(&c, at);
            c.duration = SDL_max(0, tick + c.duration - at);
            if (!c.duration)
                c.target = c.value;
        }
        if (!Append(r, c))
            return false;
    }
    return true;
}
