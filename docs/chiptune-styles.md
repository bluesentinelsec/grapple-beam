# Chiptune styles

A **style** is a JSON instrument palette. MusicXML/MIDI import is unchanged: part
names still map onto roles (lead, harmony, bass, drums, noise). The style
re-skins those roles.

C64 remains the default. Named palettes:

| Keys in `chiptune-player` | Id | Notes |
| --- | --- | --- |
| 1 | `c64` | SID-inspired default |
| 2 | `nes-smb` | NES-inspired pulse/triangle/noise |
| 3 | `gb-pokemon` | Game Boy-inspired pulses |
| 4 | `genesis-sonic` | 2-op FM approximation |
| 5 | `snes-zelda` | Warm pad approximation (not SPC samples) |
| 6 | `dos-wolf3d` | AdLib-inspired 2-op FM approximation |

Genesis/SNES/DOS ids are original synthesis, not rips or official soundtracks.

## Author a style

Put a schema-1 JSON file in `modules/mixer/styles/` (build-time: add the string
to `chip_styles_builtin.c`) or load it at runtime:

```c
Grapple_ChipStyle *style = Grapple_LoadChipStyle("my-style.json");
Grapple_RegisterChipStyle(style); /* optional catalog */
Grapple_SetChipPlayerStyle(player, Grapple_GetChipStyle("my-style"));
```

Lua / Ruby:

```lua
local style = assert(GrappleC.LoadChipStyle("my-style.json"))
assert(GrappleC.SetChipPlayerStyle(player, style))
```

Minimal file (`lead`, `harmony`, `bass`, `drums` required). Effects default dry:

```json
{
  "schema": 1,
  "id": "minimal-pulse",
  "name": "Minimal pulse band",
  "roles": {
    "lead": { "oscillator": "pulse", "duty": 0.25, "pwm": 0, "vibrato": 0, "filter": "off" },
    "harmony": { "oscillator": "pulse", "duty": 0.5, "filter": "off" },
    "bass": { "oscillator": "triangle", "triangle": "nes", "filter": "off" },
    "drums": { "oscillator": "noise", "filter": "off" }
  }
}
```

Unknown keys fail the load. `voice: sample` is reserved.

## Player / mixer

```sh
./build/debug/bin/chiptune-player /path/to/score.xml
```

The window is a 1920×1080 design surface (scales to 4K). Each track has a
volume slider with a numeric readout (0.00–2.00). Those mix values are stored
**per style**, so switching 1–6 restores that palette's volumes. Reverb, chorus,
delay, phaser and flanger are per-track. **Edit instrument** loads ADSR / duty /
PWM knobs for that track's role; those edits also stick to the current style.

Space play/pause, arrows seek, R/Home restart, N next file, L loop, 1–6 styles.
**Dump config** writes the live mix/instrument JSON to the user temp directory
(`$TMPDIR` / `%TEMP%` / `/tmp`) and shows the path in the UI, a dialog, and stdout.
