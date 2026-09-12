# Chiptune MusicXML playback

Implementation is tracked in issue #68 and draft PR #69. This document records
the actual supported behavior as work progresses; it is not a claim that every
MusicXML element already has a performance interpretation.

`Grapple_LoadChipSong()` and its SDL_IOStream variant detect MIDI, uncompressed
MusicXML, or compressed MXL by content. The resulting immutable song uses the same player and
instrument-name contract as MIDI and declarative composition. All XML parsing
and score compilation happen at load time, outside the audio callback.

## Implemented foundation

- Partwise and timewise documents; the supplied Guitar Pro MusicXML 2.0 export.
- Pitched notes, rests, simultaneous chord notes, backup/forward, multiple
  voices/staves, pickups and measure alignment across parts.
- Exact divisions, including changes and decimal durations; tuplets use the
  encoded duration without applying their displayed ratio a second time.
- Sounding pitch alterations, staff-specific transposition, percussion IDs,
  program/volume/pan metadata, note dynamics, explicit sound tempo and dotted
  metronome beat units.
- Ties preserve note identity and sustain across measures without retriggering.
- Mirrored TAB staves are omitted only when their complete normalized note set
  matches a standard-notation staff in the same part. Distinct staves and
  independent unison parts remain audible.
- Score parts and declarative parts have independent controller state, including
  when more than sixteen parts use the same nominal MIDI channel. Imported MIDI
  retains its original shared-channel semantics.
- Exact frame conversion avoids overflowing large fractional time products.

The importer chooses a common integer resolution that exactly represents the
input durations; it never rounds them to a default MIDI grid. The current
resource limit is 100 million ticks per quarter note, 65,536 measures, 32 staves
per part, 256 parts, one million retained events and 24 hours of performance.
Scores that exceed exact arithmetic/resource limits fail explicitly.

## Pending implementation

Hairpins/pedals/swing, remaining guitar techniques,
advanced transport and the final support matrix remain tracked on the issue.
Remaining expressive constructs are not yet guaranteed to affect playback;
this importer is under development.

Expat is vendored and built statically from pinned source; see `deps/expat.md`.
The XML tree is limited to 300,000 nodes, depth 128, 64 attributes per element,
1 MiB of text per element and 64 MiB of retained tree allocations. Entity
declarations/external entities are rejected; DOCTYPE declarations do not fetch
DTDs. No filesystem extraction or network activity is needed for XML import.

## MXL containers

The importer reads `META-INF/container.xml` and extracts its referenced score
from memory. Stored and raw-deflated ZIP entries are supported, with CRC and
central/local-header checks. Paths must be relative and unique. Encrypted,
ZIP64, split archives and multiple score roots are rejected explicitly. Limits
are 4,096 entries, 64 MiB per input/entry and 256 MiB declared total expansion.
No files are extracted or mounted. The inflater is the existing vendored
PhysFS/miniz code, compiled without linking the VFS or HTTP components.

## Import policy and diagnostics

`Grapple_GetChipImportDefaults` initializes a policy. Pass it to
`Grapple_LoadChipSongEx` or `_IOEx` along with an optional error output.
Strict mode is the default: recognized unsupported playback instructions
fail with a diagnostic category, part/measure and XML line. Permissive mode
retains warnings for unsupported instructions that can safely be skipped;
malformed timing and unsupported structural transformations still fail.

Inspect retained messages with `Grapple_GetChipDiagnosticCount`,
`Grapple_ReadChipDiagnostic` and `Grapple_GetChipDiagnosticMessage`. These APIs
also work through C++, Lua and Ruby. Informational diagnostics identify each
omitted TAB mirror. Diagnostic records/messages live with the immutable song.
A maximum of 1,024 diagnostics bounds retained memory.

The staff policy is 0 for automatic mirror detection, -1 to retain every
staff, or 1..32 to select a staff in multi-staff parts. Single-staff parts
such as percussion remain present. Timing interpretation fields configure the supported techniques described below.

## Score navigation

