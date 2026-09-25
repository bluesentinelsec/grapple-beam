/*
 * grapple_platformer_ruby.c — the platformer level and player as Ruby
 * objects. Original Grapple code (zlib).
 *
 *   level = Grapple.create_level(engine, width: 212, height: 15)
 *   level.create_floor(x: 0, y: 13, width: 69)
 *   level.create_wall(x: 28, y: 9, height: 4)
 *   mario = level.create_player(x: 3, y: 12)
 *
 * Keyword arguments where Lua takes a table, predicate methods where Lua
 * returns a boolean, otherwise the same surface — see grapple_platformer_lua.c
 * for what each call does.
 */
#include "gen_support_ruby.h"

#include <SDL3/SDL.h>
#include <grapple/bindings.h>
#include <grapple/platformer.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

static void LevelFree(mrb_state *mrb, void *ptr)
{
    (void)mrb;
    if (ptr != NULL)
    {
        Grapple_DestroyPlatformer((Grapple_Platformer *)ptr);
    }
}

static const mrb_data_type kLevelType = {"Grapple::Level", LevelFree};

static struct RClass *g_level_class;
static struct RClass *g_player_class;

static mrb_value Fail(mrb_state *mrb)
{
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%s", SDL_GetError());
    return mrb_nil_value();
}

static Grapple_Platformer *LevelOf(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = (Grapple_Platformer *)mrb_data_get_ptr(mrb, self, &kLevelType);
    if (level == NULL)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "the level has been destroyed");
    }
    return level;
}

/* A player keeps its level in @level, so it cannot outlive it silently. */
static Grapple_Platformer *PlayerLevel(mrb_state *mrb, mrb_value self)
{
    const mrb_value level_obj = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@level"));
    Grapple_Platformer *level = LevelOf(mrb, level_obj);
    if (!Grapple_PlatformerHasPlayer(level))
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "the level has no player");
    }
    return level;
}

/* --- option reading --------------------------------------------------------- */

static mrb_value Key(mrb_state *mrb, mrb_value options, const char *name)
{
    if (mrb_nil_p(options) || !mrb_hash_p(options))
    {
        return mrb_nil_value();
    }
    mrb_value value = mrb_hash_get(mrb, options, mrb_symbol_value(mrb_intern_cstr(mrb, name)));
    if (mrb_nil_p(value))
    {
        value = mrb_hash_get(mrb, options, mrb_str_new_cstr(mrb, name));
    }
    return value;
}

static bool IsNumber(mrb_value v)
{
    return mrb_fixnum_p(v) || mrb_float_p(v);
}

static float ReqNumber(mrb_state *mrb, mrb_value options, const char *name, const char *what)
{
    const mrb_value value = Key(mrb, options, name);
    if (!IsNumber(value))
    {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "%s needs a numeric '%s'", what, name);
    }
    return (float)mrb_as_float(mrb, value);
}

static float OptNumber(mrb_state *mrb, mrb_value options, const char *name, float fallback)
{
    const mrb_value value = Key(mrb, options, name);
    return IsNumber(value) ? (float)mrb_as_float(mrb, value) : fallback;
}

static const char *OptString(mrb_state *mrb, mrb_value options, const char *name,
                             const char *fallback)
{
    const mrb_value value = Key(mrb, options, name);
    if (mrb_string_p(value))
    {
        return mrb_str_to_cstr(mrb, value);
    }
    if (mrb_symbol_p(value))
    {
        return mrb_sym_name(mrb, mrb_symbol(value));
    }
    return fallback;
}

static bool OptBool(mrb_state *mrb, mrb_value options, const char *name, bool fallback)
{
    const mrb_value value = Key(mrb, options, name);
    return mrb_nil_p(value) ? fallback : mrb_test(value);
}

static const char *NameOf(mrb_state *mrb, mrb_value key)
{
    if (mrb_symbol_p(key))
    {
        return mrb_sym_name(mrb, mrb_symbol(key));
    }
    if (mrb_string_p(key))
    {
        return mrb_str_to_cstr(mrb, key);
    }
    return NULL;
}

