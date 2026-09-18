/**
 * @file engine_light.h
 * @brief Scene lighting: presets, a clock, and lights that ride on actors.
 *
 * Original Grapple code (zlib), over Grapple::Light.
 *
 * The lighting module already knows how to light a scene. What it does not
 * know is where the camera is, which actors moved this frame, or what the
 * player set the quality slider to — and wiring those up by hand, every
 * frame, in every game, is exactly the sort of bookkeeping an engine is
 * for. So:
 *
 *     Grapple_LightSetPreset(engine, GRAPPLE_LIGHT_NIGHT);
 *
 *     Grapple_LightDef torch = Grapple_LightDefault();
 *     torch.radius = 380.0f;
 *     torch.color = (SDL_FColor){1.0f, 0.72f, 0.36f, 1.0f};
 *     torch.flicker = 0.15f;
 *     Grapple_ActorAddLight(actor, &torch);
 *
 *     // in the render hook, after drawing the world
 *     Grapple_LightRender(engine, &camera, alpha);
 *
 * and the light follows the actor for the rest of its life, including
 * through a parent's transform, and disappears when the actor does.
 *
 * ### Lights are drawn interpolated, like everything else
 *
 * An actor's light is submitted at `Grapple_ActorRenderTransform`, the
 * same place its sprite is drawn. If it were submitted at the simulation
 * position instead, a torch would lag its own flame by up to one tick — a
 * shimmer that is maddening to look at and very hard to attribute.
 *
 * ### The quality budget is honoured for you
 *
 * `dynamic_lights` and `shadows` from engine_graphics.h decide the light
 * map resolution, the ray count, the softness, and how many lights are
 * submitted at all. A player who turns lighting off gets ambient only, at
 * no cost, without the game writing a single conditional.
 */
#ifndef GRAPPLE_ENGINE_LIGHT_H
#define GRAPPLE_ENGINE_LIGHT_H

#include <grapple/engine_actor.h>
#include <grapple/engine_camera.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A scene's lighting mood.
 *
 * These are the five you asked for plus an off switch, and they are
 * deliberately coarse: a preset is a starting point a designer picks in
 * five seconds, not a colour grading system. Anything more specific is a
 * custom ambient, which is one call away.
 */
typedef enum Grapple_LightPreset
{
    /** No lighting at all: the world draws at full brightness. The right
     *  choice for a menu, and for a game that does not want lighting —
     *  costs nothing, because nothing is rendered. */
    GRAPPLE_LIGHT_NONE = 0,
    GRAPPLE_LIGHT_SUNRISE,   /**< low warm sun, long shadows */
    GRAPPLE_LIGHT_AFTERNOON, /**< bright and neutral; the safe default */
    GRAPPLE_LIGHT_SUNSET,    /**< orange, dimming */
    GRAPPLE_LIGHT_NIGHT,     /**< deep blue; torches carry the scene */
    /** Near-black. A cave or a dungeon interior, where the only light is
     *  the one the player brought with them. */
    GRAPPLE_LIGHT_DARK
} Grapple_LightPreset;

/** Apply a preset. Sets the ambient colour and the time of day to match,
 *  so a game that later starts the clock carries on from the right hour. */
extern void Grapple_LightSetPreset(Grapple_Engine *engine, Grapple_LightPreset preset);
extern Grapple_LightPreset Grapple_LightPreset_(Grapple_Engine *engine);

/** Override the ambient directly, for a game with its own art direction.
 *  Doing so puts the preset into a custom state, and stops the clock from
 *  overwriting it. */
extern void Grapple_LightSetAmbient(Grapple_Engine *engine, SDL_FColor ambient);
extern SDL_FColor Grapple_LightAmbient(Grapple_Engine *engine);

/* --- the clock ----------------------------------------------------------- */

/**
 * Run a day/night cycle.
 *
 * `hours_per_second` is how fast the clock runs: 0 stops it, and 0.05 gives
 * a twenty-minute day, which is about right for a game where the cycle is
 * scenery rather than a mechanic. The ambient colour follows the hour
 * automatically.
 *
 * Setting a custom ambient stops the clock driving it, because a game that
 * has said what colour it wants should not have it quietly overwritten a
 * frame later.
 */
extern void Grapple_LightSetClock(Grapple_Engine *engine, float hours,
                                    float hours_per_second);

/** The current hour, 0..24. */
extern float Grapple_LightHour(Grapple_Engine *engine);

