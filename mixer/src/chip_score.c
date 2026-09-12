/* Original Grapple code (zlib). Score normalization and event compilation. */
#include "chip_score.h"

static int SDLCALL CompareNotes(const void *left, const void *right)
{
    const ScoreNote *a = *(const ScoreNote *const *)left;
    const ScoreNote *b = *(const ScoreNote *const *)right;
#define COMPARE(field)                                                                             \
    if (a->field != b->field)                                                                      \
    return a->field < b->field ? -1 : 1
    COMPARE(measure);
    COMPARE(start);
    COMPARE(duration);
    COMPARE(grace);
    COMPARE(rest);
    COMPARE(pitch);
    COMPARE(velocity);
    COMPARE(expression.tuning);
    COMPARE(tie_start);
    COMPARE(tie_stop);
#undef COMPARE
    return 0;
}

static bool RemoveMirrors(ScoreReader *r)
{
    if (r->options->staff == -1)
        return true;
    ScoreNote **a = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*a));
    ScoreNote **b = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*b));
    if (!a || !b)
    {
        SDL_free(a);
        SDL_free(b);
        return false;
    }
    size_t begin = 0;
    for (int part = 0; part < r->song->info.track_count; ++part)
    {
        size_t end = begin;
        while (end < r->note_count && r->notes[end].part == part)
            ++end;
        if (r->options->staff > 0)
        {
            Uint32 staves = 0;
            for (size_t i = begin; i < end; ++i)
                staves |= 1u << r->notes[i].staff;
            if (staves && (staves & (staves - 1)))
            {
                if (!(staves & (1u << (r->options->staff - 1))))
                {
                    SDL_free(a);
                    SDL_free(b);
                    return Chip_ScoreError(r, NULL,
                                           "selected staff is absent from multi-staff part");
                }
                for (size_t i = begin; i < end; ++i)
                    r->notes[i].skipped = r->notes[i].staff != r->options->staff - 1;
            }
            begin = end;
            continue;
        }
        for (int staff = 0; staff < SCORE_MAX_STAVES; ++staff)
        {
            size_t na = 0;
            for (size_t i = begin; i < end; ++i)
                if (r->notes[i].part == part && r->notes[i].staff == staff && r->notes[i].tab)
                    a[na++] = &r->notes[i];
            if (!na)
                continue;
            SDL_qsort(a, na, sizeof(*a), CompareNotes);
            for (int other = 0; other < SCORE_MAX_STAVES; ++other)
            {
                size_t nb = 0;
                for (size_t i = begin; i < end; ++i)
                    if (r->notes[i].part == part && r->notes[i].staff == other && !r->notes[i].tab)
                        b[nb++] = &r->notes[i];
                if (na != nb)
                    continue;
                SDL_qsort(b, nb, sizeof(*b), CompareNotes);
                bool match = true;
                for (size_t i = 0; i < na; ++i)
                    if (CompareNotes(&a[i], &b[i]))
                    {
                        match = false;
                        break;
                    }
                if (match)
                {
                    for (size_t i = 0; i < na; ++i)
                        a[i]->skipped = true;
                    const Grapple_ChipDiagnostic diagnostic = {GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR,
                                                               GRAPPLE_CHIP_DIAGNOSTIC_INFO,
                                                               part,
                                                               -1,
                                                               staff + 1,
                                                               0};
                    if (!Chip_AddDiagnostic(r->song, diagnostic,
                                            "Omitted TAB staff matching a standard-notation staff"))
                    {
                        SDL_free(a);
                        SDL_free(b);
                        return false;
                    }
                    break;
                }
            }
        }
        begin = end;
    }
    SDL_free(a);
    SDL_free(b);
    return true;
}

