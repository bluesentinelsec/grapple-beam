/* Original Grapple code (zlib). Ensemble pauses and local expressive timing. */
#include "chip_score.h"

typedef struct ScorePause
{
    Sint64 tick, length, prefix;
    bool sustain;
} ScorePause;

static const ChipXmlNode *Find(const ChipXmlNode *node, const char *name)
{
    for (; node; node = node->next)
    {
        if (SDL_strcmp(node->name, name) == 0)
            return node;
        const ChipXmlNode *child = Find(node->children, name);
        if (child)
            return child;
    }
    return NULL;
}

static int SDLCALL CompareLines(const void *left, const void *right)
{
    const ScoreNote *a = *(const ScoreNote *const *)left;
    const ScoreNote *b = *(const ScoreNote *const *)right;
    if (a->expression.lane != b->expression.lane)
        return a->expression.lane < b->expression.lane ? -1 : 1;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    /* Source order places graces ahead of their principal note, including chords. */
    return (a > b) - (a < b);
}

static int SDLCALL ComparePauses(const void *left, const void *right)
{
    const ScorePause *a = left, *b = right;
    return (a->tick > b->tick) - (a->tick < b->tick);
}

static bool Pause(ScorePause **pauses, size_t *count, size_t *capacity, Sint64 tick, Sint64 length,
                  bool sustain)
{
    if (length <= 0)
        return true;
    if (!Chip_ScoreGrow((void **)pauses, capacity, *count, sizeof(**pauses)))
        return false;
    (*pauses)[(*count)++] = (ScorePause){tick, length, 0, sustain};
    return true;
}

static Sint64 Map(const ScorePause *pauses, size_t count, Sint64 tick, bool onset)
{
    size_t low = 0, high = count;
    while (low < high)
    {
        const size_t middle = low + (high - low) / 2;
        if (pauses[middle].tick < tick ||
            (pauses[middle].tick == tick && (onset || pauses[middle].sustain)))
            low = middle + 1;
        else
            high = middle;
    }
    return tick + (low ? pauses[low - 1].prefix : 0);
}

static bool GraceGroup(ScoreReader *r, ScoreNote **notes, size_t first, size_t end,
                       size_t line_first, size_t line_end, ScorePause **pauses, size_t *pause_count,
                       size_t *pause_capacity)
{
    ScoreNote *n = notes[first];
    r->part = n->part;
    r->measure = n->source_measure;
    if (!Chip_ScoreApproximation(
            r, n, "Grace notes use explicit timing or the configured grace-note policy"))
        return false;
    bool make_time = n->make_time > 0;
    const ChipXmlNode *grace = Chip_XmlChild(n->node, "grace");
    const bool previous = *Chip_XmlAttribute(grace, "steal-time-previous") != 0;
    ScoreNote *donor = NULL;
    size_t donor_index = 0;
    if (!make_time)
    {
        if (previous)
        {
            for (size_t i = first; i > line_first && first - i < 4096;)
            {
                ScoreNote *candidate = notes[--i];
                if (!candidate->grace && candidate->start + candidate->duration == n->start)
                {
                    donor = candidate;
                    donor_index = i;
                    break;
                }
            }
        }
        else
            for (size_t i = end; i < line_end && notes[i]->start == n->start; ++i)
                if (!notes[i]->grace && notes[i]->start == n->start)
                {
                    donor = notes[i];
                    donor_index = i;
                    break;
                }
        if (!donor)
            return Chip_ScoreError(r, n->node, "grace group has no adjacent donor note");
    }
    Sint64 total = 0, length = 0;
    for (size_t i = first; i < end; ++i)
    {
        ScoreNote *g = notes[i];
        const ChipXmlNode *mark = Chip_XmlChild(g->node, "grace");
        if ((g->make_time > 0) != make_time ||
            ((*Chip_XmlAttribute(mark, "steal-time-previous") != 0) != previous))
            return Chip_ScoreError(r, mark, "mixed grace timing policies in one group");
        if (i == first || !Chip_XmlChild(g->node, "chord"))
        {
            if (make_time)
                length = g->make_time;
            else
            {
                const char *value = Chip_XmlAttribute(mark, previous ? "steal-time-previous"
                                                                     : "steal-time-following");
                length = *value
                             ? (Sint64)SDL_round((double)donor->duration *
                                                 Chip_ScoreDecimal(r, mark, value, 0, 0, 100) / 100)
                             : (Sint64)SDL_round(r->options->grace_beats *
                                                 r->song->info.ticks_per_quarter);
            }
            length = SDL_max(length, 1);
            g->attack = total;
            total += length;
        }
        else
            g->attack = total - length;
        g->duration = length;
    }
    if (r->failed)
        return false;
    if (!make_time && total >= donor->duration)
    {
        const bool explicit_time =
            *Chip_XmlAttribute(grace, previous ? "steal-time-previous" : "steal-time-following") !=
            0;
        if (explicit_time || donor->duration < 2)
            return Chip_ScoreError(r, grace, "grace group consumes its entire donor note");
        const Sint64 available = donor->duration / 2;
        for (size_t i = first; i < end; ++i)
        {
            notes[i]->attack =
                (Sint64)SDL_floor((double)notes[i]->attack * (double)available / (double)total);
            notes[i]->duration = SDL_max(1, (Sint64)SDL_floor((double)notes[i]->duration *
                                                              (double)available / (double)total));
        }
        total = available;
    }
    if (make_time)
    {
        if (!Pause(pauses, pause_count, pause_capacity, n->start, total, false))
            return false;
    }
    else
    {
        const Sint64 donor_start = donor->start;
        const Sint64 donor_duration = donor->duration;
        size_t donor_first = donor_index;
        while (donor_first > line_first && notes[donor_first - 1]->start == donor_start)
            --donor_first;
        for (size_t i = donor_first; i < line_end && notes[i]->start == donor_start; ++i)
        {
            ScoreNote *d = notes[i];
            if (!d->grace && d->start == donor_start && d->duration == donor_duration)
            {
                d->duration -= total;
                if (!previous)
                    d->start += total;
            }
        }
    }
    for (size_t i = first; i < end; ++i)
    {
        ScoreNote *g = notes[i];
        if (!make_time)
        {
            g->start += g->attack - (previous ? total : 0);
            g->attack = 0;
        }
        g->expression.legato = i != first;
    }
    return true;
}

