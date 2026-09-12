/* Original Grapple code (zlib). Private player state and control-thread transport. */
#ifndef GRAPPLE_CHIP_PLAYER_H
#define GRAPPLE_CHIP_PLAYER_H
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
    bool percussion;
    bool sostenuto;
    float soft;
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
    float pan;
    bool muted, solo;
    ChipEffectBus *effects;
    bool effect_used;
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
    Uint64 tail_frames;
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
    long double clock, clock_error;
    long double tempo_frame;
    Uint64 tempo_tick;
    double speed;
    float master_gain;
    Uint64 loop_start_tick, loop_end_tick, loop_count;
    long double loop_start_frame, loop_end_frame;
    struct ChipCheckpoint *checkpoint;
    Uint64 fade_frames;
    bool positioned;
};

void Chip_PlayerRewind(Grapple_ChipPlayer *player);
void Chip_DispatchEvent(Grapple_ChipPlayer *player, const ChipEvent *event);
Grapple_ChipPreset Chip_ResolvePreset(const Grapple_ChipPlayer *player, const ChipEvent *event);
long double Chip_FrameAtTime(const Grapple_ChipPlayer *player, Uint64 time);
void Chip_UpdateBeat(Grapple_ChipPlayer *player);
void Chip_LoopBoundary(Grapple_ChipPlayer *player);
void Chip_DestroyCheckpoint(struct ChipCheckpoint *checkpoint);
bool Chip_RefreshCheckpoint(Grapple_ChipPlayer *player);
void Chip_ClearPlayerEffects(Grapple_ChipPlayer *player);
#endif
