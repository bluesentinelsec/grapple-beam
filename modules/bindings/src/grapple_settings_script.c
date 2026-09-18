#include <grapple/bindings.h>
#include <grapple/engine_settings.h>
#include <grapple/lua.h>
#include <grapple/ruby.h>
#include <lauxlib.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/hash.h>
#include <mruby/string.h>

static bool Put(Grapple_Settings *settings, const char *key, const char *value, const char *path)
{
    char *absolute = NULL;
    if ((!SDL_strcmp(key, "engine.media") || !SDL_strcmp(key, "media")) && value[0] != '/' &&
        value[0] != '\\' && !(SDL_strlen(value) > 1 && value[1] == ':'))
    {
        const char *slash = SDL_strrchr(path, '/'), *back = SDL_strrchr(path, '\\');
        if (back && (!slash || back > slash))
            slash = back;
        if (slash)
        {
            SDL_asprintf(&absolute, "%.*s/%s", (int)(slash - path), path, value);
            if (!absolute)
                return false;
            value = absolute;
        }
    }
    bool ok = Grapple_SettingsSet(settings, key, value, path);
    SDL_free(absolute);
    return ok;
}
static bool LuaTable(lua_State *L, int table, Grapple_Settings *settings, const char *prefix,
                     const char *path, int depth)
{
    if (depth > 8)
        return SDL_SetError("settings script nesting exceeds 8 tables");
    table = lua_absindex(L, table);
    lua_pushnil(L);
    while (lua_next(L, table))
    {
        if (lua_type(L, -2) != LUA_TSTRING)
        {
            lua_pop(L, 2);
            return SDL_SetError("settings keys must be strings");
        }
        size_t length;
        const char *name = lua_tolstring(L, -2, &length);
        if (SDL_strlen(name) != length)
        {
            lua_pop(L, 2);
            return SDL_SetError("settings key contains NUL bytes");
        }
        char key[256];
        int n = SDL_snprintf(key, sizeof(key), "%s%s%s", prefix, *prefix ? "." : "", name);
        bool ok = n >= 0 && (size_t)n < sizeof(key);
        char number[96];
        const char *value = NULL;
        if (!ok)
            SDL_SetError("settings key is too long");
        else if (lua_istable(L, -1))
            ok = LuaTable(L, -1, settings, key, path, depth + 1);
        else
        {
            switch (lua_type(L, -1))
            {
            case LUA_TBOOLEAN:
                value = lua_toboolean(L, -1) ? "on" : "off";
                break;
            case LUA_TSTRING:
                value = lua_tolstring(L, -1, &length);
                if (SDL_strlen(value) != length)
                    ok = SDL_SetError("settings value contains NUL bytes");
                break;
            case LUA_TNUMBER:
                SDL_snprintf(number, sizeof(number), "%.17g", (double)lua_tonumber(L, -1));
                value = number;
                break;
            default:
                ok = SDL_SetError("%s must be a scalar setting", key);
                break;
            }
            if (ok)
                ok = Put(settings, key, value, path);
        }
        lua_pop(L, 1);
        if (!ok)
        {
            lua_pop(L, 1);
            return false;
        }
    }
    return true;
}
static bool RubyHash(mrb_state *mrb, mrb_value hash, Grapple_Settings *settings, const char *prefix,
                     const char *path, int depth)
{
    if (depth > 8)
        return SDL_SetError("settings script nesting exceeds 8 hashes");
    mrb_value keys = mrb_hash_keys(mrb, hash);
    for (mrb_int i = 0; i < RARRAY_LEN(keys); ++i)
    {
        mrb_value key = mrb_ary_ref(mrb, keys, i), value = mrb_hash_get(mrb, hash, key);
        const char *name = NULL;
        if (mrb_symbol_p(key))
            name = mrb_sym_name(mrb, mrb_symbol(key));
        else if (mrb_string_p(key))
        {
            name = RSTRING_PTR(key);
            if (SDL_strlen(name) != (size_t)RSTRING_LEN(key))
                return SDL_SetError("settings key contains NUL bytes");
        }
        if (!name)
            return SDL_SetError("settings keys must be strings or symbols");
        char full[256];
        int n = SDL_snprintf(full, sizeof(full), "%s%s%s", prefix, *prefix ? "." : "", name);
        if (n < 0 || (size_t)n >= sizeof(full))
            return SDL_SetError("settings key is too long");
        if (mrb_hash_p(value))
        {
            if (!RubyHash(mrb, value, settings, full, path, depth + 1))
                return false;
            continue;
        }
        const char *text = NULL;
        char number[96];
        if (mrb_true_p(value))
            text = "on";
        else if (mrb_false_p(value) && !mrb_nil_p(value))
            text = "off";
        else if (mrb_string_p(value))
        {
            text = RSTRING_PTR(value);
            if (SDL_strlen(text) != (size_t)RSTRING_LEN(value))
                return SDL_SetError("settings value contains NUL bytes");
        }
        else if (mrb_symbol_p(value))
            text = mrb_sym_name(mrb, mrb_symbol(value));
        else if (mrb_integer_p(value))
        {
            SDL_snprintf(number, sizeof(number), "%lld", (long long)mrb_integer(value));
            text = number;
        }
        else if (mrb_float_p(value))
        {
            SDL_snprintf(number, sizeof(number), "%.17g", (double)mrb_float(value));
            text = number;
        }
        if (!text)
            return SDL_SetError("%s must be a scalar setting", full);
        if (!Put(settings, full, text, path))
            return false;
    }
    return true;
}
bool Grapple_LoadSettingsScript(Grapple_Settings *settings, const char *language, const char *path)
{
    if (!settings || !language || !path)
        return SDL_InvalidParamError("settings/language/path");
    size_t size = 0;
    char *text = SDL_LoadFile(path, &size);
    if (!text)
        return false;
    if (size > 1024 * 1024 || SDL_strlen(text) != size)
    {
        SDL_free(text);
        return SDL_SetError("settings script exceeds 1 MiB or contains NUL bytes");
    }
    Grapple_Settings *next = Grapple_CloneSettings(settings);
    bool ok = false;
    if (next && !SDL_strcmp(language, "lua"))
    {
        lua_State *L = Grapple_CreateLuaState();
        if (L)
        {
            int rc = luaL_loadbufferx(L, text, size, path, "t");
            if (rc == LUA_OK)
                rc = lua_pcall(L, 0, 1, 0);
            if (rc != LUA_OK)
                SDL_SetError("%s: %s", path, lua_tostring(L, -1));
            else if (!lua_istable(L, -1))
                SDL_SetError("%s: settings script must return a table", path);
            else
                ok = LuaTable(L, -1, next, "", path, 0);
            lua_close(L);
        }
    }
    else if (next && !SDL_strcmp(language, "ruby"))
    {
        mrb_state *mrb = Grapple_CreateRubyState();
        if (mrb)
        {
            mrbc_context *ctx = mrbc_context_new(mrb);
            if (!ctx)
            {
                mrb_close(mrb);
                SDL_free(text);
                Grapple_DestroySettings(next);
                return SDL_SetError("cannot allocate Ruby settings context");
            }
            mrbc_filename(mrb, ctx, path);
            mrb_value result = mrb_load_nstring_cxt(mrb, text, size, ctx);
            mrbc_context_free(mrb, ctx);
            if (mrb->exc)
            {
                mrb_value error = mrb_inspect(mrb, mrb_obj_value(mrb->exc));
                SDL_SetError("%s: %s", path, mrb_str_to_cstr(mrb, error));
            }
            else if (!mrb_hash_p(result))
                SDL_SetError("%s: settings script must evaluate to a hash", path);
            else
                ok = RubyHash(mrb, result, next, "", path, 0);
            mrb_close(mrb);
        }
    }
    else if (next)
        SDL_SetError("settings language must be lua or ruby");
    SDL_free(text);
    if (ok)
        ok = Grapple_SettingsOverlay(settings, next);
    Grapple_DestroySettings(next);
    return ok;
}