static bool Graces(ScoreReader *r, ScorePause **pauses, size_t *count, size_t *capacity)
{
    ScoreNote **notes = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*notes));
    if (!notes)
        return false;
    size_t total = 0;
    for (size_t i = 0; i < r->note_count; ++i)
        if (!r->notes[i].skipped)
            notes[total++] = &r->notes[i];
    SDL_qsort(notes, total, sizeof(*notes), CompareLines);
    bool ok = true;
    for (size_t first = 0; ok && first < total;)
    {
        size_t last = first + 1;
        while (last < total && notes[last]->expression.lane == notes[first]->expression.lane)
            ++last;
        for (size_t i = first; ok && i < last; ++i)
        {
            if (!notes[i]->grace)
                continue;
            size_t end = i + 1;
            while (end < last && notes[end]->grace && notes[end]->start == notes[i]->start)
                ++end;
            ok = GraceGroup(r, notes, i, end, first, last, pauses, count, capacity);
            i = end - 1;
        }
        first = last;
    }
    SDL_free(notes);
    return ok;
}

static int RollNumber(const ScoreNote *note)
{
    const ChipXmlNode *mark = Find(Chip_XmlChild(note->node, "notations"), "arpeggiate");
    const char *number = Chip_XmlAttribute(mark, "number");
    return *number ? SDL_atoi(number) : 1;
}

static int SDLCALL CompareChords(const void *left, const void *right)
{
    const ScoreNote *a = *(const ScoreNote *const *)left;
    const ScoreNote *b = *(const ScoreNote *const *)right;
    if (a->part != b->part)
        return a->part < b->part ? -1 : 1;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    if (RollNumber(a) != RollNumber(b))
        return RollNumber(a) < RollNumber(b) ? -1 : 1;
    if (a->pitch != b->pitch)
        return a->pitch < b->pitch ? -1 : 1;
    return (a > b) - (a < b);
}

