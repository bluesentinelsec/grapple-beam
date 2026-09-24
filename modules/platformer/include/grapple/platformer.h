/**
 * @file platformer.h
 * @brief Grapple::Platformer — an opinionated 2D platformer layer over the
 *        engine: a tile grid with dedicated level objects, a tuned player
 *        character, an action map with sensible defaults, and a scrolling
 *        camera. Original grapple-beam code (zlib).
 *
 * The games this is modelled on — Super Mario Bros., Super Mario World,
 * Donkey Kong Country — predate rigid-body physics engines, and their feel
 * comes from the model they used instead: an axis-aligned box swept against
 * a tile grid, one axis at a time, with hand-tuned acceleration and a jump
 * whose height depends on how long the button is held. That is what this
 * module implements. It does not use Box2D.
 *
 * A level is built either from dedicated objects,
 *
 *     Grapple_Platformer *level = Grapple_CreatePlatformer(engine, 212, 15, 16);
 *     Grapple_PlatformerCreateFloor(level, 0, 13, 69, 0);
 *     Grapple_PlatformerCreateWall(level, 28, 9, 4);
 *     Grapple_PlatformerCreateBlock(level, 20, 9, 5, 1);
 *     Grapple_PlatformerCreatePlayer(level, 3, 12);
 *     Grapple_PlatformerAttach(level);      // the engine now runs it
 *
 * or from a Tiled map: `Grapple_LoadPlatformer(engine, "levels/1-1.tmj")`.
 *
 * Every coordinate a *level* takes is in **tiles**, with (0, 0) the top-left
 * cell, because that is how a level is designed. Every coordinate the
 * *player* reports is in **pixels** (design units), because that is how the
 * rest of the engine draws. `Grapple_PlatformerTileSize` converts.
 *
 * Attaching pushes a scene that steps the player, scrolls the camera and
 * draws the level, so a game that only wants to play has no loop to write.
 * A game with its own scenes calls Step/Update/Render itself instead.
 */
#ifndef GRAPPLE_PLATFORMER_H
#define GRAPPLE_PLATFORMER_H