static void CheckKeys(mrb_state *mrb, mrb_value options, const char *const *keys, size_t count,
                      const char *what)
{
    if (!mrb_hash_p(options))
    {
        return;
    }
    const mrb_value names = mrb_hash_keys(mrb, options);
    for (mrb_int i = 0; i < RARRAY_LEN(names); ++i)
    {
        const char *name = NameOf(mrb, mrb_ary_ref(mrb, names, i));
        if (name == NULL)
        {
            continue;
        }
        bool known = false;
        for (size_t k = 0; k < count && !known; ++k)
        {
            known = SDL_strcmp(name, keys[k]) == 0;
        }
        if (!known)
        {
            mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown %s option '%s'", what, name);
        }
    }
}

/* [r, g, b, a] or { r:, g:, b:, a: }, in 0-255 or 0-1. */
static bool ReadColor(mrb_state *mrb, mrb_value value, SDL_FColor *out)
{
    float c[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    static const char *const names[] = {"r", "g", "b", "a"};
    if (mrb_array_p(value))
    {
        for (mrb_int i = 0; i < 4 && i < RARRAY_LEN(value); ++i)
        {
            const mrb_value item = mrb_ary_ref(mrb, value, i);
            if (IsNumber(item))
            {
                c[i] = (float)mrb_as_float(mrb, item);
            }
        }
    }
    else if (mrb_hash_p(value))
    {
        for (int i = 0; i < 4; ++i)
        {
            c[i] = OptNumber(mrb, value, names[i], c[i]);
        }
    }
    else
    {
        return false;
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
    if (SDL_strcasecmp(name, "empty") == 0)
    {
        *out = GRAPPLE_PLATFORMER_EMPTY;
        return true;
    }
    if (SDL_strcasecmp(name, "solid") == 0)
    {
        *out = GRAPPLE_PLATFORMER_SOLID;
        return true;
    }
    if (SDL_strcasecmp(name, "one_way") == 0)
    {
        *out = GRAPPLE_PLATFORMER_ONE_WAY;
        return true;
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

static Grapple_PlatformerTile TileArg(mrb_state *mrb, mrb_value value)
{
    const char *name = NameOf(mrb, value);
    Grapple_PlatformerTile tile;
    if (name == NULL || !ParseTile(name, &tile))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "unknown tile kind (:empty, :solid, :one_way)");
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

/* --- constructors ------------------------------------------------------------ */

static const char *const kLevelKeys[] = {
    "width",
    "height",
    "tile",
    "scroll",
    "attach",
    "background",
    "camera_smoothing",
    "camera_look_ahead",
    "camera_deadzone",
};

static void ApplyLevelOptions(mrb_state *mrb, mrb_value options, Grapple_Platformer *level)
{
    const char *scroll = OptString(mrb, options, "scroll", NULL);
    if (scroll != NULL)
    {
        Grapple_PlatformerScroll mode;
        if (!ParseScroll(scroll, &mode))
        {
            Grapple_DestroyPlatformer(level);
            mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown scroll mode '%s' (free, forward)", scroll);
        }
        Grapple_PlatformerSetScroll(level, mode);
    }
    SDL_FColor color;
    if (ReadColor(mrb, Key(mrb, options, "background"), &color))
    {
        Grapple_PlatformerSetBackgroundColor(level, color.r, color.g, color.b, color.a);
    }
    const mrb_value smoothing = Key(mrb, options, "camera_smoothing");
    if (IsNumber(smoothing))
    {
        Grapple_PlatformerSetCameraSmoothing(level, (float)mrb_as_float(mrb, smoothing));
    }
    const mrb_value look = Key(mrb, options, "camera_look_ahead");
    if (IsNumber(look))
    {
        Grapple_PlatformerSetCameraLookAhead(level, (float)mrb_as_float(mrb, look));
    }
    const mrb_value deadzone = Key(mrb, options, "camera_deadzone");
    if (mrb_array_p(deadzone) && RARRAY_LEN(deadzone) >= 2)
    {
        Grapple_PlatformerSetCameraDeadzone(
            level, (float)mrb_as_float(mrb, mrb_ary_ref(mrb, deadzone, 0)),
            (float)mrb_as_float(mrb, mrb_ary_ref(mrb, deadzone, 1)));
    }
    if (OptBool(mrb, options, "attach", true) && !Grapple_PlatformerAttach(level))
    {
        Grapple_DestroyPlatformer(level);
        Fail(mrb);
    }
}

static mrb_value WrapLevel(mrb_state *mrb, Grapple_Platformer *level)
{
    struct RData *data = mrb_data_object_alloc(mrb, g_level_class, level, &kLevelType);
    return mrb_obj_value(data);
}

static mrb_value RCreateLevel(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "o|H", &engine_value, &options);
    Grapple_Engine *engine = Grapple_RubyEngineAt(mrb, engine_value);
    CheckKeys(mrb, options, kLevelKeys, SDL_arraysize(kLevelKeys), "level");
    const int width = (int)ReqNumber(mrb, options, "width", "create_level");
    const int height = (int)ReqNumber(mrb, options, "height", "create_level");
    const int tile = (int)OptNumber(mrb, options, "tile", 16.0f);
    Grapple_Platformer *level = Grapple_CreatePlatformer(engine, width, height, tile);
    if (level == NULL)
    {
        return Fail(mrb);
    }
    ApplyLevelOptions(mrb, options, level);
    return WrapLevel(mrb, level);
}

static mrb_value RLoadLevel(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    const char *path = NULL;
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "oz|H", &engine_value, &path, &options);
    Grapple_Engine *engine = Grapple_RubyEngineAt(mrb, engine_value);
    CheckKeys(mrb, options, kLevelKeys, SDL_arraysize(kLevelKeys), "level");
    Grapple_Platformer *level = Grapple_LoadPlatformer(engine, path);
    if (level == NULL)
    {
        return Fail(mrb);
    }
    ApplyLevelOptions(mrb, options, level);
    return WrapLevel(mrb, level);
}

static mrb_value RDestroy(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = (Grapple_Platformer *)mrb_data_get_ptr(mrb, self, &kLevelType);
    if (level != NULL)
    {
        Grapple_DestroyPlatformer(level);
        DATA_PTR(self) = NULL;
    }
    return mrb_nil_value();
}

/* --- level: cells and objects ------------------------------------------------ */

static mrb_value Options(mrb_state *mrb)
{
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);
    return options;
}

static mrb_value RFloor(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "width", "depth"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "create_floor");
    return mrb_bool_value(Grapple_PlatformerCreateFloor(
        level, (int)ReqNumber(mrb, o, "x", "create_floor"),
        (int)ReqNumber(mrb, o, "y", "create_floor"),
        (int)ReqNumber(mrb, o, "width", "create_floor"), (int)OptNumber(mrb, o, "depth", 0.0f)));
}