static bool RestoreControls(ScoreReader *r, const ScoreControl *controls, size_t count,
                            const Sint64 *positions, int source)
{
    const size_t channels = (size_t)r->song->info.track_count * 16;
    ChipEvent *state = SDL_calloc(channels * 130, sizeof(*state));
    bool *used = SDL_calloc(channels, sizeof(*used));
    if (!state || !used)
    {
        SDL_free(state);
        SDL_free(used);
        return false;
    }
    for (size_t i = 0; i < count; ++i)
        if (controls[i].event.status)
            used[(size_t)controls[i].event.track * 16 + (controls[i].event.status & 15u)] = true;
    const int defaults[][2] = {{1, 0}, {7, 127}, {10, 64}, {11, 127}, {64, 0}, {66, 0}, {67, 0}};
    for (size_t channel = 0; channel < channels; ++channel)
    {
        if (!used[channel])
            continue;
        for (size_t i = 0; i < SDL_arraysize(defaults); ++i)
        {
            ChipEvent *event = &state[channel * 130 + (size_t)defaults[i][0]];
            event->track = (Uint16)(channel / 16);
            event->status = (Uint8)(0xb0u | (channel & 15u));
            event->a = (Uint8)defaults[i][0];
            event->b = (Uint8)defaults[i][1];
        }
        state[channel * 130 + 128].track = (Uint16)(channel / 16);
        state[channel * 130 + 128].status = (Uint8)(0xc0u | (channel & 15u));
        state[channel * 130 + 129].track = (Uint16)(channel / 16);
        state[channel * 130 + 129].status = (Uint8)(0xe0u | (channel & 15u));
        state[channel * 130 + 129].b = 64;
    }
    Uint32 tempo = 500000;
    Sint64 tempo_at = -1;
    for (size_t i = 0; i < count; ++i)
    {
        const ScoreControl *control = &controls[i];
        const Sint64 at = positions[control->measure] + control->start;
        if (at >= positions[source])
            continue;
        if (control->event.tempo)
        {
            if (at >= tempo_at)
            {
                tempo_at = at;
                tempo = control->event.tempo;
            }
            continue;
        }
        const int kind = control->event.status >> 4;
        const size_t key = kind == 11 ? control->event.a : kind == 12 ? 128 : 129;
        if (kind != 11 && kind != 12 && kind != 14)
            continue;
        const size_t channel = (size_t)control->event.track * 16 + (control->event.status & 15u);
        ChipEvent *event = &state[channel * 130 + key];
        if (at >= (Sint64)event->tick)
        {
            *event = control->event;
            event->tick = (Uint64)at;
        }
    }
    const int saved = r->part;
    r->part = 0;
    bool ok = Chip_ScoreControl(r, 0, 0, 0, 0, tempo);
    for (size_t i = 0; i < channels * 130 && ok; ++i)
    {
        const ChipEvent *event = &state[i];
        if (!event->status)
            continue;
        r->part = event->track;
        ok = Chip_ScoreControl(r, 0, event->status, event->a, event->b, 0);
    }
    r->part = saved;
    SDL_free(state);
    SDL_free(used);
    return ok && Chip_RestoreDirections(r, controls, count, positions, source);
}

