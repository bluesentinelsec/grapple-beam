/* Original Grapple code (zlib). Expand score repeats and explicit navigation once at import. */
#include "chip_score_order.h"

#define SCORE_ORDER_LIMIT 65536
#define SCORE_REPEAT_DEPTH 32

typedef struct RepeatFrame
{
    int start, end, pass, total;
} RepeatFrame;

static int FindLabel(const ChipScoreMeasure *measures, int count, const char *label, bool coda)
{
    int found = -1;
    for (int i = 0; i < count; ++i)
    {
        const char *candidate = coda ? measures[i].coda : measures[i].segno;
        if (candidate && SDL_strcmp(candidate, label) == 0)
        {
            if (found != -1)
                return -1;
            found = i;
        }
    }
    return found;
}

bool Chip_ScoreOrderWithPasses(const ChipScoreMeasure *measures, int count, int **order,
                               int **passes, int *length)
{
    *order = NULL;
    if (passes)
        *passes = NULL;
    *length = 0;
    if (!measures || count < 1 || count > SCORE_ORDER_LIMIT)
        return SDL_SetError("MusicXML: invalid score navigation input");
    int *ends = SDL_malloc((size_t)count * sizeof(*ends));
    Uint32 *visits = SDL_calloc((size_t)count, sizeof(*visits));
    bool *jumped_from = SDL_calloc((size_t)count, sizeof(*jumped_from));
    int *result = SDL_malloc(SCORE_ORDER_LIMIT * sizeof(*result));
    int *result_passes = passes ? SDL_malloc(SCORE_ORDER_LIMIT * sizeof(*result_passes)) : NULL;
    bool ok = false;
    if (!ends || !visits || !jumped_from || !result || (passes && !result_passes))
        goto done;
    for (int i = 0; i < count; ++i)
        ends[i] = -1;
    int starts[SCORE_REPEAT_DEPTH], depth = 0, implicit_start = 0;
    for (int i = 0; i < count; ++i)
    {
        if (measures[i].repeat_start)
        {
            if (depth == SCORE_REPEAT_DEPTH)
            {
                SDL_SetError("MusicXML: repeat nesting limit exceeded");
                goto done;
            }
            starts[depth++] = i;
        }
        if (measures[i].repeat_count)
        {
            const int start = depth ? starts[--depth] : implicit_start;
            if (ends[start] != -1 || measures[i].repeat_count < 1 || measures[i].repeat_count > 32)
            {
                SDL_SetError("MusicXML: ambiguous repeat range or count outside 1..32");
                goto done;
            }
            ends[start] = i;
            implicit_start = i + 1;
        }
    }
    if (depth)
    {
        SDL_SetError("MusicXML: forward repeat without backward repeat");
        goto done;
    }
    RepeatFrame frames[SCORE_REPEAT_DEPTH];
    int measure = 0, used = 0, iterations = 0, ending_pass = 1;
    bool jumped = false;
    while (measure < count)
    {
        if (++iterations > SCORE_ORDER_LIMIT)
        {
            SDL_SetError("MusicXML: score navigation expansion limit exceeded");
            goto done;
        }
        const ChipScoreMeasure *m = &measures[measure];
        if (ends[measure] >= 0 && (!depth || frames[depth - 1].start != measure))
        {
            if (depth == SCORE_REPEAT_DEPTH)
            {
                SDL_SetError("MusicXML: repeat nesting limit exceeded");
                goto done;
            }
            const int end = ends[measure], total = measures[end].repeat_count;
            frames[depth++] = (RepeatFrame){
                measure, end, jumped && !measures[end].repeat_after_jump ? total : 1, total};
        }
        const int pass = depth ? frames[depth - 1].pass : ending_pass;
        const bool play = !m->endings || (m->endings & (1u << (pass - 1)));
        ++visits[measure];
        if (play)
        {
            if (result_passes)
                result_passes[used] = pass;
            result[used++] = measure;
        }
        if (play && m->fine && jumped)
            break;
        if (depth && frames[depth - 1].end == measure)
        {
            RepeatFrame *frame = &frames[depth - 1];
            if (frame->pass < frame->total && (!jumped || m->repeat_after_jump))
            {
                ++frame->pass;
                measure = frame->start;
                continue;
            }
            ending_pass = frame->pass;
            --depth;
        }
        if (play && (!jumped_from[measure] || m->jump_times))
        {
            const bool time_matches =
                m->jump_times
                    ? (visits[measure] <= 32 && (m->jump_times & (1u << (visits[measure] - 1))))
                    : (m->tocoda ? jumped : visits[measure] == 1);
            int target = -1;
            bool wants_jump = false;
            if (time_matches && (m->dacapo || m->dalsegno))
            {
                wants_jump = true;
                target = m->dacapo ? 0 : FindLabel(measures, count, m->dalsegno, false);
            }
            else if (m->tocoda && time_matches && (m->jump_times || jumped))
            {
                wants_jump = true;
                target = FindLabel(measures, count, m->tocoda, true);
            }
            if (wants_jump)
            {
                if (target < 0)
                {
                    SDL_SetError("MusicXML: missing or ambiguous navigation target");
                    goto done;
                }
                jumped_from[measure] = true;
                jumped = true;
                depth = 0;
                ending_pass = 2;
                measure = target;
                continue;
            }
        }
        if (!m->endings && !depth)
            ending_pass = jumped ? 2 : 1;
        ++measure;
    }
    *order = result;
    *length = used;
    result = NULL;
    if (passes)
        *passes = result_passes;
    result_passes = NULL;
    ok = true;
done:
    SDL_free(ends);
    SDL_free(visits);
    SDL_free(jumped_from);
    SDL_free(result);
    SDL_free(result_passes);
    return ok;
}

bool Chip_ScoreOrder(const ChipScoreMeasure *measures, int count, int **order, int *length)
{
    return Chip_ScoreOrderWithPasses(measures, count, order, NULL, length);
}
