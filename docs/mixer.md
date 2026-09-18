---
title: Mixer
description: "Audio for games: SDL3_mixer with vendored codecs, statically linked MIDI, and an original chiptune synthesis layer."
---

# Mixer — `Grapple::Mixer`

A static-link-first port of SDL3_mixer 3.2.4 (the redesigned `MIX_*`
API). Every enabled decoder is vendored source compiled into the static
library — without separate codec shared libraries.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Mixer)
```

## Notation and code-authored game music

`Grapple_LoadChipSong` accepts MIDI, MusicXML and compressed MXL. Immutable songs
share a polyphonic C64-inspired player with declarative composition APIs. Named
parts select lead, pulsing harmony, bass and percussion presets; modern delay,
chorus and reverb defaults work without soundfonts or external synths.

Initialize SDL audio and `MIX_Init`, then `Grapple_PlayChipFile("level.musicxml", true)`
returns an owned managed player. Keep it alive during the game and destroy it
before audio shutdown. `PlayChipSong` plays an already imported/code-built song;
`CreateChipPlayer`/`RenderChipPlayer` provide explicit configuration and raw PCM.
The same operations are available through C++, Lua and Ruby.

See the [MusicXML contract](chiptune-musicxml.md),
[authoring guide and support matrix](chiptune-support.md),
[builds and resource limits](chiptune-validation.md), and
[runnable examples](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md).

## Format support

| Format | Status | Backend |
|--------|--------|---------|
| WAV / AIFF / AU / VOC / RAW | ✅ | built-in |
| OGG Vorbis | ✅ | stb_vorbis (vendored) |
| MP3 | ✅ | dr_mp3 (vendored) |
| FLAC | ✅ | dr_flac (vendored) |
| MIDI | ✅ | TiMidity (vendored) + generated GM patch set |
| Sine generator | ✅ | built-in |
| Opus / WavPack / tracker / GME | ❌ removed | required shared libraries — deleted, not stubbed |

Removed formats are gone from the decoder table at compile time:
unsupported files fail at `MIX_LoadAudio` with a clean error.

## Playing audio

The following is a playback fragment, after successful `SDL_Init(SDL_INIT_AUDIO)`
and `MIX_Init()`. Each pointer/boolean result must be checked; failures use
`SDL_GetError()`.

```c
#include <SDL3_mixer/SDL_mixer.h>
#include <grapple/audio_bus.h>

MIX_Mixer *mixer = Grapple_CreateAudioMixer(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
MIX_Audio *music = MIX_LoadAudio(mixer, "music_loop.ogg", false);
MIX_Track *track = MIX_CreateTrack(mixer);
Grapple_RouteAudioTrack(track, GRAPPLE_AUDIO_MUSIC);
MIX_SetTrackAudio(track, music);
MIX_PlayTrack(track, 0);
```

Keep these resources alive while the game runs. Destroy the track, audio, and
mixer in that order, then call `MIX_Quit` and `SDL_Quit` at audio/application
shutdown. The managed mixer provides master/music/SFX/speech/ambient gains and
mute controls. Ordinary C++/script playback defaults to SFX; managed chiptune
helpers use music. See [audio routing](cli-implementation.md#audio-routing) for
bus selection, live controls, and attaching buses to an existing mixer.

Assets mounted in the [VFS](vfs.md) stream directly:
`MIX_LoadAudio_IO(mixer, Grapple_OpenVFSRead("/assets/bgm.ogg"), false, true)`.

## General-MIDI decoding

TiMidity is compiled in; point `TIMIDITY_CFG` at the generated
General-MIDI patch set shipped in `modules/mixer/gm-patches/` and `.mid` files
load like any other format. Without patches configured, loads fail
cleanly rather than playing silence.

## Pre-rendered tones, sound effects, and MML

The tone/SFX/MML helpers in `<grapple/chiptune.h>` render audio during creation
into ordinary `MIX_Audio` objects. Playback then uses buffered PCM; creation costs
CPU time and stores the resulting audio. The score player above instead synthesizes
in real time:

- **`Grapple_CreateChipTone`** — one voice: square (12.5/25/50% duty),
  NES-style 16-step triangle, saw, LFSR noise (normal + metallic), or
  sine, with pitch sweep, vibrato, and attack/release envelope.
- **`Grapple_CreateChipSFX`** — canned effects: coin, laser, jump,
  explosion, powerup, hurt.
- **`Grapple_CreateChipTune`** — compiles an MML string (the 8-bit
  BASIC `PLAY` dialect) into a finished multi-channel tune:

```c
MIX_Audio *tune = Grapple_CreateChipTune(mixer,
    "T140 W1 O5 L8 C E G >C< G E C4 ;"                        /* pulse lead    */
    "T140 W2 O4 L8 E G B >E< B G E4 ;"                        /* counterpoint  */
    "T140 W3 O2 L4 C G E G ;"                                 /* triangle bass */
    "T140 W5 S2 L8 O3 C O6 C O3 C O6 C O3 C O6 C O3 C O6 C"); /* noise drums   */
```

Up to 8 `;`-separated channels; notes `A–G#`, octaves `O`/`<`/`>`,
lengths `L` and dots, tempo `T`, volume `V`, waveform `W`, envelope `S` —
the classic NES four-voice lineup (two pulse, triangle, noise) is the
example above.

Provenance and the full delete list:
[`deps/SDL3_mixer.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/SDL3_mixer.md).
