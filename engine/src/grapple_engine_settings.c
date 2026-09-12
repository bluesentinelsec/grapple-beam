#include "engine_internal.h"
#include "settings_internal.h"
const Grapple_Settings *Grapple_EngineRequestedSettings(Grapple_Engine *engine)
{
    return engine ? engine->requested_settings : NULL;
}
Grapple_Settings *Grapple_EngineActualSettings(Grapple_Engine *engine)
{
    if (!engine || !engine->requested_settings)
        return NULL;
    Grapple_Settings *s = Grapple_CloneSettings(engine->requested_settings);
    if (!s)
        return NULL;
    Grapple_GraphicsSettings gfx = engine->graphics;
    gfx.primary_display = false;
    if (gfx.effects_disabled || !Grapple_EngineEffectsAvailable(engine))
    {
        gfx.bloom = gfx.crt = gfx.crt_curvature = gfx.chromatic_aberration = 0;
        gfx.pixelation = 1;
        gfx.antialias = GRAPPLE_AA_OFF;
    }
    if (!Grapple_EngineEffectsAvailable(engine))
    {
        gfx.brightness = gfx.contrast = gfx.saturation = 1;
        gfx.color_blind = GRAPPLE_COLORBLIND_NONE;
    }
    if (engine->window)
    {
        SDL_GetWindowSize(engine->window, &gfx.window_width, &gfx.window_height);
        const SDL_DisplayMode *mode =
            SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(engine->window));
        if (mode)
            gfx.refresh_rate = mode->refresh_rate;
    }
    if (!Settings_Capture(s, NULL, &gfx, "runtime"))
    {
        Grapple_DestroySettings(s);
        return NULL;
    }
    char number[64];
    SDL_snprintf(number, sizeof(number), "%d", engine->tick_rate);
    Grapple_SettingsSet(s, "engine.tick_rate", number, "runtime");
    SDL_snprintf(number, sizeof(number), "%d", engine->max_steps);
    Grapple_SettingsSet(s, "engine.max_steps_per_frame", number, "runtime");
    SDL_snprintf(number, sizeof(number), "%.9g", (double)engine->max_frame_ns / 1e9);
    Grapple_SettingsSet(s, "engine.max_frame_seconds", number, "runtime");
    if (engine->renderer)
    {
        const char *name = SDL_GetRendererName(engine->renderer);
        if (!SDL_strcmp(name, "direct3d11"))
            name = "directx11";
        else if (!SDL_strcmp(name, "direct3d12"))
            name = "directx12";
        Grapple_SettingsSet(s, "backend", name, "runtime");
    }
    return s;
}
