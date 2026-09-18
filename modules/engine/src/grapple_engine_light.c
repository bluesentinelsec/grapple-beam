/*
 * grapple_engine_light.c — lighting, wired to actors and the camera.
 * Original Grapple code (zlib), over Grapple::Light.
 *
 * The lighting module knows how to light a scene. This file supplies the
 * three things it cannot know on its own: where the camera is, which actors
 * are carrying lights this frame, and what the player set the quality
 * slider to. That is the whole job — and it is exactly the bookkeeping
 * every game would otherwise write again, once, slightly differently.
 */
#include "engine_internal.h"

#include <grapple/engine_light.h>
#include <grapple/engine_physics.h>
#include <grapple/light.h>

/* The hour each preset corresponds to, so a game that switches on the clock
   afterwards carries on from a time that matches what is on screen. */
#define HOUR_SUNRISE 6.5f
#define HOUR_AFTERNOON 13.0f
#define HOUR_SUNSET 19.0f
#define HOUR_NIGHT 1.0f

struct Grapple_EngineLight
{
    Grapple_LightScene *scene;
    Grapple_LightPreset preset;
    SDL_FColor ambient;
    bool custom_ambient; /* the game said what it wants; the clock defers */

    float hour;
    float hours_per_second;

    bool auto_occluders;
    int submitted;

    /* The game's own occluders and dark zones, buffered until the render.
       Buffering rather than requiring them between two calls: an API where
       AddOccluder only works inside an invisible window is one that fails
       silently when somebody calls it in the wrong place. */
    SDL_FRect *occluders;
    int occluder_count, occluder_capacity;
    struct { float x1, y1, x2, y2; } *segments;
    int segment_count, segment_capacity;
    struct { SDL_FRect area; SDL_FColor ambient; } *dark_zones;
    int dark_count, dark_capacity;
};

static struct Grapple_EngineLight *Light(Grapple_Engine *engine, bool create)
{
    if (engine == NULL)
    {
        return NULL;
    }
    if (engine->light == NULL && create)
    {
        struct Grapple_EngineLight *light = (struct Grapple_EngineLight *)SDL_calloc(
            1, sizeof(struct Grapple_EngineLight));
        if (light == NULL)
        {
            return NULL;
        }
        light->preset = GRAPPLE_LIGHT_NONE;
        light->ambient = (SDL_FColor){1.0f, 1.0f, 1.0f, 1.0f};
        light->hour = HOUR_AFTERNOON;
        light->auto_occluders = true;
        engine->light = light;
    }
    return engine->light;
}

/* The lighting scene is created on first use and needs a renderer, so a
   headless engine or one that never lights anything pays nothing. */
static Grapple_LightScene *Scene(Grapple_Engine *engine,
                                   struct Grapple_EngineLight *light)
{
    if (light->scene == NULL && engine->renderer != NULL)
    {
        light->scene = Grapple_CreateLightScene(engine->renderer);
    }
    return light->scene;
}

/* --- presets ------------------------------------------------------------- */

static SDL_FColor PresetAmbient(Grapple_LightPreset preset, float *hour)
{
    switch (preset)
    {
    case GRAPPLE_LIGHT_SUNRISE:
        *hour = HOUR_SUNRISE;
        return Grapple_DayNightAmbient(HOUR_SUNRISE);
    case GRAPPLE_LIGHT_AFTERNOON:
        *hour = HOUR_AFTERNOON;
        return Grapple_DayNightAmbient(HOUR_AFTERNOON);
    case GRAPPLE_LIGHT_SUNSET:
        *hour = HOUR_SUNSET;
        return Grapple_DayNightAmbient(HOUR_SUNSET);
    case GRAPPLE_LIGHT_NIGHT:
        *hour = HOUR_NIGHT;
        return Grapple_DayNightAmbient(HOUR_NIGHT);
    case GRAPPLE_LIGHT_DARK:
        /* Not from the day/night curve: a cave is dark at noon. Not fully
           black either, or a player with no torch sees literally nothing
           and cannot tell the game from a crash. */
        *hour = HOUR_AFTERNOON;
        return (SDL_FColor){0.04f, 0.04f, 0.06f, 1.0f};
    case GRAPPLE_LIGHT_NONE:
    default:
        *hour = HOUR_AFTERNOON;
        return (SDL_FColor){1.0f, 1.0f, 1.0f, 1.0f};
    }
}

