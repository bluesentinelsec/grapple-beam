/**
 * @file engine_camera.h
 * @brief A 2D camera: scrolling, following, bounds, shake, and split screen.
 *
 * Original Grapple code (zlib).
 *
 * The camera is what maps **world** coordinates — where things are in the
 * level — onto the **design** space the engine presents (see
 * engine_graphics.h for the design-to-pixels half). A game with no camera
 * is saying "the world is exactly one screen", which is fine for a menu and
 * wrong for a level.
 *
 * It is a plain struct, not an opaque handle, because split screen means
 * having several and a game may want them in an array:
 *
 *     Grapple_Camera camera;
 *     Grapple_CameraInit(&camera, engine);
 *     camera.bounds = (SDL_FRect){0, 0, 8000, 2000};   // the level
 *
 *     // each frame
 *     Grapple_CameraFollow(&camera, player.x, player.y);
 *     Grapple_CameraUpdate(&camera, engine, dt);
 *
 *     Grapple_CameraBegin(engine, &camera);
 *     DrawWorld(&camera);                              // world coordinates
 *     Grapple_CameraEnd(engine);
 *
 * Between Begin and End the renderer is clipped to the camera's viewport
 * and scaled by its zoom, so drawing takes world coordinates run through
 * Grapple_CameraPoint / Grapple_CameraRect — a subtraction, because the
 * scale is already the renderer's job.
 *
 * There is no rotation. SDL's renderer has no general transform, so a
 * rotating camera would have to rotate every draw call individually, and a
 * field that only worked for some of them would be worse than not having
 * it.
 */
#ifndef GRAPPLE_ENGINE_CAMERA_H
#define GRAPPLE_ENGINE_CAMERA_H

#include <grapple/engine.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A camera. Initialise with Grapple_CameraInit and then set what you
 * want; the fields are public because a game inspecting or tweening its own
 * camera should not have to go through twenty accessors.
 */
typedef struct Grapple_Camera
{
    /** Where the camera is looking, in world units — the point that ends up
     *  at the centre of the viewport. */
    float x, y;

    /** World units per design unit. 1.0 is one-to-one; 2.0 shows half as
     *  much world, twice as large. Clamped to [0.05, 50] on update. */
    float zoom;

    /* --- following ------------------------------------------------------ */

    /** What the camera is trying to look at. Set with Grapple_CameraFollow
     *  rather than directly, so the deadzone is applied consistently. */
    float target_x, target_y;

    /** Seconds for the camera to close most of the distance to its target.
     *  0 snaps. Around 0.15 is a camera that feels attached to the player;
     *  0.5 is a camera that lags behind, which suits a slower game.
     *
     *  The smoothing is frame-rate independent — it is an exponential decay
     *  evaluated against dt, not a fixed fraction per frame — so a game does
     *  not feel different at 144 Hz than at 60. */
    float smoothing;

    /** A box around the camera centre, in *design* units, inside which the
     *  target may move without the camera following. Zero-sized means the
     *  camera tracks exactly.
     *
     *  This is what stops a platformer's camera twitching every time the
     *  player hops. Width and height are the full size, not the half-size. */
    float deadzone_w, deadzone_h;

    /* --- limits --------------------------------------------------------- */

    /** The camera never shows outside this rectangle, in world units. A
     *  zero-sized rectangle means unbounded.
     *
     *  When the level is smaller than the view on an axis, the camera
     *  centres on it rather than clamping to an edge — otherwise a short
     *  level would sit against the left of the screen. */
    SDL_FRect bounds;

    /* --- shake ---------------------------------------------------------- */

    /** Current shake displacement in world units, and how long is left.
     *  Driven by Grapple_CameraShake; decays on its own. */
    float shake_amount;
    float shake_seconds;
    float shake_remaining;

    /* --- viewport ------------------------------------------------------- */

    /** Where this camera draws, in design coordinates. Set by
     *  Grapple_CameraInit to the whole view, and by
     *  Grapple_CameraSplit to one player's share of it. */
    SDL_FRect viewport;

    /** Filled in by Grapple_CameraUpdate: the world rectangle currently
     *  visible, which is what to cull against. */
    SDL_FRect visible;
} Grapple_Camera;

