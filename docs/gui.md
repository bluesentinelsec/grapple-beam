---
title: GUI
description: "Nuklear immediate-mode GUI with an SDL3 backend and a weighted grid layout — tool and in-game UI with no hard-coded coordinates."
---

# GUI — `Grapple::GUI`

Nuklear v4.13.3 — the single-header immediate-mode GUI — with an original
SDL3 backend and a weighted grid layout helper. For graphical tools,
in-game UI, and general SDL applications; static everywhere SDL3 runs.

```cmake
target_link_libraries(your_app PRIVATE Grapple::GUI)
```

## Frame loop

```c
#include <grapple/gui.h>
#include <grapple/gui_grid.h>

Grapple_Gui *gui = Grapple_CreateGui(renderer, NULL, 0, 0);  /* default font */

/* each frame: */
Grapple_GuiInputBegin(gui);
while (SDL_PollEvent(&ev)) { Grapple_GuiProcessEvent(gui, &ev); }
Grapple_GuiInputEnd(gui);

struct nk_context *ctx = Grapple_GuiContext(gui);
if (nk_begin(ctx, "Inspector", nk_rect(20, 20, 280, 340),
             NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
    nk_layout_row_dynamic(ctx, 0, 1);
    if (nk_button_label(ctx, "Respawn")) { respawn(); }
    nk_checkbox_label(ctx, "God mode", &god_mode);
    nk_slider_float(ctx, 0, &volume, 1.0f, 0.01f);
}
nk_end(ctx);

SDL_RenderClear(renderer);
/* ...draw the game... */
Grapple_GuiRender(gui);          /* UI composites over the frame */
SDL_RenderPresent(renderer);
```

`Grapple_GuiWantsInput(gui)` tells the game when the UI owns the
pointer. If you don't need the raw events yourself, the whole input
block collapses to one call:

```c
while (Grapple_GuiPumpEvents(gui)) {   /* false when the user quits */
    /* ...build the UI, draw, present... */
}
```

### Inside the engine's loop, hand it the GUI instead

Everything above assumes you own the loop. If the engine owns it —
`Grapple_RunGame` — then it pumps events itself, *before* it calls any
hook, so there is no hook in which you could open Nuklear's input window.
Do not try: give the engine the GUI and it does the bracketing.

```c
const Grapple_EventSink sink = Grapple_GuiEventSink(gui);
Grapple_EngineSetEventSink(engine, &sink);
```

That is the entire integration. No `InputBegin`, no `ProcessEvent`, no
`InputEnd` anywhere in your hooks — build the UI in `update`, call
`Grapple_GuiRender` in `post_render`, and clear the sink
(`Grapple_EngineSetEventSink(engine, NULL)`) before destroying the GUI.

Scripts get the pair as one call, because a struct of function pointers
does not cross a binding:

```lua
GrappleC.AttachGui(engine, gui)
```

Nothing about `Grapple_EventSink` is GUI-specific — a debug console or a
replay recorder installs the same way, and the struct lives in
`grapple/event_sink.h` so neither module depends on the other.


## Widgets you declare once

Everything above is the immediate-mode API: you re-describe the interface
every frame, and a widget is an `if` rather than a thing. That is the right
model for a debug overlay that changes every frame, and the wrong one for a
dialog that never changes.

`<grapple/widgets.h>` is the other model — declare the tree once, bind
callbacks, and let it draw itself:

```c
Grapple_Ui *ui = Grapple_OpenUi(Grapple_EngineRenderer(engine), 15.0f);
const Grapple_EventSink sink = Grapple_UiEventSink(ui);
Grapple_EngineSetEventSink(engine, &sink);
Grapple_EngineSetOverlay(engine, Grapple_UiDrawCallback, ui);

Grapple_UiWidget *panel = Grapple_UiPanel(ui, &(Grapple_UiPanelDef){
    .title = "Settings", .fill = true, .padding = 12, .spacing = 8 });

Grapple_UiSlider(panel, &(Grapple_UiSliderDef){
    .value = 0.5f, .on_change = VolumeChanged, .user = app });

Grapple_UiButton(panel, &(Grapple_UiButtonDef){
    .text = "Close", .width = GRAPPLE_UI_FIT, .align = GRAPPLE_UI_RIGHT,
    .on_click = Close, .user = app });
```

