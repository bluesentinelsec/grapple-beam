/* Original Grapple code (zlib). MusicXML score-to-performance compiler. */
#include "chip_score.h"

int Chip_ScoreInteger(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback,
                      int low, int high);

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

Uint32 Chip_ScorePasses(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    Uint32 result = 0;
    while (*text)
    {
        char *end;
        const long first = SDL_strtol(text, &end, 10);
        if (end == text || first < 1 || first > 32)
        {
            Chip_ScoreError(r, node, "invalid ending/time-only pass list");
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
                Chip_ScoreError(r, node, "invalid ending pass range");
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
            Chip_ScoreError(r, node, "invalid pass-list separator");
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
                    return Chip_ScoreError(r, repeat, "repeat beyond measure limit");
                r->navigation[index].repeat_start = true;
            }
            else if (SDL_strcmp(direction, "backward") == 0)
            {
                if (SDL_strcmp(location, "left") == 0)
                    --index;
                if (index < 0)
                    return Chip_ScoreError(r, repeat, "backward repeat before score start");
                const int times =
                    Chip_ScoreInteger(r, repeat, Chip_XmlAttribute(repeat, "times"), 2, 1, 32);
                if (r->navigation[index].repeat_count && r->navigation[index].repeat_count != times)
                    return Chip_ScoreError(r, repeat, "conflicting repeat counts across parts");
                r->navigation[index].repeat_count = times;
                r->navigation[index].repeat_after_jump |=
                    SDL_strcmp(Chip_XmlAttribute(repeat, "after-jump"), "yes") == 0;
            }
            else
                return Chip_ScoreError(r, repeat, "invalid repeat direction");
        }
        const ChipXmlNode *ending = Chip_XmlChild(c, "ending");
        if (ending)
        {
            const char *type = Chip_XmlAttribute(ending, "type");
            if (SDL_strcmp(type, "start") == 0)
                r->ending = Chip_ScorePasses(r, ending, Chip_XmlAttribute(ending, "number"));
            else if (SDL_strcmp(type, "stop") == 0 || SDL_strcmp(type, "discontinue") == 0)
                close_ending = true;
            else
                return Chip_ScoreError(r, ending, "invalid ending type");
        }
    }
    ChipScoreMeasure *target = &r->navigation[r->measure];
    if (r->ending)
    {
        if (target->endings && target->endings != r->ending)
            return Chip_ScoreError(r, measure, "conflicting endings across parts");
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
                return Chip_ScoreError(r, sound, "conflicting score navigation labels");
            *labels[i].value = value;
        }
    }
    m->dacapo |= SDL_strcmp(Chip_XmlAttribute(sound, "dacapo"), "yes") == 0;
    m->fine |= *Chip_XmlAttribute(sound, "fine") != 0;
    m->repeat_start |= SDL_strcmp(Chip_XmlAttribute(sound, "forward-repeat"), "yes") == 0;
    const char *passes = Chip_XmlAttribute(sound, "time-only");
    if (*passes && (m->dacapo || m->dalsegno || m->tocoda))
        m->jump_times = Chip_ScorePasses(r, sound, passes);
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

int Chip_ScoreInteger(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback,
                      int low, int high)
{
    if (!*text)
        return fallback;
    ScoreNumber number;
    if (!Number(text, &number) || number.denominator != 1 || number.numerator < low ||
        number.numerator > high)
    {
        Chip_ScoreError(r, node, "invalid integer value");
        return fallback;
    }
    return (int)number.numerator;
}

double Chip_ScoreDecimal(ScoreReader *r, const ChipXmlNode *node, const char *text, double fallback,
                         double low, double high)
{
    if (!*text)
        return fallback;
    ScoreNumber number;
    if (!Number(text, &number))
    {
        Chip_ScoreError(r, node, "invalid decimal value");
        return fallback;
    }
    const double value = (double)number.numerator / (double)number.denominator;
    if (value < low || value > high)
    {
        Chip_ScoreError(r, node, "decimal value out of range");
        return fallback;
    }
    return value;
}

