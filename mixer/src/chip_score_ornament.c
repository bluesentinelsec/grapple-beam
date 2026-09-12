/* Original Grapple code (zlib). Bounded ornament expansion into ordinary notes. */
#include "chip_score.h"

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static const ChipXmlNode *Ornaments(const ScoreNote *n)
{
    return Chip_XmlChild(Chip_XmlChild(n->node, "notations"), "ornaments");
}

static const ChipXmlNode *Mark(const ScoreNote *n)
{
    const ChipXmlNode *ornaments = Ornaments(n);
    for (const ChipXmlNode *c = ornaments ? ornaments->children : NULL; c; c = c->next)
        if (Named(c, "trill-mark") || Named(c, "mordent") || Named(c, "inverted-mordent") ||
            Named(c, "turn") || Named(c, "inverted-turn") || Named(c, "delayed-turn") ||
            Named(c, "delayed-inverted-turn") || Named(c, "tremolo"))
            return c;
    return NULL;
}

static bool Append(ScoreNote **notes, size_t *count, size_t *capacity, ScoreNote note)
{
    if (*count >= CHIP_SONG_MAX_EVENTS / 2)
        return SDL_SetError("MusicXML: expanded ornament note limit exceeded");
    if (!Chip_ScoreGrow((void **)notes, capacity, *count, sizeof(**notes)))
        return false;
    (*notes)[(*count)++] = note;
    return true;
}

static double Accidental(ScoreReader *r, const ChipXmlNode *mark, double fallback)
{
    const char *text = mark->text ? mark->text : "";
    if (SDL_strcmp(text, "natural") == 0)
        return 0;
    if (SDL_strcmp(text, "sharp") == 0)
        return 1;
    if (SDL_strcmp(text, "flat") == 0)
        return -1;
    if (SDL_strcmp(text, "double-sharp") == 0 || SDL_strcmp(text, "sharp-sharp") == 0)
        return 2;
    if (SDL_strcmp(text, "flat-flat") == 0)
        return -2;
    if (SDL_strcmp(text, "quarter-sharp") == 0)
        return 0.5;
    if (SDL_strcmp(text, "quarter-flat") == 0)
        return -0.5;
    Chip_ScoreError(r, mark, "unsupported ornament accidental");
    return fallback;
}

static bool Neighbors(ScoreReader *r, const ScoreNote *n, const ChipXmlNode *mark, double *upper,
                      double *lower)
{
    *upper = n->upper_pitch;
    *lower = n->lower_pitch;
    const char *step = Chip_XmlAttribute(mark, "trill-step");
    if (*step)
    {
        if (SDL_strcmp(step, "half") != 0 && SDL_strcmp(step, "whole") != 0 &&
            SDL_strcmp(step, "unison") != 0)
            return Chip_ScoreError(r, mark, "invalid trill step");
        const double interval = SDL_strcmp(step, "whole") == 0  ? 2
                                : SDL_strcmp(step, "half") == 0 ? 1
                                                                : 0;
        *upper = (double)n->pitch + n->expression.tuning + interval;
        *lower = (double)n->pitch + n->expression.tuning - interval;
    }
    const ChipXmlNode *ornaments = Ornaments(n);
    for (const ChipXmlNode *c = ornaments ? ornaments->children : NULL; c; c = c->next)
        if (Named(c, "accidental-mark"))
        {
            const bool below = SDL_strcmp(Chip_XmlAttribute(c, "placement"), "below") == 0;
            const ChipXmlNode *pitch = Chip_XmlChild(n->node, "pitch");
            const char *text = Chip_XmlText(pitch, "step");
            static const char letters[] = "CDEFGAB";
            const char *degree = *text ? SDL_strchr(letters, *text) : NULL;
            if (!degree)
                return Chip_ScoreError(r, c, "ornament requires a pitched note");
            static const int natural[] = {0, 2, 4, 5, 7, 9, 11};
            const int index = (int)(degree - letters);
            const double alteration =
                Chip_ScoreDecimal(r, pitch, Chip_XmlText(pitch, "alter"), 0, -12, 12);
            const double base =
                (double)n->pitch + n->expression.tuning - natural[index] - alteration;
            const int next = (index + (below ? 6 : 1)) % 7;
            const int octave = below && index == 0 ? -12 : !below && index == 6 ? 12 : 0;
            const double value = base + natural[next] + octave + Accidental(r, c, 0);
            if (below)
                *lower = value;
            else
                *upper = value;
        }
    return !r->failed;
}

