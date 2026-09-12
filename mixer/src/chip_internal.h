/* Original Grapple code (zlib). Shared song representation, independent of the synth. */
#ifndef GRAPPLE_CHIP_INTERNAL_H
#define GRAPPLE_CHIP_INTERNAL_H

#include <grapple/chiptune.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CHIP_SONG_MAX_TRACKS 256
#define CHIP_SONG_MAX_EVENTS 1000000
#define CHIP_SONG_MAX_SECONDS 86400

    typedef struct ChipEvent
    {
        Uint64 tick;
        Uint64 time; /* Exact elapsed microseconds multiplied by song PPQN. */
        Uint32 order;
        Uint32 tempo;
        Uint16 track;
        Uint8 status;
        Uint8 a;
        Uint8 b;
        Uint32 note_id; /* Zero preserves MIDI's oldest matching note-off semantics. */
    } ChipEvent;

    struct Grapple_ChipSong
    {
        SDL_AtomicInt references;
        Grapple_ChipSongInfo info;
        Grapple_ChipPreset *presets;
        float *gains;
        Grapple_ChipTrackInfo *tracks;
        ChipEvent *events;
        size_t count;
        size_t capacity;
        Uint64 end_time;
        bool independent_parts;
    };

    Grapple_ChipSong *Chip_NewSong(int tracks, int ppqn);
    bool Chip_AppendEvent(Grapple_ChipSong *song, ChipEvent event);
    bool Chip_ResolveTiming(Grapple_ChipSong *song);
    void Chip_RetainSong(const Grapple_ChipSong *song);
    Grapple_ChipSong *Chip_ParseMusicXml(const void *data, size_t size);
    Uint64 Chip_TimeToFrame(const Grapple_ChipSong *song, Uint64 time, int sample_rate);

#ifdef __cplusplus
}
#endif

#endif