static bool Resolution(ScoreReader *r, const ChipXmlNode *node, Sint64 *divisions,
                       Sint64 *fractions)
{
    for (; node; node = node->next)
    {
        for (size_t a = 0; node->attributes && node->attributes[a]; a += 2)
        {
            if (SDL_strcmp(node->attributes[a], "attack") != 0 &&
                SDL_strcmp(node->attributes[a], "release") != 0 &&
                SDL_strcmp(node->attributes[a], "make-time") != 0)
                continue;
            ScoreNumber n;
            if (!Number(node->attributes[a + 1], &n) ||
                n.denominator / Gcd(n.denominator, *fractions) > SCORE_MAX_RESOLUTION / *fractions)
                return Chip_ScoreError(r, node, "invalid or excessive note timing precision");
            *fractions *= n.denominator / Gcd(n.denominator, *fractions);
        }
        if (Named(node, "grace") || Named(node, "ornaments") || Named(node, "arpeggiate") ||
            Named(node, "swing") || Named(node, "metronome") || Named(node, "wedge") ||
            Named(node, "words") || Named(node, "fermata") || Named(node, "caesura") ||
            Named(node, "staccato") || Named(node, "staccatissimo") ||
            Named(node, "detached-legato") || Named(node, "stopped") || Named(node, "notehead") ||
            Named(node, "other-technical"))
        {
            const Sint64 factor = 1000 / Gcd(1000, *fractions);
            if (factor > SCORE_MAX_RESOLUTION / *fractions)
                return Chip_ScoreError(r, node, "articulation timing resolution exceeds limit");
            *fractions *= factor;
        }
        if (Named(node, "swing") || Named(node, "words") || Named(node, "metronome"))
        {
            const int first =
                Named(node, "swing")
                    ? Chip_ScoreInteger(r, node, Chip_XmlText(node, "first"), 2, 1, 1000)
                    : 2;
            const int second =
                Named(node, "swing")
                    ? Chip_ScoreInteger(r, node, Chip_XmlText(node, "second"), 1, 1, 1000)
                    : 1;
            const Sint64 factor = (first + second) / Gcd(first + second, *fractions);
            if (factor > SCORE_MAX_RESOLUTION / *fractions)
                return Chip_ScoreError(r, node, "swing precision exceeds resolution limit");
            *fractions *= factor;
        }
        if (Named(node, "divisions") || Named(node, "duration") || Named(node, "offset") ||
            Named(node, "beat-type"))
        {
            ScoreNumber n;
            if (!node->text || !Number(node->text, &n))
                return Chip_ScoreError(r, node, "invalid score duration");
            const bool divisor = Named(node, "divisions") || Named(node, "beat-type");
            Sint64 value = divisor ? n.numerator : n.denominator;
            Sint64 *scale = divisor ? divisions : fractions;
            if (value <= 0 || value / Gcd(value, *scale) > SCORE_MAX_RESOLUTION / *scale)
                return Chip_ScoreError(r, node, "exact timing resolution exceeds resource limit");
            *scale *= value / Gcd(value, *scale);
        }
        if (!Resolution(r, node->children, divisions, fractions))
            return false;
    }
    return true;
}

Sint64 Chip_ScoreTicks(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    ScoreNumber n;
    if (!Number(text, &n))
    {
        Chip_ScoreError(r, node, "missing or invalid duration");
        return 0;
    }
    const Sint64 scale = r->song->info.ticks_per_quarter / r->divisions.numerator;
    const Sint64 multiplier = scale * r->divisions.denominator;
    const Sint64 absolute = n.numerator < 0 ? -n.numerator : n.numerator;
    if (multiplier <= 0 || absolute > SDL_MAX_SINT64 / multiplier)
    {
        Chip_ScoreError(r, node, "duration overflow");
        return 0;
    }
    const Sint64 product = n.numerator * multiplier;
    if (product % n.denominator)
    {
        Chip_ScoreError(r, node, "inexact score duration");
        return 0;
    }
    const Sint64 result = product / n.denominator;
    if (result > 864000LL * r->song->info.ticks_per_quarter ||
        result < -864000LL * r->song->info.ticks_per_quarter)
    {
        Chip_ScoreError(r, node, "duration exceeds score limit");
        return 0;
    }
    return result;
}

