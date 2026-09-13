---
title: "Runner settings"
description: "Runner settings for grapple-beam callers."
---

# Runner settings

`grapple-beam` launches Lua/Ruby game projects. The [CLI reference](cli-args.txt) lists supported arguments and defaults; `--help-all` is generated from the same typed
registry used by configuration files, scripts, and the C API.

```sh
grapple-beam --window-mode windowed --window-size 1280x720 ./my-game
grapple-beam --window-mode fullscreen-exclusive --resolution 1920x1080 --refresh-rate 60 ./my-game
grapple-beam --vsync off --max-fps 144 --effects off ./my-game
grapple-beam --music-volume .7 --speech-volume 1 ./my-game
grapple-beam --print-settings ./my-game
grapple-beam --safe-mode ./my-game
grapple-beam --list-displays
grapple-beam --list-display-modes --display primary
grapple-beam --list-backends
```

## CLI values and defaults

Use `--key value` or `--key=value`. Boolean settings require `on` or `off`;
repeated scalar options take the last value. Put all engine options before the
project. Advanced keys use `--set engine.tick_rate=120` and are listed by
`--help-all`. These keys are also available in TOML and configuration scripts.

Defaults select borderless fullscreen, primary display, vsync, display-paced FPS,
1280×720 when windowed, render scale 1, high budgets, and FXAA. Bloom, CRT, and
chromatic aberration start at zero; pixelation 1 adds no pixelation. Audio gains
start at 1 and mute is off. These are engine defaults, so games and players may
override them. See the [full option inventory](cli-args.txt) for ranges and enums.

## Projects and configuration

Launch a directory, a `.lua`/`.rb` script, or omit the path to use the current
directory. A directory discovers `main.lua` or `main.rb`; if both exist, select
`--language`. An optional `grapple.toml` declares a stable preference identity:

```toml
[game]
organization = "MyStudio"
id = "MyGame"
entry = "main.lua"
language = "lua"
```

The entrypoint stays inside the project. Without a manifest, preferences use
a hash of the absolute project root; moving the project changes that identity.
Shipping games should supply an ID. Preferences live in SDL's standard per-user,
per-application directory (Application Support on macOS, roaming AppData on Windows,
`XDG_DATA_HOME` or `~/.local/share` on Linux). `--print-settings` shows the exact directory.

Each layer supplies only its changed fields. Lowest to highest:

1. Engine defaults, with inline engine-constructor fields as game defaults.
2. Media `config.toml` (directory or readable ZIP), then project `config.toml`.
3. Project `config.lua` or `config.rb`, matching the game language.
4. Player `config.toml`.
5. Player `config.lua` or `config.rb`.
6. Each explicit `--config FILE`, in order.
7. Each explicit `--config-script FILE`, in order.
8. Individual CLI settings.

Inline constructor fields cannot override explicit file/script/CLI fields.
`--quality` expands before individual CLI fields, regardless of argument order.
Low selects low budgets, render scale .75, and AA off. Medium/high select their
matching budgets, scale 1, and FXAA. Recovery presets also precede individual flags.

TOML uses the canonical keys printed by `--print-settings`:

```toml
[display]
window_mode = "fullscreen-borderless"
vsync = true
max_fps = "display"
[effects]
bloom = 0.2
[accessibility]
ui_scale = 1.25
[audio]
music_volume = 0.7
speech_volume = 1.0
```

Lua configuration returns a table; Ruby evaluates to a hash:

```lua
return { display = { vsync = false }, audio = { music_volume = 0 } }
```

```ruby
{ display: { vsync: false }, audio: { music_volume: 0 } }
```

Configuration scripts are trusted code executed in separate interpreters before
windows/audio devices. Game bindings are unavailable there. Their working directory
is the script's directory; the game entrypoint and its imports run from the project
root. Explicit CLI paths resolve from the invoking directory; media paths inside
configuration resolve from that configuration file. Game arguments follow the
project (optionally after `--`) and are passed untouched. Asset archives remain
supported by `--media`. CLI media is selected before reading media defaults; otherwise
`media.zip`, `media.dat`, then `media/` are searched. A media setting inside a later
configuration layer changes assets without recursively reloading settings. Media
TOML can come from readable ZIP archives; game settings scripts stay beside the
entrypoint. Encrypted archive provisioning belongs to the embedding API. Packaged
project entrypoint launch is outside this CLI contract.

Unknown keys, wrong types, invalid enums, nonfinite/out-of-range numbers, missing
explicit files, and script errors fail with exit 2. Script/runtime failures return
1. Help, diagnostics, and successful execution return 0. Booleans require `on|off`
on the CLI; TOML/scripts also support native booleans. Explicit false/zero values override earlier layers.
Legacy graphics TOML accepts `image.*`, split window dimensions, old mode names,
and FPS sentinels; new preferences use canonical keys and values. The older
`Grapple_GraphicsLoadArgs` API remains permissive for existing embedders.

## Recovery and saving

`--default-settings` skips both player sources for one launch. `--reset-settings`
also moves only the player's recognized configuration files into a timestamped
backup directory. Saved games and unrelated files stay in place. Reset validates
the replacement settings before moving files and attempts rollback on a failed
move. `--reset-settings --print-settings` previews without moving files.

`--safe-mode` bypasses every configuration file/script and selects windowed
1280x720 on the primary display, software rendering, effects off, low budgets,
shadows off, and a 60 FPS cap. Recovery flags are mutually exclusive and reject
explicit configuration files. An explicitly supplied individual setting still wins.
No launch option is automatically saved. `--print-settings` reports requested
values before device creation; it still evaluates selected configuration scripts
and may create the preference directory. It does not prove a display/backend can
initialize.