static bool Expand(ScoreReader *r, ScoreNote n, const ScoreNote *partner, const ChipXmlNode *mark,
                   ScoreNote **notes, size_t *count, size_t *capacity)
{
    if (!Chip_ScoreApproximation(
            r, &n, "Ornaments use the documented configurable chiptune performance policy"))
        return false;
    const bool tremolo = Named(mark, "tremolo");
    const bool trill = Named(mark, "trill-mark");
    const bool mordent = Named(mark, "mordent") || Named(mark, "inverted-mordent");
    const bool inverted = Named(mark, "inverted-mordent") || Named(mark, "inverted-turn") ||
                          Named(mark, "delayed-inverted-turn");
    const bool delayed = Named(mark, "delayed-turn") || Named(mark, "delayed-inverted-turn");
    double upper = 0, lower = 0;
    if (!tremolo && !Neighbors(r, &n, mark, &upper, &lower))
        return false;
    Sint64 duration = n.duration + (partner ? partner->duration : 0);
    Sint64 spacing =
        SDL_max(1, (Sint64)SDL_round(r->options->ornament_beats * r->song->info.ticks_per_quarter));
    if (tremolo && SDL_strcmp(Chip_XmlAttribute(mark, "type"), "unmeasured") != 0)
    {
        int beams = 0;
        for (const ChipXmlNode *c = n.node->children; c; c = c->next)
            beams += Named(c, "beam");
        const int strokes = Chip_ScoreInteger(r, mark, mark->text ? mark->text : "", 3, 0, 8);
        spacing = SDL_max(1, r->song->info.ticks_per_quarter / (1 << SDL_min(16, strokes + beams)));
    }
    int attacks = mordent                ? 3
                  : (!trill && !tremolo) ? 4
                                         : (int)SDL_min(65536, (duration + spacing - 1) / spacing);
    const char *beats = Chip_XmlAttribute(mark, "beats");
    if (*beats)
        attacks = Chip_ScoreInteger(r, mark, beats, attacks, 2, 65536);
    if (attacks < 1 || attacks > duration || attacks == 65536)
        return Chip_ScoreError(r, mark, "ornament exceeds timing or repetition limits");
    Sint64 offset = delayed ? duration / 2 : 0;
    if (offset)
    {
        ScoreNote held = n;
        held.duration = offset;
        if (!Append(notes, count, capacity, held))
            return false;
    }
    const Sint64 available = duration - offset;
    const double second =
        Chip_ScoreDecimal(r, mark, Chip_XmlAttribute(mark, "second-beat"), 0, 0, 100) / 100;
    const double last =
        Chip_ScoreDecimal(r, mark, Chip_XmlAttribute(mark, "last-beat"), 100, 0, 100) / 100;
    const bool accelerate = SDL_strcmp(Chip_XmlAttribute(mark, "accelerate"), "yes") == 0;
    Sint64 previous = 0;
    for (int i = 0; i < attacks; ++i)
    {
        ScoreNote note = partner && (i & 1) ? *partner : n;
        note.skipped = false;
        Sint64 end =
            i + 1 == attacks ? available : (Sint64)SDL_round((double)available * (i + 1) / attacks);
        if (mordent && !*beats)
            end = i == 0   ? SDL_min(spacing, available / 3)
                  : i == 1 ? SDL_min(spacing * 2, available * 2 / 3)
                           : available;
        if (*Chip_XmlAttribute(mark, "second-beat") && i == 0)
            end = (Sint64)SDL_round((double)available * second);
        if (*Chip_XmlAttribute(mark, "last-beat") && i + 2 == attacks)
            end = (Sint64)SDL_round((double)available * last);
        if (accelerate && !*Chip_XmlAttribute(mark, "second-beat") &&
            !*Chip_XmlAttribute(mark, "last-beat") && i + 1 != attacks)
        {
            const double fraction = (double)(i + 1) / attacks;
            end = (Sint64)SDL_round((double)available * (2 * fraction - fraction * fraction));
        }
        if (end <= previous)
            return Chip_ScoreError(r, mark, "ornament timing must increase monotonically");
        note.start = n.start + offset + previous;
        note.duration = end - previous;
        note.attack = i == 0 ? n.attack : 0;
        note.release = i + 1 == attacks ? n.release : 0;
        note.expression.legato = !tremolo && (i > 0 || n.expression.legato);
        if (!tremolo)
        {
            int degree = 0;
            if (trill)
            {
                const char *start = Chip_XmlAttribute(mark, "start-note");
                const bool upper_start = SDL_strcmp(start, "upper") == 0;
                const bool lower_start = SDL_strcmp(start, "below") == 0;
                degree = ((i & 1) != (int)upper_start) ? 1 : 0;
                if (lower_start)
                    degree = (i & 1) ? 0 : -1;
            }
            else if (mordent)
                degree = i & 1 ? (inverted ? 1 : -1) : 0;
            else
            {
                static const int turn[] = {1, 0, -1, 0};
                degree = turn[i % 4] * (inverted ? -1 : 1);
            }
            const double pitch = degree > 0   ? upper
                                 : degree < 0 ? lower
                                              : (double)n.pitch + n.expression.tuning;
            note.pitch = (int)SDL_floor(pitch);
            note.expression.tuning = (float)(pitch - note.pitch);
            if (note.pitch < 0 || note.pitch > 127)
                return Chip_ScoreError(r, mark, "ornament pitch outside synthesis range");
        }
        if (!Append(notes, count, capacity, note))
            return false;
        previous = end;
    }
    return !r->failed;
}

