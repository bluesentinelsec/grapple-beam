/* GENERATED FILE - DO NOT EDIT. Aggregate Lua registration.
 * GRAPPLE_GEN_DISABLE_<LIB> gates modules whose CMake option is
 * off on this platform (e.g. NET on Emscripten). */
#include "../src/gen_support_lua.h"

#ifndef GRAPPLE_GEN_DISABLE_SDL
extern int GrappleGen_OpenLua_sdl(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_MIX
extern int GrappleGen_OpenLua_mix(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_IMG
extern int GrappleGen_OpenLua_img(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_TTF
extern int GrappleGen_OpenLua_ttf(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_NET
extern int GrappleGen_OpenLua_net(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_PHYSFS
extern int GrappleGen_OpenLua_physfs(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_B2
extern int GrappleGen_OpenLua_b2(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_NK
extern int GrappleGen_OpenLua_nk(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_GFX
extern int GrappleGen_OpenLua_gfx(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_TOML
extern int GrappleGen_OpenLua_toml(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_YAML
extern int GrappleGen_OpenLua_yaml(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_MOG
extern int GrappleGen_OpenLua_mog(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_CJSON
extern int GrappleGen_OpenLua_cjson(lua_State *L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_GRAPPLE
extern int GrappleGen_OpenLua_grapple(lua_State *L);
#endif

int Grapple_OpenGeneratedLuaBindings(lua_State *L);
int Grapple_OpenGeneratedLuaBindings(lua_State *L)
{
#ifndef GRAPPLE_GEN_DISABLE_SDL
    GrappleGen_OpenLua_sdl(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_MIX
    GrappleGen_OpenLua_mix(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_IMG
    GrappleGen_OpenLua_img(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_TTF
    GrappleGen_OpenLua_ttf(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_NET
    GrappleGen_OpenLua_net(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_PHYSFS
    GrappleGen_OpenLua_physfs(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_B2
    GrappleGen_OpenLua_b2(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_NK
    GrappleGen_OpenLua_nk(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_GFX
    GrappleGen_OpenLua_gfx(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_TOML
    GrappleGen_OpenLua_toml(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_YAML
    GrappleGen_OpenLua_yaml(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_MOG
    GrappleGen_OpenLua_mog(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_CJSON
    GrappleGen_OpenLua_cjson(L);
#endif
#ifndef GRAPPLE_GEN_DISABLE_GRAPPLE
    GrappleGen_OpenLua_grapple(L);
#endif
    return 0;
}
