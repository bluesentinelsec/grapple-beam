/* GENERATED FILE - DO NOT EDIT.
 * Regenerate: python3 -m tools.bindgen
 * mruby bindings for Grapple modules. */
#include "../src/gen_support_ruby.h"

#include <grapple/audio_bus.h>
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
#include <grapple/engine_backend.h>
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
#include <grapple/engine_settings.h>
#include <grapple/engine_text.h>
#include <grapple/gpu_build.h>
#include <grapple/gpu_primitives.h>
#include <grapple/gui.h>
#include <grapple/gui_grid.h>
#include <grapple/light.h>
#include <grapple/physics_build.h>
#include <grapple/physics_draw.h>
#include <grapple/platformer.h>
#include <grapple/regex.h>
#include <grapple/signals.h>
#include <grapple/textfile.h>
#include <grapple/tiled.h>
#include <grapple/vfs.h>
#include <string.h>

static void GenRead_Grapple_ActorMessage(mrb_state *mrb, mrb_value h, Grapple_ActorMessage *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->id = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "id");
    out->sender = (Grapple_ActorId)GrappleGen_RubyFieldInt(mrb, h, "sender");
    out->a = (float)GrappleGen_RubyFieldNum(mrb, h, "a");
    out->b = (float)GrappleGen_RubyFieldNum(mrb, h, "b");
    out->value = (Sint64)GrappleGen_RubyFieldInt(mrb, h, "value");
}

static void GenRead_Grapple_ActorTransform(mrb_state *mrb, mrb_value h, Grapple_ActorTransform *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->x = (float)GrappleGen_RubyFieldNum(mrb, h, "x");
    out->y = (float)GrappleGen_RubyFieldNum(mrb, h, "y");
    out->rotation = (float)GrappleGen_RubyFieldNum(mrb, h, "rotation");
    out->scale_x = (float)GrappleGen_RubyFieldNum(mrb, h, "scale_x");
    out->scale_y = (float)GrappleGen_RubyFieldNum(mrb, h, "scale_y");
}

static mrb_value GenPush_Grapple_ActorTransform(mrb_state *mrb, const Grapple_ActorTransform *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "x", mrb_float_value(mrb, (mrb_float)in->x));
    GrappleGen_RubyHashSet(mrb, h, "y", mrb_float_value(mrb, (mrb_float)in->y));
    GrappleGen_RubyHashSet(mrb, h, "rotation", mrb_float_value(mrb, (mrb_float)in->rotation));
    GrappleGen_RubyHashSet(mrb, h, "scale_x", mrb_float_value(mrb, (mrb_float)in->scale_x));
    GrappleGen_RubyHashSet(mrb, h, "scale_y", mrb_float_value(mrb, (mrb_float)in->scale_y));
    return h;
}

static void GenRead_Grapple_Binding(mrb_state *mrb, mrb_value h, Grapple_Binding *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->source = (Grapple_BindingSource)GrappleGen_RubyFieldInt(mrb, h, "source");
    out->code = (int)GrappleGen_RubyFieldInt(mrb, h, "code");
    out->sign = (int)GrappleGen_RubyFieldInt(mrb, h, "sign");
    out->axis_half = (int)GrappleGen_RubyFieldInt(mrb, h, "axis_half");
}

static mrb_value GenPush_Grapple_Binding(mrb_state *mrb, const Grapple_Binding *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "source", mrb_int_value(mrb, (mrb_int)in->source));
    GrappleGen_RubyHashSet(mrb, h, "code", mrb_int_value(mrb, (mrb_int)in->code));
    GrappleGen_RubyHashSet(mrb, h, "sign", mrb_int_value(mrb, (mrb_int)in->sign));
    GrappleGen_RubyHashSet(mrb, h, "axis_half", mrb_int_value(mrb, (mrb_int)in->axis_half));
    return h;
}

static void GenRead_Grapple_BodyDef(mrb_state *mrb, mrb_value h, Grapple_BodyDef *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->type = (Grapple_BodyType)GrappleGen_RubyFieldInt(mrb, h, "type");
    out->shape = (Grapple_ShapeType)GrappleGen_RubyFieldInt(mrb, h, "shape");
    out->width = (float)GrappleGen_RubyFieldNum(mrb, h, "width");
    out->height = (float)GrappleGen_RubyFieldNum(mrb, h, "height");
    out->offset_x = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_x");
    out->offset_y = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_y");
    out->density = (float)GrappleGen_RubyFieldNum(mrb, h, "density");
    out->friction = (float)GrappleGen_RubyFieldNum(mrb, h, "friction");
    out->restitution = (float)GrappleGen_RubyFieldNum(mrb, h, "restitution");
    out->fixed_rotation = (bool)GrappleGen_RubyFieldBool(mrb, h, "fixed_rotation");
    out->sensor = (bool)GrappleGen_RubyFieldBool(mrb, h, "sensor");
    out->gravity_scale = (float)GrappleGen_RubyFieldNum(mrb, h, "gravity_scale");
    out->damping = (float)GrappleGen_RubyFieldNum(mrb, h, "damping");
    out->category = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "category");
    out->collides_with = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "collides_with");
    out->bullet = (bool)GrappleGen_RubyFieldBool(mrb, h, "bullet");
}

static mrb_value GenPush_Grapple_BodyDef(mrb_state *mrb, const Grapple_BodyDef *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "type", mrb_int_value(mrb, (mrb_int)in->type));
    GrappleGen_RubyHashSet(mrb, h, "shape", mrb_int_value(mrb, (mrb_int)in->shape));
    GrappleGen_RubyHashSet(mrb, h, "width", mrb_float_value(mrb, (mrb_float)in->width));
    GrappleGen_RubyHashSet(mrb, h, "height", mrb_float_value(mrb, (mrb_float)in->height));
    GrappleGen_RubyHashSet(mrb, h, "offset_x", mrb_float_value(mrb, (mrb_float)in->offset_x));
    GrappleGen_RubyHashSet(mrb, h, "offset_y", mrb_float_value(mrb, (mrb_float)in->offset_y));
    GrappleGen_RubyHashSet(mrb, h, "density", mrb_float_value(mrb, (mrb_float)in->density));
    GrappleGen_RubyHashSet(mrb, h, "friction", mrb_float_value(mrb, (mrb_float)in->friction));
    GrappleGen_RubyHashSet(mrb, h, "restitution", mrb_float_value(mrb, (mrb_float)in->restitution));
    GrappleGen_RubyHashSet(mrb, h, "fixed_rotation", mrb_bool_value((mrb_bool)(in->fixed_rotation != 0)));
    GrappleGen_RubyHashSet(mrb, h, "sensor", mrb_bool_value((mrb_bool)(in->sensor != 0)));
    GrappleGen_RubyHashSet(mrb, h, "gravity_scale", mrb_float_value(mrb, (mrb_float)in->gravity_scale));
    GrappleGen_RubyHashSet(mrb, h, "damping", mrb_float_value(mrb, (mrb_float)in->damping));
    GrappleGen_RubyHashSet(mrb, h, "category", mrb_int_value(mrb, (mrb_int)in->category));
    GrappleGen_RubyHashSet(mrb, h, "collides_with", mrb_int_value(mrb, (mrb_int)in->collides_with));
    GrappleGen_RubyHashSet(mrb, h, "bullet", mrb_bool_value((mrb_bool)(in->bullet != 0)));
    return h;
}

static void GenRead_SDL_FRect(mrb_state *mrb, mrb_value h, SDL_FRect *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->x = (float)GrappleGen_RubyFieldNum(mrb, h, "x");
    out->y = (float)GrappleGen_RubyFieldNum(mrb, h, "y");
    out->w = (float)GrappleGen_RubyFieldNum(mrb, h, "w");
    out->h = (float)GrappleGen_RubyFieldNum(mrb, h, "h");
}

static mrb_value GenPush_SDL_FRect(mrb_state *mrb, const SDL_FRect *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "x", mrb_float_value(mrb, (mrb_float)in->x));
    GrappleGen_RubyHashSet(mrb, h, "y", mrb_float_value(mrb, (mrb_float)in->y));
    GrappleGen_RubyHashSet(mrb, h, "w", mrb_float_value(mrb, (mrb_float)in->w));
    GrappleGen_RubyHashSet(mrb, h, "h", mrb_float_value(mrb, (mrb_float)in->h));
    return h;
}

static void GenRead_Grapple_Camera(mrb_state *mrb, mrb_value h, Grapple_Camera *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->x = (float)GrappleGen_RubyFieldNum(mrb, h, "x");
    out->y = (float)GrappleGen_RubyFieldNum(mrb, h, "y");
    out->zoom = (float)GrappleGen_RubyFieldNum(mrb, h, "zoom");
    out->target_x = (float)GrappleGen_RubyFieldNum(mrb, h, "target_x");
    out->target_y = (float)GrappleGen_RubyFieldNum(mrb, h, "target_y");
    out->smoothing = (float)GrappleGen_RubyFieldNum(mrb, h, "smoothing");
    out->deadzone_w = (float)GrappleGen_RubyFieldNum(mrb, h, "deadzone_w");
    out->deadzone_h = (float)GrappleGen_RubyFieldNum(mrb, h, "deadzone_h");
    GenRead_SDL_FRect(mrb, GrappleGen_RubyFieldGet(mrb, h, "bounds"), &out->bounds);
    out->shake_amount = (float)GrappleGen_RubyFieldNum(mrb, h, "shake_amount");
    out->shake_seconds = (float)GrappleGen_RubyFieldNum(mrb, h, "shake_seconds");
    out->shake_remaining = (float)GrappleGen_RubyFieldNum(mrb, h, "shake_remaining");
    out->shake_scale = (float)GrappleGen_RubyFieldNum(mrb, h, "shake_scale");
    GenRead_SDL_FRect(mrb, GrappleGen_RubyFieldGet(mrb, h, "viewport"), &out->viewport);
    GenRead_SDL_FRect(mrb, GrappleGen_RubyFieldGet(mrb, h, "visible"), &out->visible);
    out->last_target_x = (float)GrappleGen_RubyFieldNum(mrb, h, "last_target_x");
    out->last_target_y = (float)GrappleGen_RubyFieldNum(mrb, h, "last_target_y");
    out->has_last_target = (bool)GrappleGen_RubyFieldBool(mrb, h, "has_last_target");
}

static mrb_value GenPush_Grapple_Camera(mrb_state *mrb, const Grapple_Camera *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "x", mrb_float_value(mrb, (mrb_float)in->x));
    GrappleGen_RubyHashSet(mrb, h, "y", mrb_float_value(mrb, (mrb_float)in->y));
    GrappleGen_RubyHashSet(mrb, h, "zoom", mrb_float_value(mrb, (mrb_float)in->zoom));
    GrappleGen_RubyHashSet(mrb, h, "target_x", mrb_float_value(mrb, (mrb_float)in->target_x));
    GrappleGen_RubyHashSet(mrb, h, "target_y", mrb_float_value(mrb, (mrb_float)in->target_y));
    GrappleGen_RubyHashSet(mrb, h, "smoothing", mrb_float_value(mrb, (mrb_float)in->smoothing));
    GrappleGen_RubyHashSet(mrb, h, "deadzone_w", mrb_float_value(mrb, (mrb_float)in->deadzone_w));
    GrappleGen_RubyHashSet(mrb, h, "deadzone_h", mrb_float_value(mrb, (mrb_float)in->deadzone_h));
    GrappleGen_RubyHashSet(mrb, h, "bounds", GenPush_SDL_FRect(mrb, &in->bounds));
    GrappleGen_RubyHashSet(mrb, h, "shake_amount", mrb_float_value(mrb, (mrb_float)in->shake_amount));
    GrappleGen_RubyHashSet(mrb, h, "shake_seconds", mrb_float_value(mrb, (mrb_float)in->shake_seconds));
    GrappleGen_RubyHashSet(mrb, h, "shake_remaining", mrb_float_value(mrb, (mrb_float)in->shake_remaining));
    GrappleGen_RubyHashSet(mrb, h, "shake_scale", mrb_float_value(mrb, (mrb_float)in->shake_scale));
    GrappleGen_RubyHashSet(mrb, h, "viewport", GenPush_SDL_FRect(mrb, &in->viewport));
    GrappleGen_RubyHashSet(mrb, h, "visible", GenPush_SDL_FRect(mrb, &in->visible));
    GrappleGen_RubyHashSet(mrb, h, "last_target_x", mrb_float_value(mrb, (mrb_float)in->last_target_x));
    GrappleGen_RubyHashSet(mrb, h, "last_target_y", mrb_float_value(mrb, (mrb_float)in->last_target_y));
    GrappleGen_RubyHashSet(mrb, h, "has_last_target", mrb_bool_value((mrb_bool)(in->has_last_target != 0)));
    return h;
}

static mrb_value GenPush_Grapple_ChipDiagnostic(mrb_state *mrb, const Grapple_ChipDiagnostic *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "code", mrb_int_value(mrb, (mrb_int)in->code));
    GrappleGen_RubyHashSet(mrb, h, "severity", mrb_int_value(mrb, (mrb_int)in->severity));
    GrappleGen_RubyHashSet(mrb, h, "part", mrb_int_value(mrb, (mrb_int)in->part));
    GrappleGen_RubyHashSet(mrb, h, "measure", mrb_int_value(mrb, (mrb_int)in->measure));
    GrappleGen_RubyHashSet(mrb, h, "staff", mrb_int_value(mrb, (mrb_int)in->staff));
    GrappleGen_RubyHashSet(mrb, h, "line", mrb_int_value(mrb, (mrb_int)in->line));
    {
        size_t length = 0;
        while (length < sizeof(in->voice) && in->voice[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "voice", mrb_str_new(mrb, in->voice, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->element) && in->element[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "element", mrb_str_new(mrb, in->element, (mrb_int)length));
    }
    return h;
}

static void GenRead_Grapple_ChipEffects(mrb_state *mrb, mrb_value h, Grapple_ChipEffects *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->chorus = (float)GrappleGen_RubyFieldNum(mrb, h, "chorus");
    out->delay = (float)GrappleGen_RubyFieldNum(mrb, h, "delay");
    out->delay_beats = (float)GrappleGen_RubyFieldNum(mrb, h, "delay_beats");
    out->delay_feedback = (float)GrappleGen_RubyFieldNum(mrb, h, "delay_feedback");
    out->reverb = (float)GrappleGen_RubyFieldNum(mrb, h, "reverb");
    out->motion = (float)GrappleGen_RubyFieldNum(mrb, h, "motion");
    out->pulse_beats = (double)GrappleGen_RubyFieldNum(mrb, h, "pulse_beats");
    out->pulse_depth = (float)GrappleGen_RubyFieldNum(mrb, h, "pulse_depth");
    out->phaser = (float)GrappleGen_RubyFieldNum(mrb, h, "phaser");
    out->flanger = (float)GrappleGen_RubyFieldNum(mrb, h, "flanger");
}

static mrb_value GenPush_Grapple_ChipEffects(mrb_state *mrb, const Grapple_ChipEffects *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "chorus", mrb_float_value(mrb, (mrb_float)in->chorus));
    GrappleGen_RubyHashSet(mrb, h, "delay", mrb_float_value(mrb, (mrb_float)in->delay));
    GrappleGen_RubyHashSet(mrb, h, "delay_beats", mrb_float_value(mrb, (mrb_float)in->delay_beats));
    GrappleGen_RubyHashSet(mrb, h, "delay_feedback", mrb_float_value(mrb, (mrb_float)in->delay_feedback));
    GrappleGen_RubyHashSet(mrb, h, "reverb", mrb_float_value(mrb, (mrb_float)in->reverb));
    GrappleGen_RubyHashSet(mrb, h, "motion", mrb_float_value(mrb, (mrb_float)in->motion));
    GrappleGen_RubyHashSet(mrb, h, "pulse_beats", mrb_float_value(mrb, (mrb_float)in->pulse_beats));
    GrappleGen_RubyHashSet(mrb, h, "pulse_depth", mrb_float_value(mrb, (mrb_float)in->pulse_depth));
    GrappleGen_RubyHashSet(mrb, h, "phaser", mrb_float_value(mrb, (mrb_float)in->phaser));
    GrappleGen_RubyHashSet(mrb, h, "flanger", mrb_float_value(mrb, (mrb_float)in->flanger));
    return h;
}

static void GenRead_Grapple_ChipExpression(mrb_state *mrb, mrb_value h, Grapple_ChipExpression *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->tuning = (float)GrappleGen_RubyFieldNum(mrb, h, "tuning");
    out->gain = (float)GrappleGen_RubyFieldNum(mrb, h, "gain");
    out->brightness = (float)GrappleGen_RubyFieldNum(mrb, h, "brightness");
    out->noise = (float)GrappleGen_RubyFieldNum(mrb, h, "noise");
    out->bend_start = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_start");
    out->bend_peak = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_peak");
    out->bend_end = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_end");
    out->bend_first = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_first");
    out->bend_middle = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_middle");
    out->bend_last = (float)GrappleGen_RubyFieldNum(mrb, h, "bend_last");
    out->vibrato_depth = (float)GrappleGen_RubyFieldNum(mrb, h, "vibrato_depth");
    out->vibrato_beats = (float)GrappleGen_RubyFieldNum(mrb, h, "vibrato_beats");
    out->lane = (int)GrappleGen_RubyFieldInt(mrb, h, "lane");
    out->legato = (bool)GrappleGen_RubyFieldBool(mrb, h, "legato");
    out->bend_accelerate = (bool)GrappleGen_RubyFieldBool(mrb, h, "bend_accelerate");
    out->stepped_pitch = (bool)GrappleGen_RubyFieldBool(mrb, h, "stepped_pitch");
}

static mrb_value GenPush_Grapple_ChipExpression(mrb_state *mrb, const Grapple_ChipExpression *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "tuning", mrb_float_value(mrb, (mrb_float)in->tuning));
    GrappleGen_RubyHashSet(mrb, h, "gain", mrb_float_value(mrb, (mrb_float)in->gain));
    GrappleGen_RubyHashSet(mrb, h, "brightness", mrb_float_value(mrb, (mrb_float)in->brightness));
    GrappleGen_RubyHashSet(mrb, h, "noise", mrb_float_value(mrb, (mrb_float)in->noise));
    GrappleGen_RubyHashSet(mrb, h, "bend_start", mrb_float_value(mrb, (mrb_float)in->bend_start));
    GrappleGen_RubyHashSet(mrb, h, "bend_peak", mrb_float_value(mrb, (mrb_float)in->bend_peak));
    GrappleGen_RubyHashSet(mrb, h, "bend_end", mrb_float_value(mrb, (mrb_float)in->bend_end));
    GrappleGen_RubyHashSet(mrb, h, "bend_first", mrb_float_value(mrb, (mrb_float)in->bend_first));
    GrappleGen_RubyHashSet(mrb, h, "bend_middle", mrb_float_value(mrb, (mrb_float)in->bend_middle));
    GrappleGen_RubyHashSet(mrb, h, "bend_last", mrb_float_value(mrb, (mrb_float)in->bend_last));
    GrappleGen_RubyHashSet(mrb, h, "vibrato_depth", mrb_float_value(mrb, (mrb_float)in->vibrato_depth));
    GrappleGen_RubyHashSet(mrb, h, "vibrato_beats", mrb_float_value(mrb, (mrb_float)in->vibrato_beats));
    GrappleGen_RubyHashSet(mrb, h, "lane", mrb_int_value(mrb, (mrb_int)in->lane));
    GrappleGen_RubyHashSet(mrb, h, "legato", mrb_bool_value((mrb_bool)(in->legato != 0)));
    GrappleGen_RubyHashSet(mrb, h, "bend_accelerate", mrb_bool_value((mrb_bool)(in->bend_accelerate != 0)));
    GrappleGen_RubyHashSet(mrb, h, "stepped_pitch", mrb_bool_value((mrb_bool)(in->stepped_pitch != 0)));
    return h;
}

static void GenRead_Grapple_ChipImportOptions(mrb_state *mrb, mrb_value h, Grapple_ChipImportOptions *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->strict = (bool)GrappleGen_RubyFieldBool(mrb, h, "strict");
    out->staff = (int)GrappleGen_RubyFieldInt(mrb, h, "staff");
    out->staccato_gate = (double)GrappleGen_RubyFieldNum(mrb, h, "staccato_gate");
    out->staccatissimo_gate = (double)GrappleGen_RubyFieldNum(mrb, h, "staccatissimo_gate");
    out->portato_gate = (double)GrappleGen_RubyFieldNum(mrb, h, "portato_gate");
    out->breath_gate = (double)GrappleGen_RubyFieldNum(mrb, h, "breath_gate");
    out->caesura_beats = (double)GrappleGen_RubyFieldNum(mrb, h, "caesura_beats");
    out->grace_beats = (double)GrappleGen_RubyFieldNum(mrb, h, "grace_beats");
    out->ornament_beats = (double)GrappleGen_RubyFieldNum(mrb, h, "ornament_beats");
    out->arpeggio_beats = (double)GrappleGen_RubyFieldNum(mrb, h, "arpeggio_beats");
    out->fermata_factor = (double)GrappleGen_RubyFieldNum(mrb, h, "fermata_factor");
    out->swing_ratio = (double)GrappleGen_RubyFieldNum(mrb, h, "swing_ratio");
}

static mrb_value GenPush_Grapple_ChipImportOptions(mrb_state *mrb, const Grapple_ChipImportOptions *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "strict", mrb_bool_value((mrb_bool)(in->strict != 0)));
    GrappleGen_RubyHashSet(mrb, h, "staff", mrb_int_value(mrb, (mrb_int)in->staff));
    GrappleGen_RubyHashSet(mrb, h, "staccato_gate", mrb_float_value(mrb, (mrb_float)in->staccato_gate));
    GrappleGen_RubyHashSet(mrb, h, "staccatissimo_gate", mrb_float_value(mrb, (mrb_float)in->staccatissimo_gate));
    GrappleGen_RubyHashSet(mrb, h, "portato_gate", mrb_float_value(mrb, (mrb_float)in->portato_gate));
    GrappleGen_RubyHashSet(mrb, h, "breath_gate", mrb_float_value(mrb, (mrb_float)in->breath_gate));
    GrappleGen_RubyHashSet(mrb, h, "caesura_beats", mrb_float_value(mrb, (mrb_float)in->caesura_beats));
    GrappleGen_RubyHashSet(mrb, h, "grace_beats", mrb_float_value(mrb, (mrb_float)in->grace_beats));
    GrappleGen_RubyHashSet(mrb, h, "ornament_beats", mrb_float_value(mrb, (mrb_float)in->ornament_beats));
    GrappleGen_RubyHashSet(mrb, h, "arpeggio_beats", mrb_float_value(mrb, (mrb_float)in->arpeggio_beats));
    GrappleGen_RubyHashSet(mrb, h, "fermata_factor", mrb_float_value(mrb, (mrb_float)in->fermata_factor));
    GrappleGen_RubyHashSet(mrb, h, "swing_ratio", mrb_float_value(mrb, (mrb_float)in->swing_ratio));
    return h;
}

static mrb_value GenPush_Grapple_ChipMapping(mrb_state *mrb, const Grapple_ChipMapping *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "preset", mrb_int_value(mrb, (mrb_int)in->preset));
    GrappleGen_RubyHashSet(mrb, h, "reason", mrb_int_value(mrb, (mrb_int)in->reason));
    GrappleGen_RubyHashSet(mrb, h, "program", mrb_int_value(mrb, (mrb_int)in->program));
    GrappleGen_RubyHashSet(mrb, h, "channel", mrb_int_value(mrb, (mrb_int)in->channel));
    return h;
}

static void GenRead_Grapple_ChipNote(mrb_state *mrb, mrb_value h, Grapple_ChipNote *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->track = (int)GrappleGen_RubyFieldInt(mrb, h, "track");
    out->note = (int)GrappleGen_RubyFieldInt(mrb, h, "note");
    out->velocity = (int)GrappleGen_RubyFieldInt(mrb, h, "velocity");
    out->start_tick = (Uint64)GrappleGen_RubyFieldInt(mrb, h, "start_tick");
    out->duration_ticks = (Uint64)GrappleGen_RubyFieldInt(mrb, h, "duration_ticks");
}

static mrb_value GenPush_Grapple_ChipPosition(mrb_state *mrb, const Grapple_ChipPosition *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "seconds", mrb_float_value(mrb, (mrb_float)in->seconds));
    GrappleGen_RubyHashSet(mrb, h, "beat", mrb_float_value(mrb, (mrb_float)in->beat));
    GrappleGen_RubyHashSet(mrb, h, "tick", mrb_int_value(mrb, (mrb_int)in->tick));
    GrappleGen_RubyHashSet(mrb, h, "source_measure", mrb_int_value(mrb, (mrb_int)in->source_measure));
    GrappleGen_RubyHashSet(mrb, h, "measure_visit", mrb_int_value(mrb, (mrb_int)in->measure_visit));
    GrappleGen_RubyHashSet(mrb, h, "measure_beat", mrb_float_value(mrb, (mrb_float)in->measure_beat));
    GrappleGen_RubyHashSet(mrb, h, "loop_count", mrb_int_value(mrb, (mrb_int)in->loop_count));
    return h;
}