Those three setup lines are the entire per-frame cost: `Grapple_OpenUi`
loads the platform's interface font, the sink gives it input, and the
overlay draws it after everything else. No `nk_begin`, no `nk_end`, no
input calls, and no re-declaring the panel sixty times a second.

### Lengths, so a layout survives a font change

| Written as | Means |
| --- | --- |
| `GRAPPLE_UI_PX(24)` | 24 pixels |
| `GRAPPLE_UI_EM(2.4f)` | 2.4 lines of the current font |
| `GRAPPLE_UI_PCT(0.25f)` | a quarter of the parent |
| `GRAPPLE_UI_FIT` | exactly as wide as the widget's own content |
| `{0}` (the default) | stretch to share what the fixed children left |

`GRAPPLE_UI_FIT` is the one the immediate-mode API cannot offer. It measures
the text with the live font *before* asking for the space, which is only
possible because the widget was declared before the frame it appears in —
the same reason `Grapple_GuiGridCellPart` has to take your estimate instead.

### Containers

`Grapple_UiRow` packs children side by side, `Grapple_UiColumn` stacks them,
and a panel is a column. Fixed-width children keep their width and the rest
share the remainder, which is Tk's `pack` and maps directly onto Nuklear's
row template.

Use `Grapple_UiOverlay` when widgets belong on top of one another, such as
labels over a map or controls over a preview. Direct children are drawn in
creation order, and `Grapple_UiPlace` positions their top-left corners:

```c
Grapple_UiWidget *sky = Grapple_UiOverlay(panel, &(Grapple_UiOverlayDef){
    .height = GRAPPLE_UI_PX(480) });
Grapple_UiImage(sky, &(Grapple_UiImageDef){ .path = "orion.png" });
Grapple_UiWidget *name = Grapple_UiLabel(sky, &(Grapple_UiLabelDef){
    .text = "Betelgeuse", .width = GRAPPLE_UI_FIT, .height = GRAPPLE_UI_FIT });
Grapple_UiPlace(name, GRAPPLE_UI_PCT(0.18f), GRAPPLE_UI_PCT(0.13f));
```

Percent positions follow the overlay when it resizes. Pixel and em positions
are available for fixed and font-relative placement.

### From Lua and Ruby

Each language gets its own spelling of the same tree — a table in Lua,
keyword arguments and a block in Ruby:

```lua
local ui = Grapple.ui(engine)
local panel = ui:panel{ title = "Settings", padding = 12, spacing = 8 }
local answer = panel:label{ text = "", align = "center" }
panel:button{ text = "Clear", width = "fit", align = "right",
              on_click = function() answer:set("") end }

-- Drawn where you decide, not by itself.
engine:on_post_render(function() ui:draw() end)
```

`Grapple.ui` arranges input and nothing else. Input has one correct
arrangement — Nuklear's bracket must open before the engine pumps events and
close after, and no script hook runs at that point — so it is done for you.
When the UI is drawn is a real decision, so it is yours: over a transition
or under it, skipped while a menu is closed, one panel before another.

`on_post_render` draws above the effect chain, which is usually what a menu
wants; `on_render` draws inside it, so a CRT or bloom pass will treat the UI
as part of the scene. Neither is a default worth guessing at.

Forget the call and you get a window with no interface, so a UI that has
widgets and has never been drawn logs once to say so.

```ruby
ui = Grapple.ui(engine)
panel = ui.panel(title: "Settings", padding: 12, spacing: 8)
answer = panel.label(text: "", align: :center)
panel.button(text: "Clear", width: :fit, align: :right) { answer.set("") }
```

