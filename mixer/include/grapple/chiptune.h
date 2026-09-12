/**
 * @file chiptune.h
 * @brief 8-bit era music/sfx synthesis on top of Grapple::Mixer.
 *
 * Original Grapple code (zlib, like the rest of this repo) — NOT part of
 * upstream SDL_mixer. The original tone/SFX/MML functions render offline to PCM as a
 * regular MIX_Audio, so playback, looping, gain, etc. all go through the
 * normal MIX_Track APIs. Rendering is fully deterministic: the same inputs
 * always produce bit-identical audio (the noise generator is a seeded LFSR).
 *
 * MIDI songs additionally support streaming polyphonic SID-inspired synthesis.
 * See Grapple_LoadChipSong_IO() and Grapple_CreateChipPlayer().
 *
 * Three levels of offline API, lowest to highest:
 *
 *  1. Grapple_CreateChipTone()  — one waveform "voice": square (three duty
 *     cycles), 16-step triangle, saw, or NES-style LFSR noise, with optional
 *     pitch sweep, vibrato, and attack/release envelope.
 *  2. Grapple_CreateChipSFX()   — canned game sound effects (coin, laser,
 *     jump, explosion, powerup, hurt).
 *  3. Grapple_CreateChipTune()  — compile a Music Macro Language (MML)
 *     string, the dialect of 8-bit BASIC `PLAY` statements, into a finished
 *     multi-channel tune.
 *
 * MML dialect (case-insensitive, whitespace ignored):
 *
 *   A-G[#+-][n][.]  play a note; `#`/`+` sharp, `-` flat; `n` = length
 *                   (1=whole, 4=quarter, ... up to 64), dots extend by half
 *   R[n][.]         rest
 *   On              set octave 0-8 (default 4; O4 A = 440 Hz)
 *   < >             octave down / up
 *   Ln              default note length (default 4)
 *   Tn              tempo in BPM, 20-600 (default 120)
 *   Vn              channel volume 0-15 (default 10)
 *   Wn              waveform 0-7, see Grapple_ChipWave (default 2 = square)
 *   Sn              envelope shape: 0 = flat sustain (default), 1 = decay
 *                   across the note, 2 = short percussive pluck — use S2 on
 *                   a noise channel for drums
 *   ;               start the next channel (up to 8; mixed like NES voices)
 *
 * Example — the classic NES lineup: pulse lead, pulse counterpoint,
 * triangle bass, noise percussion:
 *
 *   MIX_Audio *tune = Grapple_CreateChipTune(mixer,
 *       "T140 W1 O5 L8 C E G >C< G E C4 ;"
 *       "T140 W2 O4 L8 E G B >E< B G E4 ;"
 *       "T140 W3 O2 L4 C G E G ;"
 *       "T140 W5 S2 L8 O3 C O6 C O3 C O6 C O3 C O6 C O3 C O6 C");
 *
 * The result loops seamlessly if the channels line up on the same total
 * length. On parse errors these functions return NULL with SDL_GetError()
 * describing the offending character and position.
 */
