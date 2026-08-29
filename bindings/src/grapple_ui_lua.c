/*
 * grapple_ui_lua.c — the retained UI, spelled the way Lua would spell it.
 *
 * Original Grapple code (zlib). The C contract is in <grapple/ui.h>; this
 * file is about the shape of the calls, not about what they do.
 *
 *   local ui = Grapple.ui(engine)
 *   local panel = ui:panel{ title = "Settings", fill = true, padding = 8 }
 *   local answer = panel:label{ text = "", align = "center" }
 *   panel:button{ text = "Clear", width = "fit", align = "right",
 *                 on_click = function() answer:set("") end }
 *
 * Every widget takes one table, so a call says what each value means at the
 * point it is written and gains new options without gaining new arguments.
 * Lengths accept the units a layout actually thinks in: 24 is pixels,
 * "2.4em" is font-relative, "25%" is a share of the parent, "fit" is as
 * wide as the content.
 */
#include <grapple/bindings.h>
#include <grapple/engine.h>
#include <grapple/widgets.h>

#include "bindings_core.h"
#include "gen_support_lua.h"

#include <lauxlib.h>
#include <lua.h>

#include <SDL3/SDL.h>
#include <stdint.h>

#define UI_MT "grapple.ui"
#define WIDGET_MT "grapple.ui.widget"
#define CALLBACK_TABLE "grapple.ui.callbacks"

typedef struct UiBox
{
    Grapple_Ui *ui;
} UiBox;

typedef struct WidgetBox
{
    Grapple_UiWidget *widget;
} WidgetBox;

/* --- reading a table ----------------------------------------------------- */

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

static float OptNumber(lua_State *L, int table, const char *key, float fallback)
{
    lua_getfield(L, table, key);
    const float value = lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : fallback;
    lua_pop(L, 1);
    return value;
}

/* A length in the unit the caller wrote it in: a number is pixels, and the
   strings carry their own unit so a layout reads as a layout. */
static Grapple_UiLength OptLength(lua_State *L, int table, const char *key)
{
    Grapple_UiLength length = {0.0f, GRAPPLE_UI_UNIT_STRETCH};
    lua_getfield(L, table, key);
    if (lua_isnumber(L, -1))
    {
        length.value = (float)lua_tonumber(L, -1);
        length.unit = GRAPPLE_UI_UNIT_PX;
    }
    else if (lua_isstring(L, -1))
    {
        const char *text = lua_tostring(L, -1);
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
            const double value = SDL_atof(text);
            const size_t len = SDL_strlen(text);
            if (len > 0 && text[len - 1] == '%')
            {
                length.value = (float)value / 100.0f;
                length.unit = GRAPPLE_UI_UNIT_PCT;
            }
            else if (len > 2 && SDL_strcasecmp(text + len - 2, "em") == 0)
            {
                length.value = (float)value;
                length.unit = GRAPPLE_UI_UNIT_EM;
            }
            else
            {
                length.value = (float)value;
                length.unit = GRAPPLE_UI_UNIT_PX;
            }
        }
    }
    lua_pop(L, 1);
    return length;
}

static Grapple_UiAlign OptAlign(lua_State *L, int table, const char *key)
{
    const char *text = OptString(L, table, key, "left");
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

/* --- callbacks ----------------------------------------------------------- */

/* Script functions live in one registry table keyed by the widget itself, so
   a widget's handler survives exactly as long as the widget does and no
   bookkeeping leaks into the caller's code. */
static void CallbackTable(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, CALLBACK_TABLE);
    }
}

static void PushWidget(lua_State *L, Grapple_UiWidget *widget);

static void RememberCallback(lua_State *L, int table, const char *key,
                             Grapple_UiWidget *widget)
{
    lua_getfield(L, table, key);
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);
        return;
    }
    CallbackTable(L);          /* ... fn cbtable */
    lua_pushlightuserdata(L, widget);
    lua_pushvalue(L, -3);      /* the function */
    lua_rawset(L, -3);         /* cbtable[widget] = fn */
    lua_pop(L, 2);             /* cbtable, fn */
}

