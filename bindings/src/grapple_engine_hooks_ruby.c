/*
 * grapple_engine_hooks_ruby.c — registering Ruby blocks as engine hooks.
 * Original Grapple code (zlib).
 *
 * The mirror of the Lua file, and for the same reason: turning a Ruby
 * callable into something C can hold is the one thing the generator cannot
 * do. The engine's bridge does the dispatching and knows about neither
 * language.
 *
 * A handle here is an index into an array the interpreter keeps reachable.
 * mruby has no registry like Lua's, and a bare mrb_value stored in C is not
 * a GC root — the proc would be collected out from under the engine — so
 * the array is registered with mrb_gc_register and the handle is a position
 * in it.
 */
#include <grapple/engine.h>
#include <grapple/engine_script.h>
#include <grapple/gui.h>
#include <grapple/engine_scene.h>
#include <grapple/ruby.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/proc.h>
#include <mruby/variable.h>

#include "gen_support_ruby.h"

/* The array of live handlers lives in a global on the interpreter, so it is
   reachable by mruby's collector for exactly as long as the state is. */
#define HANDLER_ARRAY "$__grapple_engine_hooks"

static mrb_value HandlerArray(mrb_state *mrb)
{
    mrb_value array = mrb_gv_get(mrb, mrb_intern_lit(mrb, HANDLER_ARRAY));
    if (!mrb_array_p(array))
    {
        array = mrb_ary_new(mrb);
        mrb_gv_set(mrb, mrb_intern_lit(mrb, HANDLER_ARRAY), array);
    }
    return array;
}

static bool DispatchRuby(void *language_state, Sint64 handle, Grapple_ScriptHook hook,
                         float value, const void *payload)
{
    mrb_state *mrb = (mrb_state *)language_state;
    mrb_value array = HandlerArray(mrb);
    if (handle < 0 || handle >= RARRAY_LEN(array))
    {
        return true;
    }
    const mrb_value callable = mrb_ary_ref(mrb, array, (mrb_int)handle);
    if (mrb_nil_p(callable))
    {
        return true;
    }

    /* Only the hooks carrying a number take an argument, so a script writes
       a no-argument block for the rest rather than ignoring a zero. */
    mrb_value result;
    if (hook == GRAPPLE_HOOK_FIXED_UPDATE || hook == GRAPPLE_HOOK_UPDATE ||
        hook == GRAPPLE_HOOK_RENDER)
    {
        const mrb_value arg = mrb_float_value(mrb, (mrb_float)value);
        result = mrb_funcall(mrb, callable, "call", 1, arg);
    }
    else if (hook == GRAPPLE_HOOK_EVENT)
    {
        /* Borrowed: the same handle every generated SDL.* function takes,
           but pointing at a stack event the engine owns. It is valid only
           until this call returns, which is why nothing takes ownership. */
        const mrb_value arg =
            GrappleGen_RubyPushHandle(mrb, (void *)(uintptr_t)payload, "SDL_Event");
        result = mrb_funcall(mrb, callable, "call", 1, arg);
    }
    else if (hook == GRAPPLE_HOOK_RESIZE)
    {
        const Grapple_ScriptSize *size = (const Grapple_ScriptSize *)payload;
        const mrb_value w = mrb_fixnum_value(size != NULL ? size->width : 0);
        const mrb_value h = mrb_fixnum_value(size != NULL ? size->height : 0);
        result = mrb_funcall(mrb, callable, "call", 2, w, h);
    }
    else
    {
        result = mrb_funcall(mrb, callable, "call", 0);
    }

    if (mrb->exc != NULL)
    {
        /* An exception must not escape into the engine's C frames: report it
           and carry on, so one bad frame does not end the game. Say what it
           was — "engine hook raised" on its own leaves the author to guess,
           which is exactly the wrong thing to do sixty times a second. */
        const mrb_value message = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "engine hook: %s",
                     mrb_string_p(message) ? RSTRING_CSTR(mrb, message) : "raised");
        mrb->exc = NULL;
        return true;
    }
    /* Only `load` reads a result, where false aborts start-up. */
    if (hook == GRAPPLE_HOOK_LOAD)
    {
        return mrb_test(result);
    }
    return true;
}

static void ReleaseRuby(void *language_state, Sint64 handle)
{
    mrb_state *mrb = (mrb_state *)language_state;
    mrb_value array = HandlerArray(mrb);
    if (handle >= 0 && handle < RARRAY_LEN(array))
    {
        /* Nil the slot rather than removing it: every other handle is an
           index into this array, and compacting would move them. */
        mrb_ary_set(mrb, array, (mrb_int)handle, mrb_nil_value());
    }
}

