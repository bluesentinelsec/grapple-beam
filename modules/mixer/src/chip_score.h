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
typedef struct ScoreInstrument
{
    const char *id;
    const ChipXmlNode *node;
    int channel, program, unpitched;
    float gain, pan;
} ScoreInstrument;
typedef struct ScoreInstrumentChange
{
    int part, measure;
    Sint64 start;
    const ChipXmlNode *node;
    ScoreInstrument instrument;
} ScoreInstrumentChange;
typedef struct ScoreCurve
{
    Sint64 start, duration;
    Grapple_ChipExpression expression;
    size_t next;
} ScoreCurve;
typedef struct ScoreNote
{
    int part, measure, staff, pitch, velocity, channel;
    Sint64 start, duration;
    const char *voice;
    const char *instrument;
    bool tab, skipped, tie_start, tie_stop, tied_continuation;
    const ChipXmlNode *node;
    int source_measure;
    Grapple_ChipExpression expression;
    double gate;
    Sint64 attack, release;
    bool grace, rest;
    bool let_ring;
    int string_number;
    size_t curve_first, curve_last;
    const ChipXmlNode *ornament_override;
    Sint64 make_time;
    double upper_pitch, lower_pitch;
    int program;
    bool unpitched;
    float instrument_gain, instrument_pan;
} ScoreNote;
typedef enum ScoreDirectionKind
{
    SCORE_CONTROL,
    SCORE_DYNAMIC,
    SCORE_WEDGE,
    SCORE_PEDAL,
    SCORE_SWING,
    SCORE_TEMPO_RAMP,
    SCORE_METRIC,
    SCORE_SECTION,
    SCORE_NAVIGATION_BOUNDARY
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
    ScoreCurve *curves;
    size_t curve_count, curve_capacity;
    ScoreControl *controls;
    size_t control_count, control_capacity;
    Sint64 *lengths;
    int measures;
    ChipScoreMeasure *navigation;
    Uint32 ending;
    ScoreInstrument *instruments;
    int instrument_count;
    const char *default_instrument;
    ScoreInstrumentChange *instrument_changes;
    size_t instrument_change_count, instrument_change_capacity;
} ScoreReader;

void Chip_DiagnosticContext(Grapple_ChipDiagnostic *diagnostic, const ChipXmlNode *node);
bool Chip_ScoreFail(ScoreReader *r, const ChipXmlNode *node, Grapple_ChipDiagnosticCode code,
                    const char *message);
bool Chip_CheckScorePerformance(ScoreReader *r, const ChipXmlNode *node);
bool Chip_ScoreUnsupported(ScoreReader *r, const ChipXmlNode *node, const char *message);
bool Chip_ScoreWarn(ScoreReader *r, const ChipXmlNode *node, Grapple_ChipDiagnosticCode code,
                    const char *message);
bool Chip_ValidateScoreReferences(ScoreReader *r, const ChipXmlNode *list);
bool Chip_ScoreError(ScoreReader *r, const ChipXmlNode *node, const char *message);
bool Chip_ScoreGrow(void **array, size_t *capacity, size_t count, size_t item_size);
bool Chip_ScoreControl(ScoreReader *r, Sint64 start, int status, int a, int b, Uint32 tempo);
int Chip_ScoreInteger(ScoreReader *r, const ChipXmlNode *node, const char *text, int fallback,
                      int low, int high);
double Chip_ScoreDecimal(ScoreReader *r, const ChipXmlNode *node, const char *text, double fallback,
                         double low, double high);
Sint64 Chip_ScoreTicks(ScoreReader *r, const ChipXmlNode *node, const char *text);
bool Chip_ScoreApproximation(ScoreReader *r, const ScoreNote *note, const char *message);
bool Chip_ResolveTies(ScoreReader *r);
bool Chip_EmitTieCurves(ScoreReader *r, const ScoreNote *note, ChipEvent event);
bool Chip_ReadNoteExpression(ScoreReader *r, ScoreNote *note);
bool Chip_ResolveTechniques(ScoreReader *r);
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
bool Chip_InitScoreInstruments(ScoreReader *r);
const ScoreInstrument *Chip_ScoreInstrument(ScoreReader *r, const ChipXmlNode *node,
                                            const char *id);
bool Chip_ReadInstrumentChange(ScoreReader *r, const ChipXmlNode *sound, Sint64 cursor);
bool Chip_ApplyInstrumentChanges(ScoreReader *r);
bool Chip_ResolveMetronomes(ScoreReader *r);
bool Chip_ReadMetronome(ScoreReader *r, const ChipXmlNode *node, Sint64 cursor);
Uint32 Chip_ScorePasses(ScoreReader *r, const ChipXmlNode *node, const char *text);
bool Chip_CompileScore(ScoreReader *r);
#endif
