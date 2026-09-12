/* Original Grapple code (zlib). SID-inspired, not cycle-exact hardware emulation. */
#include "chip_voice.h"

#define CHIP_OVERSAMPLE 4

static float PolyBlep(double phase, double step)
{
    if (phase < step)
    {
        const float t = (float)(phase / step);
        return t + t - t * t - 1.0f;
    }
    if (phase > 1.0 - step)
    {
        const float t = (float)((phase - 1.0) / step);
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

static float Pulse(double phase, double step, float duty)
{
    double edge = phase - duty;
    if (edge < 0.0)
        edge += 1.0;
    return (phase < duty ? 1.0f : -1.0f) + PolyBlep(phase, step) - PolyBlep(edge, step);
}

static int DrumKind(int note)
{
    if (note == 35 || note == 36)
        return 1;
    if (note == 38 || note == 40 || note == 37 || note == 39)
        return 2;
    if (note == 42 || note == 44)
        return 3;
    if (note == 46)
        return 4;
    if (note == 41 || note == 43 || note == 45 || note == 47 || note == 48 || note == 50)
        return 5;
    return 6;
}

void Chip_VoiceStart(ChipSynthVoice *v, Grapple_ChipPreset preset, int note, int velocity,
                     int sample_rate)
{
    SDL_zero(*v);
    Grapple_GetChipExpressionDefaults(&v->expression);
    v->active = true;
    v->held = true;
    v->preset = preset;
    v->note = note;
    v->noise = 0x5a17bu ^ ((Uint32)note * 7919u);
    v->frequency = 440.0f * SDL_powf(2.0f, ((float)note - 69.0f) / 12.0f);
    float attack = 0.004f;
    float decay = 0.16f;
    float cutoff = 6500.0f;
    v->sustain = 0.65f;
    float level = 0.23f;
    if (preset == GRAPPLE_CHIP_PRESET_BASS)
    {
        level = 0.34f;
        v->sustain = 0.5f;
        cutoff = 1400.0f;
        decay = 0.12f;
    }
    else if (preset == GRAPPLE_CHIP_PRESET_CHORD || preset == GRAPPLE_CHIP_PRESET_RING)
    {
        attack = 0.012f;
        decay = 0.28f;
        v->sustain = 0.55f;
        level = preset == GRAPPLE_CHIP_PRESET_RING ? 0.24f : 0.14f;
        cutoff = preset == GRAPPLE_CHIP_PRESET_RING ? 7000.0f : 3800.0f;
    }
    else if (preset == GRAPPLE_CHIP_PRESET_DRUMS)
    {
        v->drum = DrumKind(note);
        attack = 0.001f;
        v->sustain = 0.0f;
        level = 0.45f;
        decay = 0.20f;
        cutoff = 8000.0f;
        if (v->drum == 1)
        {
            v->frequency = 48.0f;
            v->sweep = 170.0f;
            cutoff = 1800.0f;
            decay = 0.24f;
        }
        else if (v->drum == 2)
        {
            v->frequency = 165.0f;
            v->sweep = 100.0f;
            decay = 0.19f;
        }
        else if (v->drum == 3 || v->drum == 4)
        {
            level = 0.20f;
            decay = v->drum == 3 ? 0.065f : 0.35f;
        }
        else if (v->drum == 5)
        {
            v->frequency = 85.0f * SDL_powf(2.0f, ((float)note - 41.0f) / 12.0f);
            v->sweep = 90.0f;
            cutoff = 2600.0f;
            decay = 0.27f;
        }
        else
        {
            level = 0.20f;
            decay = 0.7f;
        }
    }
    v->attack_step = 1.0f / (attack * (float)sample_rate);
    v->decay_step = (1.0f - v->sustain) / (decay * (float)sample_rate);
    v->amplitude = level * (float)velocity / 127.0f;
    v->cutoff = 2.0f * SDL_sinf(SDL_PI_F * SDL_min(cutoff, (float)sample_rate * 0.4f) /
                                ((float)sample_rate * CHIP_OVERSAMPLE));
    v->sweep_decay = SDL_expf(-1.0f / (0.035f * (float)sample_rate));
}

void Chip_VoiceRelease(ChipSynthVoice *v, int sample_rate)
{
    if (v->released)
        return;
    v->released = true;
    const float seconds =
        (v->preset == GRAPPLE_CHIP_PRESET_RING || v->preset == GRAPPLE_CHIP_PRESET_CHORD) ? 0.22f
                                                                                          : 0.07f;
    v->release_step = v->envelope / (seconds * (float)sample_rate);
}

float Chip_VoiceSampleMotion(ChipSynthVoice *v, float bend, float modulation, int sample_rate,
                             float motion, float pulse)
{
    if (v->released)
        v->envelope = SDL_max(0.0f, v->envelope - v->release_step);
    else if (v->age < 1.0f / (v->attack_step * (float)sample_rate))
        v->envelope = SDL_min(1.0f, v->envelope + v->attack_step);
    else
        v->envelope = SDL_max(v->sustain, v->envelope - v->decay_step);
    if (v->envelope <= 0.0f && (v->released || v->age > 0.01f))
    {
        v->active = false;
        return 0.0f;
    }
    const float lfo = SDL_sinf((float)v->lfo_phase * 2.0f * SDL_PI_F);
    const float vibrato =
        (v->preset == GRAPPLE_CHIP_PRESET_LEAD && v->age > 0.12f) ? 0.0025f : 0.0f;
    float frequency =
        (v->frequency + v->sweep) * bend * (1.0f + lfo * (vibrato + modulation * 0.012f));
    frequency = SDL_clamp(frequency, 1.0f, (float)sample_rate * 0.4f);
    const double step = (double)frequency / ((double)sample_rate * CHIP_OVERSAMPLE);
    const float duty = 0.30f + 0.12f * lfo + motion * (pulse - 0.5f) * 0.35f;
    float filter =
        v->cutoff * (v->preset == GRAPPLE_CHIP_PRESET_BASS ? 0.40f + 0.60f * v->envelope : 1.0f);
    filter *= 1.0f - motion * 0.82f * (1.0f - pulse);
    filter = SDL_min(filter * v->expression.brightness, 0.65f);
    float sample = 0.0f;
    for (int i = 0; i < CHIP_OVERSAMPLE; ++i)
    {
        const float triangle = 1.0f - 4.0f * SDL_fabsf((float)v->phase - 0.5f);
        float wave;
        if (v->drum)
        {
            v->noise_phase += 12000.0 / ((double)sample_rate * CHIP_OVERSAMPLE);
            if (v->noise_phase >= 1.0)
            {
                v->noise_phase -= 1.0;
                const Uint32 feedback = ((v->noise >> 22) ^ (v->noise >> 17)) & 1u;
                v->noise = ((v->noise << 1) | feedback) & 0x7fffffu;
                v->noise_value = (v->noise & 1u) ? 1.0f : -1.0f;
            }
            wave = v->noise_value;
            if (v->drum == 1 || v->drum == 5)
                wave = triangle;
            else if (v->drum == 2)
                wave = 0.75f * wave + 0.25f * triangle;
        }
        else if (v->preset == GRAPPLE_CHIP_PRESET_RING)
        {
            /* SID ring modulation flips a triangle's polarity using another
             * oscillator's MSB. Each polyphonic note has its own modulator. */
            wave =
                (triangle * (1.0f - motion * 0.2f) + Pulse(v->phase, step, duty) * motion * 0.2f) *
                Pulse(v->mod_phase, step * 2.0, 0.5f);
        }
        else if (v->preset == GRAPPLE_CHIP_PRESET_BASS)
        {
            const float saw = (float)(2.0 * v->phase - 1.0) - PolyBlep(v->phase, step);
            wave = 0.65f * Pulse(v->phase, step, 0.5f) + 0.35f * saw;
        }
        else
            wave = Pulse(v->phase, step, duty);
        if (v->expression.noise > 0 && !v->drum)
        {
            const Uint32 feedback = ((v->noise >> 22) ^ (v->noise >> 17)) & 1u;
            v->noise = ((v->noise << 1) | feedback) & 0x7fffffu;
            wave = wave * (1 - v->expression.noise) +
                   ((v->noise & 1u) ? 1.0f : -1.0f) * v->expression.noise;
        }
        const float high = wave - v->low - (1.25f - motion * 0.55f) * v->band;
        v->band += filter * high;
        v->low += filter * v->band;
        sample += (v->drum >= 3 && v->drum != 5) ? high : v->low;
        v->phase += step;
        v->phase -= SDL_floor(v->phase);
        v->mod_phase += step * 2.0;
        v->mod_phase -= SDL_floor(v->mod_phase);
    }
    v->sweep *= v->sweep_decay;
    v->lfo_phase += 5.2 / sample_rate;
    v->lfo_phase -= SDL_floor(v->lfo_phase);
    v->age += 1.0f / (float)sample_rate;
    return sample * (1.0f / CHIP_OVERSAMPLE) * v->envelope * v->amplitude * v->expression.gain;
}

float Chip_VoiceSample(ChipSynthVoice *v, float bend, float modulation, int sample_rate)
{
    return Chip_VoiceSampleMotion(v, bend, modulation, sample_rate, 0.0f, 1.0f);
}
