/* Original Grapple code (zlib). Private SID-inspired voice DSP. */
#ifndef GRAPPLE_CHIP_VOICE_H
#define GRAPPLE_CHIP_VOICE_H

#include "chip_style.h"

#include <grapple/chiptune.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ChipSynthVoice
    {
        bool active;
        bool held;
        bool sostenuto;
        bool released;
        int track;
        int channel;
        int note;
        Uint64 serial;
        Uint32 note_id;
        Grapple_ChipExpression expression;
        double start_beat, duration_beats;
        double expression_beat, expression_duration;
        float gain_start, gain_target;
        double gain_beat, gain_duration;
        Uint32 noise;
        Grapple_ChipPreset preset;
        ChipRoleRecipe recipe;
        int drum;
        double phase;
        double mod_phase;
        double noise_phase;
        double lfo_phase;
        float frequency;
        float envelope;
        float attack_step;
        float decay_step;
        float sustain;
        float release_step;
        float amplitude;
        float instrument_gain, instrument_left, instrument_right;
        float age;
        float low;
        float band;
        float cutoff;
        float sweep;
        float sweep_decay;
        float noise_value;
    } ChipSynthVoice;

    void Chip_VoiceStart(ChipSynthVoice *voice, Grapple_ChipPreset preset, int note, int velocity,
                         int sample_rate);
    void Chip_VoiceStartRecipe(ChipSynthVoice *voice, const ChipRoleRecipe *recipe,
                               Grapple_ChipPreset preset, int note, int velocity, int sample_rate);
    void Chip_VoiceRelease(ChipSynthVoice *voice, int sample_rate);
    float Chip_VoiceSample(ChipSynthVoice *voice, float bend, float modulation, int sample_rate);
    float Chip_VoiceSampleMotion(ChipSynthVoice *voice, float bend, float modulation,
                                 int sample_rate, float motion, float pulse);

#ifdef __cplusplus
}
#endif

#endif
