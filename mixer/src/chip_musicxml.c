/* Original Grapple code (zlib). MusicXML score-to-performance compiler. */
#include "chip_internal.h"
#include "chip_xml.h"

#define SCORE_MAX_MEASURES 65536
#define SCORE_MAX_STAVES 32
#define SCORE_MAX_RESOLUTION 100000000

typedef struct ScoreNumber
{
    Sint64 numerator, denominator;
} ScoreNumber;
typedef struct ScoreNote
{
    int part, measure, staff, pitch, velocity, channel;
    Sint64 start, duration;
    const char *voice;
    const char *instrument;
    bool tab, skipped, tie_start, tie_stop;
} ScoreNote;
typedef struct ScoreControl
{
    int measure;
    Sint64 start;
    ChipEvent event;
} ScoreControl;
typedef struct ScoreReader
{
    Grapple_ChipSong *song;
    const ChipXmlNode *root;
    const ChipXmlNode *definition;
    int part, measure;
    bool failed;
    const Grapple_ChipImportOptions *options;
    Grapple_ChipDiagnostic *error;
    ScoreNumber divisions;
    Sint64 meter;
    int transpose[SCORE_MAX_STAVES];
    bool tab[SCORE_MAX_STAVES];
    int velocity[SCORE_MAX_STAVES];
    ScoreNote *notes;
    size_t note_count, note_capacity;
    ScoreControl *controls;
    size_t control_count, control_capacity;
    Sint64 *lengths;
    int measures;
} ScoreReader;

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static bool Error(ScoreReader *r, const ChipXmlNode *node, const char *message)
{
    r->failed = true;
    if (r->error && r->error->code == GRAPPLE_CHIP_DIAGNOSTIC_NONE)
        *r->error = (Grapple_ChipDiagnostic){
            GRAPPLE_CHIP_DIAGNOSTIC_SCORE,  GRAPPLE_CHIP_DIAGNOSTIC_ERROR, r->part, r->measure, 0,
            (Uint32)(node ? node->line : 0)};
    return SDL_SetError("MusicXML: part %d measure %d line %lu: %s", r->part + 1, r->measure + 1,
                        node ? node->line : 0, message);
}

static bool Unsupported(ScoreReader *r, const ChipXmlNode *node)
{
    char message[256];
    SDL_snprintf(message, sizeof(message), "Playback of <%s> is not implemented", node->name);
    const Grapple_ChipDiagnostic diagnostic = {GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED,
                                               r->options->strict ? GRAPPLE_CHIP_DIAGNOSTIC_ERROR
                                                                  : GRAPPLE_CHIP_DIAGNOSTIC_WARNING,
                                               r->part,
                                               r->measure,
                                               0,
                                               (Uint32)node->line};
    if (r->options->strict)
    {
        if (r->error)
            *r->error = diagnostic;
        return Error(r, node, message);
    }
    return Chip_AddDiagnostic(r->song, diagnostic, message);
}

static bool CheckPerformance(ScoreReader *r, const ChipXmlNode *node)
{
    static const char *const unsupported[] = {"wedge",
                                              "pedal",
                                              "slur",
                                              "arpeggiate",
                                              "glissando",
                                              "slide",
                                              "fermata",
                                              "bend",
                                              "hammer-on",
                                              "pull-off",
                                              "harmonic",
                                              "trill-mark",
                                              "mordent",
                                              "inverted-mordent",
                                              "turn",
                                              "inverted-turn",
                                              "tremolo",
                                              "staccato",
                                              "staccatissimo",
                                              "tenuto",
                                              "accent",
                                              "strong-accent",
                                              "detached-legato",
                                              "breath-mark",
                                              "caesura",
                                              "swing",
                                              "other-notation"};
    for (; node; node = node->next)
    {
        for (size_t i = 0; i < SDL_arraysize(unsupported); ++i)
            if (Named(node, unsupported[i]) && !Unsupported(r, node))
                return false;
        if (!CheckPerformance(r, node->children))
            return false;
    }
    return true;
}

