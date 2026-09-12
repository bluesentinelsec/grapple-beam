/* Original Grapple code (zlib). Indexed MusicXML instruments and per-note metadata. */
#include "chip_score.h"

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static int Compare(const void *left, const void *right)
{
    return SDL_strcmp(((const ScoreInstrument *)left)->id, ((const ScoreInstrument *)right)->id);
}

const ScoreInstrument *Chip_ScoreInstrument(ScoreReader *r, const ChipXmlNode *node, const char *id)
{
    if (!*id)
        id = r->default_instrument;
    int low = 0, high = r->instrument_count;
    while (low < high)
    {
        const int mid = low + (high - low) / 2;
        const int order = SDL_strcmp(r->instruments[mid].id, id);
        if (order < 0)
            low = mid + 1;
        else
            high = mid;
    }
    if (low < r->instrument_count && SDL_strcmp(r->instruments[low].id, id) == 0)
        return &r->instruments[low];
    Chip_ScoreError(r, node, "unresolved instrument ID");
    return NULL;
}

static bool ReadMetadata(ScoreReader *r, ScoreInstrument *instrument, const ChipXmlNode *node)
{
    if (Chip_XmlChild(node, "midi-unpitched"))
        instrument->unpitched =
            Chip_ScoreInteger(r, node, Chip_XmlText(node, "midi-unpitched"), 1, 1, 128) - 1;
    if (Chip_XmlChild(node, "midi-channel"))
        instrument->channel =
            Chip_ScoreInteger(r, node, Chip_XmlText(node, "midi-channel"), 1, 1, 16) - 1;
    else if (instrument->unpitched >= 0)
        instrument->channel = 9;
    if (Chip_XmlChild(node, "midi-program"))
        instrument->program =
            Chip_ScoreInteger(r, node, Chip_XmlText(node, "midi-program"), 1, 1, 128) - 1;
    if (Chip_XmlChild(node, "volume"))
        instrument->gain =
            (float)Chip_ScoreDecimal(r, node, Chip_XmlText(node, "volume"), 100, 0, 100) / 100;
    if (Chip_XmlChild(node, "pan"))
        instrument->pan =
            (float)Chip_ScoreDecimal(r, node, Chip_XmlText(node, "pan"), 0, -180, 180);
    return !r->failed;
}

bool Chip_InitScoreInstruments(ScoreReader *r)
{
    SDL_free(r->instruments);
    r->instruments = SDL_calloc(256, sizeof(*r->instruments));
    r->instrument_count = 0;
    if (!r->instruments)
        return false;
    for (const ChipXmlNode *c = r->definition->children; c; c = c->next)
    {
        if (!Named(c, "score-instrument") && !Named(c, "midi-instrument"))
            continue;
        const char *id = Chip_XmlAttribute(c, "id");
        if (!*id)
            return Chip_ScoreError(r, c, "instrument definition needs an ID");
        int index = 0;
        while (index < r->instrument_count && SDL_strcmp(r->instruments[index].id, id) != 0)
            ++index;
        if (index == r->instrument_count)
        {
            if (index == 256)
                return Chip_ScoreError(r, c, "instrument limit exceeded (256 per part)");
            r->instruments[index] = (ScoreInstrument){id, NULL, 0, 0, -1, 1, 0};
            ++r->instrument_count;
        }
        if (Named(c, "midi-instrument"))
        {
            if (r->instruments[index].node)
                return Chip_ScoreError(r, c, "duplicate MIDI instrument ID");
            r->instruments[index].node = c;
            if (!ReadMetadata(r, &r->instruments[index], c))
                return false;
        }
    }
    if (!r->instrument_count)
    {
        r->instruments[0] = (ScoreInstrument){"", NULL, 0, 0, -1, 1, 0};
        r->instrument_count = 1;
    }
    r->default_instrument = r->instruments[0].id;
    r->song->tracks[r->part].first_program = r->instruments[0].program;
    Uint16 channels = 0;
    for (int i = 0; i < r->instrument_count; ++i)
    {
        const ScoreInstrument *instrument = &r->instruments[i];
        const Uint16 bit = (Uint16)(1u << instrument->channel);
        if (!(channels & bit) &&
            !Chip_ScoreControl(r, 0, 0xc0 | instrument->channel, instrument->program, 0, 0))
            return false;
        channels |= bit;
    }
    SDL_qsort(r->instruments, (size_t)r->instrument_count, sizeof(*r->instruments), Compare);
    return true;
}

