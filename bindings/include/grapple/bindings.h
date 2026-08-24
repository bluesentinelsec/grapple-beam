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