void Grapple_LightSetPreset(Grapple_Engine *engine, Grapple_LightPreset preset)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    light->preset = preset;
    light->ambient = PresetAmbient(preset, &light->hour);
    light->custom_ambient = false;
}

Grapple_LightPreset Grapple_LightPreset_(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    return (light != NULL) ? light->preset : GRAPPLE_LIGHT_NONE;
}

void Grapple_LightSetAmbient(Grapple_Engine *engine, SDL_FColor ambient)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    light->ambient = ambient;
    /* A game that has said what colour it wants must not have it quietly
       overwritten by the clock a frame later. */
    light->custom_ambient = true;
    if (light->preset == GRAPPLE_LIGHT_NONE)
    {
        light->preset = GRAPPLE_LIGHT_AFTERNOON; /* lighting is now on */
    }
}

SDL_FColor Grapple_LightAmbient(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    const SDL_FColor white = {1.0f, 1.0f, 1.0f, 1.0f};
    return (light != NULL) ? light->ambient : white;
}

void Grapple_LightSetClock(Grapple_Engine *engine, float hours, float hours_per_second)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    light->hour = SDL_fmodf((hours < 0.0f) ? 0.0f : hours, 24.0f);
    light->hours_per_second = hours_per_second;
    if (!light->custom_ambient)
    {
        light->ambient = Grapple_DayNightAmbient(light->hour);
        if (light->preset == GRAPPLE_LIGHT_NONE)
        {
            light->preset = GRAPPLE_LIGHT_AFTERNOON;
        }
    }
}

float Grapple_LightHour(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    return (light != NULL) ? light->hour : 0.0f;
}

float Grapple_LightSunlight(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    return (light != NULL) ? Grapple_DayNightSunlight(light->hour) : 1.0f;
}

/* Advance the clock. Driven from the per-frame update, because a day/night
   cycle is scenery: it should follow the display's rate, not the
   simulation's. */
void Grapple_EngineLightUpdate(Grapple_Engine *engine, float dt)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    if (light == NULL || light->hours_per_second == 0.0f)
    {
        return;
    }
    light->hour = SDL_fmodf(light->hour + light->hours_per_second * dt, 24.0f);
    if (light->hour < 0.0f)
    {
        light->hour += 24.0f;
    }
    if (!light->custom_ambient)
    {
        light->ambient = Grapple_DayNightAmbient(light->hour);
    }
}

/* --- lights on actors ---------------------------------------------------- */

Grapple_LightDef Grapple_LightDefault(void)
{
    Grapple_LightDef def;
    SDL_zero(def);
    /* A zeroed struct would be a black light with no radius — which is to
       say, nothing at all. */
    def.radius = 200.0f;
    def.color = (SDL_FColor){1.0f, 0.95f, 0.85f, 1.0f};
    def.falloff = 1.0f;
    def.enabled = true;
    return def;
}

bool Grapple_ActorAddLight(Grapple_Actor *actor, const Grapple_LightDef *light)
{
    if (actor == NULL || light == NULL)
    {
        SDL_InvalidParamError("actor/light");
        return false;
    }
    Grapple_LightDef *slot = Grapple_ActorLightSlot(actor, true);
    if (slot == NULL)
    {
        return false;
    }
    *slot = *light;
    return true;
}

Grapple_LightDef *Grapple_ActorLight(Grapple_Actor *actor)
{
    return Grapple_ActorLightSlot(actor, false);
}

void Grapple_ActorRemoveLight(Grapple_Actor *actor)
{
    Grapple_ActorLightRemove(actor);
}

/* --- occluders ----------------------------------------------------------- */

void Grapple_LightAddOccluder(Grapple_Engine *engine, SDL_FRect wall)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    if (light->occluder_count >= light->occluder_capacity)
    {
        const int next = (light->occluder_capacity > 0) ? light->occluder_capacity * 2 : 64;
        SDL_FRect *grown =
            (SDL_FRect *)SDL_realloc(light->occluders, (size_t)next * sizeof(SDL_FRect));
        if (grown == NULL)
        {
            return;
        }
        light->occluders = grown;
        light->occluder_capacity = next;
    }
    light->occluders[light->occluder_count++] = wall;
}