static Sint64 Gcd(Sint64 a, Sint64 b)
{
    while (b)
    {
        const Sint64 next = a % b;
        a = b;
        b = next;
    }
    return a;
}

static bool Number(const char *text, ScoreNumber *number)
{
    while (SDL_isspace((unsigned char)*text))
        ++text;
    const bool negative = *text == '-';
    if (*text == '-' || *text == '+')
        ++text;
    Sint64 n = 0, d = 1;
    bool point = false, digit = false;
    for (; *text && !SDL_isspace((unsigned char)*text); ++text)
    {
        if (*text == '.' && !point)
        {
            point = true;
            continue;
        }
        if (*text < '0' || *text > '9' || n > 100000000000LL || d > 1000000)
            return false;
        digit = true;
        n = n * 10 + (*text - '0');
        if (point)
            d *= 10;
    }
    while (SDL_isspace((unsigned char)*text))
        ++text;
    if (*text || !digit)
        return false;
    const Sint64 gcd = Gcd(n, d);
    number->numerator = (negative ? -n : n) / gcd;
    number->denominator = d / gcd;
    return true;
}

static int Integer(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback, int low,
                   int high)
{
    if (!*text)
        return fallback;
    ScoreNumber number;
    if (!Number(text, &number) || number.denominator != 1 || number.numerator < low ||
        number.numerator > high)
    {
        Error(r, node, "invalid integer value");
        return fallback;
    }
    return (int)number.numerator;
}

static double Decimal(ScoreReader *r, const ChipXmlNode *node, const char *text, double fallback,
                      double low, double high)
{
    if (!*text)
        return fallback;
    ScoreNumber number;
    if (!Number(text, &number))
    {
        Error(r, node, "invalid decimal value");
        return fallback;
    }
    const double value = (double)number.numerator / (double)number.denominator;
    if (value < low || value > high)
    {
        Error(r, node, "decimal value out of range");
        return fallback;
    }
    return value;
}

static bool Resolution(ScoreReader *r, const ChipXmlNode *node, Sint64 *divisions,
                       Sint64 *fractions)
{
    for (; node; node = node->next)
    {
        if (Named(node, "divisions") || Named(node, "duration") || Named(node, "offset") ||
            Named(node, "beat-type"))
        {
            ScoreNumber n;
            if (!node->text || !Number(node->text, &n))
                return Error(r, node, "invalid score duration");
            const bool divisor = Named(node, "divisions") || Named(node, "beat-type");
            Sint64 value = divisor ? n.numerator : n.denominator;
            Sint64 *scale = divisor ? divisions : fractions;
            if (value <= 0 || value / Gcd(value, *scale) > SCORE_MAX_RESOLUTION / *scale)
                return Error(r, node, "exact timing resolution exceeds resource limit");
            *scale *= value / Gcd(value, *scale);
        }
        if (!Resolution(r, node->children, divisions, fractions))
            return false;
    }
    return true;
}

static Sint64 Ticks(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    ScoreNumber n;
    if (!Number(text, &n))
    {
        Error(r, node, "missing or invalid duration");
        return 0;
    }
    const Sint64 scale = r->song->info.ticks_per_quarter / r->divisions.numerator;
    const Sint64 multiplier = scale * r->divisions.denominator;
    const Sint64 absolute = n.numerator < 0 ? -n.numerator : n.numerator;
    if (multiplier <= 0 || absolute > SDL_MAX_SINT64 / multiplier)
    {
        Error(r, node, "duration overflow");
        return 0;
    }
    const Sint64 product = n.numerator * multiplier;
    if (product % n.denominator)
    {
        Error(r, node, "inexact score duration");
        return 0;
    }
    const Sint64 result = product / n.denominator;
    if (result > 864000LL * r->song->info.ticks_per_quarter ||
        result < -864000LL * r->song->info.ticks_per_quarter)
    {
        Error(r, node, "duration exceeds score limit");
        return 0;
    }
    return result;
}