A block is right for a one-liner. When a handler is worth a name, pass it as
`on_click:` instead — anything that answers to `call`, so a method works
without being wrapped in a block:

```ruby
def clear_clicked(_button)
  $answer.set("")
end

panel.button(text: "Clear", on_click: method(:clear_clicked))
```

The handler is given the widget that fired, so one function can serve a row
of buttons: the button knows which one it is.

Lengths take the units as strings: `24`, `"2.4em"`, `"25%"`, `"fit"`.
Widgets own their state (`answer:set(...)`, `entry:text()`,
`check:checked()`), and are owned by their parent, so nothing needs
destroying.

Overlay placement uses the same units and stays chainable:

```lua
local sky = panel:overlay{ height = 480 }
sky:image{ path = "orion.png" }
sky:label{ text = "Betelgeuse", width = "fit", height = "fit" }
   :place{ x = "18%", y = "13%" }
```

```ruby
sky = panel.overlay(height: 480)
sky.image(path: "orion.png")
sky.label(text: "Betelgeuse", width: :fit, height: :fit)
   .place(x: "18%", y: "13%")
```

### Pictures, and pictures you can click

```c
Grapple_UiImage(row, &(Grapple_UiImageDef){
    .path = "One.bmp", .on_click = NumberClicked, .user = app });
```

Give it a `path` — a BMP, loaded with plain SDL and owned by the widget — or
a `texture` you loaded yourself and continue to own. With `on_click` set it
is a button that happens to be a picture, which is what a toolbar is made
of. `width = GRAPPLE_UI_FIT` means the image's own width, not a text
measurement.

`path` reads whatever SDL_image reads — PNG, JPEG, QOI, WebP — because
Grapple::GUI links the vendored SDL3_image. Naming the file is all any of
the four languages has to do:

```lua
panel:image{ path = "logo.png", mode = "zoom", on_click = show_about }
```

Lua accepts `"stretch"`, `"zoom"`, `"center"`, or `"fill"` for `mode`;
Ruby accepts the matching symbols. The default is stretch in both languages.

Install a loader when the file is not where its name says it is — an atlas,
a pack file, a cache in front of the disk:

```c
Grapple_UiSetImageLoader(LoadFromAtlas, atlas);
```

An image widget can keep its place in the layout while its contents change:

```c
if (!Grapple_UiSetImagePath(coin, "images/tails.png")) {
    SDL_Log("Could not change coin image: %s", SDL_GetError());
}
```

The new path is loaded before the old owned texture is released, so a failed
load leaves the previous picture visible. Use `Grapple_UiSetImageTexture` to
switch to a caller-owned texture instead; passing `NULL` clears the picture.
The high-level script methods are chainable:

```lua
coin:set_image("images/tails.png")
```

```ruby
coin.set_image("images/tails.png")
```

Or hand over a texture you loaded yourself, which the widget uses as-is and
does not take ownership of.

A missing file gives you no widget rather than a picture-shaped hole that
draws nothing and reports nothing.

### Saying something to the player

```lua
ui:message("One")                    -- text only
ui:message("Number", "One")          -- title and text
```

A native message box, blocking until dismissed. It is here rather than left
to `SDL.ShowSimpleMessageBox` because that call wants flags first and the
window last, which is three things to remember for "say this".

### Choosing between options

```lua
panel:select{ options = { "Easy", "Normal", "Hard" }, selected = 2,
              on_change = difficulty_changed }   -- a dropdown
panel:radio{ options = { "Easy", "Normal", "Hard" } }  -- all of them at once
panel:progress{ value = 0.4 }
```

`selected` is 1-based going in and coming out, because that is what a list
is in both scripting languages. The widget owns the choice: `w:selected()`
gives the index, `w:text()` the chosen label, and `w:options()` the list.

### What a widget means, as opposed to what it says

```lua
row:image{ path = "two_of_clubs.jpg", value = "Two of Clubs", on_click = card_clicked }
...
local function card_clicked(image)
  name_label:set(image:value_text())
end
```

