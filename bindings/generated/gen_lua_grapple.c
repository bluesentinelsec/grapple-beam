/* GENERATED FILE - DO NOT EDIT.
 * Regenerate: python3 -m tools.bindgen
 * Lua bindings for Grapple modules. */
#include "../src/gen_support_lua.h"

#include <grapple/base64.h>
#include <grapple/bidi.h>
#include <grapple/chiptune.h>
#include <grapple/compress.h>
#include <grapple/crypto.h>
#include <grapple/debug_text.h>
#include <grapple/dialog.h>
#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_assets.h>
#include <grapple/engine_binding.h>
#include <grapple/engine_camera.h>
#include <grapple/engine_config.h>
#include <grapple/engine_graphics.h>
#include <grapple/engine_input.h>
#include <grapple/engine_light.h>
#include <grapple/engine_media.h>
#include <grapple/engine_physics.h>
#include <grapple/engine_render.h>
#include <grapple/engine_save.h>
#include <grapple/engine_scene.h>
#include <grapple/engine_script.h>
#include <grapple/engine_text.h>
#include <grapple/gpu_build.h>
#include <grapple/gpu_primitives.h>
#include <grapple/gui.h>
#include <grapple/gui_grid.h>
#include <grapple/light.h>
#include <grapple/physics_build.h>
#include <grapple/physics_draw.h>
#include <grapple/regex.h>
#include <grapple/signals.h>
#include <grapple/textfile.h>
#include <grapple/tiled.h>
#include <grapple/vfs.h>
#include <string.h>

static void GenRead_Grapple_ActorMessage(lua_State *L, int idx, Grapple_ActorMessage *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->id = (Uint32)GrappleGen_LuaFieldInt(L, idx, "id");
    out->sender = (Grapple_ActorId)GrappleGen_LuaFieldInt(L, idx, "sender");
    out->a = (float)GrappleGen_LuaFieldNum(L, idx, "a");
    out->b = (float)GrappleGen_LuaFieldNum(L, idx, "b");
    out->value = (Sint64)GrappleGen_LuaFieldInt(L, idx, "value");
}

static void GenRead_Grapple_ActorTransform(lua_State *L, int idx, Grapple_ActorTransform *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->x = (float)GrappleGen_LuaFieldNum(L, idx, "x");
    out->y = (float)GrappleGen_LuaFieldNum(L, idx, "y");
    out->rotation = (float)GrappleGen_LuaFieldNum(L, idx, "rotation");
    out->scale_x = (float)GrappleGen_LuaFieldNum(L, idx, "scale_x");
    out->scale_y = (float)GrappleGen_LuaFieldNum(L, idx, "scale_y");
}

static void GenPush_Grapple_ActorTransform(lua_State *L, const Grapple_ActorTransform *in)
{
    lua_createtable(L, 0, 5);
    lua_pushnumber(L, (lua_Number)in->x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, (lua_Number)in->y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, (lua_Number)in->rotation);
    lua_setfield(L, -2, "rotation");
    lua_pushnumber(L, (lua_Number)in->scale_x);
    lua_setfield(L, -2, "scale_x");
    lua_pushnumber(L, (lua_Number)in->scale_y);
    lua_setfield(L, -2, "scale_y");
}

static void GenRead_Grapple_Binding(lua_State *L, int idx, Grapple_Binding *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->source = (Grapple_BindingSource)GrappleGen_LuaFieldInt(L, idx, "source");
    out->code = (int)GrappleGen_LuaFieldInt(L, idx, "code");
    out->sign = (int)GrappleGen_LuaFieldInt(L, idx, "sign");
    out->axis_half = (int)GrappleGen_LuaFieldInt(L, idx, "axis_half");
}

static void GenPush_Grapple_Binding(lua_State *L, const Grapple_Binding *in)
{
    lua_createtable(L, 0, 4);
    lua_pushinteger(L, (lua_Integer)in->source);
    lua_setfield(L, -2, "source");
    lua_pushinteger(L, (lua_Integer)in->code);
    lua_setfield(L, -2, "code");
    lua_pushinteger(L, (lua_Integer)in->sign);
    lua_setfield(L, -2, "sign");
    lua_pushinteger(L, (lua_Integer)in->axis_half);
    lua_setfield(L, -2, "axis_half");
}

static void GenRead_Grapple_BodyDef(lua_State *L, int idx, Grapple_BodyDef *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->type = (Grapple_BodyType)GrappleGen_LuaFieldInt(L, idx, "type");
    out->shape = (Grapple_ShapeType)GrappleGen_LuaFieldInt(L, idx, "shape");
    out->width = (float)GrappleGen_LuaFieldNum(L, idx, "width");
    out->height = (float)GrappleGen_LuaFieldNum(L, idx, "height");
    out->offset_x = (float)GrappleGen_LuaFieldNum(L, idx, "offset_x");
    out->offset_y = (float)GrappleGen_LuaFieldNum(L, idx, "offset_y");
    out->density = (float)GrappleGen_LuaFieldNum(L, idx, "density");
    out->friction = (float)GrappleGen_LuaFieldNum(L, idx, "friction");
    out->restitution = (float)GrappleGen_LuaFieldNum(L, idx, "restitution");
    out->fixed_rotation = (bool)GrappleGen_LuaFieldBool(L, idx, "fixed_rotation");
    out->sensor = (bool)GrappleGen_LuaFieldBool(L, idx, "sensor");
    out->gravity_scale = (float)GrappleGen_LuaFieldNum(L, idx, "gravity_scale");
    out->damping = (float)GrappleGen_LuaFieldNum(L, idx, "damping");
    out->category = (Uint32)GrappleGen_LuaFieldInt(L, idx, "category");
    out->collides_with = (Uint32)GrappleGen_LuaFieldInt(L, idx, "collides_with");
    out->bullet = (bool)GrappleGen_LuaFieldBool(L, idx, "bullet");
}

static void GenPush_Grapple_BodyDef(lua_State *L, const Grapple_BodyDef *in)
{
    lua_createtable(L, 0, 16);
    lua_pushinteger(L, (lua_Integer)in->type);
    lua_setfield(L, -2, "type");
    lua_pushinteger(L, (lua_Integer)in->shape);
    lua_setfield(L, -2, "shape");
    lua_pushnumber(L, (lua_Number)in->width);
    lua_setfield(L, -2, "width");
    lua_pushnumber(L, (lua_Number)in->height);
    lua_setfield(L, -2, "height");
    lua_pushnumber(L, (lua_Number)in->offset_x);
    lua_setfield(L, -2, "offset_x");
    lua_pushnumber(L, (lua_Number)in->offset_y);
    lua_setfield(L, -2, "offset_y");
    lua_pushnumber(L, (lua_Number)in->density);
    lua_setfield(L, -2, "density");
    lua_pushnumber(L, (lua_Number)in->friction);
    lua_setfield(L, -2, "friction");
    lua_pushnumber(L, (lua_Number)in->restitution);
    lua_setfield(L, -2, "restitution");
    lua_pushboolean(L, (int)in->fixed_rotation);
    lua_setfield(L, -2, "fixed_rotation");
    lua_pushboolean(L, (int)in->sensor);
    lua_setfield(L, -2, "sensor");
    lua_pushnumber(L, (lua_Number)in->gravity_scale);
    lua_setfield(L, -2, "gravity_scale");
    lua_pushnumber(L, (lua_Number)in->damping);
    lua_setfield(L, -2, "damping");
    lua_pushinteger(L, (lua_Integer)in->category);
    lua_setfield(L, -2, "category");
    lua_pushinteger(L, (lua_Integer)in->collides_with);
    lua_setfield(L, -2, "collides_with");
    lua_pushboolean(L, (int)in->bullet);
    lua_setfield(L, -2, "bullet");
}

static void GenRead_SDL_FRect(lua_State *L, int idx, SDL_FRect *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->x = (float)GrappleGen_LuaFieldNum(L, idx, "x");
    out->y = (float)GrappleGen_LuaFieldNum(L, idx, "y");
    out->w = (float)GrappleGen_LuaFieldNum(L, idx, "w");
    out->h = (float)GrappleGen_LuaFieldNum(L, idx, "h");
}

static void GenPush_SDL_FRect(lua_State *L, const SDL_FRect *in)
{
    lua_createtable(L, 0, 4);
    lua_pushnumber(L, (lua_Number)in->x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, (lua_Number)in->y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, (lua_Number)in->w);
    lua_setfield(L, -2, "w");
    lua_pushnumber(L, (lua_Number)in->h);
    lua_setfield(L, -2, "h");
}

static void GenRead_Grapple_Camera(lua_State *L, int idx, Grapple_Camera *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->x = (float)GrappleGen_LuaFieldNum(L, idx, "x");
    out->y = (float)GrappleGen_LuaFieldNum(L, idx, "y");
    out->zoom = (float)GrappleGen_LuaFieldNum(L, idx, "zoom");
    out->target_x = (float)GrappleGen_LuaFieldNum(L, idx, "target_x");
    out->target_y = (float)GrappleGen_LuaFieldNum(L, idx, "target_y");
    out->smoothing = (float)GrappleGen_LuaFieldNum(L, idx, "smoothing");
    out->deadzone_w = (float)GrappleGen_LuaFieldNum(L, idx, "deadzone_w");
    out->deadzone_h = (float)GrappleGen_LuaFieldNum(L, idx, "deadzone_h");
    lua_getfield(L, idx, "bounds");
    GenRead_SDL_FRect(L, lua_gettop(L), &out->bounds);
    lua_pop(L, 1);
    out->shake_amount = (float)GrappleGen_LuaFieldNum(L, idx, "shake_amount");
    out->shake_seconds = (float)GrappleGen_LuaFieldNum(L, idx, "shake_seconds");
    out->shake_remaining = (float)GrappleGen_LuaFieldNum(L, idx, "shake_remaining");
    lua_getfield(L, idx, "viewport");
    GenRead_SDL_FRect(L, lua_gettop(L), &out->viewport);
    lua_pop(L, 1);
    lua_getfield(L, idx, "visible");
    GenRead_SDL_FRect(L, lua_gettop(L), &out->visible);
    lua_pop(L, 1);
}

static void GenPush_Grapple_Camera(lua_State *L, const Grapple_Camera *in)
{
    lua_createtable(L, 0, 14);
    lua_pushnumber(L, (lua_Number)in->x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, (lua_Number)in->y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, (lua_Number)in->zoom);
    lua_setfield(L, -2, "zoom");
    lua_pushnumber(L, (lua_Number)in->target_x);
    lua_setfield(L, -2, "target_x");
    lua_pushnumber(L, (lua_Number)in->target_y);
    lua_setfield(L, -2, "target_y");
    lua_pushnumber(L, (lua_Number)in->smoothing);
    lua_setfield(L, -2, "smoothing");
    lua_pushnumber(L, (lua_Number)in->deadzone_w);
    lua_setfield(L, -2, "deadzone_w");
    lua_pushnumber(L, (lua_Number)in->deadzone_h);
    lua_setfield(L, -2, "deadzone_h");
    GenPush_SDL_FRect(L, &in->bounds);
    lua_setfield(L, -2, "bounds");
    lua_pushnumber(L, (lua_Number)in->shake_amount);
    lua_setfield(L, -2, "shake_amount");
    lua_pushnumber(L, (lua_Number)in->shake_seconds);
    lua_setfield(L, -2, "shake_seconds");
    lua_pushnumber(L, (lua_Number)in->shake_remaining);
    lua_setfield(L, -2, "shake_remaining");
    GenPush_SDL_FRect(L, &in->viewport);
    lua_setfield(L, -2, "viewport");
    GenPush_SDL_FRect(L, &in->visible);
    lua_setfield(L, -2, "visible");
}

static void GenRead_Grapple_ChipToneDesc(lua_State *L, int idx, Grapple_ChipToneDesc *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->wave = (Grapple_ChipWave)GrappleGen_LuaFieldInt(L, idx, "wave");
    out->freq_hz = (float)GrappleGen_LuaFieldNum(L, idx, "freq_hz");
    out->freq_end_hz = (float)GrappleGen_LuaFieldNum(L, idx, "freq_end_hz");
    out->duration_ms = (Uint32)GrappleGen_LuaFieldInt(L, idx, "duration_ms");
    out->volume = (float)GrappleGen_LuaFieldNum(L, idx, "volume");
    out->attack_ms = (Uint32)GrappleGen_LuaFieldInt(L, idx, "attack_ms");
    out->release_ms = (Uint32)GrappleGen_LuaFieldInt(L, idx, "release_ms");
    out->vibrato_hz = (float)GrappleGen_LuaFieldNum(L, idx, "vibrato_hz");
    out->vibrato_semitones = (float)GrappleGen_LuaFieldNum(L, idx, "vibrato_semitones");
}

static void GenRead_Grapple_GraphicsSettings(lua_State *L, int idx, Grapple_GraphicsSettings *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->vsync = (bool)GrappleGen_LuaFieldBool(L, idx, "vsync");
    out->max_fps = (int)GrappleGen_LuaFieldInt(L, idx, "max_fps");
    out->window_mode = (Grapple_WindowMode)GrappleGen_LuaFieldInt(L, idx, "window_mode");
    out->window_width = (int)GrappleGen_LuaFieldInt(L, idx, "window_width");
    out->window_height = (int)GrappleGen_LuaFieldInt(L, idx, "window_height");
    out->display = (int)GrappleGen_LuaFieldInt(L, idx, "display");
    out->presentation = (Grapple_EnginePresentation)GrappleGen_LuaFieldInt(L, idx, "presentation");
    out->render_scale = (float)GrappleGen_LuaFieldNum(L, idx, "render_scale");
    out->filter = (Grapple_TextureFilter)GrappleGen_LuaFieldInt(L, idx, "filter");
    out->particles = (Grapple_GraphicsQuality)GrappleGen_LuaFieldInt(L, idx, "particles");
    out->dynamic_lights = (Grapple_GraphicsQuality)GrappleGen_LuaFieldInt(L, idx, "dynamic_lights");
    out->shadows = (Grapple_GraphicsQuality)GrappleGen_LuaFieldInt(L, idx, "shadows");
    out->bloom = (float)GrappleGen_LuaFieldNum(L, idx, "bloom");
    out->bloom_threshold = (float)GrappleGen_LuaFieldNum(L, idx, "bloom_threshold");
    out->crt = (float)GrappleGen_LuaFieldNum(L, idx, "crt");
    out->crt_curvature = (float)GrappleGen_LuaFieldNum(L, idx, "crt_curvature");
    out->pixelation = (int)GrappleGen_LuaFieldInt(L, idx, "pixelation");
    out->chromatic_aberration = (float)GrappleGen_LuaFieldNum(L, idx, "chromatic_aberration");
    out->antialias = (Grapple_GraphicsAA)GrappleGen_LuaFieldInt(L, idx, "antialias");
    out->brightness = (float)GrappleGen_LuaFieldNum(L, idx, "brightness");
    out->contrast = (float)GrappleGen_LuaFieldNum(L, idx, "contrast");
    out->saturation = (float)GrappleGen_LuaFieldNum(L, idx, "saturation");
    out->color_blind = (Grapple_ColorBlindMode)GrappleGen_LuaFieldInt(L, idx, "color_blind");
    out->reduced_flashing = (bool)GrappleGen_LuaFieldBool(L, idx, "reduced_flashing");
    out->screen_shake = (float)GrappleGen_LuaFieldNum(L, idx, "screen_shake");
    out->ui_scale = (float)GrappleGen_LuaFieldNum(L, idx, "ui_scale");
}

static void GenPush_Grapple_GraphicsSettings(lua_State *L, const Grapple_GraphicsSettings *in)
{
    lua_createtable(L, 0, 26);
    lua_pushboolean(L, (int)in->vsync);
    lua_setfield(L, -2, "vsync");
    lua_pushinteger(L, (lua_Integer)in->max_fps);
    lua_setfield(L, -2, "max_fps");
    lua_pushinteger(L, (lua_Integer)in->window_mode);
    lua_setfield(L, -2, "window_mode");
    lua_pushinteger(L, (lua_Integer)in->window_width);
    lua_setfield(L, -2, "window_width");
    lua_pushinteger(L, (lua_Integer)in->window_height);
    lua_setfield(L, -2, "window_height");
    lua_pushinteger(L, (lua_Integer)in->display);
    lua_setfield(L, -2, "display");
    lua_pushinteger(L, (lua_Integer)in->presentation);
    lua_setfield(L, -2, "presentation");
    lua_pushnumber(L, (lua_Number)in->render_scale);
    lua_setfield(L, -2, "render_scale");
    lua_pushinteger(L, (lua_Integer)in->filter);
    lua_setfield(L, -2, "filter");
    lua_pushinteger(L, (lua_Integer)in->particles);
    lua_setfield(L, -2, "particles");
    lua_pushinteger(L, (lua_Integer)in->dynamic_lights);
    lua_setfield(L, -2, "dynamic_lights");
    lua_pushinteger(L, (lua_Integer)in->shadows);
    lua_setfield(L, -2, "shadows");
    lua_pushnumber(L, (lua_Number)in->bloom);
    lua_setfield(L, -2, "bloom");
    lua_pushnumber(L, (lua_Number)in->bloom_threshold);
    lua_setfield(L, -2, "bloom_threshold");
    lua_pushnumber(L, (lua_Number)in->crt);
    lua_setfield(L, -2, "crt");
    lua_pushnumber(L, (lua_Number)in->crt_curvature);
    lua_setfield(L, -2, "crt_curvature");
    lua_pushinteger(L, (lua_Integer)in->pixelation);
    lua_setfield(L, -2, "pixelation");
    lua_pushnumber(L, (lua_Number)in->chromatic_aberration);
    lua_setfield(L, -2, "chromatic_aberration");
    lua_pushinteger(L, (lua_Integer)in->antialias);
    lua_setfield(L, -2, "antialias");
    lua_pushnumber(L, (lua_Number)in->brightness);
    lua_setfield(L, -2, "brightness");
    lua_pushnumber(L, (lua_Number)in->contrast);
    lua_setfield(L, -2, "contrast");
    lua_pushnumber(L, (lua_Number)in->saturation);
    lua_setfield(L, -2, "saturation");
    lua_pushinteger(L, (lua_Integer)in->color_blind);
    lua_setfield(L, -2, "color_blind");
    lua_pushboolean(L, (int)in->reduced_flashing);
    lua_setfield(L, -2, "reduced_flashing");
    lua_pushnumber(L, (lua_Number)in->screen_shake);
    lua_setfield(L, -2, "screen_shake");
    lua_pushnumber(L, (lua_Number)in->ui_scale);
    lua_setfield(L, -2, "ui_scale");
}

static void GenRead_SDL_FColor(lua_State *L, int idx, SDL_FColor *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->r = (float)GrappleGen_LuaFieldNum(L, idx, "r");
    out->g = (float)GrappleGen_LuaFieldNum(L, idx, "g");
    out->b = (float)GrappleGen_LuaFieldNum(L, idx, "b");
    out->a = (float)GrappleGen_LuaFieldNum(L, idx, "a");
}

static void GenPush_SDL_FColor(lua_State *L, const SDL_FColor *in)
{
    lua_createtable(L, 0, 4);
    lua_pushnumber(L, (lua_Number)in->r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, (lua_Number)in->g);
    lua_setfield(L, -2, "g");
    lua_pushnumber(L, (lua_Number)in->b);
    lua_setfield(L, -2, "b");
    lua_pushnumber(L, (lua_Number)in->a);
    lua_setfield(L, -2, "a");
}

static void GenRead_Grapple_Light(lua_State *L, int idx, Grapple_Light *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->x = (float)GrappleGen_LuaFieldNum(L, idx, "x");
    out->y = (float)GrappleGen_LuaFieldNum(L, idx, "y");
    out->radius = (float)GrappleGen_LuaFieldNum(L, idx, "radius");
    lua_getfield(L, idx, "color");
    GenRead_SDL_FColor(L, lua_gettop(L), &out->color);
    lua_pop(L, 1);
    out->falloff = (float)GrappleGen_LuaFieldNum(L, idx, "falloff");
    out->angle = (float)GrappleGen_LuaFieldNum(L, idx, "angle");
    out->spread = (float)GrappleGen_LuaFieldNum(L, idx, "spread");
    out->flicker = (float)GrappleGen_LuaFieldNum(L, idx, "flicker");
    out->seed = (Uint32)GrappleGen_LuaFieldInt(L, idx, "seed");
    out->no_shadows = (bool)GrappleGen_LuaFieldBool(L, idx, "no_shadows");
}

static void GenRead_Grapple_LightDef(lua_State *L, int idx, Grapple_LightDef *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->radius = (float)GrappleGen_LuaFieldNum(L, idx, "radius");
    lua_getfield(L, idx, "color");
    GenRead_SDL_FColor(L, lua_gettop(L), &out->color);
    lua_pop(L, 1);
    out->offset_x = (float)GrappleGen_LuaFieldNum(L, idx, "offset_x");
    out->offset_y = (float)GrappleGen_LuaFieldNum(L, idx, "offset_y");
    out->falloff = (float)GrappleGen_LuaFieldNum(L, idx, "falloff");
    out->cone_direction = (float)GrappleGen_LuaFieldNum(L, idx, "cone_direction");
    out->cone_width = (float)GrappleGen_LuaFieldNum(L, idx, "cone_width");
    out->flicker = (float)GrappleGen_LuaFieldNum(L, idx, "flicker");
    out->no_shadows = (bool)GrappleGen_LuaFieldBool(L, idx, "no_shadows");
    out->enabled = (bool)GrappleGen_LuaFieldBool(L, idx, "enabled");
}

static void GenPush_Grapple_LightDef(lua_State *L, const Grapple_LightDef *in)
{
    lua_createtable(L, 0, 10);
    lua_pushnumber(L, (lua_Number)in->radius);
    lua_setfield(L, -2, "radius");
    GenPush_SDL_FColor(L, &in->color);
    lua_setfield(L, -2, "color");
    lua_pushnumber(L, (lua_Number)in->offset_x);
    lua_setfield(L, -2, "offset_x");
    lua_pushnumber(L, (lua_Number)in->offset_y);
    lua_setfield(L, -2, "offset_y");
    lua_pushnumber(L, (lua_Number)in->falloff);
    lua_setfield(L, -2, "falloff");
    lua_pushnumber(L, (lua_Number)in->cone_direction);
    lua_setfield(L, -2, "cone_direction");
    lua_pushnumber(L, (lua_Number)in->cone_width);
    lua_setfield(L, -2, "cone_width");
    lua_pushnumber(L, (lua_Number)in->flicker);
    lua_setfield(L, -2, "flicker");
    lua_pushboolean(L, (int)in->no_shadows);
    lua_setfield(L, -2, "no_shadows");
    lua_pushboolean(L, (int)in->enabled);
    lua_setfield(L, -2, "enabled");
}

static void GenRead_Grapple_PhysicsDrawConfig(lua_State *L, int idx, Grapple_PhysicsDrawConfig *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->pixels_per_meter = (float)GrappleGen_LuaFieldNum(L, idx, "pixels_per_meter");
    out->offset_x = (float)GrappleGen_LuaFieldNum(L, idx, "offset_x");
    out->offset_y = (float)GrappleGen_LuaFieldNum(L, idx, "offset_y");
    out->draw_joints = (bool)GrappleGen_LuaFieldBool(L, idx, "draw_joints");
}

static void GenPush_Grapple_RayHit(lua_State *L, const Grapple_RayHit *in)
{
    lua_createtable(L, 0, 7);
    lua_pushboolean(L, (int)in->hit);
    lua_setfield(L, -2, "hit");
    lua_pushinteger(L, (lua_Integer)in->actor);
    lua_setfield(L, -2, "actor");
    lua_pushnumber(L, (lua_Number)in->x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, (lua_Number)in->y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, (lua_Number)in->normal_x);
    lua_setfield(L, -2, "normal_x");
    lua_pushnumber(L, (lua_Number)in->normal_y);
    lua_setfield(L, -2, "normal_y");
    lua_pushnumber(L, (lua_Number)in->fraction);
    lua_setfield(L, -2, "fraction");
}

static void GenPush_Grapple_RenderStats(lua_State *L, const Grapple_RenderStats *in)
{
    lua_createtable(L, 0, 3);
    lua_pushinteger(L, (lua_Integer)in->considered);
    lua_setfield(L, -2, "considered");
    lua_pushinteger(L, (lua_Integer)in->culled);
    lua_setfield(L, -2, "culled");
    lua_pushinteger(L, (lua_Integer)in->drawn);
    lua_setfield(L, -2, "drawn");
}

static void GenPush_Grapple_SaveInfo(lua_State *L, const Grapple_SaveInfo *in)
{
    lua_createtable(L, 0, 4);
    lua_pushboolean(L, (int)in->exists);
    lua_setfield(L, -2, "exists");
    lua_pushinteger(L, (lua_Integer)in->size);
    lua_setfield(L, -2, "size");
    lua_pushinteger(L, (lua_Integer)in->modified);
    lua_setfield(L, -2, "modified");
    lua_createtable(L, (int)(128), 0);
    for (int gi = 0; gi < (int)(128); ++gi) {
        lua_pushinteger(L, (lua_Integer)in->label[gi]);
        lua_rawseti(L, -2, gi + 1);
    }
    lua_setfield(L, -2, "label");
}

static void GenRead_SDL_Color(lua_State *L, int idx, SDL_Color *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->r = (Uint8)GrappleGen_LuaFieldInt(L, idx, "r");
    out->g = (Uint8)GrappleGen_LuaFieldInt(L, idx, "g");
    out->b = (Uint8)GrappleGen_LuaFieldInt(L, idx, "b");
    out->a = (Uint8)GrappleGen_LuaFieldInt(L, idx, "a");
}

static void GenRead_b2BodyId(lua_State *L, int idx, b2BodyId *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->index1 = (int32_t)GrappleGen_LuaFieldInt(L, idx, "index1");
    out->world0 = (uint16_t)GrappleGen_LuaFieldInt(L, idx, "world0");
    out->generation = (uint16_t)GrappleGen_LuaFieldInt(L, idx, "generation");
}

