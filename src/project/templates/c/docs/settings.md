# Settings and recovery

The supported starter settings are `[display]` keys `window_mode`, `vsync`,
`max_fps`, `window_width` (320–16384), and `window_height` (200–16384).
Window modes are `windowed`, `fullscreen-borderless`, and `fullscreen-exclusive`.
Frame limits accept -1 (uncapped), 0 (display-paced), or a positive integer up to
16384. Vsync is a TOML boolean. Unknown keys/types are errors.

The layers are compiled game/profile defaults, shipped config.toml, player
config.toml, explicit `--config FILE` overlays in order, then individual flags.
Use `--window-mode windowed`, `--vsync off`, and `--max-fps 60` for launch overrides.
CLI values are temporary. This C starter does not load Ruby/Lua configuration.
It adapts these common settings to the v0.9.0 graphics C API; add further settings
explicitly in the game's startup component when needed.

`--print-settings` shows the effective values and player config path without
opening a window. Preferences use SDL's per-user directory for organization
`games`, application `@GAME_ID@`, so moving the checkout does not lose preferences.
The identifiers are defined in the startup component; change them before shipping
and keep them stable afterward.

`--default-settings` skips player config for one launch. `--reset-settings` also
backs up an existing player config with a timestamp after validation; saves are
untouched. Add `--print-settings` to preview reset without moving files.
`--safe-mode` skips all config files and selects windowed software rendering.
Recovery flags are mutually exclusive and cannot be mixed with explicit configs.
Individual flags still win. Use the executable inside Contents/MacOS when passing
flags to a macOS app from a terminal.