static void GenRead_Grapple_ChipRoleControls(mrb_state *mrb, mrb_value h, Grapple_ChipRoleControls *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->level = (float)GrappleGen_RubyFieldNum(mrb, h, "level");
    out->attack_ms = (float)GrappleGen_RubyFieldNum(mrb, h, "attack_ms");
    out->decay_ms = (float)GrappleGen_RubyFieldNum(mrb, h, "decay_ms");
    out->sustain = (float)GrappleGen_RubyFieldNum(mrb, h, "sustain");
    out->release_ms = (float)GrappleGen_RubyFieldNum(mrb, h, "release_ms");
    out->duty = (float)GrappleGen_RubyFieldNum(mrb, h, "duty");
    out->pwm = (float)GrappleGen_RubyFieldNum(mrb, h, "pwm");
    out->vibrato = (float)GrappleGen_RubyFieldNum(mrb, h, "vibrato");
    out->cutoff_hz = (float)GrappleGen_RubyFieldNum(mrb, h, "cutoff_hz");
    GenRead_Grapple_ChipEffects(mrb, GrappleGen_RubyFieldGet(mrb, h, "effects"), &out->effects);
}

static mrb_value GenPush_Grapple_ChipRoleControls(mrb_state *mrb, const Grapple_ChipRoleControls *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "level", mrb_float_value(mrb, (mrb_float)in->level));
    GrappleGen_RubyHashSet(mrb, h, "attack_ms", mrb_float_value(mrb, (mrb_float)in->attack_ms));
    GrappleGen_RubyHashSet(mrb, h, "decay_ms", mrb_float_value(mrb, (mrb_float)in->decay_ms));
    GrappleGen_RubyHashSet(mrb, h, "sustain", mrb_float_value(mrb, (mrb_float)in->sustain));
    GrappleGen_RubyHashSet(mrb, h, "release_ms", mrb_float_value(mrb, (mrb_float)in->release_ms));
    GrappleGen_RubyHashSet(mrb, h, "duty", mrb_float_value(mrb, (mrb_float)in->duty));
    GrappleGen_RubyHashSet(mrb, h, "pwm", mrb_float_value(mrb, (mrb_float)in->pwm));
    GrappleGen_RubyHashSet(mrb, h, "vibrato", mrb_float_value(mrb, (mrb_float)in->vibrato));
    GrappleGen_RubyHashSet(mrb, h, "cutoff_hz", mrb_float_value(mrb, (mrb_float)in->cutoff_hz));
    GrappleGen_RubyHashSet(mrb, h, "effects", GenPush_Grapple_ChipEffects(mrb, &in->effects));
    return h;
}

static mrb_value GenPush_Grapple_ChipSection(mrb_state *mrb, const Grapple_ChipSection *in)
{
    mrb_value h = mrb_hash_new(mrb);
    {
        size_t length = 0;
        while (length < sizeof(in->name) && in->name[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "name", mrb_str_new(mrb, in->name, (mrb_int)length));
    }
    GrappleGen_RubyHashSet(mrb, h, "start_tick", mrb_int_value(mrb, (mrb_int)in->start_tick));
    GrappleGen_RubyHashSet(mrb, h, "end_tick", mrb_int_value(mrb, (mrb_int)in->end_tick));
    GrappleGen_RubyHashSet(mrb, h, "source_measure", mrb_int_value(mrb, (mrb_int)in->source_measure));
    GrappleGen_RubyHashSet(mrb, h, "measure_visit", mrb_int_value(mrb, (mrb_int)in->measure_visit));
    return h;
}

static mrb_value GenPush_Grapple_ChipSongInfo(mrb_state *mrb, const Grapple_ChipSongInfo *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "track_count", mrb_int_value(mrb, (mrb_int)in->track_count));
    GrappleGen_RubyHashSet(mrb, h, "ticks_per_quarter", mrb_int_value(mrb, (mrb_int)in->ticks_per_quarter));
    GrappleGen_RubyHashSet(mrb, h, "duration_ticks", mrb_int_value(mrb, (mrb_int)in->duration_ticks));
    GrappleGen_RubyHashSet(mrb, h, "duration_seconds", mrb_float_value(mrb, (mrb_float)in->duration_seconds));
    return h;
}

static mrb_value GenPush_Grapple_ChipStyleInfo(mrb_state *mrb, const Grapple_ChipStyleInfo *in)
{
    mrb_value h = mrb_hash_new(mrb);
    {
        size_t length = 0;
        while (length < sizeof(in->id) && in->id[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "id", mrb_str_new(mrb, in->id, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->name) && in->name[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "name", mrb_str_new(mrb, in->name, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->description) && in->description[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "description", mrb_str_new(mrb, in->description, (mrb_int)length));
    }
    GrappleGen_RubyHashSet(mrb, h, "voice", mrb_int_value(mrb, (mrb_int)in->voice));
    GrappleGen_RubyHashSet(mrb, h, "polyphony", mrb_int_value(mrb, (mrb_int)in->polyphony));
    GrappleGen_RubyHashSet(mrb, h, "builtin", mrb_bool_value((mrb_bool)(in->builtin != 0)));
    GrappleGen_RubyHashSet(mrb, h, "approximation", mrb_bool_value((mrb_bool)(in->approximation != 0)));
    return h;
}

static void GenRead_Grapple_ChipToneDesc(mrb_state *mrb, mrb_value h, Grapple_ChipToneDesc *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->wave = (Grapple_ChipWave)GrappleGen_RubyFieldInt(mrb, h, "wave");
    out->freq_hz = (float)GrappleGen_RubyFieldNum(mrb, h, "freq_hz");
    out->freq_end_hz = (float)GrappleGen_RubyFieldNum(mrb, h, "freq_end_hz");
    out->duration_ms = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "duration_ms");
    out->volume = (float)GrappleGen_RubyFieldNum(mrb, h, "volume");
    out->attack_ms = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "attack_ms");
    out->release_ms = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "release_ms");
    out->vibrato_hz = (float)GrappleGen_RubyFieldNum(mrb, h, "vibrato_hz");
    out->vibrato_semitones = (float)GrappleGen_RubyFieldNum(mrb, h, "vibrato_semitones");
}

static void GenRead_Grapple_ChipTrackMix(mrb_state *mrb, mrb_value h, Grapple_ChipTrackMix *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->gain = (float)GrappleGen_RubyFieldNum(mrb, h, "gain");
    out->pan = (float)GrappleGen_RubyFieldNum(mrb, h, "pan");
    out->muted = (bool)GrappleGen_RubyFieldBool(mrb, h, "muted");
    out->solo = (bool)GrappleGen_RubyFieldBool(mrb, h, "solo");
}

static mrb_value GenPush_Grapple_ChipTrackMix(mrb_state *mrb, const Grapple_ChipTrackMix *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "gain", mrb_float_value(mrb, (mrb_float)in->gain));
    GrappleGen_RubyHashSet(mrb, h, "pan", mrb_float_value(mrb, (mrb_float)in->pan));
    GrappleGen_RubyHashSet(mrb, h, "muted", mrb_bool_value((mrb_bool)(in->muted != 0)));
    GrappleGen_RubyHashSet(mrb, h, "solo", mrb_bool_value((mrb_bool)(in->solo != 0)));
    return h;
}

static void GenRead_Grapple_GraphicsSettings(mrb_state *mrb, mrb_value h, Grapple_GraphicsSettings *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->vsync = (bool)GrappleGen_RubyFieldBool(mrb, h, "vsync");
    out->max_fps = (int)GrappleGen_RubyFieldInt(mrb, h, "max_fps");
    out->window_mode = (Grapple_WindowMode)GrappleGen_RubyFieldInt(mrb, h, "window_mode");
    out->window_width = (int)GrappleGen_RubyFieldInt(mrb, h, "window_width");
    out->window_height = (int)GrappleGen_RubyFieldInt(mrb, h, "window_height");
    out->display = (int)GrappleGen_RubyFieldInt(mrb, h, "display");
    out->presentation = (Grapple_EnginePresentation)GrappleGen_RubyFieldInt(mrb, h, "presentation");
    out->render_scale = (float)GrappleGen_RubyFieldNum(mrb, h, "render_scale");
    out->filter = (Grapple_TextureFilter)GrappleGen_RubyFieldInt(mrb, h, "filter");
    out->particles = (Grapple_GraphicsQuality)GrappleGen_RubyFieldInt(mrb, h, "particles");
    out->dynamic_lights = (Grapple_GraphicsQuality)GrappleGen_RubyFieldInt(mrb, h, "dynamic_lights");
    out->shadows = (Grapple_GraphicsQuality)GrappleGen_RubyFieldInt(mrb, h, "shadows");
    out->bloom = (float)GrappleGen_RubyFieldNum(mrb, h, "bloom");
    out->bloom_threshold = (float)GrappleGen_RubyFieldNum(mrb, h, "bloom_threshold");
    out->crt = (float)GrappleGen_RubyFieldNum(mrb, h, "crt");
    out->crt_curvature = (float)GrappleGen_RubyFieldNum(mrb, h, "crt_curvature");
    out->pixelation = (int)GrappleGen_RubyFieldInt(mrb, h, "pixelation");
    out->chromatic_aberration = (float)GrappleGen_RubyFieldNum(mrb, h, "chromatic_aberration");
    out->antialias = (Grapple_GraphicsAA)GrappleGen_RubyFieldInt(mrb, h, "antialias");
    out->brightness = (float)GrappleGen_RubyFieldNum(mrb, h, "brightness");
    out->contrast = (float)GrappleGen_RubyFieldNum(mrb, h, "contrast");
    out->saturation = (float)GrappleGen_RubyFieldNum(mrb, h, "saturation");
    out->color_blind = (Grapple_ColorBlindMode)GrappleGen_RubyFieldInt(mrb, h, "color_blind");
    out->reduced_flashing = (bool)GrappleGen_RubyFieldBool(mrb, h, "reduced_flashing");
    out->screen_shake = (float)GrappleGen_RubyFieldNum(mrb, h, "screen_shake");
    out->ui_scale = (float)GrappleGen_RubyFieldNum(mrb, h, "ui_scale");
    out->fullscreen_width = (int)GrappleGen_RubyFieldInt(mrb, h, "fullscreen_width");
    out->fullscreen_height = (int)GrappleGen_RubyFieldInt(mrb, h, "fullscreen_height");
    out->refresh_rate = (float)GrappleGen_RubyFieldNum(mrb, h, "refresh_rate");
    out->primary_display = (bool)GrappleGen_RubyFieldBool(mrb, h, "primary_display");
    out->effects_disabled = (bool)GrappleGen_RubyFieldBool(mrb, h, "effects_disabled");
}

static mrb_value GenPush_Grapple_GraphicsSettings(mrb_state *mrb, const Grapple_GraphicsSettings *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "vsync", mrb_bool_value((mrb_bool)(in->vsync != 0)));
    GrappleGen_RubyHashSet(mrb, h, "max_fps", mrb_int_value(mrb, (mrb_int)in->max_fps));
    GrappleGen_RubyHashSet(mrb, h, "window_mode", mrb_int_value(mrb, (mrb_int)in->window_mode));
    GrappleGen_RubyHashSet(mrb, h, "window_width", mrb_int_value(mrb, (mrb_int)in->window_width));
    GrappleGen_RubyHashSet(mrb, h, "window_height", mrb_int_value(mrb, (mrb_int)in->window_height));
    GrappleGen_RubyHashSet(mrb, h, "display", mrb_int_value(mrb, (mrb_int)in->display));
    GrappleGen_RubyHashSet(mrb, h, "presentation", mrb_int_value(mrb, (mrb_int)in->presentation));
    GrappleGen_RubyHashSet(mrb, h, "render_scale", mrb_float_value(mrb, (mrb_float)in->render_scale));
    GrappleGen_RubyHashSet(mrb, h, "filter", mrb_int_value(mrb, (mrb_int)in->filter));
    GrappleGen_RubyHashSet(mrb, h, "particles", mrb_int_value(mrb, (mrb_int)in->particles));
    GrappleGen_RubyHashSet(mrb, h, "dynamic_lights", mrb_int_value(mrb, (mrb_int)in->dynamic_lights));
    GrappleGen_RubyHashSet(mrb, h, "shadows", mrb_int_value(mrb, (mrb_int)in->shadows));
    GrappleGen_RubyHashSet(mrb, h, "bloom", mrb_float_value(mrb, (mrb_float)in->bloom));
    GrappleGen_RubyHashSet(mrb, h, "bloom_threshold", mrb_float_value(mrb, (mrb_float)in->bloom_threshold));
    GrappleGen_RubyHashSet(mrb, h, "crt", mrb_float_value(mrb, (mrb_float)in->crt));
    GrappleGen_RubyHashSet(mrb, h, "crt_curvature", mrb_float_value(mrb, (mrb_float)in->crt_curvature));
    GrappleGen_RubyHashSet(mrb, h, "pixelation", mrb_int_value(mrb, (mrb_int)in->pixelation));
    GrappleGen_RubyHashSet(mrb, h, "chromatic_aberration", mrb_float_value(mrb, (mrb_float)in->chromatic_aberration));
    GrappleGen_RubyHashSet(mrb, h, "antialias", mrb_int_value(mrb, (mrb_int)in->antialias));
    GrappleGen_RubyHashSet(mrb, h, "brightness", mrb_float_value(mrb, (mrb_float)in->brightness));
    GrappleGen_RubyHashSet(mrb, h, "contrast", mrb_float_value(mrb, (mrb_float)in->contrast));
    GrappleGen_RubyHashSet(mrb, h, "saturation", mrb_float_value(mrb, (mrb_float)in->saturation));
    GrappleGen_RubyHashSet(mrb, h, "color_blind", mrb_int_value(mrb, (mrb_int)in->color_blind));
    GrappleGen_RubyHashSet(mrb, h, "reduced_flashing", mrb_bool_value((mrb_bool)(in->reduced_flashing != 0)));
    GrappleGen_RubyHashSet(mrb, h, "screen_shake", mrb_float_value(mrb, (mrb_float)in->screen_shake));
    GrappleGen_RubyHashSet(mrb, h, "ui_scale", mrb_float_value(mrb, (mrb_float)in->ui_scale));
    GrappleGen_RubyHashSet(mrb, h, "fullscreen_width", mrb_int_value(mrb, (mrb_int)in->fullscreen_width));
    GrappleGen_RubyHashSet(mrb, h, "fullscreen_height", mrb_int_value(mrb, (mrb_int)in->fullscreen_height));
    GrappleGen_RubyHashSet(mrb, h, "refresh_rate", mrb_float_value(mrb, (mrb_float)in->refresh_rate));
    GrappleGen_RubyHashSet(mrb, h, "primary_display", mrb_bool_value((mrb_bool)(in->primary_display != 0)));
    GrappleGen_RubyHashSet(mrb, h, "effects_disabled", mrb_bool_value((mrb_bool)(in->effects_disabled != 0)));
    return h;
}

static void GenRead_SDL_FColor(mrb_state *mrb, mrb_value h, SDL_FColor *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->r = (float)GrappleGen_RubyFieldNum(mrb, h, "r");
    out->g = (float)GrappleGen_RubyFieldNum(mrb, h, "g");
    out->b = (float)GrappleGen_RubyFieldNum(mrb, h, "b");
    out->a = (float)GrappleGen_RubyFieldNum(mrb, h, "a");
}

static mrb_value GenPush_SDL_FColor(mrb_state *mrb, const SDL_FColor *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "r", mrb_float_value(mrb, (mrb_float)in->r));
    GrappleGen_RubyHashSet(mrb, h, "g", mrb_float_value(mrb, (mrb_float)in->g));
    GrappleGen_RubyHashSet(mrb, h, "b", mrb_float_value(mrb, (mrb_float)in->b));
    GrappleGen_RubyHashSet(mrb, h, "a", mrb_float_value(mrb, (mrb_float)in->a));
    return h;
}

static void GenRead_Grapple_Light(mrb_state *mrb, mrb_value h, Grapple_Light *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->x = (float)GrappleGen_RubyFieldNum(mrb, h, "x");
    out->y = (float)GrappleGen_RubyFieldNum(mrb, h, "y");
    out->radius = (float)GrappleGen_RubyFieldNum(mrb, h, "radius");
    GenRead_SDL_FColor(mrb, GrappleGen_RubyFieldGet(mrb, h, "color"), &out->color);
    out->falloff = (float)GrappleGen_RubyFieldNum(mrb, h, "falloff");
    out->angle = (float)GrappleGen_RubyFieldNum(mrb, h, "angle");
    out->spread = (float)GrappleGen_RubyFieldNum(mrb, h, "spread");
    out->flicker = (float)GrappleGen_RubyFieldNum(mrb, h, "flicker");
    out->seed = (Uint32)GrappleGen_RubyFieldInt(mrb, h, "seed");
    out->no_shadows = (bool)GrappleGen_RubyFieldBool(mrb, h, "no_shadows");
}

static void GenRead_Grapple_LightDef(mrb_state *mrb, mrb_value h, Grapple_LightDef *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->radius = (float)GrappleGen_RubyFieldNum(mrb, h, "radius");
    GenRead_SDL_FColor(mrb, GrappleGen_RubyFieldGet(mrb, h, "color"), &out->color);
    out->offset_x = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_x");
    out->offset_y = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_y");
    out->falloff = (float)GrappleGen_RubyFieldNum(mrb, h, "falloff");
    out->cone_direction = (float)GrappleGen_RubyFieldNum(mrb, h, "cone_direction");
    out->cone_width = (float)GrappleGen_RubyFieldNum(mrb, h, "cone_width");
    out->flicker = (float)GrappleGen_RubyFieldNum(mrb, h, "flicker");
    out->no_shadows = (bool)GrappleGen_RubyFieldBool(mrb, h, "no_shadows");
    out->enabled = (bool)GrappleGen_RubyFieldBool(mrb, h, "enabled");
}

static mrb_value GenPush_Grapple_LightDef(mrb_state *mrb, const Grapple_LightDef *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "radius", mrb_float_value(mrb, (mrb_float)in->radius));
    GrappleGen_RubyHashSet(mrb, h, "color", GenPush_SDL_FColor(mrb, &in->color));
    GrappleGen_RubyHashSet(mrb, h, "offset_x", mrb_float_value(mrb, (mrb_float)in->offset_x));
    GrappleGen_RubyHashSet(mrb, h, "offset_y", mrb_float_value(mrb, (mrb_float)in->offset_y));
    GrappleGen_RubyHashSet(mrb, h, "falloff", mrb_float_value(mrb, (mrb_float)in->falloff));
    GrappleGen_RubyHashSet(mrb, h, "cone_direction", mrb_float_value(mrb, (mrb_float)in->cone_direction));
    GrappleGen_RubyHashSet(mrb, h, "cone_width", mrb_float_value(mrb, (mrb_float)in->cone_width));
    GrappleGen_RubyHashSet(mrb, h, "flicker", mrb_float_value(mrb, (mrb_float)in->flicker));
    GrappleGen_RubyHashSet(mrb, h, "no_shadows", mrb_bool_value((mrb_bool)(in->no_shadows != 0)));
    GrappleGen_RubyHashSet(mrb, h, "enabled", mrb_bool_value((mrb_bool)(in->enabled != 0)));
    return h;
}

static void GenRead_Grapple_PhysicsDrawConfig(mrb_state *mrb, mrb_value h, Grapple_PhysicsDrawConfig *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->pixels_per_meter = (float)GrappleGen_RubyFieldNum(mrb, h, "pixels_per_meter");
    out->offset_x = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_x");
    out->offset_y = (float)GrappleGen_RubyFieldNum(mrb, h, "offset_y");
    out->draw_joints = (bool)GrappleGen_RubyFieldBool(mrb, h, "draw_joints");
}

static mrb_value GenPush_Grapple_RayHit(mrb_state *mrb, const Grapple_RayHit *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "hit", mrb_bool_value((mrb_bool)(in->hit != 0)));
    GrappleGen_RubyHashSet(mrb, h, "actor", mrb_int_value(mrb, (mrb_int)in->actor));
    GrappleGen_RubyHashSet(mrb, h, "x", mrb_float_value(mrb, (mrb_float)in->x));
    GrappleGen_RubyHashSet(mrb, h, "y", mrb_float_value(mrb, (mrb_float)in->y));
    GrappleGen_RubyHashSet(mrb, h, "normal_x", mrb_float_value(mrb, (mrb_float)in->normal_x));
    GrappleGen_RubyHashSet(mrb, h, "normal_y", mrb_float_value(mrb, (mrb_float)in->normal_y));
    GrappleGen_RubyHashSet(mrb, h, "fraction", mrb_float_value(mrb, (mrb_float)in->fraction));
    return h;
}

