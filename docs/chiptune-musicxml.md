# Chiptune MusicXML playback

Implementation is tracked in issue #68 and draft PR #69. This document records
the actual supported behavior as work progresses; it is not a claim that every
MusicXML element already has a performance interpretation.

`Grapple_LoadChipSong()` and its SDL_IOStream variant detect MIDI or uncompressed
MusicXML by content. The resulting immutable song uses the same player and
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

Compressed MXL, structured import diagnostics/options, score navigation/repeats,
expressive articulations/ornaments and guitar techniques, microtonal pitch,
advanced transport and the final support matrix remain tracked on the issue.
Grace notes and repeats currently fail explicitly. Other expressive constructs
are not yet guaranteed to affect playback; this importer is under development.

Expat is vendored and built statically from pinned source; see `deps/expat.md`.
The XML tree is limited to 300,000 nodes, depth 128, 64 attributes per element,
1 MiB of text per element and 64 MiB of retained tree allocations. Entity
declarations/external entities are rejected; DOCTYPE declarations do not fetch
DTDs. No filesystem extraction or network activity is needed for XML import.
