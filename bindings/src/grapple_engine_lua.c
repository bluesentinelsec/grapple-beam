/*
 * grapple_engine_lua.c — one table instead of a run of setters.
 *
 * Original Grapple code (zlib).
 *
 *   local engine = Grapple.engine{
 *     title = "My Game",
 *     window = { width = 1280, height = 720 },
 *     presentation = "letterbox",
 *     tick_rate = 120,
 *   }
 *
 * The setter surface (GrappleC.ConfigCreate and its two dozen
 * ConfigSet* functions) still exists and still works. What it could not do
 * is fail usefully: a misspelled ConfigSetTitel is a nil call, and nothing
 * in a sequence of setters says which order they wanted to be called in.
 * A table has no order to get wrong, and an unrecognised key can say so by
 * name — which is most of the value here.
 */
#include <grapple/bindings.h>
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/engine_script.h>

#include "gen_support_lua.h"

#include <lauxlib.h>
#include <lua.h>

#include <SDL3/SDL.h>

#define ENGINE_MT "grapple.engine"

typedef struct EngineBox
{
    Grapple_Engine *engine;
    bool owned;
} EngineBox;

/* Every key this constructor understands. Listed once, used both to build
   the config and to reject anything that is not on it: a table that silently
   ignores what it does not recognise is worse than the setters it replaces. */
static const char *const kEngineKeys[] = {
    "title",     "window",     "design",   "presentation", "resizable",
    "high_dpi",  "fullscreen", "vsync",    "max_fps",      "tick_rate",
    "auto_mount", "headless",  "media",    "font_size",    "backend",
};

static int Reject(lua_State *L, const char *key)
{
    /* A near-miss is the common case, so say the closest legal key rather
       than printing the whole list. */
    const char *closest = NULL;
    size_t best = (size_t)-1;
    for (size_t i = 0; i < SDL_arraysize(kEngineKeys); ++i)
    {
        const char *candidate = kEngineKeys[i];
        size_t distance = 0;
        const size_t a = SDL_strlen(key);
        const size_t b = SDL_strlen(candidate);
        distance = (a > b) ? (a - b) : (b - a);
        for (size_t c = 0; c < ((a < b) ? a : b); ++c)
        {
            if (key[c] != candidate[c])
            {
                distance++;
            }
        }
        if (distance < best)
        {
            best = distance;
            closest = candidate;
        }
    }
    if (closest != NULL && best <= 3)
    {
        return luaL_error(L, "unknown engine option '%s' — did you mean '%s'?", key, closest);
    }
    return luaL_error(L, "unknown engine option '%s'", key);
}

static void CheckKeys(lua_State *L, int table)
{
    lua_pushnil(L);
    while (lua_next(L, table) != 0)
    {
        if (lua_type(L, -2) == LUA_TSTRING)
        {
            const char *key = lua_tostring(L, -2);
            bool known = false;
            for (size_t i = 0; i < SDL_arraysize(kEngineKeys) && !known; ++i)
            {
                known = SDL_strcmp(key, kEngineKeys[i]) == 0;
            }
            if (!known)
            {
                lua_pop(L, 2);
                Reject(L, key);
                return;
            }
        }
        lua_pop(L, 1);
    }
}

static const char *OptString(lua_State *L, int table, const char *key, const char *fallback)
{
    lua_getfield(L, table, key);
    const char *value = lua_isstring(L, -1) ? lua_tostring(L, -1) : fallback;
    lua_pop(L, 1);
    return value;
}

static bool OptBool(lua_State *L, int table, const char *key, bool fallback)
{
    lua_getfield(L, table, key);
    const bool value = lua_isnil(L, -1) ? fallback : (lua_toboolean(L, -1) != 0);
    lua_pop(L, 1);
    return value;
}

static int OptInt(lua_State *L, int table, const char *key, int fallback)
{
    lua_getfield(L, table, key);
    const int value = lua_isnumber(L, -1) ? (int)lua_tointeger(L, -1) : fallback;
    lua_pop(L, 1);
    return value;
}

