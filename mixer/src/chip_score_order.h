/* Original Grapple code (zlib). Bounded, format-independent score navigation. */
#ifndef GRAPPLE_CHIP_SCORE_ORDER_H
#define GRAPPLE_CHIP_SCORE_ORDER_H
#include <SDL3/SDL.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct ChipScoreMeasure
    {
        bool repeat_start;
        int repeat_count;
        bool repeat_after_jump;
        Uint32 endings;
        const char *segno;
        const char *coda;
        const char *dalsegno;
        const char *tocoda;
        bool dacapo;
        bool fine;
        Uint32 jump_times;
    } ChipScoreMeasure;

    bool Chip_ScoreOrder(const ChipScoreMeasure *measures, int count, int **order, int *length);
#ifdef __cplusplus
}
#endif
#endif