bool Chip_ExpandOrnaments(ScoreReader *r)
{
    ScoreNote *notes = NULL;
    size_t count = 0, capacity = 0;
    bool ok = true;
    size_t *next = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*next));
    size_t lanes[4096] = {0};
    if (!next)
        return false;
    for (size_t i = r->note_count; i > 0;)
    {
        --i;
        next[i] = lanes[r->notes[i].expression.lane];
        if (!r->notes[i].skipped && !r->notes[i].grace && !r->notes[i].rest)
            lanes[r->notes[i].expression.lane] = i + 1;
    }
    for (size_t i = 0; ok && i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if (n->skipped)
            continue;
        r->part = n->part;
        r->measure = n->source_measure;
        const ChipXmlNode *mark = Mark(n);
        if (!mark || n->rest)
        {
            ok = Append(&notes, &count, &capacity, *n);
            continue;
        }
        ScoreNote *partner = NULL;
        const char *type = Chip_XmlAttribute(mark, "type");
        if (Named(mark, "tremolo") && SDL_strcmp(type, "start") == 0)
        {
            partner = next[i] ? &r->notes[next[i] - 1] : NULL;
            const ChipXmlNode *stop = partner ? Mark(partner) : NULL;
            if (!partner || !Named(stop, "tremolo") ||
                SDL_strcmp(Chip_XmlAttribute(stop, "type"), "stop") != 0 ||
                partner->start != n->start + n->duration)
            {
                ok =
                    Chip_ScoreError(r, mark, "alternating tremolo needs an adjacent matching stop");
                break;
            }
            partner->skipped = true;
        }
        else if (Named(mark, "tremolo") && SDL_strcmp(type, "stop") == 0)
        {
            ok = Chip_ScoreError(r, mark, "unpaired alternating tremolo stop");
            break;
        }
        ok = Expand(r, *n, partner, mark, &notes, &count, &capacity);
    }
    SDL_free(next);
    if (ok)
    {
        SDL_free(r->notes);
        r->notes = notes;
        r->note_count = count;
        r->note_capacity = capacity;
    }
    else
        SDL_free(notes);
    return ok;
}
