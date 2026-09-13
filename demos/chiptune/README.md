# Chiptune playback demo

[`play.c`](play.c) shows loading, diagnostics, real-time playback through the
engine loop, ownership/cleanup, and optional offline WAV export. MIDI, MusicXML
and MXL all use the same library calls. It is a standalone demo, not a mode of
the `grapple-beam` game runner, and is never installed in the SDK.

After `make`, run from the repository root:

```sh
# Play each of the supplied Guitar Pro exports.
./build/debug/bin/chiptune-demo tests/mixer/assets/c64-composition-named.mid
./build/debug/bin/chiptune-demo tests/mixer/assets/c64-composition-named.xml

# Compressed MusicXML works through the same loader.
./build/debug/bin/chiptune-demo tests/mixer/assets/c64-composition.mxl

# A second positional argument exports a WAV instead of opening a device/window.
./build/debug/bin/chiptune-demo tests/mixer/assets/c64-composition-named.xml build/c64-demo.wav
```

The window closes when the song and effect tails finish. Closing it early stops
playback. Role words in track names choose melody, harmony, bass and percussion;
no index overrides or soundfonts are needed. The harmony defaults to wet,
ring-modulated eighth-note pulses. The example has 87 notes across the four
musical parts, 120 BPM and ten seconds of music, followed by effect tails.

To embed playback, initialize SDL audio and `MIX_Init()`, then:

```c
Grapple_ChipSong *song = Grapple_LoadChipSong("level.musicxml");
if (!song) { return false; /* report SDL_GetError() */ }
Grapple_ChipPlayer *player = Grapple_PlayChipSong(song, true);
Grapple_DestroyChipSong(song); /* player keeps its own reference */
/* Keep player alive while the game runs. */
Grapple_DestroyChipPlayer(player); /* before MIX_Quit / SDL_Quit */
```

For a single call, `Grapple_PlayChipFile("level.mid", true)` returns an owned
player with the same defaults. For explicit sample rate, voice limit or initial
settings, use `Grapple_CreateChipPlayer` and `Grapple_PlayChipPlayer`.

Build only this example with `cmake --build build/debug --target chiptune_demo`.
Set `GRAPPLE_BEAM_BUILD_DEMOS=OFF` for a build without demo executables.
CTest plays both supplied formats using dummy audio and a headless engine,
and separately renders WAVs without an audio device.

The same helper is demonstrated with C++ RAII in [`play.cpp`](play.cpp):

```sh
./build/debug/bin/chiptune-cpp tests/mixer/assets/c64-composition.mxl
```

[`play.lua`](play.lua) and [`play.rb`](play.rb) run inside the canonical engine
runner. Their `code` argument selects the small declarative four-part composition
in each script. Otherwise they use the file helper, which accepts any of the
three supported formats:

```sh
./grapple-beam demos/chiptune/play.lua "$PWD/tests/mixer/assets/c64-composition-named.mid"
./grapple-beam demos/chiptune/play.rb "$PWD/tests/mixer/assets/c64-composition-named.xml"
./grapple-beam demos/chiptune/play.lua code
./grapple-beam demos/chiptune/play.rb code
```

The runner changes to the script project directory. These file arguments use
absolute paths so the fixture is found regardless of that working directory.
Pass a score path explicitly when running the script demos.

For declarative C/C++ composition, see the [mixer examples](../../mixer/README.md#compose-music-in-code-c-c-lua-ruby).
All examples own their player through the game loop and destroy it before audio
shutdown. The generated C++ wrappers use `grapple::ext::ChipSong`, `ChipComposer`
and `ChipPlayer`; Lua/Ruby use `GrappleC`. Import options and diagnostic records are
plain structs in C/C++, tables in Lua, and hashes in Ruby. Players retain songs,
so releasing the caller's song handle does not interrupt playback.
