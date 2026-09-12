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
