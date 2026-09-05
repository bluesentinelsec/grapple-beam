/*
 * grapple_ui_ruby.c — the declarative engine and the retained UI, in Ruby.
 *
 * Original Grapple code (zlib). Same contracts as the Lua side
 * (grapple_engine_lua.c, grapple_ui_lua.c); what differs is the spelling,
 * because Ruby already has keyword arguments and blocks:
 *
 *   engine = Grapple.engine(title: "My Game", window: { width: 640, height: 400 })
 *   ui = Grapple.ui(engine)
 *   panel = ui.panel(title: "Settings", padding: 12)
 *   answer = panel.label(text: "", align: :center)
 *   panel.button(text: "Clear") { answer.set("") }
 *
 * A block is the handler, which is what a Ruby programmer would expect and
 * what an `on_click:` key would have got in the way of.
 */
#include <grapple/bindings.h>
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/engine_script.h>
#include <grapple/widgets.h>

#include "gen_support_ruby.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <SDL3/SDL.h>

#include <stdint.h>

#define HANDLERS_IVAR "@__ui_handlers"

/* --- data types ---------------------------------------------------------- */

static void EngineFree(mrb_state *mrb, void *ptr)
{
    (void)mrb;
    if (ptr != NULL)
    {
        Grapple_DestroyEngine((Grapple_Engine *)ptr);
    }
}

static void UiFree(mrb_state *mrb, void *ptr)
{
    (void)mrb;
    if (ptr != NULL)
    {
        Grapple_DestroyUi((Grapple_Ui *)ptr);
    }
}

/* Widgets are owned by their parent, so the wrapper frees nothing. */
static void WidgetFree(mrb_state *mrb, void *ptr)
{
    (void)mrb;
    (void)ptr;
}

static const struct mrb_data_type kEngineType = {"Grapple::Engine", EngineFree};
static const struct mrb_data_type kUiType = {"Grapple::Ui", UiFree};
static const struct mrb_data_type kWidgetType = {"Grapple::Widget", WidgetFree};

static struct RClass *g_widget_class = NULL;

/* The engine a script built but has not run — see the note on the Lua side
   in grapple_engine_lua.c. Ruby gets the same behaviour for the same
   reason: a script should be able to describe callbacks and stop.
   Hung off the interpreter rather than a C static, so one state's unstarted
   engine can never be started by another's. */
#define PENDING_ENGINE_IVAR "@__pending_engine"

static void SetPendingEngine(mrb_state *mrb, Grapple_Engine *engine)
{
    struct RClass *module = mrb_module_get(mrb, "Grapple");
    mrb_iv_set(mrb, mrb_obj_value(module), mrb_intern_cstr(mrb, PENDING_ENGINE_IVAR),
               (engine != NULL) ? mrb_cptr_value(mrb, engine) : mrb_nil_value());
}

static Grapple_Engine *PendingEngine(mrb_state *mrb)
{
    struct RClass *module = mrb_module_get(mrb, "Grapple");
    const mrb_value value =
        mrb_iv_get(mrb, mrb_obj_value(module), mrb_intern_cstr(mrb, PENDING_ENGINE_IVAR));
    return mrb_cptr_p(value) ? (Grapple_Engine *)mrb_cptr(value) : NULL;
}

/* --- reading keyword arguments ------------------------------------------- */

static mrb_value Key(mrb_state *mrb, mrb_value options, const char *name)
{
    if (mrb_nil_p(options) || !mrb_hash_p(options))
    {
        return mrb_nil_value();
    }
    return mrb_hash_get(mrb, options, mrb_symbol_value(mrb_intern_cstr(mrb, name)));
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
        /* Symbols read better than strings for a fixed set of choices. */
        return mrb_sym_name(mrb, mrb_symbol(value));
    }
    return fallback;
}

static bool OptBool(mrb_state *mrb, mrb_value options, const char *name, bool fallback)
{
    const mrb_value value = Key(mrb, options, name);
    if (mrb_nil_p(value))
    {
        return fallback;
    }
    return mrb_test(value);
}

static float OptNumber(mrb_state *mrb, mrb_value options, const char *name, float fallback)
{
    const mrb_value value = Key(mrb, options, name);
    if (mrb_fixnum_p(value) || mrb_float_p(value))
    {
        return (float)mrb_as_float(mrb, value);
    }
    return fallback;
}

static int OptInt(mrb_state *mrb, mrb_value options, const char *name, int fallback)
{
    return (int)OptNumber(mrb, options, name, (float)fallback);
}