One handler shared between several widgets has to tell them apart, and the
obvious place to hang that — the text — is a label the user reads. A picture
does not have one at all, and a button's says *Exit* when what you want is
*quit*. `value` is that second string: set it in the def, read it with
`value_text()`, and the text stays free to be what the user sees.

### Firing a widget without a mouse

```lua
button:invoke()
```

What Tk calls invoke: do what the widget does, as though it had been
clicked. A keyboard shortcut, a menu item and a test all want that, and the
last of those is why it exists here — a handler that silently never fires
looks exactly like one that works until somebody clicks it.

### The escape hatch

Any wrapper over an immediate-mode library will fail to cover something, and
the answer is never "start again in the lower API":

```c
Grapple_UiRaw(panel, &(Grapple_UiRawDef){ .draw = DrawChart, .user = app });
```

The callback is handed a real `nk_context` at that point in the layout, so
anything in this document is still available inside a tree.

### Which one to use

Retained for interfaces that persist: menus, dialogs, HUDs, editors.
Immediate for interfaces that are genuinely different every frame, and for
debug overlays where declaring a tree would cost more than redrawing one.
They compose — a `raw` node is immediate mode inside a retained tree.

## From Lua and Ruby

The GUI is fully drivable from both script languages through the
generated bindings — `GrappleC.CreateGui`, `GrappleC.GuiContext`,
`GrappleC.GuiPumpEvents`, and the whole `NK.*` widget surface:

```lua
local gui = GrappleC.CreateGui(renderer, nil, 0, 0)
local ctx = GrappleC.GuiContext(gui)
while GrappleC.GuiPumpEvents(gui) do
  if NK.begin(ctx, "Tools", NK.rect(10, 10, 200, 300),
              NK.NK_WINDOW_BORDER + NK.NK_WINDOW_TITLE) then
    NK.layout_row_dynamic(ctx, 0, 1)
    if NK.button_label(ctx, "Bake") then bake() end
    local changed, value = NK.slider_float(ctx, 0, value, 1, 0.01)
  end
  NK.end_(ctx)
  GrappleC.GuiRender(gui)
  SDL.RenderPresent(renderer)
end
```

### High-DPI

Create the window with `SDL_WINDOW_HIGH_PIXEL_DENSITY` and the GUI adapts
automatically: the font is baked at the window's pixel density (so text
is crisp on Retina rather than half-size) and mouse input is scaled to
match, so hit-testing lines up. The GUI then lays out in **pixels**;
`Grapple_GuiScale(gui)` returns that density, so multiply your own
point-based sizes by it to stay density-independent:

```c
const float s = Grapple_GuiScale(gui);          /* 2.0 on Retina */
nk_layout_row_dynamic(ctx, 46.0f * s, 2);         /* 46pt row */
```

Windowless (software) renderers stay at 1.0, so headless tests and
non-Retina displays are unaffected.

### Grid layout from scripts

The full grid helper in `<grapple/gui_grid.h>` takes a caller-owned
struct and a `const float *` of column weights, neither of which can cross
a script boundary. A gui-owned mirror does the same job for every language:

```lua
GrappleC.GuiGridWeight(gui, 0, 1.0)   -- label column
GrappleC.GuiGridWeight(gui, 1, 2.0)   -- field column, twice as wide
GrappleC.GuiGridBeginOwned(gui, 2, 46 * scale)
GrappleC.GuiGridCellOwned(gui); NK.label(ctx, "Name:", NK.NK_TEXT_LEFT)
GrappleC.GuiGridCellOwned(gui); NK.button_label(ctx, "Browse")
GrappleC.GuiGridEndOwned(gui)
```

Weights default to 1 (equal columns) and reset after each grid.

### Rows, spacing and part-width cells

A grid gives every row the same height, which stops being true the moment a
panel has a heading, a row of buttons and a status line. Three calls cover
the rest, in C and in scripts (`...Owned` for the gui-owned grid):