bool Chip_ReadInstrumentChange(ScoreReader *r, const ChipXmlNode *sound, Sint64 cursor)
{
    for (const ChipXmlNode *c = sound ? sound->children : NULL; c; c = c->next)
    {
        if (!Named(c, "midi-instrument"))
            continue;
        const ScoreInstrument *instrument = Chip_ScoreInstrument(r, c, Chip_XmlAttribute(c, "id"));
        if (!instrument ||
            !Chip_ScoreGrow((void **)&r->instrument_changes, &r->instrument_change_capacity,
                            r->instrument_change_count, sizeof(*r->instrument_changes)))
            return false;
        r->instrument_changes[r->instrument_change_count++] =
            (ScoreInstrumentChange){r->part, r->measure, cursor, c, *instrument};
    }
    return true;
}

static int CompareChange(const void *left, const void *right)
{
    const ScoreInstrumentChange *a = left, *b = right;
    if (a->part != b->part)
        return a->part < b->part ? -1 : 1;
    const int id = SDL_strcmp(a->instrument.id, b->instrument.id);
    if (id)
        return id;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return (a->node->line > b->node->line) - (a->node->line < b->node->line);
}

bool Chip_ApplyInstrumentChanges(ScoreReader *r)
{
    if (!r->instrument_change_count)
        return true;
    Sint64 *positions = SDL_calloc((size_t)r->measures + 1, sizeof(*positions));
    if (!positions)
        return false;
    for (int i = 0; i < r->measures; ++i)
        positions[i + 1] = positions[i] + r->lengths[i];
    for (size_t i = 0; i < r->instrument_change_count; ++i)
        r->instrument_changes[i].start += positions[r->instrument_changes[i].measure];
    SDL_qsort(r->instrument_changes, r->instrument_change_count, sizeof(*r->instrument_changes),
              CompareChange);
    bool ok = true;
    for (size_t i = 0; ok && i < r->instrument_change_count; ++i)
    {
        ScoreInstrumentChange *c = &r->instrument_changes[i];
        r->part = c->part;
        r->measure = c->measure;
        if (c->start < 0 || c->start > positions[r->measures])
        {
            ok = Chip_ScoreError(r, c->node, "instrument change outside score");
            break;
        }
        if (i && c->part == r->instrument_changes[i - 1].part &&
            SDL_strcmp(c->instrument.id, r->instrument_changes[i - 1].instrument.id) == 0)
            c->instrument = r->instrument_changes[i - 1].instrument;
        ok = ReadMetadata(r, &c->instrument, c->node);
    }
    for (size_t i = 0; ok && i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        const Sint64 tick = positions[n->measure] + n->start;
        size_t low = 0, high = r->instrument_change_count;
        while (low < high)
        {
            const size_t mid = low + (high - low) / 2;
            const ScoreInstrumentChange *c = &r->instrument_changes[mid];
            const int id = SDL_strcmp(c->instrument.id, n->instrument);
            if (c->part < n->part ||
                (c->part == n->part && (id < 0 || (id == 0 && c->start <= tick))))
                low = mid + 1;
            else
                high = mid;
        }
        if (!low)
            continue;
        const ScoreInstrumentChange *c = &r->instrument_changes[low - 1];
        if (c->part != n->part || SDL_strcmp(c->instrument.id, n->instrument) != 0)
            continue;
        n->program = c->instrument.program;
        n->channel = c->instrument.channel;
        n->instrument_gain = c->instrument.gain;
        n->instrument_pan = c->instrument.pan;
        if (n->unpitched && c->instrument.unpitched >= 0)
            n->pitch = c->instrument.unpitched;
    }
    SDL_free(positions);
    return ok;
}
