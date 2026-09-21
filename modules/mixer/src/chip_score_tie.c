/* Original Grapple code (zlib). Tie chains preserve attacks and individual expression spans. */
#include "chip_score.h"

static bool AppendCurve(ScoreReader *r, ScoreNote *first, const ScoreNote *note)
{
    if (!Chip_ScoreGrow((void **)&r->curves, &r->curve_capacity, r->curve_count,
                        sizeof(*r->curves)))
        return false;
    ScoreCurve curve = {note->start, note->duration, note->expression, 0};
    if (first->curve_last && curve.expression.bend_start == 0 && curve.expression.bend_peak == 0 &&
        curve.expression.bend_end == 0)
    {
        const float held = r->curves[first->curve_last - 1].expression.bend_end;
        curve.expression.bend_start = curve.expression.bend_peak = curve.expression.bend_end = held;
    }
    r->curves[r->curve_count++] = curve;
    if (first->curve_last)
        r->curves[first->curve_last - 1].next = r->curve_count;
    else
        first->curve_first = r->curve_count;
    first->curve_last = r->curve_count;
    return true;
}

bool Chip_ResolveTies(ScoreReader *r)
{
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
            {
                if (!Chip_ScoreWarn(r, n->node, GRAPPLE_CHIP_DIAGNOSTIC_SCORE,
                                    "tie stop without a matching start; beginning a new attack"))
                    return false;
                n->tie_stop = false;
            }
            else
            {
                target = active[t];
                ScoreNote *first = &r->notes[target];
                if ((!first->curve_first && !AppendCurve(r, first, first)) ||
                    !AppendCurve(r, first, n))
                    return false;
                const Sint64 combined = first->duration + n->duration;
                first->gate =
                    ((double)first->duration + (double)n->duration * n->gate) / (double)combined;
                first->release = n->release;
                first->duration = combined;
                n->skipped = true;
                n->tied_continuation = true;
                active[t] = active[--ties];
            }
        }
        if (n->tie_start)
        {
            if (ties == SDL_arraysize(active))
                return Chip_ScoreFail(r, n->node, GRAPPLE_CHIP_DIAGNOSTIC_RESOURCE,
                                      "simultaneous tie limit exceeded");
            active[ties++] = target;
        }
    }
    if (ties && !Chip_ScoreWarn(r, NULL, GRAPPLE_CHIP_DIAGNOSTIC_SCORE,
                                "unterminated tie; using the written duration"))
        return false;
    return true;
}

bool Chip_EmitTieCurves(ScoreReader *r, const ScoreNote *note, ChipEvent event)
{
    for (size_t index = note->curve_first; index; index = r->curves[index - 1].next)
    {
        const ScoreCurve *curve = &r->curves[index - 1];
        if (curve->start < note->start || curve->start >= note->start + note->duration)
            continue;
        event.tick = (Uint64)SDL_max(curve->start, note->start + note->attack);
        event.duration = (Uint64)SDL_max(curve->duration, 1);
        event.status = 0xf2;
        if (!Chip_AppendExpression(r->song, &curve->expression, &event.expression) ||
            !Chip_AppendEvent(r->song, event))
            return false;
    }
    return true;
}
