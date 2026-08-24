/**
 * @file engine_assets.h
 * @brief Loading assets: blocking when you want it, streamed when you don't.
 *
 * Original Grapple code (zlib).
 *
 * Assets come out of the mounted media archive (see engine_media.h), so a
 * path means the same thing whether the game is running against a
 * directory, a zip, or bytes compiled into the binary:
 *
 *     Grapple_TextureId hero = Grapple_LoadTexture(engine, "sprites/hero.png");
 *     sprite.texture = Grapple_Texture(engine, hero);
 *
 * ### Two ways in, because games need both
 *
 * `Grapple_LoadTexture` **blocks** until the asset is there. That is what
 * you want at start-up, in a `load` hook, or anywhere the next line of code
 * genuinely cannot proceed without the thing.
 *
 * `Grapple_LoadTextureAsync` returns a handle immediately and loads on a
 * worker thread. That is what you want for a level that should keep running
 * — a loading screen with a progress bar, or an open world streaming in
 * what the player is walking towards.
 *
 * Both return the same kind of handle and share one cache, so a path
 * already loaded synchronously is instant when asked for asynchronously,
 * and the other way round.
 *
 * ### The cache is by path, and counted
 *
 * Asking twice for the same path gives the same handle and loads once —
 * which matters more than it sounds, because forty actors of the same type
 * asking for the same sprite is the normal case, not an edge case.
 * Handles are reference counted; the last release frees the asset.
 *
 * ### Why the main thread still does some work
 *
 * Decoding a PNG is pure computation and happens on a worker. Creating an
 * `SDL_Texture` from the result is not: it touches the renderer, and SDL's
 * renderer belongs to the thread that made it. So the pipeline is
 *
 *     worker:  read from the VFS, decode  ->  SDL_Surface
 *     main:    SDL_CreateTextureFromSurface  ->  SDL_Texture
 *
 * and the main-thread half is **time-sliced**: the engine spends at most a
 * budget per frame turning decoded surfaces into textures, then stops until
 * next frame. Without that, a batch of forty textures finishing together
 * uploads forty textures in one frame and the game visibly hitches at
 * exactly the moment a loading bar is supposed to be reassuring somebody.
 * Godot and Unity both time-slice the same step for the same reason.
 */
#ifndef GRAPPLE_ENGINE_ASSETS_H
#define GRAPPLE_ENGINE_ASSETS_H

#include <grapple/engine.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A handle to a loaded asset. Zero is "nothing", and never resolves. */
typedef Uint32 Grapple_TextureId;

#ifdef __cplusplus
#define GRAPPLE_ASSET_NONE (static_cast<Grapple_TextureId>(0))
#else
#define GRAPPLE_ASSET_NONE ((Grapple_TextureId)0)
#endif

/** Where an asset is in its life. */
typedef enum Grapple_AssetStatus
{
    GRAPPLE_ASSET_MISSING = 0, /**< no such handle */
    GRAPPLE_ASSET_QUEUED,      /**< asked for, not started */
    GRAPPLE_ASSET_LOADING,     /**< a worker is decoding it */
    GRAPPLE_ASSET_DECODED,     /**< decoded, waiting for the main thread */
    GRAPPLE_ASSET_READY,
    /** Could not be loaded: absent from the archive, or not an image. Not
     *  fatal — the handle stays valid and resolves to the placeholder, so a
     *  missing file costs a wrong-looking sprite rather than a crash. */
    GRAPPLE_ASSET_FAILED
} Grapple_AssetStatus;

/* --- loading ------------------------------------------------------------- */

/** Load a texture, **blocking** until it is there. Returns
 *  GRAPPLE_ASSET_NONE only if the engine has no renderer; a missing file
 *  gives a valid handle whose status is FAILED. */
extern Grapple_TextureId Grapple_LoadTexture(Grapple_Engine *engine, const char *path);

/** Ask for a texture without waiting. The handle is usable immediately;
 *  Grapple_Texture returns the placeholder until it is ready. */
extern Grapple_TextureId Grapple_LoadTextureAsync(Grapple_Engine *engine,
                                                      const char *path);

/**
 * The texture behind a handle.
 *
 * Never NULL for a valid handle: an asset that is still loading, or that
 * failed, resolves to the **placeholder** — a magenta and black check,
 * chosen because it is impossible to mistake for art and impossible to
 * miss in a screenshot. A game may draw it without checking, and a missing
 * file looks obviously wrong rather than invisibly absent.
 */
extern SDL_Texture *Grapple_Texture(Grapple_Engine *engine, Grapple_TextureId id);

extern Grapple_AssetStatus Grapple_AssetStatusOf(Grapple_Engine *engine,
                                                     Grapple_TextureId id);

/** The path a handle was loaded from, for logging. NULL if unknown. */
extern const char *Grapple_AssetPath(Grapple_Engine *engine, Grapple_TextureId id);

/** Keep a handle alive past the release below — for a second owner. */
extern void Grapple_AssetRetain(Grapple_Engine *engine, Grapple_TextureId id);

/** Drop a reference. The asset is freed when the last one goes, so a level
 *  that releases what it loaded gets its memory back without the engine
 *  guessing when. */
extern void Grapple_AssetRelease(Grapple_Engine *engine, Grapple_TextureId id);

/* --- watching a load ----------------------------------------------------- */

/** Is everything asked for so far finished — successfully or not? What a
 *  loading screen waits on. */
extern bool Grapple_AssetsReady(Grapple_Engine *engine);

/** How far through, 0..1. Counts assets rather than bytes, which is what a
 *  progress bar can actually be driven by without knowing sizes up front.
 *  Returns 1.0 when there is nothing outstanding. */
extern float Grapple_AssetsProgress(Grapple_Engine *engine);

/** How many are still in flight. */
extern int Grapple_AssetsPending(Grapple_Engine *engine);

/** How many are loaded and resident. */
extern int Grapple_AssetsLoaded(Grapple_Engine *engine);

/** Block until everything outstanding has landed. For the end of a loading
 *  screen, or a test. */
extern void Grapple_AssetsWait(Grapple_Engine *engine);

/* --- tuning -------------------------------------------------------------- */

/**
 * Milliseconds per frame the engine may spend turning decoded pixels into
 * textures. 2 ms by default — about an eighth of a 60 Hz frame.
 *
 * Raise it on a loading screen, where there is nothing else to spend the
 * frame on and finishing sooner is the whole point; leave it low during
 * gameplay, where a hitch is worse than a late texture. Zero finalises
 * exactly one asset per frame; a negative value finalises everything
 * immediately and will hitch.
 */
extern void Grapple_AssetsSetFrameBudget(Grapple_Engine *engine, float milliseconds);
extern float Grapple_AssetsFrameBudget(Grapple_Engine *engine);

/** How many worker threads decode. Defaults to two, which is enough to
 *  keep ahead of the main thread's upload budget without competing with
 *  the game for cores. Clamped to [1, 8]; set before the first load. */
extern void Grapple_AssetsSetWorkers(Grapple_Engine *engine, int workers);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_ASSETS_H */