static void ScriptCallback(Grapple_UiWidget *widget, void *user)
{
    lua_State *L = (lua_State *)user;
    if (L == NULL)
    {
        return;
    }
    CallbackTable(L);
    lua_pushlightuserdata(L, widget);
    lua_rawget(L, -2);
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 2);
        return;
    }
    PushWidget(L, widget);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK)
    {
        /* A handler must not unwind through the engine's C frames: report
           and carry on, exactly as the engine hooks do. */
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ui callback: %s",
                     lua_tostring(L, -1) ? lua_tostring(L, -1) : "error");
        lua_pop(L, 1);
    }
    lua_pop(L, 1); /* callback table */
}

static bool HasFunction(lua_State *L, int table, const char *key)
{
    lua_getfield(L, table, key);
    const bool is_function = lua_isfunction(L, -1);
    lua_pop(L, 1);
    return is_function;
}

/* --- boxes --------------------------------------------------------------- */

static void PushWidget(lua_State *L, Grapple_UiWidget *widget)
{
    if (widget == NULL)
    {
        lua_pushnil(L);
        return;
    }
    WidgetBox *box = (WidgetBox *)lua_newuserdata(L, sizeof(*box));
    box->widget = widget;
    luaL_getmetatable(L, WIDGET_MT);
    lua_setmetatable(L, -2);
}

static Grapple_UiWidget *CheckWidget(lua_State *L, int index)
{
    WidgetBox *box = (WidgetBox *)luaL_checkudata(L, index, WIDGET_MT);
    luaL_argcheck(L, box->widget != NULL, index, "widget expected");
    return box->widget;
}

/* Every constructor takes an optional table; this keeps that uniform, and
   makes `panel:label{}` and `panel:label()` both legal. */
static int OptionsTable(lua_State *L, int index)
{
    /* Pad the stack first: an omitted argument is not a valid index, so
       lua_replace would have nothing to replace. */
    if (lua_gettop(L) < index)
    {
        lua_settop(L, index);
    }
    if (lua_isnoneornil(L, index))
    {
        lua_newtable(L);
        lua_replace(L, index);
    }
    luaL_checktype(L, index, LUA_TTABLE);
    return index;
}

/* --- constructors -------------------------------------------------------- */

/* ui:draw() — the UI, over whatever has been drawn so far. */
static int LUiDraw(lua_State *L)
{
    UiBox *box = (UiBox *)luaL_checkudata(L, 1, UI_MT);
    Grapple_UiDraw(box->ui);
    return 0;
}

static int LUiPanel(lua_State *L)
{
    UiBox *box = (UiBox *)luaL_checkudata(L, 1, UI_MT);
    const int options = OptionsTable(L, 2);

    Grapple_UiPanelDef def = {0};
    def.title = OptString(L, options, "title", NULL);
    def.fill = OptBool(L, options, "fill", true);
    def.x = OptNumber(L, options, "x", 0.0f);
    def.y = OptNumber(L, options, "y", 0.0f);
    def.width = OptNumber(L, options, "width", 0.0f);
    def.height = OptNumber(L, options, "height", 0.0f);
    def.padding = OptNumber(L, options, "padding", 0.0f);
    def.spacing = OptNumber(L, options, "spacing", 0.0f);
    def.movable = OptBool(L, options, "movable", false);
    def.scrollable = OptBool(L, options, "scrollable", false);
    def.no_border = OptBool(L, options, "no_border", false);

    PushWidget(L, Grapple_UiPanel(box->ui, &def));
    return 1;
}

static int Strip(lua_State *L, bool row)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiStripDef def = {0};
    def.height = OptLength(L, options, "height");
    def.spacing = OptNumber(L, options, "spacing", -1.0f);
    def.align = OptAlign(L, options, "align");

    PushWidget(L, row ? Grapple_UiRow(parent, &def) : Grapple_UiColumn(parent, &def));
    return 1;
}

static int LUiRow(lua_State *L) { return Strip(L, true); }
static int LUiColumn(lua_State *L) { return Strip(L, false); }

static int LUiLabel(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiLabelDef def = {0};
    def.text = OptString(L, options, "text", "");
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    def.wrap = OptBool(L, options, "wrap", false);

    PushWidget(L, Grapple_UiLabel(parent, &def));
    return 1;
}

