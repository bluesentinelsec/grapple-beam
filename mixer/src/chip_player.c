/* Original Grapple code (zlib). Sample-clock sequencer and SDL streaming adapter. */
#include "chip_effects.h"
#include "chip_internal.h"
#include "chip_voice.h"

typedef struct ChipChannel
{
    int program;
    int bend;
    int bend_semitones;
    int bend_cents;
    int rpn_msb;
    int rpn_lsb;
    bool sustain;
    float pitch;
    float volume;
    float expression;
    float pan_left;
    float pan_right;
    float modulation;
} ChipChannel;

typedef struct ChipPart
{
    Grapple_ChipPreset preset;
    Grapple_ChipPreset named_preset;
    float gain;
} ChipPart;

struct Grapple_ChipPlayer
{
    const Grapple_ChipSong *song;
    SDL_AudioStream *stream;
    MIX_Mixer *managed_mixer;
    MIX_Track *managed_track;
    ChipEffectBus effects[GRAPPLE_CHIP_PRESET_DRUMS + 1];
    bool effect_used[GRAPPLE_CHIP_PRESET_DRUMS + 1];
    double beat;
    Uint32 tempo;
    Uint64 quiet_frames;
    ChipSynthVoice *voices;
    ChipPart *parts;
    ChipChannel *channels;
    int channel_count;
    int sample_rate;
    int voice_count;
    int peak_voices;
    bool loop;
    bool ended;
    bool flushed;
    size_t event;
    Uint64 frame;
    Uint64 end_frame;
    Uint64 serial;
};

static void UpdateBend(ChipChannel *channel)
{
    const float semitones = (float)channel->bend_semitones + (float)channel->bend_cents / 100.0f;
    channel->pitch =
        SDL_powf(2.0f, ((float)channel->bend - 8192.0f) * semitones / (8192.0f * 12.0f));
}

static void ResetControllers(ChipChannel *channel)
{
    channel->bend = 8192;
    channel->bend_semitones = 2;
    channel->bend_cents = 0;
    channel->rpn_msb = 127;
    channel->rpn_lsb = 127;
    channel->sustain = false;
    channel->pitch = 1.0f;
    channel->expression = 1.0f;
    channel->modulation = 0.0f;
}

static int EventChannel(const Grapple_ChipPlayer *p, const ChipEvent *event)
{
    return (p->song->independent_parts ? event->track * 16 : 0) + (event->status & 15);
}

static void Rewind(Grapple_ChipPlayer *p)
{
    SDL_memset(p->voices, 0, (size_t)p->voice_count * sizeof(*p->voices));
    for (int i = 0; i < p->channel_count; ++i)
    {
        ChipChannel *channel = &p->channels[i];
        SDL_zero(*channel);
        ResetControllers(channel);
        channel->volume = 1.0f;
        channel->pan_left = 0.70710678f;
        channel->pan_right = 0.70710678f;
    }
    /* Some exporters place the initial program after a note at tick zero. */
    for (size_t i = 0; i < p->song->count && p->song->events[i].tick == 0; ++i)
    {
        const ChipEvent *event = &p->song->events[i];
        if ((event->status >> 4) == 12)
            p->channels[EventChannel(p, event)].program = event->a;
    }
    p->beat = 0;
    p->tempo = 500000;
    p->quiet_frames = 0;
    p->event = 0;
    p->frame = 0;
    p->serial = 0;
    p->ended = false;
    p->flushed = false;
}

static bool IsNameWordByte(unsigned char c)
{
    return SDL_isalnum(c) || c >= 128;
}