static void OptSize(mrb_state *mrb, mrb_value options, const char *name, int *width,
                    int *height)
{
    const mrb_value value = Key(mrb, options, name);
    if (mrb_hash_p(value))
    {
        *width = OptInt(mrb, value, "width", *width);
        *height = OptInt(mrb, value, "height", *height);
    }
    else if (mrb_array_p(value) && RARRAY_LEN(value) >= 2)
    {
        *width = (int)mrb_as_float(mrb, mrb_ary_ref(mrb, value, 0));
        *height = (int)mrb_as_float(mrb, mrb_ary_ref(mrb, value, 1));
    }
}

/* 24 is pixels, "2.4em" is font-relative, "25%" is a share of the parent,
   :fit is as wide as the content. Same rules as Lua, same reason. */
static Grapple_UiLength OptLength(mrb_state *mrb, mrb_value options, const char *name)
{
    Grapple_UiLength length = {0.0f, GRAPPLE_UI_UNIT_STRETCH};
    const mrb_value value = Key(mrb, options, name);

    if (mrb_fixnum_p(value) || mrb_float_p(value))
    {
        length.value = (float)mrb_as_float(mrb, value);
        length.unit = GRAPPLE_UI_UNIT_PX;
        return length;
    }
    const char *text = NULL;
    if (mrb_string_p(value))
    {
        text = mrb_str_to_cstr(mrb, value);
    }
    else if (mrb_symbol_p(value))
    {
        text = mrb_sym_name(mrb, mrb_symbol(value));
    }
    if (text == NULL)
    {
        return length;
    }
    if (SDL_strcasecmp(text, "fit") == 0)
    {
        length.unit = GRAPPLE_UI_UNIT_FIT;
    }
    else if (SDL_strcasecmp(text, "stretch") == 0)
    {
        length.unit = GRAPPLE_UI_UNIT_STRETCH;
    }
    else
    {
        const double number = SDL_atof(text);
        const size_t len = SDL_strlen(text);
        if (len > 0 && text[len - 1] == '%')
        {
            length.value = (float)number / 100.0f;
            length.unit = GRAPPLE_UI_UNIT_PCT;
        }
        else if (len > 2 && SDL_strcasecmp(text + len - 2, "em") == 0)
        {
            length.value = (float)number;
            length.unit = GRAPPLE_UI_UNIT_EM;
        }
        else
        {
            length.value = (float)number;
            length.unit = GRAPPLE_UI_UNIT_PX;
        }
    }
    return length;
}

static Grapple_UiAlign OptAlign(mrb_state *mrb, mrb_value options)
{
    const char *text = OptString(mrb, options, "align", "left");
    if (SDL_strcasecmp(text, "center") == 0 || SDL_strcasecmp(text, "centre") == 0)
    {
        return GRAPPLE_UI_CENTER;
    }
    if (SDL_strcasecmp(text, "right") == 0)
    {
        return GRAPPLE_UI_RIGHT;
    }
    return GRAPPLE_UI_LEFT;
}

static Grapple_GuiImageMode OptImageMode(mrb_state *mrb, mrb_value options)
{
    const char *text = OptString(mrb, options, "mode", "stretch");
    if (SDL_strcasecmp(text, "zoom") == 0)
    {
        return GRAPPLE_GUI_IMAGE_ZOOM;
    }
    if (SDL_strcasecmp(text, "center") == 0 || SDL_strcasecmp(text, "centre") == 0)
    {
        return GRAPPLE_GUI_IMAGE_CENTER;
    }
    if (SDL_strcasecmp(text, "fill") == 0)
    {
        return GRAPPLE_GUI_IMAGE_FILL;
    }
    return GRAPPLE_GUI_IMAGE_STRETCH;
}

/* --- handlers ------------------------------------------------------------ */

/* Blocks live in one array hung off the Grapple module, indexed by the
   widget pointer, so a handler is reachable from a C callback and stays
   alive as long as the module does. */
static mrb_value HandlerTable(mrb_state *mrb)
{
    struct RClass *module = mrb_module_get(mrb, "Grapple");
    mrb_value table = mrb_iv_get(mrb, mrb_obj_value(module), mrb_intern_cstr(mrb, HANDLERS_IVAR));
    if (!mrb_hash_p(table))
    {
        table = mrb_hash_new(mrb);
        mrb_iv_set(mrb, mrb_obj_value(module), mrb_intern_cstr(mrb, HANDLERS_IVAR), table);
    }
    return table;
}

static mrb_value WidgetValue(mrb_state *mrb, Grapple_UiWidget *widget);

