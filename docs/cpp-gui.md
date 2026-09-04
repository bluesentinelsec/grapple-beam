---
title: C++ retained GUI
description: "Build a Tk-like interface once with ordinary C++ objects, callbacks, and automatic cleanup."
---

# A simple retained GUI in C++

The C++ retained UI is for menus, settings screens, inspectors, launchers,
and other interfaces that should be described once and then keep their own
state. It sits over Grapple's Nuklear renderer, but normal application code
does not need to call Nuklear or rebuild every widget on every frame.

The experience is deliberately close to Tk:

- create a panel;
- put rows, columns, and widgets inside it;
- attach callbacks to the widgets that do something;
- draw the finished tree once per frame.

There is no template DSL, operator overloading, or chained builder syntax.
The code is ordinary C++ with named option structures and explicit error
checks.

## A complete settings panel

Link `Grapple::Cpp` and include the C++ UI header:

```cmake
target_link_libraries(your_game PRIVATE Grapple::Cpp)
```

```cpp
#include <grapple/game.h>
#include <grapple/ui.h>
```

Create the engine before the UI so the UI is destroyed first. The factory
connects the UI to the engine's event loop; mouse, keyboard, and text input
then work without event-forwarding code.

```cpp
grapple::Result<grapple::Engine> engine_result = grapple::Engine::Create();
if (!engine_result.ok()) {
  SDL_Log("%s", engine_result.status().message().c_str());
  return 1;
}
grapple::Engine engine = std::move(engine_result).value();

grapple::Result<grapple::Ui> ui_result = grapple::Ui::CreateForEngine(engine);
if (!ui_result.ok()) {
  SDL_Log("%s", ui_result.status().message().c_str());
  return 1;
}
grapple::Ui ui = std::move(ui_result).value();
```

Describe the interface once during setup:

```cpp
grapple::PanelOptions panel_options;
panel_options.title = "Settings";
panel_options.padding = 12.0f;
panel_options.spacing = 8.0f;

grapple::Result<grapple::Widget> panel_result = ui.AddPanel(panel_options);
if (!panel_result.ok()) {
  SDL_Log("%s", panel_result.status().message().c_str());
  return 1;
}
grapple::Widget panel = std::move(panel_result).value();

grapple::LabelOptions status_options;
status_options.text = "Choose a volume";
status_options.align = grapple::UiAlign::kCenter;

grapple::Result<grapple::Widget> status_result = panel.AddLabel(status_options);
if (!status_result.ok()) {
  SDL_Log("%s", status_result.status().message().c_str());
  return 1;
}
grapple::Widget status = std::move(status_result).value();

grapple::SliderOptions volume_options;
volume_options.value = 0.5f;
volume_options.min = 0.0f;
volume_options.max = 1.0f;
volume_options.on_change = [&status](grapple::Widget volume) {
  int percent = static_cast<int>(volume.value() * 100.0f);
  status.SetText("Volume: " + std::to_string(percent) + "%");
};

grapple::Result<grapple::Widget> volume_result = panel.AddSlider(volume_options);
if (!volume_result.ok()) {
  SDL_Log("%s", volume_result.status().message().c_str());
  return 1;
}

grapple::ButtonOptions close_options;
close_options.text = "Close";
close_options.width = grapple::UiLength::Fit();
close_options.align = grapple::UiAlign::kRight;
close_options.on_click = [&engine](grapple::Widget) { engine.Quit(); };

grapple::Result<grapple::Widget> close_result = panel.AddButton(close_options);
if (!close_result.ok()) {
  SDL_Log("%s", close_result.status().message().c_str());
  return 1;
}
```

Drawing remains explicit because its position in the frame is a real design
choice. Drawing in `on_post_render` puts menus above the game's effect chain:

```cpp
engine.on_post_render([&ui]() { ui.Draw(); });
return engine.Run() ? 0 : 1;
```

Use `on_render` instead when a CRT, bloom, or other scene effect should also
apply to the interface.

## Layout reads from the parent down

A panel is a vertical container. Add a row when several widgets belong beside
one another, or a column when a group should be stacked. Children are owned by
their parent, so there is no widget cleanup code.

