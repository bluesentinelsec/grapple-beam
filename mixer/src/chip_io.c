/* Original Grapple code (zlib). Library playback and bounded offline rendering helpers. */
#include "chip_internal.h"

Grapple_ChipSong *Grapple_LoadChipSongMemory(const void *data, size_t size,
                                             const Grapple_ChipImportOptions *options,
                                             Grapple_ChipDiagnostic *error)
{
    if (!data || !size || size > 64u * 1024u * 1024u)
    {
        SDL_SetError("chiptune: invalid or oversized input buffer");
        if (error)
            *error = (Grapple_ChipDiagnostic){
                GRAPPLE_CHIP_DIAGNOSTIC_INPUT, GRAPPLE_CHIP_DIAGNOSTIC_ERROR, -1, -1, 0, 0, "", ""};
        return NULL;
    }
    return Grapple_LoadChipSong_IOEx(SDL_IOFromConstMem(data, size), true, options, error);
}

Grapple_ChipPlayer *Grapple_PlayChipSong(const Grapple_ChipSong *song, bool loop)
{
    Grapple_ChipPlayer *player = Grapple_CreateChipPlayer(song, 48000, 64, loop);
    if (player && !Grapple_PlayChipPlayer(player))
    {
        Grapple_DestroyChipPlayer(player);
        player = NULL;
    }
    return player;
}

Grapple_ChipPlayer *Grapple_PlayChipFile(const char *path, bool loop)
{
    Grapple_ChipSong *song = Grapple_LoadChipSong(path);
    if (!song)
        return NULL;
    Grapple_ChipPlayer *player = Grapple_PlayChipSong(song, loop);
    Grapple_DestroyChipSong(song);
    return player;
}

static bool WaveHeader(SDL_IOStream *io, Uint32 bytes, int sample_rate)
{
    return SDL_SeekIO(io, 0, SDL_IO_SEEK_SET) == 0 && SDL_WriteIO(io, "RIFF", 4) == 4 &&
           SDL_WriteU32LE(io, bytes + 36u) && SDL_WriteIO(io, "WAVEfmt ", 8) == 8 &&
           SDL_WriteU32LE(io, 16) && SDL_WriteU16LE(io, 1) && SDL_WriteU16LE(io, 2) &&
           SDL_WriteU32LE(io, (Uint32)sample_rate) && SDL_WriteU32LE(io, (Uint32)sample_rate * 4) &&
           SDL_WriteU16LE(io, 4) && SDL_WriteU16LE(io, 16) && SDL_WriteIO(io, "data", 4) == 4 &&
           SDL_WriteU32LE(io, bytes);
}

bool Grapple_SaveChipSongWav_IO(const Grapple_ChipSong *song, SDL_IOStream *io, bool closeio,
                                int sample_rate, int voices)
{
    if (!io)
        return SDL_SetError("chiptune: NULL WAV output stream");
    Grapple_ChipPlayer *player = Grapple_CreateChipPlayer(song, sample_rate, voices, false);
    bool ok = player != NULL;
    if (ok && (song->info.duration_seconds + 12) * sample_rate * 4 > SDL_MAX_UINT32 - 36u)
        ok = SDL_SetError("chiptune: song and tails exceed RIFF output limit");
    if (ok)
        ok = WaveHeader(io, 0, sample_rate);
    Uint32 bytes = 0;
    float pcm[1024 * 2];
    Uint16 output[1024 * 2];
    while (ok)
    {
        const int frames = Grapple_RenderChipPlayer(player, pcm, 1024);
        if (frames <= 0)
        {
            ok = frames == 0;
            break;
        }
        const Uint32 count = (Uint32)frames * 4;
        if (bytes > SDL_MAX_UINT32 - 36u - count)
        {
            ok = SDL_SetError("chiptune: WAV exceeds RIFF output limit");
            break;
        }
        for (int i = 0; i < frames * 2; ++i)
            output[i] = SDL_Swap16LE((Uint16)(Sint16)(SDL_clamp(pcm[i], -1.0f, 1.0f) * 32767.0f));
        ok = SDL_WriteIO(io, output, count) == count;
        bytes += count;
    }
    if (ok)
        ok = WaveHeader(io, bytes, sample_rate) &&
             SDL_SeekIO(io, (Sint64)bytes + 44, SDL_IO_SEEK_SET) >= 0;
    Grapple_DestroyChipPlayer(player);
    if (closeio && !SDL_CloseIO(io))
        ok = false;
    return ok;
}

bool Grapple_SaveChipSongWav(const Grapple_ChipSong *song, const char *path, int sample_rate,
                             int voices)
{
    if (!song || !path || !*path || sample_rate < 8000 || sample_rate > 192000 || voices < 1 ||
        voices > 1024)
        return SDL_SetError("chiptune: invalid WAV output arguments");
    SDL_IOStream *io = SDL_IOFromFile(path, "wb");
    return io && Grapple_SaveChipSongWav_IO(song, io, true, sample_rate, voices);
}