void Grapple_LightAddOccluderLine(Grapple_Engine *engine, float x1, float y1, float x2,
                                    float y2)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    if (light->segment_count >= light->segment_capacity)
    {
        const int next = (light->segment_capacity > 0) ? light->segment_capacity * 2 : 64;
        void *grown = SDL_realloc(light->segments, (size_t)next * sizeof(*light->segments));
        if (grown == NULL)
        {
            return;
        }
        light->segments = grown;
        light->segment_capacity = next;
    }
    light->segments[light->segment_count].x1 = x1;
    light->segments[light->segment_count].y1 = y1;
    light->segments[light->segment_count].x2 = x2;
    light->segments[light->segment_count].y2 = y2;
    light->segment_count++;
}

void Grapple_LightSetAutoOccluders(Grapple_Engine *engine, bool enabled)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light != NULL)
    {
        light->auto_occluders = enabled;
    }
}

void Grapple_LightAddDarkZone(Grapple_Engine *engine, SDL_FRect area, SDL_FColor ambient)
{
    struct Grapple_EngineLight *light = Light(engine, true);
    if (light == NULL)
    {
        return;
    }
    if (light->dark_count >= light->dark_capacity)
    {
        const int next = (light->dark_capacity > 0) ? light->dark_capacity * 2 : 16;
        void *grown = SDL_realloc(light->dark_zones, (size_t)next * sizeof(*light->dark_zones));
        if (grown == NULL)
        {
            return;
        }
        light->dark_zones = grown;
        light->dark_capacity = next;
    }
    light->dark_zones[light->dark_count].area = area;
    light->dark_zones[light->dark_count].ambient = ambient;
    light->dark_count++;
}

/* --- rendering ----------------------------------------------------------- */

struct SubmitContext
{
    Grapple_Engine *engine;
    struct Grapple_EngineLight *light;
    const Grapple_Camera *camera;
    float alpha;
    int budget;   /* how many more lights may be submitted */
    bool shadows; /* whether any light casts one */
};

static bool SubmitActorLight(Grapple_Actor *actor, void *user)
{
    struct SubmitContext *ctx = (struct SubmitContext *)user;
    const Grapple_LightDef *def = Grapple_ActorLightSlot(actor, false);
    if (def == NULL || !def->enabled || def->radius <= 0.0f)
    {
        return true;
    }
    if (ctx->budget <= 0)
    {
        /* The quality budget is spent. Stopping rather than thinning is
           deliberate: a light that flickers in and out as the count drifts
           across the limit is far more distracting than one that is
           consistently absent. */
        return false;
    }

    /* Interpolated, like the sprite: a light submitted at the simulation
       position lags its own flame by up to a tick, which shimmers. */
    const Grapple_ActorTransform transform =
        Grapple_ActorRenderTransform(actor, ctx->alpha);

    /* The offset rides the actor's rotation, so a torch at the end of an
       arm sweeps as the character turns. */
    const float radians = transform.rotation * (float)(SDL_PI_D / 180.0);
    const float c = SDL_cosf(radians);
    const float s = SDL_sinf(radians);

    Grapple_Light light;
    SDL_zero(light);
    light.x = transform.x + (def->offset_x * c - def->offset_y * s);
    light.y = transform.y + (def->offset_x * s + def->offset_y * c);
    light.radius = def->radius;
    light.color = def->color;
    light.falloff = def->falloff;
    light.flicker = def->flicker;
    /* A stable per-actor seed, so two torches flicker differently and each
       one flickers the same way from frame to frame. */
    light.seed = (Uint32)Grapple_ActorGetId(actor);
    light.no_shadows = def->no_shadows || !ctx->shadows;
    if (def->cone_width > 0.0f && def->cone_width < 360.0f)
    {
        light.angle = (transform.rotation + def->cone_direction) * (float)(SDL_PI_D / 180.0);
        light.spread = def->cone_width * (float)(SDL_PI_D / 180.0);
    }

    if (Grapple_AddLight(ctx->light->scene, &light))
    {
        ctx->light->submitted++;
        ctx->budget--;
    }
    return true;
}

/* Static physics bodies double as walls, because the level's collision is
   usually exactly what should block light. */
static bool SubmitAutoOccluder(Grapple_Actor *actor, void *user)
{
    struct SubmitContext *ctx = (struct SubmitContext *)user;
    if (!Grapple_ActorHasBody(actor))
    {
        return true;
    }
    SDL_FRect bounds;
    if (!Grapple_ActorBodyBounds(actor, &bounds))
    {
        return true;
    }
    /* Only what is near the view: an occluder off screen cannot cast a
       shadow onto it, and the mask has a finite resolution to spend. */
    if (ctx->camera != NULL && !Grapple_CameraVisible(ctx->camera, bounds))
    {
        return true;
    }
    Grapple_AddOccluderRect(ctx->light->scene, bounds);
    return true;
}

