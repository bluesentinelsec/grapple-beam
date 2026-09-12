/* Original Grapple code (zlib). Composition snapshots and exact timing. */
#include "chip_internal.h"

bool Chip_AppendEvent(Grapple_ChipSong *song, ChipEvent event)
{
    if (song->count == CHIP_SONG_MAX_EVENTS)
        return SDL_SetError("chiptune song: too many events");
    if (song->count == song->capacity)
    {
        const size_t capacity =
            SDL_min(song->capacity ? song->capacity * 2 : 256, (size_t)CHIP_SONG_MAX_EVENTS);
        ChipEvent *events = SDL_realloc(song->events, capacity * sizeof(*events));
        if (!events)
            return false;
        song->events = events;
        song->capacity = capacity;
    }
    event.order = (Uint32)song->count;
    song->events[song->count++] = event;
    return true;
}

static int SDLCALL CompareEvents(const void *a, const void *b)
{
    const ChipEvent *left = a;
    const ChipEvent *right = b;
    if (left->tick != right->tick)
        return left->tick < right->tick ? -1 : 1;
    if ((left->status == 0xf1) != (right->status == 0xf1))
        return left->status == 0xf1 ? 1 : -1;
    return (left->order > right->order) - (left->order < right->order);
}

static bool AdvanceTime(const Grapple_ChipSong *song, Uint64 delta, Uint32 tempo, Uint64 *time)
{
    const Uint64 limit =
        (Uint64)CHIP_SONG_MAX_SECONDS * 1000000u * (Uint64)song->info.ticks_per_quarter;
    if (delta > (limit - *time) / tempo)
        return SDL_SetError("chiptune song: composition exceeds 24 hours");
    *time += delta * tempo;
    return true;
}

bool Chip_ResolveTiming(Grapple_ChipSong *song)
{
    if (song->count)
        SDL_qsort(song->events, song->count, sizeof(*song->events), CompareEvents);
    Uint64 tick = 0;
    Uint64 time = 0;
    Uint32 tempo = 500000;
    for (size_t i = 0; i < song->count; ++i)
    {
        ChipEvent *event = &song->events[i];
        if (!AdvanceTime(song, event->tick - tick, tempo, &time))
            return false;
        tick = event->tick;
        event->time = time;
        if (event->tempo)
            tempo = event->tempo;
    }
    if (!AdvanceTime(song, song->info.duration_ticks - tick, tempo, &time))
        return false;
    song->end_time = time;
    song->info.duration_seconds = (double)time / (1000000.0 * song->info.ticks_per_quarter);
    return true;
}

Uint64 Chip_TimeToFrame(const Grapple_ChipSong *song, Uint64 time, int sample_rate)
{
    const Uint64 divisor = (Uint64)song->info.ticks_per_quarter * 1000000u;
    const Uint64 fraction = time % divisor;
    Uint64 quotient = 0;
    Uint64 remainder = 0;
    /* Binary long multiplication/division keeps high-resolution scores exact
       without overflowing the product of fractional time and sample rate. */
    for (Uint32 bit = 1u << 18; bit; bit >>= 1)
    {
        remainder = remainder * 2 + (((Uint32)sample_rate & bit) ? fraction : 0);
        quotient = quotient * 2 + remainder / divisor;
        remainder %= divisor;
    }
    return (time / divisor) * (Uint64)sample_rate + quotient + (remainder >= (divisor + 1) / 2);
}

Grapple_ChipSong *Chip_NewSong(int tracks, int ppqn)
{
    Grapple_ChipSong *song = SDL_calloc(1, sizeof(*song));
    if (!song)
        return NULL;
    SDL_SetAtomicInt(&song->references, 1);
    song->info.track_count = tracks;
    song->info.ticks_per_quarter = ppqn;
    song->tracks = SDL_calloc((size_t)tracks, sizeof(*song->tracks));
    song->presets = SDL_calloc((size_t)tracks, sizeof(*song->presets));
    song->gains = SDL_calloc((size_t)tracks, sizeof(*song->gains));
    if (!song->tracks || !song->presets || !song->gains)
    {
        Grapple_DestroyChipSong(song);
        return NULL;
    }
    for (int i = 0; i < tracks; ++i)
    {
        song->tracks[i].first_program = -1;
        song->gains[i] = 1.0f;
    }
    return song;
}

void Chip_RetainSong(const Grapple_ChipSong *song)
{
    SDL_AtomicIncRef(&((Grapple_ChipSong *)song)->references);
}

void Grapple_DestroyChipSong(Grapple_ChipSong *song)
{
    if (song && SDL_AtomicDecRef(&song->references))
    {
        SDL_free(song->tracks);
        SDL_free(song->presets);
        SDL_free(song->gains);
        SDL_free(song->events);
        SDL_free(song->expressions);
        for (int i = 0; i < song->diagnostic_count; ++i)
            SDL_free(song->diagnostic_messages[i]);
        SDL_free(song->diagnostics);
        SDL_free(song->diagnostic_messages);
        SDL_free(song->measures);
        SDL_free(song->sections);
        SDL_free(song);
    }
}

