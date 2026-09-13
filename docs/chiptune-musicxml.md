---
title: "Chiptune MusicXML playback"
description: "Chiptune MusicXML playback for grapple-beam callers."
---

# Chiptune MusicXML playback

Compose in Guitar Pro or MuseScore, export MusicXML, and play it through the same
C64-inspired library used for MIDI and code-authored music. See the
[authoring workflow and support matrix](chiptune-support.md),
[runnable examples](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md), and
[builds, resource limits, and validation](chiptune-validation.md).

`Grapple_LoadChipSong()` and its SDL_IOStream variant detect MIDI, uncompressed
MusicXML, or compressed MXL by content. The resulting immutable song uses the same player and
instrument-name contract as MIDI and declarative composition. All XML parsing
and score compilation happen at load time, outside the audio callback.

## Supported score structure

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
input durations; it never rounds them to a default MIDI grid. The
resource limit is 100 million ticks per quarter note, 65,536 measures, 32 staves
per part, 256 parts, one million retained events and 24 hours of performance.
Scores that exceed exact arithmetic/resource limits fail explicitly.

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

Plain harmonic marks select a brighter, quieter tone using the encoded pitch;
stopped/palm-muted notes use darker, shorter gates. Cross noteheads on pitched
notes blend noise and shorten the gate; parenthesized noteheads reduce gain.
Tap/snap-pizzicato/fingernail markings sharpen attack strength and brightness.
These are deliberate C64-style timbral approximations, not acoustic instrument
models. Unrecognized technical text is diagnosed under the import policy.

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

## Dynamics, pedals, swing and tempo curves

Dynamics are resolved at musical positions, including offsets, rather than in
XML document order. Unqualified directions apply to the part; explicit staff
and voice qualifiers narrow their scope. Hairpins match by scope and number,
interpolate linearly to an explicit endpoint dynamic, or change by 32 velocity
units when none is given. Niente starts/ends at silence. The player interpolates
note-specific gain over beats without retriggering held notes. New explicit
dynamic instructions interrupt an earlier curve. Explicit note dynamics take
precedence. SF/ SFZ/ RFZ attacks briefly emphasize the note; FP/SFP decay to piano.

Damper and sostenuto extend only the applicable notes until release. Sostenuto
captures notes already held at pedal-down; later attacks are not captured.
Pedal changes release the prior held set. Soft pedal gives a quieter/darker
attack. Explicit sound pedal attributes take precedence over duplicated symbols.
A pedal left down releases at the musical end. MIDI retains channel semantics;
code composition can use `Grapple_AddChipControl` for modulation, volume, pan,
expression, damper/sostenuto/soft pedals and standard reset/note-off controls.
The C++, Lua and Ruby bindings expose these controls too.

Explicit swing ratios and eighth/16th subdivisions are honored. Only notes with
the matching type and nominal unswung duration qualify; grace notes and explicit
tuplets retain their timing. `straight` disables swing. The exact default 2:1
ratio is also available through a `Swing`/`Shuffle` direction; the ratio is
configurable. Pulse and delay remain on the shared quarter-beat tempo clock.

Recognized tempo-ramp words are `rit.`, `ritardando`, `rall.`, `rallentando`,
`accel.` and `accelerando`. They interpolate BPM to the next explicit tempo.
Without an endpoint, the policy changes tempo by 25% over up to four quarter
beats. Ramps compile at 32 steps per quarter, capped at 4,096 steps, and use the
same exact tempo integration as MIDI. This is a documented interpretation of
specific musical words, not arbitrary natural-language processing. Scoped
controllers/dynamics and active curves are restored when revisiting a passage.

## Game transport and independent mixing

`ReadChipPlayerPosition` reports the next render cursor: seconds, quarter beats,
ticks, source MusicXML measure and expanded measure visit. MIDI and code songs
have no source-measure map (`-1`). The audible device position trails the render
cursor by its buffering. `SeekChipPlayer` accepts expanded ticks and reconstructs
controllers, tempo, held notes and approximate envelope age without synthesizing
the entire preceding song. It clears queued PCM, filter/noise history and wet
tails, with a 2 ms fade-in; it does not reproduce a historical reverb waveform.
Pause state is preserved, and seeking before first play chooses the starting point.

`SetChipPlayerLoop(start, end, enabled)` supports an intro followed by an interval.
Loop starts are inclusive and ends exclusive. Prepare loops on the loading/control
thread: a bounded checkpoint restores controller and voice state at each wrap,
with no allocation or event-history scan in the callback. Notes spanning the start
are restored, notes spanning the end are discarded, effect tails carry across,
and edges fade over 2 ms. Pulse phase returns to the source beat at loop start.
Fractional sample overflow carries across loops; the frame clock uses compensated
addition for runtime tempo scaling. Score repeats and game loops are separate.

`SetChipPlayerTempo` scales speed from 0.25 to 4 without transposing notes. Pulse,
delay and expression follow beats; ADSR remains in real seconds. Reset/stop clears
sound and rewinds while preserving mix, effects, tempo scale and loop settings.
`SetChipPlayerGain` controls the complete output including wet tails before the
soft clipping stage. Musical end drains tails for up to twelve real seconds.

`SetChipTrackMix` sets gain, stereo balance, mute and solo without changing notes
or roles. Shared preset-bus tails continue when a contributing part is muted;
master gain zero silences everything. `SetChipTrackEffects` assigns a private bus
with copied settings, so one harmony can change independently. Private bus output
follows mute/solo immediately. NULL restores shared routing. Replacing a private
bus clears its old tail; it cannot remove a tail already mixed into a shared bus.
Preset-wide edits leave private buses unchanged. `ReadChipTrackEffects` and
`ReadChipTrackMapping` expose effective settings and mapping precedence, including
the current channel program for mixed-instrument MIDI tracks.

