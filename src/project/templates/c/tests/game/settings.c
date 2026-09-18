#include "../../src/@GAME_ID@/startup.h"
#include <stdio.h>
#define CHECK(expr)                                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, #expr, SDL_GetError());         \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool WriteText(const char *path, const char *text)
{
    return SDL_SaveFile(path, text, SDL_strlen(text));
}
int main(void)
{
    const char *base = "settings-fixtures/";
    const char *prefs = "settings-fixtures/player/";
    CHECK(SDL_CreateDirectory(prefs));
    CHECK(WriteText("settings-fixtures/config.toml", "[display]\nvsync=true\nmax_fps=30\n"));
    CHECK(
        WriteText("settings-fixtures/player/config.toml", "[display]\nvsync=false\nmax_fps=90\n"));
    CHECK(WriteText("settings-fixtures/explicit.toml", "[display]\nmax_fps=120\n"));
    CHECK(WriteText("settings-fixtures/player/save.dat", "precious save"));
    GameStartup s;
    char *normal[] = {"game", "--print-settings"};
    CHECK(GameConfigureAt(&s, 2, normal, base, prefs));
    CHECK(!s.graphics.vsync && s.graphics.max_fps == 90);
    GameStartupFree(&s);
    char *overrides[] = {
        "game", "--max-fps=144", "--config", "settings-fixtures/explicit.toml", "--vsync", "on"};
    CHECK(GameConfigureAt(&s, 6, overrides, base, prefs));
    CHECK(s.graphics.vsync && s.graphics.max_fps == 144);
    GameStartupFree(&s);
    char *defaults[] = {"game", "--default-settings"};
    CHECK(GameConfigureAt(&s, 2, defaults, base, prefs));
    CHECK(s.graphics.vsync && s.graphics.max_fps == 30);
    GameStartupFree(&s);
    char *preview[] = {"game", "--reset-settings", "--print-settings"};
    CHECK(GameConfigureAt(&s, 3, preview, base, prefs));
    CHECK(SDL_GetPathInfo("settings-fixtures/player/config.toml", NULL));
    GameStartupFree(&s);
    CHECK(WriteText("settings-fixtures/player/config.toml", "broken = ["));
    CHECK(!GameConfigureAt(&s, 2, normal, base, prefs));
    GameStartupFree(&s);
    char *safe[] = {"game", "--safe-mode", "--vsync=off"};
    CHECK(GameConfigureAt(&s, 3, safe, base, prefs));
    CHECK(s.graphics.window_mode == GRAPPLE_WINDOW_WINDOWED && !s.graphics.vsync);
    CHECK(s.engine.backend == GRAPPLE_BACKEND_SOFTWARE);
    GameStartupFree(&s);
    char *reset[] = {"game", "--reset-settings"};
    CHECK(GameConfigureAt(&s, 2, reset, base, prefs));
    CHECK(!SDL_GetPathInfo("settings-fixtures/player/config.toml", NULL));
    CHECK(SDL_GetPathInfo("settings-fixtures/player/save.dat", NULL));
    GameStartupFree(&s);
    CHECK(WriteText("settings-fixtures/config.toml", "[display]\nvsync=1\n"));
    CHECK(!GameConfigureAt(&s, 2, normal, base, prefs));
    GameStartupFree(&s);
    CHECK(WriteText("settings-fixtures/config.toml", "[display]\nunknown=1\n"));
    CHECK(!GameConfigureAt(&s, 2, normal, base, prefs));
    GameStartupFree(&s);
    return 0;
}
