/*
 * grapple_platformer_lua.c — the platformer level and player as Lua objects.
 *
 * Original Grapple code (zlib).
 *
 *   local level = Grapple.create_level(engine, { width = 212, height = 15 })
 *   level:create_floor{ x = 0, y = 13, width = 69 }
 *   level:create_wall{ x = 28, y = 9, height = 4 }
 *   local mario = level:create_player{ x = 3, y = 12 }
 *
 * That is a complete game: the level attaches itself to the engine, so the
 * script has no loop to write, and the runner starts it once the file has
 * finished. Objects take a table of named arguments, because a call like
 * create_floor(0, 13, 69) is three numbers whose order is a thing to look
 * up, and a missing or misspelled name can be reported by name.
 *
 * The generated GrappleC.Platformer* surface underneath is unchanged and
 * still available; this file is the table-and-method spelling of it.
 */
#include "gen_support_lua.h"

#include <SDL3/SDL.h>
#include <grapple/bindings.h>
#include <grapple/platformer.h>
#include <lauxlib.h>
#include <lua.h>

#define LEVEL_MT "grapple.platformer.level"
#define PLAYER_MT "grapple.platformer.player"

typedef struct LevelBox
{
    Grapple_Platformer *level;
} LevelBox;

/* The player object keeps its level userdata as a user value, so a player
   held past its level does not dangle: it finds a NULL and says so. */
typedef struct PlayerBox
{
    int unused;
} PlayerBox;

static int Fail(lua_State *L)
{
    return luaL_error(L, "%s", SDL_GetError());
}

static Grapple_Platformer *CheckLevel(lua_State *L, int index)
{
    LevelBox *box = (LevelBox *)luaL_checkudata(L, index, LEVEL_MT);
    if (box->level == NULL)
    {
        luaL_error(L, "the level has been destroyed");
    }
    return box->level;
}

static Grapple_Platformer *CheckPlayer(lua_State *L, int index)
{
    luaL_checkudata(L, index, PLAYER_MT);
    lua_getiuservalue(L, index, 1);
    LevelBox *box = (LevelBox *)luaL_testudata(L, -1, LEVEL_MT);
    lua_pop(L, 1);
    if (box == NULL || box->level == NULL)
    {
        luaL_error(L, "the player's level has been destroyed");
        return NULL;
    }
    if (!Grapple_PlatformerHasPlayer(box->level))
    {
        luaL_error(L, "the level has no player");
    }
    return box->level;
}

/* --- option reading ------------------------------------------------------- */

static void CheckKeys(lua_State *L, int table, const char *const *keys, size_t count,
                      const char *what)
{
    if (!lua_istable(L, table))
    {
        return;
    }
    lua_pushnil(L);
    while (lua_next(L, table) != 0)
    {
        if (lua_type(L, -2) == LUA_TSTRING)
        {
            const char *key = lua_tostring(L, -2);
            bool known = false;
            for (size_t i = 0; i < count && !known; ++i)
            {
                known = SDL_strcmp(key, keys[i]) == 0;
            }
            if (!known)
            {
                lua_pop(L, 2);
                luaL_error(L, "unknown %s option '%s'", what, key);
                return;
            }
        }
        lua_pop(L, 1);
    }
}

