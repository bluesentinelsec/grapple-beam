# Grapple::Mixer

A static-link-first port of [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
3.2.4 (the redesigned `MIX_*` API). No shared codec libraries — every enabled
decoder is vendored source, compiled into `libGrapple_Mixer.a`.

Provenance and the full delete list: [`deps/SDL3_mixer.md`](../deps/SDL3_mixer.md).

## Format support

| Format | Status | Backend |
|--------|--------|---------|
| WAV | ✅ | built-in |
| OGG Vorbis | ✅ | stb_vorbis (vendored) |
| MP3 | ✅ | dr_mp3 (vendored) |
| FLAC | ✅ | dr_flac (vendored) |
| AIFF | ✅ | built-in |
| AU | ✅ | built-in |
| VOC | ✅ | built-in |
| RAW PCM | ✅ | built-in |
| Sine wave generator | ✅ | built-in |
| MIDI | ✅ | TiMidity (vendored, statically linked) + the **generated GM patch set** in [`gm-patches/`](gm-patches/README.md) (point `TIMIDITY_CFG` at its `timidity.cfg`); without patches, loads fail cleanly |
| Opus | ❌ removed | required libopus/libopusfile |
| WavPack | ❌ removed | required libwavpack |
| Tracker (MOD/XM/IT/S3M) | ❌ removed | required libxmp |
| Game-console music (GME) | ❌ removed | required libgme |
| SoundFont MIDI | ❌ removed | required FluidSynth (TiMidity covers MIDI) |

Removed formats are gone from the build entirely (no stubs): the decoder
table is compile-time gated, so unsupported files fail at `MIX_LoadAudio`
with a clean error, and nothing references the deleted code.

## Usage

```cmake
target_link_libraries(your_game PRIVATE Grapple::Mixer)
```

```c
#include <SDL3_mixer/SDL_mixer.h>

MIX_Init();
MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
MIX_Audio *music = MIX_LoadAudio(mixer, "music_loop.ogg", false);
MIX_Track *track = MIX_CreateTrack(mixer);
MIX_SetTrackAudio(track, music);
MIX_PlayTrack(track, 0);
```

## Chiptune synthesis (Grapple extension)

Original code (not vendored SDL_mixer): `<grapple/chiptune.h>` adds 8-bit
era synthesis with high-level entry points, rendered offline into normal
`MIX_Audio` objects — deterministic, loopable, zero runtime cost beyond
ordinary playback.

- `Grapple_CreateChipTone` — one voice: square (12.5/25/50% duty), 16-step
  NES-style triangle, saw, LFSR noise (normal + metallic short-loop), or
  sine, with pitch sweep, vibrato, and attack/release envelope.
- `Grapple_CreateChipSFX` — canned effects: coin, laser, jump, explosion,
  powerup, hurt.
- `Grapple_CreateChipTune` — compiles an MML string (the 8-bit BASIC
  `PLAY` dialect: notes `A-G#`, octaves `O`/`<`/`>`, lengths `L`/dots, tempo
  `T`, volume `V`, waveform `W`, envelope shape `S`, up to 8 `;`-separated
  channels) into a finished multi-channel tune — here the classic NES
  four-voice lineup:

```c
MIX_Audio *tune = Grapple_CreateChipTune(mixer,
    "T140 W1 O5 L8 C E G >C< G E C4 ;"                     /* pulse lead   */
    "T140 W2 O4 L8 E G B >E< B G E4 ;"                     /* counterpoint */
    "T140 W3 O2 L4 C G E G ;"                              /* triangle bass */
    "T140 W5 S2 L8 O3 C O6 C O3 C O6 C O3 C O6 C O3 C O6 C"); /* noise drums */
```

Tests (`tests/mixer/`) decode a synthesized corpus in every enabled format,
assert real signal energy from headless `MIX_Generate` playback, fuzz
malformed inputs under ASan+UBSan, and run a link audit proving the test
binary depends only on OS-built-in shared libraries.

## Streaming SID-style MIDI playback

The same `<grapple/chiptune.h>` now includes a polyphonic C synthesizer and
MIDI sequencer. It synthesizes audio on demand, with no instrument files,
TiMidity configuration, additional dependencies, or whole-song PCM allocation.
This is SID-inspired sound design, not cycle-exact 6581/8580 emulation or a
`.sid` file player. The original tone/SFX/MML APIs retain their offline behavior.

Built-in presets: `LEAD` (moving pulse width and vibrato), `BASS` (filtered
pulse/saw), `CHORD` (polyphonic pulse pad), `RING` (triangle carrier with
oscillator-controlled polarity inversion), and `DRUMS` (GM note mapping to
kick, snare, hats, toms, and cymbal-like noise). `HARMONY` aliases `RING` with
the default wet, eighth-note pulse. `AUTO` first resolves role words in the part
name, using the contract below. Unrecognized names fall back to MIDI channel
10 for drums, GM programs 32–39 for bass, 88–95 for `CHORD`, and other programs
for lead. Program numbers are zero-based.
Ring modulation uses a private oscillator at twice each note's frequency;
all chord notes sound together and no other instrument loses a voice.

The player supports 1–1024 simultaneous note voices; start with 64. Releasing
notes also occupy voices. At capacity it replaces the quietest releasing
voice, or the oldest held voice. Oscillators run at four times the output
sample rate, with corrected pulse/saw edges and per-note filtering. Each
preset has its own envelope; master soft saturation prevents clipping.

### Track-name contract

Name parts in Guitar Pro, or in `Grapple_SetChipPart`, using these role words:

| Word in the name (case insensitive) | Preset, including default effects |
| --- | --- |
| `melody`, `lead` | `LEAD`: delay and reverb |
| `harmony`, `chords` | `HARMONY`: ring modulation and wet eighth-note pulse |
| `bass` | `BASS`: chorus |
| `percussion`, `percussions`, `drums` | `DRUMS`: dry |

Whole words match anywhere: `Main LEAD`, `[harmony] Guitar`, and `Bass_2` work.
ASCII punctuation and whitespace separate words; digits belong to words, so
`Lead2`, `Bassoon`, and `Disharmony` do not match. Place the role within the
stored name's first 127 bytes. If multiple roles occur, the first recognized
word wins; prefer one role per name. Any number of parts may share a role,
within the song's 256-track and player's configured voice limits. They retain
their individual notes and gains, while sharing preset effects and pulse timing.

An explicit preset in `SetChipPart` or `SetChipTrackPreset` takes precedence.
Setting the player track back to `AUTO` restores name matching, then the MIDI
fallback above. Name matches apply to all channels in a file track and remain
stable across MIDI program changes. Without a name match, `AUTO` continues to
follow program changes. Metadata-only tracks produce no voices.

The same rule serves MIDI, MusicXML/MXL and compositions authored in C,
C++, Lua and Ruby. Format detection uses file contents.

### Audition the example

The standalone [chiptune demo](../demos/chiptune/README.md) loads and plays the
supplied MIDI and MusicXML through the library. `grapple-beam` runs games and
scripts; music loading belongs in game code.

After initializing SDL audio and `MIX_Init()`, the shortest playback path is:

```c
Grapple_ChipPlayer *music = Grapple_PlayChipFile("music.musicxml", true);
if (!music) { return false; /* SDL_GetError() describes the failure. */ }
/* Keep music alive while the game runs, then destroy it before MIX_Quit(). */
Grapple_DestroyChipPlayer(music);
```

Use `Grapple_LoadChipSong` followed by `Grapple_PlayChipSong` to inspect/reuse a
song. `Grapple_LoadChipSongMemory` copies parsed content from a caller-owned
buffer. `Grapple_SaveChipSongWav(song, path, 48000, 64)` renders a finite stereo
16-bit WAV including effect tails without opening an audio device. Rendering
is blocking and belongs on a loading/tool thread; it does not modify a player.

### Use from a game

After initializing SDL audio and `MIX_Init()`, load and configure at startup:

```c
Grapple_ChipSong *song = Grapple_LoadChipSong("music.mid");
if (!song) { return false; /* SDL_GetError() describes the failure. */ }
Grapple_ChipPlayer *player = Grapple_CreateChipPlayer(song, 48000, 64, true);
if (!player) { Grapple_DestroyChipSong(song); return false; }

/* Select the actual harmony track in this file; inspect GetChipTrackInfo. */
bool configured = Grapple_SetChipTrackPreset(player, 2, GRAPPLE_CHIP_PRESET_RING, 1.0f);
MIX_Track *track = configured ? MIX_CreateTrack(mixer) : NULL;
bool playing = track &&
    MIX_SetTrackAudioStream(track, Grapple_GetChipPlayerStream(player)) &&
    MIX_PlayTrack(track, 0);
/* Check playing; report SDL_GetError() on failure. */

/* During gameplay: MIX_PauseTrack, MIX_ResumeTrack, MIX_SetTrackGain, etc. */

/* On shutdown, this order matters: */
MIX_DestroyTrack(track);
Grapple_DestroyChipPlayer(player);
Grapple_DestroyChipSong(song);
```

The player retains its song and owns its audio stream. A stream can feed one
mixer track. Destroy/detach the track before destroying its player; releasing the caller's song reference while a player exists is safe. Use `Grapple_LoadChipSong_IO()` with
`Grapple_OpenVFSRead()` for packaged media, or with `SDL_IOFromConstMem()` for
embedded MIDI bytes. No seekable input is required.

`Grapple_RenderChipPlayer()` provides allocation-free direct rendering into
stereo float PCM, useful for tests and offline export. Do not call it while a
stream consumer or another API call is using that player. Preset/gain changes,
reset, and peak-voice queries lock the owned stream and are safe during stream
playback. They are control-thread operations; the synth callback itself does
no file I/O, parsing, allocation, or application-lock acquisition. SDL manages
its own stream buffering and synchronization. Live edits can be delayed by
already-queued audio. Use mixer pause/resume for transport.

### Timing and import contract

- Standard MIDI Files types 0 and 1, with PPQN division, running status, tempo
  changes, track names, note-on/off, and velocity. Initial/trailing rests and
  end-of-track timing are preserved. Notes are owned by file track and channel;
  MIDI controllers are shared by channel, as in a single MIDI port.
- Volume (CC7), expression (CC11), pan (CC10), modulation (CC1), sustain (CC64),
  pitch bend, RPN 0 pitch-bend sensitivity (CC101/100/6/38), controller reset,
  all-notes-off, all-sound-off, and program changes. Percussion uses one-shot
  envelopes; closed/pedal hi-hat chokes open hi-hat.
- Unsupported controllers and pressure are ignored. SysEx and other metadata
  are skipped; no GS/XG setup or instrument-specific Guitar Pro effects are
  emulated. Nonzero MIDI ports, type 2, and SMPTE division fail explicitly.
- Bounded import: 64 MiB, 256 file tracks, one million retained events, 24 hours.
- Event time is accumulated as integer tick × microsecond numerators across
  tempo changes. Fractional values are carried until the final nearest-sample
  conversion. Events split audio blocks at their sample position; there is no
  frame-loop scheduling or millisecond quantization. Identical inputs produce
  identical output regardless of rendering block size within the same build.
- Non-looping playback drains release/effect tails. Looping resets all controllers,
  notes, and oscillator state at the musical end and uses a 2 ms edge fade;
  notes are not tied across the loop; effect tails carry across it. Each iteration uses the musical duration
  rounded to the nearest output sample. Reset retains preset/gain assignments.

MIDI can represent triplets precisely when the tick resolution accommodates
them: at 480 PPQN, eighth-note triplets are 160 ticks. A player cannot repair
incorrect timing already written by an exporter. Keep the original score for
comparison and check that repeats/swing were exported as intended.

MusicXML is the proposed next importer: it preserves explicit tuplet ratios,
notation durations, ties, and other score structure. Its interpreter should
produce the same internal event timeline, keeping synthesis separate from
notation. Repeats, tempo directions, grace notes and swing need an explicit
interpretation policy; XML alone does not guarantee matching performances.
MusicXML partwise/timewise and MXL import are now available through the same loader.
See [MusicXML playback](../docs/chiptune-musicxml.md) for the current support
matrix, exact timing, staff selection, diagnostics and remaining implementation work.

References: [Guitar Pro MIDI export](https://www.guitar-pro.com/docs/gp8/import-export/export/export-midi),
[MuseScore export settings](https://handbook.musescore.org/file-management/file-export),
[MusicXML tuplet representation](https://www.w3.org/2021/06/musicxml40/musicxml-reference/elements/time-modification/).

## Compose music in code (C, C++, Lua, Ruby)

`Grapple_ChipComposer` builds immutable `Grapple_ChipSong` snapshots from note
and tempo data. Notes can be declared in any order. Multiple notes at the same
tick form a chord. At 480 ticks per quarter note, use 240 ticks for eighths,
160 for eighth-note triplets, and 1920 for a whole note in 4/4. The musical end
can include trailing rests. A composer may be edited or destroyed after building;
players retain their songs, so releasing the caller's song handle is safe.
Composer edits are single-threaded; each player has independent playback state.

C uses declared note arrays and a small builder (check all returned pointers and
booleans in application code):

```c
Grapple_ChipComposer *composer = Grapple_CreateChipComposer(1, 480);
Grapple_SetChipPart(composer, 0, "harmony", GRAPPLE_CHIP_PRESET_HARMONY, 1.0f);
const Grapple_ChipNote chord[] = {
    {0, 48, 80, 0, 1920},
    {0, 52, 80, 0, 1920},
    {0, 55, 80, 0, 1920},
};
for (size_t i = 0; i < sizeof(chord) / sizeof(chord[0]); ++i)
    Grapple_AddChipNote(composer, &chord[i]);
Grapple_AddChipTempo(composer, 0, 120.0);
Grapple_ChipSong *song = Grapple_BuildChipSong(composer, 1920);
Grapple_DestroyChipComposer(composer);
Grapple_ChipPlayer *player = Grapple_CreateChipPlayer(song, 48000, 64, true);
Grapple_DestroyChipSong(song); /* The player has its own reference. */
```

For C++, `<grapple/gen/grapple.h>` (also in `<grapple/grapple.h>`) provides
move-only RAII owners and `Result`/`Status` error handling:

```cpp
auto composer = grapple::ext::ChipComposer::CreateChipComposer(1, 480);
if (!composer.ok()) { return; }
if (!composer->SetChipPart(0, "harmony", GRAPPLE_CHIP_PRESET_HARMONY, 1.0f).ok()) { return; }
const Grapple_ChipNote chord[] = {
    {0, 48, 80, 0, 1920}, {0, 52, 80, 0, 1920}, {0, 55, 80, 0, 1920}
};
for (const auto &note : chord)
    if (!composer->AddChipNote(&note).ok()) { return; }
auto song = grapple::ext::ChipSong::BuildChipSong(composer->get(), 1920);
if (!song.ok()) { return; }
auto player = grapple::ext::ChipPlayer::CreateChipPlayer(song->get(), 48000, 64, true);
if (!player.ok()) { return; }
// Keep player alive while playback is needed.
if (!player->PlayChipPlayer().ok()) { return; }
```

Lua's generated `GrappleC` surface accepts note tables and owns handles through GC:

```lua
local G = GrappleC
local composer = assert(G.CreateChipComposer(1, 480))
assert(G.SetChipPart(composer, 0, "harmony", G.GRAPPLE_CHIP_PRESET_HARMONY, 1))
local notes = {
    {track=0, note=48, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=52, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=55, velocity=80, start_tick=0, duration_ticks=1920},
}
for _, note in ipairs(notes) do assert(G.AddChipNote(composer, note)) end
local song = assert(G.BuildChipSong(composer, 1920))
local player = assert(G.CreateChipPlayer(song, 48000, 64, true))
assert(G.PlayChipPlayer(player))
-- Keep player reachable. PauseChipPlayer / PlayChipPlayer pause/resume.
-- StopChipPlayer detaches and rewinds; DestroyChipPlayer releases immediately.
```

Ruby uses the same `GrappleC` functions, note hashes, and GC ownership:

```ruby
g = GrappleC
composer = g.CreateChipComposer(1, 480)
raise "composer failed" unless composer
raise "part failed" unless g.SetChipPart(composer, 0, "harmony", g::GRAPPLE_CHIP_PRESET_HARMONY, 1)
notes = [48, 52, 55].map do |pitch|
  {track: 0, note: pitch, velocity: 80, start_tick: 0, duration_ticks: 1920}
end
notes.each { |note| raise "note failed" unless g.AddChipNote(composer, note) }
song = g.BuildChipSong(composer, 1920)
player = g.CreateChipPlayer(song, 48000, 64, true)
raise "player failed" unless player
raise "playback failed" unless g.PlayChipPlayer(player)
```

Managed playback (`PlayChipPlayer`) opens a player-owned mixer on the default
audio device. Initialize SDL audio and `MIX_Init()` first (scripts: `SDL.Init(SDL.INIT_AUDIO)`
and `MIX.Init()` (Ruby: `SDL.Init(SDL::INIT_AUDIO)` and `MIX.Init`)). Keep the player alive; stopping, explicit
destruction, or GC safely tears down its mixer before its stream. The song and
composer need not remain reachable. Native callers can instead attach the
borrowed stream to an existing mixer, provided it is detached before destroying
the player. Do not use both consumers at once. Managed transport operations must
be serialized on the application's control thread. Raw PCM output and borrowed
streams remain native-only; scripts use the managed transport API.

## Default effects and harmony pulse

| Instrument | Default treatment |
|------------|-------------------|
| Lead | 28% dotted-eighth echo (0.75 quarter-note beats), 32% feedback, 18% room reverb |
| Bass | 30% chorus strength (15% wet blend; dry fundamental retained) |
| Chord / ring harmony | Eighth-note amplitude and resonant filter/pulse-width swells, 70% pulse depth, 10% room reverb |
| Drums | Dry |

Harmony notes remain sustained; pulsing does not retrigger MIDI notes or turn
chords into arpeggios. A whole-note chord in 4/4 receives eight swells. The
pulse's phase comes from the song's beat clock, so tempo changes alter the pulse
spacing without resetting its musical position. The sound direction follows the
user's Golden Axe C64 reference, with modern effects; it is not an emulation of
that game's instrument driver.

`Grapple_GetChipPresetEffects()` returns settings to edit before passing them to
`Grapple_SetChipPresetEffects()`. `pulse_beats` is measured in quarter-note beats:
0.25 = sixteenths, 0.5 = eighths, 1 = quarters, 1.0/3.0 = eighth-note triplets.
Zero disables rhythmic pulsing; `pulse_depth` controls loudness movement and
`motion` controls filter/pulse-width movement independently. These are repeating,
uniform pulse intervals, not a pattern sequencer. Effects are shared by preset
within a player; different players have independent effect state.

Chorus, echo, and reverb run in preallocated stereo buffers without synthesis-time
allocation. Echo follows the tempo map with smoothed delay-time changes and a
four-second maximum delay. Room reverb uses damped parallel comb filters. Existing
wet tails are cleared when effect settings change; normal playback drains tails
(up to twelve seconds), loops carry effects across the boundary, and an explicit
reset clears them. Stereo buffers use roughly 8 MiB per 48 kHz player, scaling with
sample rate; this cost is independent of song length and track count.

### Presets work without effect setup

Use `GRAPPLE_CHIP_PRESET_HARMONY` for the harmony part. It names the recommended
ring-modulated timbre **and** its default wet, eighth-note pulse. It is an alias
of `GRAPPLE_CHIP_PRESET_RING`; existing callers retain the same sound. The
`CHORD` preset also pulses by default, with a pulse-wave timbre. No separate
pulse-enabling call is needed. A single MIDI mapping is enough:

```c
Grapple_SetChipTrackPreset(player, harmony_track, GRAPPLE_CHIP_PRESET_HARMONY, 1.0f);
```

MIDI program numbers describe timbres: a file's "Steel Guitar" could be harmony
or lead. Naming it "Harmony Steel Guitar" enables the harmony preset automatically;
an explicit preset remains available to override the name. Composed songs store
their names and explicit assignments in `SetChipPart`, and new players inherit them.

Pulse is reusable across instruments. To add it to bass while retaining bass
chorus, retrieve both presets and copy only the pulse/motion fields:

```c
Grapple_ChipEffects bass, harmony;
Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_BASS, &bass);
Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_HARMONY, &harmony);
bass.pulse_beats = harmony.pulse_beats;
bass.pulse_depth = harmony.pulse_depth;
bass.motion = harmony.motion;
Grapple_SetChipPresetEffects(player, GRAPPLE_CHIP_PRESET_BASS, &bass);
```

The same fields are available in Lua tables and Ruby hashes. For a different
uniform rhythm change `pulse_beats`; for example `1.0` gives quarter notes.
Set `pulse_depth` and `motion` to zero to turn off volume and timbre movement.
