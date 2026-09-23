/* Original Grapple code (zlib). Recipe-driven SID-inspired voice DSP; not cycle-exact. */
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

static float AnalogTriangle(double phase)
{
    return 1.0f - 4.0f * SDL_fabsf((float)phase - 0.5f);
}

static float NesTriangle(double phase)
{
    const int step = (int)(phase * 32.0);
    const int level = (step < 16) ? step : (31 - step);
    return ((float)level / 7.5f) - 1.0f;
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

void Chip_VoiceStartRecipe(ChipSynthVoice *v, const ChipRoleRecipe *recipe,
                           Grapple_ChipPreset preset, int note, int velocity, int sample_rate)
{
    SDL_zero(*v);
    Grapple_GetChipExpressionDefaults(&v->expression);
    v->gain_start = v->gain_target = 1;
    v->instrument_gain = v->instrument_left = v->instrument_right = 1;
    v->active = true;
    v->held = true;
    v->preset = preset;
    v->recipe = *recipe;
    v->note = note;
    v->noise = 0x5a17bu ^ ((Uint32)note * 7919u);
    v->frequency = 440.0f * SDL_powf(2.0f, ((float)note - 69.0f) / 12.0f);
    float attack = SDL_max(recipe->attack_ms, 0.1f) / 1000.0f;
    float decay = SDL_max(recipe->decay_ms, 1.0f) / 1000.0f;
    float cutoff = recipe->cutoff_hz > 0 ? recipe->cutoff_hz : 6500.0f;
    v->sustain = recipe->sustain;
    float level = recipe->level > 0 ? recipe->level : 0.23f;
    if (recipe->drums == CHIP_DRUM_GM_ANALOG)
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

void Chip_VoiceStart(ChipSynthVoice *v, Grapple_ChipPreset preset, int note, int velocity,
                     int sample_rate)
{
    Chip_VoiceStartRecipe(v, Chip_C64Recipe(preset), preset, note, velocity, sample_rate);
}

void Chip_VoiceRelease(ChipSynthVoice *v, int sample_rate)
{
    if (v->released)
        return;
    v->released = true;
    const float seconds = SDL_max(v->recipe.release_ms, 1.0f) / 1000.0f;
    v->release_step = v->envelope / (seconds * (float)sample_rate);
}

float Chip_VoiceSampleMotion(ChipSynthVoice *v, float bend, float modulation, int sample_rate,
                             float motion, float pulse)
{
    const ChipRoleRecipe *r = &v->recipe;
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
    const float vibrato = (v->age * 1000.0f > r->vibrato_delay_ms) ? r->vibrato : 0.0f;
    float frequency =
        (v->frequency + v->sweep) * bend * (1.0f + lfo * (vibrato + modulation * 0.012f));
    frequency = SDL_clamp(frequency, 1.0f, (float)sample_rate * 0.4f);
    const double step = (double)frequency / ((double)sample_rate * CHIP_OVERSAMPLE);
    float duty = r->duty + r->pwm * lfo + motion * (pulse - 0.5f) * 0.35f;
    duty = SDL_clamp(duty, 0.05f, 0.95f);
    float filter = v->cutoff;
    if (r->filter_env > 0)
        filter *= 0.40f + 0.60f * v->envelope;
    filter *= 1.0f - motion * 0.82f * (1.0f - pulse);
    filter = SDL_min(filter * v->expression.brightness, 0.65f);
    float sample = 0.0f;
    const float mix_sum = r->mix_pulse + r->mix_saw + r->mix_triangle + r->mix_noise + r->mix_sine;
    for (int i = 0; i < CHIP_OVERSAMPLE; ++i)
    {
        const float analog_tri = AnalogTriangle(v->phase);
        const float nes_tri = NesTriangle(v->phase);
        const float triangle = r->triangle == CHIP_TRIANGLE_NES ? nes_tri : analog_tri;
        float wave = 0.0f;
        if (v->drum && r->drums == CHIP_DRUM_GM_ANALOG)
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
                wave = analog_tri;
            else if (v->drum == 2)
                wave = 0.75f * wave + 0.25f * analog_tri;
        }
        else if (r->voice == GRAPPLE_CHIP_VOICE_FM)
        {
            const float index = r->fm_index > 0 ? r->fm_index : r->ring_ratio;
            const float mod = SDL_sinf((float)v->mod_phase * 2.0f * SDL_PI_F);
            wave = SDL_sinf(((float)v->phase + mod * index * 0.35f) * 2.0f * SDL_PI_F);
        }
        else if (r->ring_ratio > 0 && r->mix_triangle > 0)
        {
            wave =
                (triangle * (1.0f - motion * 0.2f) + Pulse(v->phase, step, duty) * motion * 0.2f) *
                Pulse(v->mod_phase, step * (double)r->ring_ratio, 0.5f);
        }
        else if (mix_sum > 0)
        {
            const float saw = (float)(2.0 * v->phase - 1.0) - PolyBlep(v->phase, step);
            if (r->mix_pulse > 0)
                wave += r->mix_pulse * Pulse(v->phase, step, r->mix_saw > 0 ? 0.5f : duty);
            if (r->mix_saw > 0)
                wave += r->mix_saw * saw;
            if (r->mix_triangle > 0)
                wave += r->mix_triangle * triangle;
            if (r->mix_sine > 0)
                wave += r->mix_sine * SDL_sinf((float)v->phase * 2.0f * SDL_PI_F);
            if (r->mix_noise > 0)
            {
                const Uint32 tap = r->noise_metallic ? 6u : 1u;
                const Uint32 feedback = (v->noise ^ (v->noise >> tap)) & 1u;
                v->noise = ((v->noise << 1) | feedback) & 0x7fffffu;
                wave += r->mix_noise * ((v->noise & 1u) ? 1.0f : -1.0f);
            }
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
        if (r->filter == CHIP_FILTER_OFF ||
            (v->drum && r->drums == CHIP_DRUM_GM_ANALOG && r->filter == CHIP_FILTER_OFF))
        {
            sample += wave;
        }
        else
        {
            const float high = wave - v->low - (1.25f - motion * 0.55f) * v->band;
            v->band += filter * high;
            v->low += filter * v->band;
            const bool highpass = v->drum >= 3 && v->drum != 5 && r->drums == CHIP_DRUM_GM_ANALOG;
            if (highpass || r->filter == CHIP_FILTER_SVF_HIGH)
                sample += high;
            else
                sample += v->low;
        }
        v->phase += step;
        v->phase -= SDL_floor(v->phase);
        const double mod_step = r->ring_ratio > 0 ? step * (double)r->ring_ratio : step * 2.0;
        v->mod_phase += mod_step;
        v->mod_phase -= SDL_floor(v->mod_phase);
    }
    v->sweep *= v->sweep_decay;
    const double lfo_hz = r->lfo_hz > 0 ? (double)r->lfo_hz : 5.2;
    v->lfo_phase += lfo_hz / sample_rate;
    v->lfo_phase -= SDL_floor(v->lfo_phase);
    v->age += 1.0f / (float)sample_rate;
    return sample * (1.0f / CHIP_OVERSAMPLE) * v->envelope * v->amplitude * v->expression.gain;
}

float Chip_VoiceSample(ChipSynthVoice *v, float bend, float modulation, int sample_rate)
{
    return Chip_VoiceSampleMotion(v, bend, modulation, sample_rate, 0.0f, 1.0f);
}
