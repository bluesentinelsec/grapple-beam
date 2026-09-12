/* Original Grapple code (zlib). Prepared loop checkpoints and control-thread seeking. */
#include "chip_player.h"

typedef struct ChipCheckpoint
{
    ChipSynthVoice *voices;
    ChipChannel *channels;
    size_t event;
    Uint64 serial, tempo_tick;
    Uint32 tempo;
    long double tempo_frame;
} ChipCheckpoint;

long double Chip_FrameAtTime(const Grapple_ChipPlayer *p, Uint64 time)
{
    return (long double)time * p->sample_rate /
           ((long double)p->song->info.ticks_per_quarter * 1000000);
}

static Uint64 TimeAtTick(const Grapple_ChipSong *song, Uint64 tick)
{
    Uint64 prior_tick = 0, time = 0;
    Uint32 tempo = 500000;
    for (size_t i = 0; i < song->count && song->events[i].tick <= tick; ++i)
        if (song->events[i].tempo)
        {
            prior_tick = song->events[i].tick;
            time = song->events[i].time;
            tempo = song->events[i].tempo;
        }
    return time + (tick - prior_tick) * tempo;
}

void Chip_UpdateBeat(Grapple_ChipPlayer *p)
{
    p->beat = (double)p->tempo_tick / p->song->info.ticks_per_quarter +
              (double)(p->clock - p->tempo_frame) * 1000000.0 / ((double)p->tempo * p->sample_rate);
    p->beat = SDL_max(0, p->beat);
}

static void AdvanceVoices(Grapple_ChipPlayer *p, long double frames)
{
    if (frames <= 0)
        return;
    const double seconds = (double)frames / p->sample_rate;
    for (int i = 0; i < p->voice_count; ++i)
    {
        ChipSynthVoice *v = &p->voices[i];
        if (!v->active)
            continue;
        if (v->released)
            v->envelope = SDL_max(0, v->envelope - v->release_step * (float)frames);
        else
        {
            const double attack_left =
                SDL_max(0, 1.0 / v->attack_step - (double)v->age * p->sample_rate);
            const double attack = SDL_min((double)frames, attack_left);
            v->envelope = SDL_min(1, v->envelope + (float)attack * v->attack_step);
            if ((double)frames > attack)
                v->envelope = SDL_max(v->sustain, v->envelope - (float)((double)frames - attack) *
                                                                    v->decay_step);
        }
        v->age += (float)seconds;
        if (v->envelope <= 0)
        {
            v->active = false;
            continue;
        }
        const double cycles = seconds * v->frequency * p->channels[v->channel].pitch;
        v->phase = SDL_fmod(v->phase + cycles, 1);
        v->mod_phase = SDL_fmod(v->mod_phase + cycles * 1.5, 1);
        v->lfo_phase = SDL_fmod(v->lfo_phase + seconds * 5, 1);
        v->sweep *= SDL_powf(v->sweep_decay, (float)frames);
    }
}

/* Restore musical/envelope state without synthesizing the entire preceding score.
   Historical filter/noise/reverb state is deliberately replaced by a short fade-in. */
static void Reconstruct(Grapple_ChipPlayer *p, Uint64 tick)
{
    Chip_PlayerRewind(p);
    const long double target = Chip_FrameAtTime(p, TimeAtTick(p->song, tick));
    while (p->event < p->song->count && p->song->events[p->event].tick < tick)
    {
        const ChipEvent *event = &p->song->events[p->event++];
        const long double frame = Chip_FrameAtTime(p, event->time);
        AdvanceVoices(p, (frame - p->clock) / p->speed);
        p->clock = frame;
        Chip_UpdateBeat(p);
        Chip_DispatchEvent(p, event);
    }
    AdvanceVoices(p, (target - p->clock) / p->speed);
    p->clock = target;
    p->frame = (Uint64)target;
    Chip_UpdateBeat(p);
}

void Chip_DestroyCheckpoint(ChipCheckpoint *checkpoint)
{
    if (checkpoint)
    {
        SDL_free(checkpoint->voices);
        SDL_free(checkpoint->channels);
        SDL_free(checkpoint);
    }
}

static ChipCheckpoint *MakeCheckpoint(const Grapple_ChipPlayer *p, Uint64 tick)
{
    ChipCheckpoint *checkpoint = SDL_calloc(1, sizeof(*checkpoint));
    if (!checkpoint)
        return NULL;
    checkpoint->voices = SDL_calloc((size_t)p->voice_count, sizeof(*p->voices));
    checkpoint->channels = SDL_calloc((size_t)p->channel_count, sizeof(*p->channels));
    if (!checkpoint->voices || !checkpoint->channels)
    {
        Chip_DestroyCheckpoint(checkpoint);
        return NULL;
    }
    Grapple_ChipPlayer scratch = *p;
    scratch.voices = checkpoint->voices;
    scratch.channels = checkpoint->channels;
    Reconstruct(&scratch, tick);
    checkpoint->event = scratch.event;
    checkpoint->serial = scratch.serial;
    checkpoint->tempo = scratch.tempo;
    checkpoint->tempo_tick = scratch.tempo_tick;
    checkpoint->tempo_frame = scratch.tempo_frame;
    return checkpoint;
}

bool Chip_RefreshCheckpoint(Grapple_ChipPlayer *p)
{
    if (!p->loop)
        return true;
    ChipCheckpoint *checkpoint = MakeCheckpoint(p, p->loop_start_tick);
    if (!checkpoint)
        return false;
    Chip_DestroyCheckpoint(p->checkpoint);
    p->checkpoint = checkpoint;
    return true;
}