bool Chip_ScoreGrow(void **array, size_t *capacity, size_t count, size_t item_size)
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

bool Chip_ScoreControl(ScoreReader *r, Sint64 start, int status, int a, int b, Uint32 tempo)
{
    if (!Chip_ScoreGrow((void **)&r->controls, &r->control_capacity, r->control_count,
                        sizeof(*r->controls)))
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
    return Chip_ScoreInteger(r, node, text, 1, 1, SCORE_MAX_STAVES) - 1;
}

static bool Attributes(ScoreReader *r, const ChipXmlNode *node)
{
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (Named(c, "key"))
        {
            const int fifths = Chip_ScoreInteger(r, c, Chip_XmlText(c, "fifths"), 0, -7, 7);
            static const int sharp[] = {3, 0, 4, 1, 5, 2, 6};
            static const int flat[] = {6, 2, 5, 1, 4, 0, 3};
            const char *number = Chip_XmlAttribute(c, "number");
            const int first = *number ? Staff(r, c, number) : 0;
            const int last = *number ? first + 1 : SCORE_MAX_STAVES;
            for (int staff = first; staff < last; ++staff)
            {
                SDL_memset(r->key[staff], 0, sizeof(r->key[staff]));
                for (int k = 0; k < SDL_abs(fifths); ++k)
                    r->key[staff][fifths > 0 ? sharp[k] : flat[k]] = fifths > 0 ? 1 : -1;
            }
        }
        else if (Named(c, "divisions"))
        {
            if (!Number(c->text ? c->text : "", &r->divisions) || r->divisions.numerator <= 0)
                return Chip_ScoreError(r, c, "divisions must be positive");
        }
        else if (Named(c, "transpose"))
        {
            const int amount =
                Chip_ScoreInteger(r, c, Chip_XmlText(c, "chromatic"), 0, -127, 127) +
                12 * Chip_ScoreInteger(r, c, Chip_XmlText(c, "octave-change"), 0, -10, 10);
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
            if (Chip_XmlChild(c, "senza-misura"))
            {
                r->meter = 0;
                continue;
            }
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
                            return Chip_ScoreError(r, t, "invalid additive meter");
                        beats += (int)value;
                        while (SDL_isspace((unsigned char)*end))
                            ++end;
                        if (!*end)
                            break;
                        if (*end != '+')
                            return Chip_ScoreError(r, t, "invalid additive meter");
                        text = end + 1;
                    }
                }
                else if (Named(t, "beat-type"))
                {
                    const int unit = Chip_ScoreInteger(r, t, t->text ? t->text : "", 4, 1, 4096);
                    meter += (Sint64)r->song->info.ticks_per_quarter * 4 * beats / unit;
                }
            }
            if (meter)
                r->meter = meter;
        }
    }
    return !r->failed;
}

int Chip_ScoreDynamic(const char *name)
{
    static const char *const names[] = {"pppp", "ppp", "pp", "p",   "mp",
                                        "mf",   "f",   "ff", "fff", "ffff"};
    static const int velocities[] = {16, 24, 36, 48, 64, 80, 96, 108, 120, 127};
    for (size_t i = 0; i < SDL_arraysize(names); ++i)
        if (SDL_strcmp(names[i], name) == 0)
            return velocities[i];
    if (SDL_strcmp(name, "ppppp") == 0)
        return 10;
    if (SDL_strcmp(name, "pppppp") == 0)
        return 6;
    if (SDL_strcmp(name, "fffff") == 0 || SDL_strcmp(name, "ffffff") == 0)
        return 127;
    return 80;
}

