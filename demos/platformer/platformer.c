/**
 * @file platformer.c
 * @brief A platformer, in C, on Grapple::Platformer.
 *
 * Original grapple-beam code (zlib). The opening stretch of a Super Mario
 * Bros. style level — floor, blocks, walls, stairs, pits — and a character
 * that runs and jumps around it. Every shape is a filled rectangle.
 *
 * There is no `while (running)` and no movement code: the level attaches a
 * scene to the engine that steps the player, scrolls the camera and draws
 * the cells. What is left for the game is what the game is about — the
 * level's layout and a HUD.
 *
 * Controls: A/D or the arrows walk, Shift or X runs, Space or Z jumps; an
 * Xbox-layout pad works the same. Escape quits.
 */
#include <SDL3/SDL.h>
#include <grapple/debug_text.h>
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/platformer.h>
#include <stdio.h>
#include <stdlib.h>

/* The NES drew 256x240 with 16-pixel tiles; the engine letterboxes that to
   whatever the window is, so the level is written once, in tiles. */
#define DESIGN_WIDTH 256
#define DESIGN_HEIGHT 240
#define TILE 16

typedef struct Game
{
    Grapple_Engine *engine;
    Grapple_Platformer *level;
    bool headless;
    float seconds;
} Game;

static void BuildLevel(Grapple_Platformer *level)
{
    /* The ground, with two pits in it. */
    Grapple_PlatformerCreateFloor(level, 0, 13, 69, 0);
    Grapple_PlatformerCreateFloor(level, 71, 13, 15, 0);
    Grapple_PlatformerCreateFloor(level, 89, 13, 64, 0);
    Grapple_PlatformerCreateFloor(level, 155, 13, 57, 0);

    /* Blocks to jump on and bump, and ledges to jump up through. */
    Grapple_PlatformerCreateBlock(level, 16, 9, 1, 1);
    Grapple_PlatformerCreateBlock(level, 20, 9, 5, 1);
    Grapple_PlatformerCreateBlock(level, 22, 5, 1, 1);
    Grapple_PlatformerCreatePlatform(level, 30, 8, 4);
    Grapple_PlatformerCreateBlock(level, 37, 8, 3, 1);
    Grapple_PlatformerCreateBlock(level, 45, 10, 1, 1);
    Grapple_PlatformerCreateBlock(level, 52, 9, 2, 1);
    Grapple_PlatformerCreatePlatform(level, 58, 6, 5);
    Grapple_PlatformerCreateBlock(level, 77, 9, 3, 1);
    Grapple_PlatformerCreateBlock(level, 80, 5, 8, 1);
    Grapple_PlatformerCreateBlock(level, 91, 5, 3, 1);
    Grapple_PlatformerCreateBlock(level, 94, 9, 2, 1);

    /* Walls to climb over. */
    static const int walls[][3] = {{28, 11, 2}, {29, 11, 2}, {38, 10, 3}, {39, 10, 3},
                                   {46, 9, 4},  {47, 9, 4},  {57, 9, 4},  {58, 9, 4}};
    for (size_t i = 0; i < SDL_arraysize(walls); ++i)
    {
        Grapple_PlatformerCreateWall(level, walls[i][0], walls[i][1], walls[i][2]);
    }

    /* Stairs. */
    Grapple_PlatformerCreateStairs(level, 134, 12, 4, true);
    Grapple_PlatformerCreateStairs(level, 140, 12, 4, false);
    Grapple_PlatformerCreateStairs(level, 148, 12, 4, true);
    Grapple_PlatformerCreateStairs(level, 155, 12, 4, false);
    Grapple_PlatformerCreateStairs(level, 181, 12, 8, true);
    Grapple_PlatformerCreateBlock(level, 189, 5, 1, 8);
}

static void Update(void *user, float dt)
{
    Game *game = (Game *)user;
    if (Grapple_KeyPressed(game->engine, SDL_SCANCODE_ESCAPE))
    {
        Grapple_EngineQuit(game->engine);
    }
    if (!game->headless)
    {
        return;
    }
    /* No window, nobody at the keyboard: the level plays itself — run
       right, jump now and then — and narrates, which is how CI tells a game
       that runs from one that only draws. */
    game->seconds += dt;
    const bool jump = SDL_fmodf(game->seconds, 1.5f) < 0.4f;
    Grapple_PlatformerScriptInput(game->level, 1.0f, jump, true);
    if (Grapple_PlatformerPlayerStateChanged(game->level))
    {
        float x = 0.0f;
        float y = 0.0f;
        Grapple_PlatformerPlayerPosition(game->level, &x, &y);
        printf("state=%s x=%.0f y=%.0f\n", Grapple_PlatformerPlayerStateName(game->level), x, y);
        fflush(stdout);
    }
}

/* The scene drew the level; the HUD goes over it in design coordinates. */
static void Render(void *user, float alpha)
{
    (void)alpha;
    Game *game = (Game *)user;
    SDL_Renderer *renderer = Grapple_EngineRenderer(game->engine);
    float x = 0.0f;
    Grapple_PlatformerPlayerPosition(game->level, &x, NULL);
    Grapple_SetDebugTextSize(8.0f);
    Grapple_RenderDebugTextFormat(renderer, 8.0f, 8.0f, "%s  x=%d",
                                  Grapple_PlatformerPlayerStateName(game->level), (int)(x / TILE));
    Grapple_RenderDebugText(renderer, 8.0f, 226.0f,
                            "arrows/AD move  shift run  space jump  esc quit");
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    Game game = {0};
    game.headless = SDL_getenv("GRAPPLE_HEADLESS") != NULL;

    Grapple_EngineConfig config = {0};
    config.title = "Platformer — grapple-beam";
    config.design_width = DESIGN_WIDTH;
    config.design_height = DESIGN_HEIGHT;
    config.presentation = GRAPPLE_PRESENT_LETTERBOX;
    config.no_auto_mount = true; /* rectangles only: nothing to load */
    config.headless = game.headless;
    game.engine = Grapple_CreateEngine(&config);
    if (game.engine == NULL)
    {
        SDL_Log("could not create an engine: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* 212 tiles long, like 1-1; the screen never scrolls back, like 1985. */
    game.level = Grapple_CreatePlatformer(game.engine, 212, 15, TILE);
    if (game.level == NULL)
    {
        SDL_Log("could not create the level: %s", SDL_GetError());
        Grapple_DestroyEngine(game.engine);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    Grapple_PlatformerSetScroll(game.level, GRAPPLE_PLATFORMER_SCROLL_FORWARD);
    BuildLevel(game.level);
    Grapple_PlatformerCreatePlayer(game.level, 3, 12);
    Grapple_PlatformerSetScriptedInput(game.level, game.headless);
    Grapple_PlatformerAttach(game.level);

    Grapple_GameHooks hooks = {0};
    hooks.update = Update;
    hooks.render = Render;
    const bool ok = Grapple_RunGame(game.engine, &hooks, &game);

    Grapple_DestroyPlatformer(game.level);
    Grapple_DestroyEngine(game.engine);
    SDL_Quit();
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
