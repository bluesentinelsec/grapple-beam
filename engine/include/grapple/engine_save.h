/**
 * @file engine_save.h
 * @brief Save slots: writing without losing what was already there.
 *
 * Original Grapple code (zlib).
 *
 * The engine provides the **primitives**, not the data model. It knows how
 * to put bytes somewhere safe, enumerate what is there, and hand it back;
 * it never learns what a save contains. That division is deliberate — a
 * save format is the most game-specific thing a game has, and an engine
 * that imposed one would be wrong for every game in a different way.
 *
 *     Grapple_SaveWrite(engine, 1, state, sizeof(state), "Cave of Ordeals");
 *
 *     size_t size = 0;
 *     void *data = Grapple_SaveRead(engine, 1, &size);
 *     if (data != NULL && size == sizeof(state)) {
 *         SDL_memcpy(&state, data, size);
 *     }
 *     SDL_free(data);
 *
 * ### Writes are atomic
 *
 * A save is written to a temporary file, flushed, and then renamed over the
 * target. Rename is atomic on every platform this runs on, so the old save
 * survives intact until the new one is complete on disk.
 *
 * This is the whole reason to have save primitives in an engine at all. The
 * obvious implementation — open the save file, write into it — destroys the
 * player's progress if the game crashes, the battery dies, or the disk
 * fills up halfway through. It fails rarely, and when it does it takes
 * something irreplaceable, which is the worst possible combination.
 */
#ifndef GRAPPLE_ENGINE_SAVE_H
#define GRAPPLE_ENGINE_SAVE_H

#include <grapple/engine.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Slots a game may use, 0..15. Sixteen because a UI that shows more than
 *  that is a file browser, and a game that needs one should write its own
 *  files rather than pretending they are slots. */
#define GRAPPLE_SAVE_SLOTS 16

/** Which organisation and game the save directory belongs to. Call once,
 *  before anything else here; the engine remembers it. Without it, saves
 *  go under "grapple/Game", which works and is not what you want to
 *  ship. */
extern void Grapple_SaveSetIdentity(Grapple_Engine *engine, const char *org,
                                      const char *app);

/**
 * Write a slot, atomically.
 *
 * `label` is optional and is stored beside the data for
 * Grapple_SaveInfo — a chapter name, a location, whatever a load menu
 * should show without having to parse the save itself.
 *
 * Returns false and sets SDL_GetError() on failure, having left any
 * existing save untouched.
 */
extern bool Grapple_SaveWrite(Grapple_Engine *engine, int slot, const void *data,
                                size_t size, const char *label);

/** Read a slot. Returns a SDL_malloc'd buffer — free it with SDL_free — or
 *  NULL if the slot is empty or unreadable. */
extern void *Grapple_SaveRead(Grapple_Engine *engine, int slot, size_t *size);

/** Is there anything in this slot? */
extern bool Grapple_SaveExists(Grapple_Engine *engine, int slot);

/** Delete a slot. Deleting an empty slot is not an error. */
extern bool Grapple_SaveDelete(Grapple_Engine *engine, int slot);

/** What a load menu needs to draw a row without reading the save. */
typedef struct Grapple_SaveInfo
{
    bool exists;
    size_t size;
    Sint64 modified;   /**< unix time, for "most recent" ordering */
    char label[128];
} Grapple_SaveInfo;

extern Grapple_SaveInfo Grapple_SaveInfoOf(Grapple_Engine *engine, int slot);

/** The path a slot writes to, for logging and for a player who wants to
 *  back one up. SDL_malloc'd, or NULL. */
extern char *Grapple_SavePath(Grapple_Engine *engine, int slot);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_SAVE_H */
