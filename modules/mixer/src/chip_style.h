/* Original Grapple code (zlib). Private style recipes and catalog. */
#ifndef GRAPPLE_CHIP_STYLE_H
#define GRAPPLE_CHIP_STYLE_H

#include <grapple/chiptune.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum ChipTriangleMode
    {
        CHIP_TRIANGLE_ANALOG = 0,
        CHIP_TRIANGLE_NES
    } ChipTriangleMode;

    typedef enum ChipFilterMode
    {
        CHIP_FILTER_SVF_LOW = 0,
        CHIP_FILTER_SVF_HIGH,
        CHIP_FILTER_OFF
    } ChipFilterMode;

    typedef enum ChipDrumProgram
    {
        CHIP_DRUM_NONE = 0,
        CHIP_DRUM_GM_ANALOG
    } ChipDrumProgram;

    typedef enum ChipStealMode
    {
        CHIP_STEAL_DEFAULT = 0,
        CHIP_STEAL_OLDEST,
        CHIP_STEAL_ROLE_THEN_GLOBAL
    } ChipStealMode;

    typedef struct ChipRoleRecipe
    {
        Grapple_ChipVoiceKind voice;
        float mix_pulse;
        float mix_saw;
        float mix_triangle;
        float mix_noise;
        float mix_sine;
        bool noise_metallic;
        ChipTriangleMode triangle;
        float duty;
        float pwm;
        float vibrato;
        float vibrato_delay_ms;
        float lfo_hz;
        float attack_ms;
        float decay_ms;
        float sustain;
        float release_ms;
        ChipFilterMode filter;
        float cutoff_hz;
        float filter_env;
        float ring_ratio;
        float fm_index;
        float level;
        int channels;
        Grapple_ChipPreset share;
        ChipDrumProgram drums;
        Grapple_ChipEffects effects;
    } ChipRoleRecipe;

    struct Grapple_ChipStyle
    {
        Grapple_ChipStyleInfo info;
        ChipRoleRecipe recipes[GRAPPLE_CHIP_PRESET_LAST + 1];
        ChipStealMode steal;
        float gain;
        char samples[256];
        bool interned;
        bool builtin;
    };

    const ChipRoleRecipe *Chip_C64Recipe(Grapple_ChipPreset preset);
    const Grapple_ChipStyle *Chip_C64Style(void);
    void Chip_EnsureStyleCatalog(void);
    void Chip_RegisterBuiltinStyles(void);
    Grapple_ChipStyle *Chip_ParseChipStyleJson(const char *json, size_t size, bool builtin);
    bool Chip_CopyStyleRecipes(const Grapple_ChipStyle *style, ChipRoleRecipe *out);
    bool Chip_ApplyStyleEffects(Grapple_ChipPlayer *player, const Grapple_ChipStyle *style);

#ifdef __cplusplus
}
#endif

#endif