#include <SDL3/SDL.h>
#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_binding.h>
#include <grapple/engine_camera.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Grapple_Platformer Grapple_Platformer;

    /** What a grid cell is. */
    typedef enum Grapple_PlatformerTile
    {
        GRAPPLE_PLATFORMER_EMPTY = 0,
        GRAPPLE_PLATFORMER_SOLID,   /**< blocks movement from every side */
        GRAPPLE_PLATFORMER_ONE_WAY, /**< a ledge: solid from above only */
        GRAPPLE_PLATFORMER_TILE_COUNT
    } Grapple_PlatformerTile;

    /** How the camera follows the player. */
    typedef enum Grapple_PlatformerScroll
    {
        GRAPPLE_PLATFORMER_SCROLL_FREE = 0, /**< follows in every direction */
        GRAPPLE_PLATFORMER_SCROLL_FORWARD,  /**< never scrolls back left (SMB 1) */
    } Grapple_PlatformerScroll;

    /** The player's state machine. `Grapple_PlatformerPlayerStateName` spells
     *  each one as the lower-case word — "idle", "walk" — which is what a
     *  script compares against and what an animation clip is named after. */
    typedef enum Grapple_PlayerState
    {
        GRAPPLE_PLAYER_IDLE = 0,
        GRAPPLE_PLAYER_WALK,
        GRAPPLE_PLAYER_RUN,
        GRAPPLE_PLAYER_JUMP,
        GRAPPLE_PLAYER_FALL,
        GRAPPLE_PLAYER_PAUSED,
        GRAPPLE_PLAYER_STATE_COUNT
    } Grapple_PlayerState;

    /* --- the level ---------------------------------------------------------- */

    /** An empty level `width` x `height` tiles of `tile_size` pixels each.
     *  NULL + SDL_GetError on a bad size. Destroy with
     *  Grapple_DestroyPlatformer, after the engine has stopped running it. */
    extern Grapple_Platformer *Grapple_CreatePlatformer(Grapple_Engine *engine, int width,
                                                        int height, int tile_size);

    /** A level from a Tiled `.tmj` map, loaded VFS-first like every asset.
     *
     *  Tile layers named "solid", "ground", "collision" or "walls", or with a
     *  boolean `solid` property, become solid cells wherever they hold a
     *  tile; a tileset tile with a `one_way` property becomes a ledge and one
     *  with `solid = false` is skipped. Objects in any object layer whose
     *  class (Tiled's "type") is "floor", "wall", "block" or "platform"
     *  fill their rectangle; an object of class "player" places the player.
     *  A map with no player object leaves the level without one. */
    extern Grapple_Platformer *Grapple_LoadPlatformer(Grapple_Engine *engine, const char *path);

    extern void Grapple_DestroyPlatformer(Grapple_Platformer *level);

    extern Grapple_Engine *Grapple_PlatformerEngine(Grapple_Platformer *level);
    extern int Grapple_PlatformerWidth(Grapple_Platformer *level);  /**< tiles */
    extern int Grapple_PlatformerHeight(Grapple_Platformer *level); /**< tiles */
    extern int Grapple_PlatformerTileSize(Grapple_Platformer *level);
    /** The level in pixels: width * tile_size, height * tile_size. */
    extern void Grapple_PlatformerPixelSize(Grapple_Platformer *level, float *width, float *height);

    /* --- cells -------------------------------------------------------------- */

    /** False, with SDL_GetError, outside the grid. */
    extern bool Grapple_PlatformerSetTile(Grapple_Platformer *level, int x, int y,
                                          Grapple_PlatformerTile tile);
    /** Outside the grid: the left and right edges read as solid, so the
     *  player cannot walk out of the level; above and below read as empty. */
    extern Grapple_PlatformerTile Grapple_PlatformerTileAt(Grapple_Platformer *level, int x, int y);
    /** Set every cell in a rectangle; clipped to the grid. Returns how many
     *  cells were set. */
    extern int Grapple_PlatformerFillTiles(Grapple_Platformer *level, int x, int y, int width,
                                           int height, Grapple_PlatformerTile tile);

    /* --- dedicated objects ---------------------------------------------------
     *
     * The things almost every platformer level is made of, named so a level
     * reads as one. (x, y) is always the top-left cell of the object. Each is
     * sugar over FillTiles and returns false only when nothing was inside the
     * grid. */

    /** Ground from row `y` down: `width` tiles wide and `depth` tiles deep,
     *  or all the way to the bottom of the level when depth is zero. */
    extern bool Grapple_PlatformerCreateFloor(Grapple_Platformer *level, int x, int y, int width,
                                              int depth);
    /** A one-tile-wide column `height` tiles tall, growing down from row y. */
    extern bool Grapple_PlatformerCreateWall(Grapple_Platformer *level, int x, int y, int height);
    /** A solid rectangle. A single block is width 1, height 1. */
    extern bool Grapple_PlatformerCreateBlock(Grapple_Platformer *level, int x, int y, int width,
                                              int height);
    /** A staircase of `steps` columns standing on row `y`: (x, y) is the
     *  bottom cell of the first column. Rising, column i is i + 1 tiles tall,
     *  so the first column is a single tile and the last the tallest;
     *  falling, column i is steps - i tall, so the tallest column is at x. */
    extern bool Grapple_PlatformerCreateStairs(Grapple_Platformer *level, int x, int y, int steps,
                                               bool rising);
    /** A one-way ledge `width` tiles wide: landed on from above, passed
     *  through from below and from the sides. */
    extern bool Grapple_PlatformerCreatePlatform(Grapple_Platformer *level, int x, int y,
                                                 int width);

    /** A generic solid rectangle in **pixels**, for anything the grid cannot
     *  express — a thin ledge, a moving platform to be. Returns an id for
     *  RemoveSolid, or -1. */
    extern int Grapple_PlatformerAddSolid(Grapple_Platformer *level, float x, float y, float w,
                                          float h);
    extern bool Grapple_PlatformerRemoveSolid(Grapple_Platformer *level, int id);
    extern int Grapple_PlatformerSolidCount(Grapple_Platformer *level);

    /* --- look ----------------------------------------------------------------
     *
     * Until a game has art, cells draw as filled rectangles in these colours
     * and the player as one in its own. Give a tile kind a texture and every
     * cell of that kind draws it instead; give the player's actor a sprite
     * (Grapple_ActorSprite) and the rectangle is gone. */

    extern void Grapple_PlatformerSetBackgroundColor(Grapple_Platformer *level, float r, float g,
                                                     float b, float a);
    extern void Grapple_PlatformerSetTileColor(Grapple_Platformer *level,
                                               Grapple_PlatformerTile tile, float r, float g,
                                               float b, float a);
    /** Borrowed: the level draws it but does not own it. NULL restores the
     *  coloured rectangle. */
    extern void Grapple_PlatformerSetTileTexture(Grapple_Platformer *level,
                                                 Grapple_PlatformerTile tile, SDL_Texture *texture);

    /* --- the camera --------------------------------------------------------- */

    extern void Grapple_PlatformerSetScroll(Grapple_Platformer *level,
                                            Grapple_PlatformerScroll mode);
    extern Grapple_PlatformerScroll Grapple_PlatformerScrollMode(Grapple_Platformer *level);
    /** Seconds for the camera to catch the player up; 0 locks it on. */
    extern void Grapple_PlatformerSetCameraSmoothing(Grapple_Platformer *level, float seconds);
    /** The world point at the centre of the view. */
    extern void Grapple_PlatformerCameraPosition(Grapple_Platformer *level, float *x, float *y);
    /** The camera itself, for a game that wants to draw through it. Borrowed. */
    extern Grapple_Camera *Grapple_PlatformerCamera(Grapple_Platformer *level);

    /* --- the loop ------------------------------------------------------------
     *
     * Attach pushes a scene onto the engine's stack that runs the level:
     * the player steps in the fixed tick, the camera follows in the per-frame
     * update, and the render draws cells then actors through the camera. The
     * push lands at the end of the current frame, like every scene change.
     *
     * A game with scenes of its own calls the three below from them instead. */

    extern bool Grapple_PlatformerAttach(Grapple_Platformer *level);
    extern void Grapple_PlatformerDetach(Grapple_Platformer *level);
    extern bool Grapple_PlatformerAttached(Grapple_Platformer *level);

    extern void Grapple_PlatformerStep(Grapple_Platformer *level, float step);
    extern void Grapple_PlatformerUpdate(Grapple_Platformer *level, float dt);
    extern void Grapple_PlatformerRender(Grapple_Platformer *level, float alpha);

    /* --- the player ----------------------------------------------------------
     *
     * One per level. It is an ordinary engine actor of type "player" — give it
     * a sprite, parent things to it, query it by type — whose movement the
     * level owns. Tuning defaults are Super Mario Bros. converted to pixels
     * per second at the level's tile size, so a fresh level already feels
     * right; every number is a key for SetPlayerTuning:
     *
     *   walk_speed run_speed accel decel skid_decel air_accel air_decel
     *   jump_speed jump_height run_jump_bonus jump_gravity fall_gravity
     *   max_fall coyote_time jump_buffer
     *
     * Speeds are pixels per second, accelerations pixels per second squared,
     * times seconds. jump_height is jump_speed spelled as the tiles-tall
     * standing jump it produces under jump_gravity, in pixels; setting either
     * updates the other. */

    /** Spawn the player standing on top of cell (tile_x, tile_y + 1) — that
     *  is, its feet at the bottom of cell (tile_x, tile_y). Replaces an
     *  existing player. Returns the actor, or GRAPPLE_ACTOR_NONE. */
    extern Grapple_ActorId Grapple_PlatformerCreatePlayer(Grapple_Platformer *level, int tile_x,
                                                          int tile_y);
    extern Grapple_ActorId Grapple_PlatformerPlayer(Grapple_Platformer *level);
    extern bool Grapple_PlatformerHasPlayer(Grapple_Platformer *level);
    extern void Grapple_PlatformerRemovePlayer(Grapple_Platformer *level);

    /** The collision box, in pixels, anchored at the feet. The placeholder
     *  rectangle is drawn at the same size. */
    extern void Grapple_PlatformerSetPlayerSize(Grapple_Platformer *level, float width,
                                                float height);
    extern void Grapple_PlatformerPlayerSize(Grapple_Platformer *level, float *width,
                                             float *height);
    extern void Grapple_PlatformerSetPlayerColor(Grapple_Platformer *level, float r, float g,
                                                 float b, float a);

    /** False, with SDL_GetError naming the key, for a key that does not exist. */
    extern bool Grapple_PlatformerSetPlayerTuning(Grapple_Platformer *level, const char *key,
                                                  float value);
    extern float Grapple_PlatformerPlayerTuning(Grapple_Platformer *level, const char *key);
    /** How many keys there are, and each one's name, for a tuning screen. */
    extern int Grapple_PlatformerPlayerTuningCount(void);
    extern const char *Grapple_PlatformerPlayerTuningKey(int index);

    extern Grapple_PlayerState Grapple_PlatformerPlayerState(Grapple_Platformer *level);
    extern const char *Grapple_PlatformerPlayerStateName(Grapple_Platformer *level);
    extern const char *Grapple_PlayerStateName(Grapple_PlayerState state);

    /* Frame events. Each is true for the whole of the frame in which it
       happened, whichever hook asks, the way KeyPressed is. */
    extern bool Grapple_PlatformerPlayerStateChanged(Grapple_Platformer *level);
    extern bool Grapple_PlatformerPlayerLanded(Grapple_Platformer *level);
    extern bool Grapple_PlatformerPlayerJumped(Grapple_Platformer *level);
    /** Hit a ceiling from below this frame — where a block bump will go. */
    extern bool Grapple_PlatformerPlayerBumped(Grapple_Platformer *level);
    /** Fell out of the bottom of the level this frame. The player is put
     *  back where it spawned in the same step; a game that wants a death
     *  instead pauses the player here. */
    extern bool Grapple_PlatformerPlayerFell(Grapple_Platformer *level);

    extern bool Grapple_PlatformerPlayerGrounded(Grapple_Platformer *level);
    /** -1 facing left, +1 facing right. */
    extern int Grapple_PlatformerPlayerFacing(Grapple_Platformer *level);
    /** The feet, in pixels. */
    extern void Grapple_PlatformerPlayerPosition(Grapple_Platformer *level, float *x, float *y);
    extern void Grapple_PlatformerPlayerVelocity(Grapple_Platformer *level, float *vx, float *vy);

    /** Put the player at a point (feet, pixels) with no velocity — a teleport,
     *  so the sprite does not smear. */
    extern void Grapple_PlatformerPlayerRespawn(Grapple_Platformer *level, float x, float y);
    /** Back where CreatePlayer put it. */
    extern void Grapple_PlatformerPlayerRespawnAtStart(Grapple_Platformer *level);

    /** A paused player ignores input and does not move or fall; the state
     *  reads "paused" until resumed. Independent of the engine's time scale,
     *  so a dialogue box can freeze the player over a live world. */
    extern void Grapple_PlatformerSetPlayerPaused(Grapple_Platformer *level, bool paused);
    extern bool Grapple_PlatformerPlayerPaused(Grapple_Platformer *level);

    /* --- input ---------------------------------------------------------------
     *
     * The level owns an action map with the actions a platformer needs bound
     * to a keyboard and a modern Xbox-layout pad:
     *
     *   move_x   A/D, Left/Right, left stick, d-pad       (-1..1)
     *   jump     Space, Z, pad A
     *   run      Left Shift, X, pad X, pad right trigger
     *
     * Rebind through the map (Grapple_PlatformerActions) or the text form
     * ("pad:b", "space", "-a"), and add the game's own actions to the same
     * map so one bindings.toml covers everything. */

    extern Grapple_ActionMap *Grapple_PlatformerActions(Grapple_Platformer *level);
    /** Add a binding in the text form Grapple_BindingToString writes. */
    extern bool Grapple_PlatformerBind(Grapple_Platformer *level, const char *action,
                                       const char *binding);
    extern bool Grapple_PlatformerActionDown(Grapple_Platformer *level, const char *action);
    extern bool Grapple_PlatformerActionPressed(Grapple_Platformer *level, const char *action);
    extern bool Grapple_PlatformerActionReleased(Grapple_Platformer *level, const char *action);
    extern float Grapple_PlatformerActionValue(Grapple_Platformer *level, const char *action);

    /** Drive the player from code instead of the devices: a cutscene, a
     *  replay, an attract mode, a test. While scripted input is on the
     *  action map is not read. */
    extern void Grapple_PlatformerSetScriptedInput(Grapple_Platformer *level, bool enabled);
    extern bool Grapple_PlatformerScriptedInput(Grapple_Platformer *level);
    extern void Grapple_PlatformerScriptInput(Grapple_Platformer *level, float move_x, bool jump,
                                              bool run);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_PLATFORMER_H */
