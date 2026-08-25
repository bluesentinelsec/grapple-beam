/**
 * @file bindings.h
 * @brief Game-surface script bindings for Lua and Ruby (Grapple).
 *
 * Original Grapple code (zlib). Installs a `Grapple` module exposing
 * the same API in both languages — windows/rendering, textures (VFS-aware
 * loading), audio playback, Box2D physics, VFS mounts (incl. encrypted),
 * and the crypto/compress/base64 utilities. Every native object is an
 * opaque handle finalized by the language's garbage collector; child
 * handles pin their parents so collection order is always safe.
 *
 *   local app  = Grapple.window("Game", 640, 360)     -- Lua
 *   app  = Grapple.window("Game", 640, 360)           #  Ruby
 *
 * See bindings/README.md for the full surface.
 */
#ifndef GRAPPLE_BINDINGS_H
#define GRAPPLE_BINDINGS_H

#include <SDL3/SDL.h>
#include <grapple/engine.h>
#include <grapple/engine_script.h>
#include <mruby.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>

/** Install the `Grapple` global table into a Lua state. */
extern bool Grapple_OpenLuaBindings(lua_State *L);

/** Engine hooks for Lua: GrappleC.OnUpdate(engine, fn) and friends, plus
 *  GrappleC.Run(engine). Called for you by Grapple_OpenLuaBindings.
 *
 *  Separate from the generated surface because turning a Lua function into
 *  something C can hold is the one thing the generator cannot do. */
extern bool Grapple_OpenLuaEngineHooks(lua_State *L);

/** The same for Ruby. Called for you by Grapple_OpenRubyBindings. */
extern bool Grapple_OpenRubyEngineHooks(mrb_state *mrb);

/** Install the `Regex` module — compiled patterns as objects with
 *  match/gmatch/gsub/split — into a Lua state. Lua has patterns rather
 *  than regular expressions, so this is an addition, not a replacement:
 *  string.find and friends are untouched. Called for you by
 *  Grapple_OpenLuaBindings. */
extern bool Grapple_OpenLuaRegex(lua_State *L);

/** The retained widget tree, as `Grapple.ui(engine)`. See grapple/widgets.h. */
extern bool Grapple_OpenLuaUi(lua_State *L);

/** `Grapple.engine{...}`: one table instead of a run of setters. */
extern bool Grapple_OpenLuaEngine(lua_State *L);

/** The same pair for Ruby: Grapple.engine(...) and Grapple.ui(engine). */
extern bool Grapple_OpenRubyUi(mrb_state *mrb);

/**
 * The command line a script's engine should inherit.
 *
 * The engine reads --fullscreen, --window-size and thirty-odd other options
 * from the argc/argv in its config, and a script has no way to hand it any:
 * the setters never exposed the fields, and a script does not have the
 * process arguments to begin with. The runner calls this once with the
 * engine-bound part of its own command line, and Grapple.engine{} passes it
 * on, so those flags do what they say. Not copied — the pointers must
 * outlive every engine, which the real argv does.
 */
extern void Grapple_SetScriptProcessArgs(int argc, char **argv);

/** What the runner last set, for the engine constructors. */
extern void Grapple_ScriptProcessArgs(int *argc, char ***argv);

/**
 * Run an engine the script described but never started.
 *
 * Love2D and Godot do not make a script call the loop; this is what lets a
 * script here stop after registering its callbacks. Returns false when
 * there was nothing to run — no engine, or one with no handlers. Called by
 * the runner once the script body has finished.
 */
extern bool Grapple_LuaRunPendingEngine(lua_State *L);
extern bool Grapple_RubyRunPendingEngine(mrb_state *mrb);

/** The engine behind either Ruby spelling. */
extern Grapple_Engine *Grapple_RubyEngineAt(mrb_state *mrb, mrb_value value);

/** Bind one block to one engine hook. */
extern bool Grapple_RubyBindEngineHook(mrb_state *mrb, Grapple_Engine *engine,
                                         Grapple_ScriptHook hook, mrb_value block);

/** The engine behind either spelling — the object or a generated handle. */
extern Grapple_Engine *Grapple_LuaEngineAt(lua_State *L, int index);

/** Bind one Lua function to one engine hook. Used by both spellings. */
extern bool Grapple_LuaBindEngineHook(lua_State *L, Grapple_Engine *engine,
                                        Grapple_ScriptHook hook, int fn_index);

/** Install the `Grapple` module into an mruby state. */
extern bool Grapple_OpenRubyBindings(mrb_state *mrb);

/** Install `Regexp` and `MatchData`, plus the String methods that take a
 *  pattern, over Grapple::Regex.
 *
 *  mruby has no regex engine of its own, but its compiler does know the
 *  syntax: defining this class is what makes `/re/` literals, `=~`, `$~`
 *  and `$1` work. Grapple_OpenRubyBindings calls this for you; it is
 *  exposed for hosts that install a narrower surface. */
extern bool Grapple_OpenRubyRegexp(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_BINDINGS_H */
