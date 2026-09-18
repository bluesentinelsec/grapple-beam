/* Original Grapple code (zlib). Harmonic pitch groups and guitar sustain policy. */
#include "chip_score.h"

static const ChipXmlNode *Harmonic(const ScoreNote *note)
{
    return Chip_XmlChild(Chip_XmlChild(Chip_XmlChild(note->node, "notations"), "technical"),
                         "harmonic");
}

static int GroupCompare(const ScoreNote *a, const ScoreNote *b, bool strings)
{
    if (a->part != b->part)
        return a->part < b->part ? -1 : 1;
    if (a->staff != b->staff)
        return a->staff < b->staff ? -1 : 1;
    const int instrument = SDL_strcmp(a->instrument, b->instrument);
    if (instrument)
        return instrument;
    if (strings && a->string_number != b->string_number)
        return a->string_number < b->string_number ? -1 : 1;
    if (!strings || !a->string_number)
    {
        const int voice = SDL_strcmp(a->voice, b->voice);
        if (voice)
            return voice;
    }
    return 0;
}

static int CompareHarmony(const void *left, const void *right)
{
    const ScoreNote *a = *(ScoreNote *const *)left, *b = *(ScoreNote *const *)right;
    const int group = GroupCompare(a, b, false);
    if (group)
        return group;
    return (a->start > b->start) - (a->start < b->start);
}

static int CompareStrings(const void *left, const void *right)
{
    const ScoreNote *a = *(ScoreNote *const *)left, *b = *(ScoreNote *const *)right;
    const int group = GroupCompare(a, b, true);
    if (group)
        return group;
    return (a->start > b->start) - (a->start < b->start);
}

static bool HarmonicGroup(ScoreReader *r, ScoreNote **notes, size_t count)
{
    ScoreNote *base = NULL, *touch = NULL;
    bool sounding = false;
    for (size_t i = 0; i < count; ++i)
    {
        const ChipXmlNode *harmonic = Harmonic(notes[i]);
        if (Chip_XmlChild(harmonic, "sounding-pitch"))
            sounding = true;
        if (Chip_XmlChild(harmonic, "base-pitch"))
        {
            if (base)
                return Chip_ScoreError(r, harmonic, "ambiguous simultaneous harmonic bases");
            base = notes[i];
        }
        if (Chip_XmlChild(harmonic, "touching-pitch"))
        {
            if (touch)
                return Chip_ScoreError(r, harmonic,
                                       "ambiguous simultaneous harmonic touching pitches");
            touch = notes[i];
        }
    }
    if (!base && !touch)
        return true;
    if (sounding)
    {
        if (base)
            base->skipped = true;
        if (touch)
            touch->skipped = true;
        return true;
    }
    if (!base || !touch)
        return Chip_ScoreFail(
            r, (base ? base : touch)->node, GRAPPLE_CHIP_DIAGNOSTIC_EXPORTER_OMISSION,
            "harmonic base/touch notation needs a sounding pitch or paired pitches");
    const double interval =
        (double)touch->pitch + touch->expression.tuning - base->pitch - base->expression.tuning;
    static const int intervals[] = {12, 7, 5, 4, 3};
    static const int partials[] = {2, 3, 4, 5, 6};
    int partial = 0;
    for (size_t i = 0; i < SDL_arraysize(intervals); ++i)
        if (SDL_fabs(interval - intervals[i]) < 0.01)
            partial = partials[i];
    if (!partial || base->duration != touch->duration)
        return Chip_ScoreFail(r, base->node, GRAPPLE_CHIP_DIAGNOSTIC_EXPORTER_OMISSION,
                              "harmonic needs an explicit sounding pitch for this touch interval");
    const double pitch =
        (double)base->pitch + base->expression.tuning + 12 * SDL_log((double)partial) / SDL_log(2);
    if (pitch > 127)
        return Chip_ScoreError(r, base->node, "harmonic sounding pitch exceeds synthesis range");
    base->pitch = (int)SDL_floor(pitch);
    base->expression.tuning = (float)(pitch - base->pitch);
    touch->skipped = true;
    return Chip_ScoreApproximation(r, base,
                                   "Paired harmonic pitches use the corresponding natural partial");
}

bool Chip_ResolveTechniques(ScoreReader *r)
{
    ScoreNote **notes = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*notes));
    if (!notes)
        return false;
    size_t count = 0;
    for (size_t i = 0; i < r->note_count; ++i)
        if (!r->notes[i].skipped)
            notes[count++] = &r->notes[i];
    SDL_qsort(notes, count, sizeof(*notes), CompareHarmony);
    bool ok = true;
    for (size_t begin = 0; ok && begin < count;)
    {
        size_t end = begin + 1;
        while (end < count && !GroupCompare(notes[begin], notes[end], false) &&
               notes[end]->start == notes[begin]->start)
            ++end;
        r->part = notes[begin]->part;
        r->measure = notes[begin]->source_measure;
        ok = HarmonicGroup(r, notes + begin, end - begin);
        begin = end;
    }
    SDL_qsort(notes, count, sizeof(*notes), CompareStrings);
    Sint64 next_attack = SDL_MAX_SINT64, group_attack = SDL_MAX_SINT64;
    ScoreNote *previous = NULL;
    for (size_t i = count; ok && i > 0; --i)
    {
        ScoreNote *n = notes[i - 1];
        if (!previous || GroupCompare(n, previous, true))
            next_attack = group_attack = SDL_MAX_SINT64;
        else if (n->start < group_attack)
            next_attack = group_attack;
        if (n->let_ring && !n->skipped && !n->rest)
        {
            const Sint64 end = SDL_min(next_attack, (Sint64)r->song->measures[n->measure].end);
            n->duration = SDL_max(n->duration, end - n->start);
            n->gate = 1;
            ok = Chip_ScoreApproximation(
                r, n,
                "Let-ring sustains to the next attack on the same string/voice or the bar end");
        }
        group_attack = n->start;
        previous = n;
    }
    SDL_free(notes);
    return ok;
}
