---
title: Modules
description: "Every Grapple module: CMake target, what it provides, and what is vendored underneath."
---

# Modules

Every module is a static library with a `Grapple::` CMake alias target.
Link what you need; each page below covers the API surface, examples, and
the exact upstream provenance. Deeper reference material lives with the
code in each module's `README.md` and in the [`deps/`
ledgers](https://github.com/bluesentinelsec/grapple-beam/tree/main/deps),
which record every vendored version, checksum, deletion, and local fix.

| Target | Page | Provides |
|--------|------|----------|
| `Grapple::Mixer` | [Mixer](mixer.md) | audio mixing, music, sound effects, MIDI/MusicXML/MXL and C64-inspired code/score synthesis |
| `Grapple::Gfx` | [Gfx](gfx.md) | CPU drawing primitives, rotozoom, framerate manager, GPU-batched primitives |
| `Grapple::Image` | [Image](image.md) | image loading/saving, animations |
| `Grapple::TTF` | [TTF](ttf.md) | TrueType/OpenType rendering, complex-script shaping (HarfBuzz), BiDi (SheenBidi) |
| `Grapple::Net` | [Net](net.md) | TCP/UDP sockets (SDL3_net) |
| `Grapple::Http` / `mog::mog` | [HTTP/S](http.md) | HTTP/S client (mog), statically linked |
| `Grapple::Physics` | [Physics](physics.md) | Box2D v3 rigid-body physics |
| `Grapple::GUI` | [GUI](gui.md) | immediate and retained UI, SDL3 backend, grid layout |
| `Grapple::Tiled` | [Tiled](tiled.md) | Tiled map (.tmj) parsing, VFS-aware |
| `Grapple::Regex` | [Regex](regex.md) | regular expressions (Oniguruma), Ruby syntax, `Regexp` for mruby |
| `Grapple::Light` | [Lighting](lighting.md) | dynamic 2D lighting: day/night ambient, coloured lights, hard shadows |
| `Grapple::Engine` | [Engine](engine.md) | opinionated game engine: loop, scenes, actors, assets, typed settings |
| `Grapple::VFS` | [VFS](vfs.md) | PhysFS virtual filesystem, encrypted-zip mounting, asset packing |
| `Grapple::Extras` | [Extras](extras.md) | password crypto, DEFLATE compression, base64, signals/events |
| `Grapple::Formats` | [Formats](formats.md) | JSON (cJSON), TOML (tomlc99), YAML (libyaml) |
| `Grapple::Lua` / `Grapple::Ruby` | [Lua &amp; Ruby](scripting.md) | embedded Lua 5.4.8 and mruby 4.0.0 |
| `Grapple::Bindings` | [Lua &amp; Ruby](scripting.md) | curated game API + generated bindings for both languages |
| `Grapple::Cpp` | [C++](cpp.md) | Google-style RAII wrappers, `Status`/`Result`, generated surface |

## Choose targets and build options

Source consumers link individual targets; installed SDK consumers select one
aggregate target from [Getting started](getting-started.md#link-an-installed-sdk).
CMake propagates each target's library dependencies and include directories.

`GRAPPLE_BUILD_<MODULE>` controls source modules. Most default to ON; Net and
Http are disabled for Emscripten. `GRAPPLE_BEAM_BUILD_APP=OFF` omits the runner,
`GRAPPLE_BEAM_BUILD_DEMOS=OFF` omits examples, and `GRAPPLE_BUILD_SHARED_SDK=OFF`
omits shared SDKs. Set options before `FetchContent_MakeAvailable`.

The engine and language wrappers depend on several modules. Disabling a dependency
requires disabling its consumers; CMake reports invalid combinations. Use the
[ChiptuneMinimal cache](https://github.com/bluesentinelsec/grapple-beam/blob/main/cmake/ChiptuneMinimal.cmake) for a tested mixer-only
configuration instead of guessing a dependency subset. The C++ target is a static
library with substantial inline wrappers and requires C++20.

Vendored sources and FetchContent revisions are pinned. Static linking covers the
selected engine/dependency objects; operating-system libraries/frameworks still
apply. Supported formats and renderer capabilities have runtime limits even when
the relevant target builds. Consult the module pages and [platform matrix](platforms-matrix.md).
