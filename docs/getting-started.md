---
title: Getting started
description: "Build and run a Lua/Ruby game, or embed the static or shared C/C++ SDK with CMake."
---

# Getting started

Choose the runner for Lua/Ruby projects, or link the library into a C/C++
application. Both use the same engine. These examples describe current main;
runner settings added after v0.9.0 require a build containing those changes.

## Build from source

Requirements: CMake 3.20+, a C17/C++20 compiler, Git, and the platform's development
SDK. The Makefile selects Ninja when available. Linux builds also need development
packages for the enabled SDL window/audio backends; the
[desktop workflow](https://github.com/bluesentinelsec/grapple-beam/blob/main/.github/workflows/ci.yml) records the CI package set.

```sh
git clone https://github.com/bluesentinelsec/grapple-beam.git
cd grapple-beam
make
make test
```

Windows equivalents are `build.bat debug` and `build.bat test`. Use `make release`
or `build.bat release` for optimized builds. Outputs are under `build/debug/` or
`build/release/`; multi-configuration generators may add a configuration directory.
Pass extra CMake options with `make CMAKE_FLAGS="-DOPTION=VALUE"`.

The first configure acquires pinned FetchContent dependencies; other dependency
sources are checked in. For disconnected or cross builds, supply all enabled
FetchContent sources and a CMake toolchain file. See [platforms](platforms.md) and
the [minimal music build](chiptune-validation.md) for a smaller source configuration.

## Run your first script game

For a complete C desktop workspace, use `grapple-beam new starfall --model=c`.
The [C project guide](projects.md) covers its Make/CMake build, tests, documentation,
GitHub setup, and `grapple-beam package` workflow.

Create `my-game/main.lua`:

```lua
local engine = Grapple.engine { title = "My game", design = { width = 640, height = 360 } }
local ui = Grapple.ui(engine)
local panel = ui:panel { title = "Welcome", padding = 12, spacing = 8 }
panel:label { text = "Hello from grapple-beam" }
panel:button { text = "Quit", on_click = function() engine:quit() end }
engine:on_fixed_update(function(dt)
  if engine:key_pressed("escape") then engine:quit() end
end)
engine:on_post_render(function() ui:draw() end)
```

Or create `my-game/main.rb`:

```ruby
engine = Grapple.engine(title: "My game", design: { width: 640, height: 360 })
ui = Grapple.ui(engine)
panel = ui.panel(title: "Welcome", padding: 12, spacing: 8)
panel.label(text: "Hello from grapple-beam")
panel.button(text: "Quit") { engine.quit }
engine.on_fixed_update do |dt|
  engine.quit if engine.key_pressed("escape")
end
engine.on_post_render { ui.draw }
```

```sh
./build/debug/bin/grapple-beam run --window-mode windowed ./my-game
```

Keep one entrypoint, or specify `--language lua` / `--language ruby` when both
exist. The runner starts the loop after the script registers its hooks. Close the
window or press Escape to quit. [Pong](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/README.md) is a complete game in
all four languages.

An optional `grapple.toml` provides a stable identity for player preferences:

```toml
[game]
organization = "MyStudio"
id = "MyGame"
entry = "main.lua"
language = "lua"
```

Use project `config.toml` and `config.lua` / `config.rb` for game settings;
player settings override them, and CLI overrides have highest priority.
[Runner settings](cli-implementation.md) explains source order, live changes,
saving, and recovery. Paths used by the game resolve from the project root.
Engine options go before the project; trailing arguments are passed to the game.

```sh
grapple-beam run --vsync off --max-fps 144 ./my-game -- level-2
grapple-beam run --print-settings ./my-game
grapple-beam run --safe-mode ./my-game
grapple-beam repl --language lua
grapple-beam eval --language ruby --code 'puts SDL.GetPlatform'
```

Omitting the project after `run` discovers one in the current directory. REPL and
evaluation are explicit subcommands. The runner loads game scripts; notation-file playback
uses the [music library helpers and demos](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md).

## Link an installed SDK

Download a matching desktop SDK from the
[releases](https://github.com/bluesentinelsec/grapple-beam/releases), or stage one
from your build:

```sh
cmake --install build/debug --prefix build/sdk
```

The prefix includes headers, libraries, the runner when enabled, and a CMake
package. Import the package rather than manually listing transitive system libraries:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_game LANGUAGES C CXX)
find_package(grapple-beam CONFIG REQUIRED)
add_executable(my_game main.c)
target_link_libraries(my_game PRIVATE grapple-beam::SDK)
```

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/absolute/path/to/sdk
cmake --build build
```

| Target | Use |
| --- | --- |
| `grapple-beam::SDK` | Static aggregate C API |
| `grapple-beam::SDKCxx` | Static aggregate C and C++ APIs; use with `main.cpp` |
| `grapple-beam::SDKShared` | Shared C API, when built |
| `grapple-beam::SDKCxxShared` | Shared C and C++ APIs, when built |

For a C++ executable, replace `main.c` with `main.cpp`, select the C++ target,
and request C++20 explicitly:

```cmake
target_link_libraries(my_game PRIVATE grapple-beam::SDKCxx)
target_compile_features(my_game PRIVATE cxx_std_20)
```

Choose one aggregate target. The C++ variant includes the C API. The full C SDK
also contains dependencies implemented in C++; its package supplies the runtime
link requirements. A reduced C-only music build has different requirements.
For shared linking, deploy the matching shared library as well as game assets.
Windows DLLs install in `bin/`, with import libraries in `lib/`; Unix libraries
install in `lib/`. Keep headers and binaries from the same build/version.

The [C Pong](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/pong/pong.c) example uses `Grapple_RunGame` with hooks;
the [C++ guide](cpp.md) shows checked RAII creation and cleanup.

## Build the library inside your CMake project

FetchContent makes individual `Grapple::*` source targets available:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_game LANGUAGES C CXX)
include(FetchContent)
FetchContent_Declare(grapple
  GIT_REPOSITORY https://github.com/bluesentinelsec/grapple-beam.git
  GIT_TAG 2130ce89af08abf8c8b0ac35f7247e674d4a687b)
FetchContent_MakeAvailable(grapple)
add_executable(my_game main.c)
target_link_libraries(my_game PRIVATE Grapple::Engine)
```

This revision includes the current settings API. Pin your tested release or
commit when upgrading. [Modules](modules.md) lists target names and how to disable
unneeded components. Dependencies propagate through targets; disabling a module
also requires disabling consumers that depend on it.

## Next steps

- [Engine](engine.md): update/render hooks, scenes, input, and asset lifetimes.
- [C++](cpp.md) and [Lua/Ruby](scripting.md): language-specific calling conventions.
- [Music authoring](chiptune-support.md): role names, pulse presets, supported notation.
- [Platforms](platforms.md): native packaging, mobile lifecycle, and browser restrictions.
