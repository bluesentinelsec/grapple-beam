/**
 * @file dialog.h
 * @brief Native file open/save dialogs, poll-style.
 *
 * Original Grapple code (zlib) over SDL's dialog API. SDL delivers the
 * chosen file through a callback and takes an array of filters — neither
 * of which can cross a Lua/Ruby binding boundary, and callbacks are
 * awkward in an immediate-mode frame loop even in C. This wrapper starts
 * a dialog and lets the caller poll a small state machine each frame:
 *
 *     if (nk_button_label(ctx, "Open")) {
 *         Grapple_ShowOpenFileDialog(window, "Text files", "txt", NULL);
 *     }
 *     if (Grapple_DialogStatus() == GRAPPLE_DIALOG_ACCEPTED) {
 *         load(Grapple_DialogPath());
 *         Grapple_DialogReset();
 *     }
 *
 * One dialog is tracked at a time (that is all a modal file picker can be).
 *
 * Web builds: SDL ships no Emscripten dialog backend, so this implements
 * one over browser APIs. Opening uses a hidden <input type="file">; the
 * chosen file is copied into the Emscripten filesystem and its path is
 * reported exactly like a native one, so app code is identical everywhere.
 * Saving differs by necessity — a page cannot write to the user's disk, so
 * there is no save picker: the save dialog resolves immediately to a path
 * in the virtual filesystem, and Grapple_DialogDeliverSave hands the
 * finished file to the user as a browser download. Calling it on desktop
 * is a harmless no-op, so the same code works on both.
 */
#ifndef GRAPPLE_DIALOG_H
#define GRAPPLE_DIALOG_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Grapple_DialogState
{
    GRAPPLE_DIALOG_IDLE = 0,  /**< nothing has been requested */
    GRAPPLE_DIALOG_PENDING,   /**< the dialog is open, waiting on the user */
    GRAPPLE_DIALOG_ACCEPTED,  /**< a file was chosen; see Grapple_DialogPath */
    GRAPPLE_DIALOG_CANCELLED, /**< the user dismissed the dialog */
    GRAPPLE_DIALOG_ERROR      /**< it could not be shown; see SDL_GetError */
} Grapple_DialogState;

/** Show a native "open file" dialog. `filter_name`/`filter_pattern` are
 *  optional (NULL for no filter); pattern is SDL's syntax, e.g. "txt" or
 *  "png;jpg". `default_location` may be NULL. Returns false if a dialog is
 *  already pending. */
extern bool Grapple_ShowOpenFileDialog(SDL_Window *window, const char *filter_name,
                                         const char *filter_pattern,
                                         const char *default_location);

/** Show a native "save file" dialog — the Save As half of a document app. */
extern bool Grapple_ShowSaveFileDialog(SDL_Window *window, const char *filter_name,
                                         const char *filter_pattern,
                                         const char *default_location);

/** Current state. Poll this each frame after starting a dialog. */
extern Grapple_DialogState Grapple_DialogStatus(void);

/** The chosen path once the status is ACCEPTED, else NULL. Owned by the
 *  library and valid until the next dialog or Grapple_DialogReset. */
extern const char *Grapple_DialogPath(void);

/** Hand a file the app has just written to the user.
 *
 *  Desktop: nothing to do — the file is already where the user chose, so
 *  this returns true immediately. Web: downloads it through the browser
 *  using the file's base name, which is the only way a page can deliver a
 *  file to the user's disk. Call it after saving whatever the save dialog
 *  handed you, and the same code path works on every platform. */
extern bool Grapple_DialogDeliverSave(const char *path);

/** Return to IDLE and release the stored path. Call after handling a
 *  result so the next dialog starts clean. */
extern void Grapple_DialogReset(void);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_DIALOG_H */