Control calls are serialized by the caller and lock the SDL stream against the
managed callback. Direct PCM rendering requires caller serialization with control
calls. Loop/tempo/preset changes may rebuild a checkpoint, and effect overrides
allocate; call them on a control/loading thread. Render, note dispatch and loop
wraps do not allocate. There are at most 32 private buses per player, each about
1.6 MiB at 48 kHz, in addition to the five shared buses. Buffer size scales with
sample rate. Voice/channel checkpoints are proportional to configured polyphony
and part count; callback scratch space is about 4 KiB plus voice DSP stack.

## Instruments within a part

Instrument IDs are indexed per part (up to 256). A declared score instrument
without MIDI metadata uses a pitched default. The first declared instrument is
the default for notes without an ID; lexical ID order does not change it.
Program, volume and pan are carried per note, so simultaneous instruments sharing
a nominal MIDI channel do not overwrite each other's timbre or stereo placement.
Explicit part controllers remain a separate layer. `sound/midi-instrument` changes
resolve by source position and offset before navigation expansion, including
multiple voices and out-of-order XML note traversal. They affect subsequent
attacks; held notes retain their instrument. Repeats restore the source passage's
instrument assignment. Unpitched instrument metadata selects percussion regardless
of the nominal channel, unless an explicit preset or role name overrides it.

## Metronome relationships and unmetered music

Beat units cover maxima through 1024th, dots and tied units. Metric equalities
scale the preceding quarter-note tempo by the new/old notated beat duration.
Explicit BPM at the same position takes precedence. Duplicate equivalent
relationships across parts apply once; conflicting metric ratios fail. The
common two-eighths = quarter/eighth-triplet metronome relationship sets swing
instead of changing the total beat duration. Other complete metric-note
equalities compare their summed durations. Relationships are bounded to sixteen
notes on each side; arbitrary text relations require explicit tempo instead.
Unmetered measures (`senza-misura`) use their explicit note/rest/forward durations;
an empty unmetered measure needs an explicit duration and fails otherwise.

## Additional gestures and diagnostic locations

Diagnostic records include the voice identifier, staff and local XML element name
when available. Codes distinguish malformed score data, unsupported performance,
missing/duplicate references, resource bounds, conflicts and missing sounding
information. Unknown children in performance-sensitive technical, articulation,
ornament and direction-type groups reject in strict mode and warn in permissive
mode. Unknown technical text is diagnosed instead of silently treated as a sound.

Harmonics use encoded sounding pitches when provided. A chord's explicit base and
touching pitches are visual companions when it includes a sounding pitch; they do
not create additional voices. Without a sounding pitch, paired octave, fifth,
fourth, major-third and minor-third touch intervals select partials 2–6 of the
base, retaining fractional semitone tuning. Other/ambiguous base-touch descriptions
need an explicit sounding pitch. A plain harmonic mark brightens and softens its
encoded note. This models a synth harmonic, not the physical string instrument.

Recognized `other-technical` text includes `let ring`/`let-ring`, normal/wide
vibrato, slap, pop, palm mute, ghost/dead note, upstroke, downstroke and rasgueado.
Let-ring extends to the next attack on the encoded string (or voice if no string
is supplied), capped at the bar end. Pick/bow/pluck directions, slap/pop/tap and
rasgueado use documented bright/noisy attack approximations; explicit arpeggiate
marks provide timed rolls. Pizzicato shortens the gate and brightens the attack.
Scoop/plop/doit/falloff use two-semitone entry/exit bends. These are timbral policies
for the C64 palette rather than claims of acoustic reproduction.

Bends containing `with-bar` use the same independent pitch curve; marking multiple
notes bends each without detuning unrelated parts. Acceleration is quadratic within
each bend segment. MusicXML's bend `beats` counts discrete MIDI-like control samples;
it does not specify musical duration, so the synth evaluates the continuous curve
per sample instead of adding control quantization. Trill wavy-line continuations
extend across notes in the logical voice. Long mordents use five attacks, explicit
two-note endings use their half/whole-step neighbor, and shake/vertical/Haydn symbols
use the corresponding configurable trill/turn approximation.

Tie `time-only` attributes select the actual enclosing repeat pass, including
nested repeats and endings. They are evaluated during order expansion before tie
chains are joined, so a tie on the first pass need not suppress a second-pass attack.

## Named sections

MusicXML rehearsal marks and MIDI marker meta-events become named sections.
`GetChipSectionCount` / `ReadChipSection` return copied names and expanded tick
bounds. Marks repeated by score navigation appear once per visit and retain
source-measure/visit indexes; duplicate marks from multiple parts merge. The next
mark at a distinct time ends an imported section, or the song end does. A marker
at the end is metadata with zero length and cannot define a transport loop.
Code composers use `AddChipSection(name, start_tick, end_tick)` for explicit bounds.
Sections sort by start and name, with a limit of 4,096 and names of 127 UTF-8 bytes.
Pass a section's bounds to `SetChipPlayerLoop`, or its start to `SeekChipPlayer`.
Lua/Ruby return names and diagnostic text fields as strings.

Tied notes retain each written bend/release span and the final articulation while
using one sustained synth voice. A continuation without another bend holds the
preceding bend's endpoint. Seeking reconstructs the active expression segment.
Navigation targets must be at measure starts and jumps at measure ends; split a
measure at an interior navigation/repeat boundary. The importer diagnoses interior
boundaries instead of silently moving them to a barline, even in permissive mode.
