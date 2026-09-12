/* Original Grapple code (zlib). Source diagnostics and score identity validation. */
#include "chip_score.h"

static bool Named(const ChipXmlNode *node, const char *name)
{
    return node && SDL_strcmp(node->name, name) == 0;
}

void Chip_DiagnosticContext(Grapple_ChipDiagnostic *diagnostic, const ChipXmlNode *node)
{
    if (!node)
        return;
    diagnostic->line = (Uint32)node->line;
    SDL_strlcpy(diagnostic->element, node->name, sizeof(diagnostic->element));
    for (const ChipXmlNode *parent = node; parent; parent = parent->parent)
        if (Named(parent, "note") || Named(parent, "direction"))
        {
            SDL_strlcpy(diagnostic->voice, Chip_XmlText(parent, "voice"),
                        sizeof(diagnostic->voice));
            const char *staff = Chip_XmlText(parent, "staff");
            const int number = *staff ? SDL_atoi(staff) : 1;
            diagnostic->staff = number >= 1 && number <= SCORE_MAX_STAVES ? number : 0;
            break;
        }
}

bool Chip_ScoreFail(ScoreReader *r, const ChipXmlNode *node, Grapple_ChipDiagnosticCode code,
                    const char *message)
{
    if (r->failed)
        return false;
    r->failed = true;
    if (r->error && r->error->code == GRAPPLE_CHIP_DIAGNOSTIC_NONE)
    {
        *r->error = (Grapple_ChipDiagnostic){
            code, GRAPPLE_CHIP_DIAGNOSTIC_ERROR, r->part, r->measure, 0, 0, "", ""};
        Chip_DiagnosticContext(r->error, node);
    }
    return SDL_SetError("MusicXML: part %d measure %d line %lu <%s>: %s", r->part + 1,
                        r->measure + 1, node ? node->line : 0, node ? node->name : "score",
                        message);
}

bool Chip_ScoreError(ScoreReader *r, const ChipXmlNode *node, const char *message)
{
    return Chip_ScoreFail(r, node, GRAPPLE_CHIP_DIAGNOSTIC_SCORE, message);
}

bool Chip_ScoreUnsupported(ScoreReader *r, const ChipXmlNode *node, const char *message)
{
    char fallback[256];
    if (!message)
    {
        SDL_snprintf(fallback, sizeof(fallback), "Playback of <%s> is not implemented", node->name);
        message = fallback;
    }
    if (r->options->strict)
        return Chip_ScoreFail(r, node, GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED, message);
    Grapple_ChipDiagnostic diagnostic = {GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED,
                                         GRAPPLE_CHIP_DIAGNOSTIC_WARNING,
                                         r->part,
                                         r->measure,
                                         0,
                                         0,
                                         "",
                                         ""};
    Chip_DiagnosticContext(&diagnostic, node);
    return Chip_AddDiagnostic(r->song, diagnostic, message);
}

static int PartIndex(const char *const *ids, int count, const char *id)
{
    for (int i = 0; i < count; ++i)
        if (SDL_strcmp(ids[i], id) == 0)
            return i;
    return -1;
}

static bool ValidatePartContents(ScoreReader *r, const ChipXmlNode *parent, const char *const *ids,
                                 int count)
{
    bool seen[CHIP_SONG_MAX_TRACKS] = {false};
    for (const ChipXmlNode *c = parent->children; c; c = c->next)
        if (Named(c, "part"))
        {
            const int index = PartIndex(ids, count, Chip_XmlAttribute(c, "id"));
            if (index < 0 || seen[index])
                return Chip_ScoreFail(r, c, GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE,
                                      index < 0 ? "part contents reference an unknown ID"
                                                : "duplicate part contents ID");
            seen[index] = true;
        }
    return true;
}

bool Chip_ValidateScoreReferences(ScoreReader *r, const ChipXmlNode *list)
{
    const char *ids[CHIP_SONG_MAX_TRACKS];
    int count = 0;
    for (const ChipXmlNode *c = list->children; c; c = c->next)
        if (Named(c, "score-part"))
        {
            const char *id = Chip_XmlAttribute(c, "id");
            r->part = count;
            if (!*id || PartIndex(ids, count, id) >= 0)
                return Chip_ScoreFail(r, c, GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE,
                                      "missing or duplicate score-part ID");
            if (count == CHIP_SONG_MAX_TRACKS)
                return Chip_ScoreFail(r, c, GRAPPLE_CHIP_DIAGNOSTIC_RESOURCE,
                                      "part limit exceeded (256)");
            ids[count++] = id;
        }
    r->part = 0;
    if (Named(r->root, "score-partwise"))
        return ValidatePartContents(r, r->root, ids, count);
    for (const ChipXmlNode *m = r->root->children; m; m = m->next)
        if (Named(m, "measure") && !ValidatePartContents(r, m, ids, count))
            return false;
    return true;
}

static bool Allowed(const char *list, const char *name)
{
    char token[128];
    if (SDL_strlen(name) + 3 > sizeof(token))
        return false;
    SDL_snprintf(token, sizeof(token), "|%s|", name);
    return SDL_strstr(list, token) != NULL;
}

bool Chip_CheckScorePerformance(ScoreReader *r, const ChipXmlNode *node)
{
    for (; node; node = node->next)
    {
        const char *allowed = NULL;
        if (Named(node->parent, "technical"))
            allowed = "|up-bow|down-bow|harmonic|open-string|thumb-position|fingering|pluck|"
                      "stopped|snap-pizzicato|fret|string|hammer-on|pull-off|bend|tap|heel|toe|"
                      "fingernails|open|half-muted|harmon-mute|other-technical|";
        else if (Named(node->parent, "ornaments"))
            allowed =
                "|trill-mark|turn|delayed-turn|inverted-turn|delayed-inverted-turn|"
                "vertical-turn|inverted-vertical-turn|shake|wavy-line|mordent|inverted-mordent|"
                "haydn|tremolo|accidental-mark|";
        else if (Named(node->parent, "articulations"))
            allowed =
                "|accent|strong-accent|staccato|tenuto|detached-legato|staccatissimo|spiccato|"
                "scoop|plop|doit|falloff|breath-mark|caesura|stress|unstress|soft-accent|";
        else if (Named(node->parent, "direction-type"))
            allowed =
                "|rehearsal|segno|coda|words|symbol|wedge|dynamics|dashes|bracket|pedal|"
                "metronome|octave-shift|harp-pedals|eyeglasses|scordatura|image|principal-voice|"
                "percussion|accordion-registration|staff-divide|";
        if ((allowed && !Allowed(allowed, node->name)) || Named(node, "other-notation"))
        {
            if (!Chip_ScoreUnsupported(r, node, NULL))
                return false;
            continue;
        }
        if (!Chip_CheckScorePerformance(r, node->children))
            return false;
    }
    return true;
}