static bool Grow(void **array, size_t *capacity, size_t count, size_t item_size)
{
    if (count < *capacity)
        return true;
    if (count == CHIP_SONG_MAX_EVENTS)
        return SDL_SetError("MusicXML: event limit exceeded");
    const size_t next = SDL_min(*capacity ? *capacity * 2 : 128, CHIP_SONG_MAX_EVENTS);
    void *larger = SDL_realloc(*array, next * item_size);
    if (!larger)
        return false;
    *array = larger;
    *capacity = next;
    return true;
}

static bool Control(ScoreReader *r, Sint64 start, int status, int a, int b, Uint32 tempo)
{
    if (!Grow((void **)&r->controls, &r->control_capacity, r->control_count, sizeof(*r->controls)))
        return false;
    ScoreControl c = {0};
    c.measure = r->measure;
    c.start = start;
    c.event.track = (Uint16)r->part;
    c.event.status = (Uint8)status;
    c.event.a = (Uint8)a;
    c.event.b = (Uint8)b;
    c.event.tempo = tempo;
    r->controls[r->control_count++] = c;
    return true;
}

static int Staff(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    return Integer(r, node, text, 1, 1, SCORE_MAX_STAVES) - 1;
}

static bool Attributes(ScoreReader *r, const ChipXmlNode *node)
{
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (Named(c, "divisions"))
        {
            if (!Number(c->text ? c->text : "", &r->divisions) || r->divisions.numerator <= 0)
                return Error(r, c, "divisions must be positive");
        }
        else if (Named(c, "transpose"))
        {
            const int amount = Integer(r, c, Chip_XmlText(c, "chromatic"), 0, -127, 127) +
                               12 * Integer(r, c, Chip_XmlText(c, "octave-change"), 0, -10, 10);
            const char *number = Chip_XmlAttribute(c, "number");
            if (*number)
                r->transpose[Staff(r, c, number)] = amount;
            else
                for (int i = 0; i < SCORE_MAX_STAVES; ++i)
                    r->transpose[i] = amount;
        }
        else if (Named(c, "clef"))
            r->tab[Staff(r, c, Chip_XmlAttribute(c, "number"))] =
                SDL_strcmp(Chip_XmlText(c, "sign"), "TAB") == 0;
        else if (Named(c, "time"))
        {
            Sint64 meter = 0;
            int beats = 0;
            for (const ChipXmlNode *t = c->children; t; t = t->next)
            {
                if (Named(t, "beats"))
                {
                    const char *text = t->text ? t->text : "";
                    beats = 0;
                    while (*text)
                    {
                        char *end;
                        const long value = SDL_strtol(text, &end, 10);
                        if (end == text || value < 1 || value > 1000 || beats > 1000)
                            return Error(r, t, "invalid additive meter");
                        beats += (int)value;
                        while (SDL_isspace((unsigned char)*end))
                            ++end;
                        if (!*end)
                            break;
                        if (*end != '+')
                            return Error(r, t, "invalid additive meter");
                        text = end + 1;
                    }
                }
                else if (Named(t, "beat-type"))
                {
                    const int unit = Integer(r, t, t->text ? t->text : "", 4, 1, 4096);
                    meter += (Sint64)r->song->info.ticks_per_quarter * 4 * beats / unit;
                }
            }
            if (meter)
                r->meter = meter;
        }
    }
    return !r->failed;
}

static const ChipXmlNode *Instrument(const ScoreReader *r, const char *id)
{
    const ChipXmlNode *first = NULL;
    for (const ChipXmlNode *c = r->definition->children; c; c = c->next)
        if (Named(c, "midi-instrument"))
        {
            if (!first)
                first = c;
            if (*id && SDL_strcmp(id, Chip_XmlAttribute(c, "id")) == 0)
                return c;
        }
    return *id ? NULL : first;
}