/* A handler is a block or anything else that answers to `call` — which
   includes method(:on_word_clicked). Blocks are idiomatic for a one-liner;
   a named method reads better when the handler is worth a name, and a
   script should not have to wrap one in a block to pass it. */
static mrb_value HandlerFrom(mrb_state *mrb, mrb_value options, mrb_value block,
                             const char *key)
{
    if (!mrb_nil_p(block))
    {
        return block;
    }
    const mrb_value named = Key(mrb, options, key);
    if (!mrb_nil_p(named) && mrb_respond_to(mrb, named, mrb_intern_cstr(mrb, "call")))
    {
        return named;
    }
    return mrb_nil_value();
}

/* The widget's address as an Integer.
 *
 * Not mrb_cptr_value: a cptr is not a hash key mruby can match, so every
 * lookup missed and no handler ever fired. An Integer compares by value,
 * which is the whole requirement here. */
static mrb_value HandlerKey(mrb_state *mrb, Grapple_UiWidget *widget)
{
    (void)mrb;
    return mrb_fixnum_value((mrb_int)(intptr_t)widget);
}

static void RememberHandler(mrb_state *mrb, Grapple_UiWidget *widget, mrb_value block)
{
    if (widget == NULL || mrb_nil_p(block))
    {
        return;
    }
    mrb_hash_set(mrb, HandlerTable(mrb), HandlerKey(mrb, widget), block);
}

static void ScriptCallback(Grapple_UiWidget *widget, void *user)
{
    mrb_state *mrb = (mrb_state *)user;
    if (mrb == NULL)
    {
        return;
    }
    const mrb_value block = mrb_hash_get(mrb, HandlerTable(mrb), HandlerKey(mrb, widget));
    if (mrb_nil_p(block))
    {
        return;
    }
    mrb_funcall(mrb, block, "call", 1, WidgetValue(mrb, widget));
    if (mrb->exc != NULL)
    {
        /* Never let an exception unwind into the engine's C frames. */
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ui callback raised");
        mrb->exc = NULL;
    }
}

/* --- wrappers ------------------------------------------------------------ */

static mrb_value WidgetValue(mrb_state *mrb, Grapple_UiWidget *widget)
{
    if (widget == NULL)
    {
        return mrb_nil_value();
    }
    struct RData *data = mrb_data_object_alloc(mrb, g_widget_class, widget, &kWidgetType);
    return mrb_obj_value(data);
}

static Grapple_UiWidget *WidgetOf(mrb_state *mrb, mrb_value self)
{
    return (Grapple_UiWidget *)mrb_data_get_ptr(mrb, self, &kWidgetType);
}

Grapple_Engine *Grapple_RubyEngineAt(mrb_state *mrb, mrb_value value)
{
    if (mrb_data_p(value) && DATA_TYPE(value) == &kEngineType)
    {
        return (Grapple_Engine *)DATA_PTR(value);
    }
    return (Grapple_Engine *)GrappleGen_RubyCheckHandle(mrb, value, "Grapple_Engine");
}

/* --- Grapple.engine ------------------------------------------------------ */

static const char *const kEngineKeys[] = {
    "title",      "window",   "design", "presentation", "resizable", "high_dpi",
    "fullscreen", "vsync",    "max_fps", "tick_rate",   "auto_mount", "headless",
    "media",      "font_size", "backend",
};

