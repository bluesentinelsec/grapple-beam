/**
 * @file engine_binding.h
 * @brief Actions: named intents bound to whatever the player presses.
 *
 * Original Grapple code (zlib).
 *
 * Game code should say what it means, not which key means it:
 *
 *     if (Grapple_ActionPressed(engine, map, player, "jump")) Jump();
 *     float move = Grapple_ActionValue(engine, map, player, "move_x");
 *
 * The point is not tidiness. A game written against scancodes cannot be
 * rebound without editing the game, cannot support a controller without
 * writing every check twice, and cannot be played by someone whose hands
 * do not fit the layout the designer happened to have. An action map is
 * how all three stop being the game's problem.
 *
 *     Grapple_ActionMap *map = Grapple_ActionMapCreate();
 *     Grapple_ActionBindKey(map, "jump", SDL_SCANCODE_SPACE);
 *     Grapple_ActionBindPad(map, "jump", GRAPPLE_PAD_A);
 *     Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_A, -1);
 *     Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_D, +1);
 *     Grapple_ActionBindAxis(map, "move_x", GRAPPLE_AXIS_LEFT_X, 0);
 *
 * ### Actions are signed and analog
 *
 * Every action has a value in [-1, 1], not a boolean. A key contributes its
 * binding's sign; a stick contributes its deflection. So "move_x" bound to
 * A, D and the left stick works with all three at once, and a game reads
 * one number whether the player is using a keyboard or leaning on a stick.
 * `Down` is then "is the magnitude past the threshold", which is the thing
 * a boolean action wanted to mean anyway.
 *
 * ### Who is player 2
 *
 * Gamepad bindings read the gamepad in the player's own slot: player 1 uses
 * pad 1. Keyboard and mouse bindings belong to whichever player
 * Grapple_ActionMapSetKeyboardPlayer says — player 0 by default — because
 * there is one keyboard and it cannot be shared by four people. Set it to
 * -1 for a strictly-gamepad game.
 */
#ifndef GRAPPLE_ENGINE_BINDING_H
#define GRAPPLE_ENGINE_BINDING_H

#include <grapple/engine_input.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_ActionMap Grapple_ActionMap;

/** Where a binding gets its value from. */
typedef enum Grapple_BindingSource
{
    GRAPPLE_BIND_NONE = 0,
    GRAPPLE_BIND_KEY,          /**< code is an SDL_Scancode */
    GRAPPLE_BIND_MOUSE_BUTTON, /**< code is an Grapple_MouseButton */
    GRAPPLE_BIND_MOUSE_WHEEL,  /**< code 0 is vertical, 1 horizontal */
    GRAPPLE_BIND_PAD_BUTTON,   /**< code is an Grapple_GamepadButton */
    GRAPPLE_BIND_PAD_AXIS,     /**< code is an Grapple_GamepadAxis */
    /** A stick or the d-pad held in a direction, with menu repeat. code is
     *  an Grapple_Direction. */
    GRAPPLE_BIND_PAD_DIRECTION
} Grapple_BindingSource;

/**
 * One way to trigger an action. An action may have several, and they are
 * all live at once — that is how a game supports a keyboard and a
 * controller without a mode switch.
 */
typedef struct Grapple_Binding
{
    Grapple_BindingSource source;
    int code;
    /** Which way this binding pushes the action: +1 or -1. A jump button is
     *  +1; the A key of an A/D pair is -1. */
    int sign;
    /** For an axis: ignore deflection the other way. 0 accepts both
     *  directions (a stick axis bound to a signed action), +1 or -1 accepts
     *  only that side (half a stick bound to a one-way action). */
    int axis_half;
} Grapple_Binding;

/* --- the map ------------------------------------------------------------- */

extern Grapple_ActionMap *Grapple_ActionMapCreate(void);
extern void Grapple_ActionMapDestroy(Grapple_ActionMap *map);

/** Which player the keyboard and mouse belong to. 0 by default; -1 means
 *  nobody, for a game that is gamepad-only. */
extern void Grapple_ActionMapSetKeyboardPlayer(Grapple_ActionMap *map, int player);
extern int Grapple_ActionMapKeyboardPlayer(Grapple_ActionMap *map);

/** Add a binding. The action is created if it does not exist, so there is
 *  no separate "declare the action" step. Returns false only on
 *  allocation failure or a nonsense binding. */
extern bool Grapple_ActionBind(Grapple_ActionMap *map, const char *action,
                                 Grapple_Binding binding);

/* Shorthands for the bindings games actually write. */
extern bool Grapple_ActionBindKey(Grapple_ActionMap *map, const char *action,
                                    SDL_Scancode key);