static mrb_value RWall(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "height"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "create_wall");
    return mrb_bool_value(
        Grapple_PlatformerCreateWall(level, (int)ReqNumber(mrb, o, "x", "create_wall"),
                                     (int)ReqNumber(mrb, o, "y", "create_wall"),
                                     (int)ReqNumber(mrb, o, "height", "create_wall")));
}

static mrb_value RBlock(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "width", "height"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "create_block");
    return mrb_bool_value(Grapple_PlatformerCreateBlock(
        level, (int)ReqNumber(mrb, o, "x", "create_block"),
        (int)ReqNumber(mrb, o, "y", "create_block"), (int)OptNumber(mrb, o, "width", 1.0f),
        (int)OptNumber(mrb, o, "height", 1.0f)));
}

static mrb_value RStairs(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "steps", "direction"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "create_stairs");
    const char *direction = OptString(mrb, o, "direction", "up");
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
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown stairs direction '%s' (up, down)", direction);
        return mrb_nil_value();
    }
    return mrb_bool_value(
        Grapple_PlatformerCreateStairs(level, (int)ReqNumber(mrb, o, "x", "create_stairs"),
                                       (int)ReqNumber(mrb, o, "y", "create_stairs"),
                                       (int)ReqNumber(mrb, o, "steps", "create_stairs"), rising));
}

static mrb_value RPlatform(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "width"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "create_platform");
    return mrb_bool_value(
        Grapple_PlatformerCreatePlatform(level, (int)ReqNumber(mrb, o, "x", "create_platform"),
                                         (int)ReqNumber(mrb, o, "y", "create_platform"),
                                         (int)ReqNumber(mrb, o, "width", "create_platform")));
}

static mrb_value RSolid(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "w", "h", "width", "height"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "solid");
    const float w = OptNumber(mrb, o, "w", OptNumber(mrb, o, "width", 0.0f));
    const float h = OptNumber(mrb, o, "h", OptNumber(mrb, o, "height", 0.0f));
    const int id = Grapple_PlatformerAddSolid(level, ReqNumber(mrb, o, "x", "solid"),
                                              ReqNumber(mrb, o, "y", "solid"), w, h);
    if (id < 0)
    {
        return Fail(mrb);
    }
    return mrb_fixnum_value(id);
}

