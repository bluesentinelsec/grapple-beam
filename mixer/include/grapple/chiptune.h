/**
 * @file chiptune.h
 * @brief 8-bit era music/sfx synthesis on top of Grapple::Mixer.
 *
 * Original Grapple code (zlib, like the rest of this repo) — NOT part of
 * upstream SDL_mixer. Everything renders offline to PCM and is returned as a
 * regular MIX_Audio, so playback, looping, gain, etc. all go through the
 * normal MIX_Track APIs. Rendering is fully deterministic: the same inputs
 * always produce bit-identical audio (the noise generator is a seeded LFSR).
 *
 * Three levels of API, lowest to highest:
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
extern "C" {
#endif

/** Waveforms of the 8-bit era. Values are stable (MML `Wn` uses them). */
typedef enum Grapple_ChipWave
{
    GRAPPLE_CHIP_SQUARE_125 = 0, /**< square, 12.5% duty (thin/nasal) */
    GRAPPLE_CHIP_SQUARE_25 = 1,  /**< square, 25% duty (classic lead) */
    GRAPPLE_CHIP_SQUARE_50 = 2,  /**< square, 50% duty (hollow/full) */
    GRAPPLE_CHIP_TRIANGLE = 3,   /**< 16-step quantized triangle (NES bass) */
    GRAPPLE_CHIP_SAW = 4,        /**< sawtooth */
    GRAPPLE_CHIP_NOISE = 5,      /**< 15-bit LFSR noise (drums/explosions) */
    GRAPPLE_CHIP_NOISE_METALLIC = 6, /**< short-loop LFSR (metallic buzz) */
    GRAPPLE_CHIP_SINE = 7            /**< pure sine (not on real hardware,
                                            but a useful soft voice) */
} Grapple_ChipWave;

/** One synthesized voice. Zero-init then set what you need. */
typedef struct Grapple_ChipToneDesc
{
    Grapple_ChipWave wave; /**< waveform */
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

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_CHIPTUNE_H */
