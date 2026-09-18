#include <@GAME_ID@/game.h>
#include <math.h>
void GameMove(GameState *state, float dx, float dy, float seconds)
{
    if (!state || !isfinite(seconds) || seconds < 0.0f || !isfinite(dx) || !isfinite(dy))
        return;
    state->x = fminf(1248.0f, fmaxf(0.0f, state->x + dx * 300.0f * seconds));
    state->y = fminf(688.0f, fmaxf(0.0f, state->y + dy * 300.0f * seconds));
}