static bool Direction(ScoreReader *r, const ChipXmlNode *node, Sint64 cursor)
{
    const ChipXmlNode *sound = Named(node, "sound") ? node : Chip_XmlChild(node, "sound");
    const ChipXmlNode *offset = Chip_XmlChild(sound, "offset");
    if (!offset)
        offset = Chip_XmlChild(node, "offset");
    if (offset)
        cursor += Chip_ScoreTicks(r, offset, offset->text ? offset->text : "");
    if (cursor < 0)
        return Chip_ScoreError(r, node, "direction before measure start");
    if (!NavigationSound(r, sound))
        return false;
    if (!Chip_ReadInstrumentChange(r, sound, cursor))
        return false;
    const char *tempo = Chip_XmlAttribute(sound, "tempo");
    if (*tempo)
    {
        const double bpm = Chip_ScoreDecimal(r, sound, tempo, 120, 20, 600);
        if (!Chip_ScoreControl(r, cursor, 0, 0, 0, (Uint32)SDL_round(60000000 / bpm)))
            return false;
    }
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (!Named(c, "direction-type"))
            continue;
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
        if (SDL_strcmp(instruction, "dacapo") == 0 ||
            SDL_strcmp(instruction, "dacapoalfine") == 0 ||
            SDL_strcmp(instruction, "dacapoalcoda") == 0 || SDL_strcmp(instruction, "dc") == 0 ||
            SDL_strcmp(instruction, "dcalfine") == 0 || SDL_strcmp(instruction, "dcalcoda") == 0)
            navigation->dacapo = true;
        if (!navigation->dalsegno &&
            (SDL_strcmp(instruction, "dalsegno") == 0 ||
             SDL_strcmp(instruction, "dalsegnoalfine") == 0 ||
             SDL_strcmp(instruction, "dalsegnoalcoda") == 0 || SDL_strcmp(instruction, "ds") == 0 ||
             SDL_strcmp(instruction, "dsalfine") == 0 || SDL_strcmp(instruction, "dsalcoda") == 0))
            navigation->dalsegno = "default";
        if (!navigation->tocoda && SDL_strcmp(instruction, "tocoda") == 0)
            navigation->tocoda = "default";
        if (SDL_strcmp(instruction, "fine") == 0)
            navigation->fine = true;
        const ChipXmlNode *metronome = Chip_XmlChild(c, "metronome");
        if (metronome && !*tempo && !Chip_ReadMetronome(r, metronome, cursor))
            return false;
    }
    return !r->failed && Chip_ReadDirections(r, node, sound, cursor);
}