static Grapple_ChipPreset PresetFromName(const char *name)
{
    static const struct
    {
        const char *word;
        Grapple_ChipPreset preset;
    } roles[] = {
        {"melody", GRAPPLE_CHIP_PRESET_LEAD},       {"lead", GRAPPLE_CHIP_PRESET_LEAD},
        {"harmony", GRAPPLE_CHIP_PRESET_HARMONY},   {"chords", GRAPPLE_CHIP_PRESET_HARMONY},
        {"bass", GRAPPLE_CHIP_PRESET_BASS},         {"percussion", GRAPPLE_CHIP_PRESET_DRUMS},
        {"percussions", GRAPPLE_CHIP_PRESET_DRUMS}, {"drums", GRAPPLE_CHIP_PRESET_DRUMS}};
    while (*name)
    {
        if (!IsNameWordByte((unsigned char)*name))
        {
            ++name;
            continue;
        }
        const char *start = name;
        while (IsNameWordByte((unsigned char)*name))
            ++name;
        const size_t length = (size_t)(name - start);
        for (size_t i = 0; i < SDL_arraysize(roles); ++i)
            if (SDL_strlen(roles[i].word) == length &&
                SDL_strncasecmp(start, roles[i].word, length) == 0)
                return roles[i].preset;
    }
    return GRAPPLE_CHIP_PRESET_AUTO;
}

static Grapple_ChipPreset ResolvePreset(const Grapple_ChipPlayer *p, const ChipEvent *event)
{
    const Grapple_ChipPreset preset = p->parts[event->track].preset;
    if (preset != GRAPPLE_CHIP_PRESET_AUTO)
        return preset;
    const Grapple_ChipPreset named_preset = p->parts[event->track].named_preset;
    if (named_preset != GRAPPLE_CHIP_PRESET_AUTO)
        return named_preset;
    const int channel = event->status & 15;
    if (channel == 9)
        return GRAPPLE_CHIP_PRESET_DRUMS;
    const int program = p->channels[EventChannel(p, event)].program;
    if (program >= 32 && program <= 39)
        return GRAPPLE_CHIP_PRESET_BASS;
    if (program >= 88 && program <= 95)
        return GRAPPLE_CHIP_PRESET_CHORD;
    return GRAPPLE_CHIP_PRESET_LEAD;
}

static void StartNote(Grapple_ChipPlayer *p, const ChipEvent *event)
{
    ChipSynthVoice *selected = NULL;
    ChipSynthVoice *oldest = &p->voices[0];
    for (int i = 0; i < p->voice_count; ++i)
    {
        ChipSynthVoice *v = &p->voices[i];
        if (!v->active)
        {
            selected = v;
            break;
        }
        if (v->serial < oldest->serial)
            oldest = v;
        if (v->released && (!selected || v->envelope < selected->envelope))
            selected = v;
    }
    if (!selected)
        selected = oldest;
    const Grapple_ChipPreset preset = ResolvePreset(p, event);
    if (preset == GRAPPLE_CHIP_PRESET_DRUMS && (event->a == 42 || event->a == 44))
    {
        for (int i = 0; i < p->voice_count; ++i)
        {
            ChipSynthVoice *v = &p->voices[i];
            if (v->active && v->track == event->track && v->drum == 4)
                Chip_VoiceRelease(v, p->sample_rate);
        }
    }
    const Grapple_ChipExpression *expression =
        event->expression ? &p->song->expressions[event->expression - 1] : NULL;
    bool legato = false;
    if (expression && expression->legato && preset != GRAPPLE_CHIP_PRESET_DRUMS)
        for (int i = 0; i < p->voice_count; ++i)
        {
            ChipSynthVoice *v = &p->voices[i];
            if (v->active && v->track == event->track && v->preset == preset &&
                v->expression.lane == expression->lane && v->start_beat < p->beat &&
                (!legato || v->serial > selected->serial))
            {
                selected = v;
                legato = true;
            }
        }
    const ChipSynthVoice prior = *selected;
    Chip_VoiceStart(selected, preset, event->a, event->b, p->sample_rate);
    if (legato)
    {
        selected->phase = prior.phase;
        selected->mod_phase = prior.mod_phase;
        selected->lfo_phase = prior.lfo_phase;
        selected->envelope = prior.envelope;
        selected->age = prior.age;
        selected->low = prior.low;
        selected->band = prior.band;
    }
    if (expression)
        selected->expression = *expression;
    selected->start_beat = p->beat;
    selected->duration_beats = (double)event->duration / p->song->info.ticks_per_quarter;
    selected->track = event->track;
    selected->channel = EventChannel(p, event);
    selected->note_id = event->note_id;
    selected->serial = p->serial++;
}

