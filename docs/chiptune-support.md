---
title: "From notation to game music"
description: "From notation to game music for grapple-beam callers."
---

# From notation to game music

The engine plays MusicXML through its C64-inspired synth: independent polyphonic
parts, pulse/saw/triangle/noise tones, envelopes, filtering and ring modulation.
It needs no SID ROM, soundfont, sample pack or external synth process. The presets emphasize wet, tempo-synchronized pulse motion; they are a sound
palette rather than an emulator of a particular SID chip or driver.

## Authoring in Guitar Pro or MuseScore

1. Put a role word in each track/part name. Matching is case-insensitive and uses
   whole words: `melody` / `lead`, `harmony` / `chords`, `bass`, and `percussion` /
   `percussions` / `drums`. Spaces, punctuation and underscores separate words;
   `Bassoon` does not match `bass`. The first recognized word wins a conflict.
   Add as many parts of each role as needed, up to 256 parts.
2. Write the performed notes, including each chord tone. Chord symbols alone do
   not specify a voicing. Use explicit tempo, dynamics and instrument/percussion
   assignments. Keep an original project file: export cannot preserve information
   that the authoring application omits.
3. Export MusicXML (`.xml` / `.musicxml`), or compressed MusicXML (`.mxl`). Both
   partwise and timewise structures work. MIDI remains useful for comparisons;
   MusicXML's encoded durations preserve tuplets without passing through MIDI.
4. Load with `Grapple_LoadChipSongEx` and inspect diagnostics. Strict mode is the
   default. Audition with the [standalone demo](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/chiptune/README.md), which
   also exports WAVs. Use `PlayChipFile` / `PlayChipSong` in your game, or construct
   a song with `CreateChipComposer`, `AddChipNoteEx`, tempo and controller calls.
5. Use rehearsal marks for named sections. Query their expanded tick bounds and
   choose intro/loop intervals from game code. A score's written repeats produce
   a finite performance; the game's loop is an independent transport setting.

Guitar Pro's supplied export duplicates notation in a TAB staff. Auto mode drops
only an exactly matching normalized TAB mirror within the same part. The fixture
therefore plays **87 notes in four parts, 120 BPM, ten seconds**, with concert
pitches and no doubled TAB notes. MuseScore 4.1.1 round-tripping that *combined*
notation/TAB file changes the TAB pitches and transposition metadata; those staves
are no longer equivalent. Export standard notation only, or set `options.staff=1`
for that export. Single-staff percussion remains present. The committed MuseScore
standard-staff fixture preserves all 87 expected notes. Independent piano staves,
unison parts and voices are never removed just because they share pitches.

Explicit preset overrides take precedence over role words. Otherwise unpitched
metadata/channel 10 selects drums, GM programs 32–39 select bass, 88–95 select
chords, and other pitched instruments select lead (program numbers here are
zero-based). MusicXML's channel/program/unpitched numbering is converted from
one-based values. Inspect the actual result and its reason with `ReadChipTrackMapping`.

| Role | Default behavior |
| --- | --- |
| Melody/lead | Moderate dotted-eighth delay (0.75 quarter beat), feedback and reverb |
| Harmony/chords | Wet ring tone, PWM/filter motion, eighth-note pulse (0.5 quarter beat) and room |
| Bass | Bass synth with chorus |
| Percussion/drums | Dry synthesized drum voices |

Pulse uses the song's quarter-beat clock through sustained notes and tempo/meter
changes. Its interval and depth are configurable for any preset or individual
part. Read defaults before changing one field to preserve the other settings.
Preset edits affect shared buses; `SetChipTrackEffects` creates an independent
part override. Seek resets phase to the target beat; a loop restores loop-start
phase and carries wet tails. Polyphony defaults to 64 voices for helper playback
and is configurable from 1 to 1,024, with bounded voice stealing when exhausted.

## Playback support matrix

**Implemented** means applied to timing, pitch, ordering or metadata.
**Approximated/configurable** means a documented synth/performance policy where a
symbol does not uniquely specify sound or timing. **Visual-only** means it need
not alter encoded performance. **Unsupported** constructs are diagnosed; malformed
or structurally ambiguous scores fail instead of silently producing corrupt music.
The interpreter is not a MusicXML schema validator or engraving application.