static void CheckKeys(mrb_state *mrb, mrb_value options)
{
    if (!mrb_hash_p(options))
    {
        return;
    }
    const mrb_value keys = mrb_hash_keys(mrb, options);
    for (mrb_int i = 0; i < RARRAY_LEN(keys); ++i)
    {
        const mrb_value key = mrb_ary_ref(mrb, keys, i);
        const char *name = mrb_symbol_p(key)  ? mrb_sym_name(mrb, mrb_symbol(key))
                           : mrb_string_p(key) ? mrb_str_to_cstr(mrb, key)
                                               : NULL;
        if (name == NULL)
        {
            continue;
        }
        bool known = false;
        for (size_t k = 0; k < SDL_arraysize(kEngineKeys) && !known; ++k)
        {
            known = SDL_strcmp(name, kEngineKeys[k]) == 0;
        }
        if (!known)
        {
            /* Naming the key is the whole point: a silently ignored option
               is worse than the setter it replaced. */
            mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown engine option '%s'", name);
        }
    }
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

static mrb_value REngineNew(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);
    CheckKeys(mrb, options);

    Grapple_EngineConfig config = {0};
    config.title = OptString(mrb, options, "title", NULL);

    int window_w = 0;
    int window_h = 0;
    OptSize(mrb, options, "window", &window_w, &window_h);
    config.window_width = window_w;
    config.window_height = window_h;

    int design_w = 0;
    int design_h = 0;
    OptSize(mrb, options, "design", &design_w, &design_h);
    config.design_width = design_w;
    config.design_height = design_h;

    const char *presentation = OptString(mrb, options, "presentation", NULL);
    if (presentation != NULL && !ParsePresentation(presentation, &config.presentation))
    {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown presentation '%s'", presentation);
    }

    /* The C struct names the defaults as the thing you turn off; a keyword
       argument should be the positive form. */
    config.fixed_size = !OptBool(mrb, options, "resizable", true);
    config.low_dpi = !OptBool(mrb, options, "high_dpi", true);
    config.no_vsync = !OptBool(mrb, options, "vsync", true);
    config.no_auto_mount = !OptBool(mrb, options, "auto_mount", true);
    config.fullscreen = OptBool(mrb, options, "fullscreen", false);
    config.headless = OptBool(mrb, options, "headless", false);
    config.max_fps = OptInt(mrb, options, "max_fps", 0);
    config.tick_rate = OptInt(mrb, options, "tick_rate", 0);
    config.media_path = OptString(mrb, options, "media", NULL);

    /* The engine's own flags arrive here from the runner's command line. */
    Grapple_ScriptProcessArgs(&config.argc, (char ***)&config.argv);

    Grapple_Engine *engine = Grapple_CreateEngine(&config);
    if (engine == NULL)
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "%s", SDL_GetError());
    }

    SetPendingEngine(mrb, engine);

    struct RClass *klass = mrb_class_get_under(mrb, mrb_module_get(mrb, "Grapple"), "Engine");
    struct RData *data = mrb_data_object_alloc(mrb, klass, engine, &kEngineType);
    return mrb_obj_value(data);
}

static Grapple_Engine *EngineOf(mrb_state *mrb, mrb_value self)
{
    return (Grapple_Engine *)mrb_data_get_ptr(mrb, self, &kEngineType);
}

static mrb_value SetHook(mrb_state *mrb, mrb_value self, Grapple_ScriptHook hook)
{
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "&", &block);
    if (mrb_nil_p(block))
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "a block is required");
    }
    if (!Grapple_RubyBindEngineHook(mrb, EngineOf(mrb, self), hook, block))
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "%s", SDL_GetError());
    }
    return self; /* chainable */
}

static mrb_value ROnLoad(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_LOAD);
}
static mrb_value ROnFixedUpdate(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_FIXED_UPDATE);
}
static mrb_value ROnUpdate(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_UPDATE);
}
static mrb_value ROnRender(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_RENDER);
}
static mrb_value ROnPostRender(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_POST_RENDER);
}
static mrb_value ROnEvent(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_EVENT);
}
static mrb_value ROnResize(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_RESIZE);
}
static mrb_value ROnUnload(mrb_state *mrb, mrb_value self)
{
    return SetHook(mrb, self, GRAPPLE_HOOK_UNLOAD);
}

static mrb_value RRun(mrb_state *mrb, mrb_value self)
{
    Grapple_Engine *engine = EngineOf(mrb, self);
    if (engine == PendingEngine(mrb))
    {
        SetPendingEngine(mrb, NULL); /* the script chose to run it itself */
    }
    return mrb_bool_value(Grapple_ScriptRun(engine));
}

static mrb_value RQuit(mrb_state *mrb, mrb_value self)
{
    Grapple_EngineQuit(EngineOf(mrb, self));
    return mrb_nil_value();
}

static SDL_Scancode ScancodeArg(mrb_state *mrb)
{
    mrb_value key;
    mrb_get_args(mrb, "o", &key);
    if (mrb_fixnum_p(key))
    {
        return (SDL_Scancode)mrb_fixnum(key);
    }
    const char *name = mrb_string_p(key) ? mrb_str_to_cstr(mrb, key)
                       : mrb_symbol_p(key) ? mrb_sym_name(mrb, mrb_symbol(key))
                                           : NULL;
    if (name == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "key name expected");
    }
    const SDL_Scancode code = SDL_GetScancodeFromName(name);
    if (code == SDL_SCANCODE_UNKNOWN)
    {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "unknown key '%s'", name);
    }
    return code;
}

static mrb_value RKeyPressed(mrb_state *mrb, mrb_value self)
{
    Grapple_Engine *engine = EngineOf(mrb, self);
    return mrb_bool_value(Grapple_KeyPressed(engine, ScancodeArg(mrb)));
}

