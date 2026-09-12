/* Original Grapple code (zlib). Notation-to-synth expression policies. */
#include "chip_score.h"

bool Chip_ScoreApproximation(ScoreReader *r, const ScoreNote *note, const char *message)
{
    for (int i = 0; i < r->song->diagnostic_count; ++i)
        if (r->song->diagnostics[i].code == GRAPPLE_CHIP_DIAGNOSTIC_APPROXIMATION &&
            r->song->diagnostics[i].part == note->part &&
            SDL_strcmp(r->song->diagnostic_messages[i], message) == 0)
            return true;
    Grapple_ChipDiagnostic diagnostic = {GRAPPLE_CHIP_DIAGNOSTIC_APPROXIMATION,
                                         GRAPPLE_CHIP_DIAGNOSTIC_INFO,
                                         note->part,
                                         note->source_measure,
                                         note->staff + 1,
                                         (Uint32)note->node->line,
                                         "",
                                         ""};
    Chip_DiagnosticContext(&diagnostic, note->node);
    return Chip_AddDiagnostic(r->song, diagnostic, message);
}

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

static bool ReadNotation(ScoreReader *r, ScoreNote *n, const ChipXmlNode *node)
{
    Grapple_ChipExpression *e = &n->expression;
    int bends = 0;
    bool release = false;
    for (const ChipXmlNode *c = node; c; c = c->next)
    {
        if (Named(c, "staccato"))
            n->gate = SDL_min(n->gate, r->options->staccato_gate);
        else if (Named(c, "staccatissimo") || Named(c, "spiccato"))
            n->gate = SDL_min(n->gate, r->options->staccatissimo_gate);
        else if (Named(c, "detached-legato"))
            n->gate = SDL_min(n->gate, r->options->portato_gate);
        else if (Named(c, "tenuto"))
            n->gate = SDL_min(n->gate, 1);
        else if (Named(c, "unstress") || Named(c, "soft-accent"))
            n->velocity = SDL_max(1, n->velocity - 12);
        else if (Named(c, "accent") || Named(c, "strong-accent") || Named(c, "stress"))
        {
            if (!*Chip_XmlAttribute(n->node, "dynamics"))
                n->velocity = SDL_min(127, n->velocity + (Named(c, "strong-accent") ? 26 : 16));
        }
        else if (Named(c, "bend"))
        {
            if (++bends > 2)
                return Chip_ScoreError(r, c, "more than two bend segments on a note");
            const float amount =
                (float)Chip_ScoreDecimal(r, c, Chip_XmlText(c, "bend-alter"), 0, -48, 48);
            e->bend_accelerate |= SDL_strcmp(Chip_XmlAttribute(c, "accelerate"), "yes") == 0;
            const bool prebend = Chip_XmlChild(c, "pre-bend") != NULL;
            release |= Chip_XmlChild(c, "release") != NULL;
            if (bends == 1)
            {
                e->bend_first =
                    (float)Chip_ScoreDecimal(r, c, Chip_XmlAttribute(c, "first-beat"), 25, 0, 100) /
                    100;
                e->bend_last =
                    (float)Chip_ScoreDecimal(r, c, Chip_XmlAttribute(c, "last-beat"), 75, 0, 100) /
                    100;
                e->bend_middle = e->bend_last;
                e->bend_start = prebend || release ? amount : 0;
                e->bend_peak = release ? 0 : amount;
                e->bend_end = e->bend_peak;
            }
            else
            {
                if (!release)
                    return Chip_ScoreError(r, c, "second bend must release the first bend");
                e->bend_middle = (e->bend_first + e->bend_last) * 0.5f;
                e->bend_end = 0;
            }
        }
        else if (Named(c, "scoop") || Named(c, "plop") || Named(c, "doit") || Named(c, "falloff"))
        {
            const bool ending = Named(c, "doit") || Named(c, "falloff");
            const float interval = Named(c, "plop") || Named(c, "falloff") ? -2.0f : 2.0f;
            e->bend_start = ending ? 0 : -interval;
            e->bend_peak = e->bend_end = ending ? interval : 0;
            e->bend_first = ending ? 0.75f : 0;
            e->bend_middle = e->bend_last = ending ? 1 : 0.25f;
        }
        else if (Named(c, "harmonic"))
        {
            e->brightness = 1.5f;
            e->gain *= 0.7f;
        }
        else if (Named(c, "up-bow") || Named(c, "down-bow") || Named(c, "pluck"))
            e->brightness = Named(c, "up-bow") ? 1.1f : 1.2f;
        else if (Named(c, "half-muted") || Named(c, "harmon-mute"))
            e->brightness = 0.6f;
        else if (Named(c, "fingernails") || Named(c, "snap-pizzicato") || Named(c, "tap"))
        {
            e->brightness = 1.6f;
            n->velocity = SDL_min(127, n->velocity + 12);
        }
        else if (Named(c, "stopped") || Named(c, "palm-mute"))
        {
            e->brightness = 0.35f;
            n->gate = SDL_min(n->gate, 0.4);
        }
        else if (Named(c, "other-technical") && c->text)
        {
            if (SDL_strcasecmp(c->text, "vibrato") == 0)
                e->vibrato_depth = 0.35f;
            else if (SDL_strcasecmp(c->text, "slap") == 0 || SDL_strcasecmp(c->text, "pop") == 0)
                e->brightness = 1.7f;
            else if (SDL_strcasecmp(c->text, "let ring") == 0 ||
                     SDL_strcasecmp(c->text, "let-ring") == 0)
                n->let_ring = true;
            else if (SDL_strcasecmp(c->text, "wide vibrato") == 0)
                e->vibrato_depth = 0.7f;
            else if (SDL_strcasecmp(c->text, "ghost note") == 0)
                e->gain *= 0.6f;
            else if (SDL_strcasecmp(c->text, "dead note") == 0)
            {
                e->noise = 0.85f;
                n->gate = 0.2;
            }
            else if (SDL_strcasecmp(c->text, "rasgueado") == 0 ||
                     SDL_strcasecmp(c->text, "upstroke") == 0 ||
                     SDL_strcasecmp(c->text, "downstroke") == 0)
            {
                e->brightness = 1.5f;
                e->noise = 0.15f;
            }
            else if (SDL_strcasecmp(c->text, "palm mute") == 0)
            {
                e->brightness = 0.35f;
                n->gate = SDL_min(n->gate, 0.4);
            }
            else if (!Chip_ScoreUnsupported(
                         r, c, "Unrecognized technical text; encode pitch/timing explicitly"))
                return false;
        }
        if ((Named(c, "harmonic") || Named(c, "bend") || Named(c, "other-technical") ||
             Named(c, "up-bow") || Named(c, "down-bow") || Named(c, "scoop") || Named(c, "plop") ||
             Named(c, "doit") || Named(c, "falloff")) &&
            !Chip_ScoreApproximation(
                r, n, "Instrument gestures use the documented chiptune timbre/curve policy"))
            return false;
        if (!Named(c, "bend") && !ReadNotation(r, n, c->children))
            return false;
    }
    return !r->failed;
}

