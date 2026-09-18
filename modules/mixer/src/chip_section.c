/* Original Grapple code (zlib). Immutable section metadata shared by all authoring formats. */
#include "chip_internal.h"

bool Chip_AppendSection(Grapple_ChipSong *song, Grapple_ChipSection section)
{
    if (song->section_count == 4096)
        return SDL_SetError("chiptune: section limit exceeded (4096)");
    Grapple_ChipSection *sections =
        SDL_realloc(song->sections, ((size_t)song->section_count + 1) * sizeof(*sections));
    if (!sections)
        return false;
    song->sections = sections;
    song->sections[song->section_count++] = section;
    return true;
}

static int Compare(const void *left, const void *right)
{
    const Grapple_ChipSection *a = left, *b = right;
    if (a->start_tick != b->start_tick)
        return a->start_tick < b->start_tick ? -1 : 1;
    const int name = SDL_strcmp(a->name, b->name);
    if (name)
        return name;
    return (a->end_tick > b->end_tick) - (a->end_tick < b->end_tick);
}

bool Chip_ResolveSections(Grapple_ChipSong *song)
{
    SDL_qsort(song->sections, (size_t)song->section_count, sizeof(*song->sections), Compare);
    int count = 0;
    for (int i = 0; i < song->section_count; ++i)
    {
        if (count && Compare(&song->sections[count - 1], &song->sections[i]) == 0)
            continue;
        if (song->sections[i].start_tick > song->info.duration_ticks ||
            song->sections[i].end_tick > song->info.duration_ticks)
            return SDL_SetError("chiptune: section outside musical duration");
        song->sections[count++] = song->sections[i];
    }
    song->section_count = count;
    Uint64 next = song->info.duration_ticks;
    for (int i = count - 1; i >= 0; --i)
    {
        Grapple_ChipSection *section = &song->sections[i];
        if (i + 1 < count && section->start_tick < song->sections[i + 1].start_tick)
            next = song->sections[i + 1].start_tick;
        if (!section->end_tick)
            section->end_tick = next;
    }
    return true;
}

int Grapple_GetChipSectionCount(const Grapple_ChipSong *song)
{
    return song ? song->section_count : 0;
}

bool Grapple_ReadChipSection(const Grapple_ChipSong *song, int index, Grapple_ChipSection *section)
{
    if (!song || !section || index < 0 || index >= song->section_count)
        return SDL_SetError("chiptune: invalid section output/index");
    *section = song->sections[index];
    return true;
}

bool Grapple_AddChipSection(Grapple_ChipComposer *composer, const char *name, Uint64 start_tick,
                            Uint64 end_tick)
{
    if (!composer || !name || !*name || SDL_strlen(name) > 127 || start_tick >= end_tick ||
        end_tick >
            (Uint64)CHIP_SONG_MAX_SECONDS * 10u * (Uint64)composer->song->info.ticks_per_quarter)
        return SDL_SetError("chiptune: invalid section label or bounds");
    Grapple_ChipSection section = {0};
    SDL_strlcpy(section.name, name, sizeof(section.name));
    section.start_tick = start_tick;
    section.end_tick = end_tick;
    section.source_measure = section.measure_visit = -1;
    if (!Chip_AppendSection(composer->song, section))
        return false;
    composer->song->info.duration_ticks = SDL_max(composer->song->info.duration_ticks, end_tick);
    return true;
}