```cpp
grapple::LayoutOptions row_options;
row_options.spacing = 8.0f;

grapple::Result<grapple::Widget> row_result = panel.AddRow(row_options);
if (!row_result.ok()) {
  return 1;
}
grapple::Widget row = std::move(row_result).value();

grapple::LabelOptions name_options;
name_options.text = "Name";
name_options.width = grapple::UiLength::Fit();
grapple::Result<grapple::Widget> name_result = row.AddLabel(name_options);

grapple::EntryOptions entry_options;
entry_options.text = "Ada";
grapple::Result<grapple::Widget> entry_result = row.AddEntry(entry_options);

if (!name_result.ok() || !entry_result.ok()) {
  return 1;
}
grapple::Widget name_label = std::move(name_result).value();
grapple::Widget name_entry = std::move(entry_result).value();
```

Widths and heights are explicit values with readable units:

```cpp
options.width = grapple::UiLength::Pixels(120.0f);
options.width = grapple::UiLength::Em(8.0f);
options.width = grapple::UiLength::Percent(0.25f);
options.width = grapple::UiLength::Fit();
options.width = grapple::UiLength::Stretch();
```

Stretch is the default. Fixed children keep their requested size, and
stretching siblings share what remains. Layout is recalculated when the
window or font changes; callers do not maintain coordinates.

## Widgets own their state

Keep a `Widget` handle when the program needs to read or change that widget.
Text entries own their text buffer, choice widgets own their selected item,
and checks, sliders, and progress widgets own their values.

```cpp
name_entry.SetText("Player one");
std::string current_name = name_entry.text();

sound.SetChecked(false);
bool enabled = sound.checked();

difficulty.SetSelected(2);       // C++ indices are zero-based
std::string label = difficulty.text();

progress.SetValue(0.75f);
progress.SetVisible(true);
save_button.SetDisabled(false);
```

A callback receives the widget that fired. One callback can therefore serve
many buttons. `value` stores what a widget means separately from the text the
player sees:

```cpp
grapple::ButtonOptions delete_options;
delete_options.text = "Delete slot 2";
delete_options.value = "slot-2";
delete_options.on_click = [](grapple::Widget button) {
  DeleteSave(button.value_text());
};
```

`Invoke()` runs the same callback without synthesizing mouse input. It is
useful for keyboard shortcuts and focused tests.

## Available pieces

`Widget` provides `AddLabel`, `AddButton`, `AddCheck`, `AddSlider`,
`AddEntry`, `AddSelect`, `AddRadio`, `AddProgress`, `AddImage`, and
`AddSpacer`. `AddRow` and `AddColumn` create nested layouts. Images can load
from a path or borrow an `SDL_Texture`; a borrowed texture must outlive the
widget.

When the retained set does not contain a specialized widget, `AddRaw` keeps
the surrounding layout and gives one small callback direct access to the
Nuklear context:

```cpp
grapple::RawOptions chart_options;
chart_options.draw = [](nk_context* context) {
  DrawPerformanceChart(context);
};

grapple::Result<grapple::Widget> chart = panel.AddRaw(chart_options);
if (!chart.ok()) {
  return 1;
}
```

That escape hatch keeps unusual controls local. The rest of the screen can
stay in the simpler retained model.

## Standalone use

An application that owns its SDL loop can create the retained UI directly
from a renderer:

```cpp
grapple::Result<grapple::Ui> ui_result = grapple::Ui::Open(renderer);
```

Feed `ui.event_sink()` at the same begin/event/end points described in the
[C GUI guide](gui.md), then call `ui.Draw()` before presenting the renderer.
If an existing `grapple::Gui` already owns the Nuklear backend, use
`grapple::Ui::Create(gui)` instead.

## Lifetimes

`Ui` owns the entire retained tree and cleans it up automatically. `Widget`
is a small non-owning handle. It is valid until its UI is destroyed, it is
removed, or an ancestor is removed or cleared. Do not retain handles into a
subtree after calling `Remove()` or `Clear()` on that subtree.

Callbacks are kept alive by `Ui`, so captured application state must outlive
the UI or otherwise remain valid while callbacks can run.