bool Chip_ReadNoteExpression(ScoreReader *r, ScoreNote *note)
{
    const ChipXmlNode *technical =
        Chip_XmlChild(Chip_XmlChild(note->node, "notations"), "technical");
    note->string_number =
        Chip_ScoreInteger(r, technical, Chip_XmlText(technical, "string"), 0, 0, 32);
    const char *attack = Chip_XmlAttribute(note->node, "attack");
    const char *release = Chip_XmlAttribute(note->node, "release");
    if (*attack)
        note->attack = Chip_ScoreTicks(r, note->node, attack);
    if (*release)
        note->release = Chip_ScoreTicks(r, note->node, release);
    const ChipXmlNode *head = Chip_XmlChild(note->node, "notehead");
    if (head && (SDL_strcmp(Chip_XmlAttribute(head, "parentheses"), "yes") == 0))
        note->expression.gain *= 0.6f;
    if (head && head->text &&
        (SDL_strcmp(head->text, "x") == 0 || SDL_strcmp(head->text, "cross") == 0) &&
        !Chip_XmlChild(note->node, "unpitched"))
    {
        note->expression.noise = 0.85f;
        note->gate = 0.2;
    }
    if (SDL_strcmp(Chip_XmlAttribute(note->node, "pizzicato"), "yes") == 0)
    {
        note->expression.brightness = 1.4f;
        note->gate = SDL_min(note->gate, 0.6);
    }
    return ReadNotation(r, note, Chip_XmlChild(note->node, "notations")) &&
           Chip_ValidExpression(&note->expression);
}

typedef struct ScoreLine
{
    int part, staff;
    const char *voice, *instrument;
    Uint32 slurs;
    size_t slides[16];
    bool stepped[16];
    const ChipXmlNode *trill;
} ScoreLine;