static int Channel(ScoreReader *r, const ChipXmlNode *instrument)
{
    return Integer(r, instrument, Chip_XmlText(instrument, "midi-channel"),
                   Chip_XmlChild(instrument, "midi-unpitched") ? 10 : 1, 1, 16) -
           1;
}

static bool InitInstruments(ScoreReader *r)
{
    for (const ChipXmlNode *c = r->definition->children; c; c = c->next)
    {
        if (!Named(c, "midi-instrument"))
            continue;
        const int channel = Channel(r, c);
        if (Chip_XmlChild(c, "midi-program"))
        {
            const int program = Integer(r, c, Chip_XmlText(c, "midi-program"), 1, 1, 128) - 1;
            if (r->song->tracks[r->part].first_program < 0)
                r->song->tracks[r->part].first_program = program;
            if (!Control(r, 0, 0xc0 | channel, program, 0, 0))
                return false;
        }
        const int volume =
            (int)SDL_round(Decimal(r, c, Chip_XmlText(c, "volume"), 100, 0, 100) * 1.27);
        const int pan =
            (int)SDL_round((Decimal(r, c, Chip_XmlText(c, "pan"), 0, -180, 180) + 180) * 127 / 360);
        if (!Control(r, 0, 0xb0 | channel, 7, volume, 0) ||
            !Control(r, 0, 0xb0 | channel, 10, pan, 0))
            return false;
    }
    return !r->failed;
}

static int Dynamic(const char *name)
{
    static const char *const names[] = {"pppp", "ppp", "pp", "p",   "mp",
                                        "mf",   "f",   "ff", "fff", "ffff"};
    static const int velocities[] = {16, 24, 36, 48, 64, 80, 96, 108, 120, 127};
    for (size_t i = 0; i < SDL_arraysize(names); ++i)
        if (SDL_strcmp(names[i], name) == 0)
            return velocities[i];
    return 80;
}

static bool Direction(ScoreReader *r, const ChipXmlNode *node, Sint64 cursor)
{
    const ChipXmlNode *offset = Chip_XmlChild(node, "offset");
    if (offset)
        cursor += Ticks(r, offset, offset->text ? offset->text : "");
    if (cursor < 0)
        return Error(r, node, "direction before measure start");
    const int staff = Staff(r, node, Chip_XmlText(node, "staff"));
    const ChipXmlNode *sound = Named(node, "sound") ? node : Chip_XmlChild(node, "sound");
    const char *tempo = Chip_XmlAttribute(sound, "tempo");
    if (*tempo)
    {
        const double bpm = Decimal(r, sound, tempo, 120, 20, 600);
        if (!Control(r, cursor, 0, 0, 0, (Uint32)SDL_round(60000000 / bpm)))
            return false;
    }
    const char *dynamics = Chip_XmlAttribute(sound, "dynamics");
    if (*dynamics)
        r->velocity[staff] = (int)SDL_round(Decimal(r, sound, dynamics, 80, 0, 100) * 1.27);
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (!Named(c, "direction-type"))
            continue;
        const ChipXmlNode *dynamic = Chip_XmlChild(c, "dynamics");
        if (dynamic && dynamic->children && !*dynamics)
            r->velocity[staff] = Dynamic(dynamic->children->name);
        const ChipXmlNode *metronome = Chip_XmlChild(c, "metronome");
        if (metronome && !*tempo)
        {
            const char *unit = Chip_XmlText(metronome, "beat-unit");
            double beats = SDL_strcmp(unit, "whole") == 0    ? 4
                           : SDL_strcmp(unit, "half") == 0   ? 2
                           : SDL_strcmp(unit, "eighth") == 0 ? 0.5
                           : SDL_strcmp(unit, "16th") == 0   ? 0.25
                                                             : 1;
            double dot = beats / 2;
            for (const ChipXmlNode *m = metronome->children; m; m = m->next)
                if (Named(m, "beat-unit-dot"))
                {
                    beats += dot;
                    dot /= 2;
                }
            const double bpm =
                Decimal(r, metronome, Chip_XmlText(metronome, "per-minute"), 120, 1, 10000) * beats;
            if (bpm < 20 || bpm > 600)
                return Error(r, metronome, "tempo outside 20..600 quarter beats/minute");
            if (!Control(r, cursor, 0, 0, 0, (Uint32)SDL_round(60000000 / bpm)))
                return false;
        }
    }
    return !r->failed;
}

