---
title: Platforms
description: "Build and package grapple-beam for desktop, Android, iOS, and WebAssembly."
---

# Platforms

The repository supports desktop SDK/runner builds and embedded mobile/browser
builds. Start with the [support matrix](platforms-matrix.md) for available modules,
artifacts, and the scope of runtime checks.

## Desktop

Use `make` / `build.bat`, or the installed SDK CMake package described in
[Getting started](getting-started.md). The runner hosts Lua/Ruby projects; native
applications link the library. Desktop shared variants are optional. System
libraries, graphics drivers, and platform SDK requirements still apply to static
builds.

Use `grapple-beam run --list-backends`, `--list-displays`, and `--list-display-modes`
to inspect the local machine. Available renderer and fullscreen modes depend on
its drivers and connected displays. [Runner settings](cli-implementation.md)
covers requested versus achieved settings and recovery.

## Android

The [Android project](https://github.com/bluesentinelsec/grapple-beam/tree/main/android) publishes a Prefab AAR. Native games consume
it from an Android application and provide SDLActivity lifecycle integration;
the desktop runner is not an Android entrypoint. See the
[Android workflow](https://github.com/bluesentinelsec/grapple-beam/blob/main/.github/workflows/android.yml) and
[packaged tests](https://github.com/bluesentinelsec/grapple-beam/tree/main/tests/android) for build and test configuration.

## iOS

Build the static XCFramework with [build_ios_xcframework.sh](https://github.com/bluesentinelsec/grapple-beam/blob/main/scripts/build_ios_xcframework.sh)
and embed it in an iOS application. It contains device and simulator slices.
[iOS workflow](https://github.com/bluesentinelsec/grapple-beam/blob/main/.github/workflows/ios.yml) configuration supplies deployment
versions, architectures, and SDK settings. The automatic PR job compiles the
simulator SDK; full packaging and simulator execution use the dispatched workflow.
Lua's shell execution is unavailable on iOS.

## Browser WebAssembly

Use the Emscripten toolchain and [web workflow](https://github.com/bluesentinelsec/grapple-beam/blob/main/.github/workflows/web.yml) as the
build recipe. The browser owns frame pacing: use the engine's browser loop or
Emscripten callbacks, not a blocking desktop loop. Supply assets through the
browser/virtual filesystem, and initiate audio from an appropriate user gesture.

- Raw TCP/UDP (`Grapple::Net`) and the mog C client/server (`Grapple::Http`) are
  disabled in this build. Use browser networking APIs for a web integration.
- Give the canvas a real CSS width and height; SDL uses its CSS box to determine
  initial window dimensions. See [the web demo](https://github.com/bluesentinelsec/grapple-beam/tree/main/src/web) for the page setup.
- File dialogs use browser input/download integration. See [Extras](extras.md#on-the-web)
  and [GUI file buttons](gui.md#file-buttons-that-work-in-every-browser).
- C64-inspired MIDI/MusicXML playback needs no patch files. General-MIDI decoding
  through SDL_mixer's TiMidity path requires its patch configuration; see [Mixer](mixer.md).

## Cross-compiling and dependencies

Use `CMAKE_TOOLCHAIN_FILE` and the platform switches from the corresponding build
scripts. Dependencies are built from source; provision enabled FetchContent
sources before a disconnected build. The [minimal music configuration](chiptune-validation.md)
shows how to isolate the C mixer/synth. Proprietary consoles require their own
toolchain and platform integration and are not currently covered by CI.