/** Set up a camera covering the engine's whole view, at zoom 1, looking at
 *  the origin. Everything else is zeroed, which means: no smoothing, no
 *  deadzone, no bounds, no shake. */
extern void Grapple_CameraInit(Grapple_Camera *camera, Grapple_Engine *engine);

/** Point the camera at a world position. Honours the deadzone: the target
 *  only moves the camera once it leaves the box. */
extern void Grapple_CameraFollow(Grapple_Camera *camera, float world_x, float world_y);

/** Snap to a world position immediately, ignoring smoothing — for a scene
 *  change or a teleport, where easing across the level looks like a bug. */
extern void Grapple_CameraSnap(Grapple_Camera *camera, float world_x, float world_y);

/**
 * Advance smoothing, shake and clamping, and recompute the visible rect.
 *
 * Call from the per-frame `update` hook rather than `fixed_update`: camera
 * movement is cosmetic, and it should track the display's rate.
 */
extern void Grapple_CameraUpdate(Grapple_Camera *camera, Grapple_Engine *engine, float dt);

/** Shake the camera by up to `amount` world units for `seconds`, decaying
 *  to nothing. Multiply `amount` by the player's screen-shake setting —
 *  Grapple_EngineGraphics(engine)->screen_shake — before calling. */
extern void Grapple_CameraShake(Grapple_Camera *camera, float amount, float seconds);

/* --- drawing ------------------------------------------------------------- */

/** Clip and scale the renderer to this camera. Everything drawn until
 *  Grapple_CameraEnd is confined to its viewport. */
extern bool Grapple_CameraBegin(Grapple_Engine *engine, const Grapple_Camera *camera);

/** Undo Grapple_CameraBegin: full viewport, no scale. */
extern void Grapple_CameraEnd(Grapple_Engine *engine);

/** World position to the coordinates to draw at, between Begin and End. */
extern void Grapple_CameraPoint(const Grapple_Camera *camera, float world_x, float world_y,
                                  float *out_x, float *out_y);

/** The same for a rectangle — the common case, since most 2D draws are
 *  SDL_RenderTexture into an SDL_FRect. */
extern SDL_FRect Grapple_CameraRect(const Grapple_Camera *camera, SDL_FRect world);

/** Is any of `world` on screen? Cull with this before drawing: a level
 *  wider than the view is the reason the camera exists, and drawing all of
 *  it every frame defeats the point. */
extern bool Grapple_CameraVisible(const Grapple_Camera *camera, SDL_FRect world);

/** Design coordinates — a mouse position, say — back to world coordinates.
 *  Returns false when the point is outside this camera's viewport, which is
 *  how a split-screen game works out whose half was clicked. */
extern bool Grapple_CameraScreenToWorld(const Grapple_Camera *camera, float screen_x,
                                          float screen_y, float *world_x, float *world_y);

/* --- split screen -------------------------------------------------------- */

typedef enum Grapple_SplitMode
{
    /** Stacked: players get full-width bands, one above the other. Suits
     *  side-scrollers, where horizontal room is what the game needs. */
    GRAPPLE_SPLIT_HORIZONTAL = 0,
    /** Side by side: full-height columns. Suits vertical games, and it is
     *  what a racing game wants on an ultrawide. */
    GRAPPLE_SPLIT_VERTICAL,
    /** Quarters. For three players the third gets the whole bottom half,
     *  rather than leaving a dead quadrant. */
    GRAPPLE_SPLIT_GRID
} Grapple_SplitMode;

/** Most cameras a split can produce. */
#define GRAPPLE_SPLIT_MAX 4

/**
 * Lay `count` cameras out over the engine's view.
 *
 * Each camera keeps whatever settings it already has and gets a new
 * viewport, so a game can configure its cameras once and re-split whenever
 * a player joins or leaves. `count` is clamped to [1, 4]; one camera gets
 * the whole view, which means a game can use the same code path for one
 * player and for four.
 *
 * `gap` is the space in design units left between the panes — a few units
 * of black reads as a divider, and without it two views abut and the eye
 * cannot tell where one ends.
 */
extern int Grapple_CameraSplit(Grapple_Engine *engine, Grapple_SplitMode mode, int count,
                                 float gap, Grapple_Camera *cameras);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_CAMERA_H */