bool Grapple_GetChipImportDefaults(Grapple_ChipImportOptions *options)
{
    if (!options)
        return SDL_SetError("chiptune: NULL import options output");
    *options = (Grapple_ChipImportOptions){true, 0,     0.5,   0.25,  0.75, 0.85,
                                           0.25, 0.125, 0.125, 0.125, 1.5,  2};
    return true;
}

bool Chip_AddDiagnostic(Grapple_ChipSong *song, Grapple_ChipDiagnostic diagnostic,
                        const char *message)
{
    if (song->diagnostic_count == 1024)
        return SDL_SetError("MusicXML: diagnostic limit exceeded");
    const size_t count = (size_t)song->diagnostic_count + 1;
    Grapple_ChipDiagnostic *diagnostics =
        SDL_realloc(song->diagnostics, count * sizeof(*diagnostics));
    if (!diagnostics)
        return false;
    song->diagnostics = diagnostics;
    char **messages = SDL_realloc(song->diagnostic_messages, count * sizeof(*messages));
    if (!messages)
        return false;
    song->diagnostic_messages = messages;
    messages[count - 1] = SDL_strdup(message);
    if (!messages[count - 1])
        return false;
    diagnostics[count - 1] = diagnostic;
    ++song->diagnostic_count;
    return true;
}

int Grapple_GetChipDiagnosticCount(const Grapple_ChipSong *song)
{
    return song ? song->diagnostic_count : 0;
}

bool Grapple_ReadChipDiagnostic(const Grapple_ChipSong *song, int index,
                                Grapple_ChipDiagnostic *diagnostic)
{
    if (!song || !diagnostic || index < 0 || index >= song->diagnostic_count)
        return SDL_SetError("chiptune: invalid diagnostic index/output");
    *diagnostic = song->diagnostics[index];
    return true;
}

const char *Grapple_GetChipDiagnosticMessage(const Grapple_ChipSong *song, int index)
{
    if (!song || index < 0 || index >= song->diagnostic_count)
    {
        SDL_SetError("chiptune: invalid diagnostic index");
        return NULL;
    }
    return song->diagnostic_messages[index];
}

Grapple_ChipComposer *Grapple_CreateChipComposer(int tracks, int ticks_per_quarter)
{
    if (tracks < 1 || tracks > CHIP_SONG_MAX_TRACKS || ticks_per_quarter < 1 ||
        ticks_per_quarter > 32767)
    {
        SDL_SetError("chiptune composer: invalid part count or tick resolution");
        return NULL;
    }
    Grapple_ChipComposer *composer = SDL_calloc(1, sizeof(*composer));
    if (!composer)
        return NULL;
    composer->song = Chip_NewSong(tracks, ticks_per_quarter);
    if (!composer->song)
    {
        SDL_free(composer);
        return NULL;
    }
    composer->song->independent_parts = true;
    return composer;
}

void Grapple_DestroyChipComposer(Grapple_ChipComposer *composer)
{
    if (composer)
    {
        Grapple_DestroyChipSong(composer->song);
        SDL_free(composer);
    }
}

bool Grapple_SetChipPart(Grapple_ChipComposer *composer, int track, const char *name,
                         Grapple_ChipPreset preset, float gain)
{
    if (!composer || !name || track < 0 || track >= composer->song->info.track_count ||
        preset < GRAPPLE_CHIP_PRESET_AUTO || preset > GRAPPLE_CHIP_PRESET_DRUMS ||
        !(gain >= 0.0f && gain <= 2.0f))
        return SDL_SetError("chiptune composer: invalid part description");
    Grapple_ChipSong *song = composer->song;
    SDL_strlcpy(song->tracks[track].name, name, sizeof(song->tracks[track].name));
    song->presets[track] = preset;
    song->gains[track] = gain;
    return true;
}

static bool ValidTick(const Grapple_ChipSong *song, Uint64 tick)
{
    return tick <= (Uint64)CHIP_SONG_MAX_SECONDS * 10u * (Uint64)song->info.ticks_per_quarter;
}

bool Grapple_AddChipNote(Grapple_ChipComposer *composer, const Grapple_ChipNote *note)
{
    return Grapple_AddChipNoteEx(composer, note, NULL);
}

