---
title: C++
description: "Google-style C++ bindings: RAII owners for every resource, Status/Result error handling, no exceptions — plus a generated wrapper for the entire C API."
---

# C++ — `Grapple::Cpp`

Google-style C++17 bindings in `namespace grapple`: RAII wrappers for
easy cleanup and traceable control flow with **no exceptions** (the
library compiles with `-fno-exceptions`). No dependencies beyond
standard C++ — `Status`/`Result<T>` are homegrown, not Abseil.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Cpp)
```

```cpp
#include "grapple/grapple.h"
```

## Error handling: Status and Result

Every fallible factory returns `Result<T>`; every fallible operation
returns `Status`. Both are `[[nodiscard]]`, so ignoring an error is a
compile warning, and control flow stays plain `if`/early-return:

```cpp
grapple::Result<grapple::Window> window =
    grapple::Window::Create("my_game", 640, 480, 0);
if (!window.ok()) {
    SDL_Log("%s", window.status().message().c_str());
    return 1;
}
SDL_RenderClear(window->renderer());  // raw SDL stays available underneath
```

## The curated layer

Hand-written ergonomic wrappers for the game-loop essentials:

| Header | Types |
|--------|-------|
| `grapple/app.h` | `SdlInit` (init/quit guard), `Window`, `Renderer`, `Texture` (VFS-aware loading), `Surface` |
| `grapple/mixer.h` | `Mixer`, `Audio`, `Track` |
| `grapple/physics.h` | `World`, `Body`, `Vec2` |
| `grapple/filesystem.h` | `Vfs`, `Mount` (incl. encrypted archives, unmount-on-scope-exit) |
| `grapple/extras.h` | span-based compress/encrypt/base64/hash, `SignalEmitter` + `ScopedConnection` |
| `grapple/tilemap.h` | `TiledMap`, `TiledObject` |
| `grapple/ui.h` | `Gui` (immediate mode), `Ui` and `Widget` ([retained GUI](cpp-gui.md)) |
| `grapple/script.h` | `LuaState`, `RubyState` |

All owners are move-only with deleted copies; destruction order is the
inverse of declaration, and `SdlInit` makes the classic
shutdown-ordering bug (destroying audio after `SDL_Quit`) impossible to
write.

## The generated layer — the whole C API

Beyond the curated types, a generated surface (from `tools/bindgen`)
covers **every function of every module**, namespaced per library:
`grapple::sdl`, `::mix`, `::img`, `::ttf`, `::net`, `::vfs`, `::b2`,
`::nk`, `::gfx`, `::toml`, `::yaml`, `::mog`, `::json`, `::ext`.

- **RAII owners** for every resource with a create/destroy pair — 60+
  types, including by-value ids (`SDL_PropertiesID`, Box2D
  world/body ids) with validity checks:

```cpp
auto surface = grapple::sdl::Surface::CreateSurface(
    64, 48, SDL_PIXELFORMAT_RGBA8888);          // Result<Surface>
auto font = grapple::ttf::Font::OpenFont("kenney.ttf", 24);
auto doc  = grapple::json::JsonDocument::Parse(text);
auto world = grapple::b2::PhysicsWorld::CreateWorld(&def);
world->World_Step(1.0f / 60.0f, 4);              // destroyed on scope exit
```

- **`Status` wrappers** for every bool-returning fallible function.
- **Namespace aliases** for everything else — zero overhead, and the raw
  C API always remains callable.

Coverage is 100% accounted for and machine-checked: see
[`bindings/generated/COVERAGE.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/bindings/generated/COVERAGE.md).
A CI test regenerates the bindings and fails if the committed output has
drifted.

## Disabling

C++ bindings are on by default and cost nothing if unused (header-mostly).
`set(GRAPPLE_BUILD_CPP OFF)` removes them entirely.
