---
title: Platform support matrix
description: "Available engine modules, SDK artifacts, and the scope of platform validation."
---

# Platform support matrix

The table lists components enabled in full platform builds. A build/link check
does not establish that every feature has identical behavior at runtime.

| Component | Linux | macOS | Windows | Android | iOS | Web |
|---|---|---|---|---|---|---|
| Image | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| TTF | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Mixer | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Net | ✅ | ✅ | ✅ | ✅ | ✅ | — |
| Gfx | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Extras | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Formats | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| VFS | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Physics | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| GUI | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Http | ✅ | ✅ | ✅ | ✅ | ✅ | — |
| Tiled | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Regex | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Light | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Engine | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Lua | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Ruby | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Bindings | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| C++ wrapper | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |


`Net` and `Http` targets are disabled by CMake on Emscripten. Browser applications
must use browser networking integrations. Lua on iOS reports no shell for
`os.execute`; other OS facilities are also subject to platform permissions.

## SDK packaging

| Platform | Output | Desktop runner |
| --- | --- | --- |
| Linux, macOS, Windows | Static C/C++ SDK archives, optional shared C/C++ SDKs, headers, CMake package | `grapple-beam` |
| Android | Prefab AAR with a shared native library per ABI | No; embed in an Android app |
| iOS | Static XCFramework, device and simulator slices | No; embed in an iOS app |
| Web | WebAssembly static SDK and browser demo/test artifacts | No desktop CLI |

Desktop consumers choose `SDK`, `SDKCxx`, `SDKShared`, or `SDKCxxShared` from the
installed CMake package. Shared libraries install under `lib/` on Unix; Windows
DLLs go in `bin/` and import libraries in `lib/`. C++ SDKs include the C surface.
Public export filtering is part of desktop shared-library packaging; do not rely
on private dependency symbols as an API. See [Getting started](getting-started.md).

OS libraries/frameworks remain dependencies of the application. Static engine
linkage does not remove platform lifecycle, driver, permission, or asset packaging
requirements. Desktop CLI display modes and backend names are not universally
available on mobile or in a browser.

## Validation coverage

- **Desktop:** unit tests, SDK consumers, binding freshness, and shared-library
  loading checks. Linux additionally runs ASan/UBSan. Renderer support must still
  be tested with the drivers and displays you ship against.
- **Android:** packaged JNI tests exercise native modules and bindings, including
  MusicXML triplet import and finite PCM rendering. The plain Activity test
  harness does not run a full SDL engine window; games need SDLActivity lifecycle
  integration.
- **iOS:** pull requests compile and install the simulator SDK. The dispatched
  XCFramework workflow packages device/simulator slices and runs consumer tests
  in the simulator, including a headless engine and MusicXML PCM. This full
  runtime workflow is separate from the automatic PR compilation check.
- **Web:** browser tests cover environment, image decoding, and MusicXML PCM.
  Browser engine-loop coverage is narrower than the desktop suite.

Current results are in [Actions](https://github.com/bluesentinelsec/grapple-beam/actions).
The [workflow sources](https://github.com/bluesentinelsec/grapple-beam/tree/main/.github/workflows) define which checks run for pull
requests, dispatches, and releases. Proprietary console toolchains have not been
validated; source builds permit integration work without a binary-only dependency.
