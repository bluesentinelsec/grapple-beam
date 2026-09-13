/*
 * Engine runner and interactive shell for the Grapple interpreters.
 *
 * Original Grapple code (zlib).
 *
 *   grapple-beam repl --language lua                    interactive Lua shell
 *   grapple-beam repl --language ruby                   interactive Ruby shell
 *   grapple-beam eval --language lua --code 'print(1+2)' evaluate a one-liner
 *   grapple-beam script.rb -- a b                       run a script
 *
 * Both states come up with the game bindings (Grapple module) and
 * VFS-aware require already installed; "." is on the Ruby $LOAD_PATH.
 */
#include "runner.h"

#include <grapple/bindings.h>
#include <grapple/lua.h>
#include <grapple/ruby.h>
#include <lauxlib.h>
#include <lualib.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------- Lua ------ */

static int LuaReportError(lua_State *L)
{
    fprintf(stderr, "error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return 1;
}

static int RunLua(const char *code, const char *script, int argc, char **argv)
{
    lua_State *L = Grapple_CreateLuaState();
    if (L == NULL || !Grapple_OpenLuaBindings(L))
    {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        if (L != NULL)
            lua_close(L);
        return 1;
    }
    /* arg = {script args}, like the standalone lua interpreter */
    lua_newtable(L);
    for (int i = 0; i < argc; ++i)
    {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "arg");

    int rc = 0;
    if (code != NULL)
    {
        if (luaL_dostring(L, code) != LUA_OK)
        {
            rc = LuaReportError(L);
        }
    }
    else if (script != NULL)
    {
        if (luaL_dofile(L, script) != LUA_OK)
        {
            rc = LuaReportError(L);
        }
        else
        {
            /* A script that described callbacks and stopped meant for the
               loop to run — the same bargain Love2D makes. */
            Grapple_LuaRunPendingEngine(L);
        }
    }
    else
    {
        char line[2048];
        printf("Grapple Lua %s — 'exit' to quit\n", LUA_VERSION_RELEASE);
        for (;;)
        {
            printf("lua> ");
            fflush(stdout);
            if (fgets(line, sizeof(line), stdin) == NULL || strncmp(line, "exit", 4) == 0)
            {
                break;
            }
            /* Try as an expression first so `1+2` prints 3. */
            char wrapped[2100];
            snprintf(wrapped, sizeof(wrapped), "print((function() return %s end)())", line);
            if (luaL_dostring(L, wrapped) != LUA_OK)
            {
                lua_pop(L, 1);
                if (luaL_dostring(L, line) != LUA_OK)
                {
                    LuaReportError(L);
                }
            }
        }
    }
    lua_close(L);
    return rc;
}

/* ------------------------------------------------------------ Ruby ------ */

static int RubyReportError(mrb_state *mrb)
{
    if (mrb->exc != NULL)
    {
        mrb_value msg = mrb_inspect(mrb, mrb_obj_value(mrb->exc));
        fprintf(stderr, "error: %s\n", RSTRING_CSTR(mrb, msg));
        mrb->exc = NULL;
        return 1;
    }
    return 0;
}

static int RunRuby(const char *code, const char *script, int argc, char **argv)
{
    mrb_state *mrb = Grapple_CreateRubyState();
    if (mrb == NULL || !Grapple_OpenRubyBindings(mrb))
    {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        if (mrb != NULL)
            mrb_close(mrb);
        return 1;
    }
    Grapple_RubyAddLoadPath(mrb, ".");
    mrb_value args = mrb_ary_new(mrb);
    for (int i = 0; i < argc; ++i)
    {
        mrb_ary_push(mrb, args, mrb_str_new_cstr(mrb, argv[i]));
    }
    mrb_define_global_const(mrb, "ARGV", args);

    int rc = 0;
    if (code != NULL)
    {
        mrb_load_string(mrb, code);
        rc = RubyReportError(mrb);
    }
    else if (script != NULL)
    {
        mrb_funcall(mrb, mrb_top_self(mrb), "load", 1, mrb_str_new_cstr(mrb, script));
        rc = RubyReportError(mrb);
        if (rc == 0)
        {
            Grapple_RubyRunPendingEngine(mrb);
        }
    }
    else
    {
        char line[2048];
        printf("Grapple mruby %s — 'exit' to quit\n", MRUBY_VERSION);
        for (;;)
        {
            printf("ruby> ");
            fflush(stdout);
            if (fgets(line, sizeof(line), stdin) == NULL || strncmp(line, "exit", 4) == 0)
            {
                break;
            }
            mrb_value result = mrb_load_string(mrb, line);
            if (mrb->exc != NULL)
            {
                RubyReportError(mrb);
            }
            else
            {
                mrb_value inspected = mrb_inspect(mrb, result);
                printf("=> %s\n", RSTRING_CSTR(mrb, inspected));
            }
        }
    }
    mrb_close(mrb);
    return rc;
}

int GrappleRunner_Execute(const char *language, const char *code, const char *script, int argc,
                          char **argv)
{
    if (strcmp(language, "lua") == 0)
        return RunLua(code, script, argc, argv);
    return RunRuby(code, script, argc, argv);
}
