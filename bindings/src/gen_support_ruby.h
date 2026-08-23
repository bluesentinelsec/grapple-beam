/*
 * gen_support_ruby.h — marshaling runtime for the GENERATED mruby
 * bindings. Hand-written support code (original Grapple, zlib); the
 * generated per-library stub files include this. Same handle model as
 * the Lua side: boxed pointer + C type name + optional destructor, with
 * dfree running the destructor exactly once and bound destroy functions
 * emptying the box first.
 */
#ifndef GRAPPLE_GEN_SUPPORT_RUBY_H
#define GRAPPLE_GEN_SUPPORT_RUBY_H

#include <grapple/ruby.h>

#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GrappleGen_Dtor)(void *ptr);

typedef struct GrappleGen_RubyHandle
{
    void *ptr;
    const char *ctype;
    GrappleGen_Dtor dtor;
} GrappleGen_RubyHandle;

extern const struct mrb_data_type GrappleGen_RubyHandleType;

/** Push a borrowed pointer (nil for NULL). */
extern mrb_value GrappleGen_RubyPushHandle(mrb_state *mrb, void *ptr,
                                             const char *ctype);
/** Push an owned pointer; GC calls dtor unless taken first. */
extern mrb_value GrappleGen_RubyPushOwned(mrb_state *mrb, void *ptr,
                                            const char *ctype,
                                            GrappleGen_Dtor dtor);
/** Handle argument -> pointer; nil -> NULL; wrong type raises TypeError. */
extern void *GrappleGen_RubyCheckHandle(mrb_state *mrb, mrb_value v,
                                          const char *ctype);
/** Like check, but empties the box (ownership leaves Ruby). */
extern void *GrappleGen_RubyTakeHandle(mrb_state *mrb, mrb_value v,
                                         const char *ctype);

/** Hash-field readers for generated POD converters (0/false when absent). */
extern mrb_int GrappleGen_RubyFieldInt(mrb_state *mrb, mrb_value hash,
                                         const char *field);
extern mrb_float GrappleGen_RubyFieldNum(mrb_state *mrb, mrb_value hash,
                                           const char *field);
extern mrb_bool GrappleGen_RubyFieldBool(mrb_state *mrb, mrb_value hash,
                                           const char *field);
extern void GrappleGen_RubyHashSet(mrb_state *mrb, mrb_value hash,
                                     const char *field, mrb_value v);
extern mrb_value GrappleGen_RubyFieldGet(mrb_state *mrb, mrb_value hash,
                                           const char *field);

/** Lenient argument converters (nil/absent -> 0/NULL/false). */
extern mrb_int GrappleGen_RubyToInt(mrb_state *mrb, mrb_value v);
extern mrb_float GrappleGen_RubyToNum(mrb_state *mrb, mrb_value v);
extern mrb_bool GrappleGen_RubyToBool(mrb_value v);
extern const char *GrappleGen_RubyToStr(mrb_state *mrb, mrb_value v);
extern const char *GrappleGen_RubyToBlob(mrb_state *mrb, mrb_value v,
                                           size_t *len);

/** Register the shared Handle class; called by each module opener. */
extern void GrappleGen_RubyEnsureHandleClass(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_GEN_SUPPORT_RUBY_H */