bool Grapple_LightRender(Grapple_Engine *engine, const Grapple_Camera *camera,
                           float alpha)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    if (light == NULL || engine->renderer == NULL)
    {
        return false;
    }
    light->submitted = 0;

    const Grapple_GraphicsSettings *graphics = Grapple_EngineGraphics(engine);
    const Grapple_GraphicsQuality quality =
        (graphics != NULL) ? graphics->dynamic_lights : GRAPPLE_QUALITY_HIGH;
    const Grapple_GraphicsQuality shadow_quality =
        (graphics != NULL) ? graphics->shadows : GRAPPLE_QUALITY_HIGH;

    /* Nothing to do, and nothing rendered: a preset of NONE or a player who
       turned dynamic lights off costs exactly zero, so a game may call this
       unconditionally. */
    if (light->preset == GRAPPLE_LIGHT_NONE || Scene(engine, light) == NULL)
    {
        /* Nothing rendered, but the buffers still empty: a game that
           submitted walls into a scene with lighting off must not have them
           pile up until it runs out of memory. */
        light->occluder_count = 0;
        light->segment_count = 0;
        light->dark_count = 0;
        return false;
    }

    /* The quality slider decides the map resolution, the ray count and the
       softness — the game does not write a single conditional for it. */
    Grapple_SetLightMapScale(light->scene, Grapple_GraphicsLightMapScale(quality));
    Grapple_SetLightRayCount(light->scene, Grapple_GraphicsShadowRays(shadow_quality));
    Grapple_SetLightShadowSoftness(light->scene,
                                     Grapple_GraphicsShadowSoftness(shadow_quality));

    const SDL_FRect view = (camera != NULL) ? camera->visible : Grapple_EngineViewRect(engine);
    Grapple_LightBeginFrame(light->scene, view.x, view.y);
    Grapple_SetLightAmbient(light->scene, light->ambient);

    struct SubmitContext ctx;
    ctx.engine = engine;
    ctx.light = light;
    ctx.camera = camera;
    ctx.alpha = alpha;
    ctx.budget = Grapple_GraphicsMaxDynamicLights(quality);
    ctx.shadows = shadow_quality > GRAPPLE_QUALITY_OFF;

    if (light->auto_occluders && ctx.shadows)
    {
        Grapple_ActorEach(engine, SubmitAutoOccluder, &ctx);
    }
    if (ctx.budget > 0)
    {
        Grapple_ActorEach(engine, SubmitActorLight, &ctx);
    }

    /* Then whatever the game submitted this frame, and the composite. One
       call does the whole thing, so there is no window in which an
       AddOccluder silently does nothing. */
    for (int i = 0; i < light->dark_count; ++i)
    {
        Grapple_AddDarkZone(light->scene, light->dark_zones[i].area,
                              light->dark_zones[i].ambient);
    }
    if (ctx.shadows)
    {
        for (int i = 0; i < light->occluder_count; ++i)
        {
            Grapple_AddOccluderRect(light->scene, light->occluders[i]);
        }
        for (int i = 0; i < light->segment_count; ++i)
        {
            Grapple_AddOccluderSegment(light->scene, light->segments[i].x1,
                                         light->segments[i].y1, light->segments[i].x2,
                                         light->segments[i].y2);
        }
    }
    light->occluder_count = 0;
    light->segment_count = 0;
    light->dark_count = 0;

    return Grapple_RenderLighting(light->scene);
}

float Grapple_LightAt(Grapple_Engine *engine, float x, float y)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    if (light == NULL || light->scene == NULL)
    {
        return 1.0f; /* unlit worlds are fully lit */
    }
    return Grapple_SampleLight(light->scene, x, y);
}

int Grapple_LightCount(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    return (light != NULL) ? light->submitted : 0;
}

void Grapple_EngineLightDestroy(Grapple_Engine *engine)
{
    struct Grapple_EngineLight *light = Light(engine, false);
    if (light == NULL)
    {
        return;
    }
    if (light->scene != NULL)
    {
        Grapple_DestroyLightScene(light->scene);
    }
    SDL_free(light->occluders);
    SDL_free(light->segments);
    SDL_free(light->dark_zones);
    SDL_free(light);
    engine->light = NULL;
}
