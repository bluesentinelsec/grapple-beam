/* Original Grapple code (zlib). Scoped score automation applied to independent notes. */
#include "chip_score.h"

static bool Matches(const ScoreControl *c, const ScoreNote *n)
{
    return c->event.track == n->part && (c->staff < 0 || c->staff == n->staff) &&
           (!c->voice || !*c->voice || SDL_strcmp(c->voice, n->voice) == 0);
}

static int SDLCALL CompareControls(const void *left, const void *right)
{
    const ScoreControl *a = left, *b = right;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return (a->event.order > b->event.order) - (a->event.order < b->event.order);
}

static int SDLCALL CompareNotes(const void *left, const void *right)
{
    const ScoreNote *a = *(const ScoreNote *const *)left;
    const ScoreNote *b = *(const ScoreNote *const *)right;
    if (a->expression.lane != b->expression.lane)
        return a->expression.lane < b->expression.lane ? -1 : 1;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return (a > b) - (a < b);
}

static size_t Upper(const ScoreControl *const *controls, size_t count, Sint64 at)
{
    size_t low = 0, high = count;
    while (low < high)
    {
        const size_t middle = low + (high - low) / 2;
        if (controls[middle]->start <= at)
            low = middle + 1;
        else
            high = middle;
    }
    return low;
}

static double Value(const ScoreControl *c, Sint64 at)
{
    if (!c)
        return 80;
    const double fraction =
        c->duration > 0 ? SDL_clamp((double)(at - c->start) / (double)c->duration, 0, 1) : 1;
    return c->value + (c->target - c->value) * fraction;
}

static bool GainEvent(ScoreReader *r, const ScoreNote *n, Uint32 id, Sint64 at, Sint64 duration,
                      double first, double last)
{
    ChipEvent event = {0};
    event.status = 0xf1;
    event.track = (Uint16)n->part;
    event.note_id = id;
    event.tick = (Uint64)at;
    event.duration = (Uint64)duration;
    event.value = (float)first;
    event.target = (float)last;
    return Chip_AppendEvent(r->song, event);
}

static bool Dynamics(ScoreReader *r, ScoreNote *n, Uint32 id, const ScoreControl *const *controls,
                     size_t count)
{
    if (!count || *Chip_XmlAttribute(n->node, "dynamics"))
        return true;
    const Sint64 start = n->start + n->attack;
    const Sint64 end = n->start + n->duration + n->release;
    if (start < 0 || end <= start)
        return Chip_ScoreError(r, n->node, "invalid note span for dynamics");
    size_t index = Upper(controls, count, start);
    const ScoreControl *active = index ? controls[index - 1] : NULL;
    const double initial = Value(active, start);
    const double divisor = SDL_max(initial, 1);
    n->velocity = SDL_clamp((int)SDL_round(initial) + n->velocity - 80, 1, 127);
    Sint64 position = start;
    while (position < end)
    {
        while (index < count && controls[index]->start <= position)
            active = controls[index++];
        Sint64 next = index < count ? SDL_min(end, controls[index]->start) : end;
        if (active && active->duration > 0 && active->start + active->duration > position)
            next = SDL_min(next, active->start + active->duration);
        if (next <= position)
            return Chip_ScoreError(r, n->node, "non-increasing dynamic curve");
        if (!GainEvent(r, n, id, position, next - position, Value(active, position) / divisor,
                       Value(active, next) / divisor))
            return false;
        position = next;
    }
    return true;
}

static Sint64 PedalEnd(const ScoreControl *const *controls, size_t count, Sint64 start, Sint64 end,
                       int number, Sint64 song_end)
{
    if (!count)
        return end;
    size_t index = Upper(controls, count, end);
    if (!index || controls[index - 1]->value < 64)
        return end;
    const ScoreControl *active = controls[index - 1];
    if (number == 66 && active->start <= start)
        return end;
    for (; index < count; ++index)
        if (controls[index]->value < 64)
            return controls[index]->start;
    return song_end;
}

