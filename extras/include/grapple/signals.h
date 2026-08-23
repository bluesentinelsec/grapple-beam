/**
 * @file signals.h
 * @brief Godot-style signals for C (Grapple Extras).
 *
 * Original Grapple code (zlib). An emitter owns named signals; any code
 * can connect a callback to a signal name and gets invoked (in connection
 * order) whenever that signal is emitted:
 *
 *   Grapple_SignalEmitter *door = Grapple_CreateSignalEmitter();
 *   Uint64 id = Grapple_ConnectSignal(door, "opened", on_door_opened, hud, 0);
 *   ...
 *   Grapple_EmitSignal(door, "opened", &args);   // calls on_door_opened(hud, &args)
 *   Grapple_DisconnectSignal(door, id);
 *
 * Semantics (matching Godot where it matters):
 *  - Connections made *during* an emission do not fire in that emission.
 *  - Disconnecting during an emission is safe (including self-disconnect).
 *  - GRAPPLE_SIGNAL_ONESHOT auto-disconnects after the first delivery.
 *  - Emission is reentrant (a handler may emit).
 *  - Not thread-safe by design: use from one thread, like a game loop.
 */
#ifndef GRAPPLE_SIGNALS_H
#define GRAPPLE_SIGNALS_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_SignalEmitter Grapple_SignalEmitter;

/** Callback invoked on emission. `args` is the pointer passed to Emit. */
typedef void (*Grapple_SignalCallback)(void *userdata, const char *signal, void *args);

#define GRAPPLE_SIGNAL_ONESHOT (1u << 0) /**< auto-disconnect after first delivery */

/** Create an emitter. Destroy with Grapple_DestroySignalEmitter. */
extern Grapple_SignalEmitter *Grapple_CreateSignalEmitter(void);

/** Destroy an emitter and all its connections. Must not be called from a
 *  handler currently being invoked by this emitter. */
extern void Grapple_DestroySignalEmitter(Grapple_SignalEmitter *emitter);

/**
 * Connect a callback to a signal name.
 * \returns a nonzero connection id, or 0 on error.
 */
extern Uint64 Grapple_ConnectSignal(Grapple_SignalEmitter *emitter, const char *signal,
                                      Grapple_SignalCallback callback, void *userdata,
                                      Uint32 flags);

/** Disconnect by id. \returns true if the connection existed. */
extern bool Grapple_DisconnectSignal(Grapple_SignalEmitter *emitter, Uint64 connection);

/**
 * Emit a signal: invokes every live connection for `signal` in connection
 * order, passing `args` through untouched.
 * \returns the number of callbacks invoked, or -1 on error.
 */
extern int Grapple_EmitSignal(Grapple_SignalEmitter *emitter, const char *signal, void *args);

/** Number of live connections for a signal (all signals if NULL). */
extern int Grapple_CountSignalConnections(Grapple_SignalEmitter *emitter, const char *signal);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_SIGNALS_H */
