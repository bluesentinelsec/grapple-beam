/* Original Grapple code (zlib). Bounded Standard MIDI File reader. */
#include "chip_internal.h"

#define CHIP_MIDI_MAX_BYTES (64u * 1024u * 1024u)

typedef struct ChipReader
{
    const Uint8 *data;
    size_t size;
    size_t pos;
} ChipReader;

static bool ReadBytes(ChipReader *r, size_t n, const Uint8 **out)
{
    if (n > r->size - r->pos)
        return SDL_SetError("chiptune MIDI: truncated chunk at byte %zu", r->pos);
    *out = r->data + r->pos;
    r->pos += n;
    return true;
}

static Uint32 BigEndian(const Uint8 *p, int n)
{
    Uint32 value = 0;
    for (int i = 0; i < n; ++i)
        value = (value << 8) | p[i];
    return value;
}

static bool ReadVariable(ChipReader *r, Uint32 *value)
{
    *value = 0;
    for (int i = 0; i < 4; ++i)
    {
        const Uint8 *p;
        if (!ReadBytes(r, 1, &p))
            return false;
        *value = (*value << 7) | (*p & 127u);
        if ((*p & 128u) == 0)
            return true;
    }
    return SDL_SetError("chiptune MIDI: variable-length integer exceeds four bytes");
}

static bool ReadTrack(Grapple_ChipSong *song, ChipReader *r, int track)
{
    Grapple_ChipTrackInfo *info = &song->tracks[track];
    Uint64 tick = 0;
    Uint8 running = 0;
    while (r->pos < r->size)
    {
        Uint32 delta;
        const Uint8 *p;
        if (!ReadVariable(r, &delta) || !ReadBytes(r, 1, &p))
            return false;
        if (tick > SDL_MAX_UINT64 - delta)
            return SDL_SetError("chiptune MIDI: tick overflow");
        tick += delta;
        ChipEvent event = {0};
        event.tick = tick;
        event.track = (Uint16)track;
        Uint8 status = *p;
        if (status < 128)
        {
            if (!running)
                return SDL_SetError("chiptune MIDI: running status without a channel event");
            status = running;
            --r->pos;
        }
        if (status == 255 || status == 240 || status == 247)
        {
            running = 0;
            Uint8 meta = 0;
            Uint32 size;
            if (status == 255)
            {
                if (!ReadBytes(r, 1, &p))
                    return false;
                meta = *p;
            }
            if (!ReadVariable(r, &size) || !ReadBytes(r, size, &p))
                return false;
            if (status != 255)
                continue;
            if (meta == 3)
            {
                const size_t length = SDL_min((size_t)size, sizeof(info->name) - 1);
                SDL_memcpy(info->name, p, length);
                info->name[length] = '\0';
            }
            else if (meta == 81)
            {
                if (size != 3 || BigEndian(p, 3) == 0)
                    return SDL_SetError("chiptune MIDI: invalid tempo");
                event.tempo = BigEndian(p, 3);
                if (!Chip_AppendEvent(song, event))
                    return false;
            }
            else if (meta == 33 && (size != 1 || *p != 0))
                return SDL_SetError("chiptune MIDI: multiple MIDI ports are not supported");
            else if (meta == 47)
            {
                if (size != 0)
                    return SDL_SetError("chiptune MIDI: invalid end-of-track");
                song->info.duration_ticks = SDL_max(song->info.duration_ticks, tick);
                return true;
            }
            continue;
        }
        if (status >= 240)
            return SDL_SetError("chiptune MIDI: unsupported system event 0x%02x", status);
        running = status;
        const int kind = status >> 4;
        const size_t length = (kind == 12 || kind == 13) ? 1 : 2;
        if (!ReadBytes(r, length, &p))
            return false;
        if (p[0] >= 128 || (length == 2 && p[1] >= 128))
            return SDL_SetError("chiptune MIDI: channel data has status bit set");
        event.status = status;
        event.a = p[0];
        event.b = length == 2 ? p[1] : 0;
        info->channels |= (Uint16)(1u << (status & 15));
        if (kind == 9 && event.b)
            ++info->note_count;
        if (kind == 12 && info->first_program < 0)
            info->first_program = event.a;
        if (!Chip_AppendEvent(song, event))
            return false;
    }
    return SDL_SetError("chiptune MIDI: missing end-of-track");
}