static bool Note(ScoreReader *r, const ChipXmlNode *node, Sint64 *cursor, Sint64 *previous,
                 Sint64 *extent)
{
    if (Chip_XmlChild(node, "grace"))
        return Error(r, node, "grace-note playback is not implemented yet");
    const Sint64 duration = Ticks(r, node, Chip_XmlText(node, "duration"));
    if (r->failed || duration <= 0)
        return Error(r, node, "note duration must be positive");
    const bool chord = Chip_XmlChild(node, "chord") != NULL;
    const Sint64 start = chord ? *previous : *cursor;
    if (start > 864000LL * r->song->info.ticks_per_quarter - duration)
        return Error(r, node, "measure duration exceeds score limit");
    if (!chord)
    {
        *previous = start;
        *cursor += duration;
    }
    *extent = SDL_max(*extent, start + duration);
    if (Chip_XmlChild(node, "rest"))
        return true;
    ScoreNote note = {0};
    note.part = r->part;
    note.measure = r->measure;
    note.staff = Staff(r, node, Chip_XmlText(node, "staff"));
    note.tab = r->tab[note.staff];
    note.start = start;
    note.duration = duration;
    note.velocity = r->velocity[note.staff];
    note.voice = Chip_XmlText(node, "voice");
    note.instrument = Chip_XmlAttribute(Chip_XmlChild(node, "instrument"), "id");
    const ChipXmlNode *instrument = Instrument(r, note.instrument);
    if (*note.instrument && !instrument)
        return Error(r, node, "unresolved instrument ID");
    note.channel = Channel(r, instrument);
    const ChipXmlNode *pitch = Chip_XmlChild(node, "pitch");
    if (pitch)
    {
        const char *step = Chip_XmlText(pitch, "step");
        static const char letters[] = "C D EF G A B";
        const char *letter = *step ? SDL_strchr(letters, *step) : NULL;
        if (!letter || step[1] || *step == ' ')
            return Error(r, pitch, "invalid pitch step");
        note.pitch = (int)(letter - letters) +
                     12 * (Integer(r, pitch, Chip_XmlText(pitch, "octave"), 4, 0, 9) + 1) +
                     Integer(r, pitch, Chip_XmlText(pitch, "alter"), 0, -12, 12) +
                     r->transpose[note.staff];
    }
    else if (Chip_XmlChild(node, "unpitched") && Chip_XmlChild(instrument, "midi-unpitched"))
        note.pitch =
            Integer(r, instrument, Chip_XmlText(instrument, "midi-unpitched"), 1, 1, 128) - 1;
    else
        return Error(r, node, "note needs pitch or mapped percussion instrument");
    if (note.pitch < 0 || note.pitch > 127)
        return Error(r, node, "sounding pitch outside 0..127");
    for (const ChipXmlNode *c = node->children; c; c = c->next)
        if (Named(c, "tie"))
        {
            const char *type = Chip_XmlAttribute(c, "type");
            note.tie_start |= SDL_strcmp(type, "start") == 0;
            note.tie_stop |= SDL_strcmp(type, "stop") == 0;
        }
    if (!Grow((void **)&r->notes, &r->note_capacity, r->note_count, sizeof(*r->notes)))
        return false;
    r->notes[r->note_count++] = note;
    return !r->failed;
}