| C | What it does |
| --- | --- |
| `Grapple_GuiGridRowHeight(&grid, h)` | height for the **next row only**; `<= 0` restores the grid default |
| `Grapple_GuiGridSpacing(&grid, x, y)` | pixel gap between cells for the rest of this grid |
| `Grapple_GuiGridCellPart(&grid, span, fraction, align)` | a cell holding a widget `fraction` as wide, pushed left, centre or right |

```c
Grapple_GuiGrid grid;
Grapple_GuiGridBegin(ctx, &grid, 3, NULL, 0);   /* 0 = a line of the font */
Grapple_GuiGridSpacing(&grid, 8, 8);

Grapple_GuiGridCellSpan(&grid, 3);
nk_label(ctx, "Settings", NK_TEXT_CENTERED);

Grapple_GuiGridRowHeight(&grid, line * 2.4f);   /* the button row, taller */
for (int i = 0; i < 3; ++i) {
    Grapple_GuiGridCell(&grid);
    nk_button_label(ctx, names[i]);
}

/* A quarter of the row, hugging the right edge. */
Grapple_GuiGridCellPart(&grid, 3, 0.25f, GRAPPLE_GUI_ALIGN_RIGHT);
nk_button_label(ctx, "Save");

Grapple_GuiGridEnd(&grid);
```

`Grapple_GuiGridSpacing` pushes a Nuklear style value and `...GridEnd` pops
it, so a dense grid and an airy one can share a panel without one leaking
into the other.

Two honest limits. Nuklear cannot measure a widget, so `fraction` is your
estimate rather than shrink-to-fit — a layout that must emit widths before
it has seen the widget cannot do better. And alignment needs blank space
*after* the widget, which cannot be emitted until you have drawn it, so the
trailing gap is settled by whatever grid call comes next; that is invisible
in normal use but means a cell's space is not final until the following
`Cell`, `NextRow` or `End`.

### Images

`Grapple_GuiImage(gui, texture, mode)` shows an `SDL_Texture` in the next
widget slot. Nuklear's own `nk_image` takes a struct whose handle is a
union — unreachable from a script — and only ever stretches; this takes the
texture directly and applies a sizing mode:

| Mode | Behaviour |
|---|---|
| `GRAPPLE_GUI_IMAGE_STRETCH` | fills the slot, ignoring aspect ratio |
| `GRAPPLE_GUI_IMAGE_ZOOM` | largest fit inside, aspect preserved |
| `GRAPPLE_GUI_IMAGE_FILL` | covers the slot, aspect preserved, cropped |
| `GRAPPLE_GUI_IMAGE_CENTER` | native size, centred |

```c
nk_layout_row_dynamic(ctx, 380.0f * scale, 1);
Grapple_GuiImage(gui, texture, GRAPPLE_GUI_IMAGE_ZOOM);
```

Load textures with [Grapple::Image](image.html) (`IMG_LoadTexture`), or
from a mounted [VFS](vfs.html) archive. Modes that can overflow the slot are
scissored to it, so an image never spills onto neighbouring widgets.

### Drawing textures yourself

`Grapple_GuiImage` takes a layout slot, which is right for a picture in a
form and wrong for a game panel: an inventory slot draws a background, an
icon and a stack count inside **one** rectangle, and the click is handled by
an invisible button occupying that same slot. For those, measure the
rectangle and draw into it:

```c
const struct nk_rect bounds = nk_widget_bounds(ctx);
const SDL_FRect icon = {bounds.x + 6, bounds.y + 6, bounds.w - 12, bounds.h - 24};
Grapple_GuiDrawTexture(gui, texture, icon, GRAPPLE_GUI_IMAGE_ZOOM);
nk_button_label(ctx, "");   /* the same slot, now handling input */
```

`Grapple_GuiDrawTexture` paints into the current window's canvas, so it is
clipped and layered with that window, and it does not advance the layout.

