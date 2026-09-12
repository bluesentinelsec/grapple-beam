/* Original Grapple code (zlib). Preview implementation; startup lives in main.cpp. */
#include "preview.h"

#include <grapple/chiptune.h>

static bool WriteWaveHeader(SDL_IOStream *io, Uint32 bytes)
{
    return SDL_SeekIO(io, 0, SDL_IO_SEEK_SET) == 0 && SDL_WriteIO(io, "RIFF", 4) == 4 &&
           SDL_WriteU32LE(io, bytes + 36u) && SDL_WriteIO(io, "WAVEfmt ", 8) == 8 &&
           SDL_WriteU32LE(io, 16) && SDL_WriteU16LE(io, 1) && SDL_WriteU16LE(io, 2) &&
           SDL_WriteU32LE(io, 48000) && SDL_WriteU32LE(io, 192000) && SDL_WriteU16LE(io, 4) &&
           SDL_WriteU16LE(io, 16) && SDL_WriteIO(io, "data", 4) == 4 && SDL_WriteU32LE(io, bytes);
}

static bool RenderWave(Grapple_ChipPlayer *player, const char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "wb");
    if (!io)
        return false;
    bool ok = WriteWaveHeader(io, 0);
    Uint32 bytes = 0;
    float pcm[1024 * 2];
    Uint16 output[1024 * 2];
    const Uint64 start = SDL_GetTicksNS();
    while (ok)
    {
        const int frames = Grapple_RenderChipPlayer(player, pcm, 1024);
        if (frames < 0)
        {
            ok = false;
            break;
        }
        if (frames == 0)
            break;
        const Uint32 count = (Uint32)frames * 4u;
        if (bytes > SDL_MAX_UINT32 - 36u - count)
        {
            ok = SDL_SetError("chiptune preview: WAV exceeds RIFF size limit");
            break;
        }
        for (int i = 0; i < frames * 2; ++i)
            output[i] = SDL_Swap16LE((Uint16)(Sint16)(SDL_clamp(pcm[i], -1.0f, 1.0f) * 32767.0f));
        ok = SDL_WriteIO(io, output, count) == count;
        bytes += count;
    }
    if (ok)
        ok = WriteWaveHeader(io, bytes);
    if (!SDL_CloseIO(io))
        ok = false;
    if (ok)
        SDL_Log("Rendered %.3f seconds in %.3f seconds; peak %d voices: %s",
                (double)bytes / 192000.0, (double)(SDL_GetTicksNS() - start) / 1e9,
                Grapple_GetChipPlayerPeakVoices(player), path);
    return ok;
}

static bool PlayStream(Grapple_ChipPlayer *player)
{
    const SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, 48000};
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (!mixer)
        return false;
    MIX_Track *track = MIX_CreateTrack(mixer);
    const bool ok = track && MIX_SetTrackAudioStream(track, Grapple_GetChipPlayerStream(player)) &&
                    MIX_PlayTrack(track, 0);
    if (ok)
    {
        while (MIX_TrackPlaying(track))
            SDL_Delay(20);
        SDL_Log("Playback complete; peak %d simultaneous voices",
                Grapple_GetChipPlayerPeakVoices(player));
    }
    MIX_DestroyMixer(mixer);
    return ok;
}

int ChipPreview_Run(const char *midi, const char *wav, int ring_track, int solo_track, int voices,
                    bool loop, double pulse_beats, float pulse_depth, bool dry)
{
    const bool render = wav[0] != '\0';
    if (render && loop)
    {
        SDL_Log("--chip-loop cannot be combined with --chip-wav");
        return 1;
    }
    if (!SDL_Init(render ? 0 : SDL_INIT_AUDIO))
    {
        SDL_Log("%s", SDL_GetError());
        return 1;
    }
    Grapple_ChipSong *song = Grapple_LoadChipSong(midi);
    Grapple_ChipPlayer *player = NULL;
    bool ok = song != NULL;
    if (ok)
    {
        const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(song);
        SDL_Log("MIDI: %d tracks, %d PPQN, %.3f seconds (%llu ticks)", info->track_count,
                info->ticks_per_quarter, info->duration_seconds,
                (unsigned long long)info->duration_ticks);
        if (ring_track < -1 || ring_track >= info->track_count || solo_track < -1 ||
            solo_track >= info->track_count)
            ok = SDL_SetError("chiptune preview: selected track is outside the MIDI file");
        if (ok)
            player = Grapple_CreateChipPlayer(song, 48000, voices, loop);
        ok = ok && player != NULL;
        for (int i = 0; ok && i < info->track_count; ++i)
        {
            const Grapple_ChipTrackInfo *track = Grapple_GetChipTrackInfo(song, i);
            const Grapple_ChipPreset preset =
                i == ring_track ? GRAPPLE_CHIP_PRESET_RING : GRAPPLE_CHIP_PRESET_AUTO;
            const float gain = solo_track < 0 || solo_track == i ? 1.0f : 0.0f;
            SDL_Log("Track %d: %s | %u notes | channels 0x%04x | %s", i, track->name,
                    (unsigned)track->note_count, (unsigned)track->channels,
                    i == ring_track ? "wet pulsing harmony" : "automatic preset");
            ok = Grapple_SetChipTrackPreset(player, i, preset, gain);
        }
    }
    for (int preset = GRAPPLE_CHIP_PRESET_LEAD; ok && preset <= GRAPPLE_CHIP_PRESET_DRUMS; ++preset)
    {
        Grapple_ChipEffects effects = {0};
        if (!dry)
            ok = Grapple_GetChipPresetEffects((Grapple_ChipPreset)preset, &effects);
        if (!dry && (preset == GRAPPLE_CHIP_PRESET_RING || preset == GRAPPLE_CHIP_PRESET_CHORD))
        {
            effects.pulse_beats = pulse_beats;
            effects.pulse_depth = pulse_depth;
        }
        ok = ok && Grapple_SetChipPresetEffects(player, (Grapple_ChipPreset)preset, &effects);
    }
    if (ok)
    {
        if (render)
            ok = RenderWave(player, wav);
        else if (MIX_Init())
            ok = PlayStream(player);
        else
            ok = false;
    }
    if (!ok)
        SDL_Log("Chiptune preview failed: %s", SDL_GetError());
    Grapple_DestroyChipPlayer(player);
    Grapple_DestroyChipSong(song);
    if (!render)
        MIX_Quit();
    SDL_Quit();
    return ok ? 0 : 1;
}