static bool Arpeggios(ScoreReader *r)
{
    ScoreNote **notes = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*notes));
    if (!notes)
        return false;
    size_t count = 0;
    for (size_t i = 0; i < r->note_count; ++i)
        if (!r->notes[i].skipped &&
            Find(Chip_XmlChild(r->notes[i].node, "notations"), "arpeggiate"))
            notes[count++] = &r->notes[i];
    SDL_qsort(notes, count, sizeof(*notes), CompareChords);
    for (size_t first = 0; first < count;)
    {
        size_t end = first + 1;
        while (end < count && notes[end]->part == notes[first]->part &&
               notes[end]->start == notes[first]->start &&
               RollNumber(notes[end]) == RollNumber(notes[first]))
            ++end;
        if (end - first > 1)
        {
            const ChipXmlNode *mark =
                Find(Chip_XmlChild(notes[first]->node, "notations"), "arpeggiate");
            const bool down = SDL_strcmp(Chip_XmlAttribute(mark, "direction"), "down") == 0;
            Sint64 shortest = notes[first]->duration;
            for (size_t i = first + 1; i < end; ++i)
                shortest = SDL_min(shortest, notes[i]->duration);
            const Sint64 spread =
                SDL_min(shortest / 2, (Sint64)SDL_round(r->options->arpeggio_beats *
                                                        r->song->info.ticks_per_quarter));
            for (size_t i = first; i < end; ++i)
            {
                if (*Chip_XmlAttribute(notes[i]->node, "attack"))
                    continue;
                const size_t rank = down ? end - 1 - i : i - first;
                const Sint64 offset = (Sint64)((Uint64)spread * rank / (end - first - 1));
                notes[i]->start += offset;
                notes[i]->duration -= offset;
            }
        }
        first = end;
    }
    SDL_free(notes);
    return true;
}

bool Chip_ApplyScoreTiming(ScoreReader *r)
{
    ScorePause *pauses = NULL;
    size_t count = 0, capacity = 0;
    bool ok = Graces(r, &pauses, &count, &capacity);
    for (size_t i = 0; ok && i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if ((n->skipped && !n->tied_continuation) || n->grace)
            continue;
        const Sint64 duration =
            n->curve_first ? r->curves[n->curve_first - 1].duration : n->duration;
        const ChipXmlNode *notation = Chip_XmlChild(n->node, "notations");
        if (Find(notation, "fermata"))
            ok =
                Pause(&pauses, &count, &capacity, n->start + duration,
                      (Sint64)SDL_round((double)duration * (r->options->fermata_factor - 1)), true);
        if (ok && Find(notation, "caesura"))
            ok = Pause(
                &pauses, &count, &capacity, n->start + duration,
                (Sint64)SDL_round(r->options->caesura_beats * r->song->info.ticks_per_quarter),
                false);
    }
    if (ok && count)
    {
        SDL_qsort(pauses, count, sizeof(*pauses), ComparePauses);
        size_t unique = 0;
        for (size_t i = 0; i < count; ++i)
        {
            if (unique && pauses[unique - 1].tick == pauses[i].tick)
            {
                pauses[unique - 1].length = SDL_max(pauses[unique - 1].length, pauses[i].length);
                pauses[unique - 1].sustain |= pauses[i].sustain;
            }
            else
                pauses[unique++] = pauses[i];
        }
        count = unique;
        Sint64 prefix = 0;
        for (size_t i = 0; i < count; ++i)
        {
            prefix += pauses[i].length;
            pauses[i].prefix = prefix;
            if (prefix > 864000LL * r->song->info.ticks_per_quarter)
                ok = Chip_ScoreError(r, NULL, "expressive timing exceeds score limit");
        }
        for (size_t i = 0; ok && i < r->note_count; ++i)
        {
            ScoreNote *n = &r->notes[i];
            const Sint64 start = Map(pauses, count, n->start, !n->make_time);
            if (!n->make_time)
                n->duration = Map(pauses, count, n->start + n->duration, false) - start;
            n->start = start;
            if (n->make_time)
            {
                n->start += n->attack;
                n->attack = 0;
            }
        }
        for (size_t i = 0; ok && i < r->curve_count; ++i)
        {
            ScoreCurve *curve = &r->curves[i];
            const Sint64 start = Map(pauses, count, curve->start, true);
            curve->duration = Map(pauses, count, curve->start + curve->duration, false) - start;
            curve->start = start;
        }
        for (size_t i = 0; ok && i < r->control_count; ++i)
        {
            ScoreControl *c = &r->controls[i];
            const Sint64 start = Map(pauses, count, c->start, true);
            c->duration = Map(pauses, count, c->start + c->duration, true) - start;
            c->start = start;
        }
        for (size_t i = 0; ok && i < r->song->count; ++i)
            r->song->events[i].tick =
                (Uint64)Map(pauses, count, (Sint64)r->song->events[i].tick, true);
        for (int i = 0; ok && i < r->song->measure_count; ++i)
        {
            ChipMeasurePosition *m = &r->song->measures[i];
            m->start = (Uint64)Map(pauses, count, (Sint64)m->start, false);
            m->end = (Uint64)Map(pauses, count, (Sint64)m->end, false);
        }
        r->song->info.duration_ticks =
            (Uint64)Map(pauses, count, (Sint64)r->song->info.duration_ticks, true);
    }
    SDL_free(pauses);
    return ok && Arpeggios(r);
}
