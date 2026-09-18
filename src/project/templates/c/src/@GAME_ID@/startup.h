#ifndef GAME_STARTUP_H
#define GAME_STARTUP_H
#include <grapple/engine.h>
#include <grapple/engine_graphics.h>
typedef struct GameStartup
{
    Grapple_EngineConfig engine;
    Grapple_GraphicsSettings graphics;
    char *media;
    bool self_test;
    bool print;
} GameStartup;
bool GameConfigure(GameStartup *startup, int argc, char **argv);
bool GameConfigureAt(GameStartup *startup, int argc, char **argv, const char *base,
                     const char *preferences);
void GameStartupFree(GameStartup *startup);
#endif