static int LUiButton(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiButtonDef def = {0};
    def.text = OptString(L, options, "text", "");
    def.value = OptString(L, options, "value", NULL);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    def.disabled = OptBool(L, options, "disabled", false);
    if (HasFunction(L, options, "on_click"))
    {
        def.on_click = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiButton(parent, &def);
    RememberCallback(L, options, "on_click", widget);
    PushWidget(L, widget);
    return 1;
}

static int LUiCheck(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiCheckDef def = {0};
    def.text = OptString(L, options, "text", "");
    def.checked = OptBool(L, options, "checked", false);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_change"))
    {
        def.on_change = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiCheck(parent, &def);
    RememberCallback(L, options, "on_change", widget);
    PushWidget(L, widget);
    return 1;
}

static int LUiSlider(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiSliderDef def = {0};
    def.value = OptNumber(L, options, "value", 0.0f);
    def.min = OptNumber(L, options, "min", 0.0f);
    def.max = OptNumber(L, options, "max", 1.0f);
    def.step = OptNumber(L, options, "step", 0.0f);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_change"))
    {
        def.on_change = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiSlider(parent, &def);
    RememberCallback(L, options, "on_change", widget);
    PushWidget(L, widget);
    return 1;
}

static int LUiEntry(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiEntryDef def = {0};
    def.text = OptString(L, options, "text", "");
    def.capacity = (int)OptNumber(L, options, "capacity", 0.0f);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_change"))
    {
        def.on_change = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiEntry(parent, &def);
    RememberCallback(L, options, "on_change", widget);
    PushWidget(L, widget);
    return 1;
}

/* `options = { "Easy", "Normal", "Hard" }` becomes the NULL-terminated
   array the C API wants. The strings stay owned by Lua for the length of
   the call; the widget copies them. */
static const char **ReadOptions(lua_State *L, int table, int *count)
{
    static const char *slots[64];
    *count = 0;
    lua_getfield(L, table, "options");
    if (lua_istable(L, -1))
    {
        const int n = (int)lua_rawlen(L, -1);
        for (int i = 0; i < n && *count < (int)(sizeof(slots) / sizeof(slots[0])) - 1; ++i)
        {
            lua_rawgeti(L, -1, i + 1);
            if (lua_isstring(L, -1))
            {
                slots[(*count)++] = lua_tostring(L, -1);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    slots[*count] = NULL;
    return slots;
}

static int Select(lua_State *L, bool as_radio)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    int count = 0;
    Grapple_UiSelectDef def = {0};
    def.options = ReadOptions(L, options, &count);
    /* 1-based on the way in and out, because that is what a Lua table is. */
    def.selected = (int)OptNumber(L, options, "selected", 1.0f) - 1;
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_change"))
    {
        def.on_change = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget =
        as_radio ? Grapple_UiRadio(parent, &def) : Grapple_UiSelect(parent, &def);
    RememberCallback(L, options, "on_change", widget);
    PushWidget(L, widget);
    return 1;
}

static int LUiSelect(lua_State *L) { return Select(L, false); }
static int LUiRadio(lua_State *L) { return Select(L, true); }

static int LUiProgress(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiProgressDef def = {0};
    def.value = OptNumber(L, options, "value", 0.0f);
    def.max = OptNumber(L, options, "max", 1.0f);
    def.editable = OptBool(L, options, "editable", false);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_change"))
    {
        def.on_change = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiProgress(parent, &def);
    RememberCallback(L, options, "on_change", widget);
    PushWidget(L, widget);
    return 1;
}

/* The chosen option, 1-based; with an argument, chooses one. */
static int LUiSelected(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    if (lua_isnone(L, 2))
    {
        lua_pushinteger(L, (lua_Integer)Grapple_UiSelected(widget) + 1);
        return 1;
    }
    Grapple_UiSetSelected(widget, (int)luaL_checkinteger(L, 2) - 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int LUiOptions(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    const int count = Grapple_UiOptionCount(widget);
    lua_newtable(L);
    for (int i = 0; i < count; ++i)
    {
        lua_pushstring(L, Grapple_UiOption(widget, i));
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int LUiImage(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiImageDef def = {0};
    def.path = OptString(L, options, "path", NULL);
    /* A texture from IMG.LoadTexture, which reads every format SDL_image
       does — the path form is BMP only, because the GUI module depends on
       nothing but SDL. */
    lua_getfield(L, options, "texture");
    if (!lua_isnil(L, -1))
    {
        def.texture = (SDL_Texture *)GrappleGen_LuaCheckHandle(L, -1, "SDL_Texture");
    }
    lua_pop(L, 1);
    def.value = OptString(L, options, "value", NULL);
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");
    def.align = OptAlign(L, options, "align");
    if (HasFunction(L, options, "on_click"))
    {
        def.on_click = ScriptCallback;
        def.user = L;
    }

    Grapple_UiWidget *widget = Grapple_UiImage(parent, &def);
    if (widget == NULL)
    {
        return luaL_error(L, "could not load image '%s'",
                          (def.path != NULL) ? def.path : "(texture)");
    }
    RememberCallback(L, options, "on_click", widget);
    PushWidget(L, widget);
    return 1;
}

static int LUiMessage(lua_State *L)
{
    UiBox *box = (UiBox *)luaL_checkudata(L, 1, UI_MT);
    /* ui:message("One") is the common case; a title is optional. */
    const char *first = luaL_checkstring(L, 2);
    const char *second = lua_isnoneornil(L, 3) ? NULL : luaL_checkstring(L, 3);
    if (second != NULL)
    {
        Grapple_UiMessage(box->ui, first, second);
    }
    else
    {
        Grapple_UiMessage(box->ui, "", first);
    }
    return 0;
}

static int LUiSpacer(lua_State *L)
{
    Grapple_UiWidget *parent = CheckWidget(L, 1);
    const int options = OptionsTable(L, 2);

    Grapple_UiSpacerDef def = {0};
    def.width = OptLength(L, options, "width");
    def.height = OptLength(L, options, "height");

    PushWidget(L, Grapple_UiSpacer(parent, &def));
    return 1;
}

/* --- widget methods ------------------------------------------------------ */

static int LUiSet(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    if (lua_isboolean(L, 2))
    {
        Grapple_UiSetChecked(widget, lua_toboolean(L, 2) != 0);
    }
    else if (lua_isnumber(L, 2))
    {
        Grapple_UiSetValue(widget, (float)lua_tonumber(L, 2));
    }
    else
    {
        Grapple_UiSetText(widget, luaL_checkstring(L, 2));
    }
    lua_pushvalue(L, 1); /* chainable */
    return 1;
}

static int LUiGetText(lua_State *L)
{
    lua_pushstring(L, Grapple_UiText(CheckWidget(L, 1)));
    return 1;
}

static int LUiGetChecked(lua_State *L)
{
    lua_pushboolean(L, Grapple_UiChecked(CheckWidget(L, 1)) ? 1 : 0);
    return 1;
}

static int LUiGetValue(lua_State *L)
{
    lua_pushnumber(L, (lua_Number)Grapple_UiValue(CheckWidget(L, 1)));
    return 1;
}

static int LUiVisible(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    if (lua_isnone(L, 2))
    {
        lua_pushboolean(L, Grapple_UiVisible(widget) ? 1 : 0);
        return 1;
    }
    Grapple_UiSetVisible(widget, lua_toboolean(L, 2) != 0);
    lua_pushvalue(L, 1);
    return 1;
}

static int LUiDisabled(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    if (lua_isnone(L, 2))
    {
        lua_pushboolean(L, Grapple_UiDisabled(widget) ? 1 : 0);
        return 1;
    }
    Grapple_UiSetDisabled(widget, lua_toboolean(L, 2) != 0);
    lua_pushvalue(L, 1);
    return 1;
}

/* What the widget means, as opposed to what it shows. With an argument it
   sets; without one it reads. */
static int LUiValueText(lua_State *L)
{
    Grapple_UiWidget *widget = CheckWidget(L, 1);
    if (lua_isnone(L, 2))
    {
        lua_pushstring(L, Grapple_UiValueText(widget));
        return 1;
    }
    Grapple_UiSetValueText(widget, luaL_checkstring(L, 2));
    lua_pushvalue(L, 1);
    return 1;
}

static int LUiInvoke(lua_State *L)
{
    Grapple_UiInvoke(CheckWidget(L, 1));
    lua_pushvalue(L, 1);
    return 1;
}

static int LUiRemove(lua_State *L)
{
    WidgetBox *box = (WidgetBox *)luaL_checkudata(L, 1, WIDGET_MT);
    if (box->widget != NULL)
    {
        Grapple_UiRemove(box->widget);
        box->widget = NULL;
    }
    return 0;
}

static int LUiClear(lua_State *L)
{
    Grapple_UiClear(CheckWidget(L, 1));
    return 0;
}

/* --- the ui object ------------------------------------------------------- */

static int LUiGc(lua_State *L)
{
    UiBox *box = (UiBox *)luaL_checkudata(L, 1, UI_MT);
    if (box->ui != NULL)
    {
        Grapple_DestroyUi(box->ui);
        box->ui = NULL;
    }
    return 0;
}

/*
 * Grapple.ui(engine [, { font_size = 15 }])
 *
 * Opens the GUI with the platform's interface font and points the engine's
 * input at it. Drawing is deliberately not arranged here: call ui:draw()
 * from a render callback.
 *
 * The two are not the same kind of decision. Input has one correct
 * arrangement — Nuklear's bracket has to open before the engine pumps
 * events and close after, and there is no hook a script could do that from,
 * which is why the sink exists. When the UI is drawn is a real choice: over
 * a transition or under it, skipped while paused, one panel before another.
 * Hiding that choice hid the fact that a GUI is drawn at all.
 */
static int LUiOpen(lua_State *L)
{
    /* Either the engine object or a generated handle. */
    Grapple_Engine *engine = Grapple_LuaEngineAt(L, 1);
    luaL_argcheck(L, engine != NULL, 1, "engine expected");
    const int options = OptionsTable(L, 2);
    const float points = OptNumber(L, options, "font_size", 0.0f);

    Grapple_Ui *ui = Grapple_OpenUi(Grapple_EngineRenderer(engine), points);
    if (ui == NULL)
    {
        return luaL_error(L, "%s", SDL_GetError());
    }

    const Grapple_EventSink sink = Grapple_UiEventSink(ui);
    Grapple_EngineSetEventSink(engine, &sink);
    /* Not a draw: this only lets a UI notice it is never being drawn and
       say so, instead of leaving a blank window unexplained. */
    Grapple_EngineSetOverlay(engine, Grapple_UiNoteFrameCallback, ui);

    UiBox *box = (UiBox *)lua_newuserdata(L, sizeof(*box));
    box->ui = ui;
    luaL_getmetatable(L, UI_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static void MakeMetatable(lua_State *L, const char *name, const luaL_Reg *methods,
                          lua_CFunction gc)
{
    luaL_newmetatable(L, name);
    if (gc != NULL)
    {
        lua_pushcfunction(L, gc);
        lua_setfield(L, -2, "__gc");
    }
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

bool Grapple_OpenLuaUi(lua_State *L)
{
    static const luaL_Reg ui_methods[] = {
        {"panel", LUiPanel}, {"draw", LUiDraw}, {"message", LUiMessage}, {NULL, NULL}};
    static const luaL_Reg widget_methods[] = {
        {"row", LUiRow},         {"column", LUiColumn},  {"label", LUiLabel},
        {"button", LUiButton},   {"check", LUiCheck},    {"slider", LUiSlider},
        {"entry", LUiEntry},     {"spacer", LUiSpacer},  {"set", LUiSet},
        {"select", LUiSelect},   {"radio", LUiRadio},    {"progress", LUiProgress},
        {"image", LUiImage},
        {"selected", LUiSelected}, {"options", LUiOptions},
        {"text", LUiGetText},    {"checked", LUiGetChecked}, {"value", LUiGetValue},
        {"visible", LUiVisible}, {"disabled", LUiDisabled},  {"remove", LUiRemove},
        {"invoke", LUiInvoke},   {"value_text", LUiValueText},
        {"clear", LUiClear},     {NULL, NULL}};

    if (L == NULL)
    {
        return SDL_InvalidParamError("L");
    }
    MakeMetatable(L, UI_MT, ui_methods, LUiGc);
    MakeMetatable(L, WIDGET_MT, widget_methods, NULL);

    /* Added to the curated Grapple table, beside window() and world(). */
    lua_getglobal(L, "Grapple");
    if (lua_istable(L, -1))
    {
        lua_pushcfunction(L, LUiOpen);
        lua_setfield(L, -2, "ui");
    }
    lua_pop(L, 1);
    return true;
}