static mrb_value GenPush_Grapple_RenderBackendInfo(mrb_state *mrb, const Grapple_RenderBackendInfo *in)
{
    mrb_value h = mrb_hash_new(mrb);
    {
        size_t length = 0;
        while (length < sizeof(in->name) && in->name[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "name", mrb_str_new(mrb, in->name, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->label) && in->label[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "label", mrb_str_new(mrb, in->label, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->renderer) && in->renderer[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "renderer", mrb_str_new(mrb, in->renderer, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->api_version) && in->api_version[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "api_version", mrb_str_new(mrb, in->api_version, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->device) && in->device[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "device", mrb_str_new(mrb, in->device, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->driver) && in->driver[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "driver", mrb_str_new(mrb, in->driver, (mrb_int)length));
    }
    {
        size_t length = 0;
        while (length < sizeof(in->error) && in->error[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "error", mrb_str_new(mrb, in->error, (mrb_int)length));
    }
    GrappleGen_RubyHashSet(mrb, h, "compiled", mrb_bool_value((mrb_bool)(in->compiled != 0)));
    GrappleGen_RubyHashSet(mrb, h, "available", mrb_bool_value((mrb_bool)(in->available != 0)));
    GrappleGen_RubyHashSet(mrb, h, "opengl_effects", mrb_bool_value((mrb_bool)(in->opengl_effects != 0)));
    return h;
}

static mrb_value GenPush_Grapple_RenderStats(mrb_state *mrb, const Grapple_RenderStats *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "considered", mrb_int_value(mrb, (mrb_int)in->considered));
    GrappleGen_RubyHashSet(mrb, h, "culled", mrb_int_value(mrb, (mrb_int)in->culled));
    GrappleGen_RubyHashSet(mrb, h, "drawn", mrb_int_value(mrb, (mrb_int)in->drawn));
    return h;
}

static mrb_value GenPush_Grapple_SaveInfo(mrb_state *mrb, const Grapple_SaveInfo *in)
{
    mrb_value h = mrb_hash_new(mrb);
    GrappleGen_RubyHashSet(mrb, h, "exists", mrb_bool_value((mrb_bool)(in->exists != 0)));
    GrappleGen_RubyHashSet(mrb, h, "size", mrb_int_value(mrb, (mrb_int)in->size));
    GrappleGen_RubyHashSet(mrb, h, "modified", mrb_int_value(mrb, (mrb_int)in->modified));
    {
        size_t length = 0;
        while (length < sizeof(in->label) && in->label[length]) ++length;
        GrappleGen_RubyHashSet(mrb, h, "label", mrb_str_new(mrb, in->label, (mrb_int)length));
    }
    return h;
}

static void GenRead_SDL_AudioSpec(mrb_state *mrb, mrb_value h, SDL_AudioSpec *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->format = (SDL_AudioFormat)GrappleGen_RubyFieldInt(mrb, h, "format");
    out->channels = (int)GrappleGen_RubyFieldInt(mrb, h, "channels");
    out->freq = (int)GrappleGen_RubyFieldInt(mrb, h, "freq");
}

static void GenRead_SDL_Color(mrb_state *mrb, mrb_value h, SDL_Color *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->r = (Uint8)GrappleGen_RubyFieldInt(mrb, h, "r");
    out->g = (Uint8)GrappleGen_RubyFieldInt(mrb, h, "g");
    out->b = (Uint8)GrappleGen_RubyFieldInt(mrb, h, "b");
    out->a = (Uint8)GrappleGen_RubyFieldInt(mrb, h, "a");
}

static void GenRead_b2BodyId(mrb_state *mrb, mrb_value h, b2BodyId *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->index1 = (int32_t)GrappleGen_RubyFieldInt(mrb, h, "index1");
    out->world0 = (uint16_t)GrappleGen_RubyFieldInt(mrb, h, "world0");
    out->generation = (uint16_t)GrappleGen_RubyFieldInt(mrb, h, "generation");
}

static void GenRead_b2WorldId(mrb_state *mrb, mrb_value h, b2WorldId *out)
{
    memset(out, 0, sizeof(*out));
    if (!mrb_hash_p(h)) { return; }
    out->index1 = (uint16_t)GrappleGen_RubyFieldInt(mrb, h, "index1");
    out->generation = (uint16_t)GrappleGen_RubyFieldInt(mrb, h, "generation");
}

static void GenDtor_Grapple_DestroyChipComposer(void *p)
{
    Grapple_ChipComposer *typed = (Grapple_ChipComposer *)p;
    Grapple_DestroyChipComposer(typed);
}

static void GenDtor_Grapple_DestroyChipSong(void *p)
{
    Grapple_ChipSong *typed = (Grapple_ChipSong *)p;
    Grapple_DestroyChipSong(typed);
}

static void GenDtor_Grapple_DestroyChipPlayer(void *p)
{
    Grapple_ChipPlayer *typed = (Grapple_ChipPlayer *)p;
    Grapple_DestroyChipPlayer(typed);
}

static void GenDtor_Grapple_DestroyChipStyle(void *p)
{
    Grapple_ChipStyle *typed = (Grapple_ChipStyle *)p;
    Grapple_DestroyChipStyle(typed);
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

static void GenDtor_Grapple_DestroyPlatformer(void *p)
{
    Grapple_Platformer *typed = (Grapple_Platformer *)p;
    Grapple_DestroyPlatformer(typed);
}

static mrb_value GenR_Grapple_ActionBind(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Binding a2;
    GenRead_Grapple_Binding(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    bool rv = Grapple_ActionBind(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindAxis(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadAxis a2 = (Grapple_GamepadAxis)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ActionBindAxis(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindDirection(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Direction a2 = (Grapple_Direction)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionBindDirection(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindKey(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Scancode a2 = (SDL_Scancode)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionBindKey(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindKeySigned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Scancode a2 = (SDL_Scancode)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ActionBindKeySigned(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindMouse(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_MouseButton a2 = (Grapple_MouseButton)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionBindMouse(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindPad(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionBindPad(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionBindingAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_Binding out3;
    memset(&out3, 0, sizeof(out3));
    bool rv = Grapple_ActionBindingAt(a0, a1, a2, &out3);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_Binding(mrb, &out3);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActionBindingCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_ActionBindingCount(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActionCapture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Binding out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ActionCapture(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_Binding(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActionClear(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActionClear(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActionCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    int rv = Grapple_ActionCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActionDown(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActionMap");
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ActionDown(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionMapCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap * rv = Grapple_ActionMapCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_ActionMap");
    }
}

static mrb_value GenR_Grapple_ActionMapDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    Grapple_ActionMapDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActionMapKeyboardPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    int rv = Grapple_ActionMapKeyboardPlayer(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActionMapLoad(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionMapLoad(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionMapLoadToml(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ActionMapLoadToml(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionMapSave(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ActionMapSave(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionMapSetKeyboardPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActionMapSetKeyboardPlayer(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActionMapToToml(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    char * rv = Grapple_ActionMapToToml(a0);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_ActionName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActionMap *a0 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActionMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_ActionName(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_ActionPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActionMap");
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ActionPressed(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionReleased(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActionMap");
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ActionReleased(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActionValue(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActionMap");
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float rv = Grapple_ActionValue(a0, a1, a2, a3);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_ActionVector(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActionMap *a1 = (Grapple_ActionMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActionMap");
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    const char *a4 = GrappleGen_RubyToStr(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    float io5 = (float)GrappleGen_RubyToNum(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    float io6 = (float)GrappleGen_RubyToNum(mrb, (argc > 6 ? argv[6] : mrb_nil_value()));
    Grapple_ActionVector(a0, a1, a2, a3, a4, &io5, &io6);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io5);
    rets[1] = mrb_float_value(mrb, (mrb_float)io6);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActorAddBody(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_BodyDef tmp1;
    const Grapple_BodyDef *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_BodyDef(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_ActorAddBody(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorAddLight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_LightDef tmp1;
    const Grapple_LightDef *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_LightDef(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_ActorAddLight(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorAlive(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ActorAlive(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorAngularVelocity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float rv = Grapple_ActorAngularVelocity(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_ActorApplyForce(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorApplyForce(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorApplyImpulse(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorApplyImpulse(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorBodyBounds(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    SDL_FRect out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ActorBodyBounds(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_SDL_FRect(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActorBroadcast(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorTags a2 = (Grapple_ActorTags)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorMessage tmp3;
    const Grapple_ActorMessage *a3 = NULL;
    if (argc > 3 && mrb_hash_p(argv[3])) {
        GenRead_Grapple_ActorMessage(mrb, argv[3], &tmp3);
        a3 = &tmp3;
    }
    int rv = Grapple_ActorBroadcast(a0, a1, a2, a3);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorChild(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorId rv = Grapple_ActorChild(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorChildCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    int rv = Grapple_ActorChildCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorClear(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActorClear(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorClearSprite(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorClearSprite(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_ActorCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef * rv = Grapple_ActorDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_ActorDef");
    }
}

static mrb_value GenR_Grapple_ActorDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    Grapple_ActorDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetName(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetParent(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetParent(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetPosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorDefSetPosition(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetRotation(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetRotation(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorDefSetScale(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetStateSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetStateSize(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetTags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    Grapple_ActorTags a1 = (Grapple_ActorTags)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetTags(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDefSetType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ActorDef *a0 = (Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ActorDef");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDefSetType(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorDestroy(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorEnabled(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    bool rv = Grapple_ActorEnabled(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorEngine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_Engine * rv = Grapple_ActorEngine(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Engine");
    }
}

static mrb_value GenR_Grapple_ActorFindByName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorId rv = Grapple_ActorFindByName(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorFindByType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorId rv = Grapple_ActorFindByType(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorGet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Actor * rv = Grapple_ActorGet(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Actor");
    }
}

static mrb_value GenR_Grapple_ActorGetId(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorId rv = Grapple_ActorGetId(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorGetTags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTags rv = Grapple_ActorGetTags(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorHasBody(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    bool rv = Grapple_ActorHasBody(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorHasTags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTags a1 = (Grapple_ActorTags)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ActorHasTags(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorLocal(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTransform rv = Grapple_ActorLocal(a0);
    return GenPush_Grapple_ActorTransform(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_ActorMove(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorMove(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    const char * rv = Grapple_ActorName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_ActorParent(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorId rv = Grapple_ActorParent(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorQuery(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorTags a2 = (Grapple_ActorTags)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorId io3 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    int rv = Grapple_ActorQuery(a0, a1, a2, &io3, a4);
    mrb_value rets[2];
    rets[0] = mrb_int_value(mrb, (mrb_int)rv);
    rets[1] = mrb_int_value(mrb, (mrb_int)io3);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActorRemoveBody(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorRemoveBody(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorRemoveLight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorRemoveLight(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorRenderTransform(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorTransform rv = Grapple_ActorRenderTransform(a0, a1);
    return GenPush_Grapple_ActorTransform(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_ActorSend(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorMessage tmp2;
    const Grapple_ActorMessage *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ActorMessage(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_ActorSend(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorSetAngularVelocity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorSetAngularVelocity(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSetEnabled(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorSetEnabled(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSetLocal(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTransform tmp1;
    const Grapple_ActorTransform *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_ActorTransform(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    Grapple_ActorSetLocal(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSetParent(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorId a1 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ActorSetParent(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorSetPosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorSetPosition(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSetSprite(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    const Grapple_Sprite *a1 = (const Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_Sprite");
    bool rv = Grapple_ActorSetSprite(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ActorSetTags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTags a1 = (Grapple_ActorTags)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ActorSetTags(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSetVelocity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorSetVelocity(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorSpawn(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_ActorDef *a1 = (const Grapple_ActorDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ActorDef");
    Grapple_ActorId rv = Grapple_ActorSpawn(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ActorSprite(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_Sprite * rv = Grapple_ActorSprite(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Sprite");
    }
}

static mrb_value GenR_Grapple_ActorTeleport(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorTeleport(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorTeleportBody(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorTeleportBody(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    const char * rv = Grapple_ActorType(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_ActorVelocity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorVelocity(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ActorWakeBody(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorWakeBody(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ActorWorld(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Actor *a0 = (Grapple_Actor *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Actor");
    Grapple_ActorTransform rv = Grapple_ActorWorld(a0);
    return GenPush_Grapple_ActorTransform(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_AddChipControl(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint64 a2 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_AddChipControl(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddChipNote(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    Grapple_ChipNote tmp1;
    const Grapple_ChipNote *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_ChipNote(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_AddChipNote(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddChipNoteEx(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    Grapple_ChipNote tmp1;
    const Grapple_ChipNote *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_ChipNote(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    Grapple_ChipExpression tmp2;
    const Grapple_ChipExpression *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ChipExpression(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_AddChipNoteEx(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddChipSection(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint64 a2 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint64 a3 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_AddChipSection(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddChipStyleSearchPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool rv = Grapple_AddChipStyleSearchPath(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddChipTempo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    double a2 = (double)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_AddChipTempo(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddDarkZone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    SDL_FColor a2;
    GenRead_SDL_FColor(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    bool rv = Grapple_AddDarkZone(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddLight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    Grapple_Light tmp1;
    const Grapple_Light *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_Light(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_AddLight(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddOccluderRect(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    bool rv = Grapple_AddOccluderRect(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AddOccluderSegment(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_AddOccluderSegment(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AnyInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_AnyInput(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ApplyAudioBus(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_AudioBus a0 = (Grapple_AudioBus)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ApplyAudioBus(a0, &io1, a2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = mrb_float_value(mrb, (mrb_float)io1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_AssetPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_AssetPath(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_AssetRelease(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_AssetRelease(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_AssetRetain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_AssetRetain(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_AssetStatusOf(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_AssetStatus rv = Grapple_AssetStatusOf(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_AssetsFrameBudget(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_AssetsFrameBudget(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_AssetsLoaded(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_AssetsLoaded(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_AssetsPending(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_AssetsPending(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_AssetsProgress(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_AssetsProgress(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_AssetsReady(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_AssetsReady(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AssetsSetFrameBudget(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_AssetsSetFrameBudget(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_AssetsSetWorkers(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_AssetsSetWorkers(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_AssetsWait(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_AssetsWait(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_AttachAudioBuses(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "MIX_Mixer");
    bool rv = Grapple_AttachAudioBuses(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_AudioMuted(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    bool rv = Grapple_AudioMuted();
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_BidiBaseIsRTL(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_BidiBaseIsRTL(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_BindingFromString(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_Binding out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_BindingFromString(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_Binding(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_BindingToString(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Binding a0;
    GenRead_Grapple_Binding(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &a0);
    const char *src1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    char *a1 = SDL_strdup(src1 != NULL ? src1 : "");
    size_t a2 = (size_t)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char * rv = Grapple_BindingToString(a0, a1, a2);
    SDL_free(a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_BodyDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_BodyDefDestroy(&out0);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetBullet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetBullet(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetFilter(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_BodyDefSetFilter(&out0, a1, a2);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetFixedRotation(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetFixedRotation(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetGravityScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetGravityScale(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetMaterial(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_BodyDefSetMaterial(&out0, a1, a2, a3);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetOffset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_BodyDefSetOffset(&out0, a1, a2);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetSensor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetSensor(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetShape(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_ShapeType a1 = (Grapple_ShapeType)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetShape(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_BodyDefSetSize(&out0, a1, a2);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefSetType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_BodyType a1 = (Grapple_BodyType)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_BodyDefSetType(&out0, a1);
    return GenPush_Grapple_BodyDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_BodyDefault(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_BodyDef rv = Grapple_BodyDefault();
    return GenPush_Grapple_BodyDef(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_BuildChipSong(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipComposer *a0 = (const Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipSong * rv = Grapple_BuildChipSong(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipSong", GenDtor_Grapple_DestroyChipSong);
    }
}

static mrb_value GenR_Grapple_CameraBegin(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_Camera(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_CameraBegin(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_CameraDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_CameraDestroy(&out0);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraEnd(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_CameraEnd(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_CameraFollow(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_CameraFollow(&out0, a1, a2);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraInit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_Engine *a1 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_CameraInit(&out0, a1);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraPoint(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_Camera(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_CameraPoint(a0, a1, a2, &io3, &io4);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io3);
    rets[1] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_CameraRect(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_Camera(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    SDL_FRect rv = Grapple_CameraRect(a0, a1);
    return GenPush_SDL_FRect(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_CameraScreenToWorld(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_Camera(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_CameraScreenToWorld(a0, a1, a2, &io3, &io4);
    mrb_value rets[3];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = mrb_float_value(mrb, (mrb_float)io3);
    rets[2] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 3, rets);
    }
}

static mrb_value GenR_Grapple_CameraSetBounds(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_CameraSetBounds(&out0, a1, a2, a3, a4);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSetDeadzone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_CameraSetDeadzone(&out0, a1, a2);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSetSmoothing(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_CameraSetSmoothing(&out0, a1);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSetViewport(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_CameraSetViewport(&out0, a1, a2, a3, a4);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSetZoom(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_CameraSetZoom(&out0, a1);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraShake(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_CameraShake(&out0, a1, a2);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSnap(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_CameraSnap(&out0, a1, a2);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraSplit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_SplitMode a1 = (Grapple_SplitMode)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_Camera out4;
    memset(&out4, 0, sizeof(out4));
    int rv = Grapple_CameraSplit(a0, a1, a2, a3, &out4);
    mrb_value rets[2];
    rets[0] = mrb_int_value(mrb, (mrb_int)rv);
    rets[1] = GenPush_Grapple_Camera(mrb, &out4);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_CameraUpdate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_Engine *a1 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_CameraUpdate(&out0, a1, a2);
    return GenPush_Grapple_Camera(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_CameraVisible(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera tmp0;
    const Grapple_Camera *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_Camera(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    bool rv = Grapple_CameraVisible(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_CameraX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float rv = Grapple_CameraX(&out0);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)rv);
    rets[1] = GenPush_Grapple_Camera(mrb, &out0);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_CameraY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Camera out0;
    memset(&out0, 0, sizeof(out0));
    float rv = Grapple_CameraY(&out0);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)rv);
    rets[1] = GenPush_Grapple_Camera(mrb, &out0);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ChipPlayerPlaying(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    bool rv = Grapple_ChipPlayerPlaying(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_CloneSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    Grapple_Settings * rv = Grapple_CloneSettings(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_CompileRegex(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Regex * rv = Grapple_CompileRegex(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_Regex", GenDtor_Grapple_DestroyRegex);
    }
}

static mrb_value GenR_Grapple_ConfigCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig * rv = Grapple_ConfigCreate();
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_EngineConfig", GenDtor_Grapple_ConfigDestroy);
    }
}

static mrb_value GenR_Grapple_ConfigDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_ConfigDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetAutoMount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetAutoMount(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetBackend(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_EngineBackend a1 = (Grapple_EngineBackend)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetBackend(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetDesignSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ConfigSetDesignSize(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetFullscreen(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetFullscreen(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetGraphics(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    Grapple_ConfigSetGraphics(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetHeadless(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetHeadless(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetHighDpi(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetHighDpi(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetManualClock(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetManualClock(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetMaxFps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetMaxFps(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetMediaPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetMediaPath(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetPresentation(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_EnginePresentation a1 = (Grapple_EnginePresentation)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetPresentation(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetRendererBackend(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ConfigSetRendererBackend(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ConfigSetResizable(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetResizable(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetTickRate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetTickRate(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetTitle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetTitle(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetVsync(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ConfigSetVsync(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ConfigSetWindowSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_EngineConfig *a0 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ConfigSetWindowSize(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_CountSignalConnections(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_SignalEmitter");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_CountSignalConnections(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_CreateAudioMixer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_AudioDeviceID a0 = (SDL_AudioDeviceID)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    SDL_AudioSpec tmp1;
    const SDL_AudioSpec *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_SDL_AudioSpec(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    MIX_Mixer * rv = Grapple_CreateAudioMixer(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "MIX_Mixer");
    }
}

static mrb_value GenR_Grapple_CreateBackendRenderer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Window *a0 = (SDL_Window *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Window");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Renderer * rv = Grapple_CreateBackendRenderer(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Renderer");
    }
}

static mrb_value GenR_Grapple_CreateChipComposer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipComposer * rv = Grapple_CreateChipComposer(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipComposer", GenDtor_Grapple_DestroyChipComposer);
    }
}

static mrb_value GenR_Grapple_CreateChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool a3 = (bool)GrappleGen_RubyToBool((argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_ChipPlayer * rv = Grapple_CreateChipPlayer(a0, a1, a2, a3);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipPlayer", GenDtor_Grapple_DestroyChipPlayer);
    }
}

static mrb_value GenR_Grapple_CreateChipSFX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "MIX_Mixer");
    Grapple_ChipSFX a1 = (Grapple_ChipSFX)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    MIX_Audio * rv = Grapple_CreateChipSFX(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "MIX_Audio");
    }
}

static mrb_value GenR_Grapple_CreateChipTone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "MIX_Mixer");
    Grapple_ChipToneDesc tmp1;
    const Grapple_ChipToneDesc *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_ChipToneDesc(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    MIX_Audio * rv = Grapple_CreateChipTone(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "MIX_Audio");
    }
}

static mrb_value GenR_Grapple_CreateChipTune(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    MIX_Mixer *a0 = (MIX_Mixer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "MIX_Mixer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    MIX_Audio * rv = Grapple_CreateChipTune(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "MIX_Audio");
    }
}

static mrb_value GenR_Grapple_CreateEngine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_EngineConfig *a0 = (const Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_Engine * rv = Grapple_CreateEngine(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Engine");
    }
}

static mrb_value GenR_Grapple_CreateGui(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Renderer");
    size_t len1 = 0;
    const char *a1 = GrappleGen_RubyToBlob(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &len1);
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_Gui * rv = Grapple_CreateGui(a0, (const void *)a1, (size_t)len1, a3);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_Gui", GenDtor_Grapple_DestroyGui);
    }
}

static mrb_value GenR_Grapple_CreateGuiWithGlyphs(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Renderer");
    size_t len1 = 0;
    const char *a1 = GrappleGen_RubyToBlob(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &len1);
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GuiGlyphRange a4 = (Grapple_GuiGlyphRange)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_Gui * rv = Grapple_CreateGuiWithGlyphs(a0, (const void *)a1, (size_t)len1, a3, a4);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Gui");
    }
}

static mrb_value GenR_Grapple_CreateLightScene(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Renderer");
    Grapple_LightScene * rv = Grapple_CreateLightScene(a0);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_LightScene", GenDtor_Grapple_DestroyLightScene);
    }
}

static mrb_value GenR_Grapple_CreatePlatformer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_Platformer * rv = Grapple_CreatePlatformer(a0, a1, a2, a3);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_Platformer", GenDtor_Grapple_DestroyPlatformer);
    }
}

static mrb_value GenR_Grapple_CreateSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings * rv = Grapple_CreateSettings();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_CreateSignalEmitter(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_SignalEmitter * rv = Grapple_CreateSignalEmitter();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_SignalEmitter");
    }
}

static mrb_value GenR_Grapple_CryptoSelfTest(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    bool rv = Grapple_CryptoSelfTest();
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_DayNightAmbient(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    float a0 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    SDL_FColor rv = Grapple_DayNightAmbient(a0);
    return GenPush_SDL_FColor(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_DayNightSunlight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    float a0 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float rv = Grapple_DayNightSunlight(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_DescribeRenderBackend(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Renderer");
    Grapple_RenderBackendInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_DescribeRenderBackend(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_RenderBackendInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_DestroyChipComposer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    Grapple_DestroyChipComposer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_DestroyChipPlayer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyChipSong(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipSong *a0 = (Grapple_ChipSong *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    Grapple_DestroyChipSong(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyChipStyle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipStyle *a0 = (Grapple_ChipStyle *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipStyle");
    Grapple_DestroyChipStyle(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyEngine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_DestroyEngine(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyGui(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_DestroyGui(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyLightScene(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    Grapple_DestroyLightScene(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyPlatformer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_DestroyPlatformer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroyRegex(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    Grapple_DestroyRegex(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroySettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    Grapple_DestroySettings(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DestroySignalEmitter(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_SignalEmitter");
    Grapple_DestroySignalEmitter(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DeviceAccelerometer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_DeviceAccelerometer(a0, &io1, &io2, &io3);
    mrb_value rets[3];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    rets[2] = mrb_float_value(mrb, (mrb_float)io3);
    return mrb_ary_new_from_values(mrb, 3, rets);
    }
}

static mrb_value GenR_Grapple_DeviceGyro(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_DeviceGyro(a0, &io1, &io2, &io3);
    mrb_value rets[3];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    rets[2] = mrb_float_value(mrb, (mrb_float)io3);
    return mrb_ary_new_from_values(mrb, 3, rets);
    }
}

static mrb_value GenR_Grapple_DialogDeliverSave(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool rv = Grapple_DialogDeliverSave(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_DialogPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char * rv = Grapple_DialogPath();
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_DialogReset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_DialogReset();
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DialogStatus(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_DialogState rv = Grapple_DialogStatus();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_DisconnectSignal(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_SignalEmitter *a0 = (Grapple_SignalEmitter *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_SignalEmitter");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_DisconnectSignal(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_DistanceJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef * rv = Grapple_DistanceJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2DistanceJointDef");
    }
}

static mrb_value GenR_Grapple_DistanceJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    Grapple_DistanceJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetAnchors(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_DistanceJointDefSetAnchors(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_DistanceJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetCollideConnected(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_DistanceJointDefSetCollideConnected(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetLength(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_DistanceJointDefSetLength(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetLimit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_DistanceJointDefSetLimit(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DistanceJointDefSetSpring(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2DistanceJointDef *a0 = (b2DistanceJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2DistanceJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_DistanceJointDefSetSpring(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_DrawPhysicsWorld(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WorldId a0;
    GenRead_b2WorldId(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &a0);
    SDL_Renderer *a1 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Renderer");
    Grapple_PhysicsDrawConfig tmp2;
    const Grapple_PhysicsDrawConfig *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_PhysicsDrawConfig(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_DrawPhysicsWorld(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EncodeDataBase64(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    size_t len0 = 0;
    const char *a0 = GrappleGen_RubyToBlob(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &len0);
    int io2 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    char * rv = Grapple_EncodeDataBase64((const void *)a0, (int)len0, &io2);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    mrb_value rets[2];
    rets[0] = rstr;
    rets[1] = mrb_int_value(mrb, (mrb_int)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_EngineActualSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_Settings * rv = Grapple_EngineActualSettings(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_EngineAdvance(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EngineAdvance(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineAlpha(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineAlpha(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineAssetScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineAssetScale(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineDelta(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineDelta(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineDesignExplicit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_EngineDesignExplicit(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineDesignSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int io1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int io2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_EngineDesignSize(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_int_value(mrb, (mrb_int)io1);
    rets[1] = mrb_int_value(mrb, (mrb_int)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_EngineDisplay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineDisplay(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineDisplayCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int rv = Grapple_EngineDisplayCount();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineDisplayName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_EngineDisplayName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_EngineEffectsAvailable(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_EngineEffectsAvailable(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineEmbedMedia(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    size_t len0 = 0;
    const char *a0 = GrappleGen_RubyToBlob(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &len0);
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EngineEmbedMedia((const void *)a0, (int)len0, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineFps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineFps(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineFrameCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Uint64 rv = Grapple_EngineFrameCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineFrameScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineFrameScale(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineMaxFps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineMaxFps(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineMediaPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char * rv = Grapple_EngineMediaPath(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_EngineMediaSource(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_MediaSource rv = Grapple_EngineMediaSource(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineOverloadFrames(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineOverloadFrames(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EnginePixelSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int io1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int io2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_EnginePixelSize(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_int_value(mrb, (mrb_int)io1);
    rets[1] = mrb_int_value(mrb, (mrb_int)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_EnginePresentation_(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_EnginePresentation rv = Grapple_EnginePresentation_(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineQuit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_EngineQuit(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineRenderScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineRenderScale(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineRenderer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Renderer * rv = Grapple_EngineRenderer(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Renderer");
    }
}

static mrb_value GenR_Grapple_EngineRequestedSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_Settings * rv = Grapple_EngineRequestedSettings(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_EngineSafeRect(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FRect rv = Grapple_EngineSafeRect(a0);
    return GenPush_SDL_FRect(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_EngineSetClearColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Grapple_EngineSetClearColor(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineSetDesignSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_EngineSetDesignSize(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineSetDisplay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_EngineSetDisplay(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineSetGraphics(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_EngineSetGraphics(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineSetMaxFps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EngineSetMaxFps(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineSetMediaPassword(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_EngineSetMediaPassword(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineSetPresentation(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_EnginePresentation a1 = (Grapple_EnginePresentation)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_EngineSetPresentation(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineSetRefreshRate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EngineSetRefreshRate(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineSetTickRate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_EngineSetTickRate(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineSetTimeScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EngineSetTimeScale(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineShowWindow(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_EngineShowWindow(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EngineStep(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineStep(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineStepsLastFrame(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineStepsLastFrame(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineTick(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_EngineTick(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EngineTickRate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_EngineTickRate(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EngineTimeScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_EngineTimeScale(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineUiPoints(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float rv = Grapple_EngineUiPoints(a0, a1);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EngineViewRect(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FRect rv = Grapple_EngineViewRect(a0);
    return GenPush_SDL_FRect(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_EngineWindow(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Window * rv = Grapple_EngineWindow(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Window");
    }
}

static mrb_value GenR_Grapple_EngineWindowToDesign(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_EngineWindowToDesign(a0, a1, a2, &io3, &io4);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io3);
    rets[1] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_EventCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event * rv = Grapple_EventCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Event");
    }
}

static mrb_value GenR_Grapple_EventDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Grapple_EventDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EventGamepadAxis(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    int rv = Grapple_EventGamepadAxis(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventGamepadAxisValue(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventGamepadAxisValue(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventGamepadButton(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    int rv = Grapple_EventGamepadButton(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventGamepadWhich(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Sint32 rv = Grapple_EventGamepadWhich(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventKeyModifiers(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Uint16 rv = Grapple_EventKeyModifiers(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventKeyRepeat(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    bool rv = Grapple_EventKeyRepeat(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_EventKeyScancode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    int rv = Grapple_EventKeyScancode(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventMouseButton(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    int rv = Grapple_EventMouseButton(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventMouseDeltaX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventMouseDeltaX(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventMouseDeltaY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventMouseDeltaY(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventMouseX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventMouseX(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventMouseY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventMouseY(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventSetType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_EventSetType(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_EventText(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    const char * rv = Grapple_EventText(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_EventTouchX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventTouchX(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventTouchY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventTouchY(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Uint32 rv = Grapple_EventType(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_EventWheelX(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventWheelX(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventWheelY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    float rv = Grapple_EventWheelY(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_EventWindowId(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Event *a0 = (SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Event");
    Uint32 rv = Grapple_EventWindowId(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_FilterJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2FilterJointDef * rv = Grapple_FilterJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2FilterJointDef");
    }
}

static mrb_value GenR_Grapple_FilterJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2FilterJointDef *a0 = (b2FilterJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2FilterJointDef");
    Grapple_FilterJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_FilterJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2FilterJointDef *a0 = (b2FilterJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2FilterJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_FilterJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_FingerCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_FingerCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_FreeTiledMap(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_TiledMap *a0 = (Grapple_TiledMap *)GrappleGen_RubyTakeHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    Grapple_FreeTiledMap(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUAcquireSwapchain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUCommandBuffer");
    SDL_Window *a1 = (SDL_Window *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Window");
    SDL_GPUTexture * rv = Grapple_GPUAcquireSwapchain(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTexture");
    }
}

static mrb_value GenR_Grapple_GPUBeginComputePass(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUCommandBuffer");
    Grapple_GPUComputeBindings *a1 = (Grapple_GPUComputeBindings *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_GPUComputeBindings");
    SDL_GPUComputePass * rv = Grapple_GPUBeginComputePass(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUComputePass");
    }
}

static mrb_value GenR_Grapple_GPUBindComputeStorageBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePass *a0 = (SDL_GPUComputePass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUBuffer");
    Grapple_GPUBindComputeStorageBuffer(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBindComputeStorageTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePass *a0 = (SDL_GPUComputePass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUTexture");
    Grapple_GPUBindComputeStorageTexture(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBindFragmentStorageBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUBuffer");
    Grapple_GPUBindFragmentStorageBuffer(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBindFragmentStorageTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUTexture");
    Grapple_GPUBindFragmentStorageTexture(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBindVertexStorageBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUBuffer *a2 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUBuffer");
    Grapple_GPUBindVertexStorageBuffer(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBindVertexStorageTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderPass *a0 = (SDL_GPURenderPass *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderPass");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUTexture *a2 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUTexture");
    Grapple_GPUBindVertexStorageTexture(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBlitInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBlitInfo * rv = Grapple_GPUBlitInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUBlitInfo");
    }
}

static mrb_value GenR_Grapple_GPUBlitInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBlitInfo");
    Grapple_GPUBlitInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBlitInfoSetDestination(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBlitInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Uint32 a5 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    Grapple_GPUBlitInfoSetDestination(a0, a1, a2, a3, a4, a5);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBlitInfoSetFilter(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBlitInfo");
    SDL_GPUFilter a1 = (SDL_GPUFilter)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUBlitInfoSetFilter(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBlitInfoSetSource(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBlitInfo *a0 = (SDL_GPUBlitInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBlitInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Uint32 a5 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    Grapple_GPUBlitInfoSetSource(a0, a1, a2, a3, a4, a5);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferBindingCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferBinding * rv = Grapple_GPUBufferBindingCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUBufferBinding");
    }
}

static mrb_value GenR_Grapple_GPUBufferBindingDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferBinding *a0 = (SDL_GPUBufferBinding *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferBinding");
    Grapple_GPUBufferBindingDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferBindingSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferBinding *a0 = (SDL_GPUBufferBinding *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferBinding");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUBufferBindingSet(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferLocationCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferLocation * rv = Grapple_GPUBufferLocationCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUBufferLocation");
    }
}

static mrb_value GenR_Grapple_GPUBufferLocationDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferLocation *a0 = (SDL_GPUBufferLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferLocation");
    Grapple_GPUBufferLocationDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferLocationSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferLocation *a0 = (SDL_GPUBufferLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferLocation");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUBufferLocationSet(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferRegionCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferRegion * rv = Grapple_GPUBufferRegionCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUBufferRegion");
    }
}

static mrb_value GenR_Grapple_GPUBufferRegionDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferRegion *a0 = (SDL_GPUBufferRegion *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferRegion");
    Grapple_GPUBufferRegionDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUBufferRegionSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUBufferRegion *a0 = (SDL_GPUBufferRegion *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUBufferRegion");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_GPUBufferRegionSet(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo * rv = Grapple_GPUColorTargetInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUColorTargetInfo");
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    Grapple_GPUColorTargetInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoSetClearColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GPUColorTargetInfoSetClearColor(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoSetCycle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUColorTargetInfoSetCycle(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoSetMipLayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUColorTargetInfoSetMipLayer(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoSetOps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    SDL_GPULoadOp a1 = (SDL_GPULoadOp)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUStoreOp a2 = (SDL_GPUStoreOp)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUColorTargetInfoSetOps(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUColorTargetInfoSetTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUColorTargetInfo *a0 = (SDL_GPUColorTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUColorTargetInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Grapple_GPUColorTargetInfoSetTexture(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputeBindingsAddBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GPUComputeBindings");
    SDL_GPUBuffer *a1 = (SDL_GPUBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUBuffer");
    bool a2 = (bool)GrappleGen_RubyToBool((argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GPUComputeBindingsAddBuffer(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUComputeBindingsAddTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GPUComputeBindings");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool a4 = (bool)GrappleGen_RubyToBool((argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GPUComputeBindingsAddTexture(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUComputeBindingsCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GPUComputeBindings * rv = Grapple_GPUComputeBindingsCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_GPUComputeBindings");
    }
}

static mrb_value GenR_Grapple_GPUComputeBindingsDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GPUComputeBindings *a0 = (Grapple_GPUComputeBindings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GPUComputeBindings");
    Grapple_GPUComputeBindingsDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo * rv = Grapple_GPUComputePipelineInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUComputePipelineCreateInfo");
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePipelineCreateInfo");
    Grapple_GPUComputePipelineInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoSetCode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePipelineCreateInfo");
    size_t len1 = 0;
    const char *a1 = GrappleGen_RubyToBlob(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &len1);
    Grapple_GPUComputePipelineInfoSetCode(a0, (const void *)a1, (int)len1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoSetEntrypoint(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePipelineCreateInfo");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUComputePipelineInfoSetEntrypoint(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoSetFormat(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePipelineCreateInfo");
    SDL_GPUShaderFormat a1 = (SDL_GPUShaderFormat)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUComputePipelineInfoSetFormat(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUComputePipelineInfoSetThreadCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUComputePipelineCreateInfo *a0 = (SDL_GPUComputePipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUComputePipelineCreateInfo");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_GPUComputePipelineInfoSetThreadCount(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUDepthStencilTargetInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDepthStencilTargetInfo * rv = Grapple_GPUDepthStencilTargetInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUDepthStencilTargetInfo");
    }
}

static mrb_value GenR_Grapple_GPUDepthStencilTargetInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDepthStencilTargetInfo");
    Grapple_GPUDepthStencilTargetInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUDepthStencilTargetInfoSetClear(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDepthStencilTargetInfo");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint8 a2 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUDepthStencilTargetInfoSetClear(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUDepthStencilTargetInfoSetOps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDepthStencilTargetInfo");
    SDL_GPULoadOp a1 = (SDL_GPULoadOp)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUStoreOp a2 = (SDL_GPUStoreOp)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUDepthStencilTargetInfoSetOps(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUDepthStencilTargetInfoSetTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDepthStencilTargetInfo *a0 = (SDL_GPUDepthStencilTargetInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDepthStencilTargetInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Grapple_GPUDepthStencilTargetInfoSetTexture(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoAddColorTarget(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUTextureFormat a1 = (SDL_GPUTextureFormat)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GPUPipelineInfoAddColorTarget(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoAddVertexAttribute(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    SDL_GPUVertexElementFormat a3 = (SDL_GPUVertexElementFormat)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GPUPipelineInfoAddVertexAttribute(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoAddVertexBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    SDL_GPUVertexInputRate a3 = (SDL_GPUVertexInputRate)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_GPUPipelineInfoAddVertexBuffer(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo * rv = Grapple_GPUPipelineInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUGraphicsPipelineCreateInfo");
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    Grapple_GPUPipelineInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoSetDepthStencil(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUTextureFormat a1 = (SDL_GPUTextureFormat)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool a2 = (bool)GrappleGen_RubyToBool((argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUPipelineInfoSetDepthStencil(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoSetFillMode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUFillMode a1 = (SDL_GPUFillMode)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUCullMode a2 = (SDL_GPUCullMode)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUPipelineInfoSetFillMode(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoSetPrimitive(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUPrimitiveType a1 = (SDL_GPUPrimitiveType)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUPipelineInfoSetPrimitive(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUPipelineInfoSetShaders(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUGraphicsPipelineCreateInfo *a0 = (SDL_GPUGraphicsPipelineCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUGraphicsPipelineCreateInfo");
    SDL_GPUShader *a1 = (SDL_GPUShader *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUShader");
    SDL_GPUShader *a2 = (SDL_GPUShader *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUShader");
    Grapple_GPUPipelineInfoSetShaders(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPURenderStateInfoAddSampler(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderStateCreateInfo");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    SDL_GPUSampler *a2 = (SDL_GPUSampler *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUSampler");
    bool rv = Grapple_GPURenderStateInfoAddSampler(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPURenderStateInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderStateCreateInfo * rv = Grapple_GPURenderStateInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPURenderStateCreateInfo");
    }
}

static mrb_value GenR_Grapple_GPURenderStateInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderStateCreateInfo");
    Grapple_GPURenderStateInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPURenderStateInfoSetShader(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPURenderStateCreateInfo *a0 = (SDL_GPURenderStateCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPURenderStateCreateInfo");
    SDL_GPUShader *a1 = (SDL_GPUShader *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUShader");
    Grapple_GPURenderStateInfoSetShader(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo * rv = Grapple_GPUShaderCreateInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUShaderCreateInfo");
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUShaderCreateInfo");
    Grapple_GPUShaderCreateInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoSetCode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUShaderCreateInfo");
    size_t len1 = 0;
    const char *a1 = GrappleGen_RubyToBlob(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &len1);
    Grapple_GPUShaderCreateInfoSetCode(a0, (const void *)a1, (int)len1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoSetCounts(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUShaderCreateInfo");
    Uint32 a1 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GPUShaderCreateInfoSetCounts(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoSetEntrypoint(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUShaderCreateInfo");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GPUShaderCreateInfoSetEntrypoint(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUShaderCreateInfoSetFormat(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUShaderCreateInfo *a0 = (SDL_GPUShaderCreateInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUShaderCreateInfo");
    SDL_GPUShaderFormat a1 = (SDL_GPUShaderFormat)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_GPUShaderStage a2 = (SDL_GPUShaderStage)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUShaderCreateInfoSetFormat(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUSwapchainHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Uint32 rv = Grapple_GPUSwapchainHeight();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GPUSwapchainWidth(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Uint32 rv = Grapple_GPUSwapchainWidth();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GPUTextureLocationCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureLocation * rv = Grapple_GPUTextureLocationCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTextureLocation");
    }
}

static mrb_value GenR_Grapple_GPUTextureLocationDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureLocation *a0 = (SDL_GPUTextureLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureLocation");
    Grapple_GPUTextureLocationDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureLocationSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureLocation *a0 = (SDL_GPUTextureLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureLocation");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GPUTextureLocationSet(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureRegionCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureRegion * rv = Grapple_GPUTextureRegionCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTextureRegion");
    }
}

static mrb_value GenR_Grapple_GPUTextureRegionDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureRegion *a0 = (SDL_GPUTextureRegion *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureRegion");
    Grapple_GPUTextureRegionDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureRegionSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureRegion *a0 = (SDL_GPUTextureRegion *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureRegion");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Uint32 a5 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    Grapple_GPUTextureRegionSet(a0, a1, a2, a3, a4, a5);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureSamplerBindingCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureSamplerBinding * rv = Grapple_GPUTextureSamplerBindingCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTextureSamplerBinding");
    }
}

static mrb_value GenR_Grapple_GPUTextureSamplerBindingDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureSamplerBinding *a0 = (SDL_GPUTextureSamplerBinding *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureSamplerBinding");
    Grapple_GPUTextureSamplerBindingDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureSamplerBindingSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureSamplerBinding *a0 = (SDL_GPUTextureSamplerBinding *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureSamplerBinding");
    SDL_GPUTexture *a1 = (SDL_GPUTexture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTexture");
    SDL_GPUSampler *a2 = (SDL_GPUSampler *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_GPUSampler");
    Grapple_GPUTextureSamplerBindingSet(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureTransferInfoCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureTransferInfo * rv = Grapple_GPUTextureTransferInfoCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTextureTransferInfo");
    }
}

static mrb_value GenR_Grapple_GPUTextureTransferInfoDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureTransferInfo *a0 = (SDL_GPUTextureTransferInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureTransferInfo");
    Grapple_GPUTextureTransferInfoDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTextureTransferInfoSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTextureTransferInfo *a0 = (SDL_GPUTextureTransferInfo *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTextureTransferInfo");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint32 a3 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GPUTextureTransferInfoSet(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTransferBufferLocationCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTransferBufferLocation * rv = Grapple_GPUTransferBufferLocationCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTransferBufferLocation");
    }
}

static mrb_value GenR_Grapple_GPUTransferBufferLocationDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTransferBufferLocation *a0 = (SDL_GPUTransferBufferLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTransferBufferLocation");
    Grapple_GPUTransferBufferLocationDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUTransferBufferLocationSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUTransferBufferLocation *a0 = (SDL_GPUTransferBufferLocation *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUTransferBufferLocation");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GPUTransferBufferLocationSet(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GPUUploadToTransferBuffer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDevice *a0 = (SDL_GPUDevice *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDevice");
    SDL_GPUTransferBuffer *a1 = (SDL_GPUTransferBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUTransferBuffer");
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    size_t len3 = 0;
    const char *a3 = GrappleGen_RubyToBlob(mrb, (argc > 3 ? argv[3] : mrb_nil_value()), &len3);
    bool a5 = (bool)GrappleGen_RubyToBool((argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GPUUploadToTransferBuffer(a0, a1, a2, (const void *)a3, (int)len3, a5);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GPUWaitAndAcquireSwapchain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUCommandBuffer *a0 = (SDL_GPUCommandBuffer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUCommandBuffer");
    SDL_Window *a1 = (SDL_Window *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Window");
    SDL_GPUTexture * rv = Grapple_GPUWaitAndAcquireSwapchain(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_GPUTexture");
    }
}

static mrb_value GenR_Grapple_GPUWaitForFence(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_GPUDevice *a0 = (SDL_GPUDevice *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_GPUDevice");
    SDL_GPUFence *a1 = (SDL_GPUFence *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_GPUFence");
    bool rv = Grapple_GPUWaitForFence(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadAccelerometer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GamepadAccelerometer(a0, a1, &io2, &io3, &io4);
    mrb_value rets[3];
    rets[0] = mrb_float_value(mrb, (mrb_float)io2);
    rets[1] = mrb_float_value(mrb, (mrb_float)io3);
    rets[2] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 3, rets);
    }
}

static mrb_value GenR_Grapple_GamepadAxisValue(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadAxis a2 = (Grapple_GamepadAxis)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float rv = Grapple_GamepadAxisValue(a0, a1, a2);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GamepadButtonDown(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GamepadButtonDown(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadButtonPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GamepadButtonPressed(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadButtonReleased(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadButton a2 = (Grapple_GamepadButton)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GamepadButtonReleased(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadConnected(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GamepadConnected(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_GamepadCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GamepadDeadzone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_GamepadDeadzone(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GamepadDirectionPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Direction a2 = (Grapple_Direction)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GamepadDirectionPressed(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadDirectionRepeat(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Direction a2 = (Grapple_Direction)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GamepadDirectionRepeat(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadGyro(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GamepadGyro(a0, a1, &io2, &io3, &io4);
    mrb_value rets[3];
    rets[0] = mrb_float_value(mrb, (mrb_float)io2);
    rets[1] = mrb_float_value(mrb, (mrb_float)io3);
    rets[2] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 3, rets);
    }
}

static mrb_value GenR_Grapple_GamepadHasAccelerometer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GamepadHasAccelerometer(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadHasGyro(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GamepadHasGyro(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_GamepadName(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GamepadRumble(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GamepadRumble(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadRumbleTriggers(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint32 a4 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GamepadRumbleTriggers(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadSetLED(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint8 a2 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Uint8 a3 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Uint8 a4 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_GamepadSetLED(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GamepadStick(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float io3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float io4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_GamepadStick(a0, a1, a2, &io3, &io4);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io3);
    rets[1] = mrb_float_value(mrb, (mrb_float)io4);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GamepadStopRumble(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GamepadStopRumble(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GetAudioBusGain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_AudioBus a0 = (Grapple_AudioBus)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float rv = Grapple_GetAudioBusGain(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GetChipDiagnosticCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int rv = Grapple_GetChipDiagnosticCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GetChipDiagnosticMessage(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_GetChipDiagnosticMessage(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GetChipExpressionDefaults(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipExpression out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_GetChipExpressionDefaults(&out0);
    return GenPush_Grapple_ChipExpression(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_GetChipImportDefaults(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipImportOptions out0;
    memset(&out0, 0, sizeof(out0));
    bool rv = Grapple_GetChipImportDefaults(&out0);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipImportOptions(mrb, &out0);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GetChipPlayerPeakVoices(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int rv = Grapple_GetChipPlayerPeakVoices(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GetChipPlayerStyleInfo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ChipStyleInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_GetChipPlayerStyleInfo(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipStyleInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GetChipPresetEffects(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPreset a0 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_ChipEffects out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_GetChipPresetEffects(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipEffects(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GetChipSectionCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int rv = Grapple_GetChipSectionCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GetChipStyle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const Grapple_ChipStyle * rv = Grapple_GetChipStyle(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_ChipStyle");
    }
}

static mrb_value GenR_Grapple_GetChipStyleCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int rv = Grapple_GetChipStyleCount();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GetLaunchSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings * rv = Grapple_GetLaunchSettings();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_GraphicsClamp(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_GraphicsClamp(&out0);
    return GenPush_Grapple_GraphicsSettings(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_GraphicsConfigError(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char * rv = Grapple_GraphicsConfigError();
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GraphicsConfigPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char * rv = Grapple_GraphicsConfigPath();
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GraphicsDefaults(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings rv = Grapple_GraphicsDefaults();
    return GenPush_Grapple_GraphicsSettings(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_GraphicsEqual(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    bool rv = Grapple_GraphicsEqual(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GraphicsLightMapScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float rv = Grapple_GraphicsLightMapScale(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GraphicsLoadTomlFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool rv = Grapple_GraphicsLoadTomlFile(&out0, a1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_GraphicsSettings(mrb, &out0);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GraphicsLoadTomlString(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings out0;
    memset(&out0, 0, sizeof(out0));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool rv = Grapple_GraphicsLoadTomlString(&out0, a1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_GraphicsSettings(mrb, &out0);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GraphicsMaxDynamicLights(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    int rv = Grapple_GraphicsMaxDynamicLights(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GraphicsParticleDensity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float rv = Grapple_GraphicsParticleDensity(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GraphicsQualityFromName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_GraphicsQuality io1 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GraphicsQualityFromName(a0, &io1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = mrb_int_value(mrb, (mrb_int)io1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_GraphicsQualityName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_GraphicsQualityName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GraphicsSafeMode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings rv = Grapple_GraphicsSafeMode();
    return GenPush_Grapple_GraphicsSettings(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_GraphicsSave(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GraphicsSave(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GraphicsSavePath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    char * rv = Grapple_GraphicsSavePath(a0, a1);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_GraphicsShadowRays(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    int rv = Grapple_GraphicsShadowRays(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GraphicsShadowSoftness(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsQuality a0 = (Grapple_GraphicsQuality)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float rv = Grapple_GraphicsShadowSoftness(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GraphicsToToml(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GraphicsSettings tmp0;
    const Grapple_GraphicsSettings *a0 = NULL;
    if (argc > 0 && mrb_hash_p(argv[0])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[0], &tmp0);
        a0 = &tmp0;
    }
    char * rv = Grapple_GraphicsToToml(a0);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_GuiContext(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    struct nk_context * rv = Grapple_GuiContext(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "nk_context");
    }
}

static mrb_value GenR_Grapple_GuiDrawCommandCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int rv = Grapple_GuiDrawCommandCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GuiDrawTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Texture");
    SDL_FRect a2;
    GenRead_SDL_FRect(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_GuiImageMode a3 = (Grapple_GuiImageMode)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_GuiDrawTexture(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiDrawTextureOverlay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Texture");
    SDL_FRect a2;
    GenRead_SDL_FRect(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_GuiImageMode a3 = (Grapple_GuiImageMode)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_GuiDrawTextureOverlay(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiFontHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    float rv = Grapple_GuiFontHeight(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GuiGridBeginOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GuiGridBeginOwned(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiGridCell(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    Grapple_GuiGridCell(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCellOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiGridCellOwned(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCellPart(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GuiAlign a3 = (Grapple_GuiAlign)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_GuiGridCellPart(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCellPartOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GuiAlign a3 = (Grapple_GuiAlign)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_GuiGridCellPartOwned(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCellSpan(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiGridCellSpan(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCellSpanOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiGridCellSpanOwned(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid * rv = Grapple_GuiGridCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_GuiGrid");
    }
}

static mrb_value GenR_Grapple_GuiGridDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    Grapple_GuiGridDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridEnd(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    Grapple_GuiGridEnd(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridEndOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiGridEndOwned(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridNextRow(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    Grapple_GuiGridNextRow(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridNextRowOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiGridNextRowOwned(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridRowHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiGridRowHeight(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridRowHeightOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiGridRowHeightOwned(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridSpacing(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_GuiGrid *a0 = (Grapple_GuiGrid *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_GuiGrid");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GuiGridSpacing(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridSpacingOwned(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_GuiGridSpacingOwned(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiGridWeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GuiGridWeight(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiImage(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Texture");
    Grapple_GuiImageMode a2 = (Grapple_GuiImageMode)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GuiImage(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiImageButton(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Texture");
    Grapple_GuiImageMode a2 = (Grapple_GuiImageMode)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_GuiImageButton(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiInputBegin(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiInputBegin(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiInputEnd(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiInputEnd(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiKeyPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GuiKeyPressed(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiMemoryUsed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int rv = Grapple_GuiMemoryUsed(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GuiOpenFileButton(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_GuiOpenFileButton(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiPopFont(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiPopFont(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiPopStyleColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiPopStyleColor(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiProcessEvent(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    const SDL_Event *a1 = (const SDL_Event *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Event");
    bool rv = Grapple_GuiProcessEvent(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiPumpEvents(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    bool rv = Grapple_GuiPumpEvents(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiPushFont(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiFontSize a1 = (Grapple_GuiFontSize)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GuiPushFont(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiPushStyleColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiStyleColor a1 = (Grapple_GuiStyleColor)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Color a2;
    GenRead_SDL_Color(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    bool rv = Grapple_GuiPushStyleColor(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiRender(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    bool rv = Grapple_GuiRender(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiRenderer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    SDL_Renderer * rv = Grapple_GuiRenderer(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Renderer");
    }
}

static mrb_value GenR_Grapple_GuiSaveFileButton(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    size_t len3 = 0;
    const char *a3 = GrappleGen_RubyToBlob(mrb, (argc > 3 ? argv[3] : mrb_nil_value()), &len3);
    bool rv = Grapple_GuiSaveFileButton(a0, a1, a2, (const void *)a3, (size_t)len3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiSavedPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    const char * rv = Grapple_GuiSavedPath(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_GuiScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    float rv = Grapple_GuiScale(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_GuiSetFont(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    Grapple_GuiFontSize a1 = (Grapple_GuiFontSize)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GuiSetFont(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiSetTooltipDelay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_GuiSetTooltipDelay(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_GuiTooltip(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_GuiTooltip(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_GuiTooltipDelay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    int rv = Grapple_GuiTooltipDelay(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_GuiWantsInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Gui *a0 = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Gui");
    bool rv = Grapple_GuiWantsInput(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_HMACSHA256(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    size_t len0 = 0;
    const char *a0 = GrappleGen_RubyToBlob(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &len0);
    size_t len2 = 0;
    const char *a2 = GrappleGen_RubyToBlob(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &len2);
    Uint8 io4 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_HMACSHA256((const void *)a0, (size_t)len0, (const void *)a2, (size_t)len2, &io4);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = mrb_int_value(mrb, (mrb_int)io4);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_HasDeviceMotion(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_HasDeviceMotion(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_IdleSeconds(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_IdleSeconds(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_KeyDown(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_KeyDown(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_KeyModifiers(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Keymod rv = Grapple_KeyModifiers(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_KeyPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_KeyPressed(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_KeyReleased(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_Scancode a1 = (SDL_Scancode)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_KeyReleased(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_LastInputDevice(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_InputDevice rv = Grapple_LastInputDevice(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_LightAddDarkZone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    SDL_FColor a2;
    GenRead_SDL_FColor(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_LightAddDarkZone(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightAddOccluder(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Grapple_LightAddOccluder(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightAddOccluderLine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_LightAddOccluderLine(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightAmbient(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FColor rv = Grapple_LightAmbient(a0);
    return GenPush_SDL_FColor(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_LightAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float rv = Grapple_LightAt(a0, a1, a2);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_LightBeginFrame(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_LightBeginFrame(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_LightCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_LightDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    Grapple_LightDefDestroy(&out0);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_LightDefSetColor(&out0, a1, a2, a3, a4);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetCone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_LightDefSetCone(&out0, a1, a2);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetFlicker(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_LightDefSetFlicker(&out0, a1);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetOffset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_LightDefSetOffset(&out0, a1, a2);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetRadius(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_LightDefSetRadius(&out0, a1);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefSetShadows(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef out0;
    memset(&out0, 0, sizeof(out0));
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_LightDefSetShadows(&out0, a1);
    return GenPush_Grapple_LightDef(mrb, &out0);
    }
}

static mrb_value GenR_Grapple_LightDefault(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightDef rv = Grapple_LightDefault();
    return GenPush_Grapple_LightDef(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_LightHour(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_LightHour(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_LightLineOfSight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_LightLineOfSight(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_LightPreset_(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_LightPreset rv = Grapple_LightPreset_(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_LightRender(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_Camera(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_LightRender(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_LightSetAmbient(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Grapple_LightSetAmbient(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightSetAutoOccluders(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_LightSetAutoOccluders(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightSetClock(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_LightSetClock(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightSetPreset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_LightPreset a1 = (Grapple_LightPreset)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_LightSetPreset(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_LightSunlight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_LightSunlight(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_LightUsesShaders(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    bool rv = Grapple_LightUsesShaders(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_LoadChipSong(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_ChipSong * rv = Grapple_LoadChipSong(a0);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipSong", GenDtor_Grapple_DestroyChipSong);
    }
}

static mrb_value GenR_Grapple_LoadChipSongEx(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_ChipImportOptions tmp1;
    const Grapple_ChipImportOptions *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_ChipImportOptions(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    Grapple_ChipDiagnostic out2;
    memset(&out2, 0, sizeof(out2));
    Grapple_ChipSong * rv = Grapple_LoadChipSongEx(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipSong", GenDtor_Grapple_DestroyChipSong);
    rets[1] = GenPush_Grapple_ChipDiagnostic(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_LoadChipStyle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_ChipStyle * rv = Grapple_LoadChipStyle(a0);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipStyle", GenDtor_Grapple_DestroyChipStyle);
    }
}

static mrb_value GenR_Grapple_LoadPlatformer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Platformer * rv = Grapple_LoadPlatformer(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_Platformer", GenDtor_Grapple_DestroyPlatformer);
    }
}

static mrb_value GenR_Grapple_LoadTextFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    char * rv = Grapple_LoadTextFile(a0);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_LoadTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_TextureId rv = Grapple_LoadTexture(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_LoadTextureAsync(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_TextureId rv = Grapple_LoadTextureAsync(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_LoadTiledMap(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_TiledMap * rv = Grapple_LoadTiledMap(a0);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_TiledMap", GenDtor_Grapple_FreeTiledMap);
    }
}

static mrb_value GenR_Grapple_MotorJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MotorJointDef * rv = Grapple_MotorJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2MotorJointDef");
    }
}

static mrb_value GenR_Grapple_MotorJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MotorJointDef *a0 = (b2MotorJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MotorJointDef");
    Grapple_MotorJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MotorJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MotorJointDef *a0 = (b2MotorJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MotorJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_MotorJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MountEncryptedArchive(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    size_t len0 = 0;
    const char *a0 = GrappleGen_RubyToBlob(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &len0);
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_MountEncryptedArchive((const void *)a0, (int)len0, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MountEncryptedArchiveFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_MountEncryptedArchiveFile(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MountMedia(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_MediaSource rv = Grapple_MountMedia(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_MouseCaptured(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_MouseCaptured(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MouseDelta(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_MouseDelta(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_MouseDown(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_MouseDown(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MouseJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MouseJointDef * rv = Grapple_MouseJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2MouseJointDef");
    }
}

static mrb_value GenR_Grapple_MouseJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MouseJointDef");
    Grapple_MouseJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MouseJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MouseJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_MouseJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MouseJointDefSetMaxForce(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MouseJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_MouseJointDefSetMaxForce(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MouseJointDefSetSpring(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2MouseJointDef *a0 = (b2MouseJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2MouseJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_MouseJointDefSetSpring(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_MousePosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_MousePosition(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_MousePressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_MousePressed(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MouseReleased(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_MouseButton a1 = (Grapple_MouseButton)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_MouseReleased(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_MouseWheel(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_MouseWheel(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_OpenVFSRead(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    SDL_IOStream * rv = Grapple_OpenVFSRead(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_IOStream");
    }
}

static mrb_value GenR_Grapple_PauseChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_PauseChipPlayer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PhysicsBodyCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_PhysicsBodyCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PhysicsGravity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PhysicsGravity(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PhysicsOverlap(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FRect a1;
    GenRead_SDL_FRect(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Uint32 a2 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorId io3 = (Grapple_ActorId)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    int rv = Grapple_PhysicsOverlap(a0, a1, a2, &io3, a4);
    mrb_value rets[2];
    rets[0] = mrb_int_value(mrb, (mrb_int)rv);
    rets[1] = mrb_int_value(mrb, (mrb_int)io3);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PhysicsPaused(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_PhysicsPaused(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PhysicsPixelsPerMetre(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_PhysicsPixelsPerMetre(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_PhysicsRaycast(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Uint32 a5 = (Uint32)GrappleGen_RubyToInt(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    Grapple_RayHit rv = Grapple_PhysicsRaycast(a0, a1, a2, a3, a4, a5);
    return GenPush_Grapple_RayHit(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_PhysicsSetGravity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PhysicsSetGravity(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PhysicsSetPaused(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PhysicsSetPaused(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PhysicsSetPixelsPerMetre(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PhysicsSetPixelsPerMetre(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PhysicsSetSubSteps(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PhysicsSetSubSteps(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerActionDown(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_PlatformerActionDown(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerActionPressed(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_PlatformerActionPressed(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerActionReleased(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_PlatformerActionReleased(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerActionValue(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float rv = Grapple_PlatformerActionValue(a0, a1);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerActions(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_ActionMap * rv = Grapple_PlatformerActions(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_ActionMap");
    }
}

static mrb_value GenR_Grapple_PlatformerAddSolid(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    int rv = Grapple_PlatformerAddSolid(a0, a1, a2, a3, a4);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerAttach(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerAttach(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerAttached(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerAttached(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerBind(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_PlatformerBind(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerCameraPosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerCameraPosition(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PlatformerCreateBlock(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_PlatformerCreateBlock(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerCreateFloor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_PlatformerCreateFloor(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerCreatePlatform(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_PlatformerCreatePlatform(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerCreatePlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ActorId rv = Grapple_PlatformerCreatePlayer(a0, a1, a2);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerCreateStairs(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool a4 = (bool)GrappleGen_RubyToBool((argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_PlatformerCreateStairs(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerCreateWall(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_PlatformerCreateWall(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerDetach(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerDetach(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerEngine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_Engine * rv = Grapple_PlatformerEngine(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Engine");
    }
}

static mrb_value GenR_Grapple_PlatformerFillTiles(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int a4 = (int)GrappleGen_RubyToInt(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_PlatformerTile a5 = (Grapple_PlatformerTile)GrappleGen_RubyToInt(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    int rv = Grapple_PlatformerFillTiles(a0, a1, a2, a3, a4, a5);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerHasPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerHasPlayer(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerHeight(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPixelSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerPixelSize(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_ActorId rv = Grapple_PlatformerPlayer(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerBumped(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerBumped(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerFacing(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerPlayerFacing(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerFell(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerFell(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerGrounded(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerGrounded(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerJumped(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerJumped(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerLanded(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerLanded(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerPaused(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerPaused(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerPosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerPlayerPosition(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerRespawn(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerPlayerRespawn(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerRespawnAtStart(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerPlayerRespawnAtStart(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerPlayerSize(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerState(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlayerState rv = Grapple_PlatformerPlayerState(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerStateChanged(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerPlayerStateChanged(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerStateName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char * rv = Grapple_PlatformerPlayerStateName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerTuning(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float rv = Grapple_PlatformerPlayerTuning(a0, a1);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerTuningCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int rv = Grapple_PlatformerPlayerTuningCount();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerTuningKey(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_PlatformerPlayerTuningKey(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerVelocity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float io1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float io2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerPlayerVelocity(a0, &io1, &io2);
    mrb_value rets[2];
    rets[0] = mrb_float_value(mrb, (mrb_float)io1);
    rets[1] = mrb_float_value(mrb, (mrb_float)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_PlatformerPlayerWall(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerPlayerWall(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerRemovePlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerRemovePlayer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerRemoveSolid(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_PlatformerRemoveSolid(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerRender(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerRender(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerScriptInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool a2 = (bool)GrappleGen_RubyToBool((argc > 2 ? argv[2] : mrb_nil_value()));
    bool a3 = (bool)GrappleGen_RubyToBool((argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_PlatformerScriptInput(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerScriptedInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool rv = Grapple_PlatformerScriptedInput(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerScrollMode(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerScroll rv = Grapple_PlatformerScrollMode(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerSetBackgroundColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_PlatformerSetBackgroundColor(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetCameraDeadzone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerSetCameraDeadzone(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetCameraLookAhead(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerSetCameraLookAhead(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetCameraSmoothing(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerSetCameraSmoothing(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetPlayerColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_PlatformerSetPlayerColor(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetPlayerPaused(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerSetPlayerPaused(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetPlayerSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerSetPlayerSize(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetPlayerTuning(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_PlatformerSetPlayerTuning(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerSetScriptedInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerSetScriptedInput(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetScroll(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerScroll a1 = (Grapple_PlatformerScroll)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerSetScroll(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetTile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerTile a3 = (Grapple_PlatformerTile)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_PlatformerSetTile(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlatformerSetTileColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerTile a1 = (Grapple_PlatformerTile)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    float a5 = (float)GrappleGen_RubyToNum(mrb, (argc > 5 ? argv[5] : mrb_nil_value()));
    Grapple_PlatformerSetTileColor(a0, a1, a2, a3, a4, a5);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSetTileTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    Grapple_PlatformerTile a1 = (Grapple_PlatformerTile)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Texture *a2 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), "SDL_Texture");
    Grapple_PlatformerSetTileTexture(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerSolidCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerSolidCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerStep(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerStep(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerTileAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PlatformerTile rv = Grapple_PlatformerTileAt(a0, a1, a2);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerTileSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerTileSize(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlatformerUpdate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_PlatformerUpdate(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PlatformerWidth(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Platformer *a0 = (Grapple_Platformer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Platformer");
    int rv = Grapple_PlatformerWidth(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_PlayChipFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipPlayer * rv = Grapple_PlayChipFile(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipPlayer", GenDtor_Grapple_DestroyChipPlayer);
    }
}

static mrb_value GenR_Grapple_PlayChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    bool rv = Grapple_PlayChipPlayer(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_PlayChipSong(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipPlayer * rv = Grapple_PlayChipSong(a0, a1);
    return GrappleGen_RubyPushOwned(mrb, (void *)rv, "Grapple_ChipPlayer", GenDtor_Grapple_DestroyChipPlayer);
    }
}

static mrb_value GenR_Grapple_PlayerStateName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_PlayerState a0 = (Grapple_PlayerState)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_PlayerStateName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef * rv = Grapple_PrismaticJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2PrismaticJointDef");
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    Grapple_PrismaticJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefSetAnchors(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_PrismaticJointDefSetAnchors(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefSetAxis(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_PrismaticJointDefSetAxis(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_PrismaticJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefSetLimit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_PrismaticJointDefSetLimit(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_PrismaticJointDefSetMotor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2PrismaticJointDef *a0 = (b2PrismaticJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2PrismaticJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_PrismaticJointDefSetMotor(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ProbeRenderBackend(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_RenderBackendInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ProbeRenderBackend(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_RenderBackendInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_QuitDebugText(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_QuitDebugText();
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_ReadChipDiagnostic(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipDiagnostic out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ReadChipDiagnostic(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipDiagnostic(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipPlayerPosition(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ChipPosition out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ReadChipPlayerPosition(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipPosition(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipRoleControls(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ChipPreset a1 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipRoleControls out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ReadChipRoleControls(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipRoleControls(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipSection(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipSection out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ReadChipSection(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipSection(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipSongInfo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    Grapple_ChipSongInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ReadChipSongInfo(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipSongInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipStyleInfo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipStyle *a0 = (const Grapple_ChipStyle *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipStyle");
    Grapple_ChipStyleInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ReadChipStyleInfo(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipStyleInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipStyleInfoAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_ChipStyleInfo out1;
    memset(&out1, 0, sizeof(out1));
    bool rv = Grapple_ReadChipStyleInfoAt(a0, &out1);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipStyleInfo(mrb, &out1);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipTrackEffects(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipPreset a2 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ChipEffects out3;
    memset(&out3, 0, sizeof(out3));
    bool rv = Grapple_ReadChipTrackEffects(a0, a1, a2, &out3);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipEffects(mrb, &out3);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipTrackMapping(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ChipMapping out3;
    memset(&out3, 0, sizeof(out3));
    bool rv = Grapple_ReadChipTrackMapping(a0, a1, a2, &out3);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipMapping(mrb, &out3);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_ReadChipTrackMix(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipTrackMix out2;
    memset(&out2, 0, sizeof(out2));
    bool rv = Grapple_ReadChipTrackMix(a0, a1, &out2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_ChipTrackMix(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_RegexEscape(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    char * rv = Grapple_RegexEscape(a0);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_RegexFlags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char * rv = Grapple_RegexFlags(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RegexGroup(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_RegexGroup(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RegexGroupBegin(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_RegexGroupBegin(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RegexGroupCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int rv = Grapple_RegexGroupCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RegexGroupEnd(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_RegexGroupEnd(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RegexMatchAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_RegexMatchAt(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RegexNamedGroup(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_RegexNamedGroup(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RegexNamedGroupCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int rv = Grapple_RegexNamedGroupCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RegexNamedGroupName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_RegexNamedGroupName(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RegexPattern(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char * rv = Grapple_RegexPattern(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RegexReplace(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool a3 = (bool)GrappleGen_RubyToBool((argc > 3 ? argv[3] : mrb_nil_value()));
    const char * rv = Grapple_RegexReplace(a0, a1, a2, a3);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RegexSearch(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Regex *a0 = (Grapple_Regex *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Regex");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_RegexSearch(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RegisterChipStyle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipStyle *a0 = (Grapple_ChipStyle *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipStyle");
    bool rv = Grapple_RegisterChipStyle(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RenderBackendCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int rv = Grapple_RenderBackendCount();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RenderBackendName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_RenderBackendName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_RenderBackendValid(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    bool rv = Grapple_RenderBackendValid(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RenderDebugText(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Renderer *a0 = (SDL_Renderer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Renderer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_RenderDebugText(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RenderLastStats(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_RenderStats rv = Grapple_RenderLastStats(a0);
    return GenPush_Grapple_RenderStats(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_RenderLighting(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    bool rv = Grapple_RenderLighting(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_RenderOverlay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_RenderOverlay(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_RenderWorld(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_Camera tmp1;
    const Grapple_Camera *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_Camera(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int rv = Grapple_RenderWorld(a0, a1, a2);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_ResetChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ResetChipPlayer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef * rv = Grapple_RevoluteJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2RevoluteJointDef");
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    Grapple_RevoluteJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetAnchors(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_RevoluteJointDefSetAnchors(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_RevoluteJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetCollideConnected(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_RevoluteJointDefSetCollideConnected(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetLimit(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_RevoluteJointDefSetLimit(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetMotor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_RevoluteJointDefSetMotor(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RevoluteJointDefSetSpring(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2RevoluteJointDef *a0 = (b2RevoluteJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2RevoluteJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_RevoluteJointDefSetSpring(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_RouteAudioTrack(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    MIX_Track *a0 = (MIX_Track *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "MIX_Track");
    Grapple_AudioBus a1 = (Grapple_AudioBus)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_RouteAudioTrack(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SHA256(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    size_t len0 = 0;
    const char *a0 = GrappleGen_RubyToBlob(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), &len0);
    Uint8 io2 = (Uint8)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SHA256((const void *)a0, (size_t)len0, &io2);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = mrb_int_value(mrb, (mrb_int)io2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_SampleLight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float rv = Grapple_SampleLight(a0, a1, a2);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_SaveChipSongWav(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_ChipSong *a0 = (const Grapple_ChipSong *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipSong");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SaveChipSongWav(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SaveDelete(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SaveDelete(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SaveExists(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SaveExists(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SaveInfoOf(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SaveInfo rv = Grapple_SaveInfoOf(a0, a1);
    return GenPush_Grapple_SaveInfo(mrb, &rv);
    }
}

static mrb_value GenR_Grapple_SavePath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    char * rv = Grapple_SavePath(a0, a1);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_SaveSetIdentity(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_SaveSetIdentity(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SaveWrite(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    size_t len2 = 0;
    const char *a2 = GrappleGen_RubyToBlob(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &len2);
    const char *a4 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SaveWrite(a0, a1, (const void *)a2, (size_t)len2, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneCurrent(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_Scene * rv = Grapple_SceneCurrent(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Scene");
    }
}

static mrb_value GenR_Grapple_SceneDepth(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int rv = Grapple_SceneDepth(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_SceneEngine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Scene");
    Grapple_Engine * rv = Grapple_SceneEngine(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Engine");
    }
}

static mrb_value GenR_Grapple_SceneFind(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_Scene * rv = Grapple_SceneFind(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Scene");
    }
}

static mrb_value GenR_Grapple_SceneIsActive(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Scene");
    bool rv = Grapple_SceneIsActive(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneKey(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Scene");
    Sint64 rv = Grapple_SceneKey(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_SceneName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Scene *a0 = (Grapple_Scene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Scene");
    const char * rv = Grapple_SceneName(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_ScenePending(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_ScenePending(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScenePop(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_ScenePop(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScenePush(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_SceneDef");
    bool rv = Grapple_ScenePush(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneReplace(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_SceneDef");
    bool rv = Grapple_SceneReplace(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneReset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_SceneDef");
    bool rv = Grapple_SceneReset(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneSetTransitionColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    SDL_FColor a1;
    GenRead_SDL_FColor(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Grapple_SceneSetTransitionColor(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SceneTransitionTo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const Grapple_SceneDef *a1 = (const Grapple_SceneDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_SceneDef");
    Grapple_SceneTransition a2 = (Grapple_SceneTransition)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SceneTransitionTo(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SceneTransitioning(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_SceneTransitioning(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptHasHandlers(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_ScriptHasHandlers(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptRun(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool rv = Grapple_ScriptRun(a0);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneDefine(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneDefine(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneDefined(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneDefined(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptScenePush(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ScriptScenePush(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneReplace(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneReplace(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneReset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneReset(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneSetFlags(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SceneFlags a2 = (Grapple_SceneFlags)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneSetFlags(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneSetHook(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SceneHook a2 = (Grapple_SceneHook)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Sint64 a3 = (Sint64)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneSetHook(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneSetStateSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneSetStateSize(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSceneTransitionTo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SceneTransition a2 = (Grapple_SceneTransition)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ScriptSceneTransitionTo(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptSetHook(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ScriptHook a1 = (Grapple_ScriptHook)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Sint64 a2 = (Sint64)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_ScriptSetHook(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ScriptUnbind(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_ScriptUnbind(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SeekChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SeekChipPlayer(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetAudioBusGain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_AudioBus a0 = (Grapple_AudioBus)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SetAudioBusGain(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetAudioMuted(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    bool a0 = (bool)GrappleGen_RubyToBool((argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_SetAudioMuted(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetChipPart(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipComposer *a0 = (Grapple_ChipComposer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipComposer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_ChipPreset a3 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    bool rv = Grapple_SetChipPart(a0, a1, a2, a3, a4);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipPlayerGain(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SetChipPlayerGain(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipPlayerLoop(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Uint64 a1 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Uint64 a2 = (Uint64)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool a3 = (bool)GrappleGen_RubyToBool((argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SetChipPlayerLoop(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipPlayerStyle(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    const Grapple_ChipStyle *a1 = (const Grapple_ChipStyle *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_ChipStyle");
    bool rv = Grapple_SetChipPlayerStyle(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipPlayerTempo(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    double a1 = (double)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SetChipPlayerTempo(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipPresetEffects(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ChipPreset a1 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipEffects tmp2;
    const Grapple_ChipEffects *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ChipEffects(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_SetChipPresetEffects(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipRoleControls(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_ChipPreset a1 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipRoleControls tmp2;
    const Grapple_ChipRoleControls *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ChipRoleControls(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_SetChipRoleControls(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipTrackEffects(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipEffects tmp2;
    const Grapple_ChipEffects *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ChipEffects(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_SetChipTrackEffects(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipTrackMix(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipTrackMix tmp2;
    const Grapple_ChipTrackMix *a2 = NULL;
    if (argc > 2 && mrb_hash_p(argv[2])) {
        GenRead_Grapple_ChipTrackMix(mrb, argv[2], &tmp2);
        a2 = &tmp2;
    }
    bool rv = Grapple_SetChipTrackMix(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetChipTrackPreset(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_ChipPreset a2 = (Grapple_ChipPreset)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SetChipTrackPreset(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetDebugTextSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    float a0 = (float)GrappleGen_RubyToNum(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    Grapple_SetDebugTextSize(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetDeviceMotion(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SetDeviceMotion(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetDirectionRepeat(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_SetDirectionRepeat(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetGamepadDeadzone(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetGamepadDeadzone(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetGamepadMotion(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool a2 = (bool)GrappleGen_RubyToBool((argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_SetGamepadMotion(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetLaunchSettings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    Grapple_SetLaunchSettings(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightAmbient(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    SDL_FColor a1;
    GenRead_SDL_FColor(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    Grapple_SetLightAmbient(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightDebugDraw(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightDebugDraw(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightMapScale(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightMapScale(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightRayCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightRayCount(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightRings(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightRings(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightShadowSoftness(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightShadowSoftness(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetLightUseShaders(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_LightScene *a0 = (Grapple_LightScene *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_LightScene");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetLightUseShaders(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetMouseCapture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SetMouseCapture(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SetTextInput(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetTextInput(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SetTriggerThreshold(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SetTriggerThreshold(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SettingChoices(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_SettingChoices(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int rv = Grapple_SettingCount();
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_SettingKey(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_SettingKey(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingOption(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    int a0 = (int)GrappleGen_RubyToInt(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_SettingOption(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingPolicy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const char *a0 = GrappleGen_RubyToStr(mrb, (argc > 0 ? argv[0] : mrb_nil_value()));
    const char * rv = Grapple_SettingPolicy(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingsApply(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    Grapple_EngineConfig *a1 = (Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_GraphicsSettings out2;
    memset(&out2, 0, sizeof(out2));
    bool a3 = (bool)GrappleGen_RubyToBool((argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_SettingsApply(a0, a1, &out2, a3);
    mrb_value rets[2];
    rets[0] = mrb_bool_value((mrb_bool)(rv != 0));
    rets[1] = GenPush_Grapple_GraphicsSettings(mrb, &out2);
    return mrb_ary_new_from_values(mrb, 2, rets);
    }
}

static mrb_value GenR_Grapple_SettingsCapture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_EngineConfig *a0 = (const Grapple_EngineConfig *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_EngineConfig");
    Grapple_GraphicsSettings tmp1;
    const Grapple_GraphicsSettings *a1 = NULL;
    if (argc > 1 && mrb_hash_p(argv[1])) {
        GenRead_Grapple_GraphicsSettings(mrb, argv[1], &tmp1);
        a1 = &tmp1;
    }
    Grapple_Settings * rv = Grapple_SettingsCapture(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Settings");
    }
}

static mrb_value GenR_Grapple_SettingsGet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_SettingsGet(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingsLoadFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SettingsLoadFile(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsLoadToml(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_SettingsLoadToml(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsOverlay(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const Grapple_Settings *a1 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "Grapple_Settings");
    bool rv = Grapple_SettingsOverlay(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsPlayerPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char * rv = Grapple_SettingsPlayerPath(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingsQuality(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_SettingsQuality(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsSaveChanges(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SettingsSaveChanges(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsSet(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_SettingsSet(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsSetPlayerPath(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Settings *a0 = (Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_SettingsSetPlayerPath(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SettingsSource(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_SettingsSource(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_SettingsToToml(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_Settings *a0 = (const Grapple_Settings *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Settings");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    char * rv = Grapple_SettingsToToml(a0, a1);
    mrb_value rstr = rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv);
    if (rv != NULL) { SDL_free(rv); }
    return rstr;
    }
}

static mrb_value GenR_Grapple_ShowOpenFileDialog(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Window *a0 = (SDL_Window *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Window");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ShowOpenFileDialog(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_ShowSaveFileDialog(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    SDL_Window *a0 = (SDL_Window *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "SDL_Window");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    const char *a3 = GrappleGen_RubyToStr(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    bool rv = Grapple_ShowSaveFileDialog(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_SpriteCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite * rv = Grapple_SpriteCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "Grapple_Sprite");
    }
}

static mrb_value GenR_Grapple_SpriteDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    Grapple_SpriteDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetColor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_SpriteSetColor(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetLayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_SpriteSetLayer(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetOrigin(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_SpriteSetOrigin(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetScreenSpace(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SpriteSetScreenSpace(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetSize(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_SpriteSetSize(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetSortByY(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SpriteSetSortByY(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetSource(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_SpriteSetSource(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetTexture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    SDL_Texture *a1 = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), "SDL_Texture");
    Grapple_SpriteSetTexture(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_SpriteSetVisible(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Sprite *a0 = (Grapple_Sprite *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Sprite");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_SpriteSetVisible(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_StopChipPlayer(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_ChipPlayer *a0 = (Grapple_ChipPlayer *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_ChipPlayer");
    Grapple_StopChipPlayer(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_Text(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_Text(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_TextCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_TextCount(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TextHas(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_TextHas(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_TextLanguage(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char * rv = Grapple_TextLanguage(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_TextLoad(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char *a2 = GrappleGen_RubyToStr(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    bool rv = Grapple_TextLoad(a0, a1, a2);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_TextLoadFile(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    bool rv = Grapple_TextLoadFile(a0, a1);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_TextSetLanguage(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char *a1 = GrappleGen_RubyToStr(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    Grapple_TextSetLanguage(a0, a1);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_TextTyped(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    const char * rv = Grapple_TextTyped(a0);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_Texture(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    Grapple_TextureId a1 = (Grapple_TextureId)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    SDL_Texture * rv = Grapple_Texture(a0, a1);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "SDL_Texture");
    }
}

static mrb_value GenR_Grapple_TiledLayerCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int rv = Grapple_TiledLayerCount(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledLayerName(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_TiledLayerName(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_TiledLayerType(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    const char * rv = Grapple_TiledLayerType(a0, a1);
    return (rv == NULL ? mrb_nil_value() : mrb_str_new_cstr(mrb, rv));
    }
}

static mrb_value GenR_Grapple_TiledMapHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int rv = Grapple_TiledMapHeight(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledMapWidth(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int rv = Grapple_TiledMapWidth(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledObjectAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_TiledObject *a3 = (Grapple_TiledObject *)GrappleGen_RubyCheckHandle(mrb, (argc > 3 ? argv[3] : mrb_nil_value()), "Grapple_TiledObject");
    bool rv = Grapple_TiledObjectAt(a0, a1, a2, a3);
    return mrb_bool_value((mrb_bool)(rv != 0));
    }
}

static mrb_value GenR_Grapple_TiledObjectCount(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int rv = Grapple_TiledObjectCount(a0, a1);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledRaw(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_TiledMap *a0 = (Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    cute_tiled_map_t * rv = Grapple_TiledRaw(a0);
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "cute_tiled_map_t");
    }
}

static mrb_value GenR_Grapple_TiledTileAt(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int a1 = (int)GrappleGen_RubyToInt(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    int a2 = (int)GrappleGen_RubyToInt(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    int a3 = (int)GrappleGen_RubyToInt(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    int rv = Grapple_TiledTileAt(a0, a1, a2, a3);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledTileHeight(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int rv = Grapple_TiledTileHeight(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TiledTileWidth(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    const Grapple_TiledMap *a0 = (const Grapple_TiledMap *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_TiledMap");
    int rv = Grapple_TiledTileWidth(a0);
    return mrb_int_value(mrb, (mrb_int)rv);
    }
}

static mrb_value GenR_Grapple_TouchPinch(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_TouchPinch(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_TouchRotation(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    Grapple_Engine *a0 = (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "Grapple_Engine");
    float rv = Grapple_TouchRotation(a0);
    return mrb_float_value(mrb, (mrb_float)rv);
    }
}

static mrb_value GenR_Grapple_WeldJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WeldJointDef * rv = Grapple_WeldJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2WeldJointDef");
    }
}

static mrb_value GenR_Grapple_WeldJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WeldJointDef");
    Grapple_WeldJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WeldJointDefSetAnchors(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WeldJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_WeldJointDefSetAnchors(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WeldJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WeldJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_WeldJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WeldJointDefSetSpring(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WeldJointDef *a0 = (b2WeldJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WeldJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_WeldJointDefSetSpring(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefCreate(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef * rv = Grapple_WheelJointDefCreate();
    return GrappleGen_RubyPushHandle(mrb, (void *)rv, "b2WheelJointDef");
    }
}

static mrb_value GenR_Grapple_WheelJointDefDestroy(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    Grapple_WheelJointDefDestroy(a0);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefSetAnchors(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    float a4 = (float)GrappleGen_RubyToNum(mrb, (argc > 4 ? argv[4] : mrb_nil_value()));
    Grapple_WheelJointDefSetAnchors(a0, a1, a2, a3, a4);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefSetAxis(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    float a1 = (float)GrappleGen_RubyToNum(mrb, (argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    Grapple_WheelJointDefSetAxis(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefSetBodies(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    b2BodyId a1;
    GenRead_b2BodyId(mrb, (argc > 1 ? argv[1] : mrb_nil_value()), &a1);
    b2BodyId a2;
    GenRead_b2BodyId(mrb, (argc > 2 ? argv[2] : mrb_nil_value()), &a2);
    Grapple_WheelJointDefSetBodies(a0, a1, a2);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefSetMotor(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_WheelJointDefSetMotor(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

static mrb_value GenR_Grapple_WheelJointDefSetSpring(mrb_state *mrb, mrb_value self)
{
    const mrb_value *argv = NULL;
    mrb_int argc = 0;
    (void)self;
    mrb_get_args(mrb, "*", &argv, &argc);
    {
    b2WheelJointDef *a0 = (b2WheelJointDef *)GrappleGen_RubyCheckHandle(mrb, (argc > 0 ? argv[0] : mrb_nil_value()), "b2WheelJointDef");
    bool a1 = (bool)GrappleGen_RubyToBool((argc > 1 ? argv[1] : mrb_nil_value()));
    float a2 = (float)GrappleGen_RubyToNum(mrb, (argc > 2 ? argv[2] : mrb_nil_value()));
    float a3 = (float)GrappleGen_RubyToNum(mrb, (argc > 3 ? argv[3] : mrb_nil_value()));
    Grapple_WheelJointDefSetSpring(a0, a1, a2, a3);
    return mrb_nil_value();
    }
}

void GrappleGen_OpenRuby_grapple(mrb_state *mrb);
void GrappleGen_OpenRuby_grapple(mrb_state *mrb)
{
    struct RClass *mod;
    GrappleGen_RubyEnsureHandleClass(mrb);
    mod = mrb_define_module(mrb, "GrappleC");
    mrb_define_module_function(mrb, mod, "ActionBind", GenR_Grapple_ActionBind, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindAxis", GenR_Grapple_ActionBindAxis, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindDirection", GenR_Grapple_ActionBindDirection, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindKey", GenR_Grapple_ActionBindKey, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindKeySigned", GenR_Grapple_ActionBindKeySigned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindMouse", GenR_Grapple_ActionBindMouse, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindPad", GenR_Grapple_ActionBindPad, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindingAt", GenR_Grapple_ActionBindingAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionBindingCount", GenR_Grapple_ActionBindingCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionCapture", GenR_Grapple_ActionCapture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionClear", GenR_Grapple_ActionClear, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionCount", GenR_Grapple_ActionCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionDown", GenR_Grapple_ActionDown, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapCreate", GenR_Grapple_ActionMapCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapDestroy", GenR_Grapple_ActionMapDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapKeyboardPlayer", GenR_Grapple_ActionMapKeyboardPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapLoad", GenR_Grapple_ActionMapLoad, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapLoadToml", GenR_Grapple_ActionMapLoadToml, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapSave", GenR_Grapple_ActionMapSave, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapSetKeyboardPlayer", GenR_Grapple_ActionMapSetKeyboardPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionMapToToml", GenR_Grapple_ActionMapToToml, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionName", GenR_Grapple_ActionName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionPressed", GenR_Grapple_ActionPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionReleased", GenR_Grapple_ActionReleased, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionValue", GenR_Grapple_ActionValue, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActionVector", GenR_Grapple_ActionVector, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorAddBody", GenR_Grapple_ActorAddBody, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorAddLight", GenR_Grapple_ActorAddLight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorAlive", GenR_Grapple_ActorAlive, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorAngularVelocity", GenR_Grapple_ActorAngularVelocity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorApplyForce", GenR_Grapple_ActorApplyForce, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorApplyImpulse", GenR_Grapple_ActorApplyImpulse, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorBodyBounds", GenR_Grapple_ActorBodyBounds, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorBroadcast", GenR_Grapple_ActorBroadcast, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorChild", GenR_Grapple_ActorChild, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorChildCount", GenR_Grapple_ActorChildCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorClear", GenR_Grapple_ActorClear, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorClearSprite", GenR_Grapple_ActorClearSprite, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorCount", GenR_Grapple_ActorCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefCreate", GenR_Grapple_ActorDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefDestroy", GenR_Grapple_ActorDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetName", GenR_Grapple_ActorDefSetName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetParent", GenR_Grapple_ActorDefSetParent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetPosition", GenR_Grapple_ActorDefSetPosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetRotation", GenR_Grapple_ActorDefSetRotation, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetScale", GenR_Grapple_ActorDefSetScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetStateSize", GenR_Grapple_ActorDefSetStateSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetTags", GenR_Grapple_ActorDefSetTags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDefSetType", GenR_Grapple_ActorDefSetType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorDestroy", GenR_Grapple_ActorDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorEnabled", GenR_Grapple_ActorEnabled, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorEngine", GenR_Grapple_ActorEngine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorFindByName", GenR_Grapple_ActorFindByName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorFindByType", GenR_Grapple_ActorFindByType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorGet", GenR_Grapple_ActorGet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorGetId", GenR_Grapple_ActorGetId, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorGetTags", GenR_Grapple_ActorGetTags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorHasBody", GenR_Grapple_ActorHasBody, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorHasTags", GenR_Grapple_ActorHasTags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorLocal", GenR_Grapple_ActorLocal, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorMove", GenR_Grapple_ActorMove, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorName", GenR_Grapple_ActorName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorParent", GenR_Grapple_ActorParent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorQuery", GenR_Grapple_ActorQuery, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorRemoveBody", GenR_Grapple_ActorRemoveBody, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorRemoveLight", GenR_Grapple_ActorRemoveLight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorRenderTransform", GenR_Grapple_ActorRenderTransform, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSend", GenR_Grapple_ActorSend, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetAngularVelocity", GenR_Grapple_ActorSetAngularVelocity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetEnabled", GenR_Grapple_ActorSetEnabled, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetLocal", GenR_Grapple_ActorSetLocal, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetParent", GenR_Grapple_ActorSetParent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetPosition", GenR_Grapple_ActorSetPosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetSprite", GenR_Grapple_ActorSetSprite, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetTags", GenR_Grapple_ActorSetTags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSetVelocity", GenR_Grapple_ActorSetVelocity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSpawn", GenR_Grapple_ActorSpawn, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorSprite", GenR_Grapple_ActorSprite, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorTeleport", GenR_Grapple_ActorTeleport, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorTeleportBody", GenR_Grapple_ActorTeleportBody, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorType", GenR_Grapple_ActorType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorVelocity", GenR_Grapple_ActorVelocity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorWakeBody", GenR_Grapple_ActorWakeBody, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ActorWorld", GenR_Grapple_ActorWorld, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipControl", GenR_Grapple_AddChipControl, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipNote", GenR_Grapple_AddChipNote, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipNoteEx", GenR_Grapple_AddChipNoteEx, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipSection", GenR_Grapple_AddChipSection, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipStyleSearchPath", GenR_Grapple_AddChipStyleSearchPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddChipTempo", GenR_Grapple_AddChipTempo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddDarkZone", GenR_Grapple_AddDarkZone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddLight", GenR_Grapple_AddLight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddOccluderRect", GenR_Grapple_AddOccluderRect, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AddOccluderSegment", GenR_Grapple_AddOccluderSegment, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AnyInput", GenR_Grapple_AnyInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ApplyAudioBus", GenR_Grapple_ApplyAudioBus, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetPath", GenR_Grapple_AssetPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetRelease", GenR_Grapple_AssetRelease, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetRetain", GenR_Grapple_AssetRetain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetStatusOf", GenR_Grapple_AssetStatusOf, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsFrameBudget", GenR_Grapple_AssetsFrameBudget, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsLoaded", GenR_Grapple_AssetsLoaded, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsPending", GenR_Grapple_AssetsPending, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsProgress", GenR_Grapple_AssetsProgress, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsReady", GenR_Grapple_AssetsReady, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsSetFrameBudget", GenR_Grapple_AssetsSetFrameBudget, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsSetWorkers", GenR_Grapple_AssetsSetWorkers, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AssetsWait", GenR_Grapple_AssetsWait, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AttachAudioBuses", GenR_Grapple_AttachAudioBuses, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AudioMuted", GenR_Grapple_AudioMuted, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BidiBaseIsRTL", GenR_Grapple_BidiBaseIsRTL, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BindingFromString", GenR_Grapple_BindingFromString, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BindingToString", GenR_Grapple_BindingToString, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefDestroy", GenR_Grapple_BodyDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetBullet", GenR_Grapple_BodyDefSetBullet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetFilter", GenR_Grapple_BodyDefSetFilter, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetFixedRotation", GenR_Grapple_BodyDefSetFixedRotation, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetGravityScale", GenR_Grapple_BodyDefSetGravityScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetMaterial", GenR_Grapple_BodyDefSetMaterial, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetOffset", GenR_Grapple_BodyDefSetOffset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetSensor", GenR_Grapple_BodyDefSetSensor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetShape", GenR_Grapple_BodyDefSetShape, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetSize", GenR_Grapple_BodyDefSetSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefSetType", GenR_Grapple_BodyDefSetType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BodyDefault", GenR_Grapple_BodyDefault, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "BuildChipSong", GenR_Grapple_BuildChipSong, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraBegin", GenR_Grapple_CameraBegin, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraDestroy", GenR_Grapple_CameraDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraEnd", GenR_Grapple_CameraEnd, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraFollow", GenR_Grapple_CameraFollow, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraInit", GenR_Grapple_CameraInit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraPoint", GenR_Grapple_CameraPoint, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraRect", GenR_Grapple_CameraRect, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraScreenToWorld", GenR_Grapple_CameraScreenToWorld, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSetBounds", GenR_Grapple_CameraSetBounds, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSetDeadzone", GenR_Grapple_CameraSetDeadzone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSetSmoothing", GenR_Grapple_CameraSetSmoothing, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSetViewport", GenR_Grapple_CameraSetViewport, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSetZoom", GenR_Grapple_CameraSetZoom, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraShake", GenR_Grapple_CameraShake, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSnap", GenR_Grapple_CameraSnap, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraSplit", GenR_Grapple_CameraSplit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraUpdate", GenR_Grapple_CameraUpdate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraVisible", GenR_Grapple_CameraVisible, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraX", GenR_Grapple_CameraX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CameraY", GenR_Grapple_CameraY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ChipPlayerPlaying", GenR_Grapple_ChipPlayerPlaying, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CloneSettings", GenR_Grapple_CloneSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CompileRegex", GenR_Grapple_CompileRegex, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigCreate", GenR_Grapple_ConfigCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigDestroy", GenR_Grapple_ConfigDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetAutoMount", GenR_Grapple_ConfigSetAutoMount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetBackend", GenR_Grapple_ConfigSetBackend, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetDesignSize", GenR_Grapple_ConfigSetDesignSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetFullscreen", GenR_Grapple_ConfigSetFullscreen, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetGraphics", GenR_Grapple_ConfigSetGraphics, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetHeadless", GenR_Grapple_ConfigSetHeadless, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetHighDpi", GenR_Grapple_ConfigSetHighDpi, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetManualClock", GenR_Grapple_ConfigSetManualClock, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetMaxFps", GenR_Grapple_ConfigSetMaxFps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetMediaPath", GenR_Grapple_ConfigSetMediaPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetPresentation", GenR_Grapple_ConfigSetPresentation, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetRendererBackend", GenR_Grapple_ConfigSetRendererBackend, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetResizable", GenR_Grapple_ConfigSetResizable, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetTickRate", GenR_Grapple_ConfigSetTickRate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetTitle", GenR_Grapple_ConfigSetTitle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetVsync", GenR_Grapple_ConfigSetVsync, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ConfigSetWindowSize", GenR_Grapple_ConfigSetWindowSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CountSignalConnections", GenR_Grapple_CountSignalConnections, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateAudioMixer", GenR_Grapple_CreateAudioMixer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateBackendRenderer", GenR_Grapple_CreateBackendRenderer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateChipComposer", GenR_Grapple_CreateChipComposer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateChipPlayer", GenR_Grapple_CreateChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateChipSFX", GenR_Grapple_CreateChipSFX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateChipTone", GenR_Grapple_CreateChipTone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateChipTune", GenR_Grapple_CreateChipTune, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateEngine", GenR_Grapple_CreateEngine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateGui", GenR_Grapple_CreateGui, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateGuiWithGlyphs", GenR_Grapple_CreateGuiWithGlyphs, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateLightScene", GenR_Grapple_CreateLightScene, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreatePlatformer", GenR_Grapple_CreatePlatformer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateSettings", GenR_Grapple_CreateSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CreateSignalEmitter", GenR_Grapple_CreateSignalEmitter, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "CryptoSelfTest", GenR_Grapple_CryptoSelfTest, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DayNightAmbient", GenR_Grapple_DayNightAmbient, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DayNightSunlight", GenR_Grapple_DayNightSunlight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DescribeRenderBackend", GenR_Grapple_DescribeRenderBackend, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyChipComposer", GenR_Grapple_DestroyChipComposer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyChipPlayer", GenR_Grapple_DestroyChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyChipSong", GenR_Grapple_DestroyChipSong, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyChipStyle", GenR_Grapple_DestroyChipStyle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyEngine", GenR_Grapple_DestroyEngine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyGui", GenR_Grapple_DestroyGui, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyLightScene", GenR_Grapple_DestroyLightScene, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyPlatformer", GenR_Grapple_DestroyPlatformer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroyRegex", GenR_Grapple_DestroyRegex, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroySettings", GenR_Grapple_DestroySettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DestroySignalEmitter", GenR_Grapple_DestroySignalEmitter, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DeviceAccelerometer", GenR_Grapple_DeviceAccelerometer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DeviceGyro", GenR_Grapple_DeviceGyro, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DialogDeliverSave", GenR_Grapple_DialogDeliverSave, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DialogPath", GenR_Grapple_DialogPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DialogReset", GenR_Grapple_DialogReset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DialogStatus", GenR_Grapple_DialogStatus, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DisconnectSignal", GenR_Grapple_DisconnectSignal, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefCreate", GenR_Grapple_DistanceJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefDestroy", GenR_Grapple_DistanceJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetAnchors", GenR_Grapple_DistanceJointDefSetAnchors, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetBodies", GenR_Grapple_DistanceJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetCollideConnected", GenR_Grapple_DistanceJointDefSetCollideConnected, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetLength", GenR_Grapple_DistanceJointDefSetLength, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetLimit", GenR_Grapple_DistanceJointDefSetLimit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DistanceJointDefSetSpring", GenR_Grapple_DistanceJointDefSetSpring, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "DrawPhysicsWorld", GenR_Grapple_DrawPhysicsWorld, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EncodeDataBase64", GenR_Grapple_EncodeDataBase64, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineActualSettings", GenR_Grapple_EngineActualSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineAdvance", GenR_Grapple_EngineAdvance, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineAlpha", GenR_Grapple_EngineAlpha, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineAssetScale", GenR_Grapple_EngineAssetScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDelta", GenR_Grapple_EngineDelta, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDesignExplicit", GenR_Grapple_EngineDesignExplicit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDesignSize", GenR_Grapple_EngineDesignSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDisplay", GenR_Grapple_EngineDisplay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDisplayCount", GenR_Grapple_EngineDisplayCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineDisplayName", GenR_Grapple_EngineDisplayName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineEffectsAvailable", GenR_Grapple_EngineEffectsAvailable, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineEmbedMedia", GenR_Grapple_EngineEmbedMedia, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineFps", GenR_Grapple_EngineFps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineFrameCount", GenR_Grapple_EngineFrameCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineFrameScale", GenR_Grapple_EngineFrameScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineMaxFps", GenR_Grapple_EngineMaxFps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineMediaPath", GenR_Grapple_EngineMediaPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineMediaSource", GenR_Grapple_EngineMediaSource, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineOverloadFrames", GenR_Grapple_EngineOverloadFrames, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EnginePixelSize", GenR_Grapple_EnginePixelSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EnginePresentation_", GenR_Grapple_EnginePresentation_, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineQuit", GenR_Grapple_EngineQuit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineRenderScale", GenR_Grapple_EngineRenderScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineRenderer", GenR_Grapple_EngineRenderer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineRequestedSettings", GenR_Grapple_EngineRequestedSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSafeRect", GenR_Grapple_EngineSafeRect, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetClearColor", GenR_Grapple_EngineSetClearColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetDesignSize", GenR_Grapple_EngineSetDesignSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetDisplay", GenR_Grapple_EngineSetDisplay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetGraphics", GenR_Grapple_EngineSetGraphics, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetMaxFps", GenR_Grapple_EngineSetMaxFps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetMediaPassword", GenR_Grapple_EngineSetMediaPassword, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetPresentation", GenR_Grapple_EngineSetPresentation, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetRefreshRate", GenR_Grapple_EngineSetRefreshRate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetTickRate", GenR_Grapple_EngineSetTickRate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineSetTimeScale", GenR_Grapple_EngineSetTimeScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineShowWindow", GenR_Grapple_EngineShowWindow, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineStep", GenR_Grapple_EngineStep, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineStepsLastFrame", GenR_Grapple_EngineStepsLastFrame, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineTick", GenR_Grapple_EngineTick, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineTickRate", GenR_Grapple_EngineTickRate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineTimeScale", GenR_Grapple_EngineTimeScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineUiPoints", GenR_Grapple_EngineUiPoints, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineViewRect", GenR_Grapple_EngineViewRect, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineWindow", GenR_Grapple_EngineWindow, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EngineWindowToDesign", GenR_Grapple_EngineWindowToDesign, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventCreate", GenR_Grapple_EventCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventDestroy", GenR_Grapple_EventDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventGamepadAxis", GenR_Grapple_EventGamepadAxis, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventGamepadAxisValue", GenR_Grapple_EventGamepadAxisValue, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventGamepadButton", GenR_Grapple_EventGamepadButton, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventGamepadWhich", GenR_Grapple_EventGamepadWhich, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventKeyModifiers", GenR_Grapple_EventKeyModifiers, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventKeyRepeat", GenR_Grapple_EventKeyRepeat, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventKeyScancode", GenR_Grapple_EventKeyScancode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventMouseButton", GenR_Grapple_EventMouseButton, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventMouseDeltaX", GenR_Grapple_EventMouseDeltaX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventMouseDeltaY", GenR_Grapple_EventMouseDeltaY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventMouseX", GenR_Grapple_EventMouseX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventMouseY", GenR_Grapple_EventMouseY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventSetType", GenR_Grapple_EventSetType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventText", GenR_Grapple_EventText, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventTouchX", GenR_Grapple_EventTouchX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventTouchY", GenR_Grapple_EventTouchY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventType", GenR_Grapple_EventType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventWheelX", GenR_Grapple_EventWheelX, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventWheelY", GenR_Grapple_EventWheelY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "EventWindowId", GenR_Grapple_EventWindowId, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "FilterJointDefCreate", GenR_Grapple_FilterJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "FilterJointDefDestroy", GenR_Grapple_FilterJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "FilterJointDefSetBodies", GenR_Grapple_FilterJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "FingerCount", GenR_Grapple_FingerCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "FreeTiledMap", GenR_Grapple_FreeTiledMap, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUAcquireSwapchain", GenR_Grapple_GPUAcquireSwapchain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBeginComputePass", GenR_Grapple_GPUBeginComputePass, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindComputeStorageBuffer", GenR_Grapple_GPUBindComputeStorageBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindComputeStorageTexture", GenR_Grapple_GPUBindComputeStorageTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindFragmentStorageBuffer", GenR_Grapple_GPUBindFragmentStorageBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindFragmentStorageTexture", GenR_Grapple_GPUBindFragmentStorageTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindVertexStorageBuffer", GenR_Grapple_GPUBindVertexStorageBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBindVertexStorageTexture", GenR_Grapple_GPUBindVertexStorageTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBlitInfoCreate", GenR_Grapple_GPUBlitInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBlitInfoDestroy", GenR_Grapple_GPUBlitInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBlitInfoSetDestination", GenR_Grapple_GPUBlitInfoSetDestination, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBlitInfoSetFilter", GenR_Grapple_GPUBlitInfoSetFilter, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBlitInfoSetSource", GenR_Grapple_GPUBlitInfoSetSource, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferBindingCreate", GenR_Grapple_GPUBufferBindingCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferBindingDestroy", GenR_Grapple_GPUBufferBindingDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferBindingSet", GenR_Grapple_GPUBufferBindingSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferLocationCreate", GenR_Grapple_GPUBufferLocationCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferLocationDestroy", GenR_Grapple_GPUBufferLocationDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferLocationSet", GenR_Grapple_GPUBufferLocationSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferRegionCreate", GenR_Grapple_GPUBufferRegionCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferRegionDestroy", GenR_Grapple_GPUBufferRegionDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUBufferRegionSet", GenR_Grapple_GPUBufferRegionSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoCreate", GenR_Grapple_GPUColorTargetInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoDestroy", GenR_Grapple_GPUColorTargetInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoSetClearColor", GenR_Grapple_GPUColorTargetInfoSetClearColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoSetCycle", GenR_Grapple_GPUColorTargetInfoSetCycle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoSetMipLayer", GenR_Grapple_GPUColorTargetInfoSetMipLayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoSetOps", GenR_Grapple_GPUColorTargetInfoSetOps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUColorTargetInfoSetTexture", GenR_Grapple_GPUColorTargetInfoSetTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputeBindingsAddBuffer", GenR_Grapple_GPUComputeBindingsAddBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputeBindingsAddTexture", GenR_Grapple_GPUComputeBindingsAddTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputeBindingsCreate", GenR_Grapple_GPUComputeBindingsCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputeBindingsDestroy", GenR_Grapple_GPUComputeBindingsDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoCreate", GenR_Grapple_GPUComputePipelineInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoDestroy", GenR_Grapple_GPUComputePipelineInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoSetCode", GenR_Grapple_GPUComputePipelineInfoSetCode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoSetEntrypoint", GenR_Grapple_GPUComputePipelineInfoSetEntrypoint, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoSetFormat", GenR_Grapple_GPUComputePipelineInfoSetFormat, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUComputePipelineInfoSetThreadCount", GenR_Grapple_GPUComputePipelineInfoSetThreadCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUDepthStencilTargetInfoCreate", GenR_Grapple_GPUDepthStencilTargetInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUDepthStencilTargetInfoDestroy", GenR_Grapple_GPUDepthStencilTargetInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUDepthStencilTargetInfoSetClear", GenR_Grapple_GPUDepthStencilTargetInfoSetClear, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUDepthStencilTargetInfoSetOps", GenR_Grapple_GPUDepthStencilTargetInfoSetOps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUDepthStencilTargetInfoSetTexture", GenR_Grapple_GPUDepthStencilTargetInfoSetTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoAddColorTarget", GenR_Grapple_GPUPipelineInfoAddColorTarget, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoAddVertexAttribute", GenR_Grapple_GPUPipelineInfoAddVertexAttribute, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoAddVertexBuffer", GenR_Grapple_GPUPipelineInfoAddVertexBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoCreate", GenR_Grapple_GPUPipelineInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoDestroy", GenR_Grapple_GPUPipelineInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoSetDepthStencil", GenR_Grapple_GPUPipelineInfoSetDepthStencil, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoSetFillMode", GenR_Grapple_GPUPipelineInfoSetFillMode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoSetPrimitive", GenR_Grapple_GPUPipelineInfoSetPrimitive, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUPipelineInfoSetShaders", GenR_Grapple_GPUPipelineInfoSetShaders, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPURenderStateInfoAddSampler", GenR_Grapple_GPURenderStateInfoAddSampler, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPURenderStateInfoCreate", GenR_Grapple_GPURenderStateInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPURenderStateInfoDestroy", GenR_Grapple_GPURenderStateInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPURenderStateInfoSetShader", GenR_Grapple_GPURenderStateInfoSetShader, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoCreate", GenR_Grapple_GPUShaderCreateInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoDestroy", GenR_Grapple_GPUShaderCreateInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoSetCode", GenR_Grapple_GPUShaderCreateInfoSetCode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoSetCounts", GenR_Grapple_GPUShaderCreateInfoSetCounts, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoSetEntrypoint", GenR_Grapple_GPUShaderCreateInfoSetEntrypoint, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUShaderCreateInfoSetFormat", GenR_Grapple_GPUShaderCreateInfoSetFormat, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUSwapchainHeight", GenR_Grapple_GPUSwapchainHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUSwapchainWidth", GenR_Grapple_GPUSwapchainWidth, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureLocationCreate", GenR_Grapple_GPUTextureLocationCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureLocationDestroy", GenR_Grapple_GPUTextureLocationDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureLocationSet", GenR_Grapple_GPUTextureLocationSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureRegionCreate", GenR_Grapple_GPUTextureRegionCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureRegionDestroy", GenR_Grapple_GPUTextureRegionDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureRegionSet", GenR_Grapple_GPUTextureRegionSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureSamplerBindingCreate", GenR_Grapple_GPUTextureSamplerBindingCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureSamplerBindingDestroy", GenR_Grapple_GPUTextureSamplerBindingDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureSamplerBindingSet", GenR_Grapple_GPUTextureSamplerBindingSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureTransferInfoCreate", GenR_Grapple_GPUTextureTransferInfoCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureTransferInfoDestroy", GenR_Grapple_GPUTextureTransferInfoDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTextureTransferInfoSet", GenR_Grapple_GPUTextureTransferInfoSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTransferBufferLocationCreate", GenR_Grapple_GPUTransferBufferLocationCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTransferBufferLocationDestroy", GenR_Grapple_GPUTransferBufferLocationDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUTransferBufferLocationSet", GenR_Grapple_GPUTransferBufferLocationSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUUploadToTransferBuffer", GenR_Grapple_GPUUploadToTransferBuffer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUWaitAndAcquireSwapchain", GenR_Grapple_GPUWaitAndAcquireSwapchain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GPUWaitForFence", GenR_Grapple_GPUWaitForFence, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadAccelerometer", GenR_Grapple_GamepadAccelerometer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadAxisValue", GenR_Grapple_GamepadAxisValue, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadButtonDown", GenR_Grapple_GamepadButtonDown, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadButtonPressed", GenR_Grapple_GamepadButtonPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadButtonReleased", GenR_Grapple_GamepadButtonReleased, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadConnected", GenR_Grapple_GamepadConnected, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadCount", GenR_Grapple_GamepadCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadDeadzone", GenR_Grapple_GamepadDeadzone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadDirectionPressed", GenR_Grapple_GamepadDirectionPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadDirectionRepeat", GenR_Grapple_GamepadDirectionRepeat, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadGyro", GenR_Grapple_GamepadGyro, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadHasAccelerometer", GenR_Grapple_GamepadHasAccelerometer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadHasGyro", GenR_Grapple_GamepadHasGyro, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadName", GenR_Grapple_GamepadName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadRumble", GenR_Grapple_GamepadRumble, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadRumbleTriggers", GenR_Grapple_GamepadRumbleTriggers, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadSetLED", GenR_Grapple_GamepadSetLED, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadStick", GenR_Grapple_GamepadStick, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GamepadStopRumble", GenR_Grapple_GamepadStopRumble, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetAudioBusGain", GenR_Grapple_GetAudioBusGain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipDiagnosticCount", GenR_Grapple_GetChipDiagnosticCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipDiagnosticMessage", GenR_Grapple_GetChipDiagnosticMessage, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipExpressionDefaults", GenR_Grapple_GetChipExpressionDefaults, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipImportDefaults", GenR_Grapple_GetChipImportDefaults, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipPlayerPeakVoices", GenR_Grapple_GetChipPlayerPeakVoices, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipPlayerStyleInfo", GenR_Grapple_GetChipPlayerStyleInfo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipPresetEffects", GenR_Grapple_GetChipPresetEffects, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipSectionCount", GenR_Grapple_GetChipSectionCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipStyle", GenR_Grapple_GetChipStyle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetChipStyleCount", GenR_Grapple_GetChipStyleCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GetLaunchSettings", GenR_Grapple_GetLaunchSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsClamp", GenR_Grapple_GraphicsClamp, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsConfigError", GenR_Grapple_GraphicsConfigError, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsConfigPath", GenR_Grapple_GraphicsConfigPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsDefaults", GenR_Grapple_GraphicsDefaults, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsEqual", GenR_Grapple_GraphicsEqual, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsLightMapScale", GenR_Grapple_GraphicsLightMapScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsLoadTomlFile", GenR_Grapple_GraphicsLoadTomlFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsLoadTomlString", GenR_Grapple_GraphicsLoadTomlString, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsMaxDynamicLights", GenR_Grapple_GraphicsMaxDynamicLights, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsParticleDensity", GenR_Grapple_GraphicsParticleDensity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsQualityFromName", GenR_Grapple_GraphicsQualityFromName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsQualityName", GenR_Grapple_GraphicsQualityName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsSafeMode", GenR_Grapple_GraphicsSafeMode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsSave", GenR_Grapple_GraphicsSave, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsSavePath", GenR_Grapple_GraphicsSavePath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsShadowRays", GenR_Grapple_GraphicsShadowRays, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsShadowSoftness", GenR_Grapple_GraphicsShadowSoftness, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GraphicsToToml", GenR_Grapple_GraphicsToToml, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiContext", GenR_Grapple_GuiContext, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiDrawCommandCount", GenR_Grapple_GuiDrawCommandCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiDrawTexture", GenR_Grapple_GuiDrawTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiDrawTextureOverlay", GenR_Grapple_GuiDrawTextureOverlay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiFontHeight", GenR_Grapple_GuiFontHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridBeginOwned", GenR_Grapple_GuiGridBeginOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCell", GenR_Grapple_GuiGridCell, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCellOwned", GenR_Grapple_GuiGridCellOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCellPart", GenR_Grapple_GuiGridCellPart, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCellPartOwned", GenR_Grapple_GuiGridCellPartOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCellSpan", GenR_Grapple_GuiGridCellSpan, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCellSpanOwned", GenR_Grapple_GuiGridCellSpanOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridCreate", GenR_Grapple_GuiGridCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridDestroy", GenR_Grapple_GuiGridDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridEnd", GenR_Grapple_GuiGridEnd, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridEndOwned", GenR_Grapple_GuiGridEndOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridNextRow", GenR_Grapple_GuiGridNextRow, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridNextRowOwned", GenR_Grapple_GuiGridNextRowOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridRowHeight", GenR_Grapple_GuiGridRowHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridRowHeightOwned", GenR_Grapple_GuiGridRowHeightOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridSpacing", GenR_Grapple_GuiGridSpacing, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridSpacingOwned", GenR_Grapple_GuiGridSpacingOwned, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiGridWeight", GenR_Grapple_GuiGridWeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiImage", GenR_Grapple_GuiImage, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiImageButton", GenR_Grapple_GuiImageButton, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiInputBegin", GenR_Grapple_GuiInputBegin, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiInputEnd", GenR_Grapple_GuiInputEnd, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiKeyPressed", GenR_Grapple_GuiKeyPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiMemoryUsed", GenR_Grapple_GuiMemoryUsed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiOpenFileButton", GenR_Grapple_GuiOpenFileButton, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiPopFont", GenR_Grapple_GuiPopFont, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiPopStyleColor", GenR_Grapple_GuiPopStyleColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiProcessEvent", GenR_Grapple_GuiProcessEvent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiPumpEvents", GenR_Grapple_GuiPumpEvents, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiPushFont", GenR_Grapple_GuiPushFont, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiPushStyleColor", GenR_Grapple_GuiPushStyleColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiRender", GenR_Grapple_GuiRender, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiRenderer", GenR_Grapple_GuiRenderer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiSaveFileButton", GenR_Grapple_GuiSaveFileButton, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiSavedPath", GenR_Grapple_GuiSavedPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiScale", GenR_Grapple_GuiScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiSetFont", GenR_Grapple_GuiSetFont, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiSetTooltipDelay", GenR_Grapple_GuiSetTooltipDelay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiTooltip", GenR_Grapple_GuiTooltip, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiTooltipDelay", GenR_Grapple_GuiTooltipDelay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "GuiWantsInput", GenR_Grapple_GuiWantsInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "HMACSHA256", GenR_Grapple_HMACSHA256, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "HasDeviceMotion", GenR_Grapple_HasDeviceMotion, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "IdleSeconds", GenR_Grapple_IdleSeconds, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "KeyDown", GenR_Grapple_KeyDown, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "KeyModifiers", GenR_Grapple_KeyModifiers, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "KeyPressed", GenR_Grapple_KeyPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "KeyReleased", GenR_Grapple_KeyReleased, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LastInputDevice", GenR_Grapple_LastInputDevice, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightAddDarkZone", GenR_Grapple_LightAddDarkZone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightAddOccluder", GenR_Grapple_LightAddOccluder, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightAddOccluderLine", GenR_Grapple_LightAddOccluderLine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightAmbient", GenR_Grapple_LightAmbient, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightAt", GenR_Grapple_LightAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightBeginFrame", GenR_Grapple_LightBeginFrame, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightCount", GenR_Grapple_LightCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefDestroy", GenR_Grapple_LightDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetColor", GenR_Grapple_LightDefSetColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetCone", GenR_Grapple_LightDefSetCone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetFlicker", GenR_Grapple_LightDefSetFlicker, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetOffset", GenR_Grapple_LightDefSetOffset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetRadius", GenR_Grapple_LightDefSetRadius, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefSetShadows", GenR_Grapple_LightDefSetShadows, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightDefault", GenR_Grapple_LightDefault, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightHour", GenR_Grapple_LightHour, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightLineOfSight", GenR_Grapple_LightLineOfSight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightPreset_", GenR_Grapple_LightPreset_, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightRender", GenR_Grapple_LightRender, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightSetAmbient", GenR_Grapple_LightSetAmbient, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightSetAutoOccluders", GenR_Grapple_LightSetAutoOccluders, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightSetClock", GenR_Grapple_LightSetClock, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightSetPreset", GenR_Grapple_LightSetPreset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightSunlight", GenR_Grapple_LightSunlight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LightUsesShaders", GenR_Grapple_LightUsesShaders, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadChipSong", GenR_Grapple_LoadChipSong, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadChipSongEx", GenR_Grapple_LoadChipSongEx, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadChipStyle", GenR_Grapple_LoadChipStyle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadPlatformer", GenR_Grapple_LoadPlatformer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadTextFile", GenR_Grapple_LoadTextFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadTexture", GenR_Grapple_LoadTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadTextureAsync", GenR_Grapple_LoadTextureAsync, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "LoadTiledMap", GenR_Grapple_LoadTiledMap, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MotorJointDefCreate", GenR_Grapple_MotorJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MotorJointDefDestroy", GenR_Grapple_MotorJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MotorJointDefSetBodies", GenR_Grapple_MotorJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MountEncryptedArchive", GenR_Grapple_MountEncryptedArchive, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MountEncryptedArchiveFile", GenR_Grapple_MountEncryptedArchiveFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MountMedia", GenR_Grapple_MountMedia, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseCaptured", GenR_Grapple_MouseCaptured, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseDelta", GenR_Grapple_MouseDelta, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseDown", GenR_Grapple_MouseDown, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseJointDefCreate", GenR_Grapple_MouseJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseJointDefDestroy", GenR_Grapple_MouseJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseJointDefSetBodies", GenR_Grapple_MouseJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseJointDefSetMaxForce", GenR_Grapple_MouseJointDefSetMaxForce, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseJointDefSetSpring", GenR_Grapple_MouseJointDefSetSpring, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MousePosition", GenR_Grapple_MousePosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MousePressed", GenR_Grapple_MousePressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseReleased", GenR_Grapple_MouseReleased, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "MouseWheel", GenR_Grapple_MouseWheel, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OpenVFSRead", GenR_Grapple_OpenVFSRead, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PauseChipPlayer", GenR_Grapple_PauseChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsBodyCount", GenR_Grapple_PhysicsBodyCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsGravity", GenR_Grapple_PhysicsGravity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsOverlap", GenR_Grapple_PhysicsOverlap, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsPaused", GenR_Grapple_PhysicsPaused, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsPixelsPerMetre", GenR_Grapple_PhysicsPixelsPerMetre, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsRaycast", GenR_Grapple_PhysicsRaycast, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsSetGravity", GenR_Grapple_PhysicsSetGravity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsSetPaused", GenR_Grapple_PhysicsSetPaused, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsSetPixelsPerMetre", GenR_Grapple_PhysicsSetPixelsPerMetre, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PhysicsSetSubSteps", GenR_Grapple_PhysicsSetSubSteps, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerActionDown", GenR_Grapple_PlatformerActionDown, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerActionPressed", GenR_Grapple_PlatformerActionPressed, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerActionReleased", GenR_Grapple_PlatformerActionReleased, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerActionValue", GenR_Grapple_PlatformerActionValue, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerActions", GenR_Grapple_PlatformerActions, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerAddSolid", GenR_Grapple_PlatformerAddSolid, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerAttach", GenR_Grapple_PlatformerAttach, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerAttached", GenR_Grapple_PlatformerAttached, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerBind", GenR_Grapple_PlatformerBind, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCameraPosition", GenR_Grapple_PlatformerCameraPosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreateBlock", GenR_Grapple_PlatformerCreateBlock, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreateFloor", GenR_Grapple_PlatformerCreateFloor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreatePlatform", GenR_Grapple_PlatformerCreatePlatform, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreatePlayer", GenR_Grapple_PlatformerCreatePlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreateStairs", GenR_Grapple_PlatformerCreateStairs, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerCreateWall", GenR_Grapple_PlatformerCreateWall, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerDetach", GenR_Grapple_PlatformerDetach, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerEngine", GenR_Grapple_PlatformerEngine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerFillTiles", GenR_Grapple_PlatformerFillTiles, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerHasPlayer", GenR_Grapple_PlatformerHasPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerHeight", GenR_Grapple_PlatformerHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPixelSize", GenR_Grapple_PlatformerPixelSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayer", GenR_Grapple_PlatformerPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerBumped", GenR_Grapple_PlatformerPlayerBumped, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerFacing", GenR_Grapple_PlatformerPlayerFacing, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerFell", GenR_Grapple_PlatformerPlayerFell, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerGrounded", GenR_Grapple_PlatformerPlayerGrounded, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerJumped", GenR_Grapple_PlatformerPlayerJumped, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerLanded", GenR_Grapple_PlatformerPlayerLanded, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerPaused", GenR_Grapple_PlatformerPlayerPaused, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerPosition", GenR_Grapple_PlatformerPlayerPosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerRespawn", GenR_Grapple_PlatformerPlayerRespawn, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerRespawnAtStart", GenR_Grapple_PlatformerPlayerRespawnAtStart, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerSize", GenR_Grapple_PlatformerPlayerSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerState", GenR_Grapple_PlatformerPlayerState, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerStateChanged", GenR_Grapple_PlatformerPlayerStateChanged, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerStateName", GenR_Grapple_PlatformerPlayerStateName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerTuning", GenR_Grapple_PlatformerPlayerTuning, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerTuningCount", GenR_Grapple_PlatformerPlayerTuningCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerTuningKey", GenR_Grapple_PlatformerPlayerTuningKey, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerVelocity", GenR_Grapple_PlatformerPlayerVelocity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerPlayerWall", GenR_Grapple_PlatformerPlayerWall, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerRemovePlayer", GenR_Grapple_PlatformerRemovePlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerRemoveSolid", GenR_Grapple_PlatformerRemoveSolid, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerRender", GenR_Grapple_PlatformerRender, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerScriptInput", GenR_Grapple_PlatformerScriptInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerScriptedInput", GenR_Grapple_PlatformerScriptedInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerScrollMode", GenR_Grapple_PlatformerScrollMode, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetBackgroundColor", GenR_Grapple_PlatformerSetBackgroundColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetCameraDeadzone", GenR_Grapple_PlatformerSetCameraDeadzone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetCameraLookAhead", GenR_Grapple_PlatformerSetCameraLookAhead, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetCameraSmoothing", GenR_Grapple_PlatformerSetCameraSmoothing, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetPlayerColor", GenR_Grapple_PlatformerSetPlayerColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetPlayerPaused", GenR_Grapple_PlatformerSetPlayerPaused, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetPlayerSize", GenR_Grapple_PlatformerSetPlayerSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetPlayerTuning", GenR_Grapple_PlatformerSetPlayerTuning, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetScriptedInput", GenR_Grapple_PlatformerSetScriptedInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetScroll", GenR_Grapple_PlatformerSetScroll, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetTile", GenR_Grapple_PlatformerSetTile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetTileColor", GenR_Grapple_PlatformerSetTileColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSetTileTexture", GenR_Grapple_PlatformerSetTileTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerSolidCount", GenR_Grapple_PlatformerSolidCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerStep", GenR_Grapple_PlatformerStep, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerTileAt", GenR_Grapple_PlatformerTileAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerTileSize", GenR_Grapple_PlatformerTileSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerUpdate", GenR_Grapple_PlatformerUpdate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlatformerWidth", GenR_Grapple_PlatformerWidth, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlayChipFile", GenR_Grapple_PlayChipFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlayChipPlayer", GenR_Grapple_PlayChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlayChipSong", GenR_Grapple_PlayChipSong, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PlayerStateName", GenR_Grapple_PlayerStateName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefCreate", GenR_Grapple_PrismaticJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefDestroy", GenR_Grapple_PrismaticJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefSetAnchors", GenR_Grapple_PrismaticJointDefSetAnchors, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefSetAxis", GenR_Grapple_PrismaticJointDefSetAxis, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefSetBodies", GenR_Grapple_PrismaticJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefSetLimit", GenR_Grapple_PrismaticJointDefSetLimit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "PrismaticJointDefSetMotor", GenR_Grapple_PrismaticJointDefSetMotor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ProbeRenderBackend", GenR_Grapple_ProbeRenderBackend, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "QuitDebugText", GenR_Grapple_QuitDebugText, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipDiagnostic", GenR_Grapple_ReadChipDiagnostic, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipPlayerPosition", GenR_Grapple_ReadChipPlayerPosition, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipRoleControls", GenR_Grapple_ReadChipRoleControls, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipSection", GenR_Grapple_ReadChipSection, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipSongInfo", GenR_Grapple_ReadChipSongInfo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipStyleInfo", GenR_Grapple_ReadChipStyleInfo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipStyleInfoAt", GenR_Grapple_ReadChipStyleInfoAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipTrackEffects", GenR_Grapple_ReadChipTrackEffects, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipTrackMapping", GenR_Grapple_ReadChipTrackMapping, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ReadChipTrackMix", GenR_Grapple_ReadChipTrackMix, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexEscape", GenR_Grapple_RegexEscape, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexFlags", GenR_Grapple_RegexFlags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexGroup", GenR_Grapple_RegexGroup, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexGroupBegin", GenR_Grapple_RegexGroupBegin, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexGroupCount", GenR_Grapple_RegexGroupCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexGroupEnd", GenR_Grapple_RegexGroupEnd, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexMatchAt", GenR_Grapple_RegexMatchAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexNamedGroup", GenR_Grapple_RegexNamedGroup, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexNamedGroupCount", GenR_Grapple_RegexNamedGroupCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexNamedGroupName", GenR_Grapple_RegexNamedGroupName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexPattern", GenR_Grapple_RegexPattern, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexReplace", GenR_Grapple_RegexReplace, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegexSearch", GenR_Grapple_RegexSearch, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RegisterChipStyle", GenR_Grapple_RegisterChipStyle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderBackendCount", GenR_Grapple_RenderBackendCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderBackendName", GenR_Grapple_RenderBackendName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderBackendValid", GenR_Grapple_RenderBackendValid, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderDebugText", GenR_Grapple_RenderDebugText, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderLastStats", GenR_Grapple_RenderLastStats, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderLighting", GenR_Grapple_RenderLighting, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderOverlay", GenR_Grapple_RenderOverlay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RenderWorld", GenR_Grapple_RenderWorld, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ResetChipPlayer", GenR_Grapple_ResetChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefCreate", GenR_Grapple_RevoluteJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefDestroy", GenR_Grapple_RevoluteJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetAnchors", GenR_Grapple_RevoluteJointDefSetAnchors, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetBodies", GenR_Grapple_RevoluteJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetCollideConnected", GenR_Grapple_RevoluteJointDefSetCollideConnected, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetLimit", GenR_Grapple_RevoluteJointDefSetLimit, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetMotor", GenR_Grapple_RevoluteJointDefSetMotor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RevoluteJointDefSetSpring", GenR_Grapple_RevoluteJointDefSetSpring, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "RouteAudioTrack", GenR_Grapple_RouteAudioTrack, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SHA256", GenR_Grapple_SHA256, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SampleLight", GenR_Grapple_SampleLight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveChipSongWav", GenR_Grapple_SaveChipSongWav, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveDelete", GenR_Grapple_SaveDelete, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveExists", GenR_Grapple_SaveExists, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveInfoOf", GenR_Grapple_SaveInfoOf, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SavePath", GenR_Grapple_SavePath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveSetIdentity", GenR_Grapple_SaveSetIdentity, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SaveWrite", GenR_Grapple_SaveWrite, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneCurrent", GenR_Grapple_SceneCurrent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneDepth", GenR_Grapple_SceneDepth, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneEngine", GenR_Grapple_SceneEngine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneFind", GenR_Grapple_SceneFind, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneIsActive", GenR_Grapple_SceneIsActive, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneKey", GenR_Grapple_SceneKey, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneName", GenR_Grapple_SceneName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScenePending", GenR_Grapple_ScenePending, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScenePop", GenR_Grapple_ScenePop, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScenePush", GenR_Grapple_ScenePush, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneReplace", GenR_Grapple_SceneReplace, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneReset", GenR_Grapple_SceneReset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneSetTransitionColor", GenR_Grapple_SceneSetTransitionColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneTransitionTo", GenR_Grapple_SceneTransitionTo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SceneTransitioning", GenR_Grapple_SceneTransitioning, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptHasHandlers", GenR_Grapple_ScriptHasHandlers, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptRun", GenR_Grapple_ScriptRun, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneDefine", GenR_Grapple_ScriptSceneDefine, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneDefined", GenR_Grapple_ScriptSceneDefined, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptScenePush", GenR_Grapple_ScriptScenePush, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneReplace", GenR_Grapple_ScriptSceneReplace, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneReset", GenR_Grapple_ScriptSceneReset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneSetFlags", GenR_Grapple_ScriptSceneSetFlags, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneSetHook", GenR_Grapple_ScriptSceneSetHook, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneSetStateSize", GenR_Grapple_ScriptSceneSetStateSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSceneTransitionTo", GenR_Grapple_ScriptSceneTransitionTo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptSetHook", GenR_Grapple_ScriptSetHook, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ScriptUnbind", GenR_Grapple_ScriptUnbind, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SeekChipPlayer", GenR_Grapple_SeekChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetAudioBusGain", GenR_Grapple_SetAudioBusGain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetAudioMuted", GenR_Grapple_SetAudioMuted, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPart", GenR_Grapple_SetChipPart, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPlayerGain", GenR_Grapple_SetChipPlayerGain, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPlayerLoop", GenR_Grapple_SetChipPlayerLoop, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPlayerStyle", GenR_Grapple_SetChipPlayerStyle, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPlayerTempo", GenR_Grapple_SetChipPlayerTempo, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipPresetEffects", GenR_Grapple_SetChipPresetEffects, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipRoleControls", GenR_Grapple_SetChipRoleControls, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipTrackEffects", GenR_Grapple_SetChipTrackEffects, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipTrackMix", GenR_Grapple_SetChipTrackMix, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetChipTrackPreset", GenR_Grapple_SetChipTrackPreset, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetDebugTextSize", GenR_Grapple_SetDebugTextSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetDeviceMotion", GenR_Grapple_SetDeviceMotion, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetDirectionRepeat", GenR_Grapple_SetDirectionRepeat, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetGamepadDeadzone", GenR_Grapple_SetGamepadDeadzone, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetGamepadMotion", GenR_Grapple_SetGamepadMotion, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLaunchSettings", GenR_Grapple_SetLaunchSettings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightAmbient", GenR_Grapple_SetLightAmbient, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightDebugDraw", GenR_Grapple_SetLightDebugDraw, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightMapScale", GenR_Grapple_SetLightMapScale, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightRayCount", GenR_Grapple_SetLightRayCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightRings", GenR_Grapple_SetLightRings, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightShadowSoftness", GenR_Grapple_SetLightShadowSoftness, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetLightUseShaders", GenR_Grapple_SetLightUseShaders, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetMouseCapture", GenR_Grapple_SetMouseCapture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetTextInput", GenR_Grapple_SetTextInput, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SetTriggerThreshold", GenR_Grapple_SetTriggerThreshold, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingChoices", GenR_Grapple_SettingChoices, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingCount", GenR_Grapple_SettingCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingKey", GenR_Grapple_SettingKey, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingOption", GenR_Grapple_SettingOption, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingPolicy", GenR_Grapple_SettingPolicy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsApply", GenR_Grapple_SettingsApply, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsCapture", GenR_Grapple_SettingsCapture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsGet", GenR_Grapple_SettingsGet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsLoadFile", GenR_Grapple_SettingsLoadFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsLoadToml", GenR_Grapple_SettingsLoadToml, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsOverlay", GenR_Grapple_SettingsOverlay, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsPlayerPath", GenR_Grapple_SettingsPlayerPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsQuality", GenR_Grapple_SettingsQuality, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsSaveChanges", GenR_Grapple_SettingsSaveChanges, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsSet", GenR_Grapple_SettingsSet, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsSetPlayerPath", GenR_Grapple_SettingsSetPlayerPath, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsSource", GenR_Grapple_SettingsSource, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SettingsToToml", GenR_Grapple_SettingsToToml, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ShowOpenFileDialog", GenR_Grapple_ShowOpenFileDialog, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "ShowSaveFileDialog", GenR_Grapple_ShowSaveFileDialog, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteCreate", GenR_Grapple_SpriteCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteDestroy", GenR_Grapple_SpriteDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetColor", GenR_Grapple_SpriteSetColor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetLayer", GenR_Grapple_SpriteSetLayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetOrigin", GenR_Grapple_SpriteSetOrigin, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetScreenSpace", GenR_Grapple_SpriteSetScreenSpace, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetSize", GenR_Grapple_SpriteSetSize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetSortByY", GenR_Grapple_SpriteSetSortByY, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetSource", GenR_Grapple_SpriteSetSource, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetTexture", GenR_Grapple_SpriteSetTexture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "SpriteSetVisible", GenR_Grapple_SpriteSetVisible, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "StopChipPlayer", GenR_Grapple_StopChipPlayer, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "Text", GenR_Grapple_Text, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextCount", GenR_Grapple_TextCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextHas", GenR_Grapple_TextHas, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextLanguage", GenR_Grapple_TextLanguage, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextLoad", GenR_Grapple_TextLoad, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextLoadFile", GenR_Grapple_TextLoadFile, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextSetLanguage", GenR_Grapple_TextSetLanguage, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TextTyped", GenR_Grapple_TextTyped, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "Texture", GenR_Grapple_Texture, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledLayerCount", GenR_Grapple_TiledLayerCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledLayerName", GenR_Grapple_TiledLayerName, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledLayerType", GenR_Grapple_TiledLayerType, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledMapHeight", GenR_Grapple_TiledMapHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledMapWidth", GenR_Grapple_TiledMapWidth, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledObjectAt", GenR_Grapple_TiledObjectAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledObjectCount", GenR_Grapple_TiledObjectCount, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledRaw", GenR_Grapple_TiledRaw, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledTileAt", GenR_Grapple_TiledTileAt, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledTileHeight", GenR_Grapple_TiledTileHeight, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TiledTileWidth", GenR_Grapple_TiledTileWidth, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TouchPinch", GenR_Grapple_TouchPinch, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "TouchRotation", GenR_Grapple_TouchRotation, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WeldJointDefCreate", GenR_Grapple_WeldJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WeldJointDefDestroy", GenR_Grapple_WeldJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WeldJointDefSetAnchors", GenR_Grapple_WeldJointDefSetAnchors, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WeldJointDefSetBodies", GenR_Grapple_WeldJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WeldJointDefSetSpring", GenR_Grapple_WeldJointDefSetSpring, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefCreate", GenR_Grapple_WheelJointDefCreate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefDestroy", GenR_Grapple_WheelJointDefDestroy, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefSetAnchors", GenR_Grapple_WheelJointDefSetAnchors, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefSetAxis", GenR_Grapple_WheelJointDefSetAxis, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefSetBodies", GenR_Grapple_WheelJointDefSetBodies, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefSetMotor", GenR_Grapple_WheelJointDefSetMotor, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "WheelJointDefSetSpring", GenR_Grapple_WheelJointDefSetSpring, MRB_ARGS_ANY());
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_MISSING", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_MISSING));
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_QUEUED", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_QUEUED));
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_LOADING", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_LOADING));
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_DECODED", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_DECODED));
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_READY", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_READY));
    mrb_define_const(mrb, mod, "GRAPPLE_ASSET_FAILED", mrb_int_value(mrb, (mrb_int)GRAPPLE_ASSET_FAILED));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_MASTER", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_MASTER));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_MUSIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_MUSIC));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_SFX", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_SFX));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_SPEECH", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_SPEECH));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_AMBIENT", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_AMBIENT));
    mrb_define_const(mrb, mod, "GRAPPLE_AUDIO_BUS_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_AUDIO_BUS_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_KEY", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_KEY));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_MOUSE_BUTTON", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_MOUSE_BUTTON));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_MOUSE_WHEEL", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_MOUSE_WHEEL));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_PAD_BUTTON", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_PAD_BUTTON));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_PAD_AXIS", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_PAD_AXIS));
    mrb_define_const(mrb, mod, "GRAPPLE_BIND_PAD_DIRECTION", mrb_int_value(mrb, (mrb_int)GRAPPLE_BIND_PAD_DIRECTION));
    mrb_define_const(mrb, mod, "GRAPPLE_BODY_STATIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_BODY_STATIC));
    mrb_define_const(mrb, mod, "GRAPPLE_BODY_KINEMATIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_BODY_KINEMATIC));
    mrb_define_const(mrb, mod, "GRAPPLE_BODY_DYNAMIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_BODY_DYNAMIC));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_INPUT", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_INPUT));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_SCORE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_SCORE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_APPROXIMATION", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_APPROXIMATION));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_RESOURCE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_RESOURCE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_CONFLICT", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_CONFLICT));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_EXPORTER_OMISSION", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_EXPORTER_OMISSION));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_INFO", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_INFO));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_WARNING", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_WARNING));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_DIAGNOSTIC_ERROR", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_DIAGNOSTIC_ERROR));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_MAPPING_OVERRIDE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_MAPPING_OVERRIDE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_MAPPING_NAME", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_MAPPING_NAME));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_MAPPING_PERCUSSION", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_MAPPING_PERCUSSION));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_MAPPING_PROGRAM", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_MAPPING_PROGRAM));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_MAPPING_DEFAULT", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_MAPPING_DEFAULT));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_AUTO", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_AUTO));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_LEAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_LEAD));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_BASS", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_BASS));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_CHORD", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_CHORD));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_RING", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_RING));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_DRUMS", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_DRUMS));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_NOISE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_NOISE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_PRESET_HARMONY", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_PRESET_HARMONY));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_COIN", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_COIN));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_LASER", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_LASER));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_JUMP", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_JUMP));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_EXPLOSION", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_EXPLOSION));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_POWERUP", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_POWERUP));
    mrb_define_const(mrb, mod, "GRAPPLE_SFX_HURT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SFX_HURT));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_VOICE_OSCILLATOR", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_VOICE_OSCILLATOR));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_VOICE_FM", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_VOICE_FM));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_VOICE_SAMPLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_VOICE_SAMPLE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_SQUARE_125", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_SQUARE_125));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_SQUARE_25", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_SQUARE_25));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_SQUARE_50", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_SQUARE_50));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_TRIANGLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_TRIANGLE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_SAW", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_SAW));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_NOISE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_NOISE));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_NOISE_METALLIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_NOISE_METALLIC));
    mrb_define_const(mrb, mod, "GRAPPLE_CHIP_SINE", mrb_int_value(mrb, (mrb_int)GRAPPLE_CHIP_SINE));
    mrb_define_const(mrb, mod, "GRAPPLE_COLORBLIND_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_COLORBLIND_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_COLORBLIND_PROTANOPIA", mrb_int_value(mrb, (mrb_int)GRAPPLE_COLORBLIND_PROTANOPIA));
    mrb_define_const(mrb, mod, "GRAPPLE_COLORBLIND_DEUTERANOPIA", mrb_int_value(mrb, (mrb_int)GRAPPLE_COLORBLIND_DEUTERANOPIA));
    mrb_define_const(mrb, mod, "GRAPPLE_COLORBLIND_TRITANOPIA", mrb_int_value(mrb, (mrb_int)GRAPPLE_COLORBLIND_TRITANOPIA));
    mrb_define_const(mrb, mod, "GRAPPLE_DIALOG_IDLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIALOG_IDLE));
    mrb_define_const(mrb, mod, "GRAPPLE_DIALOG_PENDING", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIALOG_PENDING));
    mrb_define_const(mrb, mod, "GRAPPLE_DIALOG_ACCEPTED", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIALOG_ACCEPTED));
    mrb_define_const(mrb, mod, "GRAPPLE_DIALOG_CANCELLED", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIALOG_CANCELLED));
    mrb_define_const(mrb, mod, "GRAPPLE_DIALOG_ERROR", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIALOG_ERROR));
    mrb_define_const(mrb, mod, "GRAPPLE_DIR_UP", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIR_UP));
    mrb_define_const(mrb, mod, "GRAPPLE_DIR_DOWN", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIR_DOWN));
    mrb_define_const(mrb, mod, "GRAPPLE_DIR_LEFT", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIR_LEFT));
    mrb_define_const(mrb, mod, "GRAPPLE_DIR_RIGHT", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIR_RIGHT));
    mrb_define_const(mrb, mod, "GRAPPLE_DIR_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_DIR_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_BACKEND_OPENGL", mrb_int_value(mrb, (mrb_int)GRAPPLE_BACKEND_OPENGL));
    mrb_define_const(mrb, mod, "GRAPPLE_BACKEND_NATIVE", mrb_int_value(mrb, (mrb_int)GRAPPLE_BACKEND_NATIVE));
    mrb_define_const(mrb, mod, "GRAPPLE_BACKEND_SOFTWARE", mrb_int_value(mrb, (mrb_int)GRAPPLE_BACKEND_SOFTWARE));
    mrb_define_const(mrb, mod, "GRAPPLE_INTERPOLATE_LERP", mrb_int_value(mrb, (mrb_int)GRAPPLE_INTERPOLATE_LERP));
    mrb_define_const(mrb, mod, "GRAPPLE_INTERPOLATE_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_INTERPOLATE_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_INTERPOLATE_EXTRAPOLATE", mrb_int_value(mrb, (mrb_int)GRAPPLE_INTERPOLATE_EXTRAPOLATE));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_LETTERBOX", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_LETTERBOX));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_EXPAND", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_EXPAND));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_OVERSCAN", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_OVERSCAN));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_INTEGER", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_INTEGER));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_STRETCH", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_STRETCH));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_NATIVE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_NATIVE));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_PIXEL", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_PIXEL));
    mrb_define_const(mrb, mod, "GRAPPLE_PRESENT_PIXEL_SNAP", mrb_int_value(mrb, (mrb_int)GRAPPLE_PRESENT_PIXEL_SNAP));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_LEFT_X", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_LEFT_X));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_LEFT_Y", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_LEFT_Y));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_RIGHT_X", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_RIGHT_X));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_RIGHT_Y", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_RIGHT_Y));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_LEFT_TRIGGER", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_LEFT_TRIGGER));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_RIGHT_TRIGGER", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_RIGHT_TRIGGER));
    mrb_define_const(mrb, mod, "GRAPPLE_AXIS_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_AXIS_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_A", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_A));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_B", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_B));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_X", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_X));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_Y", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_Y));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_BACK", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_BACK));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_GUIDE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_GUIDE));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_START", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_START));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_LEFT_STICK", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_LEFT_STICK));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_RIGHT_STICK", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_RIGHT_STICK));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_LEFT_SHOULDER", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_LEFT_SHOULDER));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_RIGHT_SHOULDER", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_RIGHT_SHOULDER));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_DPAD_UP", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_DPAD_UP));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_DPAD_DOWN", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_DPAD_DOWN));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_DPAD_LEFT", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_DPAD_LEFT));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_DPAD_RIGHT", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_DPAD_RIGHT));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_SHARE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_SHARE));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_PADDLE1", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_PADDLE1));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_PADDLE2", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_PADDLE2));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_PADDLE3", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_PADDLE3));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_PADDLE4", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_PADDLE4));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_TOUCHPAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_TOUCHPAD));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_LEFT_TRIGGER", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_LEFT_TRIGGER));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_RIGHT_TRIGGER", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_RIGHT_TRIGGER));
    mrb_define_const(mrb, mod, "GRAPPLE_PAD_BUTTON_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_PAD_BUTTON_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_AA_OFF", mrb_int_value(mrb, (mrb_int)GRAPPLE_AA_OFF));
    mrb_define_const(mrb, mod, "GRAPPLE_AA_FXAA", mrb_int_value(mrb, (mrb_int)GRAPPLE_AA_FXAA));
    mrb_define_const(mrb, mod, "GRAPPLE_QUALITY_OFF", mrb_int_value(mrb, (mrb_int)GRAPPLE_QUALITY_OFF));
    mrb_define_const(mrb, mod, "GRAPPLE_QUALITY_LOW", mrb_int_value(mrb, (mrb_int)GRAPPLE_QUALITY_LOW));
    mrb_define_const(mrb, mod, "GRAPPLE_QUALITY_MEDIUM", mrb_int_value(mrb, (mrb_int)GRAPPLE_QUALITY_MEDIUM));
    mrb_define_const(mrb, mod, "GRAPPLE_QUALITY_HIGH", mrb_int_value(mrb, (mrb_int)GRAPPLE_QUALITY_HIGH));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_ALIGN_LEFT", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_ALIGN_LEFT));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_ALIGN_CENTER", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_ALIGN_CENTER));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_ALIGN_RIGHT", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_ALIGN_RIGHT));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_FONT_SMALL", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_FONT_SMALL));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_FONT_NORMAL", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_FONT_NORMAL));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_FONT_LARGE", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_FONT_LARGE));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_GLYPHS_LATIN1", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_GLYPHS_LATIN1));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_GLYPHS_PUNCTUATION", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_GLYPHS_PUNCTUATION));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_GLYPHS_CYRILLIC", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_GLYPHS_CYRILLIC));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_GLYPHS_CHINESE", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_GLYPHS_CHINESE));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_GLYPHS_KOREAN", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_GLYPHS_KOREAN));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_IMAGE_STRETCH", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_IMAGE_STRETCH));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_IMAGE_ZOOM", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_IMAGE_ZOOM));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_IMAGE_CENTER", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_IMAGE_CENTER));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_IMAGE_FILL", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_IMAGE_FILL));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_TEXT", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_TEXT));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_BUTTON", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_BUTTON));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_BUTTON_HOVER", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_BUTTON_HOVER));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_BUTTON_TEXT", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_BUTTON_TEXT));
    mrb_define_const(mrb, mod, "GRAPPLE_GUI_COLOR_HEADER", mrb_int_value(mrb, (mrb_int)GRAPPLE_GUI_COLOR_HEADER));
    mrb_define_const(mrb, mod, "GRAPPLE_DEVICE_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_DEVICE_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_DEVICE_KEYBOARD", mrb_int_value(mrb, (mrb_int)GRAPPLE_DEVICE_KEYBOARD));
    mrb_define_const(mrb, mod, "GRAPPLE_DEVICE_MOUSE", mrb_int_value(mrb, (mrb_int)GRAPPLE_DEVICE_MOUSE));
    mrb_define_const(mrb, mod, "GRAPPLE_DEVICE_GAMEPAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_DEVICE_GAMEPAD));
    mrb_define_const(mrb, mod, "GRAPPLE_DEVICE_TOUCH", mrb_int_value(mrb, (mrb_int)GRAPPLE_DEVICE_TOUCH));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_SUNRISE", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_SUNRISE));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_AFTERNOON", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_AFTERNOON));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_SUNSET", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_SUNSET));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_NIGHT", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_NIGHT));
    mrb_define_const(mrb, mod, "GRAPPLE_LIGHT_DARK", mrb_int_value(mrb, (mrb_int)GRAPPLE_LIGHT_DARK));
    mrb_define_const(mrb, mod, "GRAPPLE_MEDIA_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_MEDIA_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_MEDIA_EXPLICIT", mrb_int_value(mrb, (mrb_int)GRAPPLE_MEDIA_EXPLICIT));
    mrb_define_const(mrb, mod, "GRAPPLE_MEDIA_EMBEDDED", mrb_int_value(mrb, (mrb_int)GRAPPLE_MEDIA_EMBEDDED));
    mrb_define_const(mrb, mod, "GRAPPLE_MEDIA_ARCHIVE", mrb_int_value(mrb, (mrb_int)GRAPPLE_MEDIA_ARCHIVE));
    mrb_define_const(mrb, mod, "GRAPPLE_MEDIA_DIRECTORY", mrb_int_value(mrb, (mrb_int)GRAPPLE_MEDIA_DIRECTORY));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_LEFT", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_LEFT));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_MIDDLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_MIDDLE));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_RIGHT", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_RIGHT));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_X1", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_X1));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_X2", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_X2));
    mrb_define_const(mrb, mod, "GRAPPLE_MOUSE_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_MOUSE_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_SCROLL_FREE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_SCROLL_FREE));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_SCROLL_FORWARD", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_SCROLL_FORWARD));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_EMPTY", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_EMPTY));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_SOLID", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_SOLID));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_ONE_WAY", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_ONE_WAY));
    mrb_define_const(mrb, mod, "GRAPPLE_PLATFORMER_TILE_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLATFORMER_TILE_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_IDLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_IDLE));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_WALK", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_WALK));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_RUN", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_RUN));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_JUMP", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_JUMP));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_FALL", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_FALL));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_WALL_SLIDE", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_WALL_SLIDE));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_PAUSED", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_PAUSED));
    mrb_define_const(mrb, mod, "GRAPPLE_PLAYER_STATE_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_PLAYER_STATE_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_DEFAULT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_DEFAULT));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_UPDATE_WHEN_COVERED", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_UPDATE_WHEN_COVERED));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_TRANSPARENT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_TRANSPARENT));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_LOAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_LOAD));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_ENTER", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_ENTER));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_FIXED_UPDATE", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_FIXED_UPDATE));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_UPDATE", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_UPDATE));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_RENDER", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_RENDER));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_EVENT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_EVENT));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_EXIT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_EXIT));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_UNLOAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_UNLOAD));
    mrb_define_const(mrb, mod, "GRAPPLE_SCENE_HOOK_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_SCENE_HOOK_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_TRANSITION_NONE", mrb_int_value(mrb, (mrb_int)GRAPPLE_TRANSITION_NONE));
    mrb_define_const(mrb, mod, "GRAPPLE_TRANSITION_FADE", mrb_int_value(mrb, (mrb_int)GRAPPLE_TRANSITION_FADE));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_LOAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_LOAD));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_FIXED_UPDATE", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_FIXED_UPDATE));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_UPDATE", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_UPDATE));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_RENDER", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_RENDER));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_POST_RENDER", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_POST_RENDER));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_EVENT", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_EVENT));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_RESIZE", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_RESIZE));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_UNLOAD", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_UNLOAD));
    mrb_define_const(mrb, mod, "GRAPPLE_HOOK_COUNT", mrb_int_value(mrb, (mrb_int)GRAPPLE_HOOK_COUNT));
    mrb_define_const(mrb, mod, "GRAPPLE_SHAPE_BOX", mrb_int_value(mrb, (mrb_int)GRAPPLE_SHAPE_BOX));
    mrb_define_const(mrb, mod, "GRAPPLE_SHAPE_CIRCLE", mrb_int_value(mrb, (mrb_int)GRAPPLE_SHAPE_CIRCLE));
    mrb_define_const(mrb, mod, "GRAPPLE_SHAPE_CAPSULE", mrb_int_value(mrb, (mrb_int)GRAPPLE_SHAPE_CAPSULE));
    mrb_define_const(mrb, mod, "GRAPPLE_SPLIT_HORIZONTAL", mrb_int_value(mrb, (mrb_int)GRAPPLE_SPLIT_HORIZONTAL));
    mrb_define_const(mrb, mod, "GRAPPLE_SPLIT_VERTICAL", mrb_int_value(mrb, (mrb_int)GRAPPLE_SPLIT_VERTICAL));
    mrb_define_const(mrb, mod, "GRAPPLE_SPLIT_GRID", mrb_int_value(mrb, (mrb_int)GRAPPLE_SPLIT_GRID));
    mrb_define_const(mrb, mod, "GRAPPLE_FILTER_AUTO", mrb_int_value(mrb, (mrb_int)GRAPPLE_FILTER_AUTO));
    mrb_define_const(mrb, mod, "GRAPPLE_FILTER_LINEAR", mrb_int_value(mrb, (mrb_int)GRAPPLE_FILTER_LINEAR));
    mrb_define_const(mrb, mod, "GRAPPLE_FILTER_NEAREST", mrb_int_value(mrb, (mrb_int)GRAPPLE_FILTER_NEAREST));
    mrb_define_const(mrb, mod, "GRAPPLE_WINDOW_WINDOWED", mrb_int_value(mrb, (mrb_int)GRAPPLE_WINDOW_WINDOWED));
    mrb_define_const(mrb, mod, "GRAPPLE_WINDOW_BORDERLESS", mrb_int_value(mrb, (mrb_int)GRAPPLE_WINDOW_BORDERLESS));
    mrb_define_const(mrb, mod, "GRAPPLE_WINDOW_EXCLUSIVE", mrb_int_value(mrb, (mrb_int)GRAPPLE_WINDOW_EXCLUSIVE));
}