static bool Measure(ScoreReader *r, const ChipXmlNode *node)
{
    if (node && !CheckPerformance(r, node->children))
        return false;
    Sint64 cursor = 0, previous = 0, extent = 0;
    for (const ChipXmlNode *c = node ? node->children : NULL; c; c = c->next)
    {
        if (Named(c, "attributes"))
        {
            if (!Attributes(r, c))
                return false;
        }
        else if (Named(c, "note"))
        {
            if (!Note(r, c, &cursor, &previous, &extent))
                return false;
        }
        else if (Named(c, "backup") || Named(c, "forward"))
        {
            const Sint64 delta = Ticks(r, c, Chip_XmlText(c, "duration"));
            if (delta <= 0)
                return Error(r, c, "cursor movement must be positive");
            cursor += Named(c, "backup") ? -delta : delta;
            if (cursor < 0 || cursor > 864000LL * r->song->info.ticks_per_quarter)
                return Error(r, c, "cursor outside measure limits");
            extent = SDL_max(extent, cursor);
        }
        else if (Named(c, "direction") || Named(c, "sound"))
        {
            if (!Direction(r, c, cursor))
                return false;
        }
        else if (Named(c, "barline") && (Chip_XmlChild(c, "repeat") || Chip_XmlChild(c, "ending")))
            return Error(r, c, "repeat/ending playback is not implemented yet");
    }
    if (!extent && node)
        extent = r->meter;
    r->lengths[r->measure] = SDL_max(r->lengths[r->measure], extent);
    return !r->failed;
}

static int SDLCALL CompareNotes(const void *left, const void *right)
{
    const ScoreNote *a = *(const ScoreNote *const *)left;
    const ScoreNote *b = *(const ScoreNote *const *)right;
#define COMPARE(field)                                                                             \
    if (a->field != b->field)                                                                      \
    return a->field < b->field ? -1 : 1
    COMPARE(measure);
    COMPARE(start);
    COMPARE(duration);
    COMPARE(pitch);
    COMPARE(velocity);
    COMPARE(tie_start);
    COMPARE(tie_stop);
#undef COMPARE
    return 0;
}

static bool RemoveMirrors(ScoreReader *r)
{
    if (r->options->staff == -1)
        return true;
    ScoreNote **a = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*a));
    ScoreNote **b = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*b));
    if (!a || !b)
    {
        SDL_free(a);
        SDL_free(b);
        return false;
    }
    size_t begin = 0;
    for (int part = 0; part < r->song->info.track_count; ++part)
    {
        size_t end = begin;
        while (end < r->note_count && r->notes[end].part == part)
            ++end;
        if (r->options->staff > 0)
        {
            Uint32 staves = 0;
            for (size_t i = begin; i < end; ++i)
                staves |= 1u << r->notes[i].staff;
            if (staves && (staves & (staves - 1)))
            {
                if (!(staves & (1u << (r->options->staff - 1))))
                {
                    SDL_free(a);
                    SDL_free(b);
                    return Error(r, NULL, "selected staff is absent from multi-staff part");
                }
                for (size_t i = begin; i < end; ++i)
                    r->notes[i].skipped = r->notes[i].staff != r->options->staff - 1;
            }
            begin = end;
            continue;
        }
        for (int staff = 0; staff < SCORE_MAX_STAVES; ++staff)
        {
            size_t na = 0;
            for (size_t i = begin; i < end; ++i)
                if (r->notes[i].part == part && r->notes[i].staff == staff && r->notes[i].tab)
                    a[na++] = &r->notes[i];
            if (!na)
                continue;
            SDL_qsort(a, na, sizeof(*a), CompareNotes);
            for (int other = 0; other < SCORE_MAX_STAVES; ++other)
            {
                size_t nb = 0;
                for (size_t i = begin; i < end; ++i)
                    if (r->notes[i].part == part && r->notes[i].staff == other && !r->notes[i].tab)
                        b[nb++] = &r->notes[i];
                if (na != nb)
                    continue;
                SDL_qsort(b, nb, sizeof(*b), CompareNotes);
                bool match = true;
                for (size_t i = 0; i < na; ++i)
                    if (CompareNotes(&a[i], &b[i]))
                    {
                        match = false;
                        break;
                    }
                if (match)
                {
                    for (size_t i = 0; i < na; ++i)
                        a[i]->skipped = true;
                    const Grapple_ChipDiagnostic diagnostic = {GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR,
                                                               GRAPPLE_CHIP_DIAGNOSTIC_INFO,
                                                               part,
                                                               -1,
                                                               staff + 1,
                                                               0};
                    if (!Chip_AddDiagnostic(r->song, diagnostic,
                                            "Omitted TAB staff matching a standard-notation staff"))
                    {
                        SDL_free(a);
                        SDL_free(b);
                        return false;
                    }
                    break;
                }
            }
        }
        begin = end;
    }
    SDL_free(a);
    SDL_free(b);
    return true;
}

