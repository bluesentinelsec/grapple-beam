/*
 * grapple_engine_hooks_lua.c — registering Lua functions as engine hooks.
 * Original Grapple code (zlib).
 *
 * The generator binds anything taking numbers and handles. It cannot turn a
 * Lua function into a C function pointer, so this is the glue, and it is
 * deliberately tiny: the engine's bridge (engine_script.h) does the
 * dispatching and knows nothing about Lua; this file only converts a Lua
 * value into a handle the bridge can hold, and back again.
 *
 * A handle is a luaL_ref into the registry. That is the only way to keep a
 * Lua function alive across a C callback — a value left on the stack is
 * gone by the time the engine calls back, and one held only by a local is
 * collected.
 */
#include <grapple/engine.h>
#include <grapple/engine_script.h>
#include <grapple/gui.h>
#include <grapple/engine_scene.h>
#include <grapple/lua.h>

#include <lauxlib.h>
#include <lua.h>

#include "gen_support_lua.h"

/* The bridge hands back whatever we gave it. Going through the registry
   means the reference survives garbage collection for exactly as long as
   the engine holds it. */
static bool DispatchLua(void *language_state, Sint64 handle, Grapple_ScriptHook hook,
                        float value, const void *payload)
{
    lua_State *L = (lua_State *)language_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, (lua_Integer)handle);
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);
        return true;
    }

    /* Only the hooks that carry a number take an argument; the rest are
       called with none, so a script writes `function() ... end` for them
       rather than ignoring a meaningless zero. */
    int args = 0;
    if (hook == GRAPPLE_HOOK_FIXED_UPDATE || hook == GRAPPLE_HOOK_UPDATE ||
        hook == GRAPPLE_HOOK_RENDER)
    {
        lua_pushnumber(L, (lua_Number)value);
        args = 1;
    }
    else if (hook == GRAPPLE_HOOK_EVENT)
    {
        /* Borrowed: the same handle every generated SDL.* function takes,
           but pointing at a stack event the engine owns. It is valid only
           until this call returns, which is why nothing takes ownership. */
        GrappleGen_LuaPushHandle(L, (void *)(uintptr_t)payload, "SDL_Event");
        args = 1;
    }
    else if (hook == GRAPPLE_HOOK_RESIZE)
    {
        const Grapple_ScriptSize *size = (const Grapple_ScriptSize *)payload;
        lua_pushinteger(L, (lua_Integer)(size != NULL ? size->width : 0));
        lua_pushinteger(L, (lua_Integer)(size != NULL ? size->height : 0));
        args = 2;
    }

    if (lua_pcall(L, args, 1, 0) != LUA_OK)
    {
        /* An error in a hook must not unwind through the engine's C frames.
           Report it and carry on, so one bad frame does not take the game
           down mid-loop. */
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "engine hook: %s",
                     lua_tostring(L, -1) ? lua_tostring(L, -1) : "error");
        lua_pop(L, 1);
        return true;
    }

    /* Only `load` reads a result: returning false there aborts start-up.
       Everywhere else the return is ignored, so a script may end a handler
       with an expression without meaning anything by it. */
    bool ok = true;
    if (hook == GRAPPLE_HOOK_LOAD && !lua_isnil(L, -1))
    {
        ok = lua_toboolean(L, -1) != 0;
    }
    lua_pop(L, 1);
    return ok;
}

static void ReleaseLua(void *language_state, Sint64 handle)
{
    luaL_unref((lua_State *)language_state, LUA_REGISTRYINDEX, (int)handle);
}

