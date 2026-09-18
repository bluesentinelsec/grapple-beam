---
title: "C desktop projects"
description: "Create, develop, test and package a C game with grapple-beam."
---

# C desktop projects

`grapple-beam new` creates an editable C17 game for macOS, Windows, and Linux.
The generated project uses CMake and source-built engine dependencies. This
workflow currently supports `--model=c`; other languages and web/mobile project
templates are separate work. The existing Lua/Ruby runner remains available.

```sh
grapple-beam new starfall --model=c
cd starfall
make
make test
```

On Windows, use `build.bat` from a Visual Studio Developer Command Prompt. Install
Git 2.28+, CMake 3.25+, and a C17/C++20 compiler. The authored game is C; building
the engine and its dependencies can also require a C++ compiler. No prebuilt SDK
is required. Source downloads and builds occur on first configuration.

## Version selection

By default, creation resolves GitHub's latest stable published engine release and
writes its exact commit to `cmake/GrappleDependency.cmake`. Later builds keep that
pin. Select an exact version or full commit instead:

```sh
grapple-beam new starfall --model=c --engine-version=v0.9.0
grapple-beam new starfall --model=c --engine-commit=8aa69342b95ff06043fa664ba9e2c84dd9960d68
```

These options are mutually exclusive. Public resolution uses the built-in HTTP
client, without requiring gh authentication. Network/ref/compatibility errors
stop generation; there is no silent fallback to a branch or another release.
CI may supply `GITHUB_TOKEN` for higher GitHub metadata rate limits. It is used
only for API requests, never written to the generated project or forwarded to
download hosts. GitHub repository creation still delegates authentication to gh.
The supported baseline is v0.9.0's C engine API. The starter adapts its documented
configuration subset to that API, without depending on newer typed settings APIs.

## Workspace and source files

The project includes a playable starter, a root README and license, AGENTS.md,
CMake presets, the five Make operations, a Windows wrapper, a default TOML config,
unit/integration tests, media subdirectories, dependency provenance/notices,
clang-format/clangd/ctags configuration, Markdown player documentation, and GitHub
Actions workflows for desktop builds, documentation/Pages, and game releases.

For `starfall`, add implementations under `src/starfall/` and headers under
`include/starfall/`. Nested `.c` files are discovered on the next build; ordinary
source additions need no CMake edit. Include headers as `starfall/file.h`.
`src/main.c` stays the single application entrypoint. Tests link reusable game
code without the app entrypoint. Keep scratch files, dependencies and competing
platform variants outside the portable game source directory.

Media paths are relative to the engine mount root: `media/sprites/player.svg`
loads as `sprites/player.svg`. The `lang/` directory follows localization
conventions; the other media directories are suggested organization. Runtime
resources are staged beside the executable or inside the macOS app bundle, so
launching does not depend on the current working directory.

## Five Make operations

| Target | Behavior |
| --- | --- |
| `build` | Default; configure and build Debug. Plain `make` selects it. |
| `build-release` | Configure and build Release separately. |
| `test` | Build prerequisites and run all registered unit/integration tests with timeouts. |
| `fmt` | Format authored C sources and headers using clang-format. |
| `clean` | Remove generated build/package/docs output, compilation database copies and tags; preserve authored files and player data. |

`build.bat` has the same five operations. CMake/CTest remain directly usable;
generator and concurrency overrides are documented in the generated README.
There are no Make targets for running, packaging, distribution or documentation.
The generated README provides executable paths and direct commands for docs,
format verification, ctags, and Linux sanitizer builds.

Debug runs are windowed with debug logs. Release runs default to exclusive
fullscreen with info/warning/error logs. The starter supports shipped and player
TOML, explicit config overlays, and individual launch settings in that precedence
order. Its player guide lists supported keys and reset/safe-mode behavior. Player
identity is stable across checkout moves. Rendering tests use a real window and
software renderer, load an asset, inspect pixels and movement, and finish within
a bounded frame count with an external CTest timeout. They skip player settings.

## Git and GitHub

Creation initializes Git on `main` and leaves the files uncommitted for review.
Use `--no-git` to skip initialization, including when creating inside another
repository. Existing project files are not overwritten. Choose `--license=MIT`
instead of the default zlib license if desired; update the copyright-holder
placeholder before shipping and retain dependency/asset notices separately.

```sh
grapple-beam new starfall --model=c --github=OWNER/starfall
grapple-beam new starfall --model=c --github=OWNER/starfall --visibility=public
```

The optional GitHub flow requires authenticated gh and a configured Git author.
It commits the scaffold, creates a private repository by default, sets origin,
and pushes main. Authentication is delegated to gh; run `gh auth login` if needed.
Public visibility is explicit. Failures preserve a useful local project and
report recovery steps; the CLI does not delete upstream repositories or force-push.

The Markdown website has local build/link validation and a Pages workflow.
Select GitHub Actions under repository Settings > Pages where your account
supports it. The CLI does not change repository visibility to enable Pages.
This website hosts documentation, not a browser build of the game.

## Packaging

```sh
grapple-beam package                 # Current generated project.
grapple-beam package ./starfall
```

The CLI configures/builds Release, runs the tests, installs only the game's CMake
component, audits runtime imports, and checks the staged game's real assets and
rendering outside the source tree. It produces an OS/architecture/version ZIP
and SHA-256 sidecar in `dist/`, plus a separate debug-symbol archive. macOS output is an `.app`; Windows and Linux ship
the executable and its resources/notices. Players do not need the developer CLI,
the source checkout or a separately installed engine.

The package command requires a CLI containing C desktop project support; the
v0.9.0 **engine library** is supported, but its CLI predates these commands.
Generated CI source-builds its packaging CLI from the recorded tooling commit.
Keep this CLI pin separate from the engine dependency pin.

Packages support the starter's static dependency selection. Adding shared
libraries requires explicit installation/audit rules; unexpected runtime imports
fail packaging. Build each desktop payload on its own OS. Linux inherits the
build host's system-library baseline, which should be documented for releases.
Unsigned archives still need applicable signing/notarization/trust and store
steps before broad distribution. Signing, installers and cross-OS compilation
are outside this workflow.

The game has its own root `VERSION`. Pushing a matching `vX.Y.Z` game tag runs the
generated release workflow and publishes the validated archives/checksums. The
`new` command itself does not publish a game release.
