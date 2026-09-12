/* Original Grapple code (zlib). Private notation compiler state. */
#ifndef GRAPPLE_CHIP_SCORE_H
#define GRAPPLE_CHIP_SCORE_H
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
    const ChipXmlNode *node;
    int source_measure;
    Grapple_ChipExpression expression;
    double gate;
    Sint64 attack, release;
    bool grace, rest;
    Sint64 make_time;
    double upper_pitch, lower_pitch;
} ScoreNote;
typedef enum ScoreDirectionKind
{
    SCORE_CONTROL,
    SCORE_DYNAMIC,
    SCORE_WEDGE,
    SCORE_PEDAL,
    SCORE_SWING,
    SCORE_TEMPO_RAMP
} ScoreDirectionKind;
typedef struct ScoreControl
{
    int measure;
    Sint64 start;
    ChipEvent event;
    ScoreDirectionKind kind;
    int staff, number;
    const char *voice;
    const ChipXmlNode *node;
    Sint64 duration;
    double value, target;
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
    int key[SCORE_MAX_STAVES][7];
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

bool Chip_ScoreError(ScoreReader *r, const ChipXmlNode *node, const char *message);
bool Chip_ScoreGrow(void **array, size_t *capacity, size_t count, size_t item_size);
bool Chip_ScoreControl(ScoreReader *r, Sint64 start, int status, int a, int b, Uint32 tempo);
int Chip_ScoreInteger(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback,
                      int low, int high);
double Chip_ScoreDecimal(ScoreReader *r, const ChipXmlNode *node, const char *text, double fallback,
                         double low, double high);
Sint64 Chip_ScoreTicks(ScoreReader *r, const ChipXmlNode *node, const char *text);
bool Chip_ScoreApproximation(ScoreReader *r, const ScoreNote *note, const char *message);
bool Chip_ReadNoteExpression(ScoreReader *r, ScoreNote *note);
bool Chip_ResolveNoteExpression(ScoreReader *r);
bool Chip_ApplyScoreTiming(ScoreReader *r);
bool Chip_ExpandOrnaments(ScoreReader *r);
bool Chip_ReadDirections(ScoreReader *r, const ChipXmlNode *node, const ChipXmlNode *sound,
                         Sint64 cursor);
bool Chip_PrepareDirections(ScoreReader *r);
bool Chip_RestoreDirections(ScoreReader *r, const ScoreControl *controls, size_t count,
                            const Sint64 *positions, int source);
bool Chip_ApplySwing(ScoreReader *r);
bool Chip_ApplyDirections(ScoreReader *r);
int Chip_ScoreDynamic(const char *name);
bool Chip_CompileScore(ScoreReader *r);
#endif