For something that must float above *every* panel — the icon under the
cursor during a drag — use `Grapple_GuiDrawTextureOverlay`. It queues the
draw and flushes it after the whole GUI has rendered, in call order.
Nuklear's own overlay buffer cannot be used for this: it is re-initialised
for the mouse cursor on every frame.

```c
if (dragging) {
    const SDL_FRect at = {mouse_x - 24, mouse_y - 24, 48, 48};
    Grapple_GuiDrawTextureOverlay(gui, icon, at, GRAPPLE_GUI_IMAGE_ZOOM);
}
```

The queue is emptied each frame, so call it every frame the ghost should be
visible; up to `GRAPPLE_GUI_MAX_OVERLAYS` draws per frame.

### What the frame cost

`Grapple_GuiDrawCommandCount(gui)` and `Grapple_GuiMemoryUsed(gui)`
report the last rendered frame — the two numbers a debug overlay wants.
Hiding a panel visibly drops both, which makes them useful for finding the
panel that is quietly expensive.

### Glyph ranges, and why an em dash renders as a box

A font atlas is rasterised **once**, when the GUI is created, so the glyph
range chosen there decides for the GUI's lifetime what text can be drawn.
Nuklear's default stops at U+00FF, which is why an em dash, a curly quote
or an arrow pasted into a label comes out as the missing-glyph box no
matter which font you supply.

```c
gui = Grapple_CreateGuiWithGlyphs(renderer, font, font_len, 18.0f,
                                    GRAPPLE_GUI_GLYPHS_PUNCTUATION);
```

`GRAPPLE_GUI_GLYPHS_PUNCTUATION` adds dashes, quotes, bullets, ellipsis,
arrows and currency to Latin-1 — what UI text actually contains.
`CYRILLIC`, `CHINESE` and `KOREAN` are also available, and are the only way
to use the GUI for those scripts. Wider ranges cost atlas space and baking
time, so pick the narrowest that covers your strings, and pair anything
above Latin-1 with a font that actually has the glyphs: the built-in
default font is ASCII.

### Tooltips

`Grapple_GuiTooltip(gui, text)` shows hover text for the **next** widget
with desktop timing: it appears only after the pointer has rested on that
widget, and hides again the moment the pointer moves. Nuklear's own
`nk_tooltip` draws immediately and stays up for as long as the pointer is
inside the widget, which is not how tooltips behave.

```c
Grapple_GuiTooltip(gui, "Create a new document");
nk_button_label(ctx, "New");
```

The dwell defaults to 1000 ms; `Grapple_GuiSetTooltipDelay(gui, ms)`
changes it (0 shows immediately) and `Grapple_GuiTooltipDelay` reads it
back. The call returns true on frames where the tooltip is displayed.

### File buttons that work in every browser

`Grapple_ShowOpenFileDialog` is enough on desktop, but on the web a picker
or a download only opens from inside the *real* click handler. An SDL app
sees a click one frame later, by which time Safari has withdrawn permission
— Firefox is laxer, which is why a dialog can appear to work until someone
tries it in Safari. The fix is to let the browser's own elements take the
click, so the GUI offers two widgets that are ordinary buttons on desktop
and transparent DOM overlays on the web:

```c
/* Open: the picker's result arrives through the usual dialog state machine. */
Grapple_GuiOpenFileButton(gui, "Open", "Text files", "txt");
if (Grapple_DialogStatus() == GRAPPLE_DIALOG_ACCEPTED) {
    load(Grapple_DialogPath());
    Grapple_DialogReset();
}

/* Save: pass the document's current bytes every frame — the download link
   has to hold them before the click, not after it. */
if (Grapple_GuiSaveFileButton(gui, "Save", "untitled.txt", body, len)) {
    printf("saved to %s\n", Grapple_GuiSavedPath(gui));
}
```

The save button shows the native save dialog on desktop and writes the file
itself once a path is chosen, which takes a few frames — keep calling it
with the same arguments until it returns true. On the web the bytes are
re-blobbed only when they change, so serialising each frame is cheap.