static void StopNote(Grapple_ChipPlayer *p, const ChipEvent *event)
{
    ChipSynthVoice *oldest = NULL;
    for (int i = 0; i < p->voice_count; ++i)
    {
        ChipSynthVoice *v = &p->voices[i];
        if (v->active && v->held && v->track == event->track &&
            v->channel == EventChannel(p, event) && v->note == event->a &&
            (!event->note_id || v->note_id == event->note_id) &&
            (!oldest || v->serial < oldest->serial))
            oldest = v;
    }
    if (oldest)
    {
        oldest->held = false;
        if (!oldest->drum && !p->channels[oldest->channel].sustain)
            Chip_VoiceRelease(oldest, p->sample_rate);
    }
}

static void ReleaseSustain(Grapple_ChipPlayer *p, int channel)
{
    for (int i = 0; i < p->voice_count; ++i)
    {
        ChipSynthVoice *v = &p->voices[i];
        if (v->active && !v->held && !v->drum && v->channel == channel)
            Chip_VoiceRelease(v, p->sample_rate);
    }
}

static void ControlChange(Grapple_ChipPlayer *p, int channel_index, int control, int value)
{
    ChipChannel *channel = &p->channels[channel_index];
    switch (control)
    {
    case 1:
        channel->modulation = (float)value / 127.0f;
        break;
    case 7:
        channel->volume = (float)value / 127.0f;
        break;
    case 10: {
        const float angle = (float)value * (SDL_PI_F * 0.5f / 127.0f);
        channel->pan_left = SDL_cosf(angle);
        channel->pan_right = SDL_sinf(angle);
        break;
    }
    case 11:
        channel->expression = (float)value / 127.0f;
        break;
    case 64:
        channel->sustain = value >= 64;
        if (!channel->sustain)
            ReleaseSustain(p, channel_index);
        break;
    case 100:
        channel->rpn_lsb = value;
        break;
    case 101:
        channel->rpn_msb = value;
        break;
    case 98:
    case 99:
        channel->rpn_lsb = channel->rpn_msb = 127;
        break;
    case 6:
    case 38:
        if (channel->rpn_lsb == 0 && channel->rpn_msb == 0)
        {
            if (control == 6)
                channel->bend_semitones = value;
            else
                channel->bend_cents = SDL_min(value, 99);
            UpdateBend(channel);
        }
        break;
    case 121:
        ResetControllers(channel);
        ReleaseSustain(p, channel_index);
        break;
    case 120:
    case 123:
        for (int i = 0; i < p->voice_count; ++i)
        {
            ChipSynthVoice *v = &p->voices[i];
            if (v->active && v->channel == channel_index)
            {
                v->held = false;
                if (control == 120)
                    v->active = false;
                else if (!channel->sustain)
                    Chip_VoiceRelease(v, p->sample_rate);
            }
        }
        break;
    default:
        break;
    }
}

static void DispatchEvent(Grapple_ChipPlayer *p, const ChipEvent *event)
{
    if (event->tempo)
    {
        p->tempo = event->tempo;
        return;
    }
    const int channel = EventChannel(p, event);
    switch (event->status >> 4)
    {
    case 8:
        StopNote(p, event);
        break;
    case 9:
        if (event->b)
            StartNote(p, event);
        else
            StopNote(p, event);
        break;
    case 11:
        ControlChange(p, channel, event->a, event->b);
        break;
    case 12:
        p->channels[channel].program = event->a;
        break;
    case 14:
        p->channels[channel].bend = event->a | (event->b << 7);
        UpdateBend(&p->channels[channel]);
        break;
    default:
        break;
    }
}