static void GenRead_b2WorldId(lua_State *L, int idx, b2WorldId *out)
{
    memset(out, 0, sizeof(*out));
    if (!lua_istable(L, idx)) { return; }
    out->index1 = (uint16_t)GrappleGen_LuaFieldInt(L, idx, "index1");
    out->generation = (uint16_t)GrappleGen_LuaFieldInt(L, idx, "generation");
}

static void GenDtor_Grapple_FreeTiledMap(void *p)
{
    Grapple_TiledMap *typed = (Grapple_TiledMap *)p;
    Grapple_FreeTiledMap(typed);
}

static void GenDtor_Grapple_DestroyGui(void *p)
{
    Grapple_Gui *typed = (Grapple_Gui *)p;
    Grapple_DestroyGui(typed);
}

static void GenDtor_Grapple_ConfigDestroy(void *p)
{
    Grapple_EngineConfig *typed = (Grapple_EngineConfig *)p;
    Grapple_ConfigDestroy(typed);
}

static void GenDtor_Grapple_DestroyRegex(void *p)
{
    Grapple_Regex *typed = (Grapple_Regex *)p;
    Grapple_DestroyRegex(typed);
}

static void GenDtor_Grapple_DestroyLightScene(void *p)
{
    Grapple_LightScene *typed = (Grapple_LightScene *)p;
    Grapple_DestroyLightScene(typed);
}

