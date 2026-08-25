/**
 * @file event_sink.h
 * @brief A place to hand SDL events, shared by modules that do not know
 *        about each other.
 *
 * Original Grapple code (zlib).
 *
 * An immediate-mode GUI has to see every event of a frame, and it has to be
 * told when the frame's events start and stop arriving — Nuklear's input
 * accumulates between nk_input_begin and nk_input_end. A game loop that
 * pumps events itself therefore has to bracket somebody else's input, which
 * is a thing the loop should do rather than the caller.
 *
 * This is the interface that lets it, without either side depending on the
 * other: the engine takes a sink and drives it, Grapple::GUI supplies one
 * (Grapple_GuiEventSink), and neither module links the other.
 *
 *   Grapple_Gui *gui = Grapple_CreateGui(renderer, NULL, 0, 15.0f);
 *   const Grapple_EventSink sink = Grapple_GuiEventSink(gui);
 *   Grapple_EngineSetEventSink(engine, &sink);
 *   // that is all: no InputBegin, no ProcessEvent, no InputEnd anywhere.
 *
 * Nothing here is GUI-specific. A debug console, a replay recorder or an
 * input remapper can be installed the same way.
 */
#ifndef GRAPPLE_EVENT_SINK_H
#define GRAPPLE_EVENT_SINK_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Somewhere to send a frame's events.
 *
 * `begin` is called before the frame's events are drained, `event` once per
 * event in arrival order, and `end` after the last one — even in a frame
 * that had no events at all, because "nothing happened this frame" is
 * information an immediate-mode UI needs.
 *
 * Any callback may be NULL. `user` is passed back untouched.
 */
typedef struct Grapple_EventSink
{
    void *user;
    void (*begin)(void *user);
    void (*event)(void *user, const SDL_Event *event);
    void (*end)(void *user);
} Grapple_EventSink;

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_EVENT_SINK_H */