void Chip_ClearPlayerEffects(Grapple_ChipPlayer *p)
{
    for (int bus = GRAPPLE_CHIP_PRESET_LEAD; bus <= GRAPPLE_CHIP_PRESET_DRUMS; ++bus)
        Chip_EffectsClear(&p->effects[bus]);
    SDL_zeroa(p->effect_used);
    for (int i = 0; i < p->song->info.track_count; ++i)
    {
        if (p->parts[i].effects)
            Chip_EffectsClear(p->parts[i].effects);
        p->parts[i].effect_used = false;
    }
}

void Chip_LoopBoundary(Grapple_ChipPlayer *p)
{
    const ChipCheckpoint *c = p->checkpoint;
    const long double overflow = p->clock - p->loop_end_frame;
    SDL_memcpy(p->voices, c->voices, (size_t)p->voice_count * sizeof(*p->voices));
    SDL_memcpy(p->channels, c->channels, (size_t)p->channel_count * sizeof(*p->channels));
    p->event = c->event;
    p->serial = c->serial;
    p->tempo = c->tempo;
    p->tempo_tick = c->tempo_tick;
    p->tempo_frame = c->tempo_frame;
    p->clock = p->loop_start_frame + overflow;
    p->frame = (Uint64)p->clock;
    p->ended = false;
    p->quiet_frames = 0;
    p->tail_frames = 0;
    p->fade_frames = (Uint64)((double)p->sample_rate * 0.002);
    ++p->loop_count;
    Chip_UpdateBeat(p);
}

bool Grapple_SetChipPlayerLoop(Grapple_ChipPlayer *p, Uint64 start_tick, Uint64 end_tick,
                               bool enabled)
{
    if (!p || (enabled && (start_tick >= end_tick || end_tick > p->song->info.duration_ticks)))
        return SDL_SetError("chiptune: invalid loop interval");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    const long double start = enabled ? Chip_FrameAtTime(p, TimeAtTick(p->song, start_tick)) : 0;
    const long double end = enabled ? Chip_FrameAtTime(p, TimeAtTick(p->song, end_tick)) : 0;
    if (enabled && end - start < 4)
    {
        SDL_UnlockAudioStream(p->stream);
        return SDL_SetError("chiptune: loop must span at least four native sample frames");
    }
    ChipCheckpoint *checkpoint = enabled ? MakeCheckpoint(p, start_tick) : NULL;
    if (enabled && !checkpoint)
    {
        SDL_UnlockAudioStream(p->stream);
        return false;
    }
    ChipCheckpoint *old = p->checkpoint;
    p->checkpoint = checkpoint;
    p->loop = enabled;
    p->loop_start_tick = start_tick;
    p->loop_end_tick = end_tick;
    p->loop_start_frame = start;
    p->loop_end_frame = end;
    p->loop_count = 0;
    p->fade_frames = enabled ? (Uint64)((double)p->sample_rate * 0.002) : 0;
    if (enabled && p->clock >= end)
    {
        Reconstruct(p, start_tick);
        SDL_ClearAudioStream(p->stream);
        Chip_ClearPlayerEffects(p);
    }
    SDL_UnlockAudioStream(p->stream);
    Chip_DestroyCheckpoint(old);
    return true;
}

bool Grapple_SeekChipPlayer(Grapple_ChipPlayer *p, Uint64 tick)
{
    if (!p || tick > p->song->info.duration_ticks)
        return SDL_SetError("chiptune: seek is outside the song");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    if (p->loop && tick >= p->loop_end_tick)
    {
        SDL_UnlockAudioStream(p->stream);
        return SDL_SetError("chiptune: seek must precede the active loop end");
    }
    Reconstruct(p, tick);
    SDL_ClearAudioStream(p->stream);
    Chip_ClearPlayerEffects(p);
    p->fade_frames = (Uint64)((double)p->sample_rate * 0.002);
    p->loop_count = 0;
    p->positioned = true;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_SetChipPlayerTempo(Grapple_ChipPlayer *p, double scale)
{
    if (!p || !(scale >= 0.25 && scale <= 4))
        return SDL_SetError("chiptune: tempo scale must be 0.25 through 4");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    const double prior = p->speed;
    p->speed = scale;
    ChipCheckpoint *checkpoint = p->loop ? MakeCheckpoint(p, p->loop_start_tick) : NULL;
    if (p->loop && !checkpoint)
    {
        p->speed = prior;
        SDL_UnlockAudioStream(p->stream);
        return false;
    }
    ChipCheckpoint *old = p->checkpoint;
    p->checkpoint = checkpoint;
    SDL_UnlockAudioStream(p->stream);
    Chip_DestroyCheckpoint(old);
    return true;
}

bool Grapple_ReadChipPlayerPosition(Grapple_ChipPlayer *p, Grapple_ChipPosition *position)
{
    if (!p || !position)
        return SDL_SetError("chiptune: invalid position output");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    SDL_zero(*position);
    position->seconds = SDL_min((double)p->clock / p->sample_rate, p->song->info.duration_seconds);
    position->beat =
        SDL_min(p->beat, (double)p->song->info.duration_ticks / p->song->info.ticks_per_quarter);
    position->tick = (Uint64)(position->beat * p->song->info.ticks_per_quarter);
    position->loop_count = p->loop_count;
    position->source_measure = position->measure_visit = -1;
    int low = 0, high = p->song->measure_count;
    while (low < high)
    {
        const int mid = low + (high - low) / 2;
        if (p->song->measures[mid].start <= position->tick)
            low = mid + 1;
        else
            high = mid;
    }
    if (low > 0)
    {
        const ChipMeasurePosition *measure = &p->song->measures[low - 1];
        position->source_measure = measure->source;
        position->measure_visit = low - 1;
        position->measure_beat =
            position->beat - (double)measure->start / p->song->info.ticks_per_quarter;
    }
    SDL_UnlockAudioStream(p->stream);
    return true;
}
