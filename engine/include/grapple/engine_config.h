/**
 * @file engine_config.h
 * @brief Heap-allocated builders for the engine's plain-struct arguments.
 *
 * Original Grapple code (zlib).
 *
 * `Grapple_EngineConfig` and `Grapple_ActorDef` are plain structs a C
 * caller fills in on the stack. That is the right shape for C and the wrong
 * shape for a script, which has no stack to put one on and no way to write
 * a `const char *` or a function pointer into a field.
 *
 * These give the same structs a create/destroy pair and scalar setters:
 *
 *     -- Lua
 *     local cfg = GrappleC.ConfigCreate()
 *     GrappleC.ConfigSetDesignSize(cfg, 1920, 1080)
 *     GrappleC.ConfigSetTitle(cfg, "My Game")
 *     local engine = GrappleC.CreateEngine(cfg)
 *
 * The binding generator already understands that shape — a function pair
 * named create/destroy makes an owned, GC-managed handle, and setters
 * taking scalars bind themselves. So this costs one small C file and no
 * generator changes, and the resulting handles are freed by the script's
 * garbage collector if the author forgets.
 *
 * C code has no reason to use any of this: a struct on the stack is
 * cheaper and clearer. It exists for the languages that cannot do that.
 *
 * ### Strings are copied
 *
 * A `const char *` in the plain struct is borrowed, which is fine when the
 * caller owns a literal for the length of the call. A script's string may
 * be collected the moment the setter returns, so these copy — and the copy
 * lives and dies with the builder.
 */
#ifndef GRAPPLE_ENGINE_CONFIG_H
#define GRAPPLE_ENGINE_CONFIG_H

#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_camera.h>
#include <grapple/engine_graphics.h>
#include <grapple/engine_light.h>
#include <grapple/engine_physics.h>
#include <grapple/engine_render.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- engine config -------------------------------------------------------- */

/** A config on the heap, zeroed — so every field already has the documented
 *  default and a script only sets what it cares about. */
extern Grapple_EngineConfig *Grapple_ConfigCreate(void);

/** Free it. The engine copies what it needs during creation, so the config
 *  may be destroyed immediately afterwards. */
extern void Grapple_ConfigDestroy(Grapple_EngineConfig *config);

extern void Grapple_ConfigSetTitle(Grapple_EngineConfig *config, const char *title);
extern void Grapple_ConfigSetWindowSize(Grapple_EngineConfig *config, int width, int height);
extern void Grapple_ConfigSetDesignSize(Grapple_EngineConfig *config, int width, int height);
extern void Grapple_ConfigSetPresentation(Grapple_EngineConfig *config,
                                            Grapple_EnginePresentation presentation);
extern void Grapple_ConfigSetFullscreen(Grapple_EngineConfig *config, bool fullscreen);
extern void Grapple_ConfigSetVsync(Grapple_EngineConfig *config, bool vsync);
extern void Grapple_ConfigSetMaxFps(Grapple_EngineConfig *config, int max_fps);
extern void Grapple_ConfigSetTickRate(Grapple_EngineConfig *config, int ticks_per_second);
extern void Grapple_ConfigSetBackend(Grapple_EngineConfig *config,
                                       Grapple_EngineBackend backend);
extern void Grapple_ConfigSetResizable(Grapple_EngineConfig *config, bool resizable);
extern void Grapple_ConfigSetHighDpi(Grapple_EngineConfig *config, bool high_dpi);
extern void Grapple_ConfigSetHeadless(Grapple_EngineConfig *config, bool headless);
extern void Grapple_ConfigSetManualClock(Grapple_EngineConfig *config, bool manual);
extern void Grapple_ConfigSetMediaPath(Grapple_EngineConfig *config, const char *path);
extern void Grapple_ConfigSetAutoMount(Grapple_EngineConfig *config, bool enabled);

/** Apply resolved graphics settings. Copied, so the settings may be a
 *  temporary. */
extern void Grapple_ConfigSetGraphics(Grapple_EngineConfig *config,
                                        const Grapple_GraphicsSettings *graphics);

/* --- actor definitions ---------------------------------------------------- */

/** An actor definition on the heap, with the same defaults a zeroed struct
 *  has — except scale, which the engine already treats as 1 when zero. */
