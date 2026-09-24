/* Original Grapple code (zlib). Fixed-capacity stereo instrument buses. */
#ifndef GRAPPLE_CHIP_EFFECTS_H
#define GRAPPLE_CHIP_EFFECTS_H
#include <grapple/chiptune.h>
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ChipEffectBus
    {
        Grapple_ChipEffects settings;
        float *delay;
        float *chorus;
        float *flanger;
        float *room;
        int delay_size;
        int chorus_size;
        int flanger_size;
        int room_size;
        int delay_pos;
        int chorus_pos;
        int flanger_pos;
        int comb_size[8];
        int comb_pos[8];
        float damp[8];
        float phaser_z[8];
        float delay_frames;
        double phase;
        double flanger_phase;
    } ChipEffectBus;

    bool Chip_EffectsInit(ChipEffectBus *bus, Grapple_ChipPreset preset, int rate);
    void Chip_EffectsClear(ChipEffectBus *bus);
    void Chip_EffectsDestroy(ChipEffectBus *bus);
    void Chip_EffectsProcess(ChipEffectBus *bus, int rate, float seconds_per_beat, float *left,
                             float *right);
    bool Chip_EffectsValid(const Grapple_ChipEffects *settings);
#ifdef __cplusplus
}
#endif
#endif
