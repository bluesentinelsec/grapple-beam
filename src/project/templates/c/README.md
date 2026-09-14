# @GAME_ID@

An editable C17 desktop game powered by grapple-beam. Move with the arrow keys;
press Escape to quit. See [player documentation](docs/index.md).

## Tools

Install Git 2.28+, CMake 3.25+, a C17/C++20 compiler and a build tool. Game code
is C; some engine dependencies require a C++ compiler. Source dependencies are
fetched automatically on first configuration. No installed engine SDK is needed.

- macOS: Xcode command-line tools, CMake, and Make or Ninja.
- Windows: Visual Studio 2022 C++ Desktop workload and CMake; use a Developer
  Command Prompt with `build.bat`. GNU Make is optional.
- Linux: GCC/Clang, CMake, Make/Ninja, and SDL development prerequisites. See
  [building](docs/developing/building.md) and the checked-in CI package list.
- Formatting: clang-format. Website: Python 3.10+ and Pandoc 3+. API reference:
  Doxygen. Navigation: Universal Ctags. These are only needed for their tasks.

## Five Make targets

| Command | Result |
| --- | --- |
| `make` or `make build` | Debug build |
| `make build-release` | Separate Release build |
| `make test` | Build and run all unit/integration tests, with timeouts |
| `make fmt` | Format authored C sources/headers, including tests |
| `make clean` | Remove build/, dist/, compilation database and tags |

Windows `build.bat` accepts those same five names; no argument selects `build`.
Set `JOBS=8` to change concurrency. Set `CMAKE_FLAGS=-G Ninja` when creating a
fresh build directory to choose Ninja. Never change a configured directory's
CMake generator in place. `clean` preserves code, original media, Git and player
preferences; it removes local fetched build trees, which will be downloaded again.

Launch `build/debug/bin/@GAME_ID@` on Linux, `build/debug/bin/@GAME_ID@.exe` on
Windows, or `open build/debug/bin/@GAME_ID@.app` on macOS. Use the executable
inside the app's `Contents/MacOS/` directory to see terminal logs and pass flags.
Release uses `build/release` and defaults to exclusive fullscreen. Debug is
windowed with debug logs. Explicit config/player/CLI settings override defaults.

## Add code and media

Put `.c` files anywhere under `src/@GAME_ID@/`; nested directories are discovered
on the next build without editing CMake. Put public headers in
`include/@GAME_ID@/`, included as `@GAME_ID@/file.h`. `src/main.c` is the sole
application entrypoint. Reusable game code is linked into the app and its tests.
Keep scratch files and platform-specific alternatives outside the portable source
root. CMake's CONFIGURE_DEPENDS behavior is tested on the supported generators;
rerun configuration if using another generator that cannot detect source changes.

Media paths are relative to the mounted root: `media/sprites/player.svg` loads as
`sprites/player.svg`. `lang/<locale>.toml` follows engine localization conventions;
other folders are organization suggestions. Add asset licenses alongside assets.

## Tools outside Make

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
cmake -DCHECK=ON -P cmake/Format.cmake
ctags --options=.ctags src include tests
doxygen Doxyfile
python3 scripts/build-docs.py
```

The website and API reference go under `build/site` and `build/api`.
Clangd reads `build/debug/compile_commands.json`; Ninja and Makefile generators
produce it. The wrapper also copies it to the root. Debugging instructions and
Linux sanitizer presets are in [testing](docs/developing/testing.md).

## Package and ship

Run `grapple-beam package` from this directory. It builds Release, runs tests,
stages resources/notices, checks runtime dependencies, verifies the game from a
temporary directory, and writes a versioned game ZIP, separate debug-symbol ZIP, and SHA-256 sidecars under `dist/`.
An installed CLI with schema-1 C project packaging support is required; v0.9.0's
CLI predates this command even though its **engine library** is supported.
See [packaging](docs/developing/packaging.md) before distributing unsigned output.

The engine is pinned in `cmake/GrappleDependency.cmake`. Deliberately change its
commit to upgrade and run the full suite. `deps/` records initial provenance and
licenses; build/package collects dependency notices for the chosen source tree.

CI checks macOS, Windows and Linux. Tag `v` plus the game's root VERSION to build
release archives and publish a GitHub Release. Choose Settings > Pages > GitHub
Actions to enable the Markdown documentation website where your account supports
it. A private repository is never made public automatically. Edit player docs,
the LICENSE copyright holder, application identity and artwork before release.