static mrb_value RKeyDown(mrb_state *mrb, mrb_value self)
{
    Grapple_Engine *engine = EngineOf(mrb, self);
    return mrb_bool_value(Grapple_KeyDown(engine, ScancodeArg(mrb)));
}

static mrb_value RSize(mrb_state *mrb, mrb_value self)
{
    int width = 0;
    int height = 0;
    Grapple_EnginePixelSize(EngineOf(mrb, self), &width, &height);
    mrb_value pair = mrb_ary_new_capa(mrb, 2);
    mrb_ary_push(mrb, pair, mrb_fixnum_value(width));
    mrb_ary_push(mrb, pair, mrb_fixnum_value(height));
    return pair;
}

/* --- Grapple.ui ---------------------------------------------------------- */

static mrb_value RUiOpen(mrb_state *mrb, mrb_value self)
{
    (void)self;
    mrb_value engine_value;
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "o|H", &engine_value, &options);

    Grapple_Engine *engine = Grapple_RubyEngineAt(mrb, engine_value);
    if (engine == NULL)
    {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "engine expected");
    }
    Grapple_Ui *ui =
        Grapple_OpenUi(Grapple_EngineRenderer(engine), OptNumber(mrb, options, "font_size", 0.0f));
    if (ui == NULL)
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "%s", SDL_GetError());
    }

    /* Input is arranged here because it has one correct arrangement.
       Drawing is not: call ui.draw from a render callback. */
    const Grapple_EventSink sink = Grapple_UiEventSink(ui);
    Grapple_EngineSetEventSink(engine, &sink);
    /* Not a draw: this only lets a UI notice it is never being drawn. */
    Grapple_EngineSetOverlay(engine, Grapple_UiNoteFrameCallback, ui);

    struct RClass *klass = mrb_class_get_under(mrb, mrb_module_get(mrb, "Grapple"), "Ui");
    struct RData *data = mrb_data_object_alloc(mrb, klass, ui, &kUiType);
    return mrb_obj_value(data);
}

/* ui.draw — the UI, over whatever has been drawn so far. */
static mrb_value RUiDraw(mrb_state *mrb, mrb_value self)
{
    Grapple_UiDraw((Grapple_Ui *)mrb_data_get_ptr(mrb, self, &kUiType));
    return self;
}

static mrb_value RUiPanel(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);
    Grapple_Ui *ui = (Grapple_Ui *)mrb_data_get_ptr(mrb, self, &kUiType);

    Grapple_UiPanelDef def = {0};
    def.title = OptString(mrb, options, "title", NULL);
    def.fill = OptBool(mrb, options, "fill", true);
    def.x = OptNumber(mrb, options, "x", 0.0f);
    def.y = OptNumber(mrb, options, "y", 0.0f);
    def.width = OptNumber(mrb, options, "width", 0.0f);
    def.height = OptNumber(mrb, options, "height", 0.0f);
    def.padding = OptNumber(mrb, options, "padding", 0.0f);
    def.spacing = OptNumber(mrb, options, "spacing", 0.0f);
    def.movable = OptBool(mrb, options, "movable", false);
    def.scrollable = OptBool(mrb, options, "scrollable", false);
    def.no_border = OptBool(mrb, options, "no_border", false);

    return WidgetValue(mrb, Grapple_UiPanel(ui, &def));
}

static mrb_value Strip(mrb_state *mrb, mrb_value self, bool row)
{
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);
    Grapple_UiWidget *parent = WidgetOf(mrb, self);

    Grapple_UiStripDef def = {0};
    def.height = OptLength(mrb, options, "height");
    def.spacing = OptNumber(mrb, options, "spacing", -1.0f);
    def.align = OptAlign(mrb, options);

    return WidgetValue(mrb, row ? Grapple_UiRow(parent, &def) : Grapple_UiColumn(parent, &def));
}

static mrb_value RUiRow(mrb_state *mrb, mrb_value self) { return Strip(mrb, self, true); }
static mrb_value RUiColumn(mrb_state *mrb, mrb_value self) { return Strip(mrb, self, false); }

static mrb_value RUiLabel(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);

    Grapple_UiLabelDef def = {0};
    def.text = OptString(mrb, options, "text", "");
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    def.wrap = OptBool(mrb, options, "wrap", false);

    return WidgetValue(mrb, Grapple_UiLabel(WidgetOf(mrb, self), &def));
}