bool Grapple_AddChipNoteEx(Grapple_ChipComposer *composer, const Grapple_ChipNote *note,
                           const Grapple_ChipExpression *expression)
{
    if (!composer || !note || note->track < 0 || note->track >= composer->song->info.track_count ||
        note->note < 0 || note->note > 127 || note->velocity < 1 || note->velocity > 127 ||
        note->duration_ticks == 0 || note->duration_ticks > SDL_MAX_UINT64 - note->start_tick ||
        !ValidTick(composer->song, note->start_tick + note->duration_ticks))
        return SDL_SetError("chiptune composer: invalid note");
    Grapple_ChipSong *song = composer->song;
    if (song->count > CHIP_SONG_MAX_EVENTS - 2)
        return SDL_SetError("chiptune composer: too many notes");
    ChipEvent event = {0};
    event.tick = note->start_tick;
    event.track = (Uint16)note->track;
    event.status = 0x90;
    event.a = (Uint8)note->note;
    event.b = (Uint8)note->velocity;
    event.duration = note->duration_ticks;
    event.note_id = (Uint32)song->count + 1;
    if (!Chip_AppendExpression(song, expression, &event.expression))
        return false;
    if (!Chip_AppendEvent(song, event))
    {
        song->expression_count -= event.expression != 0;
        return false;
    }
    event.tick += note->duration_ticks;
    event.status = 0x80;
    event.b = 0;
    if (!Chip_AppendEvent(song, event))
    {
        --song->count;
        song->expression_count -= event.expression != 0;
        return false;
    }
    ++song->tracks[note->track].note_count;
    song->tracks[note->track].channels = 1;
    song->info.duration_ticks = SDL_max(song->info.duration_ticks, event.tick);
    return true;
}

bool Grapple_AddChipControl(Grapple_ChipComposer *composer, int track, Uint64 tick, int controller,
                            int value)
{
    if (!composer || track < 0 || track >= composer->song->info.track_count ||
        !ValidTick(composer->song, tick) || value < 0 || value > 127)
        return SDL_SetError("chiptune composer: invalid controller arguments");
    switch (controller)
    {
    case 1:
    case 7:
    case 10:
    case 11:
    case 64:
    case 66:
    case 67:
    case 120:
    case 121:
    case 123:
        break;
    default:
        return SDL_SetError("chiptune composer: unsupported controller");
    }
    ChipEvent event = {0};
    event.tick = tick;
    event.track = (Uint16)track;
    event.status = 0xb0;
    event.a = (Uint8)controller;
    event.b = (Uint8)value;
    if (!Chip_AppendEvent(composer->song, event))
        return false;
    composer->song->info.duration_ticks = SDL_max(composer->song->info.duration_ticks, tick);
    return true;
}

bool Grapple_AddChipTempo(Grapple_ChipComposer *composer, Uint64 tick, double bpm)
{
    if (!composer || !(bpm >= 20.0 && bpm <= 600.0) || !ValidTick(composer->song, tick))
        return SDL_SetError("chiptune composer: invalid tempo or tick");
    ChipEvent event = {0};
    event.tick = tick;
    event.tempo = (Uint32)(60000000.0 / bpm + 0.5);
    if (!Chip_AppendEvent(composer->song, event))
        return false;
    composer->song->info.duration_ticks = SDL_max(composer->song->info.duration_ticks, tick);
    return true;
}

Grapple_ChipSong *Grapple_BuildChipSong(const Grapple_ChipComposer *composer, Uint64 end_tick)
{
    if (!composer || (end_tick && end_tick < composer->song->info.duration_ticks) ||
        !ValidTick(composer->song, end_tick))
    {
        SDL_SetError("chiptune composer: end precedes the last event or exceeds bounds");
        return NULL;
    }
    const Grapple_ChipSong *source = composer->song;
    Grapple_ChipSong *song = Chip_NewSong(source->info.track_count, source->info.ticks_per_quarter);
    if (!song)
        return NULL;
    const size_t tracks = (size_t)source->info.track_count;
    song->independent_parts = source->independent_parts;
    SDL_memcpy(song->tracks, source->tracks, tracks * sizeof(*song->tracks));
    SDL_memcpy(song->presets, source->presets, tracks * sizeof(*song->presets));
    SDL_memcpy(song->gains, source->gains, tracks * sizeof(*song->gains));
    song->info.duration_ticks = end_tick ? end_tick : source->info.duration_ticks;
    for (size_t i = 0; i < source->expression_count; ++i)
    {
        Uint32 index;
        if (!Chip_AppendExpression(song, &source->expressions[i], &index))
            goto fail;
    }
    for (size_t i = 0; i < source->count; ++i)
        if (!Chip_AppendEvent(song, source->events[i]))
            goto fail;
    for (int i = 0; i < source->section_count; ++i)
        if (!Chip_AppendSection(song, source->sections[i]))
            goto fail;
    if (!Chip_ResolveTiming(song) || !Chip_ResolveSections(song))
        goto fail;
    return song;
fail:
    Grapple_DestroyChipSong(song);
    return NULL;
}

bool Grapple_ReadChipSongInfo(const Grapple_ChipSong *song, Grapple_ChipSongInfo *info)
{
    if (!song || !info)
        return SDL_SetError("chiptune: NULL song or metadata output");
    *info = song->info;
    return true;
}
