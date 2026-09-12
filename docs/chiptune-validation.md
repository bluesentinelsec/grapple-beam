# Chiptune builds, limits and validation

## Source builds and the two deliverables

The project ships the **`grapple-beam` engine/script runner** and the **embeddable
SDK** (C/C++ static archives and optional dynamic variants). The runner's only
desktop entrypoint is thin `src/main.cpp`, hosting the former `grapple` script
runner through `src/runner`. Audio-file playback belongs to library helpers and
optional demos. There is no separate installed `grapple` runner or audio-track
mode in `grapple-beam`. `GRAPPLE_BEAM_BUILD_APP` controls the runner;
`GRAPPLE_BEAM_BUILD_DEMOS=OFF` omits the one-off examples.

Expat 2.8.4 is pinned, vendored and compiled as a private static target, with
provenance, digest, license and update instructions in [deps/expat.md](../deps/expat.md).
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

The minimal arm64 iOS Simulator build was configured with dependency downloads
disabled and produced both SDK archives from source using Apple's toolchain.
Expat entropy hooks (`EXPAT_WITH_*`, `EXPAT_DEV_URANDOM`) remain configurable for
platform toolchains: provide an upstream-supported entropy backend rather than
silently removing hash hardening. Default desktop import can call that platform
entropy backend; it never retrieves a DTD or external entity. Proprietary console
SDKs have not been tested. Source portability is not console certification.

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
  runtime, network service, synthesizer process or parser dependency is added to
  the C core. The full SDK's unrelated components retain their own requirements.
- A player allocates five shared effect buses (about 1.6 MiB each at 48 kHz), voice
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

## Measurements

Measured on Apple M1 Max, macOS 26.3.1, Apple Clang, Release minimal source build,
48 kHz stereo, 512-frame blocks, default wet presets. Three benchmark repetitions
with a 0.2-second minimum per repetition; numbers below are mean CPU time. CPU
frequency/affinity metadata was unavailable to Google Benchmark, so no frequency
estimate is used. These are host measurements, not frame-budget guarantees.

| Work | Mean CPU time | Practical interpretation |
| --- | --- | --- |
| Import original 93,285-byte Guitar Pro XML from memory | 1.74 ms | Includes score interpretation and disposal; excludes file I/O |
| Render 8 sustained voices, 512 stereo frames | 0.334 ms | About 3.1% of the 10.67 ms audio-block interval |
| Render 32 sustained voices | 1.058 ms | About 9.9% of that interval |
| Render 64 sustained voices | 2.031 ms | About 19.0% of that interval |

Run `build/chiptune-release/bin/grapple_chiptune_bench` after configuring the
minimal benchmark build above. The supplied song's complete 48 kHz WAV (music plus
effect tails) took 0.92 seconds wall time using the **Debug** standalone demo;
`/usr/bin/time -l` reported 12.5 MiB peak RSS for the process. Resident pages differ
from reserved/allocated effect capacity. The output can be reproduced from the
committed fixture rather than storing a large generated WAV in the repository.

## Acceptance evidence

The committed test corpus covers Guitar Pro MIDI/XML, MuseScore standard/TAB
round-trips, stored/deflated MXL and independent synthetic musical expectations.
Assertions cover event pitch/onset/duration/part identity, five or more instruments,
more than sixteen independent parts, expression/audio behavior, role resolution,
wet pulse defaults, shared/private effect isolation, transport and fractional loops.
A deterministic 288-case MIDI/XML/MXL mutation corpus complements targeted hostile
XML/archive/reference/navigation tests; successful mutations also render finite PCM.
This is bounded regression fuzzing, not a claim of exhaustive fuzz coverage.

All nine standalone/script demonstrations passed locally, including MIDI/XML
playback, WAV export, compressed-MXL C++ playback and Lua/Ruby file/declarative
music. C and C++ consumers of the installed SDK both import/render MusicXML.
Generated bindings pass freshness checks, and `make doc` includes the chiptune API.
Native sanitizer tests pass with ASan/UBSan (macOS does not provide LeakSanitizer).
Linux sanitizer CI also passes, including complete default-effect demo renders.

The PR's existing desktop, Android, iOS and Web workflows are the platform gates.
Browser and packaged Android/iOS tests now explicitly import triplet MusicXML and
render nonzero finite PCM without opening an audio device. Their latest status is
reported on [PR #69](https://github.com/bluesentinelsec/grapple-beam/pull/69), rather
than treating an unrun target as validated. The user approved the earlier pulse
demo; subsequent synthesis changes are backed by numerical/audio regression tests.
New subjective audition on every target is not claimed.

Final local validation: **758/758 tests passed** through `make test`, including
SDK installation and the canonical runner tests. Both shared SDK variants also
load/render MIDI, XML and MXL through Python ctypes. The runner and C demo pass
static dependency audits (only OS libraries); bindings are fresh and Doxygen
reports no warnings for the added header.