extern bool Grapple_ActionBindKeySigned(Grapple_ActionMap *map, const char *action,
                                          SDL_Scancode key, int sign);
extern bool Grapple_ActionBindMouse(Grapple_ActionMap *map, const char *action,
                                      Grapple_MouseButton button);
extern bool Grapple_ActionBindPad(Grapple_ActionMap *map, const char *action,
                                    Grapple_GamepadButton button);
/** `half` is 0 for the whole axis, or +1/-1 for one side of it. */
extern bool Grapple_ActionBindAxis(Grapple_ActionMap *map, const char *action,
                                     Grapple_GamepadAxis axis, int half);
extern bool Grapple_ActionBindDirection(Grapple_ActionMap *map, const char *action,
                                          Grapple_Direction direction);

/** Forget every binding for an action — step one of rebinding it. */
extern void Grapple_ActionClear(Grapple_ActionMap *map, const char *action);

/** How many actions the map knows, and their names, for a settings screen
 *  that lists them. */
extern int Grapple_ActionCount(Grapple_ActionMap *map);
extern const char *Grapple_ActionName(Grapple_ActionMap *map, int index);

/** The bindings on one action, for the same screen. */
extern int Grapple_ActionBindingCount(Grapple_ActionMap *map, const char *action);
extern bool Grapple_ActionBindingAt(Grapple_ActionMap *map, const char *action, int index,
                                      Grapple_Binding *out);

/* --- reading ------------------------------------------------------------- */

/** The action's value, in [-1, 1]. Several bindings sum and clamp, so
 *  holding A and D cancels out rather than fighting. */
extern float Grapple_ActionValue(Grapple_Engine *engine, Grapple_ActionMap *map,
                                   int player, const char *action);

/** Past the threshold now. */
extern bool Grapple_ActionDown(Grapple_Engine *engine, Grapple_ActionMap *map, int player,
                                 const char *action);

/** Crossed the threshold this frame — and, like every edge in this engine,
 *  stays true for all of it, including every fixed step. */
extern bool Grapple_ActionPressed(Grapple_Engine *engine, Grapple_ActionMap *map,
                                    int player, const char *action);

extern bool Grapple_ActionReleased(Grapple_Engine *engine, Grapple_ActionMap *map,
                                     int player, const char *action);

/** Two actions as a vector, normalised so that diagonal movement is not
 *  faster than straight movement — the oldest bug in 2D games. */
extern void Grapple_ActionVector(Grapple_Engine *engine, Grapple_ActionMap *map,
                                   int player, const char *x_action, const char *y_action,
                                   float *x, float *y);

/* --- rebinding ----------------------------------------------------------- */

/**
 * Capture whatever the player presses next, for a "press a key to bind"
 * prompt.
 *
 * Returns true on the frame something was pressed, with the binding in
 * `out`. Movement is ignored — a stick resting off-centre or a mouse being
 * moved would otherwise capture itself the instant the prompt opened.
 *
 * `player` limits capture to one player's gamepad, or pass -1 for any.
 */
extern bool Grapple_ActionCapture(Grapple_Engine *engine, int player,
                                    Grapple_Binding *out);

/** A binding as text — "space", "pad:a", "axis:left_x", "mouse:left" — for
 *  a settings screen and for saving. Writes into `buffer` and returns it. */
extern const char *Grapple_BindingToString(Grapple_Binding binding, char *buffer,
                                             size_t size);

/** Parse what BindingToString wrote. Returns false on anything else. */
extern bool Grapple_BindingFromString(const char *text, Grapple_Binding *out);

/* --- persistence --------------------------------------------------------- */

/**
 * The map as TOML, for saving alongside the graphics settings:
 *
 *     [bindings]
 *     jump = ["space", "pad:a"]
 *     move_x = ["-a", "+d", "axis:left_x"]
 *
 * Returns a SDL_malloc'd string; free it with SDL_free.
 */
extern char *Grapple_ActionMapToToml(Grapple_ActionMap *map);

/** Load bindings from TOML. Actions present in the document **replace**
 *  their bindings entirely; actions absent from it keep the defaults the
 *  game set up. That way a player who rebound one key does not lose every
 *  other binding when the game adds a new action in a later version. */
extern bool Grapple_ActionMapLoadToml(Grapple_ActionMap *map, const char *toml);

/** Save to `bindings.toml` in the pref directory, beside config.toml. */
extern bool Grapple_ActionMapSave(Grapple_ActionMap *map, const char *org, const char *app);

/** Load it back. Returns false if there is nothing to load, which is not an
 *  error — it is what happens the first time a game runs. */
extern bool Grapple_ActionMapLoad(Grapple_ActionMap *map, const char *org, const char *app);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_BINDING_H */