/** How bright the sun is now, 0 at night to 1 at midday — for the things
 *  that follow the sun without being the ambient: whether the streetlights
 *  are on, whether the torches should be lit. */
extern float Grapple_LightSunlight(Grapple_Engine *engine);

/* --- lights on actors ---------------------------------------------------- */

/**
 * A light attached to an actor. Copied on attach.
 *
 * Position comes from the actor, so there is nothing to keep in sync —
 * which is the entire reason this lives on an actor rather than being
 * submitted by hand.
 */
typedef struct Grapple_LightDef
{
    float radius; /**< in design units; 200 if left at zero */
    SDL_FColor color;
    /** Offset from the actor's position, so a torch can be at the end of an
     *  arm rather than in the middle of a chest. */
    float offset_x, offset_y;

    float falloff; /**< 0 or 1 linear, 2 quadratic — higher is tighter */
    /** Cone direction in degrees and width in degrees. A width of 0 or 360
     *  is omnidirectional. The cone rotates with the actor, so a torch held
     *  by a turning character sweeps the room. */
    float cone_direction, cone_width;
    float flicker; /**< 0 steady, 1 wild; a torch wants about 0.15 */
    bool no_shadows;
    bool enabled;
} Grapple_LightDef;

/** A light with usable defaults: warm white, 200 units, steady, casting
 *  shadows. A zeroed struct would be a black light with no radius. */
extern Grapple_LightDef Grapple_LightDefault(void);

/** Attach a light to an actor, replacing any it had. */
extern bool Grapple_ActorAddLight(Grapple_Actor *actor, const Grapple_LightDef *light);

/** The actor's light, to read or change in place — for a torch that dims
 *  as it burns down. NULL if it has none. */
extern Grapple_LightDef *Grapple_ActorLight(Grapple_Actor *actor);

extern void Grapple_ActorRemoveLight(Grapple_Actor *actor);

/* --- occluders ----------------------------------------------------------- */

/**
 * Walls, for this frame.
 *
 * Occluders are submitted per frame rather than kept, because the set that
 * matters is the set near the camera, and a level's geometry is the game's
 * to iterate. Call these any time during the frame, before
 * Grapple_LightRender — they are buffered until then, so there is no
 * window in which one of these silently does nothing.
 *
 * Actors with a **static physics body** are submitted for you when
 * `Grapple_LightSetAutoOccluders` is on, which covers the common case of
 * "the level's collision is also what blocks light".
 */
extern void Grapple_LightAddOccluder(Grapple_Engine *engine, SDL_FRect wall);
extern void Grapple_LightAddOccluderLine(Grapple_Engine *engine, float x1, float y1,
                                           float x2, float y2);

/** Submit every static physics body as an occluder. On by default: the
 *  level's collision is usually exactly what should block light, and a
 *  game that disagrees can turn it off and submit its own. */
extern void Grapple_LightSetAutoOccluders(Grapple_Engine *engine, bool enabled);

/** A rectangle that is darker than the ambient — the inside of a building
 *  seen from outside, a cave mouth. Submitted per frame like occluders. */
extern void Grapple_LightAddDarkZone(Grapple_Engine *engine, SDL_FRect area,
                                       SDL_FColor ambient);

/* --- drawing ------------------------------------------------------------- */

/**
 * Composite the lighting over the frame.
 *
 * Call **after** drawing the world and **before** the HUD: lighting
 * multiplies over what is already there, so anything drawn afterwards is
 * unlit — which is what a HUD wants and what the world does not.
 *
 * Does nothing when the preset is NONE or the player has turned dynamic
 * lights off, so a game may call it unconditionally.
 *
 * Returns false if lighting could not be rendered, which a game may ignore.
 */
extern bool Grapple_LightRender(Grapple_Engine *engine, const Grapple_Camera *camera,
                                  float alpha);

/** How much light reaches a world point, 0..1 — for gameplay that depends
 *  on it: whether a guard can see the player, whether a plant grows. Reads
 *  the map rendered last frame, so it is cheap. */
extern float Grapple_LightAt(Grapple_Engine *engine, float x, float y);

/** How many lights were submitted last frame, after the quality budget was
 *  applied. For a debug overlay, and for noticing that a budget is
 *  silently dropping half the scene. */
extern int Grapple_LightCount(Grapple_Engine *engine);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_LIGHT_H */
