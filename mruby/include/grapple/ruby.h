/**
 * @file ruby.h
 * @brief mruby state helper with require support (Grapple Ruby).
 *
 * Original Grapple code (zlib) around the vendored mruby 4.0.0. mruby
 * core deliberately ships without require; Grapple_CreateRubyState adds
 * CRuby-faithful Kernel#require / #require_relative / #load:
 *
 *  - $LOAD_PATH searched in order; entries are VFS paths when PhysFS is
 *    initialized (so modules can live in a mounted, optionally encrypted,
 *    zip archive), with real-filesystem fallback.
 *  - $LOADED_FEATURES dedup: require loads once, returns false on repeats;
 *    load always re-executes. Circular requires are guarded.
 *  - Resolution tries "<name>.rb" (compiled at runtime by mruby-compiler
 *    with the real filename for backtraces) then "<name>.mrb"
 *    (precompiled RITE bytecode).
 *
 * Kernel#puts/print/p are also provided (no IO gem — keeps the build free
 * of per-platform HAL code). Use the native mruby C API for everything
 * else.
 */
#ifndef GRAPPLE_RUBY_H
#define GRAPPLE_RUBY_H

#include <SDL3/SDL.h>
#include <mruby.h>

#ifdef __cplusplus
extern "C" {
#endif

/** New mruby state with require/puts installed. Close with mrb_close(). */
extern mrb_state *Grapple_CreateRubyState(void);

/** Append a directory (VFS or real) to $LOAD_PATH. */
extern bool Grapple_RubyAddLoadPath(mrb_state *mrb, const char *path);

/** Run a script from the mounted VFS. False + SDL_GetError on error. */
extern bool Grapple_RubyRunVFSScript(mrb_state *mrb, const char *vfsPath);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_RUBY_H */