static mrb_value RRemoveSolid(mrb_state *mrb, mrb_value self)
{
    mrb_int id = 0;
    mrb_get_args(mrb, "i", &id);
    return mrb_bool_value(Grapple_PlatformerRemoveSolid(LevelOf(mrb, self), (int)id));
}

static mrb_value RFill(mrb_state *mrb, mrb_value self)
{
    static const char *const keys[] = {"x", "y", "width", "height", "kind"};
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    CheckKeys(mrb, o, keys, SDL_arraysize(keys), "fill");
    Grapple_PlatformerTile tile = GRAPPLE_PLATFORMER_SOLID;
    const char *kind = OptString(mrb, o, "kind", "solid");
    if (!ParseTile(kind, &tile))
    {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown tile kind '%s' (empty, solid, one_way)", kind);
    }
    return mrb_fixnum_value(Grapple_PlatformerFillTiles(
        level, (int)ReqNumber(mrb, o, "x", "fill"), (int)ReqNumber(mrb, o, "y", "fill"),
        (int)ReqNumber(mrb, o, "width", "fill"), (int)ReqNumber(mrb, o, "height", "fill"), tile));
}

static mrb_value RSetTile(mrb_state *mrb, mrb_value self)
{
    mrb_int x = 0;
    mrb_int y = 0;
    mrb_value kind;
    mrb_get_args(mrb, "iio", &x, &y, &kind);
    if (!Grapple_PlatformerSetTile(LevelOf(mrb, self), (int)x, (int)y, TileArg(mrb, kind)))
    {
        return Fail(mrb);
    }
    return mrb_nil_value();
}

static mrb_value RTileAt(mrb_state *mrb, mrb_value self)
{
    mrb_int x = 0;
    mrb_int y = 0;
    mrb_get_args(mrb, "ii", &x, &y);
    return mrb_symbol_value(mrb_intern_cstr(
        mrb, TileName(Grapple_PlatformerTileAt(LevelOf(mrb, self), (int)x, (int)y))));
}

static mrb_value RSize(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = LevelOf(mrb, self);
    mrb_value out = mrb_ary_new_capa(mrb, 3);
    mrb_ary_push(mrb, out, mrb_fixnum_value(Grapple_PlatformerWidth(level)));
    mrb_ary_push(mrb, out, mrb_fixnum_value(Grapple_PlatformerHeight(level)));
    mrb_ary_push(mrb, out, mrb_fixnum_value(Grapple_PlatformerTileSize(level)));
    return out;
}

static mrb_value Pair(mrb_state *mrb, float a, float b)
{
    mrb_value out = mrb_ary_new_capa(mrb, 2);
    mrb_ary_push(mrb, out, mrb_float_value(mrb, a));
    mrb_ary_push(mrb, out, mrb_float_value(mrb, b));
    return out;
}

static mrb_value RPixelSize(mrb_state *mrb, mrb_value self)
{
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPixelSize(LevelOf(mrb, self), &w, &h);
    return Pair(mrb, w, h);
}

/* --- level: look, camera, loop, input ---------------------------------------- */

static mrb_value RBackground(mrb_state *mrb, mrb_value self)
{
    mrb_value value;
    mrb_get_args(mrb, "o", &value);
    SDL_FColor color;
    if (!ReadColor(mrb, value, &color))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "background needs a colour [r, g, b]");
    }
    Grapple_PlatformerSetBackgroundColor(LevelOf(mrb, self), color.r, color.g, color.b, color.a);
    return mrb_nil_value();
}

static mrb_value RTileColor(mrb_state *mrb, mrb_value self)
{
    mrb_value kind;
    mrb_value value;
    mrb_get_args(mrb, "oo", &kind, &value);
    SDL_FColor color;
    if (!ReadColor(mrb, value, &color))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "tile_color needs a colour [r, g, b]");
    }
    Grapple_PlatformerSetTileColor(LevelOf(mrb, self), TileArg(mrb, kind), color.r, color.g,
                                   color.b, color.a);
    return mrb_nil_value();
}

static mrb_value RSetScroll(mrb_state *mrb, mrb_value self)
{
    mrb_value value;
    mrb_get_args(mrb, "o", &value);
    const char *name = NameOf(mrb, value);
    Grapple_PlatformerScroll mode;
    if (name == NULL || !ParseScroll(name, &mode))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "unknown scroll mode (:free, :forward)");
    }
    Grapple_PlatformerSetScroll(LevelOf(mrb, self), mode);
    return mrb_nil_value();
}

