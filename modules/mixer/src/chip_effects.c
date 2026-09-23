/* Original Grapple code (zlib). Modulated chorus, tempo echo and damped parallel-comb room. */
#include "chip_effects.h"

bool Grapple_GetChipPresetEffects(Grapple_ChipPreset preset, Grapple_ChipEffects *effects)
{
    if (!effects || preset < GRAPPLE_CHIP_PRESET_FIRST || preset > GRAPPLE_CHIP_PRESET_LAST)
        return SDL_SetError("chiptune: invalid effect preset");
    SDL_zero(*effects);
    if (preset == GRAPPLE_CHIP_PRESET_LEAD)
    {
        effects->delay = 0.28f;
        effects->delay_beats = 0.75f;
        effects->delay_feedback = 0.32f;
        effects->reverb = 0.18f;
    }
    else if (preset == GRAPPLE_CHIP_PRESET_BASS)
        effects->chorus = 0.30f;
    else if (preset == GRAPPLE_CHIP_PRESET_CHORD || preset == GRAPPLE_CHIP_PRESET_RING)
    {
        effects->motion = 1.0f;
        effects->pulse_beats = 0.5;
        effects->pulse_depth = 0.7f;
        effects->reverb = 0.10f;
    }
    return true;
}

bool Chip_EffectsValid(const Grapple_ChipEffects *s)
{
    return s && s->chorus >= 0 && s->chorus <= 1 && s->delay >= 0 && s->delay <= 1 &&
           s->delay_beats >= 0 && s->delay_beats <= 4 && s->delay_feedback >= 0 &&
           s->delay_feedback <= 0.85f && s->reverb >= 0 && s->reverb <= 1 && s->motion >= 0 &&
           s->motion <= 1 && (s->delay == 0 || s->delay_beats > 0) && s->pulse_depth >= 0 &&
           s->pulse_depth <= 1 &&
           (s->pulse_beats == 0 || (s->pulse_beats >= 0.0625 && s->pulse_beats <= 16));
}

bool Chip_EffectsInit(ChipEffectBus *b, Grapple_ChipPreset preset, int rate)
{
    SDL_zero(*b);
    if (!Grapple_GetChipPresetEffects(preset, &b->settings))
        return false;
    b->delay_size = rate * 4 + 2;
    b->chorus_size = rate / 20 + 2;
    const float lengths[8] = {0.0297f, 0.0371f, 0.0411f, 0.0437f,
                              0.0307f, 0.0383f, 0.0423f, 0.0449f};
    for (int i = 0; i < 8; ++i)
    {
        b->comb_size[i] = (int)(lengths[i] * (float)rate);
        b->room_size += b->comb_size[i];
    }
    b->delay = SDL_calloc((size_t)b->delay_size * 2, sizeof(float));
    b->chorus = SDL_calloc((size_t)b->chorus_size * 2, sizeof(float));
    b->room = SDL_calloc((size_t)b->room_size, sizeof(float));
    return b->delay && b->chorus && b->room;
}

void Chip_EffectsClear(ChipEffectBus *b)
{
    if (b->delay)
        SDL_memset(b->delay, 0, (size_t)b->delay_size * 2 * sizeof(float));
    if (b->chorus)
        SDL_memset(b->chorus, 0, (size_t)b->chorus_size * 2 * sizeof(float));
    if (b->room)
        SDL_memset(b->room, 0, (size_t)b->room_size * sizeof(float));
    SDL_zeroa(b->comb_pos);
    SDL_zeroa(b->damp);
    b->delay_pos = b->chorus_pos = 0;
    b->phase = 0;
    b->delay_frames = 0;
}

void Chip_EffectsDestroy(ChipEffectBus *b)
{
    SDL_free(b->delay);
    SDL_free(b->chorus);
    SDL_free(b->room);
}

static float ReadDelay(const float *buffer, int size, int position, float frames, int channel)
{
    /* Float addition can round a slightly negative read position up to size. */
    double index = (double)position - (double)frames;
    if (index < 0)
        index += (double)size;
    const int first = (int)index;
    const int second = (first + 1) % size;
    const float fraction = (float)(index - (double)first);
    return buffer[first * 2 + channel] * (1 - fraction) + buffer[second * 2 + channel] * fraction;
}

void Chip_EffectsProcess(ChipEffectBus *b, int rate, float seconds_per_beat, float *left,
                         float *right)
{
    const Grapple_ChipEffects *s = &b->settings;
    if (s->chorus > 0)
    {
        b->chorus[b->chorus_pos * 2] = *left;
        b->chorus[b->chorus_pos * 2 + 1] = *right;
        const float lfo = SDL_sinf((float)b->phase * 2.0f * SDL_PI_F);
        const float delayed_left = ReadDelay(b->chorus, b->chorus_size, b->chorus_pos,
                                             (0.016f + lfo * 0.003f) * (float)rate, 0);
        const float delayed_right = ReadDelay(b->chorus, b->chorus_size, b->chorus_pos,
                                              (0.018f - lfo * 0.003f) * (float)rate, 1);
        *left = *left * (1 - s->chorus * 0.5f) + delayed_left * s->chorus * 0.5f;
        *right = *right * (1 - s->chorus * 0.5f) + delayed_right * s->chorus * 0.5f;
        b->chorus_pos = (b->chorus_pos + 1) % b->chorus_size;
        b->phase += 0.65 / rate;
        if (b->phase >= 1)
            b->phase -= 1;
    }
    if (s->delay > 0)
    {
        const float target = SDL_clamp(seconds_per_beat * s->delay_beats * (float)rate, 1.0f,
                                       (float)(b->delay_size - 2));
        if (b->delay_frames == 0)
            b->delay_frames = target;
        b->delay_frames += (target - b->delay_frames) * (1.0f / (0.02f * (float)rate));
        const float echo_left =
            ReadDelay(b->delay, b->delay_size, b->delay_pos, b->delay_frames, 0);
        const float echo_right =
            ReadDelay(b->delay, b->delay_size, b->delay_pos, b->delay_frames, 1);
        b->delay[b->delay_pos * 2] = *left + echo_right * s->delay_feedback;
        b->delay[b->delay_pos * 2 + 1] = *right + echo_left * s->delay_feedback;
        *left += echo_left * s->delay;
        *right += echo_right * s->delay;
        b->delay_pos = (b->delay_pos + 1) % b->delay_size;
    }
    if (s->reverb > 0)
    {
        const float input = (*left + *right) * 0.5f;
        float wet[2] = {0, 0};
        int offset = 0;
        for (int i = 0; i < 8; ++i)
        {
            float *cell = &b->room[offset + b->comb_pos[i]];
            const float delayed = *cell;
            b->damp[i] = 0.55f * b->damp[i] + 0.45f * delayed;
            *cell = input + b->damp[i] * 0.76f;
            wet[i / 4] += delayed * 0.25f;
            b->comb_pos[i] = (b->comb_pos[i] + 1) % b->comb_size[i];
            offset += b->comb_size[i];
        }
        *left += wet[0] * s->reverb;
        *right += wet[1] * s->reverb;
    }
}
