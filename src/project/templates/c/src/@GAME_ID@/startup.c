#include "startup.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <toml.h>

static bool Mode(const char *value, Grapple_WindowMode *mode)
{
    if (!SDL_strcmp(value, "windowed"))
        *mode = GRAPPLE_WINDOW_WINDOWED;
    else if (!SDL_strcmp(value, "fullscreen-borderless"))
        *mode = GRAPPLE_WINDOW_BORDERLESS;
    else if (!SDL_strcmp(value, "fullscreen-exclusive"))
        *mode = GRAPPLE_WINDOW_EXCLUSIVE;
    else
        return SDL_SetError("Invalid window mode: %s", value);
    return true;
}
static bool LoadConfig(GameStartup *s, const char *path, bool required)
{
    if (!required && !SDL_GetPathInfo(path, NULL))
        return true;
    FILE *file = NULL;
#ifdef _MSC_VER
    if (fopen_s(&file, path, "r") != 0)
        file = NULL;
#else
    file = fopen(path, "r");
#endif
    if (!file)
        return SDL_SetError("Cannot read config: %s", path);
    char error[256];
    toml_table_t *root = toml_parse_file(file, error, sizeof(error));
    fclose(file);
    if (!root)
        return SDL_SetError("%s: %s", path, error);
    bool ok = true;
    for (int i = 0; toml_key_in(root, i); ++i)
        if (SDL_strcmp(toml_key_in(root, i), "display"))
            ok = SDL_SetError("Unknown config section in %s: %s", path, toml_key_in(root, i));
    toml_table_t *display = toml_table_in(root, "display");
    if (toml_key_exists(root, "display") && !display)
        ok = SDL_SetError("display must be a table");
    for (int i = 0; ok && display && toml_key_in(display, i); ++i)
    {
        const char *key = toml_key_in(display, i);
        if (!SDL_strcmp(key, "window_mode"))
        {
            toml_datum_t v = toml_string_in(display, key);
            ok = v.ok ? Mode(v.u.s, &s->graphics.window_mode)
                      : SDL_SetError("window_mode must be a string");
            if (v.ok)
                free(v.u.s);
        }
        else if (!SDL_strcmp(key, "vsync"))
        {
            toml_datum_t v = toml_bool_in(display, key);
            ok = v.ok != 0;
            if (ok)
                s->graphics.vsync = v.u.b != 0;
            else
                SDL_SetError("vsync must be a boolean");
        }
        else
        {
            toml_datum_t v = toml_int_in(display, key);
            if (!v.ok || v.u.i < -1 || v.u.i > 16384)
                ok = SDL_SetError("Invalid integer setting: %s", key);
            else if (!SDL_strcmp(key, "max_fps"))
                s->graphics.max_fps = (int)v.u.i;
            else if (!SDL_strcmp(key, "window_width") && v.u.i >= 320)
                s->graphics.window_width = (int)v.u.i;
            else if (!SDL_strcmp(key, "window_height") && v.u.i >= 200)
                s->graphics.window_height = (int)v.u.i;
            else
                ok = SDL_SetError("Unknown or invalid display setting: %s", key);
        }
    }
    toml_free(root);
    return ok;
}
static const char *Value(int *index, int argc, char **argv, const char *name)
{
    const char *arg = argv[*index];
    const size_t length = SDL_strlen(name);
    if (!SDL_strncmp(arg, name, length) && arg[length] == '=')
        return arg + length + 1;
    if (!SDL_strcmp(arg, name) && *index + 1 < argc)
        return argv[++*index];
    return NULL;
}
bool GameConfigureAt(GameStartup *s, int argc, char **argv, const char *base,
                     const char *preferences)
{
    SDL_zero(*s);
    s->graphics = Grapple_GraphicsDefaults();
    s->graphics.window_mode = GAME_RELEASE ? GRAPPLE_WINDOW_EXCLUSIVE : GRAPPLE_WINDOW_WINDOWED;
    s->graphics.window_width = 1280;
    s->graphics.window_height = 720;
    s->engine.title = "@GAME_ID@";
    s->engine.design_width = 1280;
    s->engine.design_height = 720;
    SDL_SetLogPriorities(GAME_RELEASE ? SDL_LOG_PRIORITY_INFO : SDL_LOG_PRIORITY_DEBUG);
    bool reset = false, defaults = false, safe = false;
    for (int i = 1; i < argc; ++i)
    {
        if (!SDL_strcmp(argv[i], "--self-test"))
            s->self_test = true;
        if (!SDL_strcmp(argv[i], "--reset-settings"))
            reset = true;
        if (!SDL_strcmp(argv[i], "--default-settings"))
            defaults = true;
        if (!SDL_strcmp(argv[i], "--safe-mode"))
            safe = true;
        if (!SDL_strcmp(argv[i], "--print-settings"))
            s->print = true;
    }
    if ((int)reset + (int)defaults + (int)safe > 1)
        return SDL_SetError("Recovery options are mutually exclusive");
    if (!base)
        return false;
    char *config = NULL, *player = NULL, *pref = NULL;
    SDL_asprintf(&s->media, "%smedia", base);
    SDL_asprintf(&config, "%sconfig.toml", base);
    if (!s->media || !config)
    {
        SDL_free(config);
        return false;
    }
    bool ok = true;
    if (!safe)
        ok = LoadConfig(s, config, true);
    SDL_free(config);
    if (!s->self_test)
    {
        pref = preferences ? SDL_strdup(preferences) : SDL_GetPrefPath("games", "@GAME_ID@");
        if (pref)
            SDL_asprintf(&player, "%sconfig.toml", pref);
        if (!player)
            ok = false;
        if (ok && !safe && !reset && !defaults)
            ok = LoadConfig(s, player, false);
    }
    if (safe)
    {
        s->graphics.window_mode = GRAPPLE_WINDOW_WINDOWED;
        s->graphics.max_fps = 60;
        s->engine.backend = GRAPPLE_BACKEND_SOFTWARE;
    }
    // Explicit config files overlay player preferences before individual CLI fields.
    for (int i = 1; ok && i < argc; ++i)
    {
        const char *v = Value(&i, argc, argv, "--config");
        if (v)
            ok = (safe || reset || defaults)
                     ? SDL_SetError("Recovery cannot be combined with --config")
                     : LoadConfig(s, v, true);
    }
    for (int i = 1; ok && i < argc; ++i)
    {
        const char *v = NULL;
        if ((v = Value(&i, argc, argv, "--window-mode")))
            ok = Mode(v, &s->graphics.window_mode);
        else if ((v = Value(&i, argc, argv, "--vsync")))
        {
            if (!SDL_strcmp(v, "on"))
                s->graphics.vsync = true;
            else if (!SDL_strcmp(v, "off"))
                s->graphics.vsync = false;
            else
                ok = SDL_SetError("--vsync requires on or off");
        }
        else if ((v = Value(&i, argc, argv, "--max-fps")))
        {
            char *end = NULL;
            errno = 0;
            long value = strtol(v, &end, 10);
            if (errno || !*v || *end || value < -1 || value > 16384)
                ok = SDL_SetError("Invalid --max-fps");
            else
                s->graphics.max_fps = (int)value;
        }
        else if (Value(&i, argc, argv, "--config"))
        {
        }
        else if (SDL_strcmp(argv[i], "--self-test") && SDL_strcmp(argv[i], "--print-settings") &&
                 SDL_strcmp(argv[i], "--reset-settings") &&
                 SDL_strcmp(argv[i], "--default-settings") && SDL_strcmp(argv[i], "--safe-mode"))
            ok = SDL_SetError("Unknown or incomplete option: %s", argv[i]);
    }
    if (ok && reset && !s->print && player && SDL_GetPathInfo(player, NULL))
    {
        SDL_Time now;
        char *backup = NULL;
        if (!SDL_GetCurrentTime(&now))
            ok = false;
        else
            SDL_asprintf(&backup, "%s.backup-%llu", player, (unsigned long long)now);
        if (!backup)
            ok = false;
        if (ok)
            ok = SDL_RenamePath(player, backup);
        SDL_free(backup);
    }
    if (s->self_test)
    {
        s->graphics.window_mode = GRAPPLE_WINDOW_WINDOWED;
        s->graphics.vsync = false;
        s->graphics.max_fps = 60;
        s->engine.backend = GRAPPLE_BACKEND_SOFTWARE;
    }
    if (ok && s->print)
        printf("window_mode=%s\nvsync=%s\nmax_fps=%d\nplayer_config=%s\n",
               s->graphics.window_mode == GRAPPLE_WINDOW_WINDOWED    ? "windowed"
               : s->graphics.window_mode == GRAPPLE_WINDOW_EXCLUSIVE ? "fullscreen-exclusive"
                                                                     : "fullscreen-borderless",
               s->graphics.vsync ? "on" : "off", s->graphics.max_fps,
               player ? player : "isolated test");
    SDL_free(player);
    SDL_free(pref);
    s->engine.graphics = &s->graphics;
    s->engine.media_path = s->media;
    return ok;
}
bool GameConfigure(GameStartup *s, int argc, char **argv)
{
    return GameConfigureAt(s, argc, argv, SDL_GetBasePath(), NULL);
}
void GameStartupFree(GameStartup *s)
{
    SDL_free(s->media);
}