bool Chip_ApplySwing(ScoreReader *r)
{
    bool has_swing = false;
    for (size_t c = 0; c < r->control_count; ++c)
        has_swing |= r->controls[c].kind == SCORE_SWING;
    if (!has_swing)
        return true;
    SDL_qsort(r->controls, r->control_count, sizeof(*r->controls), CompareControls);
    for (size_t i = 0; i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if (n->skipped || n->grace || Chip_XmlChild(n->node, "time-modification"))
            continue;
        const ScoreControl *swing = NULL;
        for (size_t c = 0; c < r->control_count && r->controls[c].start <= n->start; ++c)
            if (r->controls[c].kind == SCORE_SWING && Matches(&r->controls[c], n))
                swing = &r->controls[c];
        if (!swing || swing->value == 0.5)
            continue;
        const char *type = Chip_XmlText(n->node, "type");
        if ((swing->target == 0.5 && SDL_strcmp(type, "eighth") != 0) ||
            (swing->target == 0.25 && SDL_strcmp(type, "16th") != 0))
            continue;
        const Sint64 unit = (Sint64)SDL_round(swing->target * r->song->info.ticks_per_quarter);
        if (unit <= 0 || n->duration != unit)
            continue;
        const Sint64 base = r->lengths[n->measure];
        const Sint64 position = (n->start - base) % (2 * unit);
        const Sint64 first = (Sint64)SDL_round((double)(2 * unit) * swing->value);
        if (position == 0)
            n->duration = first;
        else if (position == unit)
        {
            n->start += first - unit;
            n->duration = 2 * unit - first;
        }
    }
    return true;
}

bool Chip_ApplyDirections(ScoreReader *r)
{
    SDL_qsort(r->controls, r->control_count, sizeof(*r->controls), CompareControls);
    ScoreNote **notes = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*notes));
    const ScoreControl **dynamic = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*dynamic));
    const ScoreControl **damper = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*damper));
    const ScoreControl **sostenuto = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*sostenuto));
    const ScoreControl **soft = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*soft));
    if (!notes || !dynamic || !damper || !sostenuto || !soft)
    {
        SDL_free(notes);
        SDL_free(dynamic);
        SDL_free(damper);
        SDL_free(sostenuto);
        SDL_free(soft);
        return false;
    }
    size_t count = 0;
    for (size_t i = 0; i < r->note_count; ++i)
        if (!r->notes[i].skipped && !r->notes[i].rest)
            notes[count++] = &r->notes[i];
    SDL_qsort(notes, count, sizeof(*notes), CompareNotes);
    bool ok = true;
    for (size_t first = 0; ok && first < count;)
    {
        size_t end = first + 1;
        while (end < count && notes[end]->expression.lane == notes[first]->expression.lane)
            ++end;
        size_t dynamics = 0, dampers = 0, sostenutos = 0, softs = 0;
        for (size_t c = 0; c < r->control_count; ++c)
        {
            const ScoreControl *control = &r->controls[c];
            if (!Matches(control, notes[first]))
                continue;
            if (control->kind == SCORE_DYNAMIC)
                dynamic[dynamics++] = control;
            if (control->kind == SCORE_PEDAL)
            {
                if (control->number == 64)
                    damper[dampers++] = control;
                else if (control->number == 66)
                    sostenuto[sostenutos++] = control;
                else
                    soft[softs++] = control;
            }
        }
        for (size_t i = first; ok && i < end; ++i)
        {
            ScoreNote *n = notes[i];
            r->part = n->part;
            r->measure = n->source_measure;
            const Sint64 nominal_end =
                n->start + (Sint64)SDL_round((double)n->duration * n->gate) + n->release;
            const Sint64 song_end = (Sint64)r->song->info.duration_ticks;
            const Sint64 held_end =
                SDL_max(PedalEnd(damper, dampers, n->start, nominal_end, 64, song_end),
                        PedalEnd(sostenuto, sostenutos, n->start, nominal_end, 66, song_end));
            if (held_end > nominal_end && !Chip_XmlChild(n->node, "unpitched"))
            {
                n->duration = held_end - n->start;
                n->gate = 1;
                n->release = 0;
            }
            const size_t soft_index = Upper(soft, softs, n->start);
            if (soft_index)
            {
                const float amount = (float)soft[soft_index - 1]->value / 127;
                n->expression.gain *= 1 - amount * 0.3f;
                n->expression.brightness *= 1 - amount * 0.3f;
            }
            ok = Dynamics(r, n, (Uint32)(n - r->notes) + 1, dynamic, dynamics);
        }
        first = end;
    }
    SDL_free(notes);
    SDL_free(dynamic);
    SDL_free(damper);
    SDL_free(sostenuto);
    SDL_free(soft);
    return ok;
}
