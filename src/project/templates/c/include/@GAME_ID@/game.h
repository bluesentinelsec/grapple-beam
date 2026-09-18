#ifndef GAME_APPLICATION_H
#define GAME_APPLICATION_H
#include <stdbool.h>
/** @brief Movable player in a 1280 by 720 design space. */
typedef struct GameState
{
    float x; /**< Horizontal position in design units. */
    float y; /**< Vertical position in design units. */
} GameState;
/** @brief Advance movement, clamped to the visible play field.
 * @param state Borrowed mutable game state.
 * @param dx Horizontal input, normally -1 through 1.
 * @param dy Vertical input, normally -1 through 1.
 * @param seconds Nonnegative elapsed simulation time. */
void GameMove(GameState *state, float dx, float dy, float seconds);
/** @brief Start the application using borrowed process arguments.
 * @param argc Argument count. @param argv Argument array.
 * @return Zero on clean exit, nonzero on startup/runtime failure. */
int GameRun(int argc, char **argv);
#endif