Two consequences of the overlay: the browser click never reaches Nuklear, so
these buttons do not show hover or press shading on the web, and one of each
is supported at a time — enough for a File menu, and all a modal picker can
be. The overlay is hidden automatically on any frame that does not draw the
button, so it never swallows clicks meant for other widgets.

### Two things immediate mode will catch you with

**A window's rectangle is read once.** `nk_begin` uses the rect you pass
only when it *creates* the window; afterwards the window owns its geometry,
which is exactly what makes panels movable and resizable. So panels do not
re-flow when the SDL window is resized unless you push the new bounds in:

```c
if (width != last_width || height != last_height) {
    nk_window_set_bounds(ctx, "Inventory", new_area);   /* only on resize */
}
```

Doing it every frame instead would pin the window and the user could never
move it.

**`nk_item_is_any_active` is not "is a text field focused".** It is true
whenever any panel is up, so gating keyboard navigation on it swallows every
key forever. The flag you want is the `NK_EDIT_ACTIVE` bit that
`nk_edit_string` returns for the field that has focus:

```c
const nk_flags state = nk_edit_string(ctx, NK_EDIT_FIELD, buf, &len, cap,
                                      nk_filter_default);
text_focused |= (state & NK_EDIT_ACTIVE) != 0;
```

### Keyboard and theming from scripts

`Grapple_GuiKeyPressed(gui, SDL_SCANCODE_ESCAPE)` reports keys seen
during the last pump — SDL's keyboard-state API returns a raw array that
cannot cross a binding boundary, so this is how scripts implement
"Escape quits". `Grapple_GuiPushStyleColor` / `PopStyleColor` theme the
window background, text, buttons and header for the same reason:
Nuklear's own style stack takes union-typed style items.

```lua
GrappleC.GuiPushStyleColor(gui, GrappleC.GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND,
                             {r = 28, g = 30, b = 38, a = 255})
-- ...build the window...
GrappleC.GuiPopStyleColor(gui, 1)
```

`GuiPumpEvents` exists because `SDL_Event` is a union and cannot cross a
script boundary — it is the supported way to feed input to the GUI from
Lua and Ruby. Three idioms to know: `nk_bool` crosses as a real boolean
(use truthiness, not `~= 0` — `0` is truthy in both languages),
out-parameters come back as extra return values, and enum constants are
registered by name (`NK.NK_WINDOW_TITLE`). The weighted grid helper
below stays C/C++-only: it takes a `const float *` weights array and a
caller-owned struct, so scripts use Nuklear's native
`layout_row_begin`/`push`/`end` instead. The full Nuklear widget set is available through the context:
buttons, check/radio, sliders, progress, spinboxes, single/multi-line
edit with clipboard, combos, lists, trees, menus, popups, tooltips,
charts, color picker. Always include `<grapple/nuklear.h>` (never the
raw header) so every translation unit sees the pinned configuration.

## Grid layout — no hard-coded coordinates

Column weights declared once; widgets placed cell by cell with auto-wrap
and spans. Widths are weight-proportional to the window, so the same
code lays out correctly at any display size:

```c
static const float weights[] = {1, 2};        /* label : field = 1 : 2 */
Grapple_GuiGrid grid;
Grapple_GuiGridBegin(ctx, &grid, 2, weights, 0);  /* 0 = font-based rows */

Grapple_GuiGridCell(&grid);  nk_label(ctx, "Name:", NK_TEXT_LEFT);
Grapple_GuiGridCell(&grid);  nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD,
                                                              name, sizeof(name),
                                                              nk_filter_default);
Grapple_GuiGridCellSpan(&grid, 2);  nk_button_label(ctx, "Apply");
Grapple_GuiGridEnd(&grid);
```

Tests run fully headless: pixel readback proves rendering, fabricated
SDL events drive clicks, toggles, a real slider drag, and UTF-8 text
entry.

Provenance and configuration:
[`deps/nuklear.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/nuklear.md).
