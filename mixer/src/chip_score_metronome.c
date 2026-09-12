/* Original Grapple code (zlib). Notated beat units and metric relationships. */
#include "chip_score.h"

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static double Unit(ScoreReader *r, const ChipXmlNode *node, const char *text)
{
    static const char *const names[] = {"maxima",  "long",   "breve", "whole", "half",
                                        "quarter", "eighth", "16th",  "32nd",  "64th",
                                        "128th",   "256th",  "512th", "1024th"};
    double duration = 32;
    for (size_t i = 0; i < SDL_arraysize(names); ++i, duration *= 0.5)
        if (SDL_strcmp(text, names[i]) == 0)
            return duration;
    Chip_ScoreError(r, node, "unknown metronome beat unit");
    return 0;
}

static double MetronomeNote(ScoreReader *r, const ChipXmlNode *node, double *ratio)
{
    double duration = Unit(r, node, Chip_XmlText(node, "metronome-type"));
    double dot = duration / 2;
    bool stop = false;
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (Named(c, "metronome-dot"))
        {
            duration += dot;
            dot /= 2;
        }
        else if (Named(c, "metronome-tuplet"))
        {
            const int actual = Chip_ScoreInteger(r, c, Chip_XmlText(c, "actual-notes"), 3, 1, 1000);
            const int normal = Chip_ScoreInteger(r, c, Chip_XmlText(c, "normal-notes"), 2, 1, 1000);
            if (SDL_strcmp(Chip_XmlAttribute(c, "type"), "stop") == 0)
                stop = true;
            else
                *ratio = (double)normal / actual;
        }
    }
    duration *= *ratio;
    if (stop)
        *ratio = 1;
    return duration;
}

bool Chip_ReadMetronome(ScoreReader *r, const ChipXmlNode *node, Sint64 cursor)
{
    double sides[2] = {0}, notes[2][16] = {{0}};
    int counts[2] = {0};
    double ratios[2] = {1, 1};
    int side = 0, units = 0;
    double dot = 0;
    for (const ChipXmlNode *c = node->children; c; c = c->next)
    {
        if (Named(c, "beat-unit"))
        {
            if (++units > 2)
                return Chip_ScoreError(r, c, "too many metronome beat units");
            side = units - 1;
            sides[side] = Unit(r, c, c->text ? c->text : "");
            dot = sides[side] / 2;
        }
        else if (Named(c, "beat-unit-dot"))
        {
            sides[side] += dot;
            dot /= 2;
        }
        else if (Named(c, "beat-unit-tied"))
        {
            double tied = Unit(r, c, Chip_XmlText(c, "beat-unit"));
            double tied_dot = tied / 2;
            for (const ChipXmlNode *d = c->children; d; d = d->next)
                if (Named(d, "beat-unit-dot"))
                {
                    tied += tied_dot;
                    tied_dot /= 2;
                }
            sides[side] += tied;
        }
        else if (Named(c, "metronome-relation"))
        {
            if (side || !c->text ||
                (SDL_strcmp(c->text, "equals") != 0 && SDL_strcmp(c->text, "=") != 0))
                return Chip_ScoreError(r, c, "metronome relation must be equality");
            side = 1;
        }
        else if (Named(c, "metronome-note"))
        {
            if (counts[side] == 16)
                return Chip_ScoreError(r, c, "metronome relationship exceeds 16 notes per side");
            const double duration = MetronomeNote(r, c, &ratios[side]);
            notes[side][counts[side]++] = duration;
            sides[side] += duration;
        }
    }
    if (r->failed)
        return false;
    const char *per_minute = Chip_XmlText(node, "per-minute");
    if (*per_minute)
    {
        const double bpm = Chip_ScoreDecimal(r, node, per_minute, 120, 1, 100000) * sides[0];
        if (bpm < 20 || bpm > 600)
            return Chip_ScoreError(r, node, "tempo outside 20..600 quarter beats/minute");
        return Chip_ScoreControl(r, cursor, 0, 0, 0, (Uint32)SDL_round(60000000 / bpm));
    }
    if (sides[0] <= 0 || sides[1] <= 0)
        return Chip_ScoreError(r, node, "metronome needs a tempo or complete metric relationship");
    ScoreControl control = {0};
    control.event.track = (Uint16)r->part;
    control.event.order = (Uint32)r->control_count;
    control.measure = r->measure;
    control.start = cursor;
    control.node = node;
    control.staff = -1;
    control.voice = "";
    control.kind = SCORE_METRIC;
    control.value = sides[1] / sides[0];
    if (counts[0] == 2 && counts[1] == 2 && notes[0][0] == notes[0][1] &&
        (notes[0][0] == 0.5 || notes[0][0] == 0.25) && SDL_fabs(sides[0] - sides[1]) < 1e-9)
    {
        control.kind = SCORE_SWING;
        control.value = notes[1][0] / sides[1];
        control.target = notes[0][0];
    }
    if (!Chip_ScoreGrow((void **)&r->controls, &r->control_capacity, r->control_count,
                        sizeof(*r->controls)))
        return false;
    r->controls[r->control_count++] = control;
    return true;
}

bool Chip_ResolveMetronomes(ScoreReader *r)
{
    Uint32 tempo = 500000;
    for (size_t begin = 0; begin < r->control_count;)
    {
        size_t end = begin + 1;
        while (end < r->control_count &&
               r->controls[end].event.tick == r->controls[begin].event.tick)
            ++end;
        Uint32 explicit_tempo = 0;
        double ratio = 0;
        for (size_t i = begin; i < end; ++i)
        {
            const ScoreControl *c = &r->controls[i];
            if (c->event.tempo)
                explicit_tempo = c->event.tempo;
            if (c->kind == SCORE_METRIC)
            {
                if (ratio != 0 && ratio != c->value)
                    return Chip_ScoreError(r, c->node,
                                           "conflicting simultaneous metric modulations");
                ratio = c->value;
            }
        }
        if (explicit_tempo)
            tempo = explicit_tempo;
        else if (ratio != 0)
        {
            const double bpm = 60000000.0 / tempo * ratio;
            if (bpm < 20 || bpm > 600)
                return Chip_ScoreError(r, r->controls[begin].node,
                                       "metric modulation tempo outside 20..600 BPM");
            tempo = (Uint32)SDL_round(60000000 / bpm);
        }
        for (size_t i = begin; i < end; ++i)
            if (r->controls[i].kind == SCORE_METRIC)
            {
                r->controls[i].event.tempo = tempo;
                r->controls[i].kind = SCORE_CONTROL;
            }
        begin = end;
    }
    return true;
}