| Notation / feature | State | Interpretation and boundaries |
| --- | --- | --- |
| Notes, rests, whole-measure rests, chords, backup/forward, voices/staves | Implemented | Independent identities and simultaneous notes; silent parts retain ordering |
| Whole through short notes, dots, triplets, arbitrary/nested tuplets | Implemented | Exact encoded duration/divisions; displayed tuplet ratios are not applied twice |
| Pickups, incomplete/empty bars, additive/changing/compound meter | Implemented | Align performed extents across parts; empty metered bars use meter |
| Unmetered passages | Implemented | Explicit durations required; no guessed length for empty unmetered bars |
| Sharps/flats/naturals/doubles, key changes, enharmonic spelling | Implemented | Sounding pitch comes from step/alter/octave plus explicit transposition |
| Fractional alteration | Implemented | Equal-tempered semitone tuning per note, within documented pitch limits |
| Instrument/staff transpose | Implemented | Chromatic and octave transpose; sounding percussion uses instrument IDs |
| Clefs, display 8va/8vb, octave-shift signs, key glyphs | Visual-only | Do not double-apply display information to encoded sounding pitches |
| Ties, including chord tones and repeat-pass `time-only` | Implemented | One sustained attack; individual bend spans and final release articulation remain |
| Repeats, nested repeats, repeat counts, numbered/ranged endings | Implemented | Bounded finite expansion with source-state restoration |
| D.C., D.S., segno, coda, to-coda, fine | Implemented | Explicit sound directives and recognized conventional words; measure boundaries |
| Interior repeat/jump/target boundaries | Unsupported | Split the measure there; never silently shift navigation to its barline |
| Measure-repeat signs | Visual-only | MusicXML includes the performed notes; do not duplicate them again |
| Tempo, dotted/tied metronome units, metric equalities | Implemented | Maxima through 1024th; explicit BPM wins duplicate derived relationships |
| Swing/shuffle, straight, swing metronome relationship | Implemented/configurable | Explicit ratio and eighth/16th subdivision; default 2:1; encoded tuplets excluded |
| Ritardando/rallentando/accelerando | Approximated/configurable | Recognized words; stepped linear BPM ramp to explicit endpoint, otherwise ±25% |
| Dynamics, explicit velocity/expression, accents/marcato, sfz/fp | Implemented/configurable | Scoped part/staff/voice; explicit note dynamics take precedence |
| Crescendo/diminuendo, niente | Approximated/configurable | Linear note gain to explicit endpoint, otherwise a 32-velocity-unit change |
| Staccato/staccatissimo, tenuto, portato, breath | Approximated/configurable | Default gates 50%, 25%, 100%, 75%, 85%; encoded attack/release offsets honored |
| Slurs, legato, hammer-on/pull-off | Approximated/configurable | Matched voice lanes retain oscillator/envelope state; no cross-part pitch changes |
| Damper, sostenuto, soft pedal and pedal changes | Implemented | Scope-aware note holds; soft pedal darkens/softens attack; release at musical end |
| Grace/acciaccatura/appoggiatura, grace chords | Approximated/configurable | Explicit stolen/made time preferred; otherwise bounded following-note donation |
| Trill, shake, mordent/inverted/long, turn/inverted/delayed | Approximated/configurable | Bounded note expansion, key-aware/explicit neighbors, configurable subdivision |
| Ornament accidentals, wavy continuation, two-note endings | Implemented/configurable | Per-note neighbor/rate/ending rules; vertical/Haydn signs use turn policies |
| Repeated-note and alternating-note tremolo | Implemented/configurable | Strokes/beams determine attacks; no double application of tuplet duration factors |
| Arpeggiation/rolled chords, numbered groups, direction | Approximated/configurable | Explicit chord tones spread over 0.125 quarter beat by default |
| Fermata and caesura | Approximated/configurable | Ensemble hold 1.5× / added 0.25-beat silence by default; simultaneous holds merge |
| Slide/glissando, portamento | Approximated/configurable | Matched endpoints; latter-half continuous bend or chromatic steps |
| Bend, release, pre-bend, tremolo-bar/with-bar | Implemented/configurable | Independent continuous curves; acceleration honored; no MIDI control quantization |
| Vibrato/wide vibrato | Approximated/configurable | Independent beat-based pitch modulation through expression API/recognized technical text |
| Natural/artificial harmonics | Approximated/configurable | Explicit sounding pitch preferred; common base/touch pairs select partials 2–6 |
| Ambiguous harmonic or unpitched note without sound metadata | Unsupported | Exporter-omission diagnostic; display location cannot recover the missing pitch |
| Palm mute, stopped/muted/dead/ghost notes, pizzicato | Approximated/configurable | Darker/shorter or noisy/soft attacks as appropriate |
| Let ring | Approximated/configurable | Recognized technical text; next same-string attack (voice fallback), capped at bar end |
| Pick/bow directions, pluck, slap/pop/tap, fingernail, rasgueado | Approximated/configurable | Bright/noisy attack policies; use arpeggiate for explicitly timed strums |
| Scoop/plop/doit/falloff | Approximated/configurable | Two-semitone entry/exit pitch gestures |
| Rehearsal marks, MIDI markers, code section labels | Implemented metadata | Expanded start/end ticks, names, source measure/visit |
| Lyrics, fingering, fret/string labels, stems/beams, notehead layout, text, chord diagrams | Visual-only | No singing/accompaniment/engraving; string IDs still inform let-ring and beams inform tremolo |
| Cue notes | Visual-only | Advance the written position but do not produce audio |
| Chord symbols, slash/rhythm notation, staff tuning/scordatura display | Visual-only | Explicit pitches/rhythms remain authoritative; no invented voicings or acoustic retuning |
| Unknown technical/ornament/articulation/direction-type semantics | Unsupported | Strict rejection or retained permissive warning with available source location |
| Arbitrary prose / authoring-app effects omitted by export | Visual-only / unrecoverable | Only documented musical words are interpreted; put required playback into explicit notation/code |

