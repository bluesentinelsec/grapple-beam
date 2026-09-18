#include "settings_internal.h"

#include <stdlib.h>
#include <toml.h>

static bool ReadValue(Grapple_Settings *settings, const char *key, const char *value,
                      const char *source)
{
    char canonical[256];
    SDL_strlcpy(canonical, key, sizeof(canonical));
    if (!SDL_strncmp(key, "image.", 6))
        SDL_snprintf(canonical, sizeof(canonical), "accessibility.%s", key + 6);
    else if (!SDL_strchr(key, '.'))
        for (char *p = canonical; *p; ++p)
            if (*p == '_')
                *p = '-';
    if (!SDL_strcmp(canonical, "display.window_mode") || !SDL_strcmp(canonical, "window-mode"))
    {
        if (!SDL_strcmp(value, "borderless"))
            value = "fullscreen-borderless";
        if (!SDL_strcmp(value, "exclusive"))
            value = "fullscreen-exclusive";
    }
    if (!SDL_strcmp(canonical, "display.max_fps") || !SDL_strcmp(canonical, "max-fps"))
    {
        if (!SDL_strcmp(value, "0"))
            value = "display";
        if (!SDL_strcmp(value, "-1"))
            value = "unlimited";
    }
    return Grapple_SettingsSet(settings, canonical, value, source);
}
static bool LegacySize(Grapple_Settings *settings, const toml_table_t *table, const char *source)
{
    const char *names[] = {"window_width", "window_height"};
    int width = 0, height = 0;
    SDL_sscanf(Grapple_SettingsGet(settings, "window-size"), "%dx%d", &width, &height);
    int *dimensions[] = {&width, &height};
    bool provided = false;
    for (int i = 0; i < 2; ++i)
    {
        if (!toml_raw_in(table, names[i]))
        {
            if (toml_array_in(table, names[i]) || toml_table_in(table, names[i]))
                return SDL_SetError("%s: %s must be an integer", source, names[i]);
            continue;
        }
        toml_datum_t value = toml_int_in(table, names[i]);
        if (!value.ok || value.u.i < 0 || value.u.i > 16384)
            return SDL_SetError("%s: invalid legacy window dimension %s", source, names[i]);
        if (value.u.i > 0)
        {
            *dimensions[i] = (int)value.u.i;
            provided = true;
        }
    }
    if (!provided)
        return true;
    char size[64];
    SDL_snprintf(size, sizeof(size), "%dx%d", width, height);
    return Grapple_SettingsSet(settings, "window-size", size, source);
}
static bool ReadTable(Grapple_Settings *settings, const toml_table_t *table, const char *prefix,
                      const char *source, int depth)
{
    if (depth > 8)
        return SDL_SetError("settings TOML nesting exceeds 8 tables");
    const bool legacy_size = !*prefix || !SDL_strcmp(prefix, "display");
    if (legacy_size && !LegacySize(settings, table, source))
        return false;
    for (int i = 0;; ++i)
    {
        const char *key = toml_key_in(table, i);
        if (!key)
            break;
        if (legacy_size && (!SDL_strcmp(key, "window_width") || !SDL_strcmp(key, "window_height")))
            continue;
        char full[256];
        int len = SDL_snprintf(full, sizeof(full), "%s%s%s", prefix, *prefix ? "." : "", key);
        if (len < 0 || (size_t)len >= sizeof(full))
            return SDL_SetError("settings key is too long");
        toml_table_t *child = toml_table_in(table, key);
        if (child)
        {
            bool known = !SDL_strcmp(full, "image");
            for (int j = 0; j < Grapple_SettingCount() && !known; ++j)
            {
                const char *registered = Grapple_SettingKey(j);
                const size_t length = SDL_strlen(full);
                known = SDL_strncmp(registered, full, length) == 0 && registered[length] == '.';
            }
            if (!known)
                return SDL_SetError("%s: unknown settings table %s", source, full);
            if (!ReadTable(settings, child, full, source, depth + 1))
                return false;
            continue;
        }
        toml_raw_t raw = toml_raw_in(table, key);
        char *str = NULL;
        char value[96];
        int b;
        int64_t integer;
        double number;
        if (!raw)
            return SDL_SetError("%s: %s must be a scalar setting", source, full);
        if (!toml_rtos(raw, &str))
        {
            bool ok = ReadValue(settings, full, str, source);
            free(str);
            if (!ok)
                return false;
        }
        else
        {
            if (!toml_rtob(raw, &b))
                SDL_strlcpy(value, b ? "on" : "off", sizeof(value));
            else if (!toml_rtoi(raw, &integer))
                SDL_snprintf(value, sizeof(value), "%lld", (long long)integer);
            else if (!toml_rtod(raw, &number))
                SDL_snprintf(value, sizeof(value), "%.17g", number);
            else
                return SDL_SetError("%s: unsupported value type for %s", source, full);
            if (!ReadValue(settings, full, value, source))
                return false;
        }
    }
    return true;
}
bool Grapple_SettingsLoadToml(Grapple_Settings *s, const char *text, const char *source)
{
    if (!s || !text)
        return SDL_InvalidParamError("settings/toml");
    if (SDL_strlen(text) > 1024 * 1024)
        return SDL_SetError("settings TOML exceeds 1 MiB");
    char *copy = SDL_strdup(text);
    if (!copy)
        return false;
    char error[256];
    toml_table_t *table = toml_parse(copy, error, sizeof(error));
    SDL_free(copy);
    if (!table)
        return SDL_SetError("%s: %s", source ? source : "TOML", error);
    Grapple_Settings *next = Grapple_CloneSettings(s);
    bool ok = next && ReadTable(next, table, "", source ? source : "TOML", 0);
    toml_free(table);
    if (ok)
        Settings_Swap(s, next);
    Grapple_DestroySettings(next);
    return ok;
}
bool Grapple_SettingsLoadFile(Grapple_Settings *s, const char *path)
{
    if (!s || !path)
        return SDL_InvalidParamError("settings/path");
    size_t size = 0;
    char *text = SDL_LoadFile(path, &size);
    if (!text)
        return false;
    if (size > 1024 * 1024 || SDL_strlen(text) != size)
    {
        SDL_free(text);
        return SDL_SetError("settings file is too large or contains NUL bytes");
    }
    Grapple_Settings *next = Grapple_CloneSettings(s);
    bool ok = next && Grapple_SettingsLoadToml(next, text, path);
    SDL_free(text);
    if (ok)
    {
        const char *media = Grapple_SettingsGet(next, "media");
        if (*media && !SDL_strcmp(Grapple_SettingsSource(next, "media"), path) && media[0] != '/' &&
            media[0] != '\\' && !(SDL_strlen(media) > 1 && media[1] == ':'))
        {
            const char *slash = SDL_strrchr(path, '/'), *back = SDL_strrchr(path, '\\');
            if (back && (!slash || back > slash))
                slash = back;
            if (slash)
            {
                char *absolute = NULL;
                SDL_asprintf(&absolute, "%.*s/%s", (int)(slash - path), path, media);
                ok = absolute && Grapple_SettingsSet(next, "media", absolute, path);
                SDL_free(absolute);
            }
        }
    }
    if (ok)
        Settings_Swap(s, next);
    Grapple_DestroySettings(next);
    return ok;
}
static bool WriteQuoted(SDL_IOStream *io, const char *text)
{
    if (SDL_WriteIO(io, "\"", 1) != 1)
        return false;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p)
    {
        if (*p == '"' || *p == '\\')
        {
            if (SDL_WriteIO(io, "\\", 1) != 1)
                return false;
        }
        if (*p < 32 || *p == 127)
        {
            if (!SDL_IOprintf(io, "\\u%04x", (unsigned int)*p))
                return false;
        }
        else if (SDL_WriteIO(io, p, 1) != 1)
            return false;
    }
    return SDL_WriteIO(io, "\"", 1) == 1;
}
char *Grapple_SettingsToToml(const Grapple_Settings *s, bool only)
{
    if (!s)
    {
        SDL_InvalidParamError("settings");
        return NULL;
    }
    SDL_IOStream *io = SDL_IOFromDynamicMem();
    if (!io)
        return NULL;
    bool ok = true;
    for (int i = 0; i < Grapple_SettingCount() && ok; ++i)
    {
        if ((only && !s->provided[i]) || !*s->values[i])
            continue;
        ok = WriteQuoted(io, Grapple_SettingKey(i)) && SDL_WriteIO(io, " = ", 3) == 3 &&
             WriteQuoted(io, s->values[i]) && SDL_WriteIO(io, "\n", 1) == 1;
    }
    if (ok)
        ok = SDL_WriteIO(io, "\0", 1) == 1;
    char *result = NULL;
    if (ok)
    {
        const char *buffer = SDL_GetPointerProperty(SDL_GetIOProperties(io),
                                                    SDL_PROP_IOSTREAM_DYNAMIC_MEMORY_POINTER, NULL);
        result = SDL_strdup(buffer);
    }
    SDL_CloseIO(io);
    return result;
}
bool Grapple_SettingsSaveChanges(const Grapple_Settings *changes, const char *path)
{
    if (!changes || !path)
        return SDL_InvalidParamError("changes/path");
    Grapple_Settings *saved = Grapple_CreateSettings();
    if (!saved)
        return false;
    SDL_PathInfo info;
    bool exists = SDL_GetPathInfo(path, &info);
    bool ok = (!exists || Grapple_SettingsLoadFile(saved, path)) &&
              Grapple_SettingsOverlay(saved, changes);
    char *text = ok ? Grapple_SettingsToToml(saved, true) : NULL;
    Grapple_DestroySettings(saved);
    if (!text)
        return false;
    char *temporary = NULL;
    SDL_Time timestamp = 0;
    SDL_GetCurrentTime(&timestamp);
    SDL_asprintf(&temporary, "%s.tmp-%llu-%llu", path, (unsigned long long)timestamp,
                 (unsigned long long)SDL_GetTicksNS());
    if (!temporary)
    {
        SDL_free(text);
        return false;
    }
    SDL_IOStream *io = SDL_IOFromFile(temporary, "wb");
    ok = io != NULL;
    if (io)
    {
        ok = SDL_WriteIO(io, text, SDL_strlen(text)) == SDL_strlen(text) && SDL_FlushIO(io);
        if (!SDL_CloseIO(io))
            ok = false;
    }
    if (ok)
        ok = SDL_RenamePath(temporary, path);
    if (!ok)
    {
        char error[512];
        SDL_strlcpy(error, SDL_GetError(), sizeof(error));
        SDL_RemovePath(temporary);
        SDL_SetError("%s", error);
    }
    SDL_free(text);
    SDL_free(temporary);
    return ok;
}
