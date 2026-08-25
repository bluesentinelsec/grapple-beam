/*
 * engine_internal.h — shared between the engine's translation units.
 *
 * The engine is one library but several subsystems: the loop
 * (grapple_engine.c) and the scene stack (grapple_engine_scene.c) so
 * far. They share the engine object, which lives here rather than in the
 * public header because a game has no business knowing its layout.
 */
#ifndef GRAPPLE_ENGINE_INTERNAL_H
#define GRAPPLE_ENGINE_INTERNAL_H

#include <grapple/engine.h>
#include <grapple/engine_graphics.h>
#include <grapple/engine_media.h>
#include <grapple/engine_input.h>
#include <grapple/engine_light.h>
#include <grapple/engine_render.h>
#include <grapple/engine_scene.h>

#define NS_PER_SECOND 1000000000ull

struct Grapple_PostFX;
struct Grapple_ActorWorld;
struct Grapple_DrawItem;
struct Grapple_Input;
struct Grapple_Physics;
struct Grapple_Assets;
struct Grapple_EngineLight;
struct Grapple_Saves;
struct Grapple_Text;
struct Grapple_ScriptBridge;

struct Grapple_Engine
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *headless_surface; /* owned by us: see DestroyEngine */

    int design_width, design_height;   /* the reference space */
    float view_width, view_height;     /* what is actually visible */
    Grapple_EnginePresentation presentation;
    SDL_FColor clear_color;

    /* Timing. All internal time is nanoseconds; the API speaks seconds. */
    Uint64 step_ns;         /* one simulation step */
    Uint64 accumulator_ns;  /* unsimulated time carried between frames */
    Uint64 last_ns;         /* clock reading at the start of the last frame */
    Uint64 refresh_ns;      /* display period, 0 if unknown */
    Uint64 max_frame_ns;    /* longer than this is a stall */
    Uint64 manual_ns;       /* the injected clock, when manual */
    bool manual_clock;

    int tick_rate;
    int max_steps;
    int max_fps;          /* 0 = follow the display, <0 = no limiter */
    Uint64 frame_start_ns; /* for the limiter */
    Grapple_EngineInterpolation interpolation;
    float time_scale;

    float delta_seconds;
    float alpha;
    int steps_last_frame;
    int overload_frames;
    Uint64 frame_count;

    /* A short moving average, so a debug overlay does not flicker. */
    float fps;
    float fps_accumulator;
    int fps_frames;

    bool running;
    const Grapple_GameHooks *hooks;
    void *user;

    /* Where this frame's events are echoed, for whoever needs all of them
       bracketed — an immediate-mode GUI, typically. See event_sink.h. */
    Grapple_EventSink event_sink;

    /* Drawn last of all, above even post_render: a UI layer that draws
       itself, so attaching one does not cost the game its own hook. */
    void (*overlay)(void *user);
    void *overlay_user;

    /* Allocated on first use, so a game that never touches scenes pays
       nothing for them. */
    struct Grapple_SceneStack *scenes;

    /* Graphics settings, and the offscreen frame the render-scale and
       post-processing paths share. Both want the frame in a texture rather
       than in the backbuffer, so there is only ever one. */
    Grapple_GraphicsSettings graphics;
    SDL_Texture *frame_target;
    int frame_target_w, frame_target_h;
    bool frame_target_active; /* set between Begin and End, so End knows */
    struct Grapple_PostFX *postfx;
    bool postfx_failed; /* tried once, could not: do not try every frame */

    Grapple_MediaSource media_source;
    char media_path[512];

    /* Allocated on first spawn, so a game that never uses actors pays
       nothing for them. */
    struct Grapple_ActorWorld *actors;

    /* The draw list: grown once and reused, because this runs every frame
       and possibly once per camera. */
    struct Grapple_DrawItem *draw_list;
    int draw_capacity;
    Grapple_RenderStats render_stats;

    struct Grapple_Input *input;
    struct Grapple_Physics *physics;
    struct Grapple_Assets *assets;
    struct Grapple_EngineLight *light;
    struct Grapple_Saves *saves;
    struct Grapple_Text *text;
    struct Grapple_ScriptBridge *script;
};

extern void Grapple_EngineSavesDestroy(Grapple_Engine *engine);
extern void Grapple_EngineTextDestroy(Grapple_Engine *engine);

/* --- lighting ------------------------------------------------------------ */

/** Advance the day/night clock. Per-frame, because a cycle is scenery. */
extern void Grapple_EngineLightUpdate(Grapple_Engine *engine, float dt);
extern void Grapple_EngineLightDestroy(Grapple_Engine *engine);

/** The actor's attached light, as with the sprite. */
extern Grapple_LightDef *Grapple_ActorLightSlot(Grapple_Actor *actor, bool create);
extern void Grapple_ActorLightRemove(Grapple_Actor *actor);


/* --- assets -------------------------------------------------------------- */

/** Turn decoded surfaces into textures, within this frame's time budget.
 *  Main thread only: SDL's renderer belongs to the thread that made it. */
extern void Grapple_EngineAssetsPump(Grapple_Engine *engine);
extern void Grapple_EngineAssetsDestroy(Grapple_Engine *engine);

/* --- physics ------------------------------------------------------------- */

/** Advance the world one fixed step and write the results onto actors. */
extern void Grapple_EnginePhysicsStep(Grapple_Engine *engine, float step);
extern void Grapple_EnginePhysicsDestroy(Grapple_Engine *engine);

/** A Box2D body handle, stored on the actor as its three fields so that the
 *  actor system needs no Box2D header. */
