# grapple-beam

*A static-first SDL3 game engine for C, C++, Lua, and Ruby.*

[![CI](https://github.com/bluesentinelsec/grapple-beam/actions/workflows/ci.yml/badge.svg)](https://github.com/bluesentinelsec/grapple-beam/actions/workflows/ci.yml)
[![License: zlib](https://img.shields.io/badge/License-zlib-informational.svg)](LICENSE)

<p align="center"><img src="docs/assets/grapple-beam-logo.png" alt="grapple-beam: a 2D game engine for retro and beyond" width="420"></p>

grapple-beam provides two ways to build a game:

- **Run a Lua or Ruby project** with the `grapple-beam` executable. It hosts the
  engine loop, input, rendering, audio, and script bindings.
- **Embed the engine in C or C++** using individual CMake modules or the aggregate
  SDK. Static linking is the default; desktop SDKs also offer shared libraries.

The engine includes fixed-step simulation with interpolated rendering, scenes,
actors, physics, asset loading, UI, lighting, and player settings. Music can come
from MIDI, MusicXML, or code, with polyphonic C64-inspired instruments and modern
chorus, delay, and reverb.

[Getting started](docs/getting-started.md) · [Runner and settings](docs/cli-implementation.md) ·
[C desktop projects](docs/projects.md) ·
[Modules](docs/modules.md) · [Published documentation](https://bluesentinelsec.github.io/grapple-beam/)

## Build and run

Use CMake 3.20+, a C17/C++20 toolchain, and your platform's development SDK.
Dependencies are built from pinned source, either checked into the repository
or acquired with FetchContent. Initial configuration needs network access unless
those sources are already supplied. OS libraries and frameworks are still required.

```sh
git clone https://github.com/bluesentinelsec/grapple-beam.git
cd grapple-beam
make
./build/debug/bin/grapple-beam run --window-mode windowed demos/pong/pong.lua
./build/debug/bin/grapple-beam run --window-mode windowed demos/pong/pong.rb
```

On Windows, use `build.bat debug`; the executable is under `build/debug/bin/`
(with a configuration subdirectory when using a multi-configuration generator).
`make test` / `build.bat test` builds and runs the tests. `make release` builds
an optimized runner and libraries.

These guides describe **current main**, including settings added after v0.9.0.
For a released SDK, use the documentation bundled with that release and matching
headers/libraries.

## Create a C desktop game

```sh
grapple-beam new starfall --model=c
cd starfall
make                 # Debug; equivalent to make build.
make test
grapple-beam package
```

The generated project includes CMake/FetchContent integration, a playable C starter,
media/configuration, tests, player docs, Git initialization, and desktop CI/release
workflows. Its five Make targets are `build`, `build-release`, `test`, `fmt`, and
`clean`; Windows gets matching `build.bat` operations. Game sources under
`src/starfall/` are discovered automatically. Packaging belongs to the CLI.
See [C desktop projects](docs/projects.md) for version pins, optional GitHub creation,
requirements, and distribution details. These CLI commands require a build containing
project support; the default engine source pin can still use v0.9.0.

## Launch and configure a script game

```sh
grapple-beam run --window-mode fullscreen-borderless ./my-game
grapple-beam run --vsync off --max-fps 144 --music-volume .7 ./my-game
grapple-beam run --print-settings ./my-game
grapple-beam run --safe-mode ./my-game
grapple-beam repl --language lua
```

A project directory contains `main.lua` or `main.rb`; an optional `grapple.toml`
selects an entrypoint and stable player-preference identity. With no path, `run`
discovers a project in the current directory. Engine options precede the
project; trailing arguments belong to the game. `grapple-beam --help` lists
`new`, `package`, `run`, `repl`, and `eval`.

Settings merge from engine/game defaults, project TOML and scripts, player TOML
and scripts, explicit configuration files, then CLI overrides. Scripts win over
TOML within each tier. `--default-settings`, `--reset-settings`, and `--safe-mode`
provide recovery; CLI changes are never automatically saved. See the
[settings guide](docs/cli-implementation.md) for the exact order, persistence API,
display modes, graphics capabilities, accessibility controls, and audio buses.

## Embed the library

An installed SDK exports CMake targets with platform link requirements:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_game LANGUAGES C CXX)
find_package(grapple-beam CONFIG REQUIRED)
add_executable(my_game main.c)
target_link_libraries(my_game PRIVATE grapple-beam::SDK)
```

Configure with `-DCMAKE_PREFIX_PATH=/path/to/sdk`. For C++, use
`grapple-beam::SDKCxx` and request `cxx_std_20` on your executable; for dynamic linking, use `SDKShared` or `SDKCxxShared`.
[Getting started](docs/getting-started.md) also covers FetchContent and selecting
individual `Grapple::*` source targets.

## Learn by task

| Task | Guide | Examples |
| --- | --- | --- |
| Build the game loop, scenes, actors, input, saves | [Engine](docs/engine.md) | [Pong in four languages](demos/pong/) |
| Use RAII and check errors in C++ | [C++](docs/cpp.md) | [C++ Pong](demos/pong/pong.cpp) |
| Script a game or embed an interpreter | [Lua and Ruby](docs/scripting.md) | [Demos](demos/README.md) |
| Compose and play MIDI/MusicXML or code | [Music authoring](docs/chiptune-support.md) | [C, C++, Lua, Ruby playback](demos/chiptune/README.md) |
| Mix music, effects, speech, and ambience | [Audio](docs/mixer.md) | [Mixer APIs](modules/mixer/README.md) |
| Build menus and options screens | [GUI](docs/gui.md), [C++ UI](docs/cpp-gui.md) | [Settings API](docs/cli-implementation.md#recovery-and-saving) |
| Package assets and load from archives | [VFS](docs/vfs.md) | [Asset packer](scripts/pack_assets.py) |
| Select modules or target mobile/web | [Modules](docs/modules.md), [Platforms](docs/platforms.md) | [Platform matrix](docs/platforms-matrix.md) |

## Dependencies, versioning, and license

Dependency versions, licenses, and local changes are recorded in [deps/](deps/)
and [CMake dependency declarations](cmake/Dependencies.cmake). Module feature
availability varies by platform and build options; see each module's guide.
Generated binding coverage and exclusions are recorded in the
[binding report](modules/bindings/generated/COVERAGE.md).

The root `VERSION` file identifies the project version. Before 1.0, minor releases
may include breaking API changes; review release notes when upgrading.
Original code uses the [zlib license](LICENSE). Dependencies retain their own
licenses, including the terms listed in their provenance records.