extern Grapple_ActorDef *Grapple_ActorDefCreate(void);
extern void Grapple_ActorDefDestroy(Grapple_ActorDef *def);

extern void Grapple_ActorDefSetType(Grapple_ActorDef *def, const char *type);
extern void Grapple_ActorDefSetName(Grapple_ActorDef *def, const char *name);
extern void Grapple_ActorDefSetPosition(Grapple_ActorDef *def, float x, float y);
extern void Grapple_ActorDefSetRotation(Grapple_ActorDef *def, float degrees);
extern void Grapple_ActorDefSetScale(Grapple_ActorDef *def, float x, float y);
extern void Grapple_ActorDefSetTags(Grapple_ActorDef *def, Grapple_ActorTags tags);
extern void Grapple_ActorDefSetParent(Grapple_ActorDef *def, Grapple_ActorId parent);

/** Bytes of per-actor state. A script has no use for raw bytes, so this is
 *  really for a C game building a def dynamically; a script keeps its own
 *  table keyed by actor id instead. */
extern void Grapple_ActorDefSetStateSize(Grapple_ActorDef *def, int size);

/* --- sprites, bodies and lights ------------------------------------------- */

/* Same treatment for the other structs a game fills in by hand. Each starts
   from the module's own default rather than zero, because a zeroed sprite is
   invisible and a zeroed body falls through the world. */

extern Grapple_Sprite *Grapple_SpriteCreate(void);
extern void Grapple_SpriteDestroy(Grapple_Sprite *sprite);
extern void Grapple_SpriteSetTexture(Grapple_Sprite *sprite, SDL_Texture *texture);
extern void Grapple_SpriteSetSize(Grapple_Sprite *sprite, float width, float height);
extern void Grapple_SpriteSetSource(Grapple_Sprite *sprite, float x, float y, float w,
                                      float h);
extern void Grapple_SpriteSetOrigin(Grapple_Sprite *sprite, float x, float y);
extern void Grapple_SpriteSetColor(Grapple_Sprite *sprite, float r, float g, float b,
                                     float a);
extern void Grapple_SpriteSetLayer(Grapple_Sprite *sprite, int layer, float order);
extern void Grapple_SpriteSetSortByY(Grapple_Sprite *sprite, bool enabled);
extern void Grapple_SpriteSetScreenSpace(Grapple_Sprite *sprite, bool enabled);
extern void Grapple_SpriteSetVisible(Grapple_Sprite *sprite, bool visible);

extern Grapple_BodyDef *Grapple_BodyDefCreate(void);
extern void Grapple_BodyDefDestroy(Grapple_BodyDef *def);
extern void Grapple_BodyDefSetType(Grapple_BodyDef *def, Grapple_BodyType type);
extern void Grapple_BodyDefSetShape(Grapple_BodyDef *def, Grapple_ShapeType shape);
extern void Grapple_BodyDefSetSize(Grapple_BodyDef *def, float width, float height);
extern void Grapple_BodyDefSetOffset(Grapple_BodyDef *def, float x, float y);
extern void Grapple_BodyDefSetMaterial(Grapple_BodyDef *def, float density, float friction,
                                         float restitution);
extern void Grapple_BodyDefSetFixedRotation(Grapple_BodyDef *def, bool fixed);
extern void Grapple_BodyDefSetSensor(Grapple_BodyDef *def, bool sensor);
extern void Grapple_BodyDefSetGravityScale(Grapple_BodyDef *def, float scale);
extern void Grapple_BodyDefSetFilter(Grapple_BodyDef *def, Uint32 category,
                                       Uint32 collides_with);
extern void Grapple_BodyDefSetBullet(Grapple_BodyDef *def, bool bullet);

extern Grapple_LightDef *Grapple_LightDefCreate(void);
extern void Grapple_LightDefDestroy(Grapple_LightDef *def);
extern void Grapple_LightDefSetRadius(Grapple_LightDef *def, float radius);
extern void Grapple_LightDefSetColor(Grapple_LightDef *def, float r, float g, float b,
                                       float a);
extern void Grapple_LightDefSetOffset(Grapple_LightDef *def, float x, float y);
extern void Grapple_LightDefSetCone(Grapple_LightDef *def, float direction, float width);
extern void Grapple_LightDefSetFlicker(Grapple_LightDef *def, float flicker);
extern void Grapple_LightDefSetShadows(Grapple_LightDef *def, bool casts_shadows);