static mrb_value RScroll(mrb_state *mrb, mrb_value self)
{
    const bool forward =
        Grapple_PlatformerScrollMode(LevelOf(mrb, self)) == GRAPPLE_PLATFORMER_SCROLL_FORWARD;
    return mrb_symbol_value(mrb_intern_cstr(mrb, forward ? "forward" : "free"));
}

static mrb_value RCameraSmoothing(mrb_state *mrb, mrb_value self)
{
    mrb_float seconds = 0.0;
    mrb_get_args(mrb, "f", &seconds);
    Grapple_PlatformerSetCameraSmoothing(LevelOf(mrb, self), (float)seconds);
    return mrb_nil_value();
}

static mrb_value RCameraLookAhead(mrb_state *mrb, mrb_value self)
{
    mrb_float pixels = 0.0;
    mrb_get_args(mrb, "f", &pixels);
    Grapple_PlatformerSetCameraLookAhead(LevelOf(mrb, self), (float)pixels);
    return mrb_nil_value();
}

static mrb_value RCameraDeadzone(mrb_state *mrb, mrb_value self)
{
    mrb_float w = 0.0;
    mrb_float h = 0.0;
    mrb_get_args(mrb, "ff", &w, &h);
    Grapple_PlatformerSetCameraDeadzone(LevelOf(mrb, self), (float)w, (float)h);
    return mrb_nil_value();
}

static mrb_value RCameraPosition(mrb_state *mrb, mrb_value self)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerCameraPosition(LevelOf(mrb, self), &x, &y);
    return Pair(mrb, x, y);
}

static mrb_value RAttach(mrb_state *mrb, mrb_value self)
{
    if (!Grapple_PlatformerAttach(LevelOf(mrb, self)))
    {
        return Fail(mrb);
    }
    return self;
}

static mrb_value RDetach(mrb_state *mrb, mrb_value self)
{
    Grapple_PlatformerDetach(LevelOf(mrb, self));
    return mrb_nil_value();
}

static mrb_value RAttached(mrb_state *mrb, mrb_value self)
{
    return mrb_bool_value(Grapple_PlatformerAttached(LevelOf(mrb, self)));
}

static mrb_value RStep(mrb_state *mrb, mrb_value self)
{
    mrb_float step = 0.0;
    mrb_get_args(mrb, "f", &step);
    Grapple_PlatformerStep(LevelOf(mrb, self), (float)step);
    return mrb_nil_value();
}

static mrb_value RUpdate(mrb_state *mrb, mrb_value self)
{
    mrb_float dt = 0.0;
    mrb_get_args(mrb, "f", &dt);
    Grapple_PlatformerUpdate(LevelOf(mrb, self), (float)dt);
    return mrb_nil_value();
}

static mrb_value RRender(mrb_state *mrb, mrb_value self)
{
    mrb_float alpha = 1.0;
    mrb_get_args(mrb, "|f", &alpha);
    Grapple_PlatformerRender(LevelOf(mrb, self), (float)alpha);
    return mrb_nil_value();
}

static mrb_value RBind(mrb_state *mrb, mrb_value self)
{
    const char *action = NULL;
    const char *binding = NULL;
    mrb_get_args(mrb, "zz", &action, &binding);
    if (!Grapple_PlatformerBind(LevelOf(mrb, self), action, binding))
    {
        return Fail(mrb);
    }
    return mrb_nil_value();
}

static const char *ActionArg(mrb_state *mrb)
{
    mrb_value value;
    mrb_get_args(mrb, "o", &value);
    const char *name = NameOf(mrb, value);
    if (name == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "an action name is required");
    }
    return name;
}

static mrb_value RActionDown(mrb_state *mrb, mrb_value self)
{
    return mrb_bool_value(Grapple_PlatformerActionDown(LevelOf(mrb, self), ActionArg(mrb)));
}

static mrb_value RActionPressed(mrb_state *mrb, mrb_value self)
{
    return mrb_bool_value(Grapple_PlatformerActionPressed(LevelOf(mrb, self), ActionArg(mrb)));
}

static mrb_value RActionReleased(mrb_state *mrb, mrb_value self)
{
    return mrb_bool_value(Grapple_PlatformerActionReleased(LevelOf(mrb, self), ActionArg(mrb)));
}

