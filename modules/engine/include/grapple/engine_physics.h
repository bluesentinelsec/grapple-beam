/**
 * @file engine_physics.h
 * @brief Physics bodies on actors, stepped with the fixed tick.
 *
 * Original Grapple code (zlib), over the vendored Box2D.
 *
 * Give an actor a body and the engine simulates it, then writes the result
 * back onto the actor's transform — so the sprite the renderer draws is
 * already in the right place, and nothing in the game has to copy positions
 * between two worlds that disagree.
 *
 *     Grapple_BodyDef def = Grapple_BodyDefault();
 *     def.type = GRAPPLE_BODY_DYNAMIC;
 *     def.shape = GRAPPLE_SHAPE_BOX;
 *     def.width = 48.0f;
 *     def.height = 72.0f;
 *     def.offset_y = -36.0f;        // the actor stands at its feet
 *     Grapple_ActorAddBody(actor, &def);
 *
 * ### It steps with the simulation, not with the frame
 *
 * The world advances once per `fixed_update`, with the same step every
 * time. That is not a detail: a solver fed a variable timestep produces
 * different results on different machines, and a game whose physics depend
 * on frame rate cannot be tested, recorded, or played fairly. The fixed
 * tick already exists for exactly this reason, and physics is its most
 * demanding customer.
 *
 * Bodies are stepped **after** the actors' own `fixed_update`, so a game
 * sets a velocity and the solver acts on it in the same step rather than
 * the next one.
 *
 * ### Pixels are not metres
 *
 * Box2D is tuned for a world measured in metres, where a person is about 2
 * units tall; games are written in pixels, where a person is about 100.
 * Feed it pixels and everything is a thousand-tonne skyscraper: contacts
 * jitter, stacks explode, and the tuning constants stop meaning what they
 * were tuned to mean.
 *
 * So the engine converts. `Grapple_PhysicsSetPixelsPerMetre` sets the
 * scale — 64 by default, meaning a 64-pixel-tall crate is a one-metre crate
 * — and everything in this header is in **design units**, the same
 * coordinates the game already draws in. The conversion happens once, at
 * the boundary, and no game code ever sees a metre.
 */
#ifndef GRAPPLE_ENGINE_PHYSICS_H
#define GRAPPLE_ENGINE_PHYSICS_H

#include <grapple/engine_actor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Grapple_BodyType
{
    /** Never moves. Ground, walls, the level. Cheapest by far — a static
     *  body costs nothing to simulate, only to collide against. */
    GRAPPLE_BODY_STATIC = 0,
    /** Moves exactly where you put it, and pushes dynamic bodies out of the
     *  way without being pushed back. Moving platforms, lifts, doors. */
    GRAPPLE_BODY_KINEMATIC,
    /** Simulated: gravity, forces, collisions, the lot. */
    GRAPPLE_BODY_DYNAMIC
} Grapple_BodyType;

typedef enum Grapple_ShapeType
{
    GRAPPLE_SHAPE_BOX = 0,
    GRAPPLE_SHAPE_CIRCLE,
    /** A rectangle with rounded ends. The right shape for a character: a
     *  box catches on every seam between two floor tiles, and a circle
     *  rolls down slopes it should stand on. */
    GRAPPLE_SHAPE_CAPSULE
} Grapple_ShapeType;

/**
 * A body to attach to an actor. Start from Grapple_BodyDefault(): a
 * zeroed struct would have no size and no density, which is a body that
 * falls through the world.
 */
typedef struct Grapple_BodyDef
{
    Grapple_BodyType type;
    Grapple_ShapeType shape;

    /** Size in design units. Boxes use both; circles use `width` as the
     *  diameter; capsules use `width` as the diameter and `height` as the
     *  full height including the round ends. */
    float width, height;

    /** Where the shape sits relative to the actor's position. An actor
     *  drawn with `origin_y = 1` stands at its feet, so its body wants
     *  `offset_y = -height / 2` to be centred on the sprite rather than
     *  half-buried in the floor. */
    float offset_x, offset_y;

    float density;     /**< 1.0 by default */
    float friction;    /**< 0.6 by default; 0 is ice */
    float restitution; /**< bounciness, 0 by default */

    /** Stop the body rotating. Almost every walking character wants this:
     *  a capsule that is free to spin will lie down the first time it
     *  bumps into something. */
    bool fixed_rotation;

    /** A sensor detects overlaps but does not collide — a trigger volume, a
     *  pickup, a damage zone. */
    bool sensor;

    /** Multiplier on gravity for this body: 0 floats, 2 falls hard, -1
     *  falls up. */
    float gravity_scale;

    /** Linear damping, for a body that should slow down on its own. */
    float damping;

    /** What this body *is*, and what it collides with, as bit masks. Zero
     *  means "everything", which is what a game wants until it does not. */
    Uint32 category;
    Uint32 collides_with;

    /** Check for tunnelling. A fast, small body can pass straight through a
     *  thin wall between two steps; this makes the solver sweep it instead.
     *  Costs more, so it is for bullets rather than for crates. */
    bool bullet;
} Grapple_BodyDef;

/** A body with sensible defaults: dynamic, a 32x32 box, density 1,
 *  friction 0.6, gravity scale 1. */
extern Grapple_BodyDef Grapple_BodyDefault(void);