static bool ExpandOrder(ScoreReader *r)
{
    for (int m = 0; m < r->measures; ++m)
    {
        if (!r->navigation[m].repeat_count || !r->navigation[m].endings)
            continue;
        Uint32 passes = r->navigation[m].endings;
        for (int next = m + 1; next < r->measures && r->navigation[next].endings; ++next)
            passes |= r->navigation[next].endings;
        int total = 0;
        for (; passes; passes >>= 1)
            ++total;
        r->navigation[m].repeat_count = SDL_max(r->navigation[m].repeat_count, total);
    }
    int *order = NULL, count = 0;
    if (!Chip_ScoreOrder(r->navigation, r->measures, &order, &count))
        return false;
    size_t *first_note = SDL_malloc((size_t)r->measures * sizeof(*first_note));
    size_t *first_control = SDL_malloc((size_t)r->measures * sizeof(*first_control));
    size_t *next_note = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*next_note));
    size_t *next_control = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*next_control));
    Sint64 *positions = SDL_calloc((size_t)r->measures + 1, sizeof(*positions));
    Sint64 *lengths = SDL_calloc(SCORE_MAX_MEASURES, sizeof(*lengths));
    r->song->measures = SDL_calloc((size_t)SDL_max(count, 1), sizeof(*r->song->measures));
    bool ok = false;
    ScoreNote *notes = NULL;
    ScoreControl *controls = NULL;
    if (!first_note || !first_control || !next_note || !next_control || !positions || !lengths ||
        !r->song->measures)
        goto done;
    for (int m = 0; m < r->measures; ++m)
    {
        first_note[m] = first_control[m] = SIZE_MAX;
        positions[m + 1] = positions[m] + r->lengths[m];
    }
    for (size_t i = r->note_count; i > 0; --i)
    {
        const int measure = r->notes[i - 1].measure;
        next_note[i - 1] = first_note[measure];
        first_note[measure] = i - 1;
    }
    for (size_t i = r->control_count; i > 0; --i)
    {
        const int measure = r->controls[i - 1].measure;
        next_control[i - 1] = first_control[measure];
        first_control[measure] = i - 1;
    }
    notes = r->notes;
    controls = r->controls;
    const size_t source_controls = r->control_count;
    r->notes = NULL;
    r->note_count = r->note_capacity = 0;
    r->controls = NULL;
    r->control_count = r->control_capacity = 0;
    for (int visit = 0; visit < count; ++visit)
    {
        const int source = order[visit];
        r->measure = visit;
        lengths[visit] = r->lengths[source];
        r->song->measures[visit].source = source;
        if (visit && source != order[visit - 1] + 1 &&
            !RestoreControls(r, controls, source_controls, positions, source))
            goto done;
        for (size_t i = first_note[source]; i != SIZE_MAX; i = next_note[i])
        {
            if (notes[i].skipped)
                continue;
            if (!Chip_ScoreGrow((void **)&r->notes, &r->note_capacity, r->note_count,
                                sizeof(*r->notes)))
                goto done;
            r->notes[r->note_count] = notes[i];
            r->notes[r->note_count++].measure = visit;
        }
        for (size_t i = first_control[source]; i != SIZE_MAX; i = next_control[i])
        {
            if (!Chip_ScoreGrow((void **)&r->controls, &r->control_capacity, r->control_count,
                                sizeof(*r->controls)))
                goto done;
            r->controls[r->control_count] = controls[i];
            r->controls[r->control_count].measure = visit;
            r->controls[r->control_count].event.order = (Uint32)r->control_count;
            ++r->control_count;
        }
    }
    SDL_free(r->lengths);
    r->lengths = lengths;
    lengths = NULL;
    r->measures = count;
    r->song->measure_count = count;
    ok = true;
done:
    SDL_free(order);
    SDL_free(first_note);
    SDL_free(first_control);
    SDL_free(next_note);
    SDL_free(next_control);
    SDL_free(positions);
    SDL_free(lengths);
    SDL_free(notes);
    SDL_free(controls);
    return ok;
}

static int SDLCALL CompareControls(const void *left, const void *right)
{
    const ScoreControl *a = left, *b = right;
    if (a->measure != b->measure)
        return a->measure < b->measure ? -1 : 1;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return (a->event.order > b->event.order) - (a->event.order < b->event.order);
}