/* `window = { width = 640, height = 400 }`, or `window = { 640, 400 }`. */
static void OptSize(lua_State *L, int table, const char *key, int *width, int *height)
{
    lua_getfield(L, table, key);
    if (lua_istable(L, -1))
    {
        const int nested = lua_gettop(L);
        *width = OptInt(L, nested, "width", *width);
        *height = OptInt(L, nested, "height", *height);
        lua_rawgeti(L, nested, 1);
        if (lua_isnumber(L, -1))
        {
            *width = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
        lua_rawgeti(L, nested, 2);
        if (lua_isnumber(L, -1))
        {
            *height = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static bool ParsePresentation(const char *name, Grapple_EnginePresentation *out)
{
    static const struct
    {
        const char *name;
        Grapple_EnginePresentation value;
    } kModes[] = {
        {"letterbox", GRAPPLE_PRESENT_LETTERBOX}, {"expand", GRAPPLE_PRESENT_EXPAND},
        {"overscan", GRAPPLE_PRESENT_OVERSCAN},   {"integer", GRAPPLE_PRESENT_INTEGER},
        {"stretch", GRAPPLE_PRESENT_STRETCH},     {"native", GRAPPLE_PRESENT_NATIVE},
    };
    for (size_t i = 0; i < SDL_arraysize(kModes); ++i)
    {
        if (SDL_strcasecmp(name, kModes[i].name) == 0)
        {
            *out = kModes[i].value;
            return true;
        }
    }
    return false;
}

Grapple_Engine *Grapple_LuaEngineAt(lua_State *L, int index)
{
    /* Either spelling: the object this file makes, or the generated handle
       every GrappleC.* function takes. */
    if (luaL_testudata(L, index, ENGINE_MT) != NULL)
    {
        EngineBox *box = (EngineBox *)lua_touserdata(L, index);
        return box->engine;
    }
    return (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, index, "Grapple_Engine");
}

static Grapple_Engine *CheckEngine(lua_State *L)
{
    EngineBox *box = (EngineBox *)luaL_checkudata(L, 1, ENGINE_MT);
    luaL_argcheck(L, box->engine != NULL, 1, "engine expected");
    return box->engine;
}

static int LEngineNew(lua_State *L)
{
    if (lua_gettop(L) < 1)
    {
        lua_settop(L, 1);
    }
    if (lua_isnoneornil(L, 1))
    {
        lua_newtable(L);
        lua_replace(L, 1);
    }
    luaL_checktype(L, 1, LUA_TTABLE);
    CheckKeys(L, 1);

    Grapple_EngineConfig config = {0};
    config.title = OptString(L, 1, "title", NULL);

    int window_w = 0;
    int window_h = 0;
    OptSize(L, 1, "window", &window_w, &window_h);
    config.window_width = window_w;
    config.window_height = window_h;

    int design_w = 0;
    int design_h = 0;
    OptSize(L, 1, "design", &design_w, &design_h);
    config.design_width = design_w;
    config.design_height = design_h;

    const char *presentation = OptString(L, 1, "presentation", NULL);
    if (presentation != NULL && !ParsePresentation(presentation, &config.presentation))
    {
        return luaL_error(L, "unknown presentation '%s' (letterbox, expand, overscan, "
                             "integer, stretch, native)",
                          presentation);
    }

    /* The C struct spells the defaults as the thing you turn off, which is
       right for a struct and wrong for a table: nobody writes no_vsync =
       false. The table takes the positive form and inverts it here. */
    config.fixed_size = !OptBool(L, 1, "resizable", true);
    config.low_dpi = !OptBool(L, 1, "high_dpi", true);
    config.no_vsync = !OptBool(L, 1, "vsync", true);
    config.no_auto_mount = !OptBool(L, 1, "auto_mount", true);
    config.fullscreen = OptBool(L, 1, "fullscreen", false);
    config.headless = OptBool(L, 1, "headless", false);
    config.max_fps = OptInt(L, 1, "max_fps", 0);
    config.tick_rate = OptInt(L, 1, "tick_rate", 0);
    config.media_path = OptString(L, 1, "media", NULL);

    Grapple_Engine *engine = Grapple_CreateEngine(&config);
    if (engine == NULL)
    {
        return luaL_error(L, "%s", SDL_GetError());
    }

    EngineBox *box = (EngineBox *)lua_newuserdata(L, sizeof(*box));
    box->engine = engine;
    box->owned = true;
    luaL_getmetatable(L, ENGINE_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int SetHook(lua_State *L, Grapple_ScriptHook hook)
{
    Grapple_Engine *engine = CheckEngine(L);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    if (!Grapple_LuaBindEngineHook(L, engine, hook, 2))
    {
        return luaL_error(L, "%s", SDL_GetError());
    }
    lua_pushvalue(L, 1); /* chainable */
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

static int LRun(lua_State *L)
{
    lua_pushboolean(L, Grapple_ScriptRun(CheckEngine(L)) ? 1 : 0);
    return 1;
}

static int LQuit(lua_State *L)
{
    Grapple_EngineQuit(CheckEngine(L));
    return 0;
}

/* Keys by name — "escape", "space", "a" — because a script that has to say
   SDL.SCANCODE_ESCAPE is carrying the C API's spelling for no reason. */
static SDL_Scancode ScancodeAt(lua_State *L, int index)
{
    if (lua_isnumber(L, index))
    {
        return (SDL_Scancode)lua_tointeger(L, index);
    }
    const char *name = luaL_checkstring(L, index);
    const SDL_Scancode code = SDL_GetScancodeFromName(name);
    if (code == SDL_SCANCODE_UNKNOWN)
    {
        luaL_error(L, "unknown key '%s'", name);
    }
    return code;
}

static int LKeyPressed(lua_State *L)
{
    Grapple_Engine *engine = CheckEngine(L);
    lua_pushboolean(L, Grapple_KeyPressed(engine, ScancodeAt(L, 2)) ? 1 : 0);
    return 1;
}

static int LKeyDown(lua_State *L)
{
    Grapple_Engine *engine = CheckEngine(L);
    lua_pushboolean(L, Grapple_KeyDown(engine, ScancodeAt(L, 2)) ? 1 : 0);
    return 1;
}

static int LSize(lua_State *L)
{
    int width = 0;
    int height = 0;
    Grapple_EnginePixelSize(CheckEngine(L), &width, &height);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    return 2;
}

static int LEngineGc(lua_State *L)
{
    EngineBox *box = (EngineBox *)luaL_checkudata(L, 1, ENGINE_MT);
    if (box->engine != NULL && box->owned)
    {
        Grapple_DestroyEngine(box->engine);
    }
    box->engine = NULL;
    return 0;
}

bool Grapple_OpenLuaEngine(lua_State *L)
{
    static const luaL_Reg methods[] = {
        {"on_load", LOnLoad},
        {"on_fixed_update", LOnFixedUpdate},
        {"on_update", LOnUpdate},
        {"on_render", LOnRender},
        {"on_post_render", LOnPostRender},
        {"on_event", LOnEvent},
        {"on_resize", LOnResize},
        {"on_unload", LOnUnload},
        {"run", LRun},
        {"quit", LQuit},
        {"key_pressed", LKeyPressed},
        {"key_down", LKeyDown},
        {"size", LSize},
        {NULL, NULL},
    };

    if (L == NULL)
    {
        return SDL_InvalidParamError("L");
    }
    luaL_newmetatable(L, ENGINE_MT);
    lua_pushcfunction(L, LEngineGc);
    lua_setfield(L, -2, "__gc");
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    lua_getglobal(L, "Grapple");
    if (lua_istable(L, -1))
    {
        lua_pushcfunction(L, LEngineNew);
        lua_setfield(L, -2, "engine");
    }
    lua_pop(L, 1);
    return true;
}