/* GrappleC.OnFixedUpdate(engine, function(step) ... end) and friends. */
static int SetHook(lua_State *L, Grapple_ScriptHook hook)
{
    /* The generated bindings box a handle in a userdata, so lua_touserdata
       would hand back the box rather than the engine — which binds happily
       and then never calls anything. Go through the same unboxer the
       generated code uses. */
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    luaL_checktype(L, 2, LUA_TFUNCTION);

    if (!Grapple_ScriptBind(engine, L, DispatchLua, ReleaseLua))
    {
        return luaL_error(L, "could not bind Lua to this engine");
    }
    lua_pushvalue(L, 2);
    const int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (!Grapple_ScriptSetHook(engine, hook, (Sint64)ref))
    {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return luaL_error(L, "%s", SDL_GetError());
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int LOnLoad(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_LOAD); }
static int LOnFixedUpdate(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_FIXED_UPDATE); }
static int LOnUpdate(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_UPDATE); }
static int LOnRender(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_RENDER); }
static int LOnPostRender(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_POST_RENDER); }
static int LOnEvent(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_EVENT); }
static int LOnResize(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_RESIZE); }
static int LOnUnload(lua_State *L) { return SetHook(L, GRAPPLE_HOOK_UNLOAD); }

/* Let the engine drive a GUI's input.
 *
 * The C equivalent is two lines (Grapple_GuiEventSink then
 * Grapple_EngineSetEventSink), but a sink is a struct of function pointers
 * and there is no sane way to hand one across a script boundary — so
 * scripts get the pair as a single call. Passing nil for the gui detaches.
 */
static int LAttachGui(lua_State *L)
{
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    if (lua_isnoneornil(L, 2))
    {
        Grapple_EngineSetEventSink(engine, NULL);
        lua_pushboolean(L, 1);
        return 1;
    }
    Grapple_Gui *gui = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_Gui");
    luaL_argcheck(L, gui != NULL, 2, "gui expected");
    const Grapple_EventSink sink = Grapple_GuiEventSink(gui);
    Grapple_EngineSetEventSink(engine, &sink);
    lua_pushboolean(L, 1);
    return 1;
}

/* Hand the loop over. Handlers fire from EngineTick too, so a script that
   would rather own the `while` simply does not call this. */
static int LRun(lua_State *L)
{
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    lua_pushboolean(L, Grapple_ScriptRun(engine));
    return 1;
}

/* --- scenes ---------------------------------------------------------------- */

/* A scene callback carries more than a game hook does: which scene, and for
   the event hook the event itself. Both cross as borrowed handles — the
   engine owns the scene, and the event belongs to the frame — so a script
   that stashes one and uses it later has a dangling pointer, exactly as it
   would in C. */
static bool DispatchSceneLua(void *language_state, Sint64 handle, Grapple_SceneHook hook,
                             Grapple_Scene *scene, float value, const SDL_Event *event)
{
    lua_State *L = (lua_State *)language_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, (lua_Integer)handle);
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);
        return true;
    }

    /* Every scene callback is given its scene, so one definition can back
       several live scenes and a script can tell them apart — a table keyed
       by the scene is how a script does what Grapple_SceneState does for
       C. */
    GrappleGen_LuaPushHandle(L, scene, "Grapple_Scene");
    int args = 1;
    if (hook == GRAPPLE_SCENE_HOOK_FIXED_UPDATE || hook == GRAPPLE_SCENE_HOOK_UPDATE ||
        hook == GRAPPLE_SCENE_HOOK_RENDER)
    {
        lua_pushnumber(L, (lua_Number)value);
        args = 2;
    }
    else if (hook == GRAPPLE_SCENE_HOOK_EVENT)
    {
        /* const is dropped here because the bindings have one handle type
           per C type; a script cannot write through it usefully anyway,
           since the setters it would need take a builder's event. */
        GrappleGen_LuaPushHandle(L, (void *)(uintptr_t)event, "SDL_Event");
        args = 2;
    }

    if (lua_pcall(L, args, 1, 0) != LUA_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "scene hook: %s",
                     lua_tostring(L, -1) ? lua_tostring(L, -1) : "error");
        lua_pop(L, 1);
        return true;
    }

    /* Only `load` reads a result, where false aborts the push. */
    bool ok = true;
    if (hook == GRAPPLE_SCENE_HOOK_LOAD && !lua_isnil(L, -1))
    {
        ok = lua_toboolean(L, -1) != 0;
    }
    lua_pop(L, 1);
    return ok;
}

/* Hook names rather than numbers, because a script writing
   GrappleC.SceneOn(e, "level", 4, fn) is unreadable and a typo in it is
   silent. A wrong name here is an error naming the ones that exist. */