/* --- SDL events ------------------------------------------------------------ */

/**
 * An SDL_Event a script can own, and accessors for reading one.
 *
 * `SDL_PollEvent` takes a pointer to an event the caller allocated, which
 * in C is a local. A script has no way to make one, and no way to read the
 * union afterwards — so the whole event loop was bound and uncallable:
 * a script could open a window and draw into it, but never learn that the
 * player had pressed a key or closed it.
 *
 *     -- a game loop owned entirely by the script
 *     local ev = GrappleC.EventCreate()
 *     local running = true
 *     while running do
 *       while SDL.PollEvent(ev) do
 *         local kind = GrappleC.EventType(ev)
 *         if kind == SDL.EVENT_QUIT then running = false end
 *         if kind == SDL.EVENT_KEY_DOWN then
 *           handle(GrappleC.EventKeyScancode(ev))
 *         end
 *       end
 *       SDL.RenderClear(renderer)
 *       -- draw
 *       SDL.RenderPresent(renderer)
 *     end
 *
 * The accessors read the union's common fields. Anything more exotic still
 * needs C, which is the right trade: these are the fields a game loop
 * actually branches on.
 */
extern SDL_Event *Grapple_EventCreate(void);
extern void Grapple_EventDestroy(SDL_Event *event);

/** The event type — compare against SDL's EVENT_* constants, which the
 *  generator already exposes. */
extern Uint32 Grapple_EventType(SDL_Event *event);

/** Set the type, so a script can synthesise an event and push it with
 *  SDL_PushEvent — a custom game event, or a test driving its own input. */
extern void Grapple_EventSetType(SDL_Event *event, Uint32 type);

/** Which window it came from, or 0. */
extern Uint32 Grapple_EventWindowId(SDL_Event *event);

/* Keyboard. */
extern int Grapple_EventKeyScancode(SDL_Event *event);
extern bool Grapple_EventKeyRepeat(SDL_Event *event);
extern Uint16 Grapple_EventKeyModifiers(SDL_Event *event);

/* Mouse. Position and motion are in window coordinates, as SDL reports
   them; a game using the engine's design space converts with
   Grapple_EngineWindowToDesign. */
extern float Grapple_EventMouseX(SDL_Event *event);
extern float Grapple_EventMouseY(SDL_Event *event);
extern float Grapple_EventMouseDeltaX(SDL_Event *event);
extern float Grapple_EventMouseDeltaY(SDL_Event *event);
extern int Grapple_EventMouseButton(SDL_Event *event);
extern float Grapple_EventWheelX(SDL_Event *event);
extern float Grapple_EventWheelY(SDL_Event *event);

/* Gamepads. `which` identifies the controller across every gamepad event. */
extern Sint32 Grapple_EventGamepadWhich(SDL_Event *event);
extern int Grapple_EventGamepadButton(SDL_Event *event);
extern int Grapple_EventGamepadAxis(SDL_Event *event);
extern float Grapple_EventGamepadAxisValue(SDL_Event *event);

/* Touch, normalised 0..1 as SDL reports it. */
extern float Grapple_EventTouchX(SDL_Event *event);
extern float Grapple_EventTouchY(SDL_Event *event);

/* Text input, or "" for any other event. */
extern const char *Grapple_EventText(SDL_Event *event);

/* --- cameras --------------------------------------------------------------- */

/** A camera on the heap. C games keep one in their own struct; a script
 *  needs a handle. */
extern Grapple_Camera *Grapple_CameraCreate(Grapple_Engine *engine);
extern void Grapple_CameraDestroy(Grapple_Camera *camera);
extern void Grapple_CameraSetZoom(Grapple_Camera *camera, float zoom);
extern void Grapple_CameraSetSmoothing(Grapple_Camera *camera, float seconds);
extern void Grapple_CameraSetDeadzone(Grapple_Camera *camera, float width, float height);
extern void Grapple_CameraSetBounds(Grapple_Camera *camera, float x, float y, float w,
                                      float h);
extern void Grapple_CameraSetViewport(Grapple_Camera *camera, float x, float y, float w,
                                        float h);
extern float Grapple_CameraX(Grapple_Camera *camera);
extern float Grapple_CameraY(Grapple_Camera *camera);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_CONFIG_H */
