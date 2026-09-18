#include "settings_internal.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum SettingKind
{
    SET_ENUM,
    SET_STRING_ENUM,
    SET_BOOL,
    SET_INVERT,
    SET_FLOAT,
    SET_INT,
    SET_SIZE,
    SET_DISPLAY,
    SET_RESOLUTION,
    SET_REFRESH,
    SET_FPS,
    SET_PATH,
    SET_STRING
} SettingKind;
typedef enum SettingTarget
{
    TARGET_NONE,
    TARGET_GFX,
    TARGET_CONFIG
} SettingTarget;
typedef struct SettingSpec
{
    const char *key, *option, *initial, *choices;
    SettingKind kind;
    double minimum, maximum;
    SettingTarget target;
    size_t offset;
} SettingSpec;
static const SettingSpec kSpecs[] = {
#include "settings_schema.inc"
};
SDL_COMPILE_TIME_ASSERT(settings_capacity, SDL_arraysize(kSpecs) <= SETTINGS_CAPACITY);
static const Grapple_Settings *g_launch;

static int Find(const char *key)
{
    if (key)
        for (int i = 0; i < (int)SDL_arraysize(kSpecs); ++i)
            if (!SDL_strcmp(key, kSpecs[i].key) ||
                (kSpecs[i].option[0] && !SDL_strcmp(key, kSpecs[i].option)))
                return i;
    return -1;
}
static int Choice(const char *choices, const char *value)
{
    int index = 0;
    for (const char *p = choices; *p; ++index)
    {
        const char *end = SDL_strchr(p, '|');
        size_t n = end ? (size_t)(end - p) : SDL_strlen(p);
        if (SDL_strlen(value) == n && !SDL_strncmp(p, value, n))
            return index;
        if (!end)
            break;
        p = end + 1;
    }
    return -1;
}
static bool Number(const char *value, double lo, double hi, bool integer)
{
    if (!value || !*value)
        return false;
    for (const char *p = value; *p; ++p)
        if (SDL_isspace((unsigned char)*p) || (integer && (*p < '0' || *p > '9')))
            return false;
    errno = 0;
    char *end = NULL;
    double n = SDL_strtod(value, &end);
    return end != value && !*end && errno != ERANGE && !SDL_isnan(n) && !SDL_isinf(n) && n >= lo &&
           n <= hi;
}
static bool Size(const char *value, int lo, int hi, int *w, int *h)
{
    const char *x = SDL_strchr(value, 'x');
    if (!x || x == value || (size_t)(x - value) >= 24)
        return false;
    char first[24];
    SDL_memcpy(first, value, (size_t)(x - value));
    first[x - value] = '\0';
    int min_h = lo == 320 ? 240 : lo;
    if (!Number(first, lo, hi, true) || !Number(x + 1, min_h, hi, true))
        return false;
    *w = SDL_atoi(first);
    *h = SDL_atoi(x + 1);
    return true;
}
static bool Valid(const SettingSpec *s, const char *v)
{
    if (!v || SDL_strlen(v) > 4096)
        return false;
    int w, h;
    switch (s->kind)
    {
    case SET_ENUM:
    case SET_STRING_ENUM:
        return Choice(s->choices, v) >= 0;
    case SET_BOOL:
    case SET_INVERT:
        return !SDL_strcmp(v, "on") || !SDL_strcmp(v, "off");
    case SET_FLOAT:
        return Number(v, s->minimum, s->maximum, false);
    case SET_INT:
        return Number(v, s->minimum, s->maximum, true);
    case SET_FPS:
        return !SDL_strcmp(v, "display") || !SDL_strcmp(v, "unlimited") ||
               Number(v, s->minimum, s->maximum, true);
    case SET_DISPLAY:
        return !SDL_strcmp(v, "primary") || Number(v, s->minimum, s->maximum, true);
    case SET_REFRESH:
        return !SDL_strcmp(v, "desktop") || Number(v, s->minimum, s->maximum, false);
    case SET_RESOLUTION:
        if (!SDL_strcmp(v, "desktop"))
            return true; /* fallthrough */
    case SET_SIZE:
        return Size(v, (int)s->minimum, (int)s->maximum, &w, &h);
    case SET_PATH:
    case SET_STRING:
        return *v != '\0';
    }
    return false;
}
int Grapple_SettingCount(void)
{
    return (int)SDL_arraysize(kSpecs);
}
const char *Grapple_SettingKey(int i)
{
    return i >= 0 && i < Grapple_SettingCount() ? kSpecs[i].key : NULL;
}
const char *Grapple_SettingOption(int i)
{
    return i >= 0 && i < Grapple_SettingCount() ? kSpecs[i].option : NULL;
}
const char *Grapple_SettingChoices(int i)
{
    if (i < 0 || i >= Grapple_SettingCount())
        return NULL;
    const SettingSpec *s = &kSpecs[i];
    if (*s->choices)
        return s->choices;
    switch (s->kind)
    {
    case SET_BOOL:
    case SET_INVERT:
        return "on|off";
    case SET_FPS:
        return "display|unlimited|10..1000";
    case SET_DISPLAY:
        return "primary|zero-based index";
    case SET_RESOLUTION:
        return "desktop|WIDTHxHEIGHT";
    case SET_REFRESH:
        return "desktop|Hz";
    case SET_SIZE:
        return "WIDTHxHEIGHT";
    case SET_INT:
        return "integer within documented range";
    case SET_FLOAT:
        return "finite number within documented range";
    default:
        return "text";
    }
}
void Grapple_DestroySettings(Grapple_Settings *s)
{
    if (s)
    {
        for (int i = 0; i < Grapple_SettingCount(); ++i)
        {
            SDL_free(s->values[i]);
            SDL_free(s->sources[i]);
        }
        SDL_free(s->player_path);
        SDL_free(s);
    }
}
Grapple_Settings *Grapple_CreateSettings(void)
{
    Grapple_Settings *s = SDL_calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        s->values[i] = SDL_strdup(kSpecs[i].initial);
        s->sources[i] = SDL_strdup("engine defaults");
        if (!s->values[i] || !s->sources[i])
        {
            Grapple_DestroySettings(s);
            return NULL;
        }
    }
    return s;
}
Grapple_Settings *Grapple_CloneSettings(const Grapple_Settings *from)
{
    if (!from)
    {
        SDL_InvalidParamError("settings");
        return NULL;
    }
    Grapple_Settings *s = SDL_calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        s->values[i] = SDL_strdup(from->values[i]);
        s->sources[i] = SDL_strdup(from->sources[i]);
        s->provided[i] = from->provided[i];
        if (!s->values[i] || !s->sources[i])
        {
            Grapple_DestroySettings(s);
            return NULL;
        }
    }
    if (from->player_path && !Grapple_SettingsSetPlayerPath(s, from->player_path))
    {
        Grapple_DestroySettings(s);
        return NULL;
    }
    return s;
}
bool Grapple_SettingsSet(Grapple_Settings *s, const char *key, const char *value,
                         const char *source)
{
    int i = Find(key);
    if (!s || i < 0)
        return SDL_SetError("unknown setting '%s'", key ? key : "(null)");
    if (!Valid(&kSpecs[i], value))
        return SDL_SetError("invalid value '%s' for %s (%s)", value ? value : "(null)", key,
                            Grapple_SettingChoices(i));
    char *v = SDL_strdup(value), *origin = SDL_strdup(source ? source : "API");
    if (!v || !origin)
    {
        SDL_free(v);
        SDL_free(origin);
        return false;
    }
    SDL_free(s->values[i]);
    SDL_free(s->sources[i]);
    s->values[i] = v;
    s->sources[i] = origin;
    s->provided[i] = true;
    return true;
}
const char *Grapple_SettingsGet(const Grapple_Settings *s, const char *key)
{
    int i = Find(key);
    return s && i >= 0 ? s->values[i] : NULL;
}
const char *Grapple_SettingsSource(const Grapple_Settings *s, const char *key)
{
    int i = Find(key);
    return s && i >= 0 ? s->sources[i] : NULL;
}
void Settings_Swap(Grapple_Settings *a, Grapple_Settings *b)
{
    Grapple_Settings t = *a;
    *a = *b;
    *b = t;
}
bool Grapple_SettingsOverlay(Grapple_Settings *s, const Grapple_Settings *over)
{
    if (!s || !over)
        return SDL_InvalidParamError("settings");
    Grapple_Settings *copy = Grapple_CloneSettings(s);
    if (!copy)
        return false;
    bool ok = !over->player_path || Grapple_SettingsSetPlayerPath(copy, over->player_path);
    for (int i = 0; i < Grapple_SettingCount() && ok; ++i)
        if (over->provided[i])
            ok = Grapple_SettingsSet(copy, kSpecs[i].key, over->values[i], over->sources[i]);
    if (ok)
        Settings_Swap(s, copy);
    Grapple_DestroySettings(copy);
    return ok;
}
bool Grapple_SettingsQuality(Grapple_Settings *s, const char *quality, const char *source)
{
    if (!s || !quality || Choice("low|medium|high", quality) < 0)
        return SDL_SetError("quality must be low, medium or high");
    Grapple_Settings *copy = Grapple_CloneSettings(s);
    if (!copy)
        return false;
    bool ok = Grapple_SettingsSet(copy, "particles", quality, source) &&
              Grapple_SettingsSet(copy, "lights", quality, source) &&
              Grapple_SettingsSet(copy, "shadows", quality, source) &&
              Grapple_SettingsSet(copy, "render-scale", !SDL_strcmp(quality, "low") ? ".75" : "1",
                                  source) &&
              Grapple_SettingsSet(copy, "antialias", !SDL_strcmp(quality, "low") ? "off" : "fxaa",
                                  source);
    if (ok)
        Settings_Swap(s, copy);
    Grapple_DestroySettings(copy);
    return ok;
}
bool Grapple_SettingsApply(const Grapple_Settings *s, Grapple_EngineConfig *config,
                           Grapple_GraphicsSettings *gfx, bool only)
{
    if (!s || !config || !gfx)
        return SDL_InvalidParamError("settings/config/graphics");
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        const SettingSpec *spec = &kSpecs[i];
        const char *v = s->values[i];
        if ((only && !s->provided[i]) || spec->target == TARGET_NONE)
            continue;
        char *dst = (spec->target == TARGET_GFX ? (char *)gfx : (char *)config) + spec->offset;
        int n = 0, w = 0, h = 0;
        float f;
        switch (spec->kind)
        {
        case SET_BOOL:
        case SET_INVERT: {
            bool b = !SDL_strcmp(v, "on");
            if (spec->kind == SET_INVERT)
                b = !b;
            SDL_memcpy(dst, &b, sizeof(b));
            break;
        }
        case SET_ENUM:
            n = Choice(spec->choices, v);
            SDL_memcpy(dst, &n, sizeof(n));
            break;
        case SET_INT:
            n = SDL_atoi(v);
            SDL_memcpy(dst, &n, sizeof(n));
            break;
        case SET_FLOAT:
            f = (float)SDL_atof(v);
            SDL_memcpy(dst, &f, sizeof(f));
            break;
        case SET_DISPLAY:
            gfx->primary_display = !SDL_strcmp(v, "primary");
            gfx->display = gfx->primary_display ? 0 : SDL_atoi(v);
            break;
        case SET_FPS:
            gfx->max_fps = !SDL_strcmp(v, "display")     ? 0
                           : !SDL_strcmp(v, "unlimited") ? -1
                                                         : SDL_atoi(v);
            break;
        case SET_REFRESH:
            gfx->refresh_rate = !SDL_strcmp(v, "desktop") ? 0 : (float)SDL_atof(v);
            break;
        case SET_RESOLUTION:
        case SET_SIZE:
            if (SDL_strcmp(v, "desktop"))
                Size(v, (int)spec->minimum, (int)spec->maximum, &w, &h);
            SDL_memcpy(dst, &w, sizeof(w));
            SDL_memcpy(dst + sizeof(w), &h, sizeof(h));
            break;
        case SET_STRING:
        case SET_STRING_ENUM:
        case SET_PATH:
            if (*v)
                SDL_memcpy(dst, &v, sizeof(v));
            break;
        }
    }
    config->graphics = gfx;
    return true;
}
void Grapple_SetLaunchSettings(const Grapple_Settings *s)
{
    g_launch = s;
}
const Grapple_Settings *Grapple_GetLaunchSettings(void)
{
    return g_launch;
}