Options screens should use the shared C API (also generated into C++, Lua, Ruby):

```c
const Grapple_Settings *requested = Grapple_EngineRequestedSettings(engine);
Grapple_Settings *actual = Grapple_EngineActualSettings(engine);
const char *source = Grapple_SettingsSource(requested, "vsync");
const char *policy = Grapple_SettingPolicy("vsync"); /* graphics/audio/game/restart */
/* Show requested, actual, source and policy in the menu. */
Grapple_DestroySettings(actual);

Grapple_Settings *changes = Grapple_CreateSettings();
Grapple_SettingsSet(changes, "vsync", "off", "player menu");
bool saved = Grapple_SettingsSaveChanges(changes, Grapple_SettingsPlayerPath(requested));
Grapple_DestroySettings(changes);
```

Check all allocation/API failures in production code. Save a fresh **changes**
snapshot, never the effective launch snapshot. Saving merges those fields into
existing player TOML with temporary-file replacement; CLI overrides are not copied.
If a field's source is the player configuration script, the menu should explain
that this script will override saved TOML next launch. Sources are paths (or
`CLI`, `CLI quality`, `safe mode`, `game startup`, `engine defaults`).

`SettingsApply` converts a resolved snapshot into C startup structures; strings
borrow the snapshot through engine creation. `SetLaunchSettings` installs a borrowed
main-thread snapshot for subsequent engine constructors. Engines retain an owned
requested snapshot, so it survives clearing the process launch scope. Embedders
can use these APIs without CLI11 or either interpreter.

## Runtime behavior and capabilities

Graphics settings apply through `EngineSetGraphics`. Backend, high-DPI, resizability,
media, headless, and advanced simulation-constructor choices require engine restart.
`EngineActualSettings` reports runtime graphics, backend, and timing, including
reduced-flashing's bloom cap and unsupported post-processing. Audio fields in that
snapshot remain launch requests; query the live mixer gain/mute getters below.

Exclusive display modes match enumerated resolution/rate combinations; unavailable
modes fail with a diagnostic. A missing saved monitor falls back to the primary
monitor with a message; an unavailable explicit CLI monitor fails. Fullscreen
transition failures propagate, and vsync reports the renderer's resulting state.
SDL may simulate vsync on some renderers; it is disabled for headless engines.
`max-fps` uses `display`, `unlimited`, or 10..1000, independently of vsync.

The effects master gate preserves bloom/CRT/etc. choices, while brightness,
contrast, saturation and color-blind correction remain independently enabled.
These shaders require the engine's OpenGL/GLES path. Other renderers report them
unavailable; the game continues. `DescribeRenderBackend` inspects an existing
renderer; `--list-backends` probes candidates and distinguishes compiled support
from usable drivers. API/device/vendor strings are queried for GL/GLES. Fields
not available from the query say **not reported**, including native API versions;
API family names are not presented as queried driver versions.

Engine lighting already consumes light/shadow budgets. No standard particle
emitter exists: custom emitters multiply counts by `GraphicsParticleDensity`.
Engine cameras now apply screen-shake preference during `CameraInit/CameraUpdate`;
do not multiply the amount again before `CameraShake`. Script `Grapple.ui(engine)`
scales its font and naturally sized widgets. C/C++ callers use `EngineUiPoints`
with `OpenUi`/`CreateGui`. Recreate UI after a UI-scale change; custom fixed layouts
must scale their dimensions. Custom shaders, cameras, particles and flashing game
content must explicitly honor preferences; global settings cannot rewrite game code.

## Audio routing

`Grapple_CreateAudioMixer` installs master/music/SFX/speech/ambient buses. Ordinary
script and C++ `Mixer::Play` playback use SFX; managed chiptune playback uses music. Route other tracks
with `Grapple_RouteAudioTrack`, or pass the optional bus name:

```lua
local voice = audio:play(sound, 0, "speech")
local wind = audio:play(sound, -1, "ambient")
```

```ruby
voice = audio.play(sound, 0, "speech")
wind = audio.play(sound, -1, "ambient")
```

C++ `Mixer::Play(audio, loops, GRAPPLE_AUDIO_SPEECH)` selects a bus explicitly.

`SetAudioBusGain`/`GetAudioBusGain` and `SetAudioMuted`/`AudioMuted` are live controls.
Master multiplies category gain once, and mute preserves all volume choices. Effect
returns contained in the category are included. Gains are shared across managed
mixers and atomically read by audio callbacks. Raw PCM integrations use
`ApplyAudioBus` exactly once at final output. Existing SDL_mixer integrations can
attach buses before playback and route their tracks; attaching reserves the mixer's
final post-mix callback, so custom callbacks must integrate deliberately. Per-song
synthesis settings remain composition APIs.

## Migrating older launch commands

Removed runner flags have actionable replacements: `--fullscreen`/`--windowed`
become `--window-mode`; `--no-vsync` becomes `--vsync off`; old `--with-*` recovery
flags lose `with-`; numeric FPS sentinels become `display`/`unlimited`. Top-level
`-e` and implicit REPL become explicit `eval --language ... --code ...` and
`repl --language ...`. Encryption passwords belong in the embedding API.

Rebuild SDK consumers against matching headers and libraries when upgrading;
the settings work extends public configuration, graphics, and camera structures.
The runner uses the existing CLI11, TOML, Lua, and mruby dependencies. Embedding
the settings C API does not require the CLI frontend or either script interpreter.