/* Bind one block to one hook. Shared with the engine object in
   grapple_ui_ruby.c, which offers the same hooks as methods. */
bool Grapple_RubyBindEngineHook(mrb_state *mrb, Grapple_Engine *engine,
                                  Grapple_ScriptHook hook, mrb_value block)
{
    if (!Grapple_ScriptBind(engine, mrb, DispatchRuby, ReleaseRuby))
    {
        return false;
    }
    mrb_value array = HandlerArray(mrb);
    mrb_ary_push(mrb, array, block);
    const Sint64 handle = (Sint64)(RARRAY_LEN(array) - 1);
    return Grapple_ScriptSetHook(engine, hook, handle);
}

static mrb_value SetHook(mrb_state *mrb, Grapple_ScriptHook hook)
{
    mrb_value engine_value;
    mrb_value callable = mrb_nil_value();
    mrb_get_args(mrb, "o|o&", &engine_value, &callable, &callable);

    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, engine_value, "Grapple_Engine");
    if (engine == NULL || mrb_nil_p(callable))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine and a callable are required");
    }

    if (!Grapple_ScriptBind(engine, mrb, DispatchRuby, ReleaseRuby))
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "could not bind Ruby to this engine");
    }

    mrb_value array = HandlerArray(mrb);
    const mrb_int index = RARRAY_LEN(array);
    mrb_ary_push(mrb, array, callable);
    if (!Grapple_ScriptSetHook(engine, hook, (Sint64)index))
    {
        mrb_ary_set(mrb, array, index, mrb_nil_value());
        mrb_raise(mrb, E_RUNTIME_ERROR, SDL_GetError());
    }
    return mrb_true_value();
}

static mrb_value ROnLoad(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_LOAD);
}
static mrb_value ROnFixedUpdate(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_FIXED_UPDATE);
}
static mrb_value ROnUpdate(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_UPDATE);
}
static mrb_value ROnRender(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_RENDER);
}
static mrb_value ROnPostRender(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_POST_RENDER);
}
static mrb_value ROnEvent(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_EVENT);
}

static mrb_value ROnResize(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_RESIZE);
}

static mrb_value ROnUnload(mrb_state *mrb, mrb_value self)
{
    (void)self;
    return SetHook(mrb, GRAPPLE_HOOK_UNLOAD);
}

/* Let the engine drive a GUI's input.
 *
 * The C equivalent is two lines (Grapple_GuiEventSink then
 * Grapple_EngineSetEventSink), but a sink is a struct of function pointers
 * and there is no sane way to hand one across a script boundary — so scripts
 * get the pair as a single call. Passing nil for the gui detaches.
 */
static mrb_value RAttachGui(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    mrb_value gui_value = mrb_nil_value();
    mrb_get_args(mrb, "o|o", &engine_value, &gui_value);
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, engine_value, "Grapple_Engine");
    if (engine == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine expected");
    }
    if (mrb_nil_p(gui_value))
    {
        Grapple_EngineSetEventSink(engine, NULL);
        return mrb_true_value();
    }
    Grapple_Gui *gui = (Grapple_Gui *)GrappleGen_RubyCheckHandle(mrb, gui_value, "Grapple_Gui");
    if (gui == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "gui expected");
    }
    const Grapple_EventSink sink = Grapple_GuiEventSink(gui);
    Grapple_EngineSetEventSink(engine, &sink);
    return mrb_true_value();
}

static mrb_value RRun(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    mrb_get_args(mrb, "o", &engine_value);
    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, engine_value, "Grapple_Engine");
    if (engine == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine expected");
    }
    return mrb_bool_value(Grapple_ScriptRun(engine));
}

/* --- scenes ---------------------------------------------------------------- */

/* A scene callback carries more than a game hook does: which scene, and for
   the event hook the event. Both cross as borrowed handles — the engine
   owns the scene, and the event belongs to the frame — so a script that
   stashes one and uses it later has a dangling pointer, as it would in C. */