bool Settings_Capture(Grapple_Settings *s, const Grapple_EngineConfig *config,
                      const Grapple_GraphicsSettings *gfx, const char *source)
{
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        const SettingSpec *spec = &kSpecs[i];
        if (spec->target == TARGET_NONE || (spec->target == TARGET_CONFIG && !config))
            continue;
        const char *ptr =
            (spec->target == TARGET_GFX ? (const char *)gfx : (const char *)config) + spec->offset;
        char buffer[128];
        const char *value = buffer;
        int n = 0;
        float f = 0;
        bool b = false;
        switch (spec->kind)
        {
        case SET_BOOL:
        case SET_INVERT:
            SDL_memcpy(&b, ptr, sizeof(b));
            value = (b != (spec->kind == SET_INVERT)) ? "on" : "off";
            break;
        case SET_ENUM: {
            SDL_memcpy(&n, ptr, sizeof(n));
            const char *start = spec->choices;
            for (int j = 0; j < n && start; ++j)
            {
                start = SDL_strchr(start, '|');
                if (start)
                    ++start;
            }
            if (!start)
                continue;
            const char *end = SDL_strchr(start, '|');
            SDL_snprintf(buffer, sizeof(buffer), "%.*s",
                         (int)(end ? end - start : (ptrdiff_t)SDL_strlen(start)), start);
            break;
        }
        case SET_INT:
        case SET_FPS:
            SDL_memcpy(&n, ptr, sizeof(n));
            if (spec->kind == SET_FPS && n <= 0)
                value = n == 0 ? "display" : "unlimited";
            else if (n <= 0)
                continue;
            else
                SDL_snprintf(buffer, sizeof(buffer), "%d", n);
            break;
        case SET_FLOAT:
        case SET_REFRESH:
            SDL_memcpy(&f, ptr, sizeof(f));
            if (spec->kind == SET_REFRESH && f == 0)
                value = "desktop";
            else
                SDL_snprintf(buffer, sizeof(buffer), "%.9g", (double)f);
            break;
        case SET_DISPLAY:
            if (gfx->primary_display)
                value = "primary";
            else
                SDL_snprintf(buffer, sizeof(buffer), "%d", gfx->display);
            break;
        case SET_SIZE:
        case SET_RESOLUTION: {
            int width, height;
            SDL_memcpy(&width, ptr, sizeof(width));
            SDL_memcpy(&height, ptr + sizeof(width), sizeof(height));
            if (width <= 0 || height <= 0)
            {
                if (spec->kind == SET_RESOLUTION)
                    value = "desktop";
                else
                    continue;
            }
            else
                SDL_snprintf(buffer, sizeof(buffer), "%dx%d", width, height);
            break;
        }
        case SET_PATH:
        case SET_STRING:
        case SET_STRING_ENUM:
            SDL_memcpy(&value, ptr, sizeof(value));
            if (!value || !*value)
                continue;
            break;
        }
        char *v = SDL_strdup(value), *origin = SDL_strdup(source);
        if (!v || !origin)
        {
            SDL_free(v);
            SDL_free(origin);
            return false;
        }
        SDL_free(s->values[i]);
        SDL_free(s->sources[i]);
        s->values[i] = v;
        s->sources[i] = origin;
        s->provided[i] = true;
    }
    return true;
}
Grapple_Settings *Grapple_SettingsCapture(const Grapple_EngineConfig *config,
                                          const Grapple_GraphicsSettings *graphics)
{
    if (!config || !graphics)
    {
        SDL_InvalidParamError("config/graphics");
        return NULL;
    }
    Grapple_Settings *s = Grapple_CreateSettings();
    if (s && !Settings_Capture(s, config, graphics, "game startup"))
    {
        Grapple_DestroySettings(s);
        return NULL;
    }
    return s;
}
const char *Grapple_SettingPolicy(const char *key)
{
    const int index = Find(key);
    if (index < 0)
        return NULL;
    const SettingSpec *s = &kSpecs[index];
    if (s->target == TARGET_NONE)
        return "audio";
    if (s->target == TARGET_CONFIG)
        return "restart";
    if (!SDL_strncmp(s->key, "quality.", 8) || !SDL_strcmp(s->option, "ui-scale") ||
        !SDL_strcmp(s->option, "screen-shake"))
        return "game";
    return "graphics";
}
bool Grapple_SettingsSetPlayerPath(Grapple_Settings *s, const char *path)
{
    if (!s || !path || !*path)
        return SDL_InvalidParamError("settings/path");
    char *copy = SDL_strdup(path);
    if (!copy)
        return false;
    SDL_free(s->player_path);
    s->player_path = copy;
    return true;
}
const char *Grapple_SettingsPlayerPath(const Grapple_Settings *s)
{
    return s ? s->player_path : NULL;
}