static mrb_value RUiButton(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);

    block = HandlerFrom(mrb, options, block, "on_click");

    Grapple_UiButtonDef def = {0};
    def.text = OptString(mrb, options, "text", "");
    def.value = OptString(mrb, options, "value", NULL);
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    def.disabled = OptBool(mrb, options, "disabled", false);
    if (!mrb_nil_p(block))
    {
        def.on_click = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiButton(WidgetOf(mrb, self), &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiCheck(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);

    block = HandlerFrom(mrb, options, block, "on_change");

    Grapple_UiCheckDef def = {0};
    def.text = OptString(mrb, options, "text", "");
    def.checked = OptBool(mrb, options, "checked", false);
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_change = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiCheck(WidgetOf(mrb, self), &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiSlider(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);

    block = HandlerFrom(mrb, options, block, "on_change");

    Grapple_UiSliderDef def = {0};
    def.value = OptNumber(mrb, options, "value", 0.0f);
    def.min = OptNumber(mrb, options, "min", 0.0f);
    def.max = OptNumber(mrb, options, "max", 1.0f);
    def.step = OptNumber(mrb, options, "step", 0.0f);
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_change = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiSlider(WidgetOf(mrb, self), &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiEntry(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);

    block = HandlerFrom(mrb, options, block, "on_change");

    Grapple_UiEntryDef def = {0};
    def.text = OptString(mrb, options, "text", "");
    def.capacity = OptInt(mrb, options, "capacity", 0);
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_change = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiEntry(WidgetOf(mrb, self), &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

/* `options: ["Easy", "Normal"]` becomes the NULL-terminated array the C API
   wants. The strings belong to Ruby for the length of the call; the widget
   copies them. */
static const char **ReadOptions(mrb_state *mrb, mrb_value options, int *count)
{
    static const char *slots[64];
    *count = 0;
    const mrb_value list = Key(mrb, options, "options");
    if (mrb_array_p(list))
    {
        const mrb_int n = RARRAY_LEN(list);
        for (mrb_int i = 0; i < n && *count < (int)(sizeof(slots) / sizeof(slots[0])) - 1; ++i)
        {
            const mrb_value item = mrb_ary_ref(mrb, list, i);
            if (mrb_string_p(item))
            {
                slots[(*count)++] = mrb_str_to_cstr(mrb, item);
            }
            else if (mrb_symbol_p(item))
            {
                slots[(*count)++] = mrb_sym_name(mrb, mrb_symbol(item));
            }
        }
    }
    slots[*count] = NULL;
    return slots;
}

static mrb_value Select(mrb_state *mrb, mrb_value self, bool as_radio)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);
    block = HandlerFrom(mrb, options, block, "on_change");

    int count = 0;
    Grapple_UiSelectDef def = {0};
    def.options = ReadOptions(mrb, options, &count);
    /* 1-based in and out, matching Ruby's own conventions for a list of
       choices rather than C's index. */
    def.selected = OptInt(mrb, options, "selected", 1) - 1;
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_change = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *parent = WidgetOf(mrb, self);
    Grapple_UiWidget *widget =
        as_radio ? Grapple_UiRadio(parent, &def) : Grapple_UiSelect(parent, &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiSelect(mrb_state *mrb, mrb_value self) { return Select(mrb, self, false); }
static mrb_value RUiRadio(mrb_state *mrb, mrb_value self) { return Select(mrb, self, true); }

static mrb_value RUiProgress(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);
    block = HandlerFrom(mrb, options, block, "on_change");

    Grapple_UiProgressDef def = {0};
    def.value = OptNumber(mrb, options, "value", 0.0f);
    def.max = OptNumber(mrb, options, "max", 1.0f);
    def.editable = OptBool(mrb, options, "editable", false);
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_change = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiProgress(WidgetOf(mrb, self), &def);
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiImage(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_value block = mrb_nil_value();
    mrb_get_args(mrb, "|H&", &options, &block);
    block = HandlerFrom(mrb, options, block, "on_click");

    Grapple_UiImageDef def = {0};
    def.path = OptString(mrb, options, "path", NULL);
    def.value = OptString(mrb, options, "value", NULL);
    def.mode = OptImageMode(mrb, options);
    const mrb_value texture = Key(mrb, options, "texture");
    if (!mrb_nil_p(texture))
    {
        def.texture = (SDL_Texture *)GrappleGen_RubyCheckHandle(mrb, texture, "SDL_Texture");
    }
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");
    def.align = OptAlign(mrb, options);
    if (!mrb_nil_p(block))
    {
        def.on_click = ScriptCallback;
        def.user = mrb;
    }

    Grapple_UiWidget *widget = Grapple_UiImage(WidgetOf(mrb, self), &def);
    if (widget == NULL)
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "could not load image '%s'",
                   (def.path != NULL) ? def.path : "(texture)");
    }
    RememberHandler(mrb, widget, block);
    return WidgetValue(mrb, widget);
}

static mrb_value RUiSelected(mrb_state *mrb, mrb_value self)
{
    mrb_value index = mrb_nil_value();
    mrb_get_args(mrb, "|o", &index);
    Grapple_UiWidget *widget = WidgetOf(mrb, self);
    if (mrb_nil_p(index))
    {
        return mrb_fixnum_value(Grapple_UiSelected(widget) + 1);
    }
    Grapple_UiSetSelected(widget, (int)mrb_as_int(mrb, index) - 1);
    return self;
}

static mrb_value RUiOptions(mrb_state *mrb, mrb_value self)
{
    Grapple_UiWidget *widget = WidgetOf(mrb, self);
    const int count = Grapple_UiOptionCount(widget);
    mrb_value list = mrb_ary_new_capa(mrb, count);
    for (int i = 0; i < count; ++i)
    {
        mrb_ary_push(mrb, list, mrb_str_new_cstr(mrb, Grapple_UiOption(widget, i)));
    }
    return list;
}

static mrb_value RUiMessage(mrb_state *mrb, mrb_value self)
{
    /* ui.message("One") is the common case; a title is optional. */
    const char *first = NULL;
    const char *second = NULL;
    mrb_get_args(mrb, "z|z", &first, &second);
    Grapple_Ui *ui = (Grapple_Ui *)mrb_data_get_ptr(mrb, self, &kUiType);
    if (second != NULL)
    {
        Grapple_UiMessage(ui, first, second);
    }
    else
    {
        Grapple_UiMessage(ui, "", first);
    }
    return mrb_nil_value();
}

static mrb_value RUiSpacer(mrb_state *mrb, mrb_value self)
{
    mrb_value options = mrb_nil_value();
    mrb_get_args(mrb, "|H", &options);

    Grapple_UiSpacerDef def = {0};
    def.width = OptLength(mrb, options, "width");
    def.height = OptLength(mrb, options, "height");

    return WidgetValue(mrb, Grapple_UiSpacer(WidgetOf(mrb, self), &def));
}

static mrb_value RUiSet(mrb_state *mrb, mrb_value self)
{
    mrb_value value;
    mrb_get_args(mrb, "o", &value);
    Grapple_UiWidget *widget = WidgetOf(mrb, self);

    if (mrb_true_p(value) || mrb_false_p(value))
    {
        Grapple_UiSetChecked(widget, mrb_test(value));
    }
    else if (mrb_fixnum_p(value) || mrb_float_p(value))
    {
        Grapple_UiSetValue(widget, (float)mrb_as_float(mrb, value));
    }
    else
    {
        Grapple_UiSetText(widget, mrb_str_to_cstr(mrb, mrb_obj_as_string(mrb, value)));
    }
    return self;
}

static mrb_value RUiSetImage(mrb_state *mrb, mrb_value self)
{
    const char *path = NULL;
    mrb_get_args(mrb, "z", &path);
    if (!Grapple_UiSetImagePath(WidgetOf(mrb, self), path))
    {
        mrb_raisef(mrb, E_RUNTIME_ERROR, "could not load image '%s': %s", path,
                   SDL_GetError());
    }
    return self;
}

static mrb_value RUiText(mrb_state *mrb, mrb_value self)
{
    return mrb_str_new_cstr(mrb, Grapple_UiText(WidgetOf(mrb, self)));
}

static mrb_value RUiChecked(mrb_state *mrb, mrb_value self)
{
    return mrb_bool_value(Grapple_UiChecked(WidgetOf(mrb, self)));
}

static mrb_value RUiValue(mrb_state *mrb, mrb_value self)
{
    return mrb_float_value(mrb, Grapple_UiValue(WidgetOf(mrb, self)));
}

static mrb_value RUiVisible(mrb_state *mrb, mrb_value self)
{
    mrb_value value = mrb_nil_value();
    mrb_get_args(mrb, "|o", &value);
    Grapple_UiWidget *widget = WidgetOf(mrb, self);
    if (mrb_nil_p(value))
    {
        return mrb_bool_value(Grapple_UiVisible(widget));
    }
    Grapple_UiSetVisible(widget, mrb_test(value));
    return self;
}

static mrb_value RUiDisabled(mrb_state *mrb, mrb_value self)
{
    mrb_value value = mrb_nil_value();
    mrb_get_args(mrb, "|o", &value);
    Grapple_UiWidget *widget = WidgetOf(mrb, self);
    if (mrb_nil_p(value))
    {
        return mrb_bool_value(Grapple_UiDisabled(widget));
    }
    Grapple_UiSetDisabled(widget, mrb_test(value));
    return self;
}

/* What the widget means, as opposed to what it shows. */
static mrb_value RUiValueText(mrb_state *mrb, mrb_value self)
{
    mrb_value value = mrb_nil_value();
    mrb_get_args(mrb, "|o", &value);
    Grapple_UiWidget *widget = WidgetOf(mrb, self);
    if (mrb_nil_p(value))
    {
        return mrb_str_new_cstr(mrb, Grapple_UiValueText(widget));
    }
    Grapple_UiSetValueText(widget, mrb_str_to_cstr(mrb, mrb_obj_as_string(mrb, value)));
    return self;
}

static mrb_value RUiInvoke(mrb_state *mrb, mrb_value self)
{
    Grapple_UiInvoke(WidgetOf(mrb, self));
    return self;
}

static mrb_value RUiRemove(mrb_state *mrb, mrb_value self)
{
    Grapple_UiRemove(WidgetOf(mrb, self));
    DATA_PTR(self) = NULL;
    return mrb_nil_value();
}

static mrb_value RUiClear(mrb_state *mrb, mrb_value self)
{
    Grapple_UiClear(WidgetOf(mrb, self));
    return self;
}

bool Grapple_OpenRubyUi(mrb_state *mrb)
{
    if (mrb == NULL)
    {
        return SDL_InvalidParamError("mrb");
    }
    struct RClass *module = mrb_define_module(mrb, "Grapple");

    struct RClass *engine = mrb_define_class_under(mrb, module, "Engine", mrb->object_class);
    MRB_SET_INSTANCE_TT(engine, MRB_TT_DATA);
    mrb_define_method(mrb, engine, "on_load", ROnLoad, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_fixed_update", ROnFixedUpdate, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_update", ROnUpdate, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_render", ROnRender, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_post_render", ROnPostRender, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_event", ROnEvent, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_resize", ROnResize, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "on_unload", ROnUnload, MRB_ARGS_BLOCK());
    mrb_define_method(mrb, engine, "run", RRun, MRB_ARGS_NONE());
    mrb_define_method(mrb, engine, "quit", RQuit, MRB_ARGS_NONE());
    mrb_define_method(mrb, engine, "key_pressed?", RKeyPressed, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, engine, "key_down?", RKeyDown, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, engine, "size", RSize, MRB_ARGS_NONE());

    struct RClass *ui = mrb_define_class_under(mrb, module, "Ui", mrb->object_class);
    MRB_SET_INSTANCE_TT(ui, MRB_TT_DATA);
    mrb_define_method(mrb, ui, "panel", RUiPanel, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, ui, "draw", RUiDraw, MRB_ARGS_NONE());
    mrb_define_method(mrb, ui, "message", RUiMessage, MRB_ARGS_ARG(1, 1));

    struct RClass *widget = mrb_define_class_under(mrb, module, "Widget", mrb->object_class);
    MRB_SET_INSTANCE_TT(widget, MRB_TT_DATA);
    g_widget_class = widget;
    mrb_define_method(mrb, widget, "row", RUiRow, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "column", RUiColumn, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "label", RUiLabel, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "button", RUiButton, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "check", RUiCheck, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "slider", RUiSlider, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "entry", RUiEntry, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "spacer", RUiSpacer, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "select", RUiSelect, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "radio", RUiRadio, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "progress", RUiProgress, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "image", RUiImage, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
    mrb_define_method(mrb, widget, "selected", RUiSelected, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "options", RUiOptions, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "set", RUiSet, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, widget, "set_image", RUiSetImage, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, widget, "text", RUiText, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "checked?", RUiChecked, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "value", RUiValue, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "visible", RUiVisible, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "disabled", RUiDisabled, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "invoke", RUiInvoke, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "value_text", RUiValueText, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, widget, "remove", RUiRemove, MRB_ARGS_NONE());
    mrb_define_method(mrb, widget, "clear", RUiClear, MRB_ARGS_NONE());

    mrb_define_module_function(mrb, module, "engine", REngineNew, MRB_ARGS_OPT(1));
    mrb_define_module_function(mrb, module, "ui", RUiOpen, MRB_ARGS_ARG(1, 1));
    return true;
}

bool Grapple_RubyRunPendingEngine(mrb_state *mrb)
{
    Grapple_Engine *engine = PendingEngine(mrb);
    if (engine == NULL || !Grapple_ScriptHasHandlers(engine))
    {
        return false;
    }
    SetPendingEngine(mrb, NULL);
    Grapple_ScriptRun(engine);
    return true;
}