/* --- the world ----------------------------------------------------------- */

/** Gravity in design units per second squared. Positive Y is down, because
 *  that is the direction the screen's Y axis already goes. Default is
 *  (0, 980) — about 1g at the default scale. */
extern void Grapple_PhysicsSetGravity(Grapple_Engine *engine, float x, float y);
extern void Grapple_PhysicsGravity(Grapple_Engine *engine, float *x, float *y);

/** How many design units make a metre. 64 by default. Set it before
 *  creating any bodies: changing it afterwards would rescale the world
 *  under them. */
extern void Grapple_PhysicsSetPixelsPerMetre(Grapple_Engine *engine, float pixels);
extern float Grapple_PhysicsPixelsPerMetre(Grapple_Engine *engine);

/** Solver iterations per step. Four by default; more is steadier for tall
 *  stacks and costs proportionally more. */
extern void Grapple_PhysicsSetSubSteps(Grapple_Engine *engine, int sub_steps);

/** Stop simulating without destroying anything — a pause menu. Bodies keep
 *  their state and carry on when it is switched back. */
extern void Grapple_PhysicsSetPaused(Grapple_Engine *engine, bool paused);
extern bool Grapple_PhysicsPaused(Grapple_Engine *engine);

/** How many bodies exist. */
extern int Grapple_PhysicsBodyCount(Grapple_Engine *engine);

/* --- bodies on actors ---------------------------------------------------- */

/** Give the actor a body, at the actor's current position. Replaces any
 *  body it already had. */
extern bool Grapple_ActorAddBody(Grapple_Actor *actor, const Grapple_BodyDef *def);

/** Remove it. The actor carries on without one. */
extern void Grapple_ActorRemoveBody(Grapple_Actor *actor);

extern bool Grapple_ActorHasBody(Grapple_Actor *actor);

/* --- driving a body ------------------------------------------------------ */

extern void Grapple_ActorSetVelocity(Grapple_Actor *actor, float x, float y);
extern void Grapple_ActorVelocity(Grapple_Actor *actor, float *x, float *y);

/** An instantaneous change in momentum — a jump, a bounce, an explosion.
 *  What you want for anything that happens *now*. */
extern void Grapple_ActorApplyImpulse(Grapple_Actor *actor, float x, float y);

/** A continuous push, applied over the step — wind, thrust, a conveyor. */
extern void Grapple_ActorApplyForce(Grapple_Actor *actor, float x, float y);

extern void Grapple_ActorSetAngularVelocity(Grapple_Actor *actor, float degrees_per_second);
extern float Grapple_ActorAngularVelocity(Grapple_Actor *actor);

/** Move the body, ignoring the simulation — a teleport. The actor's
 *  interpolation is reset with it, so it does not smear across the screen
 *  from where it used to be. */
extern void Grapple_ActorTeleportBody(Grapple_Actor *actor, float x, float y);

/** Wake a sleeping body. Box2D puts bodies that have settled to sleep,
 *  which is most of why a large world stays affordable; a body that must
 *  react to something that did not touch it needs waking first. */
extern void Grapple_ActorWakeBody(Grapple_Actor *actor);

/* --- collisions ---------------------------------------------------------- */

/**
 * Called when two bodies start or stop touching — including a sensor being
 * entered or left, where the sensor is always the first of the two, so a
 * handler can tell which is which.
 *
 * Delivered **after** the step, never during it: the solver is mid-flight
 * while it runs, and destroying an actor from inside it would corrupt the
 * world. Since actor destruction is already deferred to the end of the
 * frame, `Grapple_ActorDestroy` from a collision handler is safe — which
 * is the single most common thing a handler wants to do.
 */
typedef void (*Grapple_CollisionCallback)(Grapple_Engine *engine, Grapple_ActorId a,
                                            Grapple_ActorId b, bool began, void *user);

extern void Grapple_PhysicsSetCollisionCallback(Grapple_Engine *engine,
                                                  Grapple_CollisionCallback callback,
                                                  void *user);

/* --- queries ------------------------------------------------------------- */

typedef struct Grapple_RayHit
{
    bool hit;
    Grapple_ActorId actor;
    float x, y;             /**< where it struck, in design units */
    float normal_x, normal_y;
    float fraction; /**< how far along the ray, 0..1 */
} Grapple_RayHit;

/** The nearest thing along a ray. The bread and butter of a 2D game:
 *  ground checks, line of sight, hitscan weapons. */
extern Grapple_RayHit Grapple_PhysicsRaycast(Grapple_Engine *engine, float x, float y,
                                                 float dx, float dy, Uint32 mask);

/** The body's bounding box in world units, or false if it has none.
 *
 *  Useful for culling and debug draw, and it is what the lighting
 *  integration uses to treat static bodies as walls — a level's collision
 *  is usually exactly what should block light. */
extern bool Grapple_ActorBodyBounds(Grapple_Actor *actor, SDL_FRect *out);

/** Every actor whose body overlaps the rectangle, written into a caller's
 *  array. Returns how many, at most `capacity`. */
extern int Grapple_PhysicsOverlap(Grapple_Engine *engine, SDL_FRect area, Uint32 mask,
                                    Grapple_ActorId *out, int capacity);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_PHYSICS_H */
