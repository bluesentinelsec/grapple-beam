/* Original Grapple code (zlib). MusicXML score-to-performance compiler. */
#include "chip_internal.h"
#include "chip_score_order.h"
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
    ChipScoreMeasure *navigation;
    Uint32 ending;
} ScoreReader;

static int Integer(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback, int low,
                   int high);

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static bool Error(ScoreReader *r, const ChipXmlNode *node, const char *message)
{
    if (r->failed)
        return false;
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

static Uint32 Passes(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    Uint32 result = 0;
    while (*text)
    {
        char *end;
        const long first = SDL_strtol(text, &end, 10);
        if (end == text || first < 1 || first > 32)
        {
            Error(r, node, "invalid ending/time-only pass list");
            return 0;
        }
        while (SDL_isspace((unsigned char)*end))
            ++end;
        long last = first;
        if (*end == '-')
        {
            const char *tail = end + 1;
            last = SDL_strtol(tail, &end, 10);
            if (end == tail || last < first || last > 32)
            {
                Error(r, node, "invalid ending pass range");
                return 0;
            }
        }
        for (long pass = first; pass <= last; ++pass)
            result |= 1u << (pass - 1);
        while (SDL_isspace((unsigned char)*end))
            ++end;
        if (!*end)
            break;
        if (*end != ',')
        {
            Error(r, node, "invalid pass-list separator");
            return 0;
        }
        text = end + 1;
    }
    return result;
}

static bool Navigation(ScoreReader *r, const ChipXmlNode *measure)
{
    bool close_ending = false;
    for (const ChipXmlNode *c = measure ? measure->children : NULL; c; c = c->next)
    {
        if (!Named(c, "barline"))
            continue;
        const ChipXmlNode *repeat = Chip_XmlChild(c, "repeat");
        if (repeat)
        {
            const char *direction = Chip_XmlAttribute(repeat, "direction");
            const char *location = Chip_XmlAttribute(c, "location");
            int index = r->measure;
            if (SDL_strcmp(direction, "forward") == 0)
            {
                if (SDL_strcmp(location, "right") == 0)
                    ++index;
                if (index >= SCORE_MAX_MEASURES)
                    return Error(r, repeat, "repeat beyond measure limit");
                r->navigation[index].repeat_start = true;
            }
            else if (SDL_strcmp(direction, "backward") == 0)
            {
                if (SDL_strcmp(location, "left") == 0)
                    --index;
                if (index < 0)
                    return Error(r, repeat, "backward repeat before score start");
                const int times = Integer(r, repeat, Chip_XmlAttribute(repeat, "times"), 2, 1, 32);
                if (r->navigation[index].repeat_count && r->navigation[index].repeat_count != times)
                    return Error(r, repeat, "conflicting repeat counts across parts");
                r->navigation[index].repeat_count = times;
                r->navigation[index].repeat_after_jump |=
                    SDL_strcmp(Chip_XmlAttribute(repeat, "after-jump"), "yes") == 0;
            }
            else
                return Error(r, repeat, "invalid repeat direction");
        }
        const ChipXmlNode *ending = Chip_XmlChild(c, "ending");
        if (ending)
        {
            const char *type = Chip_XmlAttribute(ending, "type");
            if (SDL_strcmp(type, "start") == 0)
                r->ending = Passes(r, ending, Chip_XmlAttribute(ending, "number"));
            else if (SDL_strcmp(type, "stop") == 0 || SDL_strcmp(type, "discontinue") == 0)
                close_ending = true;
            else
                return Error(r, ending, "invalid ending type");
        }
    }
    ChipScoreMeasure *target = &r->navigation[r->measure];
    if (r->ending)
    {
        if (target->endings && target->endings != r->ending)
            return Error(r, measure, "conflicting endings across parts");
        target->endings = r->ending;
    }
    if (close_ending)
        r->ending = 0;
    return !r->failed;
}

static bool NavigationSound(ScoreReader *r, const ChipXmlNode *sound)
{
    if (!sound)
        return true;
    ChipScoreMeasure *m = &r->navigation[r->measure];
    struct
    {
        const char *name;
        const char **value;
    } labels[] = {{"segno", &m->segno},
                  {"coda", &m->coda},
                  {"dalsegno", &m->dalsegno},
                  {"tocoda", &m->tocoda}};
    for (size_t i = 0; i < SDL_arraysize(labels); ++i)
    {
        const char *value = Chip_XmlAttribute(sound, labels[i].name);
        if (*value)
        {
            if (*labels[i].value && SDL_strcmp(*labels[i].value, value))
                return Error(r, sound, "conflicting score navigation labels");
            *labels[i].value = value;
        }
    }
    m->dacapo |= SDL_strcmp(Chip_XmlAttribute(sound, "dacapo"), "yes") == 0;
    m->fine |= *Chip_XmlAttribute(sound, "fine") != 0;
    m->repeat_start |= SDL_strcmp(Chip_XmlAttribute(sound, "forward-repeat"), "yes") == 0;
    const char *passes = Chip_XmlAttribute(sound, "time-only");
    if (*passes && (m->dacapo || m->dalsegno || m->tocoda))
        m->jump_times = Passes(r, sound, passes);
    return !r->failed;
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
    c.event.order = (Uint32)r->control_count;
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
        const double angle = Decimal(r, c, Chip_XmlText(c, "pan"), 0, -180, 180);
        const int pan = (int)SDL_round((SDL_sin(angle * SDL_PI_D / 180) + 1) * 63.5);
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
    const ChipXmlNode *sound = Named(node, "sound") ? node : Chip_XmlChild(node, "sound");
    const ChipXmlNode *offset = Chip_XmlChild(sound, "offset");
    if (!offset)
        offset = Chip_XmlChild(node, "offset");
    if (offset)
        cursor += Ticks(r, offset, offset->text ? offset->text : "");
    if (cursor < 0)
        return Error(r, node, "direction before measure start");
    const int staff = Staff(r, node, Chip_XmlText(node, "staff"));
    if (!NavigationSound(r, sound))
        return false;
    const char *tempo = Chip_XmlAttribute(sound, "tempo");
    if (*tempo)
    {
        const double bpm = Decimal(r, sound, tempo, 120, 20, 600);
        if (!Control(r, cursor, 0, 0, 0, (Uint32)SDL_round(60000000 / bpm)))
            return false;
    }
    const char *dynamics = Chip_XmlAttribute(sound, "dynamics");
    if (*dynamics)
        r->velocity[staff] =
            SDL_min(127, (int)SDL_round(Decimal(r, sound, dynamics, 100, 0, 1000) * 0.9));
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (!Named(c, "direction-type"))
            continue;
        const ChipXmlNode *dynamic = Chip_XmlChild(c, "dynamics");
        if (dynamic && dynamic->children && !*dynamics)
            r->velocity[staff] = Dynamic(dynamic->children->name);
        ChipScoreMeasure *navigation = &r->navigation[r->measure];
        if (Chip_XmlChild(c, "segno") && !navigation->segno)
            navigation->segno = "default";
        if (Chip_XmlChild(c, "coda") && !navigation->coda)
            navigation->coda = "default";
        const char *words = Chip_XmlText(c, "words");
        char instruction[64];
        size_t length = 0;
        for (; *words && length + 1 < sizeof(instruction); ++words)
            if (SDL_isalpha((unsigned char)*words))
                instruction[length++] = (char)SDL_tolower((unsigned char)*words);
        instruction[length] = 0;
        if (SDL_strcmp(instruction, "dc") == 0 || SDL_strcmp(instruction, "dcalfine") == 0 ||
            SDL_strcmp(instruction, "dcalcoda") == 0)
            navigation->dacapo = true;
        if (!navigation->dalsegno &&
            (SDL_strcmp(instruction, "ds") == 0 || SDL_strcmp(instruction, "dsalfine") == 0 ||
             SDL_strcmp(instruction, "dsalcoda") == 0))
            navigation->dalsegno = "default";
        if (!navigation->tocoda && SDL_strcmp(instruction, "tocoda") == 0)
            navigation->tocoda = "default";
        if (SDL_strcmp(instruction, "fine") == 0)
            navigation->fine = true;
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
    if (Chip_XmlChild(node, "rest") || Chip_XmlChild(node, "cue"))
        return true;
    ScoreNote note = {0};
    note.part = r->part;
    note.measure = r->measure;
    note.staff = Staff(r, node, Chip_XmlText(node, "staff"));
    note.tab = r->tab[note.staff];
    note.start = start;
    note.duration = duration;
    note.velocity = r->velocity[note.staff];
    const char *dynamics = Chip_XmlAttribute(node, "dynamics");
    if (*dynamics)
        note.velocity =
            SDL_min(127, (int)SDL_round(Decimal(r, node, dynamics, 100, 0, 1000) * 0.9));
    note.voice = Chip_XmlText(node, "voice");
    note.instrument = Chip_XmlAttribute(Chip_XmlChild(node, "instrument"), "id");
    const ChipXmlNode *instrument = Instrument(r, note.instrument);
    if (*note.instrument && !instrument)
        return Error(r, node, "unresolved instrument ID");
    note.channel = Channel(r, instrument);
    const ChipXmlNode *pitch = Chip_XmlChild(node, "pitch");
    if (pitch)
    {
        if (!*Chip_XmlText(pitch, "octave"))
            return Error(r, pitch, "pitch is missing its octave");
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
    if (!Navigation(r, node))
        return false;
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

static bool RestoreControls(ScoreReader *r, const ScoreControl *controls, size_t count,
                            const Sint64 *positions, int source)
{
    const size_t channels = (size_t)r->song->info.track_count * 16;
    ChipEvent *state = SDL_calloc(channels * 130, sizeof(*state));
    bool *used = SDL_calloc(channels, sizeof(*used));
    if (!state || !used)
    {
        SDL_free(state);
        SDL_free(used);
        return false;
    }
    for (size_t i = 0; i < count; ++i)
        if (controls[i].event.status)
            used[(size_t)controls[i].event.track * 16 + (controls[i].event.status & 15u)] = true;
    const int defaults[][2] = {{1, 0}, {7, 127}, {10, 64}, {11, 127}, {64, 0}, {66, 0}, {67, 0}};
    for (size_t channel = 0; channel < channels; ++channel)
    {
        if (!used[channel])
            continue;
        for (size_t i = 0; i < SDL_arraysize(defaults); ++i)
        {
            ChipEvent *event = &state[channel * 130 + (size_t)defaults[i][0]];
            event->track = (Uint16)(channel / 16);
            event->status = (Uint8)(0xb0u | (channel & 15u));
            event->a = (Uint8)defaults[i][0];
            event->b = (Uint8)defaults[i][1];
        }
        state[channel * 130 + 128].track = (Uint16)(channel / 16);
        state[channel * 130 + 128].status = (Uint8)(0xc0u | (channel & 15u));
        state[channel * 130 + 129].track = (Uint16)(channel / 16);
        state[channel * 130 + 129].status = (Uint8)(0xe0u | (channel & 15u));
        state[channel * 130 + 129].b = 64;
    }
    Uint32 tempo = 500000;
    Sint64 tempo_at = -1;
    for (size_t i = 0; i < count; ++i)
    {
        const ScoreControl *control = &controls[i];
        const Sint64 at = positions[control->measure] + control->start;
        if (at >= positions[source])
            continue;
        if (control->event.tempo)
        {
            if (at >= tempo_at)
            {
                tempo_at = at;
                tempo = control->event.tempo;
            }
            continue;
        }
        const int kind = control->event.status >> 4;
        const size_t key = kind == 11 ? control->event.a : kind == 12 ? 128 : 129;
        if (kind != 11 && kind != 12 && kind != 14)
            continue;
        const size_t channel = (size_t)control->event.track * 16 + (control->event.status & 15u);
        ChipEvent *event = &state[channel * 130 + key];
        if (at >= (Sint64)event->tick)
        {
            *event = control->event;
            event->tick = (Uint64)at;
        }
    }
    const int saved = r->part;
    r->part = 0;
    bool ok = Control(r, 0, 0, 0, 0, tempo);
    for (size_t i = 0; i < channels * 130 && ok; ++i)
    {
        const ChipEvent *event = &state[i];
        if (!event->status)
            continue;
        r->part = event->track;
        ok = Control(r, 0, event->status, event->a, event->b, 0);
    }
    r->part = saved;
    SDL_free(state);
    SDL_free(used);
    return ok;
}

static bool ExpandOrder(ScoreReader *r)
{
    for (int m = 0; m < r->measures; ++m)
    {
        if (!r->navigation[m].repeat_count || !r->navigation[m].endings)
            continue;
        Uint32 passes = r->navigation[m].endings;
        for (int next = m + 1; next < r->measures && r->navigation[next].endings; ++next)
            passes |= r->navigation[next].endings;
        int total = 0;
        for (; passes; passes >>= 1)
            ++total;
        r->navigation[m].repeat_count = SDL_max(r->navigation[m].repeat_count, total);
    }
    int *order = NULL, count = 0;
    if (!Chip_ScoreOrder(r->navigation, r->measures, &order, &count))
        return false;
    size_t *first_note = SDL_malloc((size_t)r->measures * sizeof(*first_note));
    size_t *first_control = SDL_malloc((size_t)r->measures * sizeof(*first_control));
    size_t *next_note = SDL_malloc(SDL_max(r->note_count, 1) * sizeof(*next_note));
    size_t *next_control = SDL_malloc(SDL_max(r->control_count, 1) * sizeof(*next_control));
    Sint64 *positions = SDL_calloc((size_t)r->measures + 1, sizeof(*positions));
    Sint64 *lengths = SDL_calloc(SCORE_MAX_MEASURES, sizeof(*lengths));
    r->song->measures = SDL_calloc((size_t)SDL_max(count, 1), sizeof(*r->song->measures));
    bool ok = false;
    ScoreNote *notes = NULL;
    ScoreControl *controls = NULL;
    if (!first_note || !first_control || !next_note || !next_control || !positions || !lengths ||
        !r->song->measures)
        goto done;
    for (int m = 0; m < r->measures; ++m)
    {
        first_note[m] = first_control[m] = SIZE_MAX;
        positions[m + 1] = positions[m] + r->lengths[m];
    }
    for (size_t i = r->note_count; i > 0; --i)
    {
        const int measure = r->notes[i - 1].measure;
        next_note[i - 1] = first_note[measure];
        first_note[measure] = i - 1;
    }
    for (size_t i = r->control_count; i > 0; --i)
    {
        const int measure = r->controls[i - 1].measure;
        next_control[i - 1] = first_control[measure];
        first_control[measure] = i - 1;
    }
    notes = r->notes;
    controls = r->controls;
    const size_t source_controls = r->control_count;
    r->notes = NULL;
    r->note_count = r->note_capacity = 0;
    r->controls = NULL;
    r->control_count = r->control_capacity = 0;
    for (int visit = 0; visit < count; ++visit)
    {
        const int source = order[visit];
        r->measure = visit;
        lengths[visit] = r->lengths[source];
        r->song->measures[visit].source = source;
        if (visit && source != order[visit - 1] + 1 &&
            !RestoreControls(r, controls, source_controls, positions, source))
            goto done;
        for (size_t i = first_note[source]; i != SIZE_MAX; i = next_note[i])
        {
            if (notes[i].skipped)
                continue;
            if (!Grow((void **)&r->notes, &r->note_capacity, r->note_count, sizeof(*r->notes)))
                goto done;
            r->notes[r->note_count] = notes[i];
            r->notes[r->note_count++].measure = visit;
        }
        for (size_t i = first_control[source]; i != SIZE_MAX; i = next_control[i])
        {
            if (!Grow((void **)&r->controls, &r->control_capacity, r->control_count,
                      sizeof(*r->controls)))
                goto done;
            r->controls[r->control_count] = controls[i];
            r->controls[r->control_count++].measure = visit;
        }
    }
    SDL_free(r->lengths);
    r->lengths = lengths;
    lengths = NULL;
    r->measures = count;
    r->song->measure_count = count;
    ok = true;
done:
    SDL_free(order);
    SDL_free(first_note);
    SDL_free(first_control);
    SDL_free(next_note);
    SDL_free(next_control);
    SDL_free(positions);
    SDL_free(lengths);
    SDL_free(notes);
    SDL_free(controls);
    return ok;
}

static int SDLCALL CompareControls(const void *left, const void *right)
{
    const ScoreControl *a = left, *b = right;
    if (a->measure != b->measure)
        return a->measure < b->measure ? -1 : 1;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return (a->event.order > b->event.order) - (a->event.order < b->event.order);
}

static bool Compile(ScoreReader *r)
{
    if (r->control_count)
        SDL_qsort(r->controls, r->control_count, sizeof(*r->controls), CompareControls);
    const ScoreControl *previous_tempo = NULL;
    for (size_t i = 0; i < r->control_count; ++i)
    {
        const ScoreControl *control = &r->controls[i];
        if (!control->event.tempo)
            continue;
        if (previous_tempo && previous_tempo->measure == control->measure &&
            previous_tempo->start == control->start &&
            previous_tempo->event.tempo != control->event.tempo)
            return Error(r, NULL, "conflicting simultaneous global tempos");
        previous_tempo = control;
    }
    if (!RemoveMirrors(r) || !ExpandOrder(r))
        return false;
    Sint64 position = 0;
    for (int m = 0; m < r->measures; ++m)
    {
        const Sint64 length = r->lengths[m];
        r->lengths[m] = position;
        r->song->measures[m].start = (Uint64)position;
        position += length;
        r->song->measures[m].end = (Uint64)position;
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
        if (n->skipped || n->velocity == 0)
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
    r.navigation = SDL_calloc(SCORE_MAX_MEASURES, sizeof(*r.navigation));
    if (!r.song || !r.lengths || !r.navigation)
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
        r.ending = 0;
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
    SDL_free(r.navigation);
    if (r.failed)
    {
        Grapple_DestroyChipSong(r.song);
        return NULL;
    }
    return r.song;
}
