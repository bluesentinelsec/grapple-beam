---
title: "Chiptune builds, limits and validation"
description: "Chiptune builds, limits and validation for grapple-beam callers."
---

# Chiptune builds, limits and validation

## Source builds and the two deliverables

Music playback is a library feature. Use the `grapple-beam` runner for Lua/Ruby
games, or embed the C/C++ SDK. [Standalone examples](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md)
play files and export WAVs; they are not installed runner modes.
`GRAPPLE_BEAM_BUILD_APP=OFF` omits the runner, and
`GRAPPLE_BEAM_BUILD_DEMOS=OFF` omits example executables.

Expat 2.8.4 is pinned, vendored and compiled as a private static target, with
provenance, digest, license and update instructions in [deps/expat.md](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/expat.md).
XML types stay private. MXL reuses the vendored PhysFS/miniz inflater without
linking VFS or HTTP. Both aggregate SDK archives contain the required objects;
installed C and C++ consumers import XML and synthesize PCM using only their SDK
target. The link audit rejects external XML/codec libraries. SDL retains the
project's existing pinned FetchContent source acquisition.

A minimal C mixer build disables unrelated modules and the app/tool dependencies:

```sh
cmake -S . -B build/music -G Ninja -C cmake/ChiptuneMinimal.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/music --parallel
```

For offline builds, pre-populate SDL's source and add
`-DFETCHCONTENT_FULLY_DISCONNECTED=ON -DFETCHCONTENT_SOURCE_DIR_SDL3=/path/to/sdl-src`.
Use normal `-DCMAKE_TOOLCHAIN_FILE=...` options for your target. Checked-in bindings
and parser sources require no target executable/code generator during a minimal
cross-build. To benchmark, turn `GRAPPLE_BEAM_BUILD_BENCHMARKS=ON` and pre-populate
`FETCHCONTENT_SOURCE_DIR_BENCHMARK` too.

Expat entropy hooks (`EXPAT_WITH_*`, `EXPAT_DEV_URANDOM`) are configurable for
platform toolchains; provide a supported entropy backend. Import never fetches
DTDs or external entities. Proprietary console SDKs have not been tested.
See [platform support](platforms-matrix.md) for build and runtime coverage.

## Resource and real-time contract

- Input is buffered at load time: up to 64 MiB, including non-seekable SDL streams.
  Caller memory is borrowed only during the synchronous load. `_IO` ownership is
  controlled by `closeio`, including failure. VFS callers supply their SDL stream.
- XML: depth 128, 300,000 nodes, 64 attributes per element, 1 MiB text per node,
  64 MiB retained tree allocation. MXL: 4,096 entries, 64 MiB per entry and 256 MiB
  declared expansion total. Only manifest and selected score are inflated.
- Scores: 256 parts, 256 instruments per part, 32 staves, one million events,
  100 million ticks per quarter, 65,536 expanded measure visits, repeat depth 32,
  32 repeat passes, 24 hours. Arithmetic overflow fails explicitly. Section and
  simultaneous tie/lane bounds are 4,096; diagnostics are bounded to 1,024.
- Import memory includes the input/archive, selected XML, bounded temporary tree,
  score model and final immutable performance. These can coexist. Freeing the
  temporary tree/model after loading leaves the song's events, expression and
  metadata. Allocate/load on a normal game/loading thread with a conventional
  stack (at least 256 KiB recommended); parsing is unsuitable for tiny audio stacks.
- Raw rendering is interleaved native 32-bit float stereo at 8–192 kHz. WAV helpers
  write 16-bit little-endian stereo RIFF with bounded scratch buffers and tails.
  The core uses C, checked 64-bit integer timing and floating-point DSP; no C++
  runtime, network service, or synthesizer process is required by the synthesis
  core. MusicXML parsing uses the private, statically built Expat dependency at
  load time. The full SDK's unrelated components retain their own requirements.
- A player allocates five shared synthesis effect buses (about 1.6 MiB each at 48 kHz), voice
  and controller storage, and an optional loop checkpoint. Up to 32 private part
  buses are available, each with the same effect storage. Memory scales with rate,
  part count and configured polyphony. Silence does not free/reallocate buses.
- The callback performs no parsing, score expansion, allocation or historical
  scan on loop wraps. Seeking/checkpoint preparation and private-bus allocation
  belong on the serialized control thread. Managed playback uses SDL stream
  locking; raw rendering requires caller serialization with control changes.
  The application/device owns output threading and buffering.
- Output is soft-limited. Finite playback drains release/effect tails up to twelve
  real seconds. Pause preserves state; stop/reset clears tails. Seek rebuilds
  musical state, clears historical wet/filter/noise state and fades in over 2 ms.

These synth effect buses are separate from the engine music/SFX/speech/ambient
volume routing described in [Mixer](mixer.md).

## Measure your target

For benchmarks, enable `GRAPPLE_BEAM_BUILD_BENCHMARKS=ON` in the minimal build
and run `build/music/bin/grapple_chiptune_bench`. Supply the pinned Benchmark
source as well as SDL when building disconnected. Import benchmarks measure score
compilation; render benchmarks exercise sustained voices with default wet effects.
Compare render CPU time against your device's audio-block duration and leave room
for the rest of the game. Voice count, sample rate, and private effects affect cost.

The [standalone demo](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md) can render complete WAVs for
audition or profiling without opening an audio device. Keep generated WAVs outside
your source assets unless the game will ship pre-rendered audio.

## Regression fixtures and checks

The [mixer tests](https://github.com/bluesentinelsec/grapple-beam/tree/main/tests/mixer) cover Guitar Pro MIDI/XML, MuseScore notation/TAB
exports, MXL, and synthetic expectations for timing, expression, navigation,
role assignment, polyphony, wet effects, transport, and malformed input.
The mutation corpus is bounded regression fuzzing, not exhaustive coverage.

```sh
ctest --test-dir build/debug -N -R 'Chip|MusicXML|MusicXml|chiptune'
ctest --test-dir build/debug --output-on-failure -R 'Chip|MusicXML|MusicXml|chiptune'
```

C/C++ SDK consumers and Lua/Ruby examples also exercise notation playback.
Packaged Android/iOS and browser tests import triplet MusicXML and render finite,
nonzero PCM; these checks do not audition real audio devices. Use the
[platform matrix](platforms-matrix.md#validation-coverage) and current
[CI runs](https://github.com/bluesentinelsec/grapple-beam/actions) for platform evidence.