static bool Compile(ScoreReader *r)
{
    if (!RemoveMirrors(r))
        return false;
    Sint64 position = 0;
    for (int m = 0; m < r->measures; ++m)
    {
        const Sint64 length = r->lengths[m];
        r->lengths[m] = position;
        position += length;
        if (position > 864000LL * r->song->info.ticks_per_quarter)
            return Error(r, NULL, "score length limit exceeded");
    }
    r->song->info.duration_ticks = (Uint64)position;
    for (size_t i = 0; i < r->control_count; ++i)
    {
        ScoreControl *c = &r->controls[i];
        c->event.tick = (Uint64)(r->lengths[c->measure] + c->start);
        if (c->event.tick > r->song->info.duration_ticks)
            return Error(r, NULL, "direction past score end");
        if (!Chip_AppendEvent(r->song, c->event))
            return false;
    }
    for (size_t i = 0; i < r->note_count; ++i)
        r->notes[i].start += r->lengths[r->notes[i].measure];
    size_t active[4096];
    size_t ties = 0;
    for (size_t i = 0; i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if (n->skipped)
            continue;
        size_t target = i;
        if (n->tie_stop)
        {
            size_t t;
            for (t = 0; t < ties; ++t)
            {
                const ScoreNote *a = &r->notes[active[t]];
                if (a->part == n->part && a->staff == n->staff && a->pitch == n->pitch &&
                    a->start + a->duration == n->start && SDL_strcmp(a->voice, n->voice) == 0 &&
                    SDL_strcmp(a->instrument, n->instrument) == 0)
                    break;
            }
            if (t == ties)
                return Error(r, NULL, "tie stop without a matching adjacent start");
            target = active[t];
            r->notes[target].duration += n->duration;
            n->skipped = true;
            active[t] = active[--ties];
        }
        if (n->tie_start)
        {
            if (ties == SDL_arraysize(active))
                return Error(r, NULL, "simultaneous tie limit exceeded");
            active[ties++] = target;
        }
    }
    if (ties)
        return Error(r, NULL, "unterminated tie");
    for (size_t i = 0; i < r->note_count; ++i)
    {
        const ScoreNote *n = &r->notes[i];
        if (n->skipped)
            continue;
        ChipEvent event = {0};
        event.track = (Uint16)n->part;
        event.tick = (Uint64)n->start;
        event.status = (Uint8)(0x90 | n->channel);
        event.a = (Uint8)n->pitch;
        event.b = (Uint8)SDL_max(n->velocity, 1);
        event.note_id = (Uint32)i + 1;
        if (!Chip_AppendEvent(r->song, event))
            return false;
        event.tick += (Uint64)n->duration;
        event.status = (Uint8)(0x80 | n->channel);
        event.b = 0;
        if (!Chip_AppendEvent(r->song, event))
            return false;
        ++r->song->tracks[n->part].note_count;
        r->song->tracks[n->part].channels |= (Uint16)(1u << n->channel);
    }
    return Chip_ResolveTiming(r->song);
}