static Grapple_ChipSong *ParseMidi(const Uint8 *data, size_t size)
{
    ChipReader reader = {data, size, 0};
    const Uint8 *p;
    if (!ReadBytes(&reader, 8, &p))
        return NULL;
    const Uint32 header_size = BigEndian(p + 4, 4);
    if (SDL_memcmp(p, "MThd", 4) != 0 || header_size < 6)
    {
        SDL_SetError("chiptune MIDI: invalid MThd header");
        return NULL;
    }
    if (!ReadBytes(&reader, header_size, &p))
        return NULL;
    const Uint32 format = BigEndian(p, 2);
    const Uint32 tracks = BigEndian(p + 2, 2);
    const Uint32 ppqn = BigEndian(p + 4, 2);
    if (format > 1 || tracks == 0 || tracks > CHIP_SONG_MAX_TRACKS || (format == 0 && tracks != 1))
    {
        SDL_SetError("chiptune MIDI: expected type 0/1 and 1..256 tracks");
        return NULL;
    }
    if (ppqn == 0 || (ppqn & 32768u))
    {
        SDL_SetError("chiptune MIDI: expected PPQN timing; SMPTE division is unsupported");
        return NULL;
    }
    Grapple_ChipSong *song = Chip_NewSong((int)tracks, (int)ppqn);
    if (!song)
        return NULL;
    for (int track = 0; track < (int)tracks;)
    {
        if (!ReadBytes(&reader, 8, &p))
            goto fail;
        const bool is_track = SDL_memcmp(p, "MTrk", 4) == 0;
        const Uint32 chunk_size = BigEndian(p + 4, 4);
        if (!ReadBytes(&reader, chunk_size, &p))
            goto fail;
        if (!is_track)
            continue;
        ChipReader chunk = {p, chunk_size, 0};
        song->tracks[track].first_program = -1;
        if (!ReadTrack(song, &chunk, track++))
            goto fail;
    }
    if (!Chip_ResolveTiming(song))
        goto fail;
    return song;
fail:
    Grapple_DestroyChipSong(song);
    return NULL;
}

Grapple_ChipSong *Grapple_LoadChipSong_IO(SDL_IOStream *io, bool closeio)
{
    if (!io)
    {
        SDL_SetError("chiptune MIDI: NULL input stream");
        return NULL;
    }
    size_t size = 0;
    size_t capacity = 16384;
    Uint8 *data = SDL_malloc(capacity);
    Grapple_ChipSong *song = NULL;
    if (!data)
        goto done;
    for (;;)
    {
        const size_t got = SDL_ReadIO(io, data + size, capacity - size);
        size += got;
        if (SDL_GetIOStatus(io) == SDL_IO_STATUS_EOF)
            break;
        if (got == 0 || SDL_GetIOStatus(io) != SDL_IO_STATUS_READY)
        {
            SDL_SetError("chiptune MIDI: input read failed");
            goto done;
        }
        if (size == capacity)
        {
            if (capacity == CHIP_MIDI_MAX_BYTES)
            {
                Uint8 extra;
                if (SDL_ReadIO(io, &extra, 1) == 0 && SDL_GetIOStatus(io) == SDL_IO_STATUS_EOF)
                    break;
                SDL_SetError("chiptune MIDI: input exceeds 64 MiB");
                goto done;
            }
            capacity *= 2;
            Uint8 *larger = SDL_realloc(data, capacity);
            if (!larger)
                goto done;
            data = larger;
        }
    }
    song = size >= 4 && SDL_memcmp(data, "MThd", 4) == 0 ? ParseMidi(data, size)
                                                         : Chip_ParseMusicXml(data, size);
done:
    SDL_free(data);
    if (closeio)
        SDL_CloseIO(io);
    return song;
}

Grapple_ChipSong *Grapple_LoadChipSong(const char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    return io ? Grapple_LoadChipSong_IO(io, true) : NULL;
}

const Grapple_ChipSongInfo *Grapple_GetChipSongInfo(const Grapple_ChipSong *song)
{
    return song ? &song->info : NULL;
}

const Grapple_ChipTrackInfo *Grapple_GetChipTrackInfo(const Grapple_ChipSong *song, int track)
{
    if (!song || track < 0 || track >= song->info.track_count)
    {
        SDL_SetError("chiptune: invalid track index");
        return NULL;
    }
    return &song->tracks[track];
}