These rows cover the supplied Guitar Pro palette by musical function: durations,
dots/tuplets, accidentals, dynamics/hairpins, articulations, ties/slurs, ornaments,
bends/slides/harmonics, picking/plucking, navigation, tempo, pedals and annotations.
UI controls for fingering, TAB, lyrics, chord diagrams and layout do not imply
separate acoustic behavior. No claim is made that every Guitar Pro-specific effect
survives MusicXML export. Unknown exported performance semantics are diagnosed;
an effect absent from the file cannot be detected or reconstructed by a parser.

## Formats and diagnostics

| Input | Compatibility |
| --- | --- |
| Guitar Pro 8.1.5 MusicXML 2.0 | Committed original fixture with exact concert-pitch/timing oracle |
| MusicXML 3.x / 4.0 partwise/timewise | Common structures supported, synthetic feature fixtures; MuseScore 4.1.1 export committed |
| MXL | Manifest-selected single score; ZIP32 stored/deflate, CRC/path/size validation; no extraction |
| MIDI | Standard MIDI files 0/1 with PPQN timing; format 2 and SMPTE division rejected |
| XML encoding | Expat-supported UTF-8, UTF-16, ASCII and ISO-8859-1; namespaces/entities/CDATA handled under bounded policy |
| ZIP64, encryption, split archives, multiple score roots | Explicitly unsupported; export a single ordinary MXL or uncompressed MusicXML |

Diagnostics distinguish input/score errors, references, conflicts, resource
bounds, unsupported semantics, exporter omissions, approximations and TAB mirrors.
They carry part, measure, staff, voice, XML element and line when available.
Strict mode rejects unsupported playback instructions, while still accepting the
documented configurable policies. Permissive mode warns when an instruction can
safely be skipped; invalid timing, missing pitches and ambiguous navigation remain
errors. Layout-only content does not block playback. Read diagnostic messages after
loading; on failure use the error record plus `SDL_GetError()`.

The detailed [interpretation/API contract](chiptune-musicxml.md) specifies rates,
limits, transport reconstruction, timing and ownership. The checked-in public
header provides Doxygen for every option and operation.