int Grapple_RenderChipPlayer(Grapple_ChipPlayer *p, float *stereo, int frames)
{
    if (!p || frames < 0 || (!stereo && frames > 0) ||
        (size_t)frames > SIZE_MAX / (2 * sizeof(float)))
    {
        SDL_SetError("chiptune: invalid render arguments");
        return -1;
    }
    if (frames == 0)
        return 0;
    SDL_memset(stereo, 0, (size_t)frames * 2 * sizeof(float));
    int written = 0;
    while (written < frames)
    {
        if (p->loop && p->frame >= p->end_frame)
            Rewind(p);
        while (p->event < p->song->count &&
               Chip_TimeToFrame(p->song, p->song->events[p->event].time, p->sample_rate) <=
                   p->frame)
            DispatchEvent(p, &p->song->events[p->event++]);
        if (p->frame >= p->end_frame && !p->ended)
        {
            for (int i = 0; i < p->voice_count; ++i)
                if (p->voices[i].active)
                    Chip_VoiceRelease(&p->voices[i], p->sample_rate);
            p->ended = true;
        }
        float buses[GRAPPLE_CHIP_PRESET_DRUMS + 1][2] = {{0}};
        float pulse[GRAPPLE_CHIP_PRESET_DRUMS + 1] = {0};
        for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
        {
            const double spacing = p->effects[bus].settings.pulse_beats;
            pulse[bus] = spacing > 0
                             ? (float)(0.5 + 0.5 * SDL_cos(2.0 * SDL_PI_D *
                                                           SDL_fmod(p->beat / spacing, 1.0)))
                             : 1.0f;
        }
        int active = 0;
        for (int i = 0; i < p->voice_count; ++i)
        {
            ChipSynthVoice *v = &p->voices[i];
            if (!v->active)
                continue;
            ++active;
            const ChipChannel *channel = &p->channels[v->channel];
            const Grapple_ChipEffects *effects = &p->effects[v->preset].settings;
            const float shape = pulse[v->preset] * pulse[v->preset];
            const float gain = 1.0f - effects->pulse_depth * (1.0f - shape);
            const float sample =
                Chip_VoiceSampleMotion(
                    v,
                    channel->pitch *
                        SDL_powf(2, Chip_ExpressionPitch(&v->expression, p->beat - v->start_beat,
                                                         v->duration_beats) /
                                        12),
                    channel->modulation, p->sample_rate, effects->motion, pulse[v->preset]) *
                gain * p->parts[v->track].gain * channel->volume * channel->expression;
            buses[v->preset][0] += sample * channel->pan_left;
            buses[v->preset][1] += sample * channel->pan_right;
        }
        float left = 0, right = 0;
        Uint64 quiet_limit = 0;
        for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
        {
            ChipEffectBus *effects = &p->effects[bus];
            if (buses[bus][0] != 0 || buses[bus][1] != 0)
                p->effect_used[bus] = true;
            if (!p->effect_used[bus])
                continue;
            Chip_EffectsProcess(effects, p->sample_rate, (float)p->tempo / 1000000.0f,
                                &buses[bus][0], &buses[bus][1]);
            left += buses[bus][0];
            right += buses[bus][1];
            if (effects->settings.delay > 0)
                quiet_limit = SDL_max(quiet_limit, (Uint64)effects->delay_frames + 2u);
            if (effects->settings.reverb > 0 || effects->settings.chorus > 0)
                quiet_limit = SDL_max(quiet_limit, (Uint64)p->sample_rate / 10u);
        }
        p->peak_voices = SDL_max(p->peak_voices, active);
        if (active == 0 && SDL_fabsf(left) + SDL_fabsf(right) < 0.00001f)
            ++p->quiet_frames;
        else
            p->quiet_frames = 0;
        /* Wait longer than the longest echo gap before declaring a wet bus drained. */

        if (p->ended && ((active == 0 && p->quiet_frames > quiet_limit) ||
                         p->frame >= p->end_frame + (Uint64)p->sample_rate * 12u))
            break;
        p->beat += 1000000.0 / ((double)p->tempo * p->sample_rate);
        float edge_gain = 1.0f;
        if (p->loop)
        {
            const Uint64 edge = SDL_min(p->frame, p->end_frame - p->frame - 1);
            edge_gain = SDL_min(1.0f, (float)edge / ((float)p->sample_rate * 0.002f));
        }
        stereo[(size_t)written * 2] = (left / SDL_sqrtf(1.0f + left * left)) * edge_gain;
        stereo[(size_t)written * 2 + 1] = (right / SDL_sqrtf(1.0f + right * right)) * edge_gain;
        ++p->frame;
        ++written;
    }
    return written;
}

