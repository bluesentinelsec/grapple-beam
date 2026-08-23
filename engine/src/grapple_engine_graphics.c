/*
 * grapple_engine_graphics.c — the graphics settings themselves.
 * Original Grapple code (zlib).
 *
 * Defaults, validation, the quality-to-numbers helpers, and applying a
 * settings struct to a running engine. Loading and saving live next door in
 * grapple_engine_gfxconfig.c; the post-processing shaders in
 * grapple_engine_postfx.c. Split that way because this file is the part
 * with no dependencies — it is pure data handling, and it is what the tests
 * spend most of their time on.
 */
#include "engine_internal.h"

#include <grapple/engine_graphics.h>

/* --- defaults ------------------------------------------------------------ */

Grapple_GraphicsSettings Grapple_GraphicsDefaults(void)
{
    Grapple_GraphicsSettings s;
    SDL_zero(s);

    s.vsync = true;
    s.max_fps = 0; /* follow the display */
    /* Borderless fullscreen: what a finished game does on launch. Windowed
       is the developer's setting, not the shipping one. */
    s.window_mode = GRAPPLE_WINDOW_BORDERLESS;
    s.window_width = 0; /* the engine's default, when windowed */
    s.window_height = 0;
    s.display = 0;
    s.presentation = GRAPPLE_PRESENT_LETTERBOX;
    s.render_scale = 1.0f;
    s.filter = GRAPPLE_FILTER_AUTO;

    /* Budgets start high. A game should look like itself until something
       says otherwise; the tiers exist for players who need them. */
    s.particles = GRAPPLE_QUALITY_HIGH;
    s.dynamic_lights = GRAPPLE_QUALITY_HIGH;
    s.shadows = GRAPPLE_QUALITY_HIGH;

    /* Everything stylistic starts off. Bloom and CRT are decisions about
       how a game looks, and the engine does not get to make them. */
    s.bloom = 0.0f;
    s.bloom_threshold = 0.7f;
    s.crt = 0.0f;
    s.crt_curvature = 0.0f;
    s.pixelation = 1;
    s.chromatic_aberration = 0.0f;
    /* Anti-aliasing is fidelity rather than style, so it is on. A pixel-art
       game should turn it off along with setting INTEGER presentation. */
    s.antialias = GRAPPLE_AA_FXAA;

    s.brightness = 1.0f;
    s.contrast = 1.0f;
    s.saturation = 1.0f;
    /* Accessibility settings alter the image away from what the artist
       intended — exactly right for the player who needs them, wrong for
       everyone else. Opt-in, both of them. */
    s.color_blind = GRAPPLE_COLORBLIND_NONE;
    s.reduced_flashing = false;
    s.screen_shake = 1.0f;
    s.ui_scale = 1.0f;

    return s;
}

Grapple_GraphicsSettings Grapple_GraphicsSafeMode(void)
{
    Grapple_GraphicsSettings s = Grapple_GraphicsDefaults();

    /* Windowed and resizable, on the primary display, at a size every
       monitor made this century can show. A window that will not display
       correctly can at least be dragged somewhere that will. */
    s.window_mode = GRAPPLE_WINDOW_WINDOWED;
    s.window_width = 1280;
    s.window_height = 720;
    s.display = 0;
    s.presentation = GRAPPLE_PRESENT_LETTERBOX;
    s.vsync = true;
    s.max_fps = 60;

    /* Down, not off: safe mode still has to be playable enough to reach the
       options screen and fix whatever went wrong. */
    s.render_scale = 1.0f;
    s.particles = GRAPPLE_QUALITY_LOW;
    s.dynamic_lights = GRAPPLE_QUALITY_LOW;
    s.shadows = GRAPPLE_QUALITY_OFF;

    /* Every shader effect off. If the post-processing chain is what broke
       the machine, safe mode must not run it. */
    s.bloom = 0.0f;
    s.crt = 0.0f;
    s.crt_curvature = 0.0f;
    s.pixelation = 1;
    s.chromatic_aberration = 0.0f;
    s.antialias = GRAPPLE_AA_OFF;
    s.brightness = 1.0f;
    s.contrast = 1.0f;
    s.saturation = 1.0f;
    s.color_blind = GRAPPLE_COLORBLIND_NONE;
    return s;
}