static bool Connect(ScoreReader *r, ScoreNote *n, ScoreLine *line, const ChipXmlNode *node,
                    size_t index)
{
    for (const ChipXmlNode *c = node; c; c = c->next)
    {
        if (Named(c, "wavy-line"))
        {
            const char *type = Chip_XmlAttribute(c, "type");
            if (SDL_strcmp(type, "start") == 0)
            {
                line->trill = Chip_XmlChild(c->parent, "trill-mark");
                if (!line->trill)
                    line->trill = c;
                n->ornament_override = line->trill;
            }
            else if (SDL_strcmp(type, "stop") == 0)
                line->trill = NULL;
        }
        else if (Named(c, "slur") || Named(c, "hammer-on") || Named(c, "pull-off"))
        {
            const int number =
                Chip_ScoreInteger(r, c, Chip_XmlAttribute(c, "number"), 1, 1, 16) - 1;
            const Uint32 bit = 1u << number;
            const char *type = Chip_XmlAttribute(c, "type");
            if (SDL_strcmp(type, "stop") == 0)
            {
                n->expression.legato = true;
                line->slurs &= ~bit;
            }
            else if (SDL_strcmp(type, "start") == 0)
                line->slurs |= bit;
        }
        else if (Named(c, "slide") || Named(c, "glissando"))
        {
            const int number =
                Chip_ScoreInteger(r, c, Chip_XmlAttribute(c, "number"), 1, 1, 16) - 1;
            const char *type = Chip_XmlAttribute(c, "type");
            if (SDL_strcmp(type, "start") == 0)
            {
                if (line->slides[number])
                    return Chip_ScoreError(r, c, "overlapping slide with the same number");
                line->slides[number] = index + 1;
                line->stepped[number] = Named(c, "glissando");
            }
            else if (SDL_strcmp(type, "stop") == 0)
            {
                if (!line->slides[number])
                    return Chip_ScoreError(r, c, "slide stop without matching start");
                ScoreNote *a = &r->notes[line->slides[number] - 1];
                if (n->start <= a->start)
                    return Chip_ScoreError(r, c, "slide endpoint must follow its start");
                a->duration = n->start - a->start;
                a->gate = 1;
                a->expression.bend_first = 0.5f;
                a->expression.bend_middle = 1;
                a->expression.bend_last = 1;
                a->expression.bend_peak =
                    (float)(n->pitch - a->pitch) + n->expression.tuning - a->expression.tuning;
                a->expression.bend_end = a->expression.bend_peak;
                a->expression.stepped_pitch = line->stepped[number];
                if (!Chip_ValidExpression(&a->expression))
                    return Chip_ScoreError(r, c, "slide exceeds synthesis pitch range");
                n->expression.legato = true;
                line->slides[number] = 0;
            }
        }
        if (!Connect(r, n, line, c->children, index))
            return false;
    }
    return !r->failed;
}

bool Chip_ResolveNoteExpression(ScoreReader *r)
{
    ScoreLine *lines = SDL_calloc(4096, sizeof(*lines));
    if (!lines)
        return false;
    size_t count = 0;
    bool ok = true;
    for (size_t i = 0; ok && i < r->note_count; ++i)
    {
        ScoreNote *n = &r->notes[i];
        if (n->skipped)
            continue;
        r->part = n->part;
        r->measure = n->source_measure;
        size_t lane;
        for (lane = 0; lane < count; ++lane)
            if (lines[lane].part == n->part && lines[lane].staff == n->staff &&
                SDL_strcmp(lines[lane].voice, n->voice) == 0 &&
                SDL_strcmp(lines[lane].instrument, n->instrument) == 0)
                break;
        if (lane == count)
        {
            if (count == 4096)
            {
                ok = Chip_ScoreFail(r, n->node, GRAPPLE_CHIP_DIAGNOSTIC_RESOURCE,
                                    "logical voice limit exceeded");
                break;
            }
            lines[count].part = n->part;
            lines[count].staff = n->staff;
            lines[count].voice = n->voice;
            lines[count++].instrument = n->instrument;
        }
        n->ornament_override = lines[lane].trill;
        n->expression.lane = (int)lane;
        n->expression.legato = lines[lane].slurs != 0;
        ok = Connect(r, n, &lines[lane], Chip_XmlChild(n->node, "notations"), i);
    }
    for (size_t i = 0; ok && i < count; ++i)
        for (int n = 0; ok && n < 16; ++n)
            if (lines[i].slides[n])
                ok = Chip_ScoreError(r, NULL, "unterminated slide");
    SDL_free(lines);
    return ok;
}
