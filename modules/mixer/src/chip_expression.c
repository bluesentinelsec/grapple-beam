/* Original Grapple code (zlib). Independent note expression and bounded storage. */
#include "chip_internal.h"

void Grapple_GetChipExpressionDefaults(Grapple_ChipExpression *expression)
{
    if (!expression)
        return;
    SDL_zero(*expression);
    expression->gain = 1;
    expression->brightness = 1;
    expression->bend_middle = 0.5f;
    expression->bend_last = 1;
    expression->vibrato_beats = 0.25f;
}

bool Chip_ValidExpression(const Grapple_ChipExpression *e)
{
    if (!e)
        return true;
#define RANGE(field, low, high) (e->field >= (low) && e->field <= (high))
    if (!RANGE(tuning, -48, 48) || !RANGE(gain, 0, 2) || !RANGE(brightness, 0.1f, 2) ||
        !RANGE(noise, 0, 1) || !RANGE(bend_start, -48, 48) || !RANGE(bend_peak, -48, 48) ||
        !RANGE(bend_end, -48, 48) || !RANGE(bend_first, 0, 1) ||
        !RANGE(bend_middle, e->bend_first, 1) || !RANGE(bend_last, e->bend_middle, 1) ||
        !RANGE(vibrato_depth, 0, 12) || !RANGE(vibrato_beats, 0.01f, 16) || !RANGE(lane, 0, 65535))
        return SDL_SetError("chiptune: invalid per-note expression");
#undef RANGE
    return true;
}

bool Chip_AppendExpression(Grapple_ChipSong *song, const Grapple_ChipExpression *expression,
                           Uint32 *index)
{
    *index = 0;
    if (!expression)
        return true;
    if (!Chip_ValidExpression(expression))
        return false;
    if (song->expression_count == CHIP_SONG_MAX_EVENTS / 2)
        return SDL_SetError("chiptune: note-expression limit exceeded");
    if (song->expression_count == song->expression_capacity)
    {
        size_t capacity = SDL_min(song->expression_capacity ? song->expression_capacity * 2 : 128,
                                  (size_t)CHIP_SONG_MAX_EVENTS / 2);
        void *memory = SDL_realloc(song->expressions, capacity * sizeof(*song->expressions));
        if (!memory)
            return false;
        song->expressions = memory;
        song->expression_capacity = capacity;
    }
    song->expressions[song->expression_count++] = *expression;
    *index = (Uint32)song->expression_count;
    return true;
}

static float Segment(float a, float b, double position, float first, float last, bool accelerate)
{
    if (position <= first)
        return a;
    if (position >= last || first == last)
        return b;
    double fraction = (position - first) / (last - first);
    if (accelerate)
        fraction *= fraction;
    return a + (b - a) * (float)fraction;
}

float Chip_ExpressionPitch(const Grapple_ChipExpression *e, double elapsed, double duration)
{
    const double position = duration > 0 ? SDL_clamp(elapsed / duration, 0, 1) : 1;
    float bend = position < e->bend_middle
                     ? Segment(e->bend_start, e->bend_peak, position, e->bend_first, e->bend_middle,
                               e->bend_accelerate)
                     : Segment(e->bend_peak, e->bend_end, position, e->bend_middle, e->bend_last,
                               e->bend_accelerate);
    if (e->stepped_pitch)
        bend = SDL_roundf(bend);
    return e->tuning + bend +
           e->vibrato_depth * (float)SDL_sin(2 * SDL_PI_D * elapsed / e->vibrato_beats);
}