Forward/backward and implicit repeats, nested repeats, numbered/ranged alternate
endings, repeat counts, explicit D.C./D.S./segno/coda/to-coda/fine instructions,
`time-only` jump passes and `after-jump` repeats compile into a finite playback
order. Common D.C./D.S./Fine words and unnamed segno/coda signs are also recognized.
Tempo/program/controllers are restored from the source score at navigation
boundaries. The song retains source-measure positions for transport queries.
Navigation is bounded to 65,536 expanded measure visits, repeat depth 32, and
32 numbered passes; ambiguous/missing targets and unmatched repeat starts fail.

MusicXML `measure-repeat` and octave-shift display marks do not duplicate notes
or apply an extra octave shift: the performed notes are already encoded in the
file. Explicit `transpose` instructions still apply to written pitches. Cue
notes do not generate audio. Explicit dynamics use the MusicXML forte reference
of velocity 90, and sound offsets override containing direction offsets.

## Per-note expression

Fractional pitch alterations use equal-tempered semitone offsets at synthesis time,
without detuning other notes. `Grapple_AddChipNoteEx` exposes the same per-note tuning,
three-point bend curve, beat-based vibrato, gain, brightness, noise and legato lane
controls to declarative compositions. Initialize with `Grapple_GetChipExpressionDefaults`.
Curves and controls are copied into immutable songs and available in C++, Lua and Ruby.

MusicXML bends support pre-bends and bend/release pairs; first/last-beat percentages
bound the curve. Slides bend continuously over the latter half of the source note;
glissandi use chromatic semitone steps. Endpoints match by part, staff, voice,
instrument and span number. Slurs and hammer-ons/pull-offs reuse a voice envelope
within the logical line; chord notes at the same onset remain independent.

Staccato, staccatissimo and detached-legato default to 50%, 25% and 75% gates,
configurable through the import policy. Tenuto retains the full written duration.
Accents add 16 velocity units; marcato adds 26, unless explicit note dynamics already
supply performed strength. Attack/release offsets use exact division units.
Derived articulation gates round to the score resolution, refined to at least
1/1000 quarter beat where needed; encoded durations are never quantized.

Harmonic marks currently select a brighter, quieter tone using the encoded pitch;
stopped/palm-muted notes use darker, shorter gates. Cross noteheads on pitched
notes blend noise and shorten the gate; parenthesized noteheads reduce gain.
Tap/snap-pizzicato/fingernail markings sharpen attack strength and brightness.
These are deliberate C64-style timbral approximations, not acoustic instrument
models. Additional exporter-specific technical text remains under development.

## Grace notes, ornaments and ensemble timing

Grace groups honor `steal-time-following`, `steal-time-previous` and `make-time`.
Without an explicit timing attribute, each grace takes the configured 0.125
quarter beat from the following note, capped to half its duration for a group.
Grace chords share an onset. The donor must be adjacent in the same logical line;
ambiguous/overconsuming explicit allocations fail. Added-time grace groups insert
an ensemble-wide interval; simultaneous insertions use the longest requested span.

Trills alternate the main and key-signature neighbor at the configured subdivision
(default 0.125 quarter beat). Mordents play main/lower/main, inverted mordents
main/upper/main; turns play upper/main/lower/main, with inverted and delayed forms.
Explicit ornament accidentals replace the neighbor's key-signature alteration.
Supported trill attributes include attack count, starting note, trill step,
second/last attack percentages and acceleration. Common repeated and alternating
note tremolos honor strokes and attached beams; the latter combines the two
encoded half-length durations without reapplying `time-modification`.
Ornaments expand into bounded ordinary notes during import, never in the audio
callback. Informational diagnostics identify the selected interpretation policy.

Arpeggiation rolls explicitly voiced, numbered chord groups upward or downward
over the configured spread (default 0.125 quarter beat, capped at half the
shortest note). An explicit note attack offset takes precedence. Chord symbols
alone do not generate accompaniment.

Fermatas hold the ensemble at the note end for the configured extra duration
(default total length 1.5 times the note). Concurrent marks at the same position
extend it once, using the longest request. Notes ending there sustain; following
notes/controllers and source-measure positions shift together. Caesuras insert
silence (default 0.25 quarter beat); breath marks shorten the marked note
(default 85% gate) without changing the bar length. Both are configurable.
These hold policies add performed beat time; the synth's BPM-synchronized pulse
continues through the hold. They are deliberate performance interpretations,
not a claim that a fermata specifies a universal duration.