static void SDLCALL StreamAudio(void *userdata, SDL_AudioStream *stream, int additional, int total)
{
    (void)total;
    Grapple_ChipPlayer *p = userdata;
    float pcm[512 * 2];
    while (additional > 0)
    {
        const int frames = SDL_min(512, additional / (int)(2 * sizeof(float)) +
                                            (additional % (int)(2 * sizeof(float)) != 0));
        const int got = Grapple_RenderChipPlayer(p, pcm, frames);
        if (got > 0 && !SDL_PutAudioStreamData(stream, pcm, got * (int)(2 * sizeof(float))))
            return;
        if (got < frames)
        {
            if (!p->flushed)
            {
                SDL_FlushAudioStream(stream);
                p->flushed = true;
            }
            return;
        }
        additional -= got * (int)(2 * sizeof(float));
    }
}

Grapple_ChipPlayer *Grapple_CreateChipPlayer(const Grapple_ChipSong *song, int sample_rate,
                                             int voices, bool loop)
{
    if (!song || sample_rate < 8000 || sample_rate > 192000 || voices < 1 || voices > 1024 ||
        (loop && Chip_TimeToFrame(song, song->end_time, sample_rate) == 0))
    {
        SDL_SetError("chiptune: invalid player configuration or empty loop");
        return NULL;
    }
    Grapple_ChipPlayer *p = SDL_calloc(1, sizeof(*p));
    if (!p)
        return NULL;
    p->song = song;
    Chip_RetainSong(song);
    p->sample_rate = sample_rate;
    p->voice_count = voices;
    p->loop = loop;
    p->end_frame = Chip_TimeToFrame(song, song->end_time, sample_rate);
    p->voices = SDL_calloc((size_t)voices, sizeof(*p->voices));
    p->parts = SDL_calloc((size_t)song->info.track_count, sizeof(*p->parts));
    p->channel_count = song->independent_parts ? song->info.track_count * 16 : 16;
    p->channels = SDL_calloc((size_t)p->channel_count, sizeof(*p->channels));
    const SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, sample_rate};
    p->stream = SDL_CreateAudioStream(&spec, &spec);
    if (!p->voices || !p->parts || !p->stream || !p->channels)
        goto fail;
    for (int i = 0; i < song->info.track_count; ++i)
    {
        p->parts[i].gain = song->gains[i];
        p->parts[i].preset = song->presets[i];
        p->parts[i].named_preset = PresetFromName(song->tracks[i].name);
    }
    for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
        if (!Chip_EffectsInit(&p->effects[bus], (Grapple_ChipPreset)bus, sample_rate))
            goto fail;
    Rewind(p);
    if (!SDL_SetAudioStreamGetCallback(p->stream, StreamAudio, p))
        goto fail;
    return p;
fail:
    Grapple_DestroyChipPlayer(p);
    return NULL;
}