static mrb_value RActionValue(mrb_state *mrb, mrb_value self)
{
    return mrb_float_value(mrb, Grapple_PlatformerActionValue(LevelOf(mrb, self), ActionArg(mrb)));
}

static mrb_value RActions(mrb_state *mrb, mrb_value self)
{
    return GrappleGen_RubyPushHandle(mrb, Grapple_PlatformerActions(LevelOf(mrb, self)),
                                     "Grapple_ActionMap");
}

/* --- the player --------------------------------------------------------------- */

static mrb_value WrapPlayer(mrb_state *mrb, mrb_value level_obj)
{
    mrb_value player = mrb_obj_new(mrb, g_player_class, 0, NULL);
    mrb_iv_set(mrb, player, mrb_intern_lit(mrb, "@level"), level_obj);
    return player;
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

static mrb_value RCreatePlayer(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = LevelOf(mrb, self);
    const mrb_value o = Options(mrb);
    if (mrb_hash_p(o))
    {
        const mrb_value names = mrb_hash_keys(mrb, o);
        for (mrb_int i = 0; i < RARRAY_LEN(names); ++i)
        {
            const char *name = NameOf(mrb, mrb_ary_ref(mrb, names, i));
            bool known = name != NULL && IsTuningKey(name);
            for (size_t k = 0; name != NULL && k < SDL_arraysize(kPlayerKeys) && !known; ++k)
            {
                known = SDL_strcmp(name, kPlayerKeys[k]) == 0;
            }
            if (name != NULL && !known)
            {
                mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown player option '%s'", name);
            }
        }
    }
    if (Grapple_PlatformerCreatePlayer(level, (int)ReqNumber(mrb, o, "x", "create_player"),
                                       (int)ReqNumber(mrb, o, "y", "create_player")) ==
        GRAPPLE_ACTOR_NONE)
    {
        return Fail(mrb);
    }
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPlayerSize(level, &w, &h);
    Grapple_PlatformerSetPlayerSize(level, OptNumber(mrb, o, "width", w),
                                    OptNumber(mrb, o, "height", h));
    SDL_FColor color;
    if (ReadColor(mrb, Key(mrb, o, "color"), &color))
    {
        Grapple_PlatformerSetPlayerColor(level, color.r, color.g, color.b, color.a);
    }
    if (mrb_hash_p(o))
    {
        const mrb_value names = mrb_hash_keys(mrb, o);
        for (mrb_int i = 0; i < RARRAY_LEN(names); ++i)
        {
            const mrb_value key = mrb_ary_ref(mrb, names, i);
            const char *name = NameOf(mrb, key);
            const mrb_value value = mrb_hash_get(mrb, o, key);
            if (name != NULL && IsTuningKey(name) && IsNumber(value))
            {
                Grapple_PlatformerSetPlayerTuning(level, name, (float)mrb_as_float(mrb, value));
            }
        }
    }
    return WrapPlayer(mrb, self);
}

static mrb_value RPlayer(mrb_state *mrb, mrb_value self)
{
    if (!Grapple_PlatformerHasPlayer(LevelOf(mrb, self)))
    {
        return mrb_nil_value();
    }
    return WrapPlayer(mrb, self);
}

static mrb_value RRemovePlayer(mrb_state *mrb, mrb_value self)
{
    Grapple_PlatformerRemovePlayer(LevelOf(mrb, self));
    return mrb_nil_value();
}

static mrb_value RPState(mrb_state *mrb, mrb_value self)
{
    return mrb_symbol_value(
        mrb_intern_cstr(mrb, Grapple_PlatformerPlayerStateName(PlayerLevel(mrb, self))));
}

#define PLAYER_BOOL(fn, call)                                                                      \
    static mrb_value fn(mrb_state *mrb, mrb_value self)                                            \
    {                                                                                              \
        return mrb_bool_value(call(PlayerLevel(mrb, self)));                                       \
    }

PLAYER_BOOL(RPStateChanged, Grapple_PlatformerPlayerStateChanged)
PLAYER_BOOL(RPLanded, Grapple_PlatformerPlayerLanded)
PLAYER_BOOL(RPJumped, Grapple_PlatformerPlayerJumped)
PLAYER_BOOL(RPBumped, Grapple_PlatformerPlayerBumped)
PLAYER_BOOL(RPFell, Grapple_PlatformerPlayerFell)
PLAYER_BOOL(RPGrounded, Grapple_PlatformerPlayerGrounded)
PLAYER_BOOL(RPPaused, Grapple_PlatformerPlayerPaused)

static mrb_value RPWall(mrb_state *mrb, mrb_value self)
{
    return mrb_fixnum_value(Grapple_PlatformerPlayerWall(PlayerLevel(mrb, self)));
}

static mrb_value RPFacing(mrb_state *mrb, mrb_value self)
{
    return mrb_fixnum_value(Grapple_PlatformerPlayerFacing(PlayerLevel(mrb, self)));
}

static mrb_value RPPosition(mrb_state *mrb, mrb_value self)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerPosition(PlayerLevel(mrb, self), &x, &y);
    return Pair(mrb, x, y);
}

