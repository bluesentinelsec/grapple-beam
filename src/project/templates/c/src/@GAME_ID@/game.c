#include "startup.h"
#include <@GAME_ID@/game.h>
#include <SDL3/SDL.h>
#include <grapple/engine_assets.h>
#include <grapple/engine_input.h>

typedef struct Application
{
    GameState state;
    Grapple_Engine *engine;
    Grapple_TextureId sprite;
    bool self_test, passed;
    unsigned frames;
} Application;
static bool Load(void *user)
{
    Application *app = user;
    app->sprite = Grapple_LoadTexture(app->engine, "sprites/player.svg");
    if (Grapple_AssetStatusOf(app->engine, app->sprite) != GRAPPLE_ASSET_READY)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load sprites/player.svg: %s",
                     SDL_GetError());
        return false;
    }
    return true;
}
static void Update(void *user, float step)
{
    Application *app = user;
    float dx = (float)Grapple_KeyDown(app->engine, SDL_SCANCODE_RIGHT) -
               (float)Grapple_KeyDown(app->engine, SDL_SCANCODE_LEFT);
    float dy = (float)Grapple_KeyDown(app->engine, SDL_SCANCODE_DOWN) -
               (float)Grapple_KeyDown(app->engine, SDL_SCANCODE_UP);
    GameMove(&app->state, app->self_test ? 1.0f : dx, dy, step);
    if (Grapple_KeyDown(app->engine, SDL_SCANCODE_ESCAPE))
        Grapple_EngineQuit(app->engine);
}
static void Render(void *user, float alpha)
{
    (void)alpha;
    Application *app = user;
    SDL_Renderer *renderer = Grapple_EngineRenderer(app->engine);
    SDL_FRect player = {app->state.x, app->state.y, 32.0f, 32.0f};
    if (!SDL_SetRenderDrawColor(renderer, 18, 22, 34, 255) || !SDL_RenderClear(renderer) ||
        !SDL_RenderTexture(renderer, Grapple_Texture(app->engine, app->sprite), NULL, &player))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Rendering failed: %s", SDL_GetError());
        app->passed = false;
        Grapple_EngineQuit(app->engine);
        return;
    }
    if (app->self_test && ++app->frames == 12)
    {
        SDL_Surface *pixels = SDL_RenderReadPixels(renderer, NULL);
        Uint8 r = 0, g = 0, b = 0, a = 0;
        // Read the rendered sprite center in output pixels, accounting for render scale.
        int width = 0, height = 0;
        SDL_GetCurrentRenderOutputSize(renderer, &width, &height);
        const int x = (int)((app->state.x + 16.0f) * (float)width / 1280.0f);
        const int y = (int)((app->state.y + 16.0f) * (float)height / 720.0f);
        app->passed = pixels && SDL_ReadSurfacePixel(pixels, x, y, &r, &g, &b, &a) && g > 150 &&
                      r < 80 && b > 100 && app->state.x > 100.0f;
        SDL_DestroySurface(pixels);
        SDL_Log("Integration: renderer=%s frames=%u moved=%f pixel=%u,%u,%u result=%s",
                SDL_GetRendererName(renderer), app->frames, (double)app->state.x, r, g, b,
                app->passed ? "pass" : "fail");
        Grapple_EngineQuit(app->engine);
    }
}
int GameRun(int argc, char **argv)
{
    GameStartup startup;
    if (!GameConfigure(&startup, argc, argv))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        GameStartupFree(&startup);
        return 1;
    }
    if (startup.print)
    {
        GameStartupFree(&startup);
        return 0;
    }
    Application app = {0};
    app.state.x = 100.0f;
    app.state.y = 100.0f;
    app.self_test = startup.self_test;
    app.passed = true;
    app.engine = Grapple_CreateEngine(&startup.engine);
    if (!app.engine)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        GameStartupFree(&startup);
        return 1;
    }
    Grapple_GameHooks hooks = {0};
    hooks.load = Load;
    hooks.fixed_update = Update;
    hooks.render = Render;
    const bool ok = Grapple_RunGame(app.engine, &hooks, &app);
    Grapple_DestroyEngine(app.engine);
    GameStartupFree(&startup);
    SDL_Quit();
    return ok && app.passed && (!app.self_test || app.frames == 12) ? 0 : 1;
}
