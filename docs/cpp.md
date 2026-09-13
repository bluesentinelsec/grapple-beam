---
title: C++
description: "C++20 engine wrappers, resource ownership, checked Status/Result returns, and access to the C API."
---

# C++ — `Grapple::Cpp`

The C++20 API lives in `namespace grapple`. Curated wrappers provide RAII resource
ownership and `Status`/`Result<T>` error handling without exceptions. The C++ target
links its underlying engine modules and compiles with exceptions disabled; inline
wrappers also remain usable from applications with exceptions enabled.

Source consumers link `Grapple::Cpp`; installed SDK consumers link
`grapple-beam::SDKCxx` or `grapple-beam::SDKCxxShared`, with
`target_compile_features(your_game PRIVATE cxx_std_20)`. See [Getting started](getting-started.md).

## Create and run an engine

```cpp
#include <grapple/grapple.h>

int main() {
    auto sdl = grapple::SdlInit::Create(SDL_INIT_VIDEO);
    if (!sdl.ok()) {
        SDL_Log("%s", sdl.status().message().c_str());
        return 1;
    }
    Grapple_EngineConfig config = {};
    config.title = "My game";
    config.design_width = 640;
    config.design_height = 360;
    auto engine = grapple::Engine::Create(config);
    if (!engine.ok()) {
        SDL_Log("%s", engine.status().message().c_str());
        return 1;
    }
    engine->on_render([&](float alpha) {
        (void)alpha;
        auto* renderer = Grapple_EngineRenderer(engine->get());
        SDL_SetRenderDrawColor(renderer, 24, 24, 40, 255);
        SDL_RenderClear(renderer);
    });
    if (!engine->Run()) {
        SDL_Log("%s", SDL_GetError());
        return 1;
    }
    return 0;
}
```

Close the window to quit. Declare `SdlInit` before dependent owners so it is
destroyed last. Textures must not outlive their renderer, tracks must not outlive
their mixer, and borrowed actor/widget handles must not outlive their owning
engine/UI. RAII manages owned objects; it does not make every borrowed reference
independent of its parent.

## Error handling and API layers

Curated factories commonly return `Result<T>` and fallible operations return
`Status`; check `.ok()` before dereferencing. These results are `[[nodiscard]]`.
Some engine methods preserve C boolean/value returns, so consult the signature.
Use `.get()` where an operation requires the underlying C handle.

| Header | Common types and uses |
| --- | --- |
| `grapple/app.h` | `SdlInit`, `Window`, `Renderer`, `Texture`, `Surface` |
| `grapple/game.h` | `Engine`, actor handles, hooks, scenes and engine services |
| `grapple/mixer.h` | `Mixer`, `Audio`, `Track`; `Play` accepts an audio bus |
| `grapple/physics.h` | `World`, `Body`, `Vec2` |
| `grapple/filesystem.h` | `Vfs`, `Mount`, encrypted archive mounting |
| `grapple/extras.h` | Compression, encryption, base64, signals |
| `grapple/tilemap.h` | `TiledMap`, `TiledObject` |
| `grapple/ui.h` | `Gui`, `Ui`, `Widget`; [retained UI guide](cpp-gui.md) |
| `grapple/script.h` | `LuaState`, `RubyState` |
| `grapple/gen/grapple.h` | `grapple::ext` generated engine/chiptune/settings wrappers |

Generated namespaces cover additional module APIs, including `grapple::sdl`,
`mix`, `img`, `ttf`, `net`, `vfs`, `b2`, `nk`, `gfx`, `toml`, `yaml`, `mog`,
`json`, and `ext`. They contain selected resource owners, checked wrappers, and
aliases to C functions. Coverage categories and exclusions are recorded in the
[binding report](https://github.com/bluesentinelsec/grapple-beam/blob/main/bindings/generated/COVERAGE.md); availability also depends on
which modules were built. Raw C APIs remain available.

## Music and player settings

[Chiptune playback in C++](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/play.cpp) shows an owned
`grapple::ext::ChipPlayer` playing MIDI/MusicXML through the engine loop.
`ChipComposer` and `ChipSong` support code-authored compositions. Players retain
their songs; destroy players before audio shutdown. See [music authoring](chiptune-support.md).

`Mixer::Play(audio, loops, GRAPPLE_AUDIO_SPEECH)` routes speech; the default is
SFX, while managed chiptune playback uses music. [Audio routing](cli-implementation.md#audio-routing)
describes live gain and mute controls. Use the [settings API](cli-implementation.md#recovery-and-saving)
to save only player changes and inspect requested versus achieved settings.

The wrapper target is enabled by default. Set `GRAPPLE_BUILD_CPP=OFF` before
configuration to omit it, while retaining the C API.