static mrb_value RPVelocity(mrb_state *mrb, mrb_value self)
{
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerVelocity(PlayerLevel(mrb, self), &x, &y);
    return Pair(mrb, x, y);
}

static mrb_value RPSize(mrb_state *mrb, mrb_value self)
{
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPlayerSize(PlayerLevel(mrb, self), &w, &h);
    return Pair(mrb, w, h);
}

static mrb_value RPSetSize(mrb_state *mrb, mrb_value self)
{
    mrb_float w = 0.0;
    mrb_float h = 0.0;
    mrb_get_args(mrb, "ff", &w, &h);
    Grapple_PlatformerSetPlayerSize(PlayerLevel(mrb, self), (float)w, (float)h);
    return mrb_nil_value();
}

static mrb_value RPColor(mrb_state *mrb, mrb_value self)
{
    mrb_value value;
    mrb_get_args(mrb, "o", &value);
    SDL_FColor color;
    if (!ReadColor(mrb, value, &color))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "color needs a colour [r, g, b]");
    }
    Grapple_PlatformerSetPlayerColor(PlayerLevel(mrb, self), color.r, color.g, color.b, color.a);
    return mrb_nil_value();
}

static mrb_value RPSet(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = PlayerLevel(mrb, self);
    mrb_value key;
    mrb_value value = mrb_nil_value();
    mrb_get_args(mrb, "o|o", &key, &value);
    if (mrb_hash_p(key))
    {
        const mrb_value names = mrb_hash_keys(mrb, key);
        for (mrb_int i = 0; i < RARRAY_LEN(names); ++i)
        {
            const mrb_value k = mrb_ary_ref(mrb, names, i);
            const mrb_value v = mrb_hash_get(mrb, key, k);
            const char *name = NameOf(mrb, k);
            if (name == NULL || !IsNumber(v) ||
                !Grapple_PlatformerSetPlayerTuning(level, name, (float)mrb_as_float(mrb, v)))
            {
                return Fail(mrb);
            }
        }
        return mrb_nil_value();
    }
    const char *name = NameOf(mrb, key);
    if (name == NULL || !IsNumber(value) ||
        !Grapple_PlatformerSetPlayerTuning(level, name, (float)mrb_as_float(mrb, value)))
    {
        return Fail(mrb);
    }
    return mrb_nil_value();
}

static mrb_value RPGet(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = PlayerLevel(mrb, self);
    mrb_value key;
    mrb_get_args(mrb, "o", &key);
    const char *name = NameOf(mrb, key);
    if (name == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "a tuning key is required");
    }
    SDL_ClearError();
    const float value = Grapple_PlatformerPlayerTuning(level, name);
    if (value == 0.0f && SDL_GetError()[0] != '\0')
    {
        return Fail(mrb);
    }
    return mrb_float_value(mrb, value);
}

static mrb_value RPRespawn(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = PlayerLevel(mrb, self);
    mrb_float x = 0.0;
    mrb_float y = 0.0;
    const mrb_int count = mrb_get_args(mrb, "|ff", &x, &y);
    if (count == 0)
    {
        Grapple_PlatformerPlayerRespawnAtStart(level);
    }
    else
    {
        Grapple_PlatformerPlayerRespawn(level, (float)x, (float)y);
    }
    return mrb_nil_value();
}

static mrb_value RPSetPaused(mrb_state *mrb, mrb_value self)
{
    mrb_bool paused = false;
    mrb_get_args(mrb, "b", &paused);
    Grapple_PlatformerSetPlayerPaused(PlayerLevel(mrb, self), paused);
    return mrb_nil_value();
}

