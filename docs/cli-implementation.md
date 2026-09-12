# CLI settings implementation

The approved scope is in [cli-args.txt](cli-args.txt). Implementation is on
`feat/cli-settings`. The contract includes independently adjustable music,
SFX, speech, and ambient buses beneath master gain, plus concrete renderer
selection and backend/version discovery. This file separates working behavior
from the remaining work; the full contract is not yet implemented.

## Slice 1: parsing and concrete renderer selection

- CLI11 validates the implemented option set before running user code. Both
  argument spellings work, repeated settings preserve command-line order,
  numbers must be finite and within range, and booleans require `on` or `off`.
- `--window-mode windowed|fullscreen-exclusive|fullscreen-borderless` replaces
  `--fullscreen` and `--windowed`. Window size no longer selects a mode.
  Startup mode application also runs when settings originate in CLI
  overrides rather than a supplied graphics structure.
- Existing graphics controls are visible in help. `--max-fps` accepts
  `display`, `unlimited`, or an integer in 10..1000. Legacy numeric sentinel
  values are rejected by the executable.
- `repl --language lua|ruby` and `eval --language lua|ruby --code SOURCE`
  replace implicit interactive launches and top-level `-e`. Lua/Ruby script
  arguments, including leading dashes and paths with spaces, remain intact.
- `--backend` selects `auto`, `directx11`, `directx12`, `vulkan`, `metal`,
  `opengl`, `opengles2`, or `software`. Explicit selections use a concrete SDL
  renderer and never fall back. Vulkan uses SDL's Vulkan renderer, not the
  multi-API GPU renderer. CLI selection wins over the config builder.
- `--list-backends` distinguishes compiled candidates, successful hidden-window
  probes, and failures. It reports the actual SDL renderer, queried GL API
  version/device/vendor, and compatibility with the existing OpenGL effects.
  Other unavailable version fields explicitly say `not reported`; API-family
  labels are not passed off as queried versions. Platform-specific metadata
  queries beyond GL remain to be added.
- The C SDK exposes backend enumeration/probing, concrete renderer creation,
  and an owned config-builder setter. Generated C++/Lua/Ruby bindings include
  this surface. The library does not acquire a CLI11 dependency; building the
  desktop executable now requires its existing CLI11 option enabled.
- `--safe-mode` selects the existing conservative graphics preset, a resizable
  window, and the software renderer before individual CLI overrides. Full
  configuration-script bypass is part of the upcoming resolver slice.

The executable is strict. The older embeddable `Grapple_GraphicsLoadArgs`
helper remains permissive for existing C applications while the common typed
resolver is being introduced; older file spellings are still readable. New
TOML output uses canonical window-mode names.

`Grapple_EngineConfig` gains a trailing renderer-selection field. SDK consumers
must rebuild against the updated headers/library together; an old binary's
smaller configuration structure is not ABI-compatible with this branch.

## Remaining slices

- Shared C settings registry and resolver: field presence, defaults/player/
  launch values, source tracking, Lua/Ruby settings execution before startup,
  shipped TOML/script -> player TOML/script -> explicit files/scripts -> CLI.
- Project-root discovery/manifest and stable relative path handling. This
  first slice retains direct script launching; directory discovery is pending.
- `--config`, `--config-script`, `--print-settings`, `--default-settings`, and
  backed-up `--reset-settings`, including non-persistence of launch overrides.
- Display enumeration, primary-display selection, explicit exclusive resolution
  and refresh rate, actual-state reporting and handling SDL application errors.
- Expose resizable, high-DPI, headless, auto-mount, advanced typed overrides;
  unify media override precedence. Current --media retains legacy engine policy.
- Quality presets, effect master gate independent of accessibility processing,
  and consistent application of engine-consumed budgets and UI/camera settings.
- Master/music/SFX/speech/ambient mixer buses and their CLI controls, with
  standard playback helpers routed through the correct category.
- Platform-specific backend version/device queries beyond OpenGL, remaining
  configuration/application tests, platform validation, and migration docs.

Pending options are not advertised as functional: the executable rejects them
rather than accepting a request that would have no effect. Existing --config
was not automatically resolved by the ordinary script helper; it is intentionally
not exposed again until the resolver is connected.

## Validation

The first slice passed the full local `make test` suite (775 tests), including
backend API/startup tests and an end-to-end CLI contract test for Lua and Ruby.
The local macOS probe verified Metal, OpenGL, and Software availability, with
queried GL version/device/vendor and explicit Vulkan/GLES probe failure reasons.
Cross-platform runtime verification and platform-specific version queries are
still pending; local results are not a claim of Windows/Linux hardware coverage.
Generated-binding freshness, API documentation, whitespace checks, and a
strict warnings-as-errors C compile of the new backend implementation pass.