static lua_Number ReqNumber(lua_State *L, int table, const char *key, const char *what)
{
    lua_getfield(L, table, key);
    if (!lua_isnumber(L, -1))
    {
        lua_pop(L, 1);
        luaL_error(L, "%s needs a numeric '%s'", what, key);
        return 0;
    }
    const lua_Number value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

static lua_Number OptNumber(lua_State *L, int table, const char *key, lua_Number fallback)
{
    lua_getfield(L, table, key);
    const lua_Number value = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : fallback;
    lua_pop(L, 1);
    return value;
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

/* A colour table: { r, g, b [, a] } or { r = , g = , b = [, a = ] }, in
   0-255 or 0-1 — anything above 1 means the whole table is bytes. */
static bool ReadColor(lua_State *L, int index, SDL_FColor *out)
{
    if (!lua_istable(L, index))
    {
        return false;
    }
    float c[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    static const char *const names[] = {"r", "g", "b", "a"};
    for (int i = 0; i < 4; ++i)
    {
        lua_rawgeti(L, index, i + 1);
        if (!lua_isnumber(L, -1))
        {
            lua_pop(L, 1);
            lua_getfield(L, index, names[i]);
        }
        if (lua_isnumber(L, -1))
        {
            c[i] = (float)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    bool bytes = false;
    for (int i = 0; i < 4; ++i)
    {
        bytes = bytes || c[i] > 1.0f;
    }
    if (bytes)
    {
        for (int i = 0; i < 4; ++i)
        {
            c[i] /= 255.0f;
        }
        if (c[3] > 1.0f)
        {
            c[3] = 1.0f;
        }
    }
    *out = (SDL_FColor){c[0], c[1], c[2], c[3]};
    return true;
}

static bool ParseTile(const char *name, Grapple_PlatformerTile *out)
{
    static const struct
    {
        const char *name;
        Grapple_PlatformerTile value;
    } kKinds[] = {
        {"empty", GRAPPLE_PLATFORMER_EMPTY},
        {"solid", GRAPPLE_PLATFORMER_SOLID},
        {"one_way", GRAPPLE_PLATFORMER_ONE_WAY},
    };
    for (size_t i = 0; i < SDL_arraysize(kKinds); ++i)
    {
        if (SDL_strcasecmp(name, kKinds[i].name) == 0)
        {
            *out = kKinds[i].value;
            return true;
        }
    }
    return false;
}

static const char *TileName(Grapple_PlatformerTile tile)
{
    switch (tile)
    {
    case GRAPPLE_PLATFORMER_SOLID:
        return "solid";
    case GRAPPLE_PLATFORMER_ONE_WAY:
        return "one_way";
    case GRAPPLE_PLATFORMER_EMPTY:
    default:
        return "empty";
    }
}

static Grapple_PlatformerTile CheckTile(lua_State *L, int index)
{
    Grapple_PlatformerTile tile;
    const char *name = luaL_checkstring(L, index);
    if (!ParseTile(name, &tile))
    {
        luaL_error(L, "unknown tile kind '%s' (empty, solid, one_way)", name);
    }
    return tile;
}

static bool ParseScroll(const char *name, Grapple_PlatformerScroll *out)
{
    if (SDL_strcasecmp(name, "free") == 0)
    {
        *out = GRAPPLE_PLATFORMER_SCROLL_FREE;
        return true;
    }
    if (SDL_strcasecmp(name, "forward") == 0)
    {
        *out = GRAPPLE_PLATFORMER_SCROLL_FORWARD;
        return true;
    }
    return false;
}

/* --- constructors --------------------------------------------------------- */

static const char *const kLevelKeys[] = {
    "width", "height", "tile", "scroll", "attach", "background", "camera_smoothing",
};

/* Options shared by create_level and load_level, read from the table at
   `table` onto a level that already exists. */
static void ApplyLevelOptions(lua_State *L, int table, Grapple_Platformer *level)
{
    if (!lua_istable(L, table))
    {
        Grapple_PlatformerAttach(level);
        return;
    }
    const char *scroll = OptString(L, table, "scroll", NULL);
    if (scroll != NULL)
    {
        Grapple_PlatformerScroll mode;
        if (!ParseScroll(scroll, &mode))
        {
            luaL_error(L, "unknown scroll mode '%s' (free, forward)", scroll);
        }
        Grapple_PlatformerSetScroll(level, mode);
    }
    lua_getfield(L, table, "background");
    SDL_FColor color;
    if (ReadColor(L, lua_gettop(L), &color))
    {
        Grapple_PlatformerSetBackgroundColor(level, color.r, color.g, color.b, color.a);
    }
    lua_pop(L, 1);
    lua_getfield(L, table, "camera_smoothing");
    if (lua_isnumber(L, -1))
    {
        Grapple_PlatformerSetCameraSmoothing(level, (float)lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    if (OptBool(L, table, "attach", true) && !Grapple_PlatformerAttach(level))
    {
        Grapple_DestroyPlatformer(level);
        Fail(L);
    }
}

static int PushLevel(lua_State *L, Grapple_Platformer *level)
{
    LevelBox *box = (LevelBox *)lua_newuserdatauv(L, sizeof(*box), 0);
    box->level = level;
    luaL_getmetatable(L, LEVEL_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int LCreateLevel(lua_State *L)
{
    Grapple_Engine *engine = Grapple_LuaEngineAt(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, kLevelKeys, SDL_arraysize(kLevelKeys), "level");
    const int width = (int)ReqNumber(L, 2, "width", "create_level");
    const int height = (int)ReqNumber(L, 2, "height", "create_level");
    const int tile = (int)OptNumber(L, 2, "tile", 16);
    Grapple_Platformer *level = Grapple_CreatePlatformer(engine, width, height, tile);
    if (level == NULL)
    {
        return Fail(L);
    }
    ApplyLevelOptions(L, 2, level);
    return PushLevel(L, level);
}

static int LLoadLevel(lua_State *L)
{
    Grapple_Engine *engine = Grapple_LuaEngineAt(L, 1);
    const char *path = luaL_checkstring(L, 2);
    if (!lua_isnoneornil(L, 3))
    {
        luaL_checktype(L, 3, LUA_TTABLE);
        CheckKeys(L, 3, kLevelKeys, SDL_arraysize(kLevelKeys), "level");
    }
    Grapple_Platformer *level = Grapple_LoadPlatformer(engine, path);
    if (level == NULL)
    {
        return Fail(L);
    }
    ApplyLevelOptions(L, 3, level);
    return PushLevel(L, level);
}

static int LLevelGc(lua_State *L)
{
    LevelBox *box = (LevelBox *)luaL_checkudata(L, 1, LEVEL_MT);
    if (box->level != NULL)
    {
        Grapple_DestroyPlatformer(box->level);
        box->level = NULL;
    }
    return 0;
}

/* --- level: cells and objects -------------------------------------------- */

static int LFloor(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "width", "depth"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "create_floor");
    lua_pushboolean(L,
                    Grapple_PlatformerCreateFloor(level, (int)ReqNumber(L, 2, "x", "create_floor"),
                                                  (int)ReqNumber(L, 2, "y", "create_floor"),
                                                  (int)ReqNumber(L, 2, "width", "create_floor"),
                                                  (int)OptNumber(L, 2, "depth", 0)));
    return 1;
}

static int LWall(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "height"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "create_wall");
    lua_pushboolean(L, Grapple_PlatformerCreateWall(level, (int)ReqNumber(L, 2, "x", "create_wall"),
                                                    (int)ReqNumber(L, 2, "y", "create_wall"),
                                                    (int)ReqNumber(L, 2, "height", "create_wall")));
    return 1;
}

static int LBlock(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "width", "height"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "create_block");
    lua_pushboolean(L, Grapple_PlatformerCreateBlock(
                           level, (int)ReqNumber(L, 2, "x", "create_block"),
                           (int)ReqNumber(L, 2, "y", "create_block"),
                           (int)OptNumber(L, 2, "width", 1), (int)OptNumber(L, 2, "height", 1)));
    return 1;
}

static int LStairs(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "steps", "direction"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "create_stairs");
    const char *direction = OptString(L, 2, "direction", "up");
    bool rising;
    if (SDL_strcasecmp(direction, "up") == 0)
    {
        rising = true;
    }
    else if (SDL_strcasecmp(direction, "down") == 0)
    {
        rising = false;
    }
    else
    {
        return luaL_error(L, "unknown stairs direction '%s' (up, down)", direction);
    }
    lua_pushboolean(
        L, Grapple_PlatformerCreateStairs(level, (int)ReqNumber(L, 2, "x", "create_stairs"),
                                          (int)ReqNumber(L, 2, "y", "create_stairs"),
                                          (int)ReqNumber(L, 2, "steps", "create_stairs"), rising));
    return 1;
}

static int LPlatform(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "width"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "create_platform");
    lua_pushboolean(
        L, Grapple_PlatformerCreatePlatform(level, (int)ReqNumber(L, 2, "x", "create_platform"),
                                            (int)ReqNumber(L, 2, "y", "create_platform"),
                                            (int)ReqNumber(L, 2, "width", "create_platform")));
    return 1;
}

static int LSolid(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "w", "h", "width", "height"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "solid");
    const float w = (float)OptNumber(L, 2, "w", OptNumber(L, 2, "width", 0));
    const float h = (float)OptNumber(L, 2, "h", OptNumber(L, 2, "height", 0));
    const int id = Grapple_PlatformerAddSolid(level, (float)ReqNumber(L, 2, "x", "solid"),
                                              (float)ReqNumber(L, 2, "y", "solid"), w, h);
    if (id < 0)
    {
        return Fail(L);
    }
    lua_pushinteger(L, id);
    return 1;
}

static int LRemoveSolid(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    lua_pushboolean(L, Grapple_PlatformerRemoveSolid(level, (int)luaL_checkinteger(L, 2)));
    return 1;
}

static int LFill(lua_State *L)
{
    static const char *const keys[] = {"x", "y", "width", "height", "kind"};
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    CheckKeys(L, 2, keys, SDL_arraysize(keys), "fill");
    Grapple_PlatformerTile tile = GRAPPLE_PLATFORMER_SOLID;
    const char *kind = OptString(L, 2, "kind", "solid");
    if (!ParseTile(kind, &tile))
    {
        return luaL_error(L, "unknown tile kind '%s' (empty, solid, one_way)", kind);
    }
    lua_pushinteger(L, Grapple_PlatformerFillTiles(level, (int)ReqNumber(L, 2, "x", "fill"),
                                                   (int)ReqNumber(L, 2, "y", "fill"),
                                                   (int)ReqNumber(L, 2, "width", "fill"),
                                                   (int)ReqNumber(L, 2, "height", "fill"), tile));
    return 1;
}

static int LSetTile(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    if (!Grapple_PlatformerSetTile(level, (int)luaL_checkinteger(L, 2),
                                   (int)luaL_checkinteger(L, 3), CheckTile(L, 4)))
    {
        return Fail(L);
    }
    return 0;
}

static int LTileAt(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    lua_pushstring(L, TileName(Grapple_PlatformerTileAt(level, (int)luaL_checkinteger(L, 2),
                                                        (int)luaL_checkinteger(L, 3))));
    return 1;
}

static int LSize(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    lua_pushinteger(L, Grapple_PlatformerWidth(level));
    lua_pushinteger(L, Grapple_PlatformerHeight(level));
    lua_pushinteger(L, Grapple_PlatformerTileSize(level));
    return 3;
}

static int LPixelSize(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPixelSize(level, &w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}

/* --- level: look and camera --------------------------------------------- */

static int LBackground(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    SDL_FColor color;
    if (!ReadColor(L, 2, &color))
    {
        return luaL_error(L, "background needs a colour table { r, g, b }");
    }
    Grapple_PlatformerSetBackgroundColor(level, color.r, color.g, color.b, color.a);
    return 0;
}

static int LTileColor(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    const Grapple_PlatformerTile tile = CheckTile(L, 2);
    SDL_FColor color;
    if (!ReadColor(L, 3, &color))
    {
        return luaL_error(L, "tile_color needs a colour table { r, g, b }");
    }
    Grapple_PlatformerSetTileColor(level, tile, color.r, color.g, color.b, color.a);
    return 0;
}

static int LSetScroll(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    const char *name = luaL_checkstring(L, 2);
    Grapple_PlatformerScroll mode;
    if (!ParseScroll(name, &mode))
    {
        return luaL_error(L, "unknown scroll mode '%s' (free, forward)", name);
    }
    Grapple_PlatformerSetScroll(level, mode);
    return 0;
}

static int LScroll(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    lua_pushstring(L, Grapple_PlatformerScrollMode(level) == GRAPPLE_PLATFORMER_SCROLL_FORWARD
                          ? "forward"
                          : "free");
    return 1;
}

static int LCameraSmoothing(lua_State *L)
{
    Grapple_PlatformerSetCameraSmoothing(CheckLevel(L, 1), (float)luaL_checknumber(L, 2));
    return 0;
}

static int LCameraPosition(lua_State *L)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerCameraPosition(CheckLevel(L, 1), &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

/* --- level: the loop ------------------------------------------------------ */

static int LAttach(lua_State *L)
{
    if (!Grapple_PlatformerAttach(CheckLevel(L, 1)))
    {
        return Fail(L);
    }
    lua_pushvalue(L, 1);
    return 1;
}

static int LDetach(lua_State *L)
{
    Grapple_PlatformerDetach(CheckLevel(L, 1));
    return 0;
}

static int LAttached(lua_State *L)
{
    lua_pushboolean(L, Grapple_PlatformerAttached(CheckLevel(L, 1)));
    return 1;
}

static int LStep(lua_State *L)
{
    Grapple_PlatformerStep(CheckLevel(L, 1), (float)luaL_checknumber(L, 2));
    return 0;
}

static int LUpdate(lua_State *L)
{
    Grapple_PlatformerUpdate(CheckLevel(L, 1), (float)luaL_checknumber(L, 2));
    return 0;
}

static int LRender(lua_State *L)
{
    Grapple_PlatformerRender(CheckLevel(L, 1), (float)luaL_optnumber(L, 2, 1.0));
    return 0;
}

static int LDestroy(lua_State *L)
{
    return LLevelGc(L);
}

/* --- level: input ---------------------------------------------------------- */

static int LBind(lua_State *L)
{
    if (!Grapple_PlatformerBind(CheckLevel(L, 1), luaL_checkstring(L, 2), luaL_checkstring(L, 3)))
    {
        return Fail(L);
    }
    return 0;
}

static int LActionDown(lua_State *L)
{
    lua_pushboolean(L, Grapple_PlatformerActionDown(CheckLevel(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

static int LActionPressed(lua_State *L)
{
    lua_pushboolean(L, Grapple_PlatformerActionPressed(CheckLevel(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

static int LActionReleased(lua_State *L)
{
    lua_pushboolean(L, Grapple_PlatformerActionReleased(CheckLevel(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

static int LActionValue(lua_State *L)
{
    lua_pushnumber(L, Grapple_PlatformerActionValue(CheckLevel(L, 1), luaL_checkstring(L, 2)));
    return 1;
}

static int LActions(lua_State *L)
{
    /* The generated handle, so the rest of the action-map API applies. */
    GrappleGen_LuaPushHandle(L, Grapple_PlatformerActions(CheckLevel(L, 1)), "Grapple_ActionMap");
    return 1;
}

/* --- the player ------------------------------------------------------------ */

static int PushPlayer(lua_State *L, int level_index)
{
    PlayerBox *box = (PlayerBox *)lua_newuserdatauv(L, sizeof(*box), 1);
    box->unused = 0;
    lua_pushvalue(L, level_index);
    lua_setiuservalue(L, -2, 1);
    luaL_getmetatable(L, PLAYER_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static const char *const kPlayerKeys[] = {"x", "y", "width", "height", "color"};

static bool IsTuningKey(const char *key)
{
    for (int i = 0; i < Grapple_PlatformerPlayerTuningCount(); ++i)
    {
        if (SDL_strcmp(Grapple_PlatformerPlayerTuningKey(i), key) == 0)
        {
            return true;
        }
    }
    return false;
}

static int LCreatePlayer(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    /* Layout keys, or any tuning key: everything else is a typo. */
    lua_pushnil(L);
    while (lua_next(L, 2) != 0)
    {
        if (lua_type(L, -2) == LUA_TSTRING)
        {
            const char *key = lua_tostring(L, -2);
            bool known = IsTuningKey(key);
            for (size_t i = 0; i < SDL_arraysize(kPlayerKeys) && !known; ++i)
            {
                known = SDL_strcmp(key, kPlayerKeys[i]) == 0;
            }
            if (!known)
            {
                lua_pop(L, 2);
                return luaL_error(L, "unknown player option '%s'", key);
            }
        }
        lua_pop(L, 1);
    }
    if (Grapple_PlatformerCreatePlayer(level, (int)ReqNumber(L, 2, "x", "create_player"),
                                       (int)ReqNumber(L, 2, "y", "create_player")) ==
        GRAPPLE_ACTOR_NONE)
    {
        return Fail(L);
    }
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPlayerSize(level, &w, &h);
    Grapple_PlatformerSetPlayerSize(level, (float)OptNumber(L, 2, "width", w),
                                    (float)OptNumber(L, 2, "height", h));
    lua_getfield(L, 2, "color");
    SDL_FColor color;
    if (ReadColor(L, lua_gettop(L), &color))
    {
        Grapple_PlatformerSetPlayerColor(level, color.r, color.g, color.b, color.a);
    }
    lua_pop(L, 1);
    lua_pushnil(L);
    while (lua_next(L, 2) != 0)
    {
        if (lua_type(L, -2) == LUA_TSTRING && lua_isnumber(L, -1))
        {
            const char *key = lua_tostring(L, -2);
            if (IsTuningKey(key))
            {
                Grapple_PlatformerSetPlayerTuning(level, key, (float)lua_tonumber(L, -1));
            }
        }
        lua_pop(L, 1);
    }
    return PushPlayer(L, 1);
}

static int LPlayer(lua_State *L)
{
    Grapple_Platformer *level = CheckLevel(L, 1);
    if (!Grapple_PlatformerHasPlayer(level))
    {
        lua_pushnil(L);
        return 1;
    }
    return PushPlayer(L, 1);
}

static int LRemovePlayer(lua_State *L)
{
    Grapple_PlatformerRemovePlayer(CheckLevel(L, 1));
    return 0;
}

static int LPState(lua_State *L)
{
    lua_pushstring(L, Grapple_PlatformerPlayerStateName(CheckPlayer(L, 1)));
    return 1;
}

#define PLAYER_BOOL(fn, call)                                                                      \
    static int fn(lua_State *L)                                                                    \
    {                                                                                              \
        lua_pushboolean(L, call(CheckPlayer(L, 1)));                                               \
        return 1;                                                                                  \
    }

PLAYER_BOOL(LPStateChanged, Grapple_PlatformerPlayerStateChanged)
PLAYER_BOOL(LPLanded, Grapple_PlatformerPlayerLanded)
PLAYER_BOOL(LPJumped, Grapple_PlatformerPlayerJumped)
PLAYER_BOOL(LPBumped, Grapple_PlatformerPlayerBumped)
PLAYER_BOOL(LPFell, Grapple_PlatformerPlayerFell)
PLAYER_BOOL(LPGrounded, Grapple_PlatformerPlayerGrounded)
PLAYER_BOOL(LPPaused, Grapple_PlatformerPlayerPaused)

static int LPFacing(lua_State *L)
{
    lua_pushinteger(L, Grapple_PlatformerPlayerFacing(CheckPlayer(L, 1)));
    return 1;
}

static int LPPosition(lua_State *L)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerPosition(CheckPlayer(L, 1), &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int LPVelocity(lua_State *L)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerVelocity(CheckPlayer(L, 1), &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int LPSize(lua_State *L)
{
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPlayerSize(CheckPlayer(L, 1), &w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}

static int LPSetSize(lua_State *L)
{
    Grapple_PlatformerSetPlayerSize(CheckPlayer(L, 1), (float)luaL_checknumber(L, 2),
                                    (float)luaL_checknumber(L, 3));
    return 0;
}

static int LPColor(lua_State *L)
{
    Grapple_Platformer *level = CheckPlayer(L, 1);
    SDL_FColor color;
    if (!ReadColor(L, 2, &color))
    {
        return luaL_error(L, "color needs a colour table { r, g, b }");
    }
    Grapple_PlatformerSetPlayerColor(level, color.r, color.g, color.b, color.a);
    return 0;
}

static int LPSet(lua_State *L)
{
    Grapple_Platformer *level = CheckPlayer(L, 1);
    if (lua_istable(L, 2))
    {
        lua_pushnil(L);
        while (lua_next(L, 2) != 0)
        {
            if (lua_type(L, -2) != LUA_TSTRING || !lua_isnumber(L, -1) ||
                !Grapple_PlatformerSetPlayerTuning(level, lua_tostring(L, -2),
                                                   (float)lua_tonumber(L, -1)))
            {
                lua_pop(L, 2);
                return Fail(L);
            }
            lua_pop(L, 1);
        }
        return 0;
    }
    if (!Grapple_PlatformerSetPlayerTuning(level, luaL_checkstring(L, 2),
                                           (float)luaL_checknumber(L, 3)))
    {
        return Fail(L);
    }
    return 0;
}

static int LPGet(lua_State *L)
{
    Grapple_Platformer *level = CheckPlayer(L, 1);
    const char *key = luaL_checkstring(L, 2);
    SDL_ClearError();
    const float value = Grapple_PlatformerPlayerTuning(level, key);
    if (value == 0.0f && SDL_GetError()[0] != '\0')
    {
        return Fail(L);
    }
    lua_pushnumber(L, value);
    return 1;
}

static int LPRespawn(lua_State *L)
{
    Grapple_Platformer *level = CheckPlayer(L, 1);
    if (lua_isnoneornil(L, 2))
    {
        Grapple_PlatformerPlayerRespawnAtStart(level);
    }
    else
    {
        Grapple_PlatformerPlayerRespawn(level, (float)luaL_checknumber(L, 2),
                                        (float)luaL_checknumber(L, 3));
    }
    return 0;
}

static int LPSetPaused(lua_State *L)
{
    Grapple_PlatformerSetPlayerPaused(CheckPlayer(L, 1), lua_toboolean(L, 2) != 0);
    return 0;
}

static int LPActor(lua_State *L)
{
    lua_pushinteger(L, (lua_Integer)Grapple_PlatformerPlayer(CheckPlayer(L, 1)));
    return 1;
}

static int LPScriptedInput(lua_State *L)
{
    Grapple_Platformer *level = CheckPlayer(L, 1);
    if (lua_isnoneornil(L, 2))
    {
        lua_pushboolean(L, Grapple_PlatformerScriptedInput(level));
        return 1;
    }
    Grapple_PlatformerSetScriptedInput(level, lua_toboolean(L, 2) != 0);
    return 0;
}

static int LPInput(lua_State *L)
{
    Grapple_PlatformerScriptInput(CheckPlayer(L, 1), (float)luaL_checknumber(L, 2),
                                  lua_toboolean(L, 3) != 0, lua_toboolean(L, 4) != 0);
    return 0;
}

/* --- registration ----------------------------------------------------------- */

bool Grapple_OpenLuaPlatformer(lua_State *L)
{
    static const luaL_Reg level_methods[] = {
        {"create_floor", LFloor},
        {"create_wall", LWall},
        {"create_block", LBlock},
        {"create_stairs", LStairs},
        {"create_platform", LPlatform},
        {"solid", LSolid},
        {"remove_solid", LRemoveSolid},
        {"fill", LFill},
        {"set_tile", LSetTile},
        {"tile_at", LTileAt},
        {"size", LSize},
        {"pixel_size", LPixelSize},
        {"background", LBackground},
        {"tile_color", LTileColor},
        {"set_scroll", LSetScroll},
        {"scroll", LScroll},
        {"camera_smoothing", LCameraSmoothing},
        {"camera_position", LCameraPosition},
        {"attach", LAttach},
        {"detach", LDetach},
        {"attached", LAttached},
        {"step", LStep},
        {"update", LUpdate},
        {"render", LRender},
        {"destroy", LDestroy},
        {"bind", LBind},
        {"action_down", LActionDown},
        {"action_pressed", LActionPressed},
        {"action_released", LActionReleased},
        {"action_value", LActionValue},
        {"actions", LActions},
        {"create_player", LCreatePlayer},
        {"player", LPlayer},
        {"remove_player", LRemovePlayer},
        {NULL, NULL},
    };
    static const luaL_Reg player_methods[] = {
        {"state", LPState},
        {"state_changed", LPStateChanged},
        {"landed", LPLanded},
        {"jumped", LPJumped},
        {"bumped", LPBumped},
        {"fell", LPFell},
        {"grounded", LPGrounded},
        {"facing", LPFacing},
        {"position", LPPosition},
        {"velocity", LPVelocity},
        {"size", LPSize},
        {"set_size", LPSetSize},
        {"color", LPColor},
        {"set", LPSet},
        {"get", LPGet},
        {"respawn", LPRespawn},
        {"set_paused", LPSetPaused},
        {"paused", LPPaused},
        {"actor", LPActor},
        {"scripted_input", LPScriptedInput},
        {"input", LPInput},
        {NULL, NULL},
    };

    if (L == NULL)
    {
        return SDL_InvalidParamError("L");
    }
    luaL_newmetatable(L, LEVEL_MT);
    lua_pushcfunction(L, LLevelGc);
    lua_setfield(L, -2, "__gc");
    lua_newtable(L);
    luaL_setfuncs(L, level_methods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    luaL_newmetatable(L, PLAYER_MT);
    lua_newtable(L);
    luaL_setfuncs(L, player_methods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    lua_getglobal(L, "Grapple");
    if (lua_istable(L, -1))
    {
        lua_pushcfunction(L, LCreateLevel);
        lua_setfield(L, -2, "create_level");
        lua_pushcfunction(L, LLoadLevel);
        lua_setfield(L, -2, "load_level");
    }
    lua_pop(L, 1);
    return true;
}