static bool Note(ScoreReader *r, const ChipXmlNode *node, Sint64 *cursor, Sint64 *previous,
                 Sint64 *extent)
{
    const ChipXmlNode *grace = Chip_XmlChild(node, "grace");
    const Sint64 duration = grace ? 0 : Chip_ScoreTicks(r, node, Chip_XmlText(node, "duration"));
    if (r->failed || (!grace && duration <= 0))
        return Chip_ScoreError(r, node, "note duration must be positive");
    const bool chord = Chip_XmlChild(node, "chord") != NULL;
    const Sint64 start = chord ? *previous : *cursor;
    if (start > 864000LL * r->song->info.ticks_per_quarter - duration)
        return Chip_ScoreError(r, node, "measure duration exceeds score limit");
    if (!chord)
    {
        *previous = start;
        *cursor += duration;
    }
    *extent = SDL_max(*extent, start + duration);
    if (Chip_XmlChild(node, "cue"))
        return true;
    ScoreNote note = {0};
    note.part = r->part;
    note.grace = grace != NULL;
    note.rest = Chip_XmlChild(node, "rest") != NULL;
    if (grace && *Chip_XmlAttribute(grace, "make-time"))
    {
        note.make_time = Chip_ScoreTicks(r, grace, Chip_XmlAttribute(grace, "make-time"));
        if (note.make_time <= 0)
            return Chip_ScoreError(r, grace, "grace make-time must be positive");
    }
    note.node = node;
    note.source_measure = r->measure;
    note.gate = 1;
    Grapple_GetChipExpressionDefaults(&note.expression);
    note.measure = r->measure;
    note.staff = Staff(r, node, Chip_XmlText(node, "staff"));
    note.tab = r->tab[note.staff];
    note.start = start;
    note.duration = duration;
    note.velocity = r->velocity[note.staff];
    const char *dynamics = Chip_XmlAttribute(node, "dynamics");
    if (*dynamics)
        note.velocity =
            SDL_min(127, (int)SDL_round(Chip_ScoreDecimal(r, node, dynamics, 100, 0, 1000) * 0.9));
    note.voice = Chip_XmlText(node, "voice");
    note.instrument = Chip_XmlAttribute(Chip_XmlChild(node, "instrument"), "id");
    const ScoreInstrument *instrument = Chip_ScoreInstrument(r, node, note.instrument);
    if (!instrument)
        return false;
    note.instrument = instrument->id;
    note.channel = instrument->channel;
    note.program = instrument->program;
    note.instrument_gain = instrument->gain;
    note.instrument_pan = instrument->pan;
    const ChipXmlNode *pitch = Chip_XmlChild(node, "pitch");
    if (pitch)
    {
        if (!*Chip_XmlText(pitch, "octave"))
            return Chip_ScoreError(r, pitch, "pitch is missing its octave");
        const char *step = Chip_XmlText(pitch, "step");
        static const char letters[] = "C D EF G A B";
        const char *letter = *step ? SDL_strchr(letters, *step) : NULL;
        if (!letter || step[1] || *step == ' ')
            return Chip_ScoreError(r, pitch, "invalid pitch step");
        note.pitch =
            (int)(letter - letters) +
            12 * (Chip_ScoreInteger(r, pitch, Chip_XmlText(pitch, "octave"), 4, 0, 9) + 1) +
            r->transpose[note.staff];
        const double alteration =
            Chip_ScoreDecimal(r, pitch, Chip_XmlText(pitch, "alter"), 0, -12, 12);
        const int integral = (int)SDL_floor(alteration);
        note.pitch += integral;
        note.expression.tuning = (float)(alteration - integral);
        static const char degrees[] = "CDEFGAB";
        const char *degree = SDL_strchr(degrees, *step);
        const int index = (int)(degree - degrees);
        static const int natural[] = {0, 2, 4, 5, 7, 9, 11};
        const int base = note.pitch - (int)(letter - letters) - integral;
        note.upper_pitch = base + natural[(index + 1) % 7] + (index == 6 ? 12 : 0) +
                           r->key[note.staff][(index + 1) % 7];
        note.lower_pitch = base + natural[(index + 6) % 7] - (index == 0 ? 12 : 0) +
                           r->key[note.staff][(index + 6) % 7];
    }
    else if (Chip_XmlChild(node, "unpitched") && instrument->unpitched >= 0)
    {
        note.pitch = instrument->unpitched;
        note.unpitched = true;
    }
    else if (!note.rest)
        return Chip_ScoreFail(r, node, GRAPPLE_CHIP_DIAGNOSTIC_EXPORTER_OMISSION,
                              "note needs pitch or mapped percussion instrument");
    if (note.pitch < 0 || note.pitch > 127)
        return Chip_ScoreError(r, node, "sounding pitch outside 0..127");
    for (const ChipXmlNode *c = node->children; c; c = c->next)
        if (Named(c, "tie"))
        {
            const char *type = Chip_XmlAttribute(c, "type");
            note.tie_start |= SDL_strcmp(type, "start") == 0;
            note.tie_stop |= SDL_strcmp(type, "stop") == 0;
        }
    if (!Chip_ReadNoteExpression(r, &note))
        return false;
    if (!Chip_ScoreGrow((void **)&r->notes, &r->note_capacity, r->note_count, sizeof(*r->notes)))
        return false;
    r->notes[r->note_count++] = note;
    return !r->failed;
}