static int GenL_Grapple_ActionBind(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_Binding a2;
    GenRead_Grapple_Binding(L, 3, &a2);
    bool rv = Grapple_ActionBind(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindAxis(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_GamepadAxis a2 = (Grapple_GamepadAxis)luaL_checkinteger(L, 3);
    int a3 = (int)luaL_checkinteger(L, 4);
    bool rv = Grapple_ActionBindAxis(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindDirection(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_Direction a2 = (Grapple_Direction)luaL_checkinteger(L, 3);
    bool rv = Grapple_ActionBindDirection(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindKey(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    SDL_Scancode a2 = (SDL_Scancode)luaL_checkinteger(L, 3);
    bool rv = Grapple_ActionBindKey(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindKeySigned(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    SDL_Scancode a2 = (SDL_Scancode)luaL_checkinteger(L, 3);
    int a3 = (int)luaL_checkinteger(L, 4);
    bool rv = Grapple_ActionBindKeySigned(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindMouse(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_MouseButton a2 = (Grapple_MouseButton)luaL_checkinteger(L, 3);
    bool rv = Grapple_ActionBindMouse(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindPad(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)luaL_checkinteger(L, 3);
    bool rv = Grapple_ActionBindPad(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionBindingAt(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    Grapple_Binding out3;
    memset(&out3, 0, sizeof(out3));
    bool rv = Grapple_ActionBindingAt(a0, a1, a2, &out3);
    lua_pushboolean(L, (int)rv);
    GenPush_Grapple_Binding(L, &out3);
    return 2;
}

static int GenL_Grapple_ActionBindingCount(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int rv = Grapple_ActionBindingCount(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActionCapture(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_Binding out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ActionCapture(a0, a1, &out2);
    lua_pushboolean(L, (int)rv);
    GenPush_Grapple_Binding(L, &out2);
    return 2;
}

static int GenL_Grapple_ActionClear(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActionClear(a0, a1);
    return 0;
}

static int GenL_Grapple_ActionCount(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    int rv = Grapple_ActionCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActionDown(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActionMap");
    int a2 = (int)luaL_checkinteger(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_ActionDown(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionMapCreate(lua_State *L)
{
    (void)L;
    Grapple_ActionMap * rv = Grapple_ActionMapCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_ActionMap");
    return 1;
}

static int GenL_Grapple_ActionMapDestroy(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    Grapple_ActionMapDestroy(a0);
    return 0;
}

static int GenL_Grapple_ActionMapKeyboardPlayer(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    int rv = Grapple_ActionMapKeyboardPlayer(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActionMapLoad(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_ActionMapLoad(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionMapLoadToml(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ActionMapLoadToml(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionMapSave(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_ActionMapSave(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionMapSetKeyboardPlayer(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_ActionMapSetKeyboardPlayer(a0, a1);
    return 0;
}

static int GenL_Grapple_ActionMapToToml(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    char * rv = Grapple_ActionMapToToml(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_ActionName(lua_State *L)
{
    (void)L;
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActionMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_ActionName(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_ActionPressed(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActionMap");
    int a2 = (int)luaL_checkinteger(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_ActionPressed(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionReleased(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActionMap");
    int a2 = (int)luaL_checkinteger(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_ActionReleased(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActionValue(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActionMap");
    int a2 = (int)luaL_checkinteger(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    float rv = Grapple_ActionValue(a0, a1, a2, a3);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_ActionVector(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActionMap");
    int a2 = (int)luaL_checkinteger(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    const char *a4 = lua_isnoneornil(L, 5) ? NULL : luaL_checkstring(L, 5);
    float io5 = (float)luaL_optnumber(L, 6, 0);
    float io6 = (float)luaL_optnumber(L, 7, 0);
    Grapple_ActionVector(a0, a1, a2, a3, a4, &io5, &io6);
    lua_pushnumber(L, (lua_Number)io5);
    lua_pushnumber(L, (lua_Number)io6);
    return 2;
}

static int GenL_Grapple_ActorAddBody(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_BodyDef tmp1;
    const Grapple_BodyDef *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_BodyDef(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_ActorAddBody(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorAddLight(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_LightDef tmp1;
    const Grapple_LightDef *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_LightDef(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_ActorAddLight(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorAlive(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    bool rv = Grapple_ActorAlive(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorAngularVelocity(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float rv = Grapple_ActorAngularVelocity(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_ActorApplyForce(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorApplyForce(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorApplyImpulse(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorApplyImpulse(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorBodyBounds(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    SDL_FRect out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ActorBodyBounds(a0, &out1);
    lua_pushboolean(L, (int)rv);
    GenPush_SDL_FRect(L, &out1);
    return 2;
}

static int GenL_Grapple_ActorBroadcast(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorTags a2 = (Grapple_ActorTags)luaL_checkinteger(L, 3);
    Grapple_ActorMessage tmp3;
    const Grapple_ActorMessage *a3 = NULL;
    if (!lua_isnoneornil(L, 4)) {
        GenRead_Grapple_ActorMessage(L, 4, &tmp3);
        a3 = &tmp3;
    }
    int rv = Grapple_ActorBroadcast(a0, a1, a2, a3);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorChild(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_ActorId rv = Grapple_ActorChild(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorChildCount(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    int rv = Grapple_ActorChildCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorClear(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActorClear(a0);
    return 0;
}

static int GenL_Grapple_ActorClearSprite(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorClearSprite(a0);
    return 0;
}

static int GenL_Grapple_ActorCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_ActorCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorDefCreate(lua_State *L)
{
    (void)L;
    Grapple_ActorDef * rv = Grapple_ActorDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_ActorDef");
    return 1;
}

static int GenL_Grapple_ActorDefDestroy(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    Grapple_ActorDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_ActorDefSetName(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorDefSetName(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDefSetParent(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    Grapple_ActorDefSetParent(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDefSetPosition(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorDefSetPosition(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorDefSetRotation(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_ActorDefSetRotation(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDefSetScale(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorDefSetScale(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorDefSetStateSize(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_ActorDefSetStateSize(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDefSetTags(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    Grapple_ActorTags a1 = (Grapple_ActorTags)luaL_checkinteger(L, 2);
    Grapple_ActorDefSetTags(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDefSetType(lua_State *L)
{
    (void)L;
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_ActorDef");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorDefSetType(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorDestroy(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    Grapple_ActorDestroy(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorEnabled(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    bool rv = Grapple_ActorEnabled(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorEngine(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_Engine * rv = Grapple_ActorEngine(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Engine");
    return 1;
}

static int GenL_Grapple_ActorFindByName(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorId rv = Grapple_ActorFindByName(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorFindByType(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorId rv = Grapple_ActorFindByType(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorGet(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    Grapple_Actor * rv = Grapple_ActorGet(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Actor");
    return 1;
}

static int GenL_Grapple_ActorGetId(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorId rv = Grapple_ActorGetId(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorGetTags(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTags rv = Grapple_ActorGetTags(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorHasBody(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    bool rv = Grapple_ActorHasBody(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorHasTags(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTags a1 = (Grapple_ActorTags)luaL_checkinteger(L, 2);
    bool rv = Grapple_ActorHasTags(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorLocal(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTransform rv = Grapple_ActorLocal(a0);
    GenPush_Grapple_ActorTransform(L, &rv);
    return 1;
}

static int GenL_Grapple_ActorMove(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorMove(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorName(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    const char * rv = Grapple_ActorName(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_ActorParent(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorId rv = Grapple_ActorParent(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorQuery(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ActorTags a2 = (Grapple_ActorTags)luaL_checkinteger(L, 3);
    Grapple_ActorId io3 = (Grapple_ActorId)luaL_optinteger(L, 4, 0);
    int a4 = (int)luaL_checkinteger(L, 5);
    int rv = Grapple_ActorQuery(a0, a1, a2, &io3, a4);
    lua_pushinteger(L, (lua_Integer)rv);
    lua_pushinteger(L, (lua_Integer)io3);
    return 2;
}

static int GenL_Grapple_ActorRemoveBody(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorRemoveBody(a0);
    return 0;
}

static int GenL_Grapple_ActorRemoveLight(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorRemoveLight(a0);
    return 0;
}

static int GenL_Grapple_ActorRenderTransform(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_ActorTransform rv = Grapple_ActorRenderTransform(a0, a1);
    GenPush_Grapple_ActorTransform(L, &rv);
    return 1;
}

static int GenL_Grapple_ActorSend(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    Grapple_ActorMessage tmp2;
    const Grapple_ActorMessage *a2 = NULL;
    if (!lua_isnoneornil(L, 3)) {
        GenRead_Grapple_ActorMessage(L, 3, &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_ActorSend(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorSetAngularVelocity(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_ActorSetAngularVelocity(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorSetEnabled(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ActorSetEnabled(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorSetLocal(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTransform tmp1;
    const Grapple_ActorTransform *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_ActorTransform(L, 2, &tmp1);
        a1 = &tmp1;
    }
    Grapple_ActorSetLocal(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorSetParent(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorId a1 = (Grapple_ActorId)luaL_checkinteger(L, 2);
    bool rv = Grapple_ActorSetParent(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorSetPosition(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorSetPosition(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorSetSprite(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    const Grapple_Sprite *a1 = (const Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_Sprite");
    bool rv = Grapple_ActorSetSprite(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ActorSetTags(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTags a1 = (Grapple_ActorTags)luaL_checkinteger(L, 2);
    Grapple_ActorSetTags(a0, a1);
    return 0;
}

static int GenL_Grapple_ActorSetVelocity(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorSetVelocity(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorSpawn(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const Grapple_ActorDef *a1 = (const Grapple_ActorDef *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_ActorDef");
    Grapple_ActorId rv = Grapple_ActorSpawn(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_ActorSprite(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_Sprite * rv = Grapple_ActorSprite(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Sprite");
    return 1;
}

static int GenL_Grapple_ActorTeleport(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorTeleport(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorTeleportBody(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_ActorTeleportBody(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ActorType(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    const char * rv = Grapple_ActorType(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_ActorVelocity(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    Grapple_ActorVelocity(a0, &io1, &io2);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    return 2;
}

static int GenL_Grapple_ActorWakeBody(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorWakeBody(a0);
    return 0;
}

static int GenL_Grapple_ActorWorld(lua_State *L)
{
    (void)L;
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Actor");
    Grapple_ActorTransform rv = Grapple_ActorWorld(a0);
    GenPush_Grapple_ActorTransform(L, &rv);
    return 1;
}

static int GenL_Grapple_AddDarkZone(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    SDL_FColor a2;
    GenRead_SDL_FColor(L, 3, &a2);
    bool rv = Grapple_AddDarkZone(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AddLight(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    Grapple_Light tmp1;
    const Grapple_Light *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_Light(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_AddLight(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AddOccluderRect(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    bool rv = Grapple_AddOccluderRect(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AddOccluderSegment(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    bool rv = Grapple_AddOccluderSegment(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AnyInput(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_AnyInput(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AssetPath(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)luaL_checkinteger(L, 2);
    const char * rv = Grapple_AssetPath(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_AssetRelease(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)luaL_checkinteger(L, 2);
    Grapple_AssetRelease(a0, a1);
    return 0;
}

static int GenL_Grapple_AssetRetain(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)luaL_checkinteger(L, 2);
    Grapple_AssetRetain(a0, a1);
    return 0;
}

static int GenL_Grapple_AssetStatusOf(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)luaL_checkinteger(L, 2);
    Grapple_AssetStatus rv = Grapple_AssetStatusOf(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_AssetsFrameBudget(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_AssetsFrameBudget(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_AssetsLoaded(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_AssetsLoaded(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_AssetsPending(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_AssetsPending(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_AssetsProgress(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_AssetsProgress(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_AssetsReady(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_AssetsReady(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_AssetsSetFrameBudget(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_AssetsSetFrameBudget(a0, a1);
    return 0;
}

static int GenL_Grapple_AssetsSetWorkers(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_AssetsSetWorkers(a0, a1);
    return 0;
}

static int GenL_Grapple_AssetsWait(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_AssetsWait(a0);
    return 0;
}

static int GenL_Grapple_BidiBaseIsRTL(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_BidiBaseIsRTL(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_BindingFromString(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    Grapple_Binding out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_BindingFromString(a0, &out1);
    lua_pushboolean(L, (int)rv);
    GenPush_Grapple_Binding(L, &out1);
    return 2;
}

static int GenL_Grapple_BindingToString(lua_State *L)
{
    (void)L;
    Grapple_Binding a0;
    GenRead_Grapple_Binding(L, 1, &a0);
    const char *src1 = lua_isnoneornil(L, 2) ? "" : luaL_checkstring(L, 2);
    char *a1 = SDL_strdup(src1);
    size_t a2 = (size_t)luaL_checkinteger(L, 3);
    const char * rv = Grapple_BindingToString(a0, a1, a2);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    SDL_free(a1);
    return 1;
}

static int GenL_Grapple_BodyDefDestroy(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_BodyDefDestroy(&out0);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetBullet(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)lua_toboolean(L, 1);
    Grapple_BodyDefSetBullet(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetFilter(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 1);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 2);
    Grapple_BodyDefSetFilter(&out0, a1, a2);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetFixedRotation(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)lua_toboolean(L, 1);
    Grapple_BodyDefSetFixedRotation(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetGravityScale(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    Grapple_BodyDefSetGravityScale(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetMaterial(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    float a3 = (float)luaL_checknumber(L, 3);
    Grapple_BodyDefSetMaterial(&out0, a1, a2, a3);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetOffset(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_BodyDefSetOffset(&out0, a1, a2);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetSensor(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)lua_toboolean(L, 1);
    Grapple_BodyDefSetSensor(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetShape(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_ShapeType a1 = (Grapple_ShapeType)luaL_checkinteger(L, 1);
    Grapple_BodyDefSetShape(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetSize(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_BodyDefSetSize(&out0, a1, a2);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefSetType(lua_State *L)
{
    (void)L;
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_BodyType a1 = (Grapple_BodyType)luaL_checkinteger(L, 1);
    Grapple_BodyDefSetType(&out0, a1);
    GenPush_Grapple_BodyDef(L, &out0);
    return 1;
}

static int GenL_Grapple_BodyDefault(lua_State *L)
{
    (void)L;
    Grapple_BodyDef rv = Grapple_BodyDefault();
    GenPush_Grapple_BodyDef(L, &rv);
    return 1;
}

static int GenL_Grapple_CameraBegin(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_Camera(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_CameraBegin(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_CameraDestroy(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_CameraDestroy(&out0);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraEnd(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_CameraEnd(a0);
    return 0;
}

static int GenL_Grapple_CameraFollow(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_CameraFollow(&out0, a1, a2);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraInit(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_Engine *a1 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_CameraInit(&out0, a1);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraPoint(lua_State *L)
{
    (void)L;
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_Camera(L, 1, &tmp0);
        a0 = &tmp0;
    }
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    Grapple_CameraPoint(a0, a1, a2, &io3, &io4);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 2;
}

static int GenL_Grapple_CameraRect(lua_State *L)
{
    (void)L;
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_Camera(L, 1, &tmp0);
        a0 = &tmp0;
    }
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    SDL_FRect rv = Grapple_CameraRect(a0, a1);
    GenPush_SDL_FRect(L, &rv);
    return 1;
}

static int GenL_Grapple_CameraScreenToWorld(lua_State *L)
{
    (void)L;
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_Camera(L, 1, &tmp0);
        a0 = &tmp0;
    }
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    bool rv = Grapple_CameraScreenToWorld(a0, a1, a2, &io3, &io4);
    lua_pushboolean(L, (int)rv);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 3;
}

static int GenL_Grapple_CameraSetBounds(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    float a3 = (float)luaL_checknumber(L, 3);
    float a4 = (float)luaL_checknumber(L, 4);
    Grapple_CameraSetBounds(&out0, a1, a2, a3, a4);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSetDeadzone(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_CameraSetDeadzone(&out0, a1, a2);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSetSmoothing(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    Grapple_CameraSetSmoothing(&out0, a1);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSetViewport(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    float a3 = (float)luaL_checknumber(L, 3);
    float a4 = (float)luaL_checknumber(L, 4);
    Grapple_CameraSetViewport(&out0, a1, a2, a3, a4);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSetZoom(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    Grapple_CameraSetZoom(&out0, a1);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraShake(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_CameraShake(&out0, a1, a2);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSnap(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_CameraSnap(&out0, a1, a2);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraSplit(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_SplitMode a1 = (Grapple_SplitMode)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_Camera out4;
    memset(&out4, 0, sizeof(out4));
    int rv = Grapple_CameraSplit(a0, a1, a2, a3, &out4);
    lua_pushinteger(L, (lua_Integer)rv);
    GenPush_Grapple_Camera(L, &out4);
    return 2;
}

static int GenL_Grapple_CameraUpdate(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_Engine *a1 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_CameraUpdate(&out0, a1, a2);
    GenPush_Grapple_Camera(L, &out0);
    return 1;
}

static int GenL_Grapple_CameraVisible(lua_State *L)
{
    (void)L;
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_Camera(L, 1, &tmp0);
        a0 = &tmp0;
    }
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    bool rv = Grapple_CameraVisible(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_CameraX(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float rv = Grapple_CameraX(&out0);
    lua_pushnumber(L, (lua_Number)rv);
    GenPush_Grapple_Camera(L, &out0);
    return 2;
}

static int GenL_Grapple_CameraY(lua_State *L)
{
    (void)L;
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float rv = Grapple_CameraY(&out0);
    lua_pushnumber(L, (lua_Number)rv);
    GenPush_Grapple_Camera(L, &out0);
    return 2;
}

static int GenL_Grapple_CompileRegex(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_Regex * rv = Grapple_CompileRegex(a0, a1);
    GrappleGen_LuaPushOwned(L, (void *)rv, "Grapple_Regex", GenDtor_Grapple_DestroyRegex);
    return 1;
}

static int GenL_Grapple_ConfigCreate(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig * rv = Grapple_ConfigCreate();
    GrappleGen_LuaPushOwned(L, (void *)rv, "Grapple_EngineConfig", GenDtor_Grapple_ConfigDestroy);
    return 1;
}

static int GenL_Grapple_ConfigDestroy(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaTakeHandle(L, 1, "Grapple_EngineConfig");
    Grapple_ConfigDestroy(a0);
    return 0;
}

static int GenL_Grapple_ConfigSetAutoMount(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetAutoMount(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetBackend(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    Grapple_EngineBackend a1 = (Grapple_EngineBackend)luaL_checkinteger(L, 2);
    Grapple_ConfigSetBackend(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetDesignSize(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    int a1 = (int)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    Grapple_ConfigSetDesignSize(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_ConfigSetFullscreen(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetFullscreen(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetGraphics(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_GraphicsSettings(L, 2, &tmp1);
        a1 = &tmp1;
    }
    Grapple_ConfigSetGraphics(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetHeadless(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetHeadless(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetHighDpi(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetHighDpi(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetManualClock(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetManualClock(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetMaxFps(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_ConfigSetMaxFps(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetMediaPath(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ConfigSetMediaPath(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetPresentation(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    Grapple_EnginePresentation a1 = (Grapple_EnginePresentation)luaL_checkinteger(L, 2);
    Grapple_ConfigSetPresentation(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetResizable(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetResizable(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetTickRate(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_ConfigSetTickRate(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetTitle(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_ConfigSetTitle(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetVsync(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_ConfigSetVsync(a0, a1);
    return 0;
}

static int GenL_Grapple_ConfigSetWindowSize(lua_State *L)
{
    (void)L;
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    int a1 = (int)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    Grapple_ConfigSetWindowSize(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_CountSignalConnections(lua_State *L)
{
    (void)L;
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_SignalEmitter");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int rv = Grapple_CountSignalConnections(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_CreateChipSFX(lua_State *L)
{
    (void)L;
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_LuaCheckHandle(L, 1, "MIX_Mixer");
    Grapple_ChipSFX a1 = (Grapple_ChipSFX)luaL_checkinteger(L, 2);
    MIX_Audio * rv = Grapple_CreateChipSFX(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "MIX_Audio");
    return 1;
}

static int GenL_Grapple_CreateChipTone(lua_State *L)
{
    (void)L;
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_LuaCheckHandle(L, 1, "MIX_Mixer");
    Grapple_ChipToneDesc tmp1;
    const Grapple_ChipToneDesc *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_ChipToneDesc(L, 2, &tmp1);
        a1 = &tmp1;
    }
    MIX_Audio * rv = Grapple_CreateChipTone(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "MIX_Audio");
    return 1;
}

static int GenL_Grapple_CreateChipTune(lua_State *L)
{
    (void)L;
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_LuaCheckHandle(L, 1, "MIX_Mixer");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    MIX_Audio * rv = Grapple_CreateChipTune(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "MIX_Audio");
    return 1;
}

static int GenL_Grapple_CreateEngine(lua_State *L)
{
    (void)L;
    const Grapple_EngineConfig *a0 = (const Grapple_EngineConfig *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_EngineConfig");
    Grapple_Engine * rv = Grapple_CreateEngine(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Engine");
    return 1;
}

static int GenL_Grapple_CreateGui(lua_State *L)
{
    (void)L;
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Renderer");
    size_t len1 = 0;
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checklstring(L, 2, &len1);
    float a3 = (float)luaL_checknumber(L, 3);
    Grapple_Gui * rv = Grapple_CreateGui(a0, (const void *)a1, (size_t)len1, a3);
    GrappleGen_LuaPushOwned(L, (void *)rv, "Grapple_Gui", GenDtor_Grapple_DestroyGui);
    return 1;
}

static int GenL_Grapple_CreateGuiWithGlyphs(lua_State *L)
{
    (void)L;
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Renderer");
    size_t len1 = 0;
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checklstring(L, 2, &len1);
    float a3 = (float)luaL_checknumber(L, 3);
    Grapple_GuiGlyphRange a4 = (Grapple_GuiGlyphRange)luaL_checkinteger(L, 4);
    Grapple_Gui * rv = Grapple_CreateGuiWithGlyphs(a0, (const void *)a1, (size_t)len1, a3, a4);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Gui");
    return 1;
}

static int GenL_Grapple_CreateLightScene(lua_State *L)
{
    (void)L;
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Renderer");
    Grapple_LightScene * rv = Grapple_CreateLightScene(a0);
    GrappleGen_LuaPushOwned(L, (void *)rv, "Grapple_LightScene", GenDtor_Grapple_DestroyLightScene);
    return 1;
}

static int GenL_Grapple_CreateSignalEmitter(lua_State *L)
{
    (void)L;
    Grapple_SignalEmitter * rv = Grapple_CreateSignalEmitter();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_SignalEmitter");
    return 1;
}

static int GenL_Grapple_CryptoSelfTest(lua_State *L)
{
    (void)L;
    bool rv = Grapple_CryptoSelfTest();
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_DayNightAmbient(lua_State *L)
{
    (void)L;
    float a0 = (float)luaL_checknumber(L, 1);
    SDL_FColor rv = Grapple_DayNightAmbient(a0);
    GenPush_SDL_FColor(L, &rv);
    return 1;
}

static int GenL_Grapple_DayNightSunlight(lua_State *L)
{
    (void)L;
    float a0 = (float)luaL_checknumber(L, 1);
    float rv = Grapple_DayNightSunlight(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_DestroyEngine(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_DestroyEngine(a0);
    return 0;
}

static int GenL_Grapple_DestroyGui(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaTakeHandle(L, 1, "Grapple_Gui");
    Grapple_DestroyGui(a0);
    return 0;
}

static int GenL_Grapple_DestroyLightScene(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaTakeHandle(L, 1, "Grapple_LightScene");
    Grapple_DestroyLightScene(a0);
    return 0;
}

static int GenL_Grapple_DestroyRegex(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaTakeHandle(L, 1, "Grapple_Regex");
    Grapple_DestroyRegex(a0);
    return 0;
}

static int GenL_Grapple_DestroySignalEmitter(lua_State *L)
{
    (void)L;
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_SignalEmitter");
    Grapple_DestroySignalEmitter(a0);
    return 0;
}

static int GenL_Grapple_DeviceAccelerometer(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    Grapple_DeviceAccelerometer(a0, &io1, &io2, &io3);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    lua_pushnumber(L, (lua_Number)io3);
    return 3;
}

static int GenL_Grapple_DeviceGyro(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    Grapple_DeviceGyro(a0, &io1, &io2, &io3);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    lua_pushnumber(L, (lua_Number)io3);
    return 3;
}

static int GenL_Grapple_DialogDeliverSave(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    bool rv = Grapple_DialogDeliverSave(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_DialogPath(lua_State *L)
{
    (void)L;
    const char * rv = Grapple_DialogPath();
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_DialogReset(lua_State *L)
{
    (void)L;
    Grapple_DialogReset();
    return 0;
}

static int GenL_Grapple_DialogStatus(lua_State *L)
{
    (void)L;
    Grapple_DialogState rv = Grapple_DialogStatus();
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_DisconnectSignal(lua_State *L)
{
    (void)L;
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_SignalEmitter");
    Uint64 a1 = (Uint64)luaL_checkinteger(L, 2);
    bool rv = Grapple_DisconnectSignal(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_DistanceJointDefCreate(lua_State *L)
{
    (void)L;
    b2DistanceJointDef * rv = Grapple_DistanceJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2DistanceJointDef");
    return 1;
}

static int GenL_Grapple_DistanceJointDefDestroy(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    Grapple_DistanceJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetAnchors(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_DistanceJointDefSetAnchors(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_DistanceJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetCollideConnected(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_DistanceJointDefSetCollideConnected(a0, a1);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetLength(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_DistanceJointDefSetLength(a0, a1);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetLimit(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_DistanceJointDefSetLimit(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_DistanceJointDefSetSpring(lua_State *L)
{
    (void)L;
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2DistanceJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_DistanceJointDefSetSpring(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_DrawPhysicsWorld(lua_State *L)
{
    (void)L;
    b2WorldId a0;
    GenRead_b2WorldId(L, 1, &a0);
    SDL_Renderer *a1 = (SDL_Renderer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Renderer");
    Grapple_PhysicsDrawConfig tmp2;
    const Grapple_PhysicsDrawConfig *a2 = NULL;
    if (!lua_isnoneornil(L, 3)) {
        GenRead_Grapple_PhysicsDrawConfig(L, 3, &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_DrawPhysicsWorld(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EncodeDataBase64(lua_State *L)
{
    (void)L;
    size_t len0 = 0;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checklstring(L, 1, &len0);
    int io2 = (int)luaL_optinteger(L, 2, 0);
    char * rv = Grapple_EncodeDataBase64((const void *)a0, (int)len0, &io2);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    lua_pushinteger(L, (lua_Integer)io2);
    return 2;
}

static int GenL_Grapple_EngineAdvance(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Uint64 a1 = (Uint64)luaL_checkinteger(L, 2);
    Grapple_EngineAdvance(a0, a1);
    return 0;
}

static int GenL_Grapple_EngineAlpha(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineAlpha(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineAssetScale(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineAssetScale(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineDelta(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineDelta(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineDesignSize(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int io1 = (int)luaL_optinteger(L, 2, 0);
    int io2 = (int)luaL_optinteger(L, 3, 0);
    Grapple_EngineDesignSize(a0, &io1, &io2);
    lua_pushinteger(L, (lua_Integer)io1);
    lua_pushinteger(L, (lua_Integer)io2);
    return 2;
}

static int GenL_Grapple_EngineDisplay(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineDisplay(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineDisplayCount(lua_State *L)
{
    (void)L;
    int rv = Grapple_EngineDisplayCount();
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineDisplayName(lua_State *L)
{
    (void)L;
    int a0 = (int)luaL_checkinteger(L, 1);
    const char * rv = Grapple_EngineDisplayName(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_EngineEffectsAvailable(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_EngineEffectsAvailable(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineEmbedMedia(lua_State *L)
{
    (void)L;
    size_t len0 = 0;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checklstring(L, 1, &len0);
    const char *a2 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_EngineEmbedMedia((const void *)a0, (int)len0, a2);
    return 0;
}

static int GenL_Grapple_EngineFps(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineFps(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineFrameCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Uint64 rv = Grapple_EngineFrameCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineMaxFps(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineMaxFps(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineMediaPath(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char * rv = Grapple_EngineMediaPath(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_EngineMediaSource(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_MediaSource rv = Grapple_EngineMediaSource(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineOverloadFrames(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineOverloadFrames(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EnginePixelSize(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int io1 = (int)luaL_optinteger(L, 2, 0);
    int io2 = (int)luaL_optinteger(L, 3, 0);
    Grapple_EnginePixelSize(a0, &io1, &io2);
    lua_pushinteger(L, (lua_Integer)io1);
    lua_pushinteger(L, (lua_Integer)io2);
    return 2;
}

static int GenL_Grapple_EnginePresentation_(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_EnginePresentation rv = Grapple_EnginePresentation_(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineQuit(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_EngineQuit(a0);
    return 0;
}

static int GenL_Grapple_EngineRenderScale(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineRenderScale(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineRenderer(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Renderer * rv = Grapple_EngineRenderer(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_Renderer");
    return 1;
}

static int GenL_Grapple_EngineSafeRect(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FRect rv = Grapple_EngineSafeRect(a0);
    GenPush_SDL_FRect(L, &rv);
    return 1;
}

static int GenL_Grapple_EngineSetClearColor(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(L, 2, &a1);
    Grapple_EngineSetClearColor(a0, a1);
    return 0;
}

static int GenL_Grapple_EngineSetDisplay(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_EngineSetDisplay(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineSetGraphics(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_GraphicsSettings(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_EngineSetGraphics(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineSetMaxFps(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_EngineSetMaxFps(a0, a1);
    return 0;
}

static int GenL_Grapple_EngineSetMediaPassword(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    Grapple_EngineSetMediaPassword(a0);
    return 0;
}

static int GenL_Grapple_EngineSetPresentation(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_EnginePresentation a1 = (Grapple_EnginePresentation)luaL_checkinteger(L, 2);
    bool rv = Grapple_EngineSetPresentation(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineSetRefreshRate(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_EngineSetRefreshRate(a0, a1);
    return 0;
}

static int GenL_Grapple_EngineSetTickRate(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_EngineSetTickRate(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineSetTimeScale(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_EngineSetTimeScale(a0, a1);
    return 0;
}

static int GenL_Grapple_EngineShowWindow(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_EngineShowWindow(a0);
    return 0;
}

static int GenL_Grapple_EngineStep(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineStep(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineStepsLastFrame(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineStepsLastFrame(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineTick(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_EngineTick(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EngineTickRate(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_EngineTickRate(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EngineTimeScale(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_EngineTimeScale(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EngineViewRect(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FRect rv = Grapple_EngineViewRect(a0);
    GenPush_SDL_FRect(L, &rv);
    return 1;
}

static int GenL_Grapple_EngineWindow(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Window * rv = Grapple_EngineWindow(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_Window");
    return 1;
}

static int GenL_Grapple_EngineWindowToDesign(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    Grapple_EngineWindowToDesign(a0, a1, a2, &io3, &io4);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 2;
}

static int GenL_Grapple_EventCreate(lua_State *L)
{
    (void)L;
    SDL_Event * rv = Grapple_EventCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_Event");
    return 1;
}

static int GenL_Grapple_EventDestroy(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Grapple_EventDestroy(a0);
    return 0;
}

static int GenL_Grapple_EventGamepadAxis(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    int rv = Grapple_EventGamepadAxis(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventGamepadAxisValue(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventGamepadAxisValue(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventGamepadButton(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    int rv = Grapple_EventGamepadButton(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventGamepadWhich(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Sint32 rv = Grapple_EventGamepadWhich(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventKeyModifiers(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Uint16 rv = Grapple_EventKeyModifiers(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventKeyRepeat(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    bool rv = Grapple_EventKeyRepeat(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_EventKeyScancode(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    int rv = Grapple_EventKeyScancode(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventMouseButton(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    int rv = Grapple_EventMouseButton(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventMouseDeltaX(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventMouseDeltaX(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventMouseDeltaY(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventMouseDeltaY(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventMouseX(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventMouseX(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventMouseY(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventMouseY(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventSetType(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Grapple_EventSetType(a0, a1);
    return 0;
}

static int GenL_Grapple_EventText(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    const char * rv = Grapple_EventText(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_EventTouchX(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventTouchX(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventTouchY(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventTouchY(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventType(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Uint32 rv = Grapple_EventType(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_EventWheelX(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventWheelX(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventWheelY(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    float rv = Grapple_EventWheelY(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_EventWindowId(lua_State *L)
{
    (void)L;
    SDL_Event *a0 = (SDL_Event *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Event");
    Uint32 rv = Grapple_EventWindowId(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_FilterJointDefCreate(lua_State *L)
{
    (void)L;
    b2FilterJointDef * rv = Grapple_FilterJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2FilterJointDef");
    return 1;
}

static int GenL_Grapple_FilterJointDefDestroy(lua_State *L)
{
    (void)L;
    b2FilterJointDef *a0 = (b2FilterJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2FilterJointDef");
    Grapple_FilterJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_FilterJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2FilterJointDef *a0 = (b2FilterJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2FilterJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_FilterJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_FingerCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_FingerCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_FreeTiledMap(lua_State *L)
{
    (void)L;
    Grapple_TiledMap *a0 = (Grapple_TiledMap *)GrappleGen_LuaTakeHandle(L, 1, "Grapple_TiledMap");
    Grapple_FreeTiledMap(a0);
    return 0;
}

static int GenL_Grapple_GPUAcquireSwapchain(lua_State *L)
{
    (void)L;
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUCommandBuffer");
    SDL_Window *a1 = (SDL_Window *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Window");
    SDL_GPUTexture * rv = Grapple_GPUAcquireSwapchain(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTexture");
    return 1;
}

static int GenL_Grapple_GPUBeginComputePass(lua_State *L)
{
    (void)L;
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUCommandBuffer");
    Grapple_GPUComputeBindings *a1 = (Grapple_GPUComputeBindings *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_GPUComputeBindings");
    SDL_GPUComputePass * rv = Grapple_GPUBeginComputePass(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUComputePass");
    return 1;
}

static int GenL_Grapple_GPUBindComputeStorageBuffer(lua_State *L)
{
    (void)L;
    SDL_GPUComputePass *a0 = (SDL_GPUComputePass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUBuffer");
    Grapple_GPUBindComputeStorageBuffer(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBindComputeStorageTexture(lua_State *L)
{
    (void)L;
    SDL_GPUComputePass *a0 = (SDL_GPUComputePass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUTexture");
    Grapple_GPUBindComputeStorageTexture(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBindFragmentStorageBuffer(lua_State *L)
{
    (void)L;
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUBuffer");
    Grapple_GPUBindFragmentStorageBuffer(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBindFragmentStorageTexture(lua_State *L)
{
    (void)L;
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUTexture");
    Grapple_GPUBindFragmentStorageTexture(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBindVertexStorageBuffer(lua_State *L)
{
    (void)L;
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUBuffer");
    Grapple_GPUBindVertexStorageBuffer(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBindVertexStorageTexture(lua_State *L)
{
    (void)L;
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUTexture");
    Grapple_GPUBindVertexStorageTexture(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBlitInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUBlitInfo * rv = Grapple_GPUBlitInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUBlitInfo");
    return 1;
}

static int GenL_Grapple_GPUBlitInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBlitInfo");
    Grapple_GPUBlitInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUBlitInfoSetDestination(lua_State *L)
{
    (void)L;
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBlitInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Uint32 a5 = (Uint32)luaL_checkinteger(L, 6);
    Grapple_GPUBlitInfoSetDestination(a0, a1, a2, a3, a4, a5);
    return 0;
}

static int GenL_Grapple_GPUBlitInfoSetFilter(lua_State *L)
{
    (void)L;
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBlitInfo");
    SDL_GPUFilter a1 = (SDL_GPUFilter)luaL_checkinteger(L, 2);
    Grapple_GPUBlitInfoSetFilter(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUBlitInfoSetSource(lua_State *L)
{
    (void)L;
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBlitInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Uint32 a5 = (Uint32)luaL_checkinteger(L, 6);
    Grapple_GPUBlitInfoSetSource(a0, a1, a2, a3, a4, a5);
    return 0;
}

static int GenL_Grapple_GPUBufferBindingCreate(lua_State *L)
{
    (void)L;
    SDL_GPUBufferBinding * rv = Grapple_GPUBufferBindingCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUBufferBinding");
    return 1;
}

static int GenL_Grapple_GPUBufferBindingDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUBufferBinding *a0 = (SDL_GPUBufferBinding *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferBinding");
    Grapple_GPUBufferBindingDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUBufferBindingSet(lua_State *L)
{
    (void)L;
    SDL_GPUBufferBinding *a0 = (SDL_GPUBufferBinding *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferBinding");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Grapple_GPUBufferBindingSet(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBufferLocationCreate(lua_State *L)
{
    (void)L;
    SDL_GPUBufferLocation * rv = Grapple_GPUBufferLocationCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUBufferLocation");
    return 1;
}

static int GenL_Grapple_GPUBufferLocationDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUBufferLocation *a0 = (SDL_GPUBufferLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferLocation");
    Grapple_GPUBufferLocationDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUBufferLocationSet(lua_State *L)
{
    (void)L;
    SDL_GPUBufferLocation *a0 = (SDL_GPUBufferLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferLocation");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Grapple_GPUBufferLocationSet(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUBufferRegionCreate(lua_State *L)
{
    (void)L;
    SDL_GPUBufferRegion * rv = Grapple_GPUBufferRegionCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUBufferRegion");
    return 1;
}

static int GenL_Grapple_GPUBufferRegionDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUBufferRegion *a0 = (SDL_GPUBufferRegion *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferRegion");
    Grapple_GPUBufferRegionDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUBufferRegionSet(lua_State *L)
{
    (void)L;
    SDL_GPUBufferRegion *a0 = (SDL_GPUBufferRegion *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUBufferRegion");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Grapple_GPUBufferRegionSet(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo * rv = Grapple_GPUColorTargetInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUColorTargetInfo");
    return 1;
}

static int GenL_Grapple_GPUColorTargetInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    Grapple_GPUColorTargetInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoSetClearColor(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_GPUColorTargetInfoSetClearColor(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoSetCycle(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_GPUColorTargetInfoSetCycle(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoSetMipLayer(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Grapple_GPUColorTargetInfoSetMipLayer(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoSetOps(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    SDL_GPULoadOp a1 = (SDL_GPULoadOp)luaL_checkinteger(L, 2);
    SDL_GPUStoreOp a2 = (SDL_GPUStoreOp)luaL_checkinteger(L, 3);
    Grapple_GPUColorTargetInfoSetOps(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUColorTargetInfoSetTexture(lua_State *L)
{
    (void)L;
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUColorTargetInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Grapple_GPUColorTargetInfoSetTexture(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUComputeBindingsAddBuffer(lua_State *L)
{
    (void)L;
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GPUComputeBindings");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUBuffer");
    bool a2 = (bool)lua_toboolean(L, 3);
    bool rv = Grapple_GPUComputeBindingsAddBuffer(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUComputeBindingsAddTexture(lua_State *L)
{
    (void)L;
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GPUComputeBindings");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    bool a4 = (bool)lua_toboolean(L, 5);
    bool rv = Grapple_GPUComputeBindingsAddTexture(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUComputeBindingsCreate(lua_State *L)
{
    (void)L;
    Grapple_GPUComputeBindings * rv = Grapple_GPUComputeBindingsCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_GPUComputeBindings");
    return 1;
}

static int GenL_Grapple_GPUComputeBindingsDestroy(lua_State *L)
{
    (void)L;
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GPUComputeBindings");
    Grapple_GPUComputeBindingsDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUComputePipelineInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo * rv = Grapple_GPUComputePipelineInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUComputePipelineCreateInfo");
    return 1;
}

static int GenL_Grapple_GPUComputePipelineInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePipelineCreateInfo");
    Grapple_GPUComputePipelineInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUComputePipelineInfoSetCode(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePipelineCreateInfo");
    size_t len1 = 0;
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checklstring(L, 2, &len1);
    Grapple_GPUComputePipelineInfoSetCode(a0, (const void *)a1, (int)len1);
    return 0;
}

static int GenL_Grapple_GPUComputePipelineInfoSetEntrypoint(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePipelineCreateInfo");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_GPUComputePipelineInfoSetEntrypoint(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUComputePipelineInfoSetFormat(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePipelineCreateInfo");
    SDL_GPUShaderFormat a1 = (SDL_GPUShaderFormat)luaL_checkinteger(L, 2);
    Grapple_GPUComputePipelineInfoSetFormat(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUComputePipelineInfoSetThreadCount(lua_State *L)
{
    (void)L;
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUComputePipelineCreateInfo");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Grapple_GPUComputePipelineInfoSetThreadCount(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_GPUDepthStencilTargetInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUDepthStencilTargetInfo * rv = Grapple_GPUDepthStencilTargetInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUDepthStencilTargetInfo");
    return 1;
}

static int GenL_Grapple_GPUDepthStencilTargetInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDepthStencilTargetInfo");
    Grapple_GPUDepthStencilTargetInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUDepthStencilTargetInfoSetClear(lua_State *L)
{
    (void)L;
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDepthStencilTargetInfo");
    float a1 = (float)luaL_checknumber(L, 2);
    Uint8 a2 = (Uint8)luaL_checkinteger(L, 3);
    Grapple_GPUDepthStencilTargetInfoSetClear(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUDepthStencilTargetInfoSetOps(lua_State *L)
{
    (void)L;
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDepthStencilTargetInfo");
    SDL_GPULoadOp a1 = (SDL_GPULoadOp)luaL_checkinteger(L, 2);
    SDL_GPUStoreOp a2 = (SDL_GPUStoreOp)luaL_checkinteger(L, 3);
    Grapple_GPUDepthStencilTargetInfoSetOps(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUDepthStencilTargetInfoSetTexture(lua_State *L)
{
    (void)L;
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDepthStencilTargetInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Grapple_GPUDepthStencilTargetInfoSetTexture(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUPipelineInfoAddColorTarget(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUTextureFormat a1 = (SDL_GPUTextureFormat)luaL_checkinteger(L, 2);
    bool rv = Grapple_GPUPipelineInfoAddColorTarget(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUPipelineInfoAddVertexAttribute(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    SDL_GPUVertexElementFormat a3 = (SDL_GPUVertexElementFormat)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    bool rv = Grapple_GPUPipelineInfoAddVertexAttribute(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUPipelineInfoAddVertexBuffer(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    SDL_GPUVertexInputRate a3 = (SDL_GPUVertexInputRate)luaL_checkinteger(L, 4);
    bool rv = Grapple_GPUPipelineInfoAddVertexBuffer(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUPipelineInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo * rv = Grapple_GPUPipelineInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUGraphicsPipelineCreateInfo");
    return 1;
}

static int GenL_Grapple_GPUPipelineInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    Grapple_GPUPipelineInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUPipelineInfoSetDepthStencil(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUTextureFormat a1 = (SDL_GPUTextureFormat)luaL_checkinteger(L, 2);
    bool a2 = (bool)lua_toboolean(L, 3);
    Grapple_GPUPipelineInfoSetDepthStencil(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUPipelineInfoSetFillMode(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUFillMode a1 = (SDL_GPUFillMode)luaL_checkinteger(L, 2);
    SDL_GPUCullMode a2 = (SDL_GPUCullMode)luaL_checkinteger(L, 3);
    Grapple_GPUPipelineInfoSetFillMode(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUPipelineInfoSetPrimitive(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUPrimitiveType a1 = (SDL_GPUPrimitiveType)luaL_checkinteger(L, 2);
    Grapple_GPUPipelineInfoSetPrimitive(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUPipelineInfoSetShaders(lua_State *L)
{
    (void)L;
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUShader *a1 = (SDL_GPUShader *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUShader");
    SDL_GPUShader *a2 = (SDL_GPUShader *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUShader");
    Grapple_GPUPipelineInfoSetShaders(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPURenderStateInfoAddSampler(lua_State *L)
{
    (void)L;
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderStateCreateInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    SDL_GPUSampler *a2 = (SDL_GPUSampler *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUSampler");
    bool rv = Grapple_GPURenderStateInfoAddSampler(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPURenderStateInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPURenderStateCreateInfo * rv = Grapple_GPURenderStateInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPURenderStateCreateInfo");
    return 1;
}

static int GenL_Grapple_GPURenderStateInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderStateCreateInfo");
    Grapple_GPURenderStateInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPURenderStateInfoSetShader(lua_State *L)
{
    (void)L;
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPURenderStateCreateInfo");
    SDL_GPUShader *a1 = (SDL_GPUShader *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUShader");
    Grapple_GPURenderStateInfoSetShader(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUShaderCreateInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo * rv = Grapple_GPUShaderCreateInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUShaderCreateInfo");
    return 1;
}

static int GenL_Grapple_GPUShaderCreateInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUShaderCreateInfo");
    Grapple_GPUShaderCreateInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUShaderCreateInfoSetCode(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUShaderCreateInfo");
    size_t len1 = 0;
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checklstring(L, 2, &len1);
    Grapple_GPUShaderCreateInfoSetCode(a0, (const void *)a1, (int)len1);
    return 0;
}

static int GenL_Grapple_GPUShaderCreateInfoSetCounts(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUShaderCreateInfo");
    Uint32 a1 = (Uint32)luaL_checkinteger(L, 2);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Grapple_GPUShaderCreateInfoSetCounts(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_GPUShaderCreateInfoSetEntrypoint(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUShaderCreateInfo");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_GPUShaderCreateInfoSetEntrypoint(a0, a1);
    return 0;
}

static int GenL_Grapple_GPUShaderCreateInfoSetFormat(lua_State *L)
{
    (void)L;
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUShaderCreateInfo");
    SDL_GPUShaderFormat a1 = (SDL_GPUShaderFormat)luaL_checkinteger(L, 2);
    SDL_GPUShaderStage a2 = (SDL_GPUShaderStage)luaL_checkinteger(L, 3);
    Grapple_GPUShaderCreateInfoSetFormat(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUSwapchainHeight(lua_State *L)
{
    (void)L;
    Uint32 rv = Grapple_GPUSwapchainHeight();
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GPUSwapchainWidth(lua_State *L)
{
    (void)L;
    Uint32 rv = Grapple_GPUSwapchainWidth();
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GPUTextureLocationCreate(lua_State *L)
{
    (void)L;
    SDL_GPUTextureLocation * rv = Grapple_GPUTextureLocationCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTextureLocation");
    return 1;
}

static int GenL_Grapple_GPUTextureLocationDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUTextureLocation *a0 = (SDL_GPUTextureLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureLocation");
    Grapple_GPUTextureLocationDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUTextureLocationSet(lua_State *L)
{
    (void)L;
    SDL_GPUTextureLocation *a0 = (SDL_GPUTextureLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureLocation");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Grapple_GPUTextureLocationSet(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_GPUTextureRegionCreate(lua_State *L)
{
    (void)L;
    SDL_GPUTextureRegion * rv = Grapple_GPUTextureRegionCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTextureRegion");
    return 1;
}

static int GenL_Grapple_GPUTextureRegionDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUTextureRegion *a0 = (SDL_GPUTextureRegion *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureRegion");
    Grapple_GPUTextureRegionDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUTextureRegionSet(lua_State *L)
{
    (void)L;
    SDL_GPUTextureRegion *a0 = (SDL_GPUTextureRegion *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureRegion");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Uint32 a5 = (Uint32)luaL_checkinteger(L, 6);
    Grapple_GPUTextureRegionSet(a0, a1, a2, a3, a4, a5);
    return 0;
}

static int GenL_Grapple_GPUTextureSamplerBindingCreate(lua_State *L)
{
    (void)L;
    SDL_GPUTextureSamplerBinding * rv = Grapple_GPUTextureSamplerBindingCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTextureSamplerBinding");
    return 1;
}

static int GenL_Grapple_GPUTextureSamplerBindingDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUTextureSamplerBinding *a0 = (SDL_GPUTextureSamplerBinding *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureSamplerBinding");
    Grapple_GPUTextureSamplerBindingDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUTextureSamplerBindingSet(lua_State *L)
{
    (void)L;
    SDL_GPUTextureSamplerBinding *a0 = (SDL_GPUTextureSamplerBinding *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureSamplerBinding");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTexture");
    SDL_GPUSampler *a2 = (SDL_GPUSampler *)GrappleGen_LuaCheckHandle(L, 3, "SDL_GPUSampler");
    Grapple_GPUTextureSamplerBindingSet(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUTextureTransferInfoCreate(lua_State *L)
{
    (void)L;
    SDL_GPUTextureTransferInfo * rv = Grapple_GPUTextureTransferInfoCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTextureTransferInfo");
    return 1;
}

static int GenL_Grapple_GPUTextureTransferInfoDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUTextureTransferInfo *a0 = (SDL_GPUTextureTransferInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureTransferInfo");
    Grapple_GPUTextureTransferInfoDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUTextureTransferInfoSet(lua_State *L)
{
    (void)L;
    SDL_GPUTextureTransferInfo *a0 = (SDL_GPUTextureTransferInfo *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTextureTransferInfo");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Uint32 a3 = (Uint32)luaL_checkinteger(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    Grapple_GPUTextureTransferInfoSet(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_GPUTransferBufferLocationCreate(lua_State *L)
{
    (void)L;
    SDL_GPUTransferBufferLocation * rv = Grapple_GPUTransferBufferLocationCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTransferBufferLocation");
    return 1;
}

static int GenL_Grapple_GPUTransferBufferLocationDestroy(lua_State *L)
{
    (void)L;
    SDL_GPUTransferBufferLocation *a0 = (SDL_GPUTransferBufferLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTransferBufferLocation");
    Grapple_GPUTransferBufferLocationDestroy(a0);
    return 0;
}

static int GenL_Grapple_GPUTransferBufferLocationSet(lua_State *L)
{
    (void)L;
    SDL_GPUTransferBufferLocation *a0 = (SDL_GPUTransferBufferLocation *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUTransferBufferLocation");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Grapple_GPUTransferBufferLocationSet(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GPUUploadToTransferBuffer(lua_State *L)
{
    (void)L;
    SDL_GPUDevice *a0 = (SDL_GPUDevice *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDevice");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    size_t len3 = 0;
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checklstring(L, 4, &len3);
    bool a5 = (bool)lua_toboolean(L, 5);
    bool rv = Grapple_GPUUploadToTransferBuffer(a0, a1, a2, (const void *)a3, (int)len3, a5);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GPUWaitAndAcquireSwapchain(lua_State *L)
{
    (void)L;
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUCommandBuffer");
    SDL_Window *a1 = (SDL_Window *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Window");
    SDL_GPUTexture * rv = Grapple_GPUWaitAndAcquireSwapchain(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_GPUTexture");
    return 1;
}

static int GenL_Grapple_GPUWaitForFence(lua_State *L)
{
    (void)L;
    SDL_GPUDevice *a0 = (SDL_GPUDevice *)GrappleGen_LuaCheckHandle(L, 1, "SDL_GPUDevice");
    SDL_GPUFence *a1 = (SDL_GPUFence *)GrappleGen_LuaCheckHandle(L, 2, "SDL_GPUFence");
    bool rv = Grapple_GPUWaitForFence(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadAccelerometer(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    Grapple_GamepadAccelerometer(a0, a1, &io2, &io3, &io4);
    lua_pushnumber(L, (lua_Number)io2);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 3;
}

static int GenL_Grapple_GamepadAxisValue(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GamepadAxis a2 = (Grapple_GamepadAxis)luaL_checkinteger(L, 3);
    float rv = Grapple_GamepadAxisValue(a0, a1, a2);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GamepadButtonDown(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)luaL_checkinteger(L, 3);
    bool rv = Grapple_GamepadButtonDown(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadButtonPressed(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)luaL_checkinteger(L, 3);
    bool rv = Grapple_GamepadButtonPressed(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadButtonReleased(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)luaL_checkinteger(L, 3);
    bool rv = Grapple_GamepadButtonReleased(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadConnected(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_GamepadConnected(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_GamepadCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GamepadDeadzone(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_GamepadDeadzone(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GamepadDirectionPressed(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_Direction a2 = (Grapple_Direction)luaL_checkinteger(L, 3);
    bool rv = Grapple_GamepadDirectionPressed(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadDirectionRepeat(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_Direction a2 = (Grapple_Direction)luaL_checkinteger(L, 3);
    bool rv = Grapple_GamepadDirectionRepeat(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadGyro(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    Grapple_GamepadGyro(a0, a1, &io2, &io3, &io4);
    lua_pushnumber(L, (lua_Number)io2);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 3;
}

static int GenL_Grapple_GamepadHasAccelerometer(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_GamepadHasAccelerometer(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadHasGyro(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_GamepadHasGyro(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadName(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_GamepadName(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_GamepadRumble(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    bool rv = Grapple_GamepadRumble(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadRumbleTriggers(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Uint32 a4 = (Uint32)luaL_checkinteger(L, 5);
    bool rv = Grapple_GamepadRumbleTriggers(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadSetLED(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Uint8 a2 = (Uint8)luaL_checkinteger(L, 3);
    Uint8 a3 = (Uint8)luaL_checkinteger(L, 4);
    Uint8 a4 = (Uint8)luaL_checkinteger(L, 5);
    bool rv = Grapple_GamepadSetLED(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GamepadStick(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    float io3 = (float)luaL_optnumber(L, 4, 0);
    float io4 = (float)luaL_optnumber(L, 5, 0);
    Grapple_GamepadStick(a0, a1, a2, &io3, &io4);
    lua_pushnumber(L, (lua_Number)io3);
    lua_pushnumber(L, (lua_Number)io4);
    return 2;
}

static int GenL_Grapple_GamepadStopRumble(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GamepadStopRumble(a0, a1);
    return 0;
}

static int GenL_Grapple_GraphicsClamp(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_GraphicsClamp(&out0);
    GenPush_Grapple_GraphicsSettings(L, &out0);
    return 1;
}

static int GenL_Grapple_GraphicsConfigError(lua_State *L)
{
    (void)L;
    const char * rv = Grapple_GraphicsConfigError();
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_GraphicsConfigPath(lua_State *L)
{
    (void)L;
    const char * rv = Grapple_GraphicsConfigPath();
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_GraphicsDefaults(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings rv = Grapple_GraphicsDefaults();
    GenPush_Grapple_GraphicsSettings(L, &rv);
    return 1;
}

static int GenL_Grapple_GraphicsEqual(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_GraphicsSettings(L, 1, &tmp0);
        a0 = &tmp0;
    }
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_GraphicsSettings(L, 2, &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_GraphicsEqual(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GraphicsLightMapScale(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    float rv = Grapple_GraphicsLightMapScale(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GraphicsLoadTomlFile(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    const char *a1 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    bool rv = Grapple_GraphicsLoadTomlFile(&out0, a1);
    lua_pushboolean(L, (int)rv);
    GenPush_Grapple_GraphicsSettings(L, &out0);
    return 2;
}

static int GenL_Grapple_GraphicsLoadTomlString(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    const char *a1 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    bool rv = Grapple_GraphicsLoadTomlString(&out0, a1);
    lua_pushboolean(L, (int)rv);
    GenPush_Grapple_GraphicsSettings(L, &out0);
    return 2;
}

static int GenL_Grapple_GraphicsMaxDynamicLights(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    int rv = Grapple_GraphicsMaxDynamicLights(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GraphicsParticleDensity(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    float rv = Grapple_GraphicsParticleDensity(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GraphicsQualityFromName(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    Grapple_GraphicsQuality io1 = (Grapple_GraphicsQuality)luaL_optinteger(L, 2, 0);
    bool rv = Grapple_GraphicsQualityFromName(a0, &io1);
    lua_pushboolean(L, (int)rv);
    lua_pushinteger(L, (lua_Integer)io1);
    return 2;
}

static int GenL_Grapple_GraphicsQualityName(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    const char * rv = Grapple_GraphicsQualityName(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_GraphicsSafeMode(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings rv = Grapple_GraphicsSafeMode();
    GenPush_Grapple_GraphicsSettings(L, &rv);
    return 1;
}

static int GenL_Grapple_GraphicsSave(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_GraphicsSettings(L, 1, &tmp0);
        a0 = &tmp0;
    }
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_GraphicsSave(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GraphicsSavePath(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    char * rv = Grapple_GraphicsSavePath(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_GraphicsShadowRays(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    int rv = Grapple_GraphicsShadowRays(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GraphicsShadowSoftness(lua_State *L)
{
    (void)L;
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)luaL_checkinteger(L, 1);
    float rv = Grapple_GraphicsShadowSoftness(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GraphicsToToml(lua_State *L)
{
    (void)L;
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (!lua_isnoneornil(L, 1)) {
        GenRead_Grapple_GraphicsSettings(L, 1, &tmp0);
        a0 = &tmp0;
    }
    char * rv = Grapple_GraphicsToToml(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_GuiContext(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    struct nk_context * rv = Grapple_GuiContext(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "nk_context");
    return 1;
}

static int GenL_Grapple_GuiDrawCommandCount(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int rv = Grapple_GuiDrawCommandCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GuiDrawTexture(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Texture");
    SDL_FRect a2;
    GenRead_SDL_FRect(L, 3, &a2);
    Grapple_GuiImageMode a3 = (Grapple_GuiImageMode)luaL_checkinteger(L, 4);
    bool rv = Grapple_GuiDrawTexture(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiDrawTextureOverlay(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Texture");
    SDL_FRect a2;
    GenRead_SDL_FRect(L, 3, &a2);
    Grapple_GuiImageMode a3 = (Grapple_GuiImageMode)luaL_checkinteger(L, 4);
    bool rv = Grapple_GuiDrawTextureOverlay(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiFontHeight(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    float rv = Grapple_GuiFontHeight(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GuiGridBeginOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    bool rv = Grapple_GuiGridBeginOwned(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiGridCell(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    Grapple_GuiGridCell(a0);
    return 0;
}

static int GenL_Grapple_GuiGridCellOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiGridCellOwned(a0);
    return 0;
}

static int GenL_Grapple_GuiGridCellPart(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_GuiAlign a3 = (Grapple_GuiAlign)luaL_checkinteger(L, 4);
    Grapple_GuiGridCellPart(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_GuiGridCellPartOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_GuiAlign a3 = (Grapple_GuiAlign)luaL_checkinteger(L, 4);
    Grapple_GuiGridCellPartOwned(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_GuiGridCellSpan(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GuiGridCellSpan(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiGridCellSpanOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GuiGridCellSpanOwned(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiGridCreate(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid * rv = Grapple_GuiGridCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_GuiGrid");
    return 1;
}

static int GenL_Grapple_GuiGridDestroy(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    Grapple_GuiGridDestroy(a0);
    return 0;
}

static int GenL_Grapple_GuiGridEnd(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    Grapple_GuiGridEnd(a0);
    return 0;
}

static int GenL_Grapple_GuiGridEndOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiGridEndOwned(a0);
    return 0;
}

static int GenL_Grapple_GuiGridNextRow(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    Grapple_GuiGridNextRow(a0);
    return 0;
}

static int GenL_Grapple_GuiGridNextRowOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiGridNextRowOwned(a0);
    return 0;
}

static int GenL_Grapple_GuiGridRowHeight(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_GuiGridRowHeight(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiGridRowHeightOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_GuiGridRowHeightOwned(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiGridSpacing(lua_State *L)
{
    (void)L;
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_GuiGrid");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_GuiGridSpacing(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GuiGridSpacingOwned(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_GuiGridSpacingOwned(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_GuiGridWeight(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    bool rv = Grapple_GuiGridWeight(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiImage(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Texture");
    Grapple_GuiImageMode a2 = (Grapple_GuiImageMode)luaL_checkinteger(L, 3);
    bool rv = Grapple_GuiImage(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiImageButton(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Texture");
    Grapple_GuiImageMode a2 = (Grapple_GuiImageMode)luaL_checkinteger(L, 3);
    bool rv = Grapple_GuiImageButton(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiInputBegin(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiInputBegin(a0);
    return 0;
}

static int GenL_Grapple_GuiInputEnd(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiInputEnd(a0);
    return 0;
}

static int GenL_Grapple_GuiKeyPressed(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_GuiKeyPressed(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiMemoryUsed(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int rv = Grapple_GuiMemoryUsed(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GuiOpenFileButton(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_GuiOpenFileButton(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiPopFont(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GuiPopFont(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiPopStyleColor(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GuiPopStyleColor(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiProcessEvent(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    const SDL_Event *a1 = (const SDL_Event *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Event");
    bool rv = Grapple_GuiProcessEvent(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiPumpEvents(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    bool rv = Grapple_GuiPumpEvents(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiPushFont(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiFontSize a1 = (Grapple_GuiFontSize)luaL_checkinteger(L, 2);
    bool rv = Grapple_GuiPushFont(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiPushStyleColor(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiStyleColor a1 = (Grapple_GuiStyleColor)luaL_checkinteger(L, 2);
    SDL_Color a2;
    GenRead_SDL_Color(L, 3, &a2);
    bool rv = Grapple_GuiPushStyleColor(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiRender(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    bool rv = Grapple_GuiRender(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiRenderer(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    SDL_Renderer * rv = Grapple_GuiRenderer(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_Renderer");
    return 1;
}

static int GenL_Grapple_GuiSaveFileButton(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    size_t len3 = 0;
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checklstring(L, 4, &len3);
    bool rv = Grapple_GuiSaveFileButton(a0, a1, a2, (const void *)a3, (size_t)len3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiSavedPath(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    const char * rv = Grapple_GuiSavedPath(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_GuiScale(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    float rv = Grapple_GuiScale(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_GuiSetFont(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    Grapple_GuiFontSize a1 = (Grapple_GuiFontSize)luaL_checkinteger(L, 2);
    bool rv = Grapple_GuiSetFont(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiSetTooltipDelay(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_GuiSetTooltipDelay(a0, a1);
    return 0;
}

static int GenL_Grapple_GuiTooltip(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_GuiTooltip(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_GuiTooltipDelay(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    int rv = Grapple_GuiTooltipDelay(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_GuiWantsInput(lua_State *L)
{
    (void)L;
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Gui");
    bool rv = Grapple_GuiWantsInput(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_HMACSHA256(lua_State *L)
{
    (void)L;
    size_t len0 = 0;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checklstring(L, 1, &len0);
    size_t len2 = 0;
    const char *a2 = lua_isnoneornil(L, 2) ? NULL : luaL_checklstring(L, 2, &len2);
    Uint8 io4 = (Uint8)luaL_optinteger(L, 3, 0);
    bool rv = Grapple_HMACSHA256((const void *)a0, (size_t)len0, (const void *)a2, (size_t)len2, &io4);
    lua_pushboolean(L, (int)rv);
    lua_pushinteger(L, (lua_Integer)io4);
    return 2;
}

static int GenL_Grapple_HasDeviceMotion(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_HasDeviceMotion(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_IdleSeconds(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_IdleSeconds(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_KeyDown(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)luaL_checkinteger(L, 2);
    bool rv = Grapple_KeyDown(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_KeyModifiers(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Keymod rv = Grapple_KeyModifiers(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_KeyPressed(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)luaL_checkinteger(L, 2);
    bool rv = Grapple_KeyPressed(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_KeyReleased(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)luaL_checkinteger(L, 2);
    bool rv = Grapple_KeyReleased(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_LastInputDevice(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_InputDevice rv = Grapple_LastInputDevice(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_LightAddDarkZone(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    SDL_FColor a2;
    GenRead_SDL_FColor(L, 3, &a2);
    Grapple_LightAddDarkZone(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_LightAddOccluder(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    Grapple_LightAddOccluder(a0, a1);
    return 0;
}

static int GenL_Grapple_LightAddOccluderLine(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_LightAddOccluderLine(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_LightAmbient(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FColor rv = Grapple_LightAmbient(a0);
    GenPush_SDL_FColor(L, &rv);
    return 1;
}

static int GenL_Grapple_LightAt(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float rv = Grapple_LightAt(a0, a1, a2);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_LightBeginFrame(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_LightBeginFrame(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_LightCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_LightCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_LightDefDestroy(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_LightDefDestroy(&out0);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetColor(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    float a3 = (float)luaL_checknumber(L, 3);
    float a4 = (float)luaL_checknumber(L, 4);
    Grapple_LightDefSetColor(&out0, a1, a2, a3, a4);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetCone(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_LightDefSetCone(&out0, a1, a2);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetFlicker(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    Grapple_LightDefSetFlicker(&out0, a1);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetOffset(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    float a2 = (float)luaL_checknumber(L, 2);
    Grapple_LightDefSetOffset(&out0, a1, a2);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetRadius(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)luaL_checknumber(L, 1);
    Grapple_LightDefSetRadius(&out0, a1);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefSetShadows(lua_State *L)
{
    (void)L;
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)lua_toboolean(L, 1);
    Grapple_LightDefSetShadows(&out0, a1);
    GenPush_Grapple_LightDef(L, &out0);
    return 1;
}

static int GenL_Grapple_LightDefault(lua_State *L)
{
    (void)L;
    Grapple_LightDef rv = Grapple_LightDefault();
    GenPush_Grapple_LightDef(L, &rv);
    return 1;
}

static int GenL_Grapple_LightHour(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_LightHour(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_LightLineOfSight(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    bool rv = Grapple_LightLineOfSight(a0, a1, a2, a3, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_LightPreset_(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_LightPreset rv = Grapple_LightPreset_(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_LightRender(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_Camera(L, 2, &tmp1);
        a1 = &tmp1;
    }
    float a2 = (float)luaL_checknumber(L, 3);
    bool rv = Grapple_LightRender(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_LightSetAmbient(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(L, 2, &a1);
    Grapple_LightSetAmbient(a0, a1);
    return 0;
}

static int GenL_Grapple_LightSetAutoOccluders(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_LightSetAutoOccluders(a0, a1);
    return 0;
}

static int GenL_Grapple_LightSetClock(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_LightSetClock(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_LightSetPreset(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_LightPreset a1 = (Grapple_LightPreset)luaL_checkinteger(L, 2);
    Grapple_LightSetPreset(a0, a1);
    return 0;
}

static int GenL_Grapple_LightSunlight(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_LightSunlight(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_LightUsesShaders(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    bool rv = Grapple_LightUsesShaders(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_LoadTextFile(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    char * rv = Grapple_LoadTextFile(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_LoadTexture(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_TextureId rv = Grapple_LoadTexture(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_LoadTextureAsync(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_TextureId rv = Grapple_LoadTextureAsync(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_LoadTiledMap(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    Grapple_TiledMap * rv = Grapple_LoadTiledMap(a0);
    GrappleGen_LuaPushOwned(L, (void *)rv, "Grapple_TiledMap", GenDtor_Grapple_FreeTiledMap);
    return 1;
}

static int GenL_Grapple_MotorJointDefCreate(lua_State *L)
{
    (void)L;
    b2MotorJointDef * rv = Grapple_MotorJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2MotorJointDef");
    return 1;
}

static int GenL_Grapple_MotorJointDefDestroy(lua_State *L)
{
    (void)L;
    b2MotorJointDef *a0 = (b2MotorJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MotorJointDef");
    Grapple_MotorJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_MotorJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2MotorJointDef *a0 = (b2MotorJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MotorJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_MotorJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_MountEncryptedArchive(lua_State *L)
{
    (void)L;
    size_t len0 = 0;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checklstring(L, 1, &len0);
    const char *a2 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a3 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_MountEncryptedArchive((const void *)a0, (int)len0, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MountEncryptedArchiveFile(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_MountEncryptedArchiveFile(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MountMedia(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_MediaSource rv = Grapple_MountMedia(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_MouseCaptured(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_MouseCaptured(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MouseDelta(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    Grapple_MouseDelta(a0, &io1, &io2);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    return 2;
}

static int GenL_Grapple_MouseDown(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)luaL_checkinteger(L, 2);
    bool rv = Grapple_MouseDown(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MouseJointDefCreate(lua_State *L)
{
    (void)L;
    b2MouseJointDef * rv = Grapple_MouseJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2MouseJointDef");
    return 1;
}

static int GenL_Grapple_MouseJointDefDestroy(lua_State *L)
{
    (void)L;
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MouseJointDef");
    Grapple_MouseJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_MouseJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MouseJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_MouseJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_MouseJointDefSetMaxForce(lua_State *L)
{
    (void)L;
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MouseJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_MouseJointDefSetMaxForce(a0, a1);
    return 0;
}

static int GenL_Grapple_MouseJointDefSetSpring(lua_State *L)
{
    (void)L;
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2MouseJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_MouseJointDefSetSpring(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_MousePosition(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    Grapple_MousePosition(a0, &io1, &io2);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    return 2;
}

static int GenL_Grapple_MousePressed(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)luaL_checkinteger(L, 2);
    bool rv = Grapple_MousePressed(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MouseReleased(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)luaL_checkinteger(L, 2);
    bool rv = Grapple_MouseReleased(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_MouseWheel(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    Grapple_MouseWheel(a0, &io1, &io2);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    return 2;
}

static int GenL_Grapple_OpenVFSRead(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    SDL_IOStream * rv = Grapple_OpenVFSRead(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_IOStream");
    return 1;
}

static int GenL_Grapple_PhysicsBodyCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_PhysicsBodyCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_PhysicsGravity(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float io1 = (float)luaL_optnumber(L, 2, 0);
    float io2 = (float)luaL_optnumber(L, 3, 0);
    Grapple_PhysicsGravity(a0, &io1, &io2);
    lua_pushnumber(L, (lua_Number)io1);
    lua_pushnumber(L, (lua_Number)io2);
    return 2;
}

static int GenL_Grapple_PhysicsOverlap(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(L, 2, &a1);
    Uint32 a2 = (Uint32)luaL_checkinteger(L, 3);
    Grapple_ActorId io3 = (Grapple_ActorId)luaL_optinteger(L, 4, 0);
    int a4 = (int)luaL_checkinteger(L, 5);
    int rv = Grapple_PhysicsOverlap(a0, a1, a2, &io3, a4);
    lua_pushinteger(L, (lua_Integer)rv);
    lua_pushinteger(L, (lua_Integer)io3);
    return 2;
}

static int GenL_Grapple_PhysicsPaused(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_PhysicsPaused(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_PhysicsPixelsPerMetre(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_PhysicsPixelsPerMetre(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_PhysicsRaycast(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Uint32 a5 = (Uint32)luaL_checkinteger(L, 6);
    Grapple_RayHit rv = Grapple_PhysicsRaycast(a0, a1, a2, a3, a4, a5);
    GenPush_Grapple_RayHit(L, &rv);
    return 1;
}

static int GenL_Grapple_PhysicsSetGravity(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_PhysicsSetGravity(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_PhysicsSetPaused(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_PhysicsSetPaused(a0, a1);
    return 0;
}

static int GenL_Grapple_PhysicsSetPixelsPerMetre(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_PhysicsSetPixelsPerMetre(a0, a1);
    return 0;
}

static int GenL_Grapple_PhysicsSetSubSteps(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_PhysicsSetSubSteps(a0, a1);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefCreate(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef * rv = Grapple_PrismaticJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2PrismaticJointDef");
    return 1;
}

static int GenL_Grapple_PrismaticJointDefDestroy(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    Grapple_PrismaticJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefSetAnchors(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_PrismaticJointDefSetAnchors(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefSetAxis(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_PrismaticJointDefSetAxis(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_PrismaticJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefSetLimit(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_PrismaticJointDefSetLimit(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_PrismaticJointDefSetMotor(lua_State *L)
{
    (void)L;
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2PrismaticJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_PrismaticJointDefSetMotor(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_QuitDebugText(lua_State *L)
{
    (void)L;
    Grapple_QuitDebugText();
    return 0;
}

static int GenL_Grapple_RegexEscape(lua_State *L)
{
    (void)L;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
    char * rv = Grapple_RegexEscape(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_RegexFlags(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char * rv = Grapple_RegexFlags(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_RegexGroup(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_RegexGroup(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_RegexGroupBegin(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int a1 = (int)luaL_checkinteger(L, 2);
    int rv = Grapple_RegexGroupBegin(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RegexGroupCount(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int rv = Grapple_RegexGroupCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RegexGroupEnd(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int a1 = (int)luaL_checkinteger(L, 2);
    int rv = Grapple_RegexGroupEnd(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RegexMatchAt(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    bool rv = Grapple_RegexMatchAt(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_RegexNamedGroup(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int rv = Grapple_RegexNamedGroup(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RegexNamedGroupCount(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int rv = Grapple_RegexNamedGroupCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RegexNamedGroupName(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_RegexNamedGroupName(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_RegexPattern(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char * rv = Grapple_RegexPattern(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_RegexReplace(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool a3 = (bool)lua_toboolean(L, 4);
    const char * rv = Grapple_RegexReplace(a0, a1, a2, a3);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_RegexSearch(lua_State *L)
{
    (void)L;
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Regex");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    bool rv = Grapple_RegexSearch(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_RenderDebugText(lua_State *L)
{
    (void)L;
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Renderer");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_RenderDebugText(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_RenderLastStats(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_RenderStats rv = Grapple_RenderLastStats(a0);
    GenPush_Grapple_RenderStats(L, &rv);
    return 1;
}

static int GenL_Grapple_RenderLighting(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    bool rv = Grapple_RenderLighting(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_RenderOverlay(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    int rv = Grapple_RenderOverlay(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RenderWorld(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (!lua_isnoneornil(L, 2)) {
        GenRead_Grapple_Camera(L, 2, &tmp1);
        a1 = &tmp1;
    }
    float a2 = (float)luaL_checknumber(L, 3);
    int rv = Grapple_RenderWorld(a0, a1, a2);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_RevoluteJointDefCreate(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef * rv = Grapple_RevoluteJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2RevoluteJointDef");
    return 1;
}

static int GenL_Grapple_RevoluteJointDefDestroy(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    Grapple_RevoluteJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetAnchors(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_RevoluteJointDefSetAnchors(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_RevoluteJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetCollideConnected(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_RevoluteJointDefSetCollideConnected(a0, a1);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetLimit(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_RevoluteJointDefSetLimit(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetMotor(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_RevoluteJointDefSetMotor(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_RevoluteJointDefSetSpring(lua_State *L)
{
    (void)L;
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2RevoluteJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_RevoluteJointDefSetSpring(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_SHA256(lua_State *L)
{
    (void)L;
    size_t len0 = 0;
    const char *a0 = lua_isnoneornil(L, 1) ? NULL : luaL_checklstring(L, 1, &len0);
    Uint8 io2 = (Uint8)luaL_optinteger(L, 2, 0);
    bool rv = Grapple_SHA256((const void *)a0, (size_t)len0, &io2);
    lua_pushboolean(L, (int)rv);
    lua_pushinteger(L, (lua_Integer)io2);
    return 2;
}

static int GenL_Grapple_SampleLight(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float rv = Grapple_SampleLight(a0, a1, a2);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_SaveDelete(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_SaveDelete(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SaveExists(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool rv = Grapple_SaveExists(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SaveInfoOf(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_SaveInfo rv = Grapple_SaveInfoOf(a0, a1);
    GenPush_Grapple_SaveInfo(L, &rv);
    return 1;
}

static int GenL_Grapple_SavePath(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    char * rv = Grapple_SavePath(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    if (rv != NULL) { SDL_free(rv); }
    return 1;
}

static int GenL_Grapple_SaveSetIdentity(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    Grapple_SaveSetIdentity(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_SaveWrite(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    size_t len2 = 0;
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checklstring(L, 3, &len2);
    const char *a4 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_SaveWrite(a0, a1, (const void *)a2, (size_t)len2, a4);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneCurrent(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_Scene * rv = Grapple_SceneCurrent(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Scene");
    return 1;
}

static int GenL_Grapple_SceneDepth(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int rv = Grapple_SceneDepth(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_SceneEngine(lua_State *L)
{
    (void)L;
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Scene");
    Grapple_Engine * rv = Grapple_SceneEngine(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Engine");
    return 1;
}

static int GenL_Grapple_SceneFind(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_Scene * rv = Grapple_SceneFind(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Scene");
    return 1;
}

static int GenL_Grapple_SceneIsActive(lua_State *L)
{
    (void)L;
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Scene");
    bool rv = Grapple_SceneIsActive(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneKey(lua_State *L)
{
    (void)L;
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Scene");
    Sint64 rv = Grapple_SceneKey(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_SceneName(lua_State *L)
{
    (void)L;
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Scene");
    const char * rv = Grapple_SceneName(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_ScenePop(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_ScenePop(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScenePush(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_SceneDef");
    bool rv = Grapple_ScenePush(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneReplace(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_SceneDef");
    bool rv = Grapple_SceneReplace(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneReset(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_SceneDef");
    bool rv = Grapple_SceneReset(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneSetTransitionColor(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(L, 2, &a1);
    Grapple_SceneSetTransitionColor(a0, a1);
    return 0;
}

static int GenL_Grapple_SceneTransitionTo(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_LuaCheckHandle(L, 2, "Grapple_SceneDef");
    Grapple_SceneTransition a2 = (Grapple_SceneTransition)luaL_checkinteger(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    bool rv = Grapple_SceneTransitionTo(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SceneTransitioning(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_SceneTransitioning(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptHasHandlers(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_ScriptHasHandlers(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptRun(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool rv = Grapple_ScriptRun(a0);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneDefine(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ScriptSceneDefine(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneDefined(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ScriptSceneDefined(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptScenePush(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ScriptScenePush(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneReplace(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ScriptSceneReplace(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneReset(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_ScriptSceneReset(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneSetFlags(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_SceneFlags a2 = (Grapple_SceneFlags)luaL_checkinteger(L, 3);
    bool rv = Grapple_ScriptSceneSetFlags(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneSetHook(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_SceneHook a2 = (Grapple_SceneHook)luaL_checkinteger(L, 3);
    Sint64 a3 = (Sint64)luaL_checkinteger(L, 4);
    bool rv = Grapple_ScriptSceneSetHook(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneSetStateSize(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    bool rv = Grapple_ScriptSceneSetStateSize(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSceneTransitionTo(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_SceneTransition a2 = (Grapple_SceneTransition)luaL_checkinteger(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    bool rv = Grapple_ScriptSceneTransitionTo(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptSetHook(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ScriptHook a1 = (Grapple_ScriptHook)luaL_checkinteger(L, 2);
    Sint64 a2 = (Sint64)luaL_checkinteger(L, 3);
    bool rv = Grapple_ScriptSetHook(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ScriptUnbind(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_ScriptUnbind(a0);
    return 0;
}

static int GenL_Grapple_SetDebugTextSize(lua_State *L)
{
    (void)L;
    float a0 = (float)luaL_checknumber(L, 1);
    Grapple_SetDebugTextSize(a0);
    return 0;
}

static int GenL_Grapple_SetDeviceMotion(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool a1 = (bool)lua_toboolean(L, 2);
    bool rv = Grapple_SetDeviceMotion(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SetDirectionRepeat(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_SetDirectionRepeat(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_SetGamepadDeadzone(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_SetGamepadDeadzone(a0, a1);
    return 0;
}

static int GenL_Grapple_SetGamepadMotion(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    int a1 = (int)luaL_checkinteger(L, 2);
    bool a2 = (bool)lua_toboolean(L, 3);
    bool rv = Grapple_SetGamepadMotion(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SetLightAmbient(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    SDL_FColor a1;
    GenRead_SDL_FColor(L, 2, &a1);
    Grapple_SetLightAmbient(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightDebugDraw(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SetLightDebugDraw(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightMapScale(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_SetLightMapScale(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightRayCount(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_SetLightRayCount(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightRings(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    int a1 = (int)luaL_checkinteger(L, 2);
    Grapple_SetLightRings(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightShadowSoftness(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_SetLightShadowSoftness(a0, a1);
    return 0;
}

static int GenL_Grapple_SetLightUseShaders(lua_State *L)
{
    (void)L;
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_LightScene");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SetLightUseShaders(a0, a1);
    return 0;
}

static int GenL_Grapple_SetMouseCapture(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool a1 = (bool)lua_toboolean(L, 2);
    bool rv = Grapple_SetMouseCapture(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SetTextInput(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SetTextInput(a0, a1);
    return 0;
}

static int GenL_Grapple_SetTriggerThreshold(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float a1 = (float)luaL_checknumber(L, 2);
    Grapple_SetTriggerThreshold(a0, a1);
    return 0;
}

static int GenL_Grapple_ShowOpenFileDialog(lua_State *L)
{
    (void)L;
    SDL_Window *a0 = (SDL_Window *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Window");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_ShowOpenFileDialog(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_ShowSaveFileDialog(lua_State *L)
{
    (void)L;
    SDL_Window *a0 = (SDL_Window *)GrappleGen_LuaCheckHandle(L, 1, "SDL_Window");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    const char *a3 = lua_isnoneornil(L, 4) ? NULL : luaL_checkstring(L, 4);
    bool rv = Grapple_ShowSaveFileDialog(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_SpriteCreate(lua_State *L)
{
    (void)L;
    Grapple_Sprite * rv = Grapple_SpriteCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "Grapple_Sprite");
    return 1;
}

static int GenL_Grapple_SpriteDestroy(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    Grapple_SpriteDestroy(a0);
    return 0;
}

static int GenL_Grapple_SpriteSetColor(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_SpriteSetColor(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_SpriteSetLayer(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    int a1 = (int)luaL_checkinteger(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_SpriteSetLayer(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_SpriteSetOrigin(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_SpriteSetOrigin(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_SpriteSetScreenSpace(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SpriteSetScreenSpace(a0, a1);
    return 0;
}

static int GenL_Grapple_SpriteSetSize(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_SpriteSetSize(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_SpriteSetSortByY(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SpriteSetSortByY(a0, a1);
    return 0;
}

static int GenL_Grapple_SpriteSetSource(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_SpriteSetSource(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_SpriteSetTexture(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, 2, "SDL_Texture");
    Grapple_SpriteSetTexture(a0, a1);
    return 0;
}

static int GenL_Grapple_SpriteSetVisible(lua_State *L)
{
    (void)L;
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Sprite");
    bool a1 = (bool)lua_toboolean(L, 2);
    Grapple_SpriteSetVisible(a0, a1);
    return 0;
}

static int GenL_Grapple_Text(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char * rv = Grapple_Text(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_TextCount(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    int rv = Grapple_TextCount(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TextHas(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_TextHas(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_TextLanguage(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char * rv = Grapple_TextLanguage(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_TextLoad(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    const char *a2 = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    bool rv = Grapple_TextLoad(a0, a1, a2);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_TextLoadFile(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    bool rv = Grapple_TextLoadFile(a0, a1);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_TextSetLanguage(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char *a1 = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    Grapple_TextSetLanguage(a0, a1);
    return 0;
}

static int GenL_Grapple_TextTyped(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    const char * rv = Grapple_TextTyped(a0);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_Texture(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)luaL_checkinteger(L, 2);
    SDL_Texture * rv = Grapple_Texture(a0, a1);
    GrappleGen_LuaPushHandle(L, (void *)rv, "SDL_Texture");
    return 1;
}

static int GenL_Grapple_TiledLayerCount(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int rv = Grapple_TiledLayerCount(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledLayerName(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_TiledLayerName(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_TiledLayerType(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    const char * rv = Grapple_TiledLayerType(a0, a1);
    if (rv == NULL) { lua_pushnil(L); } else { lua_pushstring(L, rv); }
    return 1;
}

static int GenL_Grapple_TiledMapHeight(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int rv = Grapple_TiledMapHeight(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledMapWidth(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int rv = Grapple_TiledMapWidth(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledObjectAt(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    Grapple_TiledObject *a3 = (Grapple_TiledObject *)GrappleGen_LuaCheckHandle(L, 4, "Grapple_TiledObject");
    bool rv = Grapple_TiledObjectAt(a0, a1, a2, a3);
    lua_pushboolean(L, (int)rv);
    return 1;
}

static int GenL_Grapple_TiledObjectCount(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    int rv = Grapple_TiledObjectCount(a0, a1);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledRaw(lua_State *L)
{
    (void)L;
    Grapple_TiledMap *a0 = (Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    cute_tiled_map_t * rv = Grapple_TiledRaw(a0);
    GrappleGen_LuaPushHandle(L, (void *)rv, "cute_tiled_map_t");
    return 1;
}

static int GenL_Grapple_TiledTileAt(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int a1 = (int)luaL_checkinteger(L, 2);
    int a2 = (int)luaL_checkinteger(L, 3);
    int a3 = (int)luaL_checkinteger(L, 4);
    int rv = Grapple_TiledTileAt(a0, a1, a2, a3);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledTileHeight(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int rv = Grapple_TiledTileHeight(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TiledTileWidth(lua_State *L)
{
    (void)L;
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_TiledMap");
    int rv = Grapple_TiledTileWidth(a0);
    lua_pushinteger(L, (lua_Integer)rv);
    return 1;
}

static int GenL_Grapple_TouchPinch(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_TouchPinch(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_TouchRotation(lua_State *L)
{
    (void)L;
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_LuaCheckHandle(L, 1, "Grapple_Engine");
    float rv = Grapple_TouchRotation(a0);
    lua_pushnumber(L, (lua_Number)rv);
    return 1;
}

static int GenL_Grapple_WeldJointDefCreate(lua_State *L)
{
    (void)L;
    b2WeldJointDef * rv = Grapple_WeldJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2WeldJointDef");
    return 1;
}

static int GenL_Grapple_WeldJointDefDestroy(lua_State *L)
{
    (void)L;
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WeldJointDef");
    Grapple_WeldJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_WeldJointDefSetAnchors(lua_State *L)
{
    (void)L;
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WeldJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_WeldJointDefSetAnchors(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_WeldJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WeldJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_WeldJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_WeldJointDefSetSpring(lua_State *L)
{
    (void)L;
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WeldJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_WeldJointDefSetSpring(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_WheelJointDefCreate(lua_State *L)
{
    (void)L;
    b2WheelJointDef * rv = Grapple_WheelJointDefCreate();
    GrappleGen_LuaPushHandle(L, (void *)rv, "b2WheelJointDef");
    return 1;
}

static int GenL_Grapple_WheelJointDefDestroy(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    Grapple_WheelJointDefDestroy(a0);
    return 0;
}

static int GenL_Grapple_WheelJointDefSetAnchors(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    float a4 = (float)luaL_checknumber(L, 5);
    Grapple_WheelJointDefSetAnchors(a0, a1, a2, a3, a4);
    return 0;
}

static int GenL_Grapple_WheelJointDefSetAxis(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    float a1 = (float)luaL_checknumber(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    Grapple_WheelJointDefSetAxis(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_WheelJointDefSetBodies(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(L, 2, &a1);
    b2BodyId a2;
    GenRead_b2BodyId(L, 3, &a2);
    Grapple_WheelJointDefSetBodies(a0, a1, a2);
    return 0;
}

static int GenL_Grapple_WheelJointDefSetMotor(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_WheelJointDefSetMotor(a0, a1, a2, a3);
    return 0;
}

static int GenL_Grapple_WheelJointDefSetSpring(lua_State *L)
{
    (void)L;
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_LuaCheckHandle(L, 1, "b2WheelJointDef");
    bool a1 = (bool)lua_toboolean(L, 2);
    float a2 = (float)luaL_checknumber(L, 3);
    float a3 = (float)luaL_checknumber(L, 4);
    Grapple_WheelJointDefSetSpring(a0, a1, a2, a3);
    return 0;
}

int GrappleGen_OpenLua_grapple(lua_State *L);
int GrappleGen_OpenLua_grapple(lua_State *L)
{
    lua_createtable(L, 0, 619);
    lua_pushcfunction(L, GenL_Grapple_ActionBind);
    lua_setfield(L, -2, "ActionBind");
    lua_pushcfunction(L, GenL_Grapple_ActionBindAxis);
    lua_setfield(L, -2, "ActionBindAxis");
    lua_pushcfunction(L, GenL_Grapple_ActionBindDirection);
    lua_setfield(L, -2, "ActionBindDirection");
    lua_pushcfunction(L, GenL_Grapple_ActionBindKey);
    lua_setfield(L, -2, "ActionBindKey");
    lua_pushcfunction(L, GenL_Grapple_ActionBindKeySigned);
    lua_setfield(L, -2, "ActionBindKeySigned");
    lua_pushcfunction(L, GenL_Grapple_ActionBindMouse);
    lua_setfield(L, -2, "ActionBindMouse");
    lua_pushcfunction(L, GenL_Grapple_ActionBindPad);
    lua_setfield(L, -2, "ActionBindPad");
    lua_pushcfunction(L, GenL_Grapple_ActionBindingAt);
    lua_setfield(L, -2, "ActionBindingAt");
    lua_pushcfunction(L, GenL_Grapple_ActionBindingCount);
    lua_setfield(L, -2, "ActionBindingCount");
    lua_pushcfunction(L, GenL_Grapple_ActionCapture);
    lua_setfield(L, -2, "ActionCapture");
    lua_pushcfunction(L, GenL_Grapple_ActionClear);
    lua_setfield(L, -2, "ActionClear");
    lua_pushcfunction(L, GenL_Grapple_ActionCount);
    lua_setfield(L, -2, "ActionCount");
    lua_pushcfunction(L, GenL_Grapple_ActionDown);
    lua_setfield(L, -2, "ActionDown");
    lua_pushcfunction(L, GenL_Grapple_ActionMapCreate);
    lua_setfield(L, -2, "ActionMapCreate");
    lua_pushcfunction(L, GenL_Grapple_ActionMapDestroy);
    lua_setfield(L, -2, "ActionMapDestroy");
    lua_pushcfunction(L, GenL_Grapple_ActionMapKeyboardPlayer);
    lua_setfield(L, -2, "ActionMapKeyboardPlayer");
    lua_pushcfunction(L, GenL_Grapple_ActionMapLoad);
    lua_setfield(L, -2, "ActionMapLoad");
    lua_pushcfunction(L, GenL_Grapple_ActionMapLoadToml);
    lua_setfield(L, -2, "ActionMapLoadToml");
    lua_pushcfunction(L, GenL_Grapple_ActionMapSave);
    lua_setfield(L, -2, "ActionMapSave");
    lua_pushcfunction(L, GenL_Grapple_ActionMapSetKeyboardPlayer);
    lua_setfield(L, -2, "ActionMapSetKeyboardPlayer");
    lua_pushcfunction(L, GenL_Grapple_ActionMapToToml);
    lua_setfield(L, -2, "ActionMapToToml");
    lua_pushcfunction(L, GenL_Grapple_ActionName);
    lua_setfield(L, -2, "ActionName");
    lua_pushcfunction(L, GenL_Grapple_ActionPressed);
    lua_setfield(L, -2, "ActionPressed");
    lua_pushcfunction(L, GenL_Grapple_ActionReleased);
    lua_setfield(L, -2, "ActionReleased");
    lua_pushcfunction(L, GenL_Grapple_ActionValue);
    lua_setfield(L, -2, "ActionValue");
    lua_pushcfunction(L, GenL_Grapple_ActionVector);
    lua_setfield(L, -2, "ActionVector");
    lua_pushcfunction(L, GenL_Grapple_ActorAddBody);
    lua_setfield(L, -2, "ActorAddBody");
    lua_pushcfunction(L, GenL_Grapple_ActorAddLight);
    lua_setfield(L, -2, "ActorAddLight");
    lua_pushcfunction(L, GenL_Grapple_ActorAlive);
    lua_setfield(L, -2, "ActorAlive");
    lua_pushcfunction(L, GenL_Grapple_ActorAngularVelocity);
    lua_setfield(L, -2, "ActorAngularVelocity");
    lua_pushcfunction(L, GenL_Grapple_ActorApplyForce);
    lua_setfield(L, -2, "ActorApplyForce");
    lua_pushcfunction(L, GenL_Grapple_ActorApplyImpulse);
    lua_setfield(L, -2, "ActorApplyImpulse");
    lua_pushcfunction(L, GenL_Grapple_ActorBodyBounds);
    lua_setfield(L, -2, "ActorBodyBounds");
    lua_pushcfunction(L, GenL_Grapple_ActorBroadcast);
    lua_setfield(L, -2, "ActorBroadcast");
    lua_pushcfunction(L, GenL_Grapple_ActorChild);
    lua_setfield(L, -2, "ActorChild");
    lua_pushcfunction(L, GenL_Grapple_ActorChildCount);
    lua_setfield(L, -2, "ActorChildCount");
    lua_pushcfunction(L, GenL_Grapple_ActorClear);
    lua_setfield(L, -2, "ActorClear");
    lua_pushcfunction(L, GenL_Grapple_ActorClearSprite);
    lua_setfield(L, -2, "ActorClearSprite");
    lua_pushcfunction(L, GenL_Grapple_ActorCount);
    lua_setfield(L, -2, "ActorCount");
    lua_pushcfunction(L, GenL_Grapple_ActorDefCreate);
    lua_setfield(L, -2, "ActorDefCreate");
    lua_pushcfunction(L, GenL_Grapple_ActorDefDestroy);
    lua_setfield(L, -2, "ActorDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetName);
    lua_setfield(L, -2, "ActorDefSetName");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetParent);
    lua_setfield(L, -2, "ActorDefSetParent");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetPosition);
    lua_setfield(L, -2, "ActorDefSetPosition");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetRotation);
    lua_setfield(L, -2, "ActorDefSetRotation");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetScale);
    lua_setfield(L, -2, "ActorDefSetScale");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetStateSize);
    lua_setfield(L, -2, "ActorDefSetStateSize");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetTags);
    lua_setfield(L, -2, "ActorDefSetTags");
    lua_pushcfunction(L, GenL_Grapple_ActorDefSetType);
    lua_setfield(L, -2, "ActorDefSetType");
    lua_pushcfunction(L, GenL_Grapple_ActorDestroy);
    lua_setfield(L, -2, "ActorDestroy");
    lua_pushcfunction(L, GenL_Grapple_ActorEnabled);
    lua_setfield(L, -2, "ActorEnabled");
    lua_pushcfunction(L, GenL_Grapple_ActorEngine);
    lua_setfield(L, -2, "ActorEngine");
    lua_pushcfunction(L, GenL_Grapple_ActorFindByName);
    lua_setfield(L, -2, "ActorFindByName");
    lua_pushcfunction(L, GenL_Grapple_ActorFindByType);
    lua_setfield(L, -2, "ActorFindByType");
    lua_pushcfunction(L, GenL_Grapple_ActorGet);
    lua_setfield(L, -2, "ActorGet");
    lua_pushcfunction(L, GenL_Grapple_ActorGetId);
    lua_setfield(L, -2, "ActorGetId");
    lua_pushcfunction(L, GenL_Grapple_ActorGetTags);
    lua_setfield(L, -2, "ActorGetTags");
    lua_pushcfunction(L, GenL_Grapple_ActorHasBody);
    lua_setfield(L, -2, "ActorHasBody");
    lua_pushcfunction(L, GenL_Grapple_ActorHasTags);
    lua_setfield(L, -2, "ActorHasTags");
    lua_pushcfunction(L, GenL_Grapple_ActorLocal);
    lua_setfield(L, -2, "ActorLocal");
    lua_pushcfunction(L, GenL_Grapple_ActorMove);
    lua_setfield(L, -2, "ActorMove");
    lua_pushcfunction(L, GenL_Grapple_ActorName);
    lua_setfield(L, -2, "ActorName");
    lua_pushcfunction(L, GenL_Grapple_ActorParent);
    lua_setfield(L, -2, "ActorParent");
    lua_pushcfunction(L, GenL_Grapple_ActorQuery);
    lua_setfield(L, -2, "ActorQuery");
    lua_pushcfunction(L, GenL_Grapple_ActorRemoveBody);
    lua_setfield(L, -2, "ActorRemoveBody");
    lua_pushcfunction(L, GenL_Grapple_ActorRemoveLight);
    lua_setfield(L, -2, "ActorRemoveLight");
    lua_pushcfunction(L, GenL_Grapple_ActorRenderTransform);
    lua_setfield(L, -2, "ActorRenderTransform");
    lua_pushcfunction(L, GenL_Grapple_ActorSend);
    lua_setfield(L, -2, "ActorSend");
    lua_pushcfunction(L, GenL_Grapple_ActorSetAngularVelocity);
    lua_setfield(L, -2, "ActorSetAngularVelocity");
    lua_pushcfunction(L, GenL_Grapple_ActorSetEnabled);
    lua_setfield(L, -2, "ActorSetEnabled");
    lua_pushcfunction(L, GenL_Grapple_ActorSetLocal);
    lua_setfield(L, -2, "ActorSetLocal");
    lua_pushcfunction(L, GenL_Grapple_ActorSetParent);
    lua_setfield(L, -2, "ActorSetParent");
    lua_pushcfunction(L, GenL_Grapple_ActorSetPosition);
    lua_setfield(L, -2, "ActorSetPosition");
    lua_pushcfunction(L, GenL_Grapple_ActorSetSprite);
    lua_setfield(L, -2, "ActorSetSprite");
    lua_pushcfunction(L, GenL_Grapple_ActorSetTags);
    lua_setfield(L, -2, "ActorSetTags");
    lua_pushcfunction(L, GenL_Grapple_ActorSetVelocity);
    lua_setfield(L, -2, "ActorSetVelocity");
    lua_pushcfunction(L, GenL_Grapple_ActorSpawn);
    lua_setfield(L, -2, "ActorSpawn");
    lua_pushcfunction(L, GenL_Grapple_ActorSprite);
    lua_setfield(L, -2, "ActorSprite");
    lua_pushcfunction(L, GenL_Grapple_ActorTeleport);
    lua_setfield(L, -2, "ActorTeleport");
    lua_pushcfunction(L, GenL_Grapple_ActorTeleportBody);
    lua_setfield(L, -2, "ActorTeleportBody");
    lua_pushcfunction(L, GenL_Grapple_ActorType);
    lua_setfield(L, -2, "ActorType");
    lua_pushcfunction(L, GenL_Grapple_ActorVelocity);
    lua_setfield(L, -2, "ActorVelocity");
    lua_pushcfunction(L, GenL_Grapple_ActorWakeBody);
    lua_setfield(L, -2, "ActorWakeBody");
    lua_pushcfunction(L, GenL_Grapple_ActorWorld);
    lua_setfield(L, -2, "ActorWorld");
    lua_pushcfunction(L, GenL_Grapple_AddDarkZone);
    lua_setfield(L, -2, "AddDarkZone");
    lua_pushcfunction(L, GenL_Grapple_AddLight);
    lua_setfield(L, -2, "AddLight");
    lua_pushcfunction(L, GenL_Grapple_AddOccluderRect);
    lua_setfield(L, -2, "AddOccluderRect");
    lua_pushcfunction(L, GenL_Grapple_AddOccluderSegment);
    lua_setfield(L, -2, "AddOccluderSegment");
    lua_pushcfunction(L, GenL_Grapple_AnyInput);
    lua_setfield(L, -2, "AnyInput");
    lua_pushcfunction(L, GenL_Grapple_AssetPath);
    lua_setfield(L, -2, "AssetPath");
    lua_pushcfunction(L, GenL_Grapple_AssetRelease);
    lua_setfield(L, -2, "AssetRelease");
    lua_pushcfunction(L, GenL_Grapple_AssetRetain);
    lua_setfield(L, -2, "AssetRetain");
    lua_pushcfunction(L, GenL_Grapple_AssetStatusOf);
    lua_setfield(L, -2, "AssetStatusOf");
    lua_pushcfunction(L, GenL_Grapple_AssetsFrameBudget);
    lua_setfield(L, -2, "AssetsFrameBudget");
    lua_pushcfunction(L, GenL_Grapple_AssetsLoaded);
    lua_setfield(L, -2, "AssetsLoaded");
    lua_pushcfunction(L, GenL_Grapple_AssetsPending);
    lua_setfield(L, -2, "AssetsPending");
    lua_pushcfunction(L, GenL_Grapple_AssetsProgress);
    lua_setfield(L, -2, "AssetsProgress");
    lua_pushcfunction(L, GenL_Grapple_AssetsReady);
    lua_setfield(L, -2, "AssetsReady");
    lua_pushcfunction(L, GenL_Grapple_AssetsSetFrameBudget);
    lua_setfield(L, -2, "AssetsSetFrameBudget");
    lua_pushcfunction(L, GenL_Grapple_AssetsSetWorkers);
    lua_setfield(L, -2, "AssetsSetWorkers");
    lua_pushcfunction(L, GenL_Grapple_AssetsWait);
    lua_setfield(L, -2, "AssetsWait");
    lua_pushcfunction(L, GenL_Grapple_BidiBaseIsRTL);
    lua_setfield(L, -2, "BidiBaseIsRTL");
    lua_pushcfunction(L, GenL_Grapple_BindingFromString);
    lua_setfield(L, -2, "BindingFromString");
    lua_pushcfunction(L, GenL_Grapple_BindingToString);
    lua_setfield(L, -2, "BindingToString");
    lua_pushcfunction(L, GenL_Grapple_BodyDefDestroy);
    lua_setfield(L, -2, "BodyDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetBullet);
    lua_setfield(L, -2, "BodyDefSetBullet");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetFilter);
    lua_setfield(L, -2, "BodyDefSetFilter");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetFixedRotation);
    lua_setfield(L, -2, "BodyDefSetFixedRotation");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetGravityScale);
    lua_setfield(L, -2, "BodyDefSetGravityScale");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetMaterial);
    lua_setfield(L, -2, "BodyDefSetMaterial");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetOffset);
    lua_setfield(L, -2, "BodyDefSetOffset");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetSensor);
    lua_setfield(L, -2, "BodyDefSetSensor");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetShape);
    lua_setfield(L, -2, "BodyDefSetShape");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetSize);
    lua_setfield(L, -2, "BodyDefSetSize");
    lua_pushcfunction(L, GenL_Grapple_BodyDefSetType);
    lua_setfield(L, -2, "BodyDefSetType");
    lua_pushcfunction(L, GenL_Grapple_BodyDefault);
    lua_setfield(L, -2, "BodyDefault");
    lua_pushcfunction(L, GenL_Grapple_CameraBegin);
    lua_setfield(L, -2, "CameraBegin");
    lua_pushcfunction(L, GenL_Grapple_CameraDestroy);
    lua_setfield(L, -2, "CameraDestroy");
    lua_pushcfunction(L, GenL_Grapple_CameraEnd);
    lua_setfield(L, -2, "CameraEnd");
    lua_pushcfunction(L, GenL_Grapple_CameraFollow);
    lua_setfield(L, -2, "CameraFollow");
    lua_pushcfunction(L, GenL_Grapple_CameraInit);
    lua_setfield(L, -2, "CameraInit");
    lua_pushcfunction(L, GenL_Grapple_CameraPoint);
    lua_setfield(L, -2, "CameraPoint");
    lua_pushcfunction(L, GenL_Grapple_CameraRect);
    lua_setfield(L, -2, "CameraRect");
    lua_pushcfunction(L, GenL_Grapple_CameraScreenToWorld);
    lua_setfield(L, -2, "CameraScreenToWorld");
    lua_pushcfunction(L, GenL_Grapple_CameraSetBounds);
    lua_setfield(L, -2, "CameraSetBounds");
    lua_pushcfunction(L, GenL_Grapple_CameraSetDeadzone);
    lua_setfield(L, -2, "CameraSetDeadzone");
    lua_pushcfunction(L, GenL_Grapple_CameraSetSmoothing);
    lua_setfield(L, -2, "CameraSetSmoothing");
    lua_pushcfunction(L, GenL_Grapple_CameraSetViewport);
    lua_setfield(L, -2, "CameraSetViewport");
    lua_pushcfunction(L, GenL_Grapple_CameraSetZoom);
    lua_setfield(L, -2, "CameraSetZoom");
    lua_pushcfunction(L, GenL_Grapple_CameraShake);
    lua_setfield(L, -2, "CameraShake");
    lua_pushcfunction(L, GenL_Grapple_CameraSnap);
    lua_setfield(L, -2, "CameraSnap");
    lua_pushcfunction(L, GenL_Grapple_CameraSplit);
    lua_setfield(L, -2, "CameraSplit");
    lua_pushcfunction(L, GenL_Grapple_CameraUpdate);
    lua_setfield(L, -2, "CameraUpdate");
    lua_pushcfunction(L, GenL_Grapple_CameraVisible);
    lua_setfield(L, -2, "CameraVisible");
    lua_pushcfunction(L, GenL_Grapple_CameraX);
    lua_setfield(L, -2, "CameraX");
    lua_pushcfunction(L, GenL_Grapple_CameraY);
    lua_setfield(L, -2, "CameraY");
    lua_pushcfunction(L, GenL_Grapple_CompileRegex);
    lua_setfield(L, -2, "CompileRegex");
    lua_pushcfunction(L, GenL_Grapple_ConfigCreate);
    lua_setfield(L, -2, "ConfigCreate");
    lua_pushcfunction(L, GenL_Grapple_ConfigDestroy);
    lua_setfield(L, -2, "ConfigDestroy");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetAutoMount);
    lua_setfield(L, -2, "ConfigSetAutoMount");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetBackend);
    lua_setfield(L, -2, "ConfigSetBackend");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetDesignSize);
    lua_setfield(L, -2, "ConfigSetDesignSize");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetFullscreen);
    lua_setfield(L, -2, "ConfigSetFullscreen");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetGraphics);
    lua_setfield(L, -2, "ConfigSetGraphics");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetHeadless);
    lua_setfield(L, -2, "ConfigSetHeadless");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetHighDpi);
    lua_setfield(L, -2, "ConfigSetHighDpi");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetManualClock);
    lua_setfield(L, -2, "ConfigSetManualClock");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetMaxFps);
    lua_setfield(L, -2, "ConfigSetMaxFps");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetMediaPath);
    lua_setfield(L, -2, "ConfigSetMediaPath");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetPresentation);
    lua_setfield(L, -2, "ConfigSetPresentation");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetResizable);
    lua_setfield(L, -2, "ConfigSetResizable");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetTickRate);
    lua_setfield(L, -2, "ConfigSetTickRate");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetTitle);
    lua_setfield(L, -2, "ConfigSetTitle");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetVsync);
    lua_setfield(L, -2, "ConfigSetVsync");
    lua_pushcfunction(L, GenL_Grapple_ConfigSetWindowSize);
    lua_setfield(L, -2, "ConfigSetWindowSize");
    lua_pushcfunction(L, GenL_Grapple_CountSignalConnections);
    lua_setfield(L, -2, "CountSignalConnections");
    lua_pushcfunction(L, GenL_Grapple_CreateChipSFX);
    lua_setfield(L, -2, "CreateChipSFX");
    lua_pushcfunction(L, GenL_Grapple_CreateChipTone);
    lua_setfield(L, -2, "CreateChipTone");
    lua_pushcfunction(L, GenL_Grapple_CreateChipTune);
    lua_setfield(L, -2, "CreateChipTune");
    lua_pushcfunction(L, GenL_Grapple_CreateEngine);
    lua_setfield(L, -2, "CreateEngine");
    lua_pushcfunction(L, GenL_Grapple_CreateGui);
    lua_setfield(L, -2, "CreateGui");
    lua_pushcfunction(L, GenL_Grapple_CreateGuiWithGlyphs);
    lua_setfield(L, -2, "CreateGuiWithGlyphs");
    lua_pushcfunction(L, GenL_Grapple_CreateLightScene);
    lua_setfield(L, -2, "CreateLightScene");
    lua_pushcfunction(L, GenL_Grapple_CreateSignalEmitter);
    lua_setfield(L, -2, "CreateSignalEmitter");
    lua_pushcfunction(L, GenL_Grapple_CryptoSelfTest);
    lua_setfield(L, -2, "CryptoSelfTest");
    lua_pushcfunction(L, GenL_Grapple_DayNightAmbient);
    lua_setfield(L, -2, "DayNightAmbient");
    lua_pushcfunction(L, GenL_Grapple_DayNightSunlight);
    lua_setfield(L, -2, "DayNightSunlight");
    lua_pushcfunction(L, GenL_Grapple_DestroyEngine);
    lua_setfield(L, -2, "DestroyEngine");
    lua_pushcfunction(L, GenL_Grapple_DestroyGui);
    lua_setfield(L, -2, "DestroyGui");
    lua_pushcfunction(L, GenL_Grapple_DestroyLightScene);
    lua_setfield(L, -2, "DestroyLightScene");
    lua_pushcfunction(L, GenL_Grapple_DestroyRegex);
    lua_setfield(L, -2, "DestroyRegex");
    lua_pushcfunction(L, GenL_Grapple_DestroySignalEmitter);
    lua_setfield(L, -2, "DestroySignalEmitter");
    lua_pushcfunction(L, GenL_Grapple_DeviceAccelerometer);
    lua_setfield(L, -2, "DeviceAccelerometer");
    lua_pushcfunction(L, GenL_Grapple_DeviceGyro);
    lua_setfield(L, -2, "DeviceGyro");
    lua_pushcfunction(L, GenL_Grapple_DialogDeliverSave);
    lua_setfield(L, -2, "DialogDeliverSave");
    lua_pushcfunction(L, GenL_Grapple_DialogPath);
    lua_setfield(L, -2, "DialogPath");
    lua_pushcfunction(L, GenL_Grapple_DialogReset);
    lua_setfield(L, -2, "DialogReset");
    lua_pushcfunction(L, GenL_Grapple_DialogStatus);
    lua_setfield(L, -2, "DialogStatus");
    lua_pushcfunction(L, GenL_Grapple_DisconnectSignal);
    lua_setfield(L, -2, "DisconnectSignal");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefCreate);
    lua_setfield(L, -2, "DistanceJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefDestroy);
    lua_setfield(L, -2, "DistanceJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetAnchors);
    lua_setfield(L, -2, "DistanceJointDefSetAnchors");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetBodies);
    lua_setfield(L, -2, "DistanceJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetCollideConnected);
    lua_setfield(L, -2, "DistanceJointDefSetCollideConnected");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetLength);
    lua_setfield(L, -2, "DistanceJointDefSetLength");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetLimit);
    lua_setfield(L, -2, "DistanceJointDefSetLimit");
    lua_pushcfunction(L, GenL_Grapple_DistanceJointDefSetSpring);
    lua_setfield(L, -2, "DistanceJointDefSetSpring");
    lua_pushcfunction(L, GenL_Grapple_DrawPhysicsWorld);
    lua_setfield(L, -2, "DrawPhysicsWorld");
    lua_pushcfunction(L, GenL_Grapple_EncodeDataBase64);
    lua_setfield(L, -2, "EncodeDataBase64");
    lua_pushcfunction(L, GenL_Grapple_EngineAdvance);
    lua_setfield(L, -2, "EngineAdvance");
    lua_pushcfunction(L, GenL_Grapple_EngineAlpha);
    lua_setfield(L, -2, "EngineAlpha");
    lua_pushcfunction(L, GenL_Grapple_EngineAssetScale);
    lua_setfield(L, -2, "EngineAssetScale");
    lua_pushcfunction(L, GenL_Grapple_EngineDelta);
    lua_setfield(L, -2, "EngineDelta");
    lua_pushcfunction(L, GenL_Grapple_EngineDesignSize);
    lua_setfield(L, -2, "EngineDesignSize");
    lua_pushcfunction(L, GenL_Grapple_EngineDisplay);
    lua_setfield(L, -2, "EngineDisplay");
    lua_pushcfunction(L, GenL_Grapple_EngineDisplayCount);
    lua_setfield(L, -2, "EngineDisplayCount");
    lua_pushcfunction(L, GenL_Grapple_EngineDisplayName);
    lua_setfield(L, -2, "EngineDisplayName");
    lua_pushcfunction(L, GenL_Grapple_EngineEffectsAvailable);
    lua_setfield(L, -2, "EngineEffectsAvailable");
    lua_pushcfunction(L, GenL_Grapple_EngineEmbedMedia);
    lua_setfield(L, -2, "EngineEmbedMedia");
    lua_pushcfunction(L, GenL_Grapple_EngineFps);
    lua_setfield(L, -2, "EngineFps");
    lua_pushcfunction(L, GenL_Grapple_EngineFrameCount);
    lua_setfield(L, -2, "EngineFrameCount");
    lua_pushcfunction(L, GenL_Grapple_EngineMaxFps);
    lua_setfield(L, -2, "EngineMaxFps");
    lua_pushcfunction(L, GenL_Grapple_EngineMediaPath);
    lua_setfield(L, -2, "EngineMediaPath");
    lua_pushcfunction(L, GenL_Grapple_EngineMediaSource);
    lua_setfield(L, -2, "EngineMediaSource");
    lua_pushcfunction(L, GenL_Grapple_EngineOverloadFrames);
    lua_setfield(L, -2, "EngineOverloadFrames");
    lua_pushcfunction(L, GenL_Grapple_EnginePixelSize);
    lua_setfield(L, -2, "EnginePixelSize");
    lua_pushcfunction(L, GenL_Grapple_EnginePresentation_);
    lua_setfield(L, -2, "EnginePresentation_");
    lua_pushcfunction(L, GenL_Grapple_EngineQuit);
    lua_setfield(L, -2, "EngineQuit");
    lua_pushcfunction(L, GenL_Grapple_EngineRenderScale);
    lua_setfield(L, -2, "EngineRenderScale");
    lua_pushcfunction(L, GenL_Grapple_EngineRenderer);
    lua_setfield(L, -2, "EngineRenderer");
    lua_pushcfunction(L, GenL_Grapple_EngineSafeRect);
    lua_setfield(L, -2, "EngineSafeRect");
    lua_pushcfunction(L, GenL_Grapple_EngineSetClearColor);
    lua_setfield(L, -2, "EngineSetClearColor");
    lua_pushcfunction(L, GenL_Grapple_EngineSetDisplay);
    lua_setfield(L, -2, "EngineSetDisplay");
    lua_pushcfunction(L, GenL_Grapple_EngineSetGraphics);
    lua_setfield(L, -2, "EngineSetGraphics");
    lua_pushcfunction(L, GenL_Grapple_EngineSetMaxFps);
    lua_setfield(L, -2, "EngineSetMaxFps");
    lua_pushcfunction(L, GenL_Grapple_EngineSetMediaPassword);
    lua_setfield(L, -2, "EngineSetMediaPassword");
    lua_pushcfunction(L, GenL_Grapple_EngineSetPresentation);
    lua_setfield(L, -2, "EngineSetPresentation");
    lua_pushcfunction(L, GenL_Grapple_EngineSetRefreshRate);
    lua_setfield(L, -2, "EngineSetRefreshRate");
    lua_pushcfunction(L, GenL_Grapple_EngineSetTickRate);
    lua_setfield(L, -2, "EngineSetTickRate");
    lua_pushcfunction(L, GenL_Grapple_EngineSetTimeScale);
    lua_setfield(L, -2, "EngineSetTimeScale");
    lua_pushcfunction(L, GenL_Grapple_EngineShowWindow);
    lua_setfield(L, -2, "EngineShowWindow");
    lua_pushcfunction(L, GenL_Grapple_EngineStep);
    lua_setfield(L, -2, "EngineStep");
    lua_pushcfunction(L, GenL_Grapple_EngineStepsLastFrame);
    lua_setfield(L, -2, "EngineStepsLastFrame");
    lua_pushcfunction(L, GenL_Grapple_EngineTick);
    lua_setfield(L, -2, "EngineTick");
    lua_pushcfunction(L, GenL_Grapple_EngineTickRate);
    lua_setfield(L, -2, "EngineTickRate");
    lua_pushcfunction(L, GenL_Grapple_EngineTimeScale);
    lua_setfield(L, -2, "EngineTimeScale");
    lua_pushcfunction(L, GenL_Grapple_EngineViewRect);
    lua_setfield(L, -2, "EngineViewRect");
    lua_pushcfunction(L, GenL_Grapple_EngineWindow);
    lua_setfield(L, -2, "EngineWindow");
    lua_pushcfunction(L, GenL_Grapple_EngineWindowToDesign);
    lua_setfield(L, -2, "EngineWindowToDesign");
    lua_pushcfunction(L, GenL_Grapple_EventCreate);
    lua_setfield(L, -2, "EventCreate");
    lua_pushcfunction(L, GenL_Grapple_EventDestroy);
    lua_setfield(L, -2, "EventDestroy");
    lua_pushcfunction(L, GenL_Grapple_EventGamepadAxis);
    lua_setfield(L, -2, "EventGamepadAxis");
    lua_pushcfunction(L, GenL_Grapple_EventGamepadAxisValue);
    lua_setfield(L, -2, "EventGamepadAxisValue");
    lua_pushcfunction(L, GenL_Grapple_EventGamepadButton);
    lua_setfield(L, -2, "EventGamepadButton");
    lua_pushcfunction(L, GenL_Grapple_EventGamepadWhich);
    lua_setfield(L, -2, "EventGamepadWhich");
    lua_pushcfunction(L, GenL_Grapple_EventKeyModifiers);
    lua_setfield(L, -2, "EventKeyModifiers");
    lua_pushcfunction(L, GenL_Grapple_EventKeyRepeat);
    lua_setfield(L, -2, "EventKeyRepeat");
    lua_pushcfunction(L, GenL_Grapple_EventKeyScancode);
    lua_setfield(L, -2, "EventKeyScancode");
    lua_pushcfunction(L, GenL_Grapple_EventMouseButton);
    lua_setfield(L, -2, "EventMouseButton");
    lua_pushcfunction(L, GenL_Grapple_EventMouseDeltaX);
    lua_setfield(L, -2, "EventMouseDeltaX");
    lua_pushcfunction(L, GenL_Grapple_EventMouseDeltaY);
    lua_setfield(L, -2, "EventMouseDeltaY");
    lua_pushcfunction(L, GenL_Grapple_EventMouseX);
    lua_setfield(L, -2, "EventMouseX");
    lua_pushcfunction(L, GenL_Grapple_EventMouseY);
    lua_setfield(L, -2, "EventMouseY");
    lua_pushcfunction(L, GenL_Grapple_EventSetType);
    lua_setfield(L, -2, "EventSetType");
    lua_pushcfunction(L, GenL_Grapple_EventText);
    lua_setfield(L, -2, "EventText");
    lua_pushcfunction(L, GenL_Grapple_EventTouchX);
    lua_setfield(L, -2, "EventTouchX");
    lua_pushcfunction(L, GenL_Grapple_EventTouchY);
    lua_setfield(L, -2, "EventTouchY");
    lua_pushcfunction(L, GenL_Grapple_EventType);
    lua_setfield(L, -2, "EventType");
    lua_pushcfunction(L, GenL_Grapple_EventWheelX);
    lua_setfield(L, -2, "EventWheelX");
    lua_pushcfunction(L, GenL_Grapple_EventWheelY);
    lua_setfield(L, -2, "EventWheelY");
    lua_pushcfunction(L, GenL_Grapple_EventWindowId);
    lua_setfield(L, -2, "EventWindowId");
    lua_pushcfunction(L, GenL_Grapple_FilterJointDefCreate);
    lua_setfield(L, -2, "FilterJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_FilterJointDefDestroy);
    lua_setfield(L, -2, "FilterJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_FilterJointDefSetBodies);
    lua_setfield(L, -2, "FilterJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_FingerCount);
    lua_setfield(L, -2, "FingerCount");
    lua_pushcfunction(L, GenL_Grapple_FreeTiledMap);
    lua_setfield(L, -2, "FreeTiledMap");
    lua_pushcfunction(L, GenL_Grapple_GPUAcquireSwapchain);
    lua_setfield(L, -2, "GPUAcquireSwapchain");
    lua_pushcfunction(L, GenL_Grapple_GPUBeginComputePass);
    lua_setfield(L, -2, "GPUBeginComputePass");
    lua_pushcfunction(L, GenL_Grapple_GPUBindComputeStorageBuffer);
    lua_setfield(L, -2, "GPUBindComputeStorageBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUBindComputeStorageTexture);
    lua_setfield(L, -2, "GPUBindComputeStorageTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUBindFragmentStorageBuffer);
    lua_setfield(L, -2, "GPUBindFragmentStorageBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUBindFragmentStorageTexture);
    lua_setfield(L, -2, "GPUBindFragmentStorageTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUBindVertexStorageBuffer);
    lua_setfield(L, -2, "GPUBindVertexStorageBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUBindVertexStorageTexture);
    lua_setfield(L, -2, "GPUBindVertexStorageTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUBlitInfoCreate);
    lua_setfield(L, -2, "GPUBlitInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUBlitInfoDestroy);
    lua_setfield(L, -2, "GPUBlitInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUBlitInfoSetDestination);
    lua_setfield(L, -2, "GPUBlitInfoSetDestination");
    lua_pushcfunction(L, GenL_Grapple_GPUBlitInfoSetFilter);
    lua_setfield(L, -2, "GPUBlitInfoSetFilter");
    lua_pushcfunction(L, GenL_Grapple_GPUBlitInfoSetSource);
    lua_setfield(L, -2, "GPUBlitInfoSetSource");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferBindingCreate);
    lua_setfield(L, -2, "GPUBufferBindingCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferBindingDestroy);
    lua_setfield(L, -2, "GPUBufferBindingDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferBindingSet);
    lua_setfield(L, -2, "GPUBufferBindingSet");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferLocationCreate);
    lua_setfield(L, -2, "GPUBufferLocationCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferLocationDestroy);
    lua_setfield(L, -2, "GPUBufferLocationDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferLocationSet);
    lua_setfield(L, -2, "GPUBufferLocationSet");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferRegionCreate);
    lua_setfield(L, -2, "GPUBufferRegionCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferRegionDestroy);
    lua_setfield(L, -2, "GPUBufferRegionDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUBufferRegionSet);
    lua_setfield(L, -2, "GPUBufferRegionSet");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoCreate);
    lua_setfield(L, -2, "GPUColorTargetInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoDestroy);
    lua_setfield(L, -2, "GPUColorTargetInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoSetClearColor);
    lua_setfield(L, -2, "GPUColorTargetInfoSetClearColor");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoSetCycle);
    lua_setfield(L, -2, "GPUColorTargetInfoSetCycle");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoSetMipLayer);
    lua_setfield(L, -2, "GPUColorTargetInfoSetMipLayer");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoSetOps);
    lua_setfield(L, -2, "GPUColorTargetInfoSetOps");
    lua_pushcfunction(L, GenL_Grapple_GPUColorTargetInfoSetTexture);
    lua_setfield(L, -2, "GPUColorTargetInfoSetTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUComputeBindingsAddBuffer);
    lua_setfield(L, -2, "GPUComputeBindingsAddBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUComputeBindingsAddTexture);
    lua_setfield(L, -2, "GPUComputeBindingsAddTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUComputeBindingsCreate);
    lua_setfield(L, -2, "GPUComputeBindingsCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUComputeBindingsDestroy);
    lua_setfield(L, -2, "GPUComputeBindingsDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoCreate);
    lua_setfield(L, -2, "GPUComputePipelineInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoDestroy);
    lua_setfield(L, -2, "GPUComputePipelineInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoSetCode);
    lua_setfield(L, -2, "GPUComputePipelineInfoSetCode");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoSetEntrypoint);
    lua_setfield(L, -2, "GPUComputePipelineInfoSetEntrypoint");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoSetFormat);
    lua_setfield(L, -2, "GPUComputePipelineInfoSetFormat");
    lua_pushcfunction(L, GenL_Grapple_GPUComputePipelineInfoSetThreadCount);
    lua_setfield(L, -2, "GPUComputePipelineInfoSetThreadCount");
    lua_pushcfunction(L, GenL_Grapple_GPUDepthStencilTargetInfoCreate);
    lua_setfield(L, -2, "GPUDepthStencilTargetInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUDepthStencilTargetInfoDestroy);
    lua_setfield(L, -2, "GPUDepthStencilTargetInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUDepthStencilTargetInfoSetClear);
    lua_setfield(L, -2, "GPUDepthStencilTargetInfoSetClear");
    lua_pushcfunction(L, GenL_Grapple_GPUDepthStencilTargetInfoSetOps);
    lua_setfield(L, -2, "GPUDepthStencilTargetInfoSetOps");
    lua_pushcfunction(L, GenL_Grapple_GPUDepthStencilTargetInfoSetTexture);
    lua_setfield(L, -2, "GPUDepthStencilTargetInfoSetTexture");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoAddColorTarget);
    lua_setfield(L, -2, "GPUPipelineInfoAddColorTarget");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoAddVertexAttribute);
    lua_setfield(L, -2, "GPUPipelineInfoAddVertexAttribute");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoAddVertexBuffer);
    lua_setfield(L, -2, "GPUPipelineInfoAddVertexBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoCreate);
    lua_setfield(L, -2, "GPUPipelineInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoDestroy);
    lua_setfield(L, -2, "GPUPipelineInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoSetDepthStencil);
    lua_setfield(L, -2, "GPUPipelineInfoSetDepthStencil");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoSetFillMode);
    lua_setfield(L, -2, "GPUPipelineInfoSetFillMode");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoSetPrimitive);
    lua_setfield(L, -2, "GPUPipelineInfoSetPrimitive");
    lua_pushcfunction(L, GenL_Grapple_GPUPipelineInfoSetShaders);
    lua_setfield(L, -2, "GPUPipelineInfoSetShaders");
    lua_pushcfunction(L, GenL_Grapple_GPURenderStateInfoAddSampler);
    lua_setfield(L, -2, "GPURenderStateInfoAddSampler");
    lua_pushcfunction(L, GenL_Grapple_GPURenderStateInfoCreate);
    lua_setfield(L, -2, "GPURenderStateInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPURenderStateInfoDestroy);
    lua_setfield(L, -2, "GPURenderStateInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPURenderStateInfoSetShader);
    lua_setfield(L, -2, "GPURenderStateInfoSetShader");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoCreate);
    lua_setfield(L, -2, "GPUShaderCreateInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoDestroy);
    lua_setfield(L, -2, "GPUShaderCreateInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoSetCode);
    lua_setfield(L, -2, "GPUShaderCreateInfoSetCode");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoSetCounts);
    lua_setfield(L, -2, "GPUShaderCreateInfoSetCounts");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoSetEntrypoint);
    lua_setfield(L, -2, "GPUShaderCreateInfoSetEntrypoint");
    lua_pushcfunction(L, GenL_Grapple_GPUShaderCreateInfoSetFormat);
    lua_setfield(L, -2, "GPUShaderCreateInfoSetFormat");
    lua_pushcfunction(L, GenL_Grapple_GPUSwapchainHeight);
    lua_setfield(L, -2, "GPUSwapchainHeight");
    lua_pushcfunction(L, GenL_Grapple_GPUSwapchainWidth);
    lua_setfield(L, -2, "GPUSwapchainWidth");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureLocationCreate);
    lua_setfield(L, -2, "GPUTextureLocationCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureLocationDestroy);
    lua_setfield(L, -2, "GPUTextureLocationDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureLocationSet);
    lua_setfield(L, -2, "GPUTextureLocationSet");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureRegionCreate);
    lua_setfield(L, -2, "GPUTextureRegionCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureRegionDestroy);
    lua_setfield(L, -2, "GPUTextureRegionDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureRegionSet);
    lua_setfield(L, -2, "GPUTextureRegionSet");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureSamplerBindingCreate);
    lua_setfield(L, -2, "GPUTextureSamplerBindingCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureSamplerBindingDestroy);
    lua_setfield(L, -2, "GPUTextureSamplerBindingDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureSamplerBindingSet);
    lua_setfield(L, -2, "GPUTextureSamplerBindingSet");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureTransferInfoCreate);
    lua_setfield(L, -2, "GPUTextureTransferInfoCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureTransferInfoDestroy);
    lua_setfield(L, -2, "GPUTextureTransferInfoDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUTextureTransferInfoSet);
    lua_setfield(L, -2, "GPUTextureTransferInfoSet");
    lua_pushcfunction(L, GenL_Grapple_GPUTransferBufferLocationCreate);
    lua_setfield(L, -2, "GPUTransferBufferLocationCreate");
    lua_pushcfunction(L, GenL_Grapple_GPUTransferBufferLocationDestroy);
    lua_setfield(L, -2, "GPUTransferBufferLocationDestroy");
    lua_pushcfunction(L, GenL_Grapple_GPUTransferBufferLocationSet);
    lua_setfield(L, -2, "GPUTransferBufferLocationSet");
    lua_pushcfunction(L, GenL_Grapple_GPUUploadToTransferBuffer);
    lua_setfield(L, -2, "GPUUploadToTransferBuffer");
    lua_pushcfunction(L, GenL_Grapple_GPUWaitAndAcquireSwapchain);
    lua_setfield(L, -2, "GPUWaitAndAcquireSwapchain");
    lua_pushcfunction(L, GenL_Grapple_GPUWaitForFence);
    lua_setfield(L, -2, "GPUWaitForFence");
    lua_pushcfunction(L, GenL_Grapple_GamepadAccelerometer);
    lua_setfield(L, -2, "GamepadAccelerometer");
    lua_pushcfunction(L, GenL_Grapple_GamepadAxisValue);
    lua_setfield(L, -2, "GamepadAxisValue");
    lua_pushcfunction(L, GenL_Grapple_GamepadButtonDown);
    lua_setfield(L, -2, "GamepadButtonDown");
    lua_pushcfunction(L, GenL_Grapple_GamepadButtonPressed);
    lua_setfield(L, -2, "GamepadButtonPressed");
    lua_pushcfunction(L, GenL_Grapple_GamepadButtonReleased);
    lua_setfield(L, -2, "GamepadButtonReleased");
    lua_pushcfunction(L, GenL_Grapple_GamepadConnected);
    lua_setfield(L, -2, "GamepadConnected");
    lua_pushcfunction(L, GenL_Grapple_GamepadCount);
    lua_setfield(L, -2, "GamepadCount");
    lua_pushcfunction(L, GenL_Grapple_GamepadDeadzone);
    lua_setfield(L, -2, "GamepadDeadzone");
    lua_pushcfunction(L, GenL_Grapple_GamepadDirectionPressed);
    lua_setfield(L, -2, "GamepadDirectionPressed");
    lua_pushcfunction(L, GenL_Grapple_GamepadDirectionRepeat);
    lua_setfield(L, -2, "GamepadDirectionRepeat");
    lua_pushcfunction(L, GenL_Grapple_GamepadGyro);
    lua_setfield(L, -2, "GamepadGyro");
    lua_pushcfunction(L, GenL_Grapple_GamepadHasAccelerometer);
    lua_setfield(L, -2, "GamepadHasAccelerometer");
    lua_pushcfunction(L, GenL_Grapple_GamepadHasGyro);
    lua_setfield(L, -2, "GamepadHasGyro");
    lua_pushcfunction(L, GenL_Grapple_GamepadName);
    lua_setfield(L, -2, "GamepadName");
    lua_pushcfunction(L, GenL_Grapple_GamepadRumble);
    lua_setfield(L, -2, "GamepadRumble");
    lua_pushcfunction(L, GenL_Grapple_GamepadRumbleTriggers);
    lua_setfield(L, -2, "GamepadRumbleTriggers");
    lua_pushcfunction(L, GenL_Grapple_GamepadSetLED);
    lua_setfield(L, -2, "GamepadSetLED");
    lua_pushcfunction(L, GenL_Grapple_GamepadStick);
    lua_setfield(L, -2, "GamepadStick");
    lua_pushcfunction(L, GenL_Grapple_GamepadStopRumble);
    lua_setfield(L, -2, "GamepadStopRumble");
    lua_pushcfunction(L, GenL_Grapple_GraphicsClamp);
    lua_setfield(L, -2, "GraphicsClamp");
    lua_pushcfunction(L, GenL_Grapple_GraphicsConfigError);
    lua_setfield(L, -2, "GraphicsConfigError");
    lua_pushcfunction(L, GenL_Grapple_GraphicsConfigPath);
    lua_setfield(L, -2, "GraphicsConfigPath");
    lua_pushcfunction(L, GenL_Grapple_GraphicsDefaults);
    lua_setfield(L, -2, "GraphicsDefaults");
    lua_pushcfunction(L, GenL_Grapple_GraphicsEqual);
    lua_setfield(L, -2, "GraphicsEqual");
    lua_pushcfunction(L, GenL_Grapple_GraphicsLightMapScale);
    lua_setfield(L, -2, "GraphicsLightMapScale");
    lua_pushcfunction(L, GenL_Grapple_GraphicsLoadTomlFile);
    lua_setfield(L, -2, "GraphicsLoadTomlFile");
    lua_pushcfunction(L, GenL_Grapple_GraphicsLoadTomlString);
    lua_setfield(L, -2, "GraphicsLoadTomlString");
    lua_pushcfunction(L, GenL_Grapple_GraphicsMaxDynamicLights);
    lua_setfield(L, -2, "GraphicsMaxDynamicLights");
    lua_pushcfunction(L, GenL_Grapple_GraphicsParticleDensity);
    lua_setfield(L, -2, "GraphicsParticleDensity");
    lua_pushcfunction(L, GenL_Grapple_GraphicsQualityFromName);
    lua_setfield(L, -2, "GraphicsQualityFromName");
    lua_pushcfunction(L, GenL_Grapple_GraphicsQualityName);
    lua_setfield(L, -2, "GraphicsQualityName");
    lua_pushcfunction(L, GenL_Grapple_GraphicsSafeMode);
    lua_setfield(L, -2, "GraphicsSafeMode");
    lua_pushcfunction(L, GenL_Grapple_GraphicsSave);
    lua_setfield(L, -2, "GraphicsSave");
    lua_pushcfunction(L, GenL_Grapple_GraphicsSavePath);
    lua_setfield(L, -2, "GraphicsSavePath");
    lua_pushcfunction(L, GenL_Grapple_GraphicsShadowRays);
    lua_setfield(L, -2, "GraphicsShadowRays");
    lua_pushcfunction(L, GenL_Grapple_GraphicsShadowSoftness);
    lua_setfield(L, -2, "GraphicsShadowSoftness");
    lua_pushcfunction(L, GenL_Grapple_GraphicsToToml);
    lua_setfield(L, -2, "GraphicsToToml");
    lua_pushcfunction(L, GenL_Grapple_GuiContext);
    lua_setfield(L, -2, "GuiContext");
    lua_pushcfunction(L, GenL_Grapple_GuiDrawCommandCount);
    lua_setfield(L, -2, "GuiDrawCommandCount");
    lua_pushcfunction(L, GenL_Grapple_GuiDrawTexture);
    lua_setfield(L, -2, "GuiDrawTexture");
    lua_pushcfunction(L, GenL_Grapple_GuiDrawTextureOverlay);
    lua_setfield(L, -2, "GuiDrawTextureOverlay");
    lua_pushcfunction(L, GenL_Grapple_GuiFontHeight);
    lua_setfield(L, -2, "GuiFontHeight");
    lua_pushcfunction(L, GenL_Grapple_GuiGridBeginOwned);
    lua_setfield(L, -2, "GuiGridBeginOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCell);
    lua_setfield(L, -2, "GuiGridCell");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCellOwned);
    lua_setfield(L, -2, "GuiGridCellOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCellPart);
    lua_setfield(L, -2, "GuiGridCellPart");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCellPartOwned);
    lua_setfield(L, -2, "GuiGridCellPartOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCellSpan);
    lua_setfield(L, -2, "GuiGridCellSpan");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCellSpanOwned);
    lua_setfield(L, -2, "GuiGridCellSpanOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridCreate);
    lua_setfield(L, -2, "GuiGridCreate");
    lua_pushcfunction(L, GenL_Grapple_GuiGridDestroy);
    lua_setfield(L, -2, "GuiGridDestroy");
    lua_pushcfunction(L, GenL_Grapple_GuiGridEnd);
    lua_setfield(L, -2, "GuiGridEnd");
    lua_pushcfunction(L, GenL_Grapple_GuiGridEndOwned);
    lua_setfield(L, -2, "GuiGridEndOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridNextRow);
    lua_setfield(L, -2, "GuiGridNextRow");
    lua_pushcfunction(L, GenL_Grapple_GuiGridNextRowOwned);
    lua_setfield(L, -2, "GuiGridNextRowOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridRowHeight);
    lua_setfield(L, -2, "GuiGridRowHeight");
    lua_pushcfunction(L, GenL_Grapple_GuiGridRowHeightOwned);
    lua_setfield(L, -2, "GuiGridRowHeightOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridSpacing);
    lua_setfield(L, -2, "GuiGridSpacing");
    lua_pushcfunction(L, GenL_Grapple_GuiGridSpacingOwned);
    lua_setfield(L, -2, "GuiGridSpacingOwned");
    lua_pushcfunction(L, GenL_Grapple_GuiGridWeight);
    lua_setfield(L, -2, "GuiGridWeight");
    lua_pushcfunction(L, GenL_Grapple_GuiImage);
    lua_setfield(L, -2, "GuiImage");
    lua_pushcfunction(L, GenL_Grapple_GuiImageButton);
    lua_setfield(L, -2, "GuiImageButton");
    lua_pushcfunction(L, GenL_Grapple_GuiInputBegin);
    lua_setfield(L, -2, "GuiInputBegin");
    lua_pushcfunction(L, GenL_Grapple_GuiInputEnd);
    lua_setfield(L, -2, "GuiInputEnd");
    lua_pushcfunction(L, GenL_Grapple_GuiKeyPressed);
    lua_setfield(L, -2, "GuiKeyPressed");
    lua_pushcfunction(L, GenL_Grapple_GuiMemoryUsed);
    lua_setfield(L, -2, "GuiMemoryUsed");
    lua_pushcfunction(L, GenL_Grapple_GuiOpenFileButton);
    lua_setfield(L, -2, "GuiOpenFileButton");
    lua_pushcfunction(L, GenL_Grapple_GuiPopFont);
    lua_setfield(L, -2, "GuiPopFont");
    lua_pushcfunction(L, GenL_Grapple_GuiPopStyleColor);
    lua_setfield(L, -2, "GuiPopStyleColor");
    lua_pushcfunction(L, GenL_Grapple_GuiProcessEvent);
    lua_setfield(L, -2, "GuiProcessEvent");
    lua_pushcfunction(L, GenL_Grapple_GuiPumpEvents);
    lua_setfield(L, -2, "GuiPumpEvents");
    lua_pushcfunction(L, GenL_Grapple_GuiPushFont);
    lua_setfield(L, -2, "GuiPushFont");
    lua_pushcfunction(L, GenL_Grapple_GuiPushStyleColor);
    lua_setfield(L, -2, "GuiPushStyleColor");
    lua_pushcfunction(L, GenL_Grapple_GuiRender);
    lua_setfield(L, -2, "GuiRender");
    lua_pushcfunction(L, GenL_Grapple_GuiRenderer);
    lua_setfield(L, -2, "GuiRenderer");
    lua_pushcfunction(L, GenL_Grapple_GuiSaveFileButton);
    lua_setfield(L, -2, "GuiSaveFileButton");
    lua_pushcfunction(L, GenL_Grapple_GuiSavedPath);
    lua_setfield(L, -2, "GuiSavedPath");
    lua_pushcfunction(L, GenL_Grapple_GuiScale);
    lua_setfield(L, -2, "GuiScale");
    lua_pushcfunction(L, GenL_Grapple_GuiSetFont);
    lua_setfield(L, -2, "GuiSetFont");
    lua_pushcfunction(L, GenL_Grapple_GuiSetTooltipDelay);
    lua_setfield(L, -2, "GuiSetTooltipDelay");
    lua_pushcfunction(L, GenL_Grapple_GuiTooltip);
    lua_setfield(L, -2, "GuiTooltip");
    lua_pushcfunction(L, GenL_Grapple_GuiTooltipDelay);
    lua_setfield(L, -2, "GuiTooltipDelay");
    lua_pushcfunction(L, GenL_Grapple_GuiWantsInput);
    lua_setfield(L, -2, "GuiWantsInput");
    lua_pushcfunction(L, GenL_Grapple_HMACSHA256);
    lua_setfield(L, -2, "HMACSHA256");
    lua_pushcfunction(L, GenL_Grapple_HasDeviceMotion);
    lua_setfield(L, -2, "HasDeviceMotion");
    lua_pushcfunction(L, GenL_Grapple_IdleSeconds);
    lua_setfield(L, -2, "IdleSeconds");
    lua_pushcfunction(L, GenL_Grapple_KeyDown);
    lua_setfield(L, -2, "KeyDown");
    lua_pushcfunction(L, GenL_Grapple_KeyModifiers);
    lua_setfield(L, -2, "KeyModifiers");
    lua_pushcfunction(L, GenL_Grapple_KeyPressed);
    lua_setfield(L, -2, "KeyPressed");
    lua_pushcfunction(L, GenL_Grapple_KeyReleased);
    lua_setfield(L, -2, "KeyReleased");
    lua_pushcfunction(L, GenL_Grapple_LastInputDevice);
    lua_setfield(L, -2, "LastInputDevice");
    lua_pushcfunction(L, GenL_Grapple_LightAddDarkZone);
    lua_setfield(L, -2, "LightAddDarkZone");
    lua_pushcfunction(L, GenL_Grapple_LightAddOccluder);
    lua_setfield(L, -2, "LightAddOccluder");
    lua_pushcfunction(L, GenL_Grapple_LightAddOccluderLine);
    lua_setfield(L, -2, "LightAddOccluderLine");
    lua_pushcfunction(L, GenL_Grapple_LightAmbient);
    lua_setfield(L, -2, "LightAmbient");
    lua_pushcfunction(L, GenL_Grapple_LightAt);
    lua_setfield(L, -2, "LightAt");
    lua_pushcfunction(L, GenL_Grapple_LightBeginFrame);
    lua_setfield(L, -2, "LightBeginFrame");
    lua_pushcfunction(L, GenL_Grapple_LightCount);
    lua_setfield(L, -2, "LightCount");
    lua_pushcfunction(L, GenL_Grapple_LightDefDestroy);
    lua_setfield(L, -2, "LightDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetColor);
    lua_setfield(L, -2, "LightDefSetColor");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetCone);
    lua_setfield(L, -2, "LightDefSetCone");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetFlicker);
    lua_setfield(L, -2, "LightDefSetFlicker");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetOffset);
    lua_setfield(L, -2, "LightDefSetOffset");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetRadius);
    lua_setfield(L, -2, "LightDefSetRadius");
    lua_pushcfunction(L, GenL_Grapple_LightDefSetShadows);
    lua_setfield(L, -2, "LightDefSetShadows");
    lua_pushcfunction(L, GenL_Grapple_LightDefault);
    lua_setfield(L, -2, "LightDefault");
    lua_pushcfunction(L, GenL_Grapple_LightHour);
    lua_setfield(L, -2, "LightHour");
    lua_pushcfunction(L, GenL_Grapple_LightLineOfSight);
    lua_setfield(L, -2, "LightLineOfSight");
    lua_pushcfunction(L, GenL_Grapple_LightPreset_);
    lua_setfield(L, -2, "LightPreset_");
    lua_pushcfunction(L, GenL_Grapple_LightRender);
    lua_setfield(L, -2, "LightRender");
    lua_pushcfunction(L, GenL_Grapple_LightSetAmbient);
    lua_setfield(L, -2, "LightSetAmbient");
    lua_pushcfunction(L, GenL_Grapple_LightSetAutoOccluders);
    lua_setfield(L, -2, "LightSetAutoOccluders");
    lua_pushcfunction(L, GenL_Grapple_LightSetClock);
    lua_setfield(L, -2, "LightSetClock");
    lua_pushcfunction(L, GenL_Grapple_LightSetPreset);
    lua_setfield(L, -2, "LightSetPreset");
    lua_pushcfunction(L, GenL_Grapple_LightSunlight);
    lua_setfield(L, -2, "LightSunlight");
    lua_pushcfunction(L, GenL_Grapple_LightUsesShaders);
    lua_setfield(L, -2, "LightUsesShaders");
    lua_pushcfunction(L, GenL_Grapple_LoadTextFile);
    lua_setfield(L, -2, "LoadTextFile");
    lua_pushcfunction(L, GenL_Grapple_LoadTexture);
    lua_setfield(L, -2, "LoadTexture");
    lua_pushcfunction(L, GenL_Grapple_LoadTextureAsync);
    lua_setfield(L, -2, "LoadTextureAsync");
    lua_pushcfunction(L, GenL_Grapple_LoadTiledMap);
    lua_setfield(L, -2, "LoadTiledMap");
    lua_pushcfunction(L, GenL_Grapple_MotorJointDefCreate);
    lua_setfield(L, -2, "MotorJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_MotorJointDefDestroy);
    lua_setfield(L, -2, "MotorJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_MotorJointDefSetBodies);
    lua_setfield(L, -2, "MotorJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_MountEncryptedArchive);
    lua_setfield(L, -2, "MountEncryptedArchive");
    lua_pushcfunction(L, GenL_Grapple_MountEncryptedArchiveFile);
    lua_setfield(L, -2, "MountEncryptedArchiveFile");
    lua_pushcfunction(L, GenL_Grapple_MountMedia);
    lua_setfield(L, -2, "MountMedia");
    lua_pushcfunction(L, GenL_Grapple_MouseCaptured);
    lua_setfield(L, -2, "MouseCaptured");
    lua_pushcfunction(L, GenL_Grapple_MouseDelta);
    lua_setfield(L, -2, "MouseDelta");
    lua_pushcfunction(L, GenL_Grapple_MouseDown);
    lua_setfield(L, -2, "MouseDown");
    lua_pushcfunction(L, GenL_Grapple_MouseJointDefCreate);
    lua_setfield(L, -2, "MouseJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_MouseJointDefDestroy);
    lua_setfield(L, -2, "MouseJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_MouseJointDefSetBodies);
    lua_setfield(L, -2, "MouseJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_MouseJointDefSetMaxForce);
    lua_setfield(L, -2, "MouseJointDefSetMaxForce");
    lua_pushcfunction(L, GenL_Grapple_MouseJointDefSetSpring);
    lua_setfield(L, -2, "MouseJointDefSetSpring");
    lua_pushcfunction(L, GenL_Grapple_MousePosition);
    lua_setfield(L, -2, "MousePosition");
    lua_pushcfunction(L, GenL_Grapple_MousePressed);
    lua_setfield(L, -2, "MousePressed");
    lua_pushcfunction(L, GenL_Grapple_MouseReleased);
    lua_setfield(L, -2, "MouseReleased");
    lua_pushcfunction(L, GenL_Grapple_MouseWheel);
    lua_setfield(L, -2, "MouseWheel");
    lua_pushcfunction(L, GenL_Grapple_OpenVFSRead);
    lua_setfield(L, -2, "OpenVFSRead");
    lua_pushcfunction(L, GenL_Grapple_PhysicsBodyCount);
    lua_setfield(L, -2, "PhysicsBodyCount");
    lua_pushcfunction(L, GenL_Grapple_PhysicsGravity);
    lua_setfield(L, -2, "PhysicsGravity");
    lua_pushcfunction(L, GenL_Grapple_PhysicsOverlap);
    lua_setfield(L, -2, "PhysicsOverlap");
    lua_pushcfunction(L, GenL_Grapple_PhysicsPaused);
    lua_setfield(L, -2, "PhysicsPaused");
    lua_pushcfunction(L, GenL_Grapple_PhysicsPixelsPerMetre);
    lua_setfield(L, -2, "PhysicsPixelsPerMetre");
    lua_pushcfunction(L, GenL_Grapple_PhysicsRaycast);
    lua_setfield(L, -2, "PhysicsRaycast");
    lua_pushcfunction(L, GenL_Grapple_PhysicsSetGravity);
    lua_setfield(L, -2, "PhysicsSetGravity");
    lua_pushcfunction(L, GenL_Grapple_PhysicsSetPaused);
    lua_setfield(L, -2, "PhysicsSetPaused");
    lua_pushcfunction(L, GenL_Grapple_PhysicsSetPixelsPerMetre);
    lua_setfield(L, -2, "PhysicsSetPixelsPerMetre");
    lua_pushcfunction(L, GenL_Grapple_PhysicsSetSubSteps);
    lua_setfield(L, -2, "PhysicsSetSubSteps");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefCreate);
    lua_setfield(L, -2, "PrismaticJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefDestroy);
    lua_setfield(L, -2, "PrismaticJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefSetAnchors);
    lua_setfield(L, -2, "PrismaticJointDefSetAnchors");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefSetAxis);
    lua_setfield(L, -2, "PrismaticJointDefSetAxis");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefSetBodies);
    lua_setfield(L, -2, "PrismaticJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefSetLimit);
    lua_setfield(L, -2, "PrismaticJointDefSetLimit");
    lua_pushcfunction(L, GenL_Grapple_PrismaticJointDefSetMotor);
    lua_setfield(L, -2, "PrismaticJointDefSetMotor");
    lua_pushcfunction(L, GenL_Grapple_QuitDebugText);
    lua_setfield(L, -2, "QuitDebugText");
    lua_pushcfunction(L, GenL_Grapple_RegexEscape);
    lua_setfield(L, -2, "RegexEscape");
    lua_pushcfunction(L, GenL_Grapple_RegexFlags);
    lua_setfield(L, -2, "RegexFlags");
    lua_pushcfunction(L, GenL_Grapple_RegexGroup);
    lua_setfield(L, -2, "RegexGroup");
    lua_pushcfunction(L, GenL_Grapple_RegexGroupBegin);
    lua_setfield(L, -2, "RegexGroupBegin");
    lua_pushcfunction(L, GenL_Grapple_RegexGroupCount);
    lua_setfield(L, -2, "RegexGroupCount");
    lua_pushcfunction(L, GenL_Grapple_RegexGroupEnd);
    lua_setfield(L, -2, "RegexGroupEnd");
    lua_pushcfunction(L, GenL_Grapple_RegexMatchAt);
    lua_setfield(L, -2, "RegexMatchAt");
    lua_pushcfunction(L, GenL_Grapple_RegexNamedGroup);
    lua_setfield(L, -2, "RegexNamedGroup");
    lua_pushcfunction(L, GenL_Grapple_RegexNamedGroupCount);
    lua_setfield(L, -2, "RegexNamedGroupCount");
    lua_pushcfunction(L, GenL_Grapple_RegexNamedGroupName);
    lua_setfield(L, -2, "RegexNamedGroupName");
    lua_pushcfunction(L, GenL_Grapple_RegexPattern);
    lua_setfield(L, -2, "RegexPattern");
    lua_pushcfunction(L, GenL_Grapple_RegexReplace);
    lua_setfield(L, -2, "RegexReplace");
    lua_pushcfunction(L, GenL_Grapple_RegexSearch);
    lua_setfield(L, -2, "RegexSearch");
    lua_pushcfunction(L, GenL_Grapple_RenderDebugText);
    lua_setfield(L, -2, "RenderDebugText");
    lua_pushcfunction(L, GenL_Grapple_RenderLastStats);
    lua_setfield(L, -2, "RenderLastStats");
    lua_pushcfunction(L, GenL_Grapple_RenderLighting);
    lua_setfield(L, -2, "RenderLighting");
    lua_pushcfunction(L, GenL_Grapple_RenderOverlay);
    lua_setfield(L, -2, "RenderOverlay");
    lua_pushcfunction(L, GenL_Grapple_RenderWorld);
    lua_setfield(L, -2, "RenderWorld");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefCreate);
    lua_setfield(L, -2, "RevoluteJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefDestroy);
    lua_setfield(L, -2, "RevoluteJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetAnchors);
    lua_setfield(L, -2, "RevoluteJointDefSetAnchors");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetBodies);
    lua_setfield(L, -2, "RevoluteJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetCollideConnected);
    lua_setfield(L, -2, "RevoluteJointDefSetCollideConnected");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetLimit);
    lua_setfield(L, -2, "RevoluteJointDefSetLimit");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetMotor);
    lua_setfield(L, -2, "RevoluteJointDefSetMotor");
    lua_pushcfunction(L, GenL_Grapple_RevoluteJointDefSetSpring);
    lua_setfield(L, -2, "RevoluteJointDefSetSpring");
    lua_pushcfunction(L, GenL_Grapple_SHA256);
    lua_setfield(L, -2, "SHA256");
    lua_pushcfunction(L, GenL_Grapple_SampleLight);
    lua_setfield(L, -2, "SampleLight");
    lua_pushcfunction(L, GenL_Grapple_SaveDelete);
    lua_setfield(L, -2, "SaveDelete");
    lua_pushcfunction(L, GenL_Grapple_SaveExists);
    lua_setfield(L, -2, "SaveExists");
    lua_pushcfunction(L, GenL_Grapple_SaveInfoOf);
    lua_setfield(L, -2, "SaveInfoOf");
    lua_pushcfunction(L, GenL_Grapple_SavePath);
    lua_setfield(L, -2, "SavePath");
    lua_pushcfunction(L, GenL_Grapple_SaveSetIdentity);
    lua_setfield(L, -2, "SaveSetIdentity");
    lua_pushcfunction(L, GenL_Grapple_SaveWrite);
    lua_setfield(L, -2, "SaveWrite");
    lua_pushcfunction(L, GenL_Grapple_SceneCurrent);
    lua_setfield(L, -2, "SceneCurrent");
    lua_pushcfunction(L, GenL_Grapple_SceneDepth);
    lua_setfield(L, -2, "SceneDepth");
    lua_pushcfunction(L, GenL_Grapple_SceneEngine);
    lua_setfield(L, -2, "SceneEngine");
    lua_pushcfunction(L, GenL_Grapple_SceneFind);
    lua_setfield(L, -2, "SceneFind");
    lua_pushcfunction(L, GenL_Grapple_SceneIsActive);
    lua_setfield(L, -2, "SceneIsActive");
    lua_pushcfunction(L, GenL_Grapple_SceneKey);
    lua_setfield(L, -2, "SceneKey");
    lua_pushcfunction(L, GenL_Grapple_SceneName);
    lua_setfield(L, -2, "SceneName");
    lua_pushcfunction(L, GenL_Grapple_ScenePop);
    lua_setfield(L, -2, "ScenePop");
    lua_pushcfunction(L, GenL_Grapple_ScenePush);
    lua_setfield(L, -2, "ScenePush");
    lua_pushcfunction(L, GenL_Grapple_SceneReplace);
    lua_setfield(L, -2, "SceneReplace");
    lua_pushcfunction(L, GenL_Grapple_SceneReset);
    lua_setfield(L, -2, "SceneReset");
    lua_pushcfunction(L, GenL_Grapple_SceneSetTransitionColor);
    lua_setfield(L, -2, "SceneSetTransitionColor");
    lua_pushcfunction(L, GenL_Grapple_SceneTransitionTo);
    lua_setfield(L, -2, "SceneTransitionTo");
    lua_pushcfunction(L, GenL_Grapple_SceneTransitioning);
    lua_setfield(L, -2, "SceneTransitioning");
    lua_pushcfunction(L, GenL_Grapple_ScriptHasHandlers);
    lua_setfield(L, -2, "ScriptHasHandlers");
    lua_pushcfunction(L, GenL_Grapple_ScriptRun);
    lua_setfield(L, -2, "ScriptRun");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneDefine);
    lua_setfield(L, -2, "ScriptSceneDefine");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneDefined);
    lua_setfield(L, -2, "ScriptSceneDefined");
    lua_pushcfunction(L, GenL_Grapple_ScriptScenePush);
    lua_setfield(L, -2, "ScriptScenePush");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneReplace);
    lua_setfield(L, -2, "ScriptSceneReplace");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneReset);
    lua_setfield(L, -2, "ScriptSceneReset");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneSetFlags);
    lua_setfield(L, -2, "ScriptSceneSetFlags");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneSetHook);
    lua_setfield(L, -2, "ScriptSceneSetHook");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneSetStateSize);
    lua_setfield(L, -2, "ScriptSceneSetStateSize");
    lua_pushcfunction(L, GenL_Grapple_ScriptSceneTransitionTo);
    lua_setfield(L, -2, "ScriptSceneTransitionTo");
    lua_pushcfunction(L, GenL_Grapple_ScriptSetHook);
    lua_setfield(L, -2, "ScriptSetHook");
    lua_pushcfunction(L, GenL_Grapple_ScriptUnbind);
    lua_setfield(L, -2, "ScriptUnbind");
    lua_pushcfunction(L, GenL_Grapple_SetDebugTextSize);
    lua_setfield(L, -2, "SetDebugTextSize");
    lua_pushcfunction(L, GenL_Grapple_SetDeviceMotion);
    lua_setfield(L, -2, "SetDeviceMotion");
    lua_pushcfunction(L, GenL_Grapple_SetDirectionRepeat);
    lua_setfield(L, -2, "SetDirectionRepeat");
    lua_pushcfunction(L, GenL_Grapple_SetGamepadDeadzone);
    lua_setfield(L, -2, "SetGamepadDeadzone");
    lua_pushcfunction(L, GenL_Grapple_SetGamepadMotion);
    lua_setfield(L, -2, "SetGamepadMotion");
    lua_pushcfunction(L, GenL_Grapple_SetLightAmbient);
    lua_setfield(L, -2, "SetLightAmbient");
    lua_pushcfunction(L, GenL_Grapple_SetLightDebugDraw);
    lua_setfield(L, -2, "SetLightDebugDraw");
    lua_pushcfunction(L, GenL_Grapple_SetLightMapScale);
    lua_setfield(L, -2, "SetLightMapScale");
    lua_pushcfunction(L, GenL_Grapple_SetLightRayCount);
    lua_setfield(L, -2, "SetLightRayCount");
    lua_pushcfunction(L, GenL_Grapple_SetLightRings);
    lua_setfield(L, -2, "SetLightRings");
    lua_pushcfunction(L, GenL_Grapple_SetLightShadowSoftness);
    lua_setfield(L, -2, "SetLightShadowSoftness");
    lua_pushcfunction(L, GenL_Grapple_SetLightUseShaders);
    lua_setfield(L, -2, "SetLightUseShaders");
    lua_pushcfunction(L, GenL_Grapple_SetMouseCapture);
    lua_setfield(L, -2, "SetMouseCapture");
    lua_pushcfunction(L, GenL_Grapple_SetTextInput);
    lua_setfield(L, -2, "SetTextInput");
    lua_pushcfunction(L, GenL_Grapple_SetTriggerThreshold);
    lua_setfield(L, -2, "SetTriggerThreshold");
    lua_pushcfunction(L, GenL_Grapple_ShowOpenFileDialog);
    lua_setfield(L, -2, "ShowOpenFileDialog");
    lua_pushcfunction(L, GenL_Grapple_ShowSaveFileDialog);
    lua_setfield(L, -2, "ShowSaveFileDialog");
    lua_pushcfunction(L, GenL_Grapple_SpriteCreate);
    lua_setfield(L, -2, "SpriteCreate");
    lua_pushcfunction(L, GenL_Grapple_SpriteDestroy);
    lua_setfield(L, -2, "SpriteDestroy");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetColor);
    lua_setfield(L, -2, "SpriteSetColor");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetLayer);
    lua_setfield(L, -2, "SpriteSetLayer");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetOrigin);
    lua_setfield(L, -2, "SpriteSetOrigin");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetScreenSpace);
    lua_setfield(L, -2, "SpriteSetScreenSpace");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetSize);
    lua_setfield(L, -2, "SpriteSetSize");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetSortByY);
    lua_setfield(L, -2, "SpriteSetSortByY");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetSource);
    lua_setfield(L, -2, "SpriteSetSource");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetTexture);
    lua_setfield(L, -2, "SpriteSetTexture");
    lua_pushcfunction(L, GenL_Grapple_SpriteSetVisible);
    lua_setfield(L, -2, "SpriteSetVisible");
    lua_pushcfunction(L, GenL_Grapple_Text);
    lua_setfield(L, -2, "Text");
    lua_pushcfunction(L, GenL_Grapple_TextCount);
    lua_setfield(L, -2, "TextCount");
    lua_pushcfunction(L, GenL_Grapple_TextHas);
    lua_setfield(L, -2, "TextHas");
    lua_pushcfunction(L, GenL_Grapple_TextLanguage);
    lua_setfield(L, -2, "TextLanguage");
    lua_pushcfunction(L, GenL_Grapple_TextLoad);
    lua_setfield(L, -2, "TextLoad");
    lua_pushcfunction(L, GenL_Grapple_TextLoadFile);
    lua_setfield(L, -2, "TextLoadFile");
    lua_pushcfunction(L, GenL_Grapple_TextSetLanguage);
    lua_setfield(L, -2, "TextSetLanguage");
    lua_pushcfunction(L, GenL_Grapple_TextTyped);
    lua_setfield(L, -2, "TextTyped");
    lua_pushcfunction(L, GenL_Grapple_Texture);
    lua_setfield(L, -2, "Texture");
    lua_pushcfunction(L, GenL_Grapple_TiledLayerCount);
    lua_setfield(L, -2, "TiledLayerCount");
    lua_pushcfunction(L, GenL_Grapple_TiledLayerName);
    lua_setfield(L, -2, "TiledLayerName");
    lua_pushcfunction(L, GenL_Grapple_TiledLayerType);
    lua_setfield(L, -2, "TiledLayerType");
    lua_pushcfunction(L, GenL_Grapple_TiledMapHeight);
    lua_setfield(L, -2, "TiledMapHeight");
    lua_pushcfunction(L, GenL_Grapple_TiledMapWidth);
    lua_setfield(L, -2, "TiledMapWidth");
    lua_pushcfunction(L, GenL_Grapple_TiledObjectAt);
    lua_setfield(L, -2, "TiledObjectAt");
    lua_pushcfunction(L, GenL_Grapple_TiledObjectCount);
    lua_setfield(L, -2, "TiledObjectCount");
    lua_pushcfunction(L, GenL_Grapple_TiledRaw);
    lua_setfield(L, -2, "TiledRaw");
    lua_pushcfunction(L, GenL_Grapple_TiledTileAt);
    lua_setfield(L, -2, "TiledTileAt");
    lua_pushcfunction(L, GenL_Grapple_TiledTileHeight);
    lua_setfield(L, -2, "TiledTileHeight");
    lua_pushcfunction(L, GenL_Grapple_TiledTileWidth);
    lua_setfield(L, -2, "TiledTileWidth");
    lua_pushcfunction(L, GenL_Grapple_TouchPinch);
    lua_setfield(L, -2, "TouchPinch");
    lua_pushcfunction(L, GenL_Grapple_TouchRotation);
    lua_setfield(L, -2, "TouchRotation");
    lua_pushcfunction(L, GenL_Grapple_WeldJointDefCreate);
    lua_setfield(L, -2, "WeldJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_WeldJointDefDestroy);
    lua_setfield(L, -2, "WeldJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_WeldJointDefSetAnchors);
    lua_setfield(L, -2, "WeldJointDefSetAnchors");
    lua_pushcfunction(L, GenL_Grapple_WeldJointDefSetBodies);
    lua_setfield(L, -2, "WeldJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_WeldJointDefSetSpring);
    lua_setfield(L, -2, "WeldJointDefSetSpring");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefCreate);
    lua_setfield(L, -2, "WheelJointDefCreate");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefDestroy);
    lua_setfield(L, -2, "WheelJointDefDestroy");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefSetAnchors);
    lua_setfield(L, -2, "WheelJointDefSetAnchors");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefSetAxis);
    lua_setfield(L, -2, "WheelJointDefSetAxis");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefSetBodies);
    lua_setfield(L, -2, "WheelJointDefSetBodies");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefSetMotor);
    lua_setfield(L, -2, "WheelJointDefSetMotor");
    lua_pushcfunction(L, GenL_Grapple_WheelJointDefSetSpring);
    lua_setfield(L, -2, "WheelJointDefSetSpring");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_MISSING);
    lua_setfield(L, -2, "GRAPPLE_ASSET_MISSING");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_QUEUED);
    lua_setfield(L, -2, "GRAPPLE_ASSET_QUEUED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_LOADING);
    lua_setfield(L, -2, "GRAPPLE_ASSET_LOADING");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_DECODED);
    lua_setfield(L, -2, "GRAPPLE_ASSET_DECODED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_READY);
    lua_setfield(L, -2, "GRAPPLE_ASSET_READY");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_ASSET_FAILED);
    lua_setfield(L, -2, "GRAPPLE_ASSET_FAILED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_NONE);
    lua_setfield(L, -2, "GRAPPLE_BIND_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_KEY);
    lua_setfield(L, -2, "GRAPPLE_BIND_KEY");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_MOUSE_BUTTON);
    lua_setfield(L, -2, "GRAPPLE_BIND_MOUSE_BUTTON");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_MOUSE_WHEEL);
    lua_setfield(L, -2, "GRAPPLE_BIND_MOUSE_WHEEL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_PAD_BUTTON);
    lua_setfield(L, -2, "GRAPPLE_BIND_PAD_BUTTON");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_PAD_AXIS);
    lua_setfield(L, -2, "GRAPPLE_BIND_PAD_AXIS");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BIND_PAD_DIRECTION);
    lua_setfield(L, -2, "GRAPPLE_BIND_PAD_DIRECTION");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BODY_STATIC);
    lua_setfield(L, -2, "GRAPPLE_BODY_STATIC");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BODY_KINEMATIC);
    lua_setfield(L, -2, "GRAPPLE_BODY_KINEMATIC");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BODY_DYNAMIC);
    lua_setfield(L, -2, "GRAPPLE_BODY_DYNAMIC");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_COIN);
    lua_setfield(L, -2, "GRAPPLE_SFX_COIN");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_LASER);
    lua_setfield(L, -2, "GRAPPLE_SFX_LASER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_JUMP);
    lua_setfield(L, -2, "GRAPPLE_SFX_JUMP");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_EXPLOSION);
    lua_setfield(L, -2, "GRAPPLE_SFX_EXPLOSION");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_POWERUP);
    lua_setfield(L, -2, "GRAPPLE_SFX_POWERUP");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SFX_HURT);
    lua_setfield(L, -2, "GRAPPLE_SFX_HURT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_SQUARE_125);
    lua_setfield(L, -2, "GRAPPLE_CHIP_SQUARE_125");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_SQUARE_25);
    lua_setfield(L, -2, "GRAPPLE_CHIP_SQUARE_25");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_SQUARE_50);
    lua_setfield(L, -2, "GRAPPLE_CHIP_SQUARE_50");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_TRIANGLE);
    lua_setfield(L, -2, "GRAPPLE_CHIP_TRIANGLE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_SAW);
    lua_setfield(L, -2, "GRAPPLE_CHIP_SAW");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_NOISE);
    lua_setfield(L, -2, "GRAPPLE_CHIP_NOISE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_NOISE_METALLIC);
    lua_setfield(L, -2, "GRAPPLE_CHIP_NOISE_METALLIC");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_CHIP_SINE);
    lua_setfield(L, -2, "GRAPPLE_CHIP_SINE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_COLORBLIND_NONE);
    lua_setfield(L, -2, "GRAPPLE_COLORBLIND_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_COLORBLIND_PROTANOPIA);
    lua_setfield(L, -2, "GRAPPLE_COLORBLIND_PROTANOPIA");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_COLORBLIND_DEUTERANOPIA);
    lua_setfield(L, -2, "GRAPPLE_COLORBLIND_DEUTERANOPIA");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_COLORBLIND_TRITANOPIA);
    lua_setfield(L, -2, "GRAPPLE_COLORBLIND_TRITANOPIA");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIALOG_IDLE);
    lua_setfield(L, -2, "GRAPPLE_DIALOG_IDLE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIALOG_PENDING);
    lua_setfield(L, -2, "GRAPPLE_DIALOG_PENDING");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIALOG_ACCEPTED);
    lua_setfield(L, -2, "GRAPPLE_DIALOG_ACCEPTED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIALOG_CANCELLED);
    lua_setfield(L, -2, "GRAPPLE_DIALOG_CANCELLED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIALOG_ERROR);
    lua_setfield(L, -2, "GRAPPLE_DIALOG_ERROR");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIR_UP);
    lua_setfield(L, -2, "GRAPPLE_DIR_UP");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIR_DOWN);
    lua_setfield(L, -2, "GRAPPLE_DIR_DOWN");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIR_LEFT);
    lua_setfield(L, -2, "GRAPPLE_DIR_LEFT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIR_RIGHT);
    lua_setfield(L, -2, "GRAPPLE_DIR_RIGHT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DIR_COUNT);
    lua_setfield(L, -2, "GRAPPLE_DIR_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BACKEND_OPENGL);
    lua_setfield(L, -2, "GRAPPLE_BACKEND_OPENGL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BACKEND_NATIVE);
    lua_setfield(L, -2, "GRAPPLE_BACKEND_NATIVE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_BACKEND_SOFTWARE);
    lua_setfield(L, -2, "GRAPPLE_BACKEND_SOFTWARE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_INTERPOLATE_LERP);
    lua_setfield(L, -2, "GRAPPLE_INTERPOLATE_LERP");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_INTERPOLATE_NONE);
    lua_setfield(L, -2, "GRAPPLE_INTERPOLATE_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_INTERPOLATE_EXTRAPOLATE);
    lua_setfield(L, -2, "GRAPPLE_INTERPOLATE_EXTRAPOLATE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_LETTERBOX);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_LETTERBOX");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_EXPAND);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_EXPAND");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_OVERSCAN);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_OVERSCAN");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_INTEGER);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_INTEGER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_STRETCH);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_STRETCH");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PRESENT_NATIVE);
    lua_setfield(L, -2, "GRAPPLE_PRESENT_NATIVE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_LEFT_X);
    lua_setfield(L, -2, "GRAPPLE_AXIS_LEFT_X");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_LEFT_Y);
    lua_setfield(L, -2, "GRAPPLE_AXIS_LEFT_Y");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_RIGHT_X);
    lua_setfield(L, -2, "GRAPPLE_AXIS_RIGHT_X");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_RIGHT_Y);
    lua_setfield(L, -2, "GRAPPLE_AXIS_RIGHT_Y");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_LEFT_TRIGGER);
    lua_setfield(L, -2, "GRAPPLE_AXIS_LEFT_TRIGGER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_RIGHT_TRIGGER);
    lua_setfield(L, -2, "GRAPPLE_AXIS_RIGHT_TRIGGER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AXIS_COUNT);
    lua_setfield(L, -2, "GRAPPLE_AXIS_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_A);
    lua_setfield(L, -2, "GRAPPLE_PAD_A");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_B);
    lua_setfield(L, -2, "GRAPPLE_PAD_B");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_X);
    lua_setfield(L, -2, "GRAPPLE_PAD_X");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_Y);
    lua_setfield(L, -2, "GRAPPLE_PAD_Y");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_BACK);
    lua_setfield(L, -2, "GRAPPLE_PAD_BACK");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_GUIDE);
    lua_setfield(L, -2, "GRAPPLE_PAD_GUIDE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_START);
    lua_setfield(L, -2, "GRAPPLE_PAD_START");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_LEFT_STICK);
    lua_setfield(L, -2, "GRAPPLE_PAD_LEFT_STICK");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_RIGHT_STICK);
    lua_setfield(L, -2, "GRAPPLE_PAD_RIGHT_STICK");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_LEFT_SHOULDER);
    lua_setfield(L, -2, "GRAPPLE_PAD_LEFT_SHOULDER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_RIGHT_SHOULDER);
    lua_setfield(L, -2, "GRAPPLE_PAD_RIGHT_SHOULDER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_DPAD_UP);
    lua_setfield(L, -2, "GRAPPLE_PAD_DPAD_UP");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_DPAD_DOWN);
    lua_setfield(L, -2, "GRAPPLE_PAD_DPAD_DOWN");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_DPAD_LEFT);
    lua_setfield(L, -2, "GRAPPLE_PAD_DPAD_LEFT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_DPAD_RIGHT);
    lua_setfield(L, -2, "GRAPPLE_PAD_DPAD_RIGHT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_SHARE);
    lua_setfield(L, -2, "GRAPPLE_PAD_SHARE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_PADDLE1);
    lua_setfield(L, -2, "GRAPPLE_PAD_PADDLE1");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_PADDLE2);
    lua_setfield(L, -2, "GRAPPLE_PAD_PADDLE2");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_PADDLE3);
    lua_setfield(L, -2, "GRAPPLE_PAD_PADDLE3");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_PADDLE4);
    lua_setfield(L, -2, "GRAPPLE_PAD_PADDLE4");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_TOUCHPAD);
    lua_setfield(L, -2, "GRAPPLE_PAD_TOUCHPAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_LEFT_TRIGGER);
    lua_setfield(L, -2, "GRAPPLE_PAD_LEFT_TRIGGER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_RIGHT_TRIGGER);
    lua_setfield(L, -2, "GRAPPLE_PAD_RIGHT_TRIGGER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_PAD_BUTTON_COUNT);
    lua_setfield(L, -2, "GRAPPLE_PAD_BUTTON_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AA_OFF);
    lua_setfield(L, -2, "GRAPPLE_AA_OFF");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_AA_FXAA);
    lua_setfield(L, -2, "GRAPPLE_AA_FXAA");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_QUALITY_OFF);
    lua_setfield(L, -2, "GRAPPLE_QUALITY_OFF");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_QUALITY_LOW);
    lua_setfield(L, -2, "GRAPPLE_QUALITY_LOW");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_QUALITY_MEDIUM);
    lua_setfield(L, -2, "GRAPPLE_QUALITY_MEDIUM");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_QUALITY_HIGH);
    lua_setfield(L, -2, "GRAPPLE_QUALITY_HIGH");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_ALIGN_LEFT);
    lua_setfield(L, -2, "GRAPPLE_GUI_ALIGN_LEFT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_ALIGN_CENTER);
    lua_setfield(L, -2, "GRAPPLE_GUI_ALIGN_CENTER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_ALIGN_RIGHT);
    lua_setfield(L, -2, "GRAPPLE_GUI_ALIGN_RIGHT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_FONT_SMALL);
    lua_setfield(L, -2, "GRAPPLE_GUI_FONT_SMALL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_FONT_NORMAL);
    lua_setfield(L, -2, "GRAPPLE_GUI_FONT_NORMAL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_FONT_LARGE);
    lua_setfield(L, -2, "GRAPPLE_GUI_FONT_LARGE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_GLYPHS_LATIN1);
    lua_setfield(L, -2, "GRAPPLE_GUI_GLYPHS_LATIN1");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_GLYPHS_PUNCTUATION);
    lua_setfield(L, -2, "GRAPPLE_GUI_GLYPHS_PUNCTUATION");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_GLYPHS_CYRILLIC);
    lua_setfield(L, -2, "GRAPPLE_GUI_GLYPHS_CYRILLIC");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_GLYPHS_CHINESE);
    lua_setfield(L, -2, "GRAPPLE_GUI_GLYPHS_CHINESE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_GLYPHS_KOREAN);
    lua_setfield(L, -2, "GRAPPLE_GUI_GLYPHS_KOREAN");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_IMAGE_STRETCH);
    lua_setfield(L, -2, "GRAPPLE_GUI_IMAGE_STRETCH");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_IMAGE_ZOOM);
    lua_setfield(L, -2, "GRAPPLE_GUI_IMAGE_ZOOM");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_IMAGE_CENTER);
    lua_setfield(L, -2, "GRAPPLE_GUI_IMAGE_CENTER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_IMAGE_FILL);
    lua_setfield(L, -2, "GRAPPLE_GUI_IMAGE_FILL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_TEXT);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_TEXT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_BUTTON);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_BUTTON");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_BUTTON_HOVER);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_BUTTON_HOVER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_BUTTON_TEXT);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_BUTTON_TEXT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_GUI_COLOR_HEADER);
    lua_setfield(L, -2, "GRAPPLE_GUI_COLOR_HEADER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DEVICE_NONE);
    lua_setfield(L, -2, "GRAPPLE_DEVICE_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DEVICE_KEYBOARD);
    lua_setfield(L, -2, "GRAPPLE_DEVICE_KEYBOARD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DEVICE_MOUSE);
    lua_setfield(L, -2, "GRAPPLE_DEVICE_MOUSE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DEVICE_GAMEPAD);
    lua_setfield(L, -2, "GRAPPLE_DEVICE_GAMEPAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_DEVICE_TOUCH);
    lua_setfield(L, -2, "GRAPPLE_DEVICE_TOUCH");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_NONE);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_SUNRISE);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_SUNRISE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_AFTERNOON);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_AFTERNOON");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_SUNSET);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_SUNSET");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_NIGHT);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_NIGHT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_LIGHT_DARK);
    lua_setfield(L, -2, "GRAPPLE_LIGHT_DARK");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MEDIA_NONE);
    lua_setfield(L, -2, "GRAPPLE_MEDIA_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MEDIA_EXPLICIT);
    lua_setfield(L, -2, "GRAPPLE_MEDIA_EXPLICIT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MEDIA_EMBEDDED);
    lua_setfield(L, -2, "GRAPPLE_MEDIA_EMBEDDED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MEDIA_ARCHIVE);
    lua_setfield(L, -2, "GRAPPLE_MEDIA_ARCHIVE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MEDIA_DIRECTORY);
    lua_setfield(L, -2, "GRAPPLE_MEDIA_DIRECTORY");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_LEFT);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_LEFT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_MIDDLE);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_MIDDLE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_RIGHT);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_RIGHT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_X1);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_X1");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_X2);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_X2");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_MOUSE_COUNT);
    lua_setfield(L, -2, "GRAPPLE_MOUSE_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_DEFAULT);
    lua_setfield(L, -2, "GRAPPLE_SCENE_DEFAULT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_UPDATE_WHEN_COVERED);
    lua_setfield(L, -2, "GRAPPLE_SCENE_UPDATE_WHEN_COVERED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_TRANSPARENT);
    lua_setfield(L, -2, "GRAPPLE_SCENE_TRANSPARENT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_LOAD);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_LOAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_ENTER);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_ENTER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_FIXED_UPDATE);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_FIXED_UPDATE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_UPDATE);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_UPDATE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_RENDER);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_RENDER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_EVENT);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_EVENT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_EXIT);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_EXIT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_UNLOAD);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_UNLOAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SCENE_HOOK_COUNT);
    lua_setfield(L, -2, "GRAPPLE_SCENE_HOOK_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_TRANSITION_NONE);
    lua_setfield(L, -2, "GRAPPLE_TRANSITION_NONE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_TRANSITION_FADE);
    lua_setfield(L, -2, "GRAPPLE_TRANSITION_FADE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_LOAD);
    lua_setfield(L, -2, "GRAPPLE_HOOK_LOAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_FIXED_UPDATE);
    lua_setfield(L, -2, "GRAPPLE_HOOK_FIXED_UPDATE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_UPDATE);
    lua_setfield(L, -2, "GRAPPLE_HOOK_UPDATE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_RENDER);
    lua_setfield(L, -2, "GRAPPLE_HOOK_RENDER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_POST_RENDER);
    lua_setfield(L, -2, "GRAPPLE_HOOK_POST_RENDER");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_EVENT);
    lua_setfield(L, -2, "GRAPPLE_HOOK_EVENT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_RESIZE);
    lua_setfield(L, -2, "GRAPPLE_HOOK_RESIZE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_UNLOAD);
    lua_setfield(L, -2, "GRAPPLE_HOOK_UNLOAD");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_HOOK_COUNT);
    lua_setfield(L, -2, "GRAPPLE_HOOK_COUNT");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SHAPE_BOX);
    lua_setfield(L, -2, "GRAPPLE_SHAPE_BOX");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SHAPE_CIRCLE);
    lua_setfield(L, -2, "GRAPPLE_SHAPE_CIRCLE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SHAPE_CAPSULE);
    lua_setfield(L, -2, "GRAPPLE_SHAPE_CAPSULE");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SPLIT_HORIZONTAL);
    lua_setfield(L, -2, "GRAPPLE_SPLIT_HORIZONTAL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SPLIT_VERTICAL);
    lua_setfield(L, -2, "GRAPPLE_SPLIT_VERTICAL");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_SPLIT_GRID);
    lua_setfield(L, -2, "GRAPPLE_SPLIT_GRID");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_FILTER_AUTO);
    lua_setfield(L, -2, "GRAPPLE_FILTER_AUTO");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_FILTER_LINEAR);
    lua_setfield(L, -2, "GRAPPLE_FILTER_LINEAR");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_FILTER_NEAREST);
    lua_setfield(L, -2, "GRAPPLE_FILTER_NEAREST");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_WINDOW_WINDOWED);
    lua_setfield(L, -2, "GRAPPLE_WINDOW_WINDOWED");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_WINDOW_BORDERLESS);
    lua_setfield(L, -2, "GRAPPLE_WINDOW_BORDERLESS");
    lua_pushinteger(L, (lua_Integer)GRAPPLE_WINDOW_EXCLUSIVE);
    lua_setfield(L, -2, "GRAPPLE_WINDOW_EXCLUSIVE");
    lua_setglobal(L, "GrappleC");
    return 0;
}