Grapple_ChipSong *Chip_ParseMusicXml(const void *data, size_t size,
                                     const Grapple_ChipImportOptions *options,
                                     Grapple_ChipDiagnostic *error)
{
    ChipXmlNode *root = Chip_XmlParse(data, size);
    if (!root)
        return NULL;
    ScoreReader r = {0};
    r.root = root;
    r.options = options;
    r.error = error;
    if (!Named(root, "score-partwise") && !Named(root, "score-timewise"))
    {
        Error(&r, root, "expected score-partwise or score-timewise");
        goto done;
    }
    const ChipXmlNode *list = Chip_XmlChild(root, "part-list");
    int parts = 0;
    for (const ChipXmlNode *c = list ? list->children : NULL; c; c = c->next)
        if (Named(c, "score-part"))
            ++parts;
    if (!parts || parts > CHIP_SONG_MAX_TRACKS)
    {
        Error(&r, root, "expected 1..256 score parts");
        goto done;
    }
    Sint64 divisions = 1, fractions = 1;
    if (!Resolution(&r, root, &divisions, &fractions))
        goto done;
    if (divisions > SCORE_MAX_RESOLUTION / fractions)
    {
        Error(&r, root, "exact timing resolution exceeds resource limit");
        goto done;
    }
    r.song = Chip_NewSong(parts, (int)(divisions * fractions));
    r.lengths = SDL_calloc(SCORE_MAX_MEASURES, sizeof(*r.lengths));
    if (!r.song || !r.lengths)
    {
        r.failed = true;
        goto done;
    }
    r.song->independent_parts = true;
    for (const ChipXmlNode *definition = list->children; definition; definition = definition->next)
    {
        if (!Named(definition, "score-part"))
            continue;
        r.definition = definition;
        r.divisions = (ScoreNumber){1, 1};
        r.meter = (Sint64)r.song->info.ticks_per_quarter * 4;
        SDL_zero(r.transpose);
        SDL_zero(r.tab);
        for (int s = 0; s < SCORE_MAX_STAVES; ++s)
            r.velocity[s] = 80;
        r.measure = 0;
        SDL_strlcpy(r.song->tracks[r.part].name, Chip_XmlText(definition, "part-name"),
                    sizeof(r.song->tracks[r.part].name));
        if (!InitInstruments(&r))
        {
            r.failed = true;
            goto done;
        }
        const char *id = Chip_XmlAttribute(definition, "id");
        const ChipXmlNode *container = root;
        if (Named(root, "score-partwise"))
        {
            container = NULL;
            for (const ChipXmlNode *c = root->children; c; c = c->next)
                if (Named(c, "part") && SDL_strcmp(id, Chip_XmlAttribute(c, "id")) == 0)
                {
                    container = c;
                    break;
                }
            if (!container)
            {
                Error(&r, definition, "missing part contents");
                goto done;
            }
        }
        for (const ChipXmlNode *measure = container->children; measure; measure = measure->next)
        {
            if (!Named(measure, "measure"))
                continue;
            if (r.measure == SCORE_MAX_MEASURES)
            {
                Error(&r, measure, "measure limit exceeded");
                goto done;
            }
            const ChipXmlNode *contents = measure;
            if (Named(root, "score-timewise"))
            {
                contents = NULL;
                for (const ChipXmlNode *c = measure->children; c; c = c->next)
                    if (Named(c, "part") && SDL_strcmp(id, Chip_XmlAttribute(c, "id")) == 0)
                    {
                        contents = c;
                        break;
                    }
            }
            if (!Measure(&r, contents))
            {
                r.failed = true;
                goto done;
            }
            ++r.measure;
        }
        r.measures = SDL_max(r.measures, r.measure);
        ++r.part;
    }
    if (!r.measures || !Compile(&r))
        r.failed = true;
done:
    Chip_XmlDestroy(root);
    SDL_free(r.notes);
    SDL_free(r.controls);
    SDL_free(r.lengths);
    if (r.failed)
    {
        Grapple_DestroyChipSong(r.song);
        return NULL;
    }
    return r.song;
}
