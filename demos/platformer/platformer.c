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
 * Xbox-layout pad works the same. Press into a wall while falling to slide,
 * jump to kick off it. Escape quits.
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
    Grapple_PlatformerCreateFloor(level, 0, 20, 69, 0);
    Grapple_PlatformerCreateFloor(level, 71, 20, 15, 0);
    Grapple_PlatformerCreateFloor(level, 89, 20, 64, 0);
    Grapple_PlatformerCreateFloor(level, 155, 20, 57, 0);

    /* Blocks to jump on and bump; a tower of floating ones climbs to a high
       platform, which is where the vertical scrolling shows. */
    Grapple_PlatformerCreateBlock(level, 16, 16, 1, 1);
    Grapple_PlatformerCreateBlock(level, 20, 16, 5, 1);
    Grapple_PlatformerCreateBlock(level, 22, 12, 1, 1);
    Grapple_PlatformerCreateBlock(level, 37, 15, 3, 1);
    Grapple_PlatformerCreateBlock(level, 45, 17, 1, 1);
    Grapple_PlatformerCreateBlock(level, 52, 16, 2, 1);
    Grapple_PlatformerCreateBlock(level, 77, 16, 3, 1);
    Grapple_PlatformerCreateBlock(level, 80, 12, 8, 1);
    Grapple_PlatformerCreateBlock(level, 91, 12, 3, 1);
    Grapple_PlatformerCreateBlock(level, 94, 16, 2, 1);
    Grapple_PlatformerCreateBlock(level, 64, 17, 2, 1);
    Grapple_PlatformerCreateBlock(level, 66, 14, 2, 1);
    Grapple_PlatformerCreateBlock(level, 68, 11, 2, 1);
    Grapple_PlatformerCreateBlock(level, 70, 8, 2, 1);
    Grapple_PlatformerCreateBlock(level, 72, 5, 6, 1);
    Grapple_PlatformerCreateBlock(level, 96, 3, 4, 1);
    Grapple_PlatformerCreateBlock(level, 105, 3, 6, 1);
    Grapple_PlatformerCreateBlock(level, 189, 12, 1, 8);

    /* Ledges: jump up through them, land on them from above. */
    Grapple_PlatformerCreatePlatform(level, 30, 15, 4);
    Grapple_PlatformerCreatePlatform(level, 58, 13, 5);
    Grapple_PlatformerCreatePlatform(level, 113, 6, 4);
    Grapple_PlatformerCreatePlatform(level, 119, 9, 4);

    /* Walls: pipes to climb over, and at x = 100 a shaft three tiles wide to
       wall-jump up. */
    static const int walls[][3] = {{28, 18, 2},  {29, 18, 2}, {38, 17, 3}, {39, 17, 3},
                                   {46, 16, 4},  {47, 16, 4}, {57, 16, 4}, {58, 16, 4},
                                   {100, 4, 16}, {104, 4, 16}};
    for (size_t i = 0; i < SDL_arraysize(walls); ++i)
    {
        Grapple_PlatformerCreateWall(level, walls[i][0], walls[i][1], walls[i][2]);
    }

    /* Stairs. */
    Grapple_PlatformerCreateStairs(level, 134, 19, 4, true);
    Grapple_PlatformerCreateStairs(level, 140, 19, 4, false);
    Grapple_PlatformerCreateStairs(level, 148, 19, 4, true);
    Grapple_PlatformerCreateStairs(level, 155, 19, 4, false);
    Grapple_PlatformerCreateStairs(level, 181, 19, 8, true);
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
                            "arrows/AD move  shift run  space jump (walls too)  esc quit");
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

    /* 212 tiles long, like 1-1, and 22 tall against a 15-row view so there is
       somewhere to scroll up to. The camera leads the way the player faces
       and follows anywhere; GRAPPLE_PLATFORMER_SCROLL_FORWARD gets 1985 back. */
    game.level = Grapple_CreatePlatformer(game.engine, 212, 22, TILE);
    if (game.level == NULL)
    {
        SDL_Log("could not create the level: %s", SDL_GetError());
        Grapple_DestroyEngine(game.engine);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    BuildLevel(game.level);
    Grapple_PlatformerCreatePlayer(game.level, 3, 19);
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