static float ClampF(float v, float lo, float hi)
{
    if (!(v == v)) /* NaN: a hand-written config can contain one */
    {
        return lo;
    }
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static int ClampI(int v, int lo, int hi)
{
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static Grapple_GraphicsQuality ClampQuality(Grapple_GraphicsQuality q)
{
    if ((int)q < (int)GRAPPLE_QUALITY_OFF)
    {
        return GRAPPLE_QUALITY_OFF;
    }
    if ((int)q > (int)GRAPPLE_QUALITY_HIGH)
    {
        return GRAPPLE_QUALITY_HIGH;
    }
    return q;
}

void Grapple_GraphicsClamp(Grapple_GraphicsSettings *s)
{
    if (s == NULL)
    {
        return;
    }

    if ((int)s->window_mode < 0 || (int)s->window_mode > (int)GRAPPLE_WINDOW_EXCLUSIVE)
    {
        s->window_mode = GRAPPLE_WINDOW_WINDOWED;
    }
    if ((int)s->presentation < 0 || (int)s->presentation > (int)GRAPPLE_PRESENT_NATIVE)
    {
        s->presentation = GRAPPLE_PRESENT_LETTERBOX;
    }
    if ((int)s->filter < 0 || (int)s->filter > (int)GRAPPLE_FILTER_NEAREST)
    {
        s->filter = GRAPPLE_FILTER_AUTO;
    }
    if ((int)s->antialias < 0 || (int)s->antialias > (int)GRAPPLE_AA_FXAA)
    {
        s->antialias = GRAPPLE_AA_OFF;
    }
    if ((int)s->color_blind < 0 || (int)s->color_blind > (int)GRAPPLE_COLORBLIND_TRITANOPIA)
    {
        s->color_blind = GRAPPLE_COLORBLIND_NONE;
    }

    /* A frame cap below 10 is a mistake, not a preference — it would make a
       game unplayable and look like a hang. Negative stays negative: that is
       the documented "no limiter". */
    if (s->max_fps > 0)
    {
        s->max_fps = ClampI(s->max_fps, 10, 1000);
    }

    s->render_scale = ClampF(s->render_scale, 0.25f, 2.0f);

    /* A window smaller than this cannot show a usable UI, and a stored
       negative would be an SDL error rather than a preference. Zero stays
       zero: that is "use the engine's default". */
    if (s->window_width != 0 || s->window_height != 0)
    {
        s->window_width = ClampI(s->window_width, 320, 16384);
        s->window_height = ClampI(s->window_height, 240, 16384);
    }
    if (s->display < 0)
    {
        s->display = 0;
    }
    s->particles = ClampQuality(s->particles);
    s->dynamic_lights = ClampQuality(s->dynamic_lights);
    s->shadows = ClampQuality(s->shadows);

    s->bloom = ClampF(s->bloom, 0.0f, 1.0f);
    s->bloom_threshold = ClampF(s->bloom_threshold, 0.0f, 1.0f);
    s->crt = ClampF(s->crt, 0.0f, 1.0f);
    s->crt_curvature = ClampF(s->crt_curvature, 0.0f, 1.0f);
    s->pixelation = ClampI(s->pixelation, 1, 64);
    s->chromatic_aberration = ClampF(s->chromatic_aberration, 0.0f, 1.0f);

    s->brightness = ClampF(s->brightness, 0.5f, 2.0f);
    s->contrast = ClampF(s->contrast, 0.5f, 2.0f);
    s->saturation = ClampF(s->saturation, 0.0f, 2.0f);
    s->screen_shake = ClampF(s->screen_shake, 0.0f, 1.0f);
    s->ui_scale = ClampF(s->ui_scale, 0.5f, 3.0f);

    /* Photosensitivity is a safety setting, so it overrides the aesthetic
       ones rather than sitting beside them: bloom is what turns a bright
       frame into a flash. */
    if (s->reduced_flashing)
    {
        s->bloom = ClampF(s->bloom, 0.0f, 0.25f);
    }
}

bool Grapple_GraphicsEqual(const Grapple_GraphicsSettings *a,
                             const Grapple_GraphicsSettings *b)
{
    if (a == NULL || b == NULL)
    {
        return a == b;
    }
    /* Field by field rather than memcmp: the struct has padding, and two
       structs that differ only in padding are the same settings. */
    return a->vsync == b->vsync && a->max_fps == b->max_fps &&
           a->window_mode == b->window_mode && a->window_width == b->window_width &&
           a->window_height == b->window_height && a->display == b->display &&
           a->presentation == b->presentation &&
           a->render_scale == b->render_scale && a->filter == b->filter &&
           a->particles == b->particles && a->dynamic_lights == b->dynamic_lights &&
           a->shadows == b->shadows && a->bloom == b->bloom &&
           a->bloom_threshold == b->bloom_threshold && a->crt == b->crt &&
           a->crt_curvature == b->crt_curvature && a->pixelation == b->pixelation &&
           a->chromatic_aberration == b->chromatic_aberration && a->antialias == b->antialias &&
           a->brightness == b->brightness && a->contrast == b->contrast &&
           a->saturation == b->saturation && a->color_blind == b->color_blind &&
           a->reduced_flashing == b->reduced_flashing && a->screen_shake == b->screen_shake &&
           a->ui_scale == b->ui_scale;
}

/* --- budgets, turned into numbers ---------------------------------------- */

float Grapple_GraphicsParticleDensity(Grapple_GraphicsQuality quality)
{
    switch (ClampQuality(quality))
    {
    case GRAPPLE_QUALITY_OFF:
        return 0.0f;
    case GRAPPLE_QUALITY_LOW:
        return 0.25f;
    case GRAPPLE_QUALITY_MEDIUM:
        return 0.6f;
    case GRAPPLE_QUALITY_HIGH:
    default:
        return 1.0f;
    }
}

float Grapple_GraphicsLightMapScale(Grapple_GraphicsQuality quality)
{
    switch (ClampQuality(quality))
    {
    /* Even "off" returns a usable scale: turning dynamic lights off means
       drawing no lights, not rendering the light map at zero pixels. */
    case GRAPPLE_QUALITY_OFF:
    case GRAPPLE_QUALITY_LOW:
        return 0.5f;
    case GRAPPLE_QUALITY_MEDIUM:
        return 0.75f;
    case GRAPPLE_QUALITY_HIGH:
    default:
        return 1.0f;
    }
}

int Grapple_GraphicsMaxDynamicLights(Grapple_GraphicsQuality quality)
{
    switch (ClampQuality(quality))
    {
    case GRAPPLE_QUALITY_OFF:
        return 0;
    case GRAPPLE_QUALITY_LOW:
        return 8;
    case GRAPPLE_QUALITY_MEDIUM:
        return 24;
    case GRAPPLE_QUALITY_HIGH:
    default:
        return 64;
    }
}

int Grapple_GraphicsShadowRays(Grapple_GraphicsQuality quality)
{
    switch (ClampQuality(quality))
    {
    case GRAPPLE_QUALITY_OFF:
        return 0;
    case GRAPPLE_QUALITY_LOW:
        return 32;
    case GRAPPLE_QUALITY_MEDIUM:
        return 96;
    case GRAPPLE_QUALITY_HIGH:
    default:
        return 256;
    }
}

float Grapple_GraphicsShadowSoftness(Grapple_GraphicsQuality quality)
{
    /* Soft edges need rays to be smooth. Below HIGH there are not enough,
       and a soft shadow built from 32 rays reads as banding — so the lower
       tiers get hard shadows, which look deliberate instead of broken. */
    return (ClampQuality(quality) >= GRAPPLE_QUALITY_HIGH) ? 0.35f : 0.0f;
}

const char *Grapple_GraphicsQualityName(Grapple_GraphicsQuality quality)
{
    switch (ClampQuality(quality))
    {
    case GRAPPLE_QUALITY_OFF:
        return "off";
    case GRAPPLE_QUALITY_LOW:
        return "low";
    case GRAPPLE_QUALITY_MEDIUM:
        return "medium";
    case GRAPPLE_QUALITY_HIGH:
    default:
        return "high";
    }
}

bool Grapple_GraphicsQualityFromName(const char *name, Grapple_GraphicsQuality *out)
{
    if (name == NULL || out == NULL)
    {
        return false;
    }
    if (SDL_strcasecmp(name, "off") == 0 || SDL_strcasecmp(name, "none") == 0)
    {
        *out = GRAPPLE_QUALITY_OFF;
        return true;
    }
    if (SDL_strcasecmp(name, "low") == 0)
    {
        *out = GRAPPLE_QUALITY_LOW;
        return true;
    }
    if (SDL_strcasecmp(name, "medium") == 0 || SDL_strcasecmp(name, "med") == 0)
    {
        *out = GRAPPLE_QUALITY_MEDIUM;
        return true;
    }
    if (SDL_strcasecmp(name, "high") == 0)
    {
        *out = GRAPPLE_QUALITY_HIGH;
        return true;
    }
    return false;
}

/* --- monitors ------------------------------------------------------------ */

int Grapple_EngineDisplayCount(void)
{
    int count = 0;
    SDL_DisplayID *ids = SDL_GetDisplays(&count);
    SDL_free(ids);
    return count;
}

/* The SDL_DisplayID for a 0-based index, or 0. Settings store an index
   because an ID is assigned per run and would mean nothing in a file
   written yesterday. */
static SDL_DisplayID DisplayAt(int index)
{
    int count = 0;
    SDL_DisplayID *ids = SDL_GetDisplays(&count);
    SDL_DisplayID id = 0;
    if (ids != NULL && index >= 0 && index < count)
    {
        id = ids[index];
    }
    SDL_free(ids);
    return id;
}

const char *Grapple_EngineDisplayName(int index)
{
    const SDL_DisplayID id = DisplayAt(index);
    return (id != 0) ? SDL_GetDisplayName(id) : NULL;
}

int Grapple_EngineDisplay(Grapple_Engine *engine)
{
    if (engine == NULL || engine->window == NULL)
    {
        return 0;
    }
    const SDL_DisplayID current = SDL_GetDisplayForWindow(engine->window);
    int count = 0;
    SDL_DisplayID *ids = SDL_GetDisplays(&count);
    int index = 0;
    for (int i = 0; i < count; ++i)
    {
        if (ids[i] == current)
        {
            index = i;
            break;
        }
    }
    SDL_free(ids);
    return index;
}

bool Grapple_EngineSetDisplay(Grapple_Engine *engine, int index)
{
    if (engine == NULL || engine->window == NULL)
    {
        SDL_InvalidParamError("engine");
        return false;
    }
    const SDL_DisplayID id = DisplayAt(index);
    if (id == 0)
    {
        SDL_SetError("no display %d (there are %d)", index, Grapple_EngineDisplayCount());
        return false;
    }

    /* Leave fullscreen before moving. A fullscreen window is owned by its
       display, so repositioning it while fullscreen either does nothing or
       leaves it half on each monitor. */
    const Grapple_WindowMode mode = engine->graphics.window_mode;
    if (mode != GRAPPLE_WINDOW_WINDOWED)
    {
        SDL_SetWindowFullscreen(engine->window, false);
        SDL_SyncWindow(engine->window);
    }

    SDL_SetWindowPosition(engine->window, SDL_WINDOWPOS_CENTERED_DISPLAY(id),
                          SDL_WINDOWPOS_CENTERED_DISPLAY(id));
    SDL_SyncWindow(engine->window);

    if (mode != GRAPPLE_WINDOW_WINDOWED)
    {
        SDL_SetWindowFullscreenMode(engine->window,
                                    (mode == GRAPPLE_WINDOW_EXCLUSIVE)
                                        ? SDL_GetDesktopDisplayMode(id)
                                        : NULL);
        SDL_SetWindowFullscreen(engine->window, true);
        SDL_SyncWindow(engine->window);
    }

    engine->graphics.display = index;
    /* The new monitor may be a different size or density, so the design
       space has to be re-fitted to it before anything draws. */
    Grapple_EngineSetPresentation(engine, engine->presentation);
    return true;
}

/* --- applying to a running engine ---------------------------------------- */

const Grapple_GraphicsSettings *Grapple_EngineGraphics(Grapple_Engine *engine)
{
    return (engine != NULL) ? &engine->graphics : NULL;
}

bool Grapple_EngineEffectsAvailable(Grapple_Engine *engine)
{
    return (engine != NULL) && Grapple_EnginePostFXAvailable(engine);
}

bool Grapple_EngineSetGraphics(Grapple_Engine *engine, const Grapple_GraphicsSettings *in)
{
    if (engine == NULL || in == NULL)
    {
        SDL_InvalidParamError("engine/settings");
        return false;
    }

    Grapple_GraphicsSettings next = *in;
    Grapple_GraphicsClamp(&next);

    const Grapple_GraphicsSettings previous = engine->graphics;
    engine->graphics = next;

    if (engine->renderer == NULL)
    {
        return true; /* settings-only engine: nothing to apply them to yet */
    }

    if (next.vsync != previous.vsync || engine->frame_count == 0)
    {
        SDL_SetRenderVSync(engine->renderer, next.vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
    }

    engine->max_fps = next.max_fps;

    if (next.presentation != previous.presentation || engine->frame_count == 0)
    {
        Grapple_EngineSetPresentation(engine, next.presentation);
    }

    if (engine->window != NULL && next.display != previous.display)
    {
        Grapple_EngineSetDisplay(engine, next.display);
    }

    if (engine->window != NULL && next.window_mode == GRAPPLE_WINDOW_WINDOWED &&
        next.window_width > 0 && next.window_height > 0 &&
        (next.window_width != previous.window_width ||
         next.window_height != previous.window_height ||
         next.window_mode != previous.window_mode))
    {
        SDL_SetWindowSize(engine->window, next.window_width, next.window_height);
    }

    if (engine->window != NULL &&
        (next.window_mode != previous.window_mode || engine->frame_count == 0))
    {
        switch (next.window_mode)
        {
        case GRAPPLE_WINDOW_BORDERLESS:
            /* A NULL display mode is what makes SDL's fullscreen the
               borderless-desktop kind rather than a mode switch. */
            SDL_SetWindowFullscreenMode(engine->window, NULL);
            SDL_SetWindowFullscreen(engine->window, true);
            break;
        case GRAPPLE_WINDOW_EXCLUSIVE: {
            const SDL_DisplayID display = SDL_GetDisplayForWindow(engine->window);
            const SDL_DisplayMode *mode = SDL_GetDesktopDisplayMode(display);
            SDL_SetWindowFullscreenMode(engine->window, mode);
            SDL_SetWindowFullscreen(engine->window, true);
            break;
        }
        case GRAPPLE_WINDOW_WINDOWED:
        default:
            SDL_SetWindowFullscreen(engine->window, false);
            break;
        }
    }

    Grapple_EngineApplyFilter(engine);
    Grapple_EnginePostFXInvalidate(engine);
    return true;
}

void Grapple_EngineApplyFilter(Grapple_Engine *engine)
{
    if (engine == NULL || engine->renderer == NULL)
    {
        return;
    }
    SDL_ScaleMode mode;
    switch (engine->graphics.filter)
    {
    case GRAPPLE_FILTER_NEAREST:
        mode = SDL_SCALEMODE_NEAREST;
        break;
    case GRAPPLE_FILTER_LINEAR:
        mode = SDL_SCALEMODE_LINEAR;
        break;
    case GRAPPLE_FILTER_AUTO:
    default:
        /* Integer scaling exists for pixel art, and pixel art wants point
           sampling — so AUTO follows the presentation mode. */
        mode = (engine->presentation == GRAPPLE_PRESENT_INTEGER) ? SDL_SCALEMODE_NEAREST
                                                                   : SDL_SCALEMODE_LINEAR;
        break;
    }
    SDL_SetDefaultTextureScaleMode(engine->renderer, mode);
}