extern void Grapple_ActorSetBody(Grapple_Actor *actor, int index, Uint16 world,
                                   Uint16 generation, float offset_x, float offset_y);
extern bool Grapple_ActorGetBody(Grapple_Actor *actor, int *index, Uint16 *world,
                                   Uint16 *generation, float *offset_x, float *offset_y);
extern void Grapple_ActorClearBody(Grapple_Actor *actor);

/* --- input --------------------------------------------------------------- */

extern bool Grapple_EngineInputInit(Grapple_Engine *engine);
extern void Grapple_EngineInputDestroy(Grapple_Engine *engine);

/** Adopt controllers that were already plugged in at startup: they do not
 *  all produce an ADDED event. */
extern void Grapple_EngineInputOpenGamepads(Grapple_Engine *engine);

/** Snapshot last frame's state; edges are the difference. */
extern void Grapple_EngineInputBeginFrame(Grapple_Engine *engine);

/** Fold one SDL event into the state table. */
extern void Grapple_EngineInputEvent(Grapple_Engine *engine, const SDL_Event *event);

/** Sample the things that are polled rather than evented — stick axes —
 *  and advance the menu-repeat clocks. */
extern void Grapple_EngineInputEndFrame(Grapple_Engine *engine, float dt);

/* Small hooks the binding layer uses. */
extern bool Grapple_InputKeyDownRaw(Grapple_Engine *engine, int scancode);
extern int Grapple_InputFirstPressedKey(Grapple_Engine *engine);

/* --- rendering ----------------------------------------------------------- */

/** The actor's sprite storage. `create` gives it one if it has none. */
extern Grapple_Sprite *Grapple_ActorSpriteSlot(Grapple_Actor *actor, bool create);

/** Forget the actor's sprite. */
extern void Grapple_ActorSpriteRemove(Grapple_Actor *actor);

/** Free the draw list with the engine. */
extern void Grapple_RenderDestroy(Grapple_Engine *engine);

/* --- actors, driven by the loop ------------------------------------------ */

/** Snapshot transforms for interpolation, then run one simulation step. */
extern void Grapple_ActorDispatchFixedUpdate(Grapple_Engine *engine, float step);

/** Per-frame cosmetic update. */
extern void Grapple_ActorDispatchUpdate(Grapple_Engine *engine, float dt);

/** Drain the message queue, once, after the updates. */
extern void Grapple_ActorDeliverMessages(Grapple_Engine *engine);

/** Admit spawned actors and free destroyed ones, at the end of the frame. */
extern void Grapple_ActorApplyPending(Grapple_Engine *engine);

/** Free the world with the engine. */
extern void Grapple_ActorWorldDestroy(Grapple_Engine *engine);

/* --- media --------------------------------------------------------------- */

/** Run the mount search and record what it found on the engine. */
extern void Grapple_EngineMountMedia(Grapple_Engine *engine, const char *explicit_path,
                                       int argc, char *const *argv);

/** Point the settings resolver at the mounted archive, so a game's shipped
 *  config.toml is found wherever it lives. */
extern void Grapple_EngineInstallConfigReader(void);

/* --- graphics ------------------------------------------------------------ */

/** Push engine->graphics.filter to the renderer's default scale mode. */
extern void Grapple_EngineApplyFilter(Grapple_Engine *engine);

/** Redirect rendering into the offscreen frame when the settings need one
 *  (render scale below native, or any post-effect enabled). No-op
 *  otherwise, so a game using neither pays nothing. */
extern void Grapple_EngineBeginFrameTarget(Grapple_Engine *engine);

/** Composite the offscreen frame back to the window, then run the
 *  post-processing chain over the result. Always called, because the chain
 *  applies whether or not a target was used. */
extern void Grapple_EngineEndFrameTarget(Grapple_Engine *engine);

/** True when the renderer can run the post-processing shaders (OpenGL or
 *  OpenGL ES) and at least one effect is enabled. */
extern bool Grapple_EnginePostFXAvailable(Grapple_Engine *engine);

/** Drop cached shader state after a settings change. */
extern void Grapple_EnginePostFXInvalidate(Grapple_Engine *engine);

/** Run the effect chain over the frame currently in the framebuffer.
 *  Returns false when the chain could not run, in which case the frame is
 *  simply left as it was drawn. */
extern bool Grapple_EnginePostFXPresent(Grapple_Engine *engine);

extern void Grapple_EnginePostFXDestroy(Grapple_Engine *engine);

extern void Grapple_EngineDestroyFrameTarget(Grapple_Engine *engine);

/* --- scene stack, driven by the loop ------------------------------------ */

/** Dispatch a simulation step to the scene stack. */
extern void Grapple_SceneDispatchFixedUpdate(Grapple_Engine *engine, float step);

/** Dispatch a per-frame update, and advance any transition in progress. */
extern void Grapple_SceneDispatchUpdate(Grapple_Engine *engine, float dt);

/** Draw the stack, bottom-up from the lowest visible scene, then the
 *  transition overlay if one is running. */
extern void Grapple_SceneDispatchRender(Grapple_Engine *engine, float alpha);

/** Give an event to the top scene. */
extern void Grapple_SceneDispatchEvent(Grapple_Engine *engine, const SDL_Event *event);

/** Tear the whole stack down, top first. Called when the engine is
 *  destroyed so scenes always see their exit and unload hooks. */
extern void Grapple_SceneShutdown(Grapple_Engine *engine);

#endif /* GRAPPLE_ENGINE_INTERNAL_H */