static bool DispatchSceneRuby(void *language_state, Sint64 handle, Grapple_SceneHook hook,
                              Grapple_Scene *scene, float value, const SDL_Event *event)
{
    mrb_state *mrb = (mrb_state *)language_state;
    mrb_value array = HandlerArray(mrb);
    if (handle < 0 || handle >= RARRAY_LEN(array))
    {
        return true;
    }
    const mrb_value callable = mrb_ary_ref(mrb, array, (mrb_int)handle);
    if (mrb_nil_p(callable))
    {
        return true;
    }

    /* Every callback is given its scene, so one definition can back several
       live scenes and a script can tell them apart — a hash keyed by the
       scene does what Grapple_SceneState does for C. */
    const mrb_value scene_value = GrappleGen_RubyPushHandle(mrb, scene, "Grapple_Scene");
    mrb_value result;
    if (hook == GRAPPLE_SCENE_HOOK_FIXED_UPDATE || hook == GRAPPLE_SCENE_HOOK_UPDATE ||
        hook == GRAPPLE_SCENE_HOOK_RENDER)
    {
        result = mrb_funcall(mrb, callable, "call", 2, scene_value,
                             mrb_float_value(mrb, (mrb_float)value));
    }
    else if (hook == GRAPPLE_SCENE_HOOK_EVENT)
    {
        /* const is dropped because the bindings have one handle type per C
           type; a script cannot usefully write through it in any case. */
        const mrb_value event_value =
            GrappleGen_RubyPushHandle(mrb, (void *)(uintptr_t)event, "SDL_Event");
        result = mrb_funcall(mrb, callable, "call", 2, scene_value, event_value);
    }
    else
    {
        result = mrb_funcall(mrb, callable, "call", 1, scene_value);
    }

    if (mrb->exc != NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "scene hook raised");
        mrb->exc = NULL;
        return true;
    }
    if (hook == GRAPPLE_SCENE_HOOK_LOAD)
    {
        return mrb_test(result);
    }
    return true;
}

/* Hook names rather than numbers: a typo in a number is silent, a typo in
   a name is an error naming the ones that exist. */
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
static mrb_value RSceneDefine(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    const char *name = NULL;
    mrb_get_args(mrb, "oz", &engine_value, &name);

    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, engine_value, "Grapple_Engine");
    if (engine == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine expected");
    }
    /* Defining a scene binds the language too, so a script that uses only
       scenes never has to know the game hooks exist. */
    if (!Grapple_ScriptBind(engine, mrb, DispatchRuby, ReleaseRuby) ||
        !Grapple_ScriptSetSceneDispatch(engine, DispatchSceneRuby))
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "could not bind Ruby to this engine");
    }
    if (!Grapple_ScriptSceneDefine(engine, name))
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, SDL_GetError());
    }
    return mrb_true_value();
}

/* GrappleC.SceneOn(engine, "level", "render") { |scene, alpha| ... } */
static mrb_value RSceneOn(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    const char *name = NULL;
    const char *hook_name = NULL;
    mrb_value callable = mrb_nil_value();
    mrb_get_args(mrb, "ozz|o&", &engine_value, &name, &hook_name, &callable, &callable);

    Grapple_Engine *engine =
        (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, engine_value, "Grapple_Engine");
    if (engine == NULL || mrb_nil_p(callable))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine, scene name, hook name and a callable "
                                         "are required");
    }

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
        mrb_raise(mrb, E_ARGUMENT_ERROR,
                  "unknown scene hook: expected load, enter, fixed_update, update, "
                  "render, event, exit or unload");
    }

    mrb_value array = HandlerArray(mrb);
    const mrb_int index = RARRAY_LEN(array);
    mrb_ary_push(mrb, array, callable);
    if (!Grapple_ScriptSceneSetHook(engine, name, hook, (Sint64)index))
    {
        mrb_ary_set(mrb, array, index, mrb_nil_value());
        mrb_raise(mrb, E_RUNTIME_ERROR, SDL_GetError());
    }
    return mrb_true_value();
}

bool Grapple_OpenRubyEngineHooks(mrb_state *mrb)
{
    if (mrb == NULL)
    {
        return false;
    }
    /* Added to the generated module, so a script sees one namespace for the
       engine however each piece was produced. */
    struct RClass *mod = mrb_module_get(mrb, "GrappleC");
    if (mod == NULL)
    {
        return false;
    }
    mrb_define_module_function(mrb, mod, "OnLoad", ROnLoad, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnFixedUpdate", ROnFixedUpdate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnUpdate", ROnUpdate, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnRender", ROnRender, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnPostRender", ROnPostRender, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnEvent", ROnEvent, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnResize", ROnResize, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "OnUnload", ROnUnload, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "AttachGui", RAttachGui, MRB_ARGS_ANY());
    mrb_define_module_function(mrb, mod, "Run", RRun, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, mod, "SceneDefine", RSceneDefine, MRB_ARGS_REQ(2));
    mrb_define_module_function(mrb, mod, "SceneOn", RSceneOn, MRB_ARGS_ANY());
    return true;
}
