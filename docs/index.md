---
title: Home
description: "grapple-beam: an SDL3 game runner and embeddable C/C++ engine, with Lua/Ruby scripting and C64-inspired music."
---

# grapple-beam

![grapple-beam: a 2D game engine for retro and beyond](assets/grapple-beam-logo.png)

Build a 2D game in C, C++, Lua, or Ruby. Use the **`grapple-beam` runner** for
script projects, or **embed the SDK** in your application. The source tree builds
its dependencies statically; desktop SDKs also provide shared-library targets.
Platform SDKs and OS libraries remain part of the build and runtime environment.

These pages describe current main, including the runner settings introduced after
v0.9.0. Use matching release documentation when consuming a released SDK.

## Start a game

1. [Build or install](getting-started.md), then run a Lua/Ruby project or link a C/C++ game.
2. [Configure the runner](cli-implementation.md): window modes, graphics, audio,
   preference files, scripts, CLI overrides, and recovery.
3. [Use the engine](engine.md): fixed updates, rendering, scenes, actors, input,
   physics, assets, localization, and saves.

The [Pong demos](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/README.md) show the engine loop in all four languages.
The [C++ guide](cpp.md) covers resource ownership and error handling; the
[scripting guide](scripting.md) covers callbacks, language bindings, and imports.

## Add game features

| Need | Guide |
| --- | --- |
| MIDI, MusicXML, or declarative C64-inspired music | [Authoring and supported notation](chiptune-support.md), [playback API](chiptune-musicxml.md) |
| Music, sound effects, and volume buses | [Mixer](mixer.md) |
| Menus, HUDs, and options screens | [GUI](gui.md), [C++ retained UI](cpp-gui.md) |
| Images, primitives, text, and lighting | [Image](image.md), [Gfx](gfx.md), [TTF](ttf.md), [Lighting](lighting.md) |
| Maps and collision simulation | [Tiled](tiled.md), [Physics](physics.md) |
| Asset archives, data, and utilities | [VFS](vfs.md), [Formats](formats.md), [Extras](extras.md), [Regex](regex.md) |
| Network connections and HTTP | [Net](net.md), [HTTP/S](http.md) |

[Modules](modules.md) lists CMake targets. [Platforms](platforms.md) and the
[support matrix](platforms-matrix.md) describe packaging and runtime differences.
Public headers document API ownership and failure behavior; generated
[script signatures](https://github.com/bluesentinelsec/grapple-beam/blob/main/modules/bindings/generated/SCRIPT_API.md) and
[binding coverage](https://github.com/bluesentinelsec/grapple-beam/blob/main/modules/bindings/generated/COVERAGE.md) complement these guides.