static mrb_value RPActor(mrb_state *mrb, mrb_value self)
{
    return mrb_int_value(mrb, (mrb_int)Grapple_PlatformerPlayer(PlayerLevel(mrb, self)));
}

static mrb_value RPScriptedInput(mrb_state *mrb, mrb_value self)
{
    Grapple_Platformer *level = PlayerLevel(mrb, self);
    mrb_bool enabled = false;
    if (mrb_get_args(mrb, "|b", &enabled) == 0)
    {
        return mrb_bool_value(Grapple_PlatformerScriptedInput(level));
    }
    Grapple_PlatformerSetScriptedInput(level, enabled);
    return mrb_nil_value();
}

static mrb_value RPInput(mrb_state *mrb, mrb_value self)
{
    mrb_float move = 0.0;
    mrb_bool jump = false;
    mrb_bool run = false;
    mrb_get_args(mrb, "f|bb", &move, &jump, &run);
    Grapple_PlatformerScriptInput(PlayerLevel(mrb, self), (float)move, jump, run);
    return mrb_nil_value();
}

/* --- registration --------------------------------------------------------------- */

bool Grapple_OpenRubyPlatformer(mrb_state *mrb)
{
    if (mrb == NULL)
    {
        return SDL_InvalidParamError("mrb");
    }
    struct RClass *module = mrb_define_module(mrb, "Grapple");

    struct RClass *level = mrb_define_class_under(mrb, module, "Level", mrb->object_class);
    MRB_SET_INSTANCE_TT(level, MRB_TT_DATA);
    g_level_class = level;
    mrb_define_method(mrb, level, "create_floor", RFloor, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "create_wall", RWall, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "create_block", RBlock, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "create_stairs", RStairs, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "create_platform", RPlatform, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "solid", RSolid, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "remove_solid", RRemoveSolid, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "fill", RFill, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "set_tile", RSetTile, MRB_ARGS_REQ(3));
    mrb_define_method(mrb, level, "tile_at", RTileAt, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, level, "size", RSize, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "pixel_size", RPixelSize, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "background", RBackground, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "tile_color", RTileColor, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, level, "scroll=", RSetScroll, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "set_scroll", RSetScroll, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "scroll", RScroll, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "camera_smoothing=", RCameraSmoothing, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "camera_look_ahead=", RCameraLookAhead, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "camera_deadzone", RCameraDeadzone, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, level, "camera_position", RCameraPosition, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "attach", RAttach, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "detach", RDetach, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "attached?", RAttached, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "step", RStep, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "update", RUpdate, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "render", RRender, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "destroy", RDestroy, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "bind", RBind, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, level, "action_down?", RActionDown, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "action_pressed?", RActionPressed, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "action_released?", RActionReleased, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "action_value", RActionValue, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, level, "actions", RActions, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "create_player", RCreatePlayer, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, level, "player", RPlayer, MRB_ARGS_NONE());
    mrb_define_method(mrb, level, "remove_player", RRemovePlayer, MRB_ARGS_NONE());

    struct RClass *player = mrb_define_class_under(mrb, module, "Player", mrb->object_class);
    g_player_class = player;
    mrb_define_method(mrb, player, "state", RPState, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "state_changed?", RPStateChanged, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "landed?", RPLanded, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "jumped?", RPJumped, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "bumped?", RPBumped, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "fell?", RPFell, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "grounded?", RPGrounded, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "paused?", RPPaused, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "facing", RPFacing, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "wall", RPWall, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "position", RPPosition, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "velocity", RPVelocity, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "size", RPSize, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "set_size", RPSetSize, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, player, "color=", RPColor, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, player, "set", RPSet, MRB_ARGS_ARG(1, 1));
    mrb_define_method(mrb, player, "get", RPGet, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, player, "respawn", RPRespawn, MRB_ARGS_OPT(2));
    mrb_define_method(mrb, player, "paused=", RPSetPaused, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, player, "actor", RPActor, MRB_ARGS_NONE());
    mrb_define_method(mrb, player, "scripted_input", RPScriptedInput, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, player, "input", RPInput, MRB_ARGS_ARG(1, 2));

    mrb_define_module_function(mrb, module, "create_level", RCreateLevel, MRB_ARGS_ARG(1, 1));
    mrb_define_module_function(mrb, module, "load_level", RLoadLevel, MRB_ARGS_ARG(2, 1));
    return true;
}