bool Chip_CompileScore(ScoreReader *r)
{
    if (!Chip_ApplyInstrumentChanges(r) || !Chip_PrepareDirections(r))
        return false;
    if (r->control_count)
        SDL_qsort(r->controls, r->control_count, sizeof(*r->controls), CompareControls);
    const ScoreControl *previous_tempo = NULL;
    for (size_t i = 0; i < r->control_count; ++i)
    {
        const ScoreControl *control = &r->controls[i];
        if (!control->event.tempo)
            continue;
        if (previous_tempo && previous_tempo->measure == control->measure &&
            previous_tempo->start == control->start &&
            previous_tempo->event.tempo != control->event.tempo)
            return Chip_ScoreError(r, NULL, "conflicting simultaneous global tempos");
        previous_tempo = control;
    }
    if (!RemoveMirrors(r) || !ExpandOrder(r))
        return false;
    Sint64 position = 0;
    for (int m = 0; m < r->measures; ++m)
    {
        const Sint64 length = r->lengths[m];
        r->lengths[m] = position;
        r->song->measures[m].start = (Uint64)position;
        position += length;
        r->song->measures[m].end = (Uint64)position;
        if (position > 864000LL * r->song->info.ticks_per_quarter)
            return Chip_ScoreError(r, NULL, "score length limit exceeded");
    }
    r->song->info.duration_ticks = (Uint64)position;
    for (size_t i = 0; i < r->control_count; ++i)
    {
        ScoreControl *c = &r->controls[i];
        c->start += r->lengths[c->measure];
        c->event.tick = (Uint64)c->start;
        if (c->kind != SCORE_CONTROL)
            continue;
        if (c->event.tick > r->song->info.duration_ticks)
            return Chip_ScoreError(r, NULL, "direction past score end");
        if (!Chip_AppendEvent(r->song, c->event))
            return false;
    }
    for (size_t i = 0; i < r->note_count; ++i)
        r->notes[i].start += r->lengths[r->notes[i].measure];
    if (!Chip_ResolveNoteExpression(r))
        return false;
    size_t active[4096];
    size_t ties = 0;
    for (size_t i = 0; i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if (n->skipped)
            continue;
        size_t target = i;
        if (n->tie_stop)
        {
            size_t t;
            for (t = 0; t < ties; ++t)
            {
                const ScoreNote *a = &r->notes[active[t]];
                if (a->part == n->part && a->staff == n->staff && a->pitch == n->pitch &&
                    a->expression.tuning == n->expression.tuning &&
                    a->start + a->duration == n->start && SDL_strcmp(a->voice, n->voice) == 0 &&
                    SDL_strcmp(a->instrument, n->instrument) == 0)
                    break;
            }
            if (t == ties)
                return Chip_ScoreError(r, NULL, "tie stop without a matching adjacent start");
            target = active[t];
            r->notes[target].duration += n->duration;
            n->skipped = true;
            active[t] = active[--ties];
        }
        if (n->tie_start)
        {
            if (ties == SDL_arraysize(active))
                return Chip_ScoreError(r, NULL, "simultaneous tie limit exceeded");
            active[ties++] = target;
        }
    }
    if (ties)
        return Chip_ScoreError(r, NULL, "unterminated tie");
    if (!Chip_ApplySwing(r) || !Chip_ApplyScoreTiming(r) || !Chip_ExpandOrnaments(r) ||
        !Chip_ApplyDirections(r))
        return false;
    for (size_t i = 0; i < r->note_count; ++i)
    {
        const ScoreNote *n = &r->notes[i];
        if (n->skipped || n->rest || n->velocity == 0)
            continue;
        ChipEvent event = {0};
        event.track = (Uint16)n->part;
        event.tick = (Uint64)(n->start + n->attack);
        event.status = (Uint8)(0x90 | n->channel);
        event.a = (Uint8)n->pitch;
        event.b = (Uint8)SDL_max(n->velocity, 1);
        event.instrument_data = true;
        event.unpitched = n->unpitched;
        event.program = (Uint8)n->program;
        event.instrument_gain = n->instrument_gain;
        event.instrument_pan = n->instrument_pan;
        event.note_id = (Uint32)i + 1;
        event.duration = (Uint64)n->duration;
        if (!Chip_AppendExpression(r->song, &n->expression, &event.expression))
            return false;
        if (!Chip_AppendEvent(r->song, event))
            return false;
        const Sint64 end =
            n->start + SDL_max(1, (Sint64)SDL_round((double)n->duration * n->gate)) + n->release;
        if (n->start + n->attack < 0 || end <= n->start + n->attack)
            return Chip_ScoreError(r, n->node, "articulation produced an invalid note span");
        event.tick = (Uint64)end;
        r->song->info.duration_ticks = SDL_max(r->song->info.duration_ticks, event.tick);
        event.status = (Uint8)(0x80 | n->channel);
        event.b = 0;
        if (!Chip_AppendEvent(r->song, event))
            return false;
        ++r->song->tracks[n->part].note_count;
        r->song->tracks[n->part].channels |= (Uint16)(1u << n->channel);
    }
    return Chip_ResolveTiming(r->song);
}