void Grapple_DestroyChipPlayer(Grapple_ChipPlayer *p)
{
    if (p)
    {
        MIX_DestroyMixer(p->managed_mixer);
        SDL_DestroyAudioStream(p->stream);
        for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
            Chip_EffectsDestroy(&p->effects[bus]);
        Grapple_DestroyChipSong((Grapple_ChipSong *)p->song);
        SDL_free(p->parts);
        SDL_free(p->channels);
        SDL_free(p->voices);
        SDL_free(p);
    }
}

SDL_AudioStream *Grapple_GetChipPlayerStream(Grapple_ChipPlayer *p)
{
    return p ? p->stream : NULL;
}

bool Grapple_SetChipTrackPreset(Grapple_ChipPlayer *p, int track, Grapple_ChipPreset preset,
                                float gain)
{
    if (!p || track < 0 || track >= p->song->info.track_count ||
        preset < GRAPPLE_CHIP_PRESET_AUTO || preset > GRAPPLE_CHIP_PRESET_DRUMS ||
        !(gain >= 0.0f && gain <= 2.0f))
        return SDL_SetError("chiptune: invalid track preset or gain");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    p->parts[track].preset = preset;
    p->parts[track].gain = gain;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

void Grapple_ResetChipPlayer(Grapple_ChipPlayer *p)
{
    if (p && SDL_LockAudioStream(p->stream))
    {
        SDL_ClearAudioStream(p->stream);
        Rewind(p);
        for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
            Chip_EffectsClear(&p->effects[bus]);
        SDL_zeroa(p->effect_used);
        p->peak_voices = 0;
        SDL_UnlockAudioStream(p->stream);
    }
}

int Grapple_GetChipPlayerPeakVoices(Grapple_ChipPlayer *p)
{
    int peak = 0;
    if (p && SDL_LockAudioStream(p->stream))
    {
        peak = p->peak_voices;
        SDL_UnlockAudioStream(p->stream);
    }
    return peak;
}

bool Grapple_SetChipPresetEffects(Grapple_ChipPlayer *p, Grapple_ChipPreset preset,
                                  const Grapple_ChipEffects *effects)
{
    if (!p || preset < GRAPPLE_CHIP_PRESET_LEAD || preset > GRAPPLE_CHIP_PRESET_DRUMS ||
        !Chip_EffectsValid(effects))
        return SDL_SetError("chiptune: invalid effect settings");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    p->effects[preset].settings = *effects;
    Chip_EffectsClear(&p->effects[preset]);
    p->effect_used[preset] = false;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_PlayChipPlayer(Grapple_ChipPlayer *p)
{
    if (!p)
        return SDL_SetError("chiptune: NULL player");
    if (!p->managed_mixer)
    {
        const SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, p->sample_rate};
        p->managed_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
        if (!p->managed_mixer)
            return false;
        p->managed_track = MIX_CreateTrack(p->managed_mixer);
        if (!p->managed_track || !MIX_SetTrackAudioStream(p->managed_track, p->stream))
        {
            MIX_DestroyMixer(p->managed_mixer);
            p->managed_mixer = NULL;
            p->managed_track = NULL;
            return false;
        }
    }
    if (MIX_TrackPaused(p->managed_track))
        return MIX_ResumeTrack(p->managed_track);
    if (MIX_TrackPlaying(p->managed_track))
        return true;
    Grapple_ResetChipPlayer(p);
    return MIX_PlayTrack(p->managed_track, 0);
}

void Grapple_PauseChipPlayer(Grapple_ChipPlayer *p)
{
    if (p && p->managed_track)
        MIX_PauseTrack(p->managed_track);
}

void Grapple_StopChipPlayer(Grapple_ChipPlayer *p)
{
    if (p)
    {
        MIX_DestroyMixer(p->managed_mixer);
        p->managed_mixer = NULL;
        p->managed_track = NULL;
        Grapple_ResetChipPlayer(p);
    }
}

bool Grapple_ChipPlayerPlaying(Grapple_ChipPlayer *p)
{
    return p && p->managed_track && MIX_TrackPlaying(p->managed_track);
}