#ifndef GRAPPLE_CHIPTUNE_H
#define GRAPPLE_CHIPTUNE_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** Waveforms of the 8-bit era. Values are stable (MML `Wn` uses them). */
    typedef enum Grapple_ChipWave
    {
        GRAPPLE_CHIP_SQUARE_125 = 0,     /**< square, 12.5% duty (thin/nasal) */
        GRAPPLE_CHIP_SQUARE_25 = 1,      /**< square, 25% duty (classic lead) */
        GRAPPLE_CHIP_SQUARE_50 = 2,      /**< square, 50% duty (hollow/full) */
        GRAPPLE_CHIP_TRIANGLE = 3,       /**< 16-step quantized triangle (NES bass) */
        GRAPPLE_CHIP_SAW = 4,            /**< sawtooth */
        GRAPPLE_CHIP_NOISE = 5,          /**< 15-bit LFSR noise (drums/explosions) */
        GRAPPLE_CHIP_NOISE_METALLIC = 6, /**< short-loop LFSR (metallic buzz) */
        GRAPPLE_CHIP_SINE = 7            /**< pure sine (not on real hardware,
                                                but a useful soft voice) */
    } Grapple_ChipWave;

    /** One synthesized voice. Zero-init then set what you need. */
    typedef struct Grapple_ChipToneDesc
    {
        Grapple_ChipWave wave;   /**< waveform */
        float freq_hz;           /**< pitch; for noise, the LFSR clock feel */
        float freq_end_hz;       /**< >0: exponential sweep to this pitch */
        Uint32 duration_ms;      /**< total length, must be > 0 */
        float volume;            /**< 0..1 */
        Uint32 attack_ms;        /**< linear fade-in */
        Uint32 release_ms;       /**< linear fade-out at the end */
        float vibrato_hz;        /**< vibrato rate; 0 = none */
        float vibrato_semitones; /**< vibrato depth in semitones */
    } Grapple_ChipToneDesc;

    /** Canned sound effects. */
    typedef enum Grapple_ChipSFX
    {
        GRAPPLE_SFX_COIN = 0,
        GRAPPLE_SFX_LASER,
        GRAPPLE_SFX_JUMP,
        GRAPPLE_SFX_EXPLOSION,
        GRAPPLE_SFX_POWERUP,
        GRAPPLE_SFX_HURT
    } Grapple_ChipSFX;

    /**
     * Render a single tone/voice to a MIX_Audio.
     * \returns a new MIX_Audio (destroy with MIX_DestroyAudio), or NULL on error.
     */
    extern MIX_Audio *Grapple_CreateChipTone(MIX_Mixer *mixer, const Grapple_ChipToneDesc *desc);

    /**
     * Render a canned game sound effect.
     * \returns a new MIX_Audio, or NULL on error.
     */
    extern MIX_Audio *Grapple_CreateChipSFX(MIX_Mixer *mixer, Grapple_ChipSFX which);

    /**
     * Compile an MML string (dialect documented above) into a MIX_Audio.
     * \returns a new MIX_Audio, or NULL on parse error (see SDL_GetError()).
     */
    extern MIX_Audio *Grapple_CreateChipTune(MIX_Mixer *mixer, const char *mml);

    /** @brief Built-in SID-inspired instrument programs; no external patches needed. */
    typedef enum Grapple_ChipPreset
    {
        GRAPPLE_CHIP_PRESET_AUTO =
            0,                     /**< Use the first case-insensitive role word in the part name:
                                   melody/lead, harmony/chords, bass, percussion/percussions/drums.
                                   Whole words are separated by ASCII punctuation or whitespace
                                   (including underscores); digits belong to words. Otherwise
                                   use MIDI channel 10 for drums, GM bass/pads for bass/chord,
                                   and lead for remaining programs. */
        GRAPPLE_CHIP_PRESET_LEAD,  /**< Pulse-width modulation and vibrato. */
        GRAPPLE_CHIP_PRESET_BASS,  /**< Filtered pulse/saw bass. */
        GRAPPLE_CHIP_PRESET_CHORD, /**< Polyphonic pulse pad. */
        GRAPPLE_CHIP_PRESET_RING,  /**< Triangle carrier polarity-switched by a private oscillator.
                                    */
        GRAPPLE_CHIP_PRESET_DRUMS, /**< GM note-number mapping to synthesized percussion. */
        GRAPPLE_CHIP_PRESET_HARMONY =
            GRAPPLE_CHIP_PRESET_RING /**< Recommended harmony: ring timbre, wet eighth-note pulse by
                                        default. */
    } Grapple_ChipPreset;

    /** @brief Immutable composition. Players retain it independently. */
    typedef struct Grapple_ChipSong Grapple_ChipSong;
    /** @brief Severity of a score import diagnostic. */
    typedef enum Grapple_ChipDiagnosticSeverity
    {
        GRAPPLE_CHIP_DIAGNOSTIC_INFO,
        GRAPPLE_CHIP_DIAGNOSTIC_WARNING,
        GRAPPLE_CHIP_DIAGNOSTIC_ERROR
    } Grapple_ChipDiagnosticSeverity;
    /** @brief Stable machine-readable categories for import diagnostics. */
    typedef enum Grapple_ChipDiagnosticCode
    {
        GRAPPLE_CHIP_DIAGNOSTIC_NONE,
        GRAPPLE_CHIP_DIAGNOSTIC_INPUT,
        GRAPPLE_CHIP_DIAGNOSTIC_SCORE,
        GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED,
        GRAPPLE_CHIP_DIAGNOSTIC_APPROXIMATION,
        GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR
    } Grapple_ChipDiagnosticCode;
    /** @brief Source location and category; message text is retrieved separately. */
    typedef struct Grapple_ChipDiagnostic
    {
        Grapple_ChipDiagnosticCode code;         /**< NONE means no failure in an error output. */
        Grapple_ChipDiagnosticSeverity severity; /**< Informational, warning or error. */
        int part;                                /**< Zero-based part, or -1 if unavailable. */
        int measure; /**< Zero-based source measure, or -1 if unavailable. */
        int staff;   /**< One-based staff, or 0 if unavailable. */
        Uint32 line; /**< One-based XML line, or 0 if unavailable. */
    } Grapple_ChipDiagnostic;
    /** @brief Score interpretation policy; obtain defaults before overriding fields. */
    typedef struct Grapple_ChipImportOptions
    {
        bool strict; /**< Reject unsupported playback semantics instead of reporting warnings. */
        int staff; /**< 0: detect TAB mirrors; -1: retain all; 1..32: select a staff in multi-staff
                      parts. */
        double staccato_gate;      /**< Fraction of notated length, 0 selects 0.5. */
        double staccatissimo_gate; /**< Fraction of notated length, 0 selects 0.25. */
        double portato_gate;       /**< Detached-legato fraction, 0 selects 0.75. */
        double breath_gate;        /**< Breath-mark gate fraction, 0 selects 0.85. */
        double caesura_beats;      /**< Ensemble pause in quarter beats, 0 selects 0.25. */
        double grace_beats; /**< Default grace-note duration in quarter beats; 0 selects 0.125. */
        double ornament_beats; /**< Default ornament subdivision; 0 selects 0.125 quarter beats. */
        double arpeggio_beats; /**< Total rolled-chord spread; 0 selects 0.125 quarter beats. */
        double fermata_factor; /**< Default fermata duration multiplier; 0 selects 1.5. */
        double swing_ratio;    /**< Long/short ratio for explicitly requested swing; 0 selects 2. */
    } Grapple_ChipImportOptions;
    /**
     * @brief Copy the default score import policy (strict, automatic staff selection).
     * @param options Output value owned by the caller.
     * @return True, or false for NULL output.
     */
    extern bool Grapple_GetChipImportDefaults(Grapple_ChipImportOptions *options);
    /**
     * @brief Load a notation file with explicit interpretation policy.
     * @param path MIDI/MusicXML/MXL file to read.
     * @param options Copied during loading; NULL uses defaults.
     * @param error Optional error location/category; message is in SDL_GetError().
     * @return Owned song, or NULL on failure. Successful imports retain warnings.
     */
    extern Grapple_ChipSong *Grapple_LoadChipSongEx(const char *path,
                                                    const Grapple_ChipImportOptions *options,
                                                    Grapple_ChipDiagnostic *error);
    /**
     * @brief Read a notation stream using an explicit interpretation policy.
     * @param io Stream positioned at the document start; may be nonseekable.
     * @param closeio Close the stream on success and failure when true.
     * @param options Copied during loading; NULL uses defaults.
     * @param error Optional error location/category; message is in SDL_GetError().
     * @return Owned song, or NULL on failure.
     */
    extern Grapple_ChipSong *Grapple_LoadChipSong_IOEx(SDL_IOStream *io, bool closeio,
                                                       const Grapple_ChipImportOptions *options,
                                                       Grapple_ChipDiagnostic *error);
    /** @brief Count retained import diagnostics. @param song Song to inspect. @return Count, or 0
     * for NULL. */
    extern int Grapple_GetChipDiagnosticCount(const Grapple_ChipSong *song);
    /**
     * @brief Copy one retained import diagnostic.
     * @param song Song to inspect.
     * @param index Zero-based diagnostic index.
     * @param diagnostic Caller-owned output.
     * @return True, or false for invalid arguments.
     */
    extern bool Grapple_ReadChipDiagnostic(const Grapple_ChipSong *song, int index,
                                           Grapple_ChipDiagnostic *diagnostic);
    /**
     * @brief Read diagnostic message text.
     * @param song Song to inspect.
     * @param index Zero-based diagnostic index.
     * @return Borrowed text valid while song lives; NULL for an invalid index. Scripts copy it.
     */
    extern const char *Grapple_GetChipDiagnosticMessage(const Grapple_ChipSong *song, int index);
    /** @brief Stateful polyphonic renderer with an owned SDL audio stream. */
    typedef struct Grapple_ChipPlayer Grapple_ChipPlayer;

    /** @brief MIDI timing and track information, including conductor tracks. */
    typedef struct Grapple_ChipSongInfo
    {
        int track_count;         /**< Number of file tracks, including metadata-only tracks. */
        int ticks_per_quarter;   /**< Original MIDI PPQN; no rhythmic quantization is applied. */
        Uint64 duration_ticks;   /**< End of the last track, preserving leading/trailing rests. */
        double duration_seconds; /**< Musical duration, excluding synthesis release tails. */
    } Grapple_ChipSongInfo;

    /** @brief A file track; MIDI channel numbers and track indices are distinct. */
    typedef struct Grapple_ChipTrackInfo
    {
        char name[128];    /**< NUL-terminated track name; long names are truncated. */
        Uint16 channels;   /**< Bit mask of MIDI channels used (bit 9 is percussion). */
        Uint32 note_count; /**< Number of nonzero-velocity note-on events. */
        int first_program; /**< First zero-based GM program, or -1 if absent. */
    } Grapple_ChipTrackInfo;

    /**
     * @brief Load MIDI type 0/1, MusicXML or compressed MXL without rendering audio.
     * @param io Input stream positioned at the document start; may be nonseekable.
     * @param closeio Whether to close io on success or failure.
     * @return Owned song, or NULL with SDL_GetError().
     * @details Supports PPQN timing, tempo maps, running status, and track names.
     * Type 2, SMPTE division and nonzero MIDI ports are rejected explicitly.
     * SysEx and unrecognized metadata are skipped. Limits: 64 MiB, 256 tracks,
     * one million retained events, and 24 hours. All parsing/allocation occurs here.
     * MusicXML partwise/timewise scores preserve exact division-based timing,
     * independent parts, ties, transposition and mapped percussion. Matching
     * notation/TAB mirrors are played once. MusicXML support is documented in
     * docs/chiptune-musicxml.md.
     */
    extern Grapple_ChipSong *Grapple_LoadChipSong_IO(SDL_IOStream *io, bool closeio);

    /**
     * @brief Load a MIDI or MusicXML song from a filesystem path.
     * @param path Score filename; the format is detected from its contents.
     * @return Owned song, or NULL with SDL_GetError().
     */
    extern Grapple_ChipSong *Grapple_LoadChipSong(const char *path);

    /**
     * @brief Release a song; NULL is allowed.
     * @param song Song reference to release; existing players retain their own references.
     */
    extern void Grapple_DestroyChipSong(Grapple_ChipSong *song);

    /**
     * @brief Inspect immutable song metadata.
     * @param song Loaded song.
     * @return Borrowed metadata valid until song destruction, or NULL for NULL song.
     */
    extern const Grapple_ChipSongInfo *Grapple_GetChipSongInfo(const Grapple_ChipSong *song);

    /**
     * @brief Inspect a track, including its original name and channel mask.
     * @param song Loaded song.
     * @param track Zero-based file track index (including conductor tracks).
     * @return Borrowed metadata, or NULL with SDL_GetError() for an invalid index.
     */
    extern const Grapple_ChipTrackInfo *Grapple_GetChipTrackInfo(const Grapple_ChipSong *song,
                                                                 int track);

    /**
     * @brief Create a stereo F32 player, preallocating its voices and stream.
     * @param song Immutable song; the player retains a reference until its destruction.
     * @param sample_rate Output rate, 8000..192000 Hz.
     * @param voices Simultaneous note capacity, 1..1024 (64 is a useful default).
     * @param loop Restart at the exact musical end, resetting voices/controllers.
     * @return Owned player, or NULL with SDL_GetError().
     * @details Each ring-modulated note has a private modulator, consuming no other
     * note voice. At capacity, the quietest releasing voice, then the oldest held
     * voice, is replaced. Non-looping playback includes up to twelve seconds of effect tails.
     */
    extern Grapple_ChipPlayer *Grapple_CreateChipPlayer(const Grapple_ChipSong *song,
                                                        int sample_rate, int voices, bool loop);

    /**
     * @brief Destroy a player and its owned stream; NULL is allowed.
     * @param player Player already detached from any mixer track or stream consumer.
     */
    extern void Grapple_DestroyChipPlayer(Grapple_ChipPlayer *player);

    /**
     * @brief Get the player's demand-driven stream for MIX_SetTrackAudioStream().
     * @param player Player to stream.
     * @return Borrowed stream; do not destroy, reconfigure, or replace its callback.
     * @details Assign to at most one MIX_Track. Destroy/detach that track before the
     * player. MIX_PlayTrack/PauseTrack/ResumeTrack control playback. Set presets
     * before playing for immediate results; queued audio adds latency to live edits.
     */
    extern SDL_AudioStream *Grapple_GetChipPlayerStream(Grapple_ChipPlayer *player);

    /**
     * @brief Override one file track's preset and gain, including all its channels.
     * @param player Player to configure.
     * @param track Zero-based file track index.
     * @param preset Explicit preset overrides the part name; AUTO restores name-based mapping,
     * falling back to MIDI program changes when no role word matches.
     * @param gain Linear gain, 0..2; zero mutes the part.
     * @return True on success, false with SDL_GetError().
     * @details Safe during stream playback. Preset changes affect new notes; gain
     * changes affect active notes. MIDI volume, expression and pan remain effective.
     */
    extern bool Grapple_SetChipTrackPreset(Grapple_ChipPlayer *player, int track,
                                           Grapple_ChipPreset preset, float gain);

    /**
     * @brief Render directly into interleaved stereo F32 PCM, without allocations.
     * @param player Player with no concurrent stream consumer or API calls.
     * @param stereo Buffer for frames * 2 floats; unused output is zero-filled.
     * @param frames Requested frame count (nonnegative).
     * @return Frames rendered; zero at end, or -1 with SDL_GetError() on bad input.
     * @details Timing is sample-clock driven and independent of block size. Use
     * either direct rendering or the borrowed stream, never both concurrently.
     */
    extern int Grapple_RenderChipPlayer(Grapple_ChipPlayer *player, float *stereo, int frames);

    /**
     * @brief Rewind playback and clear queued audio, voices, and MIDI controllers.
     * @param player Player to reset; track presets/gains are preserved.
     * @details Safe during stream playback. A stopped MIX_Track needs MIX_PlayTrack
     * again. Consumer buffers outside the player's stream are not cleared.
     */
    extern void Grapple_ResetChipPlayer(Grapple_ChipPlayer *player);

    /**
     * @brief Query the peak simultaneous note voices since creation/reset.
     * @param player Player to inspect; safe during stream playback.
     * @return Peak voice count, or zero for NULL.
     */
    extern int Grapple_GetChipPlayerPeakVoices(Grapple_ChipPlayer *player);

    /** @brief One declarative note. Tick positions preserve triplets exactly at 480 PPQN. */
    typedef struct Grapple_ChipNote
    {
        int track;             /**< Zero-based instrument part. */
        int note;              /**< MIDI pitch or percussion key, 0..127. */
        int velocity;          /**< Note strength, 1..127. */
        Uint64 start_tick;     /**< Absolute onset in the composition's tick timebase. */
        Uint64 duration_ticks; /**< Positive duration; chord notes may share an onset. */
    } Grapple_ChipNote;

    /** @brief Per-note performance controls. Initialize with Grapple_GetChipExpressionDefaults. */
    typedef struct Grapple_ChipExpression
    {
        float tuning;        /**< Constant pitch offset in semitones, -48..48. */
        float gain;          /**< Additional linear amplitude, 0..2. Default 1. */
        float brightness;    /**< Filter multiplier, 0.1..2. Default 1. */
        float noise;         /**< Noise blend for muted/dead notes, 0..1. */
        float bend_start;    /**< Initial pitch displacement in semitones, -48..48. */
        float bend_peak;     /**< Middle pitch displacement in semitones, -48..48. */
        float bend_end;      /**< Final pitch displacement in semitones, -48..48. */
        float bend_first;    /**< Fraction of duration before first bend segment, 0..1. */
        float bend_middle;   /**< Fraction of duration at peak, first..last. Default 0.5. */
        float bend_last;     /**< Fraction of duration after final segment, middle..1. Default 1. */
        float vibrato_depth; /**< Pitch vibrato depth in semitones, 0..12. */
        float vibrato_beats; /**< Vibrato period in quarter-note beats, 0.01..16. Default 0.25. */
        int lane;            /**< Logical monophonic line within a part, 0..65535. */
        bool legato; /**< Reuse the preceding voice in the lane without restarting its envelope. */
        bool stepped_pitch; /**< Quantize the bend to semitone steps for chromatic glissando. */
    } Grapple_ChipExpression;

    /** @brief Initialize neutral per-note expression.
     * @param expression Output value; NULL is ignored. */
    extern void Grapple_GetChipExpressionDefaults(Grapple_ChipExpression *expression);

    /** @brief Mutable composition builder, independent of a file format. */
    typedef struct Grapple_ChipComposer Grapple_ChipComposer;

    /**
     * @brief Create a composer with a fixed number of parts and tick resolution.
     * @param tracks Number of parts, 1..256.
     * @param ticks_per_quarter Resolution, 1..32767; 480 supports eighth triplets at 160 ticks.
     * @return Owned composer, or NULL with SDL_GetError(). Default tempo is 120 BPM.
     */
    extern Grapple_ChipComposer *Grapple_CreateChipComposer(int tracks, int ticks_per_quarter);
    /** @brief Destroy a composer; built songs are independent snapshots. @param composer Owned
     * composer, or NULL. */
    extern void Grapple_DestroyChipComposer(Grapple_ChipComposer *composer);
    /**
     * @brief Name a part and select its default instrument and gain.
     * @param composer Composer to edit.
     * @param track Zero-based part index.
     * @param name Part name, copied into the composition.
     * @param preset Default instrument; AUTO resolves role words in the part name.
     * @param gain Linear gain, 0..2.
     * @return True on success, false with SDL_GetError().
     */
    extern bool Grapple_SetChipPart(Grapple_ChipComposer *composer, int track, const char *name,
                                    Grapple_ChipPreset preset, float gain);
    /**
     * @brief Add a note; notes may be declared in any order and may overlap.
     * @param composer Composer to edit.
     * @param note Note description, copied during the call.
     * @return True on success, false with SDL_GetError(). Rejection leaves the composer unchanged.
     */
    extern bool Grapple_AddChipNote(Grapple_ChipComposer *composer, const Grapple_ChipNote *note);
    /** @brief Add a note with independent pitch, timbre and legato controls.
     * @param composer Composer to edit.
     * @param note Note description, copied during the call.
     * @param expression Expression, copied; NULL selects neutral defaults.
     * @return True on success; false with SDL_GetError(), without changing the composition. */
    extern bool Grapple_AddChipNoteEx(Grapple_ChipComposer *composer, const Grapple_ChipNote *note,
                                      const Grapple_ChipExpression *expression);
    /**
     * @brief Add a tempo change at an absolute tick position.
     * @param composer Composer to edit.
     * @param tick Position of the change.
     * @param bpm Quarter notes per minute, 20..600; stored to MIDI microsecond precision.
     * @return True on success, false with SDL_GetError().
     */
    extern bool Grapple_AddChipTempo(Grapple_ChipComposer *composer, Uint64 tick, double bpm);
    /**
     * @brief Build an immutable, independently owned song snapshot.
     * @param composer Composer to snapshot; may be edited or destroyed afterwards.
     * @param end_tick Musical end (including rests), or zero to use the last event.
     * @return Owned song, or NULL with SDL_GetError(). End cannot precede the last event.
     */
    extern Grapple_ChipSong *Grapple_BuildChipSong(const Grapple_ChipComposer *composer,
                                                   Uint64 end_tick);

    /** @brief Per-instrument effects. Zero initialization disables every effect, including motion.
     */
    typedef struct Grapple_ChipEffects
    {
        float chorus; /**< Chorus strength, 0..1 (up to 50% wet, retaining the dry fundamental). */
        float delay;  /**< Stereo echo wet gain, 0..1. */
        float delay_beats;    /**< Quarter-note beats per echo, 0..4; audible delays require >0. */
        float delay_feedback; /**< Echo regeneration, 0..0.85. */
        float reverb;         /**< Room reverb wet gain, 0..1. */
        float motion;         /**< Harmony pulse-width/filter sweep depth, 0..1. */
        double pulse_beats; /**< Quarter-note beats per pulse, 0.0625..16; 0 disables pulse. Default
                               0.5 (eighths). */
        float
            pulse_depth; /**< Harmony amplitude pulse depth, 0..1; zero leaves sustained volume. */
    } Grapple_ChipEffects;
    /**
     * @brief Retrieve the built-in effect settings for an instrument.
     * @param preset Explicit instrument (LEAD through DRUMS); AUTO is invalid.
     * @param effects Output settings. Lead: tempo delay/reverb; bass: chorus; drums: dry.
     * @return True on success, false with SDL_GetError(). CHORD/RING enable harmony motion.
     */
    extern bool Grapple_GetChipPresetEffects(Grapple_ChipPreset preset,
                                             Grapple_ChipEffects *effects);
    /**
     * @brief Override effects for every voice using an instrument preset.
     * @param player Player to configure; safe during stream playback.
     * @param preset Explicit instrument (LEAD through DRUMS).
     * @param effects Settings copied during the call. Existing echo/reverb tails are cleared.
     * @return True on success, false with SDL_GetError().
     * @details Echo follows tempo changes, with smoothed delay-time transitions; delay is capped
     * at four seconds. Non-looping effect tails are bounded to twelve seconds after the musical
     * end.
     */
    extern bool Grapple_SetChipPresetEffects(Grapple_ChipPlayer *player, Grapple_ChipPreset preset,
                                             const Grapple_ChipEffects *effects);
    /**
     * @brief Play through a player-owned mixer on the default audio device.
     * @param player Player to start or resume. Initialize SDL audio and MIX_Init first.
     * @return True on success, false with SDL_GetError().
     * @details Intended for simple applications and scripts. The owned mixer is destroyed before
     * the player stream. Do not also attach the stream to another consumer or render it directly.
     * Game integrations may instead use GetChipPlayerStream with their existing mixer.
     */
    extern bool Grapple_PlayChipPlayer(Grapple_ChipPlayer *player);
    /** @brief Pause the player-owned mixer without rewinding. @param player Managed player. */
    extern void Grapple_PauseChipPlayer(Grapple_ChipPlayer *player);
    /** @brief Stop and detach the player-owned mixer, then rewind. @param player Managed player. */
    extern void Grapple_StopChipPlayer(Grapple_ChipPlayer *player);
    /** @brief Query managed playback. @param player Managed player. @return True while playing,
     * false when paused/stopped or unmanaged. */
    extern bool Grapple_ChipPlayerPlaying(Grapple_ChipPlayer *player);
    /**
     * @brief Copy metadata into caller storage (also available as script tables/hashes).
     * @param song Loaded or composed song.
     * @param info Output metadata.
     * @return True on success, false with SDL_GetError().
     */
    extern bool Grapple_ReadChipSongInfo(const Grapple_ChipSong *song, Grapple_ChipSongInfo *info);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_CHIPTUNE_H */