static bool Measure(ScoreReader *r, const ChipXmlNode *node)
{
    if (!Navigation(r, node))
        return false;
    if (node && !Chip_CheckScorePerformance(r, node->children))
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
            const Sint64 delta = Chip_ScoreTicks(r, c, Chip_XmlText(c, "duration"));
            if (delta <= 0)
                return Chip_ScoreError(r, c, "cursor movement must be positive");
            cursor += Named(c, "backup") ? -delta : delta;
            if (cursor < 0 || cursor > 864000LL * r->song->info.ticks_per_quarter)
                return Chip_ScoreError(r, c, "cursor outside measure limits");
            extent = SDL_max(extent, cursor);
        }
        else if (Named(c, "direction") || Named(c, "sound"))
        {
            if (!Direction(r, c, cursor))
                return false;
        }
    }
    if (!extent && node)
    {
        if (!r->meter)
            return Chip_ScoreError(r, node, "empty unmetered measure needs explicit durations");
        extent = r->meter;
    }
    r->lengths[r->measure] = SDL_max(r->lengths[r->measure], extent);
    return !r->failed;
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
        Chip_ScoreError(&r, root, "expected score-partwise or score-timewise");
        goto done;
    }
    const ChipXmlNode *list = Chip_XmlChild(root, "part-list");
    int parts = 0;
    for (const ChipXmlNode *c = list ? list->children : NULL; c; c = c->next)
        if (Named(c, "score-part"))
            ++parts;
    if (!parts || parts > CHIP_SONG_MAX_TRACKS)
    {
        Chip_ScoreError(&r, root, "expected 1..256 score parts");
        goto done;
    }
    if (!Chip_ValidateScoreReferences(&r, list))
        goto done;
    Sint64 divisions = 1, fractions = 1;
    if (!Resolution(&r, root, &divisions, &fractions))
        goto done;
    if (divisions > SCORE_MAX_RESOLUTION / fractions)
    {
        Chip_ScoreError(&r, root, "exact timing resolution exceeds resource limit");
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
        SDL_zero(r.key);
        r.meter = (Sint64)r.song->info.ticks_per_quarter * 4;
        SDL_zero(r.transpose);
        SDL_zero(r.tab);
        for (int s = 0; s < SCORE_MAX_STAVES; ++s)
            r.velocity[s] = 80;
        r.measure = 0;
        r.ending = 0;
        SDL_strlcpy(r.song->tracks[r.part].name, Chip_XmlText(definition, "part-name"),
                    sizeof(r.song->tracks[r.part].name));
        if (!Chip_InitScoreInstruments(&r))
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
                Chip_ScoreFail(&r, definition, GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE,
                               "missing part contents");
                goto done;
            }
        }
        for (const ChipXmlNode *measure = container->children; measure; measure = measure->next)
        {
            if (!Named(measure, "measure"))
                continue;
            if (r.measure == SCORE_MAX_MEASURES)
            {
                Chip_ScoreError(&r, measure, "measure limit exceeded");
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
    if (!r.measures || !Chip_CompileScore(&r))
        r.failed = true;
done:
    Chip_XmlDestroy(root);
    SDL_free(r.notes);
    SDL_free(r.controls);
    SDL_free(r.lengths);
    SDL_free(r.navigation);
    SDL_free(r.instruments);
    SDL_free(r.instrument_changes);
    if (r.failed)
    {
        Grapple_DestroyChipSong(r.song);
        return NULL;
    }
    return r.song;
}