static const struct
{
    const char *name;
    Grapple_SceneHook hook;
} kSceneHooks[] = {
    {"load", GRAPPLE_SCENE_HOOK_LOAD},
    {"enter", GRAPPLE_SCENE_HOOK_ENTER},
    {"fixed_update", GRAPPLE_SCENE_HOOK_FIXED_UPDATE},
    {"update", GRAPPLE_SCENE_HOOK_UPDATE},
    {"render", GRAPPLE_SCENE_HOOK_RENDER},
    {"event", GRAPPLE_SCENE_HOOK_EVENT},
    {"exit", GRAPPLE_SCENE_HOOK_EXIT},
    {"unload", GRAPPLE_SCENE_HOOK_UNLOAD},
};

/* GrappleC.SceneDefine(engine, "level") */
static int LSceneDefine(lua_State *L)
{
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    const char *name = luaL_checkstring(L, 2);

    /* Defining a scene binds the language too, so a script that uses only
       scenes never has to know the game hooks exist. */
    if (!Grapple_ScriptBind(engine, L, DispatchLua, ReleaseLua) ||
        !Grapple_ScriptSetSceneDispatch(engine, DispatchSceneLua))
    {
        return luaL_error(L, "could not bind Lua to this engine");
    }
    if (!Grapple_ScriptSceneDefine(engine, name))
    {
        return luaL_error(L, "%s", SDL_GetError());
    }
    lua_pushboolean(L, 1);
    return 1;
}

/* GrappleC.SceneOn(engine, "level", "render", function(scene, alpha) ... end) */
static int LSceneOn(lua_State *L)
{
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    const char *name = luaL_checkstring(L, 2);
    const char *hook_name = luaL_checkstring(L, 3);
    luaL_checktype(L, 4, LUA_TFUNCTION);

    Grapple_SceneHook hook = GRAPPLE_SCENE_HOOK_COUNT;
    for (size_t i = 0; i < SDL_arraysize(kSceneHooks); ++i)
    {
        if (SDL_strcmp(kSceneHooks[i].name, hook_name) == 0)
        {
            hook = kSceneHooks[i].hook;
            break;
        }
    }
    if (hook == GRAPPLE_SCENE_HOOK_COUNT)
    {
        return luaL_error(L, "unknown scene hook '%s': expected load, enter, fixed_update, "
                             "update, render, event, exit or unload",
                          hook_name);
    }

    lua_pushvalue(L, 4);
    const int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (!Grapple_ScriptSceneSetHook(engine, name, hook, (Sint64)ref))
    {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return luaL_error(L, "%s", SDL_GetError());
    }
    lua_pushboolean(L, 1);
    return 1;
}

/* Installed by the opener below rather than exported: scenes and hooks
   are one surface to a script, so there is one entry point. */
static bool OpenLuaSceneHooks(lua_State *L)
{
    static const luaL_Reg scenes[] = {
        {"SceneDefine", LSceneDefine}, {"SceneOn", LSceneOn}, {NULL, NULL}};
    if (L == NULL)
    {
        return false;
    }
    lua_getglobal(L, "GrappleC");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    luaL_setfuncs(L, scenes, 0);
    lua_pop(L, 1);
    return true;
}

bool Grapple_OpenLuaEngineHooks(lua_State *L)
{
    static const luaL_Reg hooks[] = {{"OnLoad", LOnLoad},
                                     {"OnFixedUpdate", LOnFixedUpdate},
                                     {"OnUpdate", LOnUpdate},
                                     {"OnRender", LOnRender},
                                     {"OnPostRender", LOnPostRender},
                                     {"OnEvent", LOnEvent},
                                     {"OnResize", LOnResize},
                                     {"OnUnload", LOnUnload},
                                     {"AttachGui", LAttachGui},
                                     {"Run", LRun},
                                     {NULL, NULL}};
    if (L == NULL)
    {
        return false;
    }
    /* Added to the generated module rather than a new one, so a script sees
       one namespace for the engine however each piece was produced. */
    lua_getglobal(L, "GrappleC");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    luaL_setfuncs(L, hooks, 0);
    lua_pop(L, 1);
    return OpenLuaSceneHooks(L);
}
