#include <@GAME_ID@/game.h>
#include <math.h>
#include <stdio.h>
#define CHECK(expr)                                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
        {                                                                                          \
            fprintf(stderr, "Failed: %s:%d: %s\n", __FILE__, __LINE__, #expr);                     \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void)
{
    GameState state = {100.0f, 100.0f};
    GameMove(&state, 1.0f, -1.0f, 0.1f);
    CHECK(fabsf(state.x - 130.0f) < 0.01f);
    CHECK(fabsf(state.y - 70.0f) < 0.01f);
    GameMove(&state, 1.0f, -1.0f, 100.0f);
    CHECK(state.x == 1248.0f && state.y == 0.0f);
    GameMove(&state, -1.0f, 1.0f, -1.0f);
    CHECK(state.x == 1248.0f && state.y == 0.0f);
    GameMove(&state, NAN, 1.0f, 1.0f);
    CHECK(isfinite(state.x));
    return 0;
}
