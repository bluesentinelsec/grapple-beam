#include "chip_internal.h"

#include <algorithm>
#include <cmath>
#include <grapple/chiptune.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace
{
using Song = std::unique_ptr<Grapple_ChipSong, decltype(&Grapple_DestroyChipSong)>;
using Player = std::unique_ptr<Grapple_ChipPlayer, decltype(&Grapple_DestroyChipPlayer)>;
Song LoadXml(const std::string &xml)
{
    return Song(Grapple_LoadChipSong_IO(SDL_IOFromConstMem(xml.data(), xml.size()), true),
                Grapple_DestroyChipSong);
}
std::string Score(const std::string &measures)
{
    return "<score-partwise version='4.0'><part-list><score-part id='P1'>"
           "<part-name>harmony</part-name></score-part></part-list><part id='P1'>" +
           measures + "</part></score-partwise>";
}
std::string Note(const std::string &duration, const std::string &extra = "")
{
    return "<note><pitch><step>C</step><octave>4</octave></pitch><duration>" + duration +
           "</duration>" + extra + "</note>";
}
std::vector<ChipEvent> Onsets(const Grapple_ChipSong *song, int track = -1)
{
    std::vector<ChipEvent> events;
    for (size_t i = 0; i < song->count; ++i)
        if ((song->events[i].status >> 4) == 9 && song->events[i].b &&
            (track == -1 || song->events[i].track == track))
            events.push_back(song->events[i]);
    return events;
}
} // namespace

TEST(ChipMusicXml, GuitarProPerformanceMatchesMidiNotesPitchesAndTiming)
{
    const Song xml(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml"),
                   Grapple_DestroyChipSong);
    ASSERT_TRUE(xml) << SDL_GetError();
    const Song midi(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.mid"),
                    Grapple_DestroyChipSong);
    ASSERT_TRUE(midi);
    EXPECT_EQ(xml->info.track_count, 4);
    EXPECT_DOUBLE_EQ(xml->info.duration_seconds, 10);
    const char *names[] = {"melody", "harmony", "bass", "percussion"};
    for (int track = 0; track < 4; ++track)
    {
        EXPECT_STREQ(xml->tracks[track].name, names[track]);
        const auto expected = Onsets(midi.get(), track + 1);
        const auto actual = Onsets(xml.get(), track);
        ASSERT_EQ(actual.size(), expected.size()) << track;
        for (size_t i = 0; i < actual.size(); ++i)
        {
            EXPECT_EQ(actual[i].a, expected[i].a) << track << ':' << i;
            EXPECT_EQ(actual[i].tick * static_cast<Uint64>(midi->info.ticks_per_quarter),
                      expected[i].tick * static_cast<Uint64>(xml->info.ticks_per_quarter));
        }
    }
    ASSERT_TRUE(SDL_Init(0));
    const Player player(Grapple_CreateChipPlayer(xml.get(), 8000, 64, false),
                        Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player) << SDL_GetError();
    std::vector<float> samples(8000 * 2 * 14);
    EXPECT_GT(Grapple_RenderChipPlayer(player.get(), samples.data(), 8000 * 14), 80000);
    double energy = 0;
    for (float sample : samples)
    {
        ASSERT_TRUE(std::isfinite(sample));
        energy += sample * sample;
    }
    EXPECT_GT(energy, 1);
    EXPECT_GE(Grapple_GetChipPlayerPeakVoices(player.get()), 8);
}

TEST(ChipMusicXml, TupletsChangingDivisionsAndDecimalDurationsAreExact)
{
    const auto song = LoadXml(
        Score("<measure implicit='yes'><attributes><divisions>3</divisions></attributes>" +
              Note("1", "<time-modification><actual-notes>3</actual-notes><normal-notes>2</"
                        "normal-notes></time-modification>") +
              Note("1") + Note("1") +
              "</measure><measure><attributes><divisions>7</divisions></attributes>" + Note("1") +
              Note("1") + "</measure><measure><attributes><divisions>1</divisions></attributes>" +
              Note("0.25") + Note("0.25") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 7u);
    const Uint64 q = static_cast<Uint64>(song->info.ticks_per_quarter);
    EXPECT_EQ(notes[1].tick * 3, q);
    EXPECT_EQ(notes[2].tick * 3, 2 * q);
    EXPECT_EQ(notes[3].tick, q);
    EXPECT_EQ(notes[4].tick * 7, 8 * q);
    EXPECT_EQ(notes[5].tick * 7, 9 * q);
    EXPECT_EQ((notes[6].tick - notes[5].tick) * 4, q);
}

TEST(ChipMusicXml, ChordsVoicesTiesAndPickupsPreservePerformance)
{
    const auto song = LoadXml(
        Score("<measure implicit='yes'><attributes><divisions>1</divisions></attributes>" +
              Note("1", "<voice>upper</voice><tie type='start'/>") +
              "<backup><duration>1</duration></backup>" + Note("1", "<voice>lower</voice>") +
              "</measure><measure>" + Note("1", "<voice>upper</voice><tie type='stop'/>") +
              Note("1", "<chord/><voice>upper</voice>") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_EQ(Onsets(song.get()).size(), 3u);
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 1);
    const auto notes = Onsets(song.get());
    EXPECT_EQ(notes[0].tick, 0u);
    EXPECT_EQ(notes[1].tick, 0u);
    EXPECT_EQ(notes[2].tick, static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_NE(notes[0].note_id, notes[1].note_id);
}

TEST(ChipMusicXml, TimewiseScoresAndDottedMetronomeTempoWork)
{
    const auto song = LoadXml(
        "<score-timewise><part-list><score-part id='P'><part-name>lead</part-name>"
        "</score-part></part-list><measure><part id='P'><attributes><divisions>1</divisions>"
        "</attributes><direction><direction-type><metronome><beat-unit>half</beat-unit>"
        "<beat-unit-dot/><per-minute>60</per-minute></metronome></direction-type></direction>" +
        Note("3") + "</part></measure></score-timewise>");
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_NEAR(song->info.duration_seconds, 1, 0.000002);
}

TEST(ChipMusicXml, HighResolutionFrameConversionDoesNotOverflow)
{
    const Song song(Chip_NewSong(1, 100000000), Grapple_DestroyChipSong);
    ASSERT_TRUE(song);
    const Uint64 second = 100000000000000ULL;
    const Uint64 time = second * 86399 + second / 8;
    EXPECT_EQ(Chip_TimeToFrame(song.get(), time, 192000), 16588632000ULL);
}

TEST(ChipMusicXml, DistinctTabStavesAndIndependentUnisonsArePreserved)
{
    const auto song = LoadXml(
        Score("<measure><attributes><divisions>1</divisions><clef number='2'><sign>TAB</sign>"
              "</clef></attributes>" +
              Note("1", "<staff>1</staff>") + "<backup><duration>1</duration></backup>" +
              Note("2", "<staff>2</staff>") + "<backup><duration>2</duration></backup>" +
              Note("1", "<staff>3</staff>") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_EQ(Onsets(song.get()).size(), 3u);
}

TEST(ChipMusicXml, RejectsInvalidTimingPitchAndInstrumentReferences)
{
    EXPECT_FALSE(LoadXml(Score("<measure><backup><duration>1</duration></backup></measure>")));
    EXPECT_FALSE(LoadXml(Score("<measure>" + Note("0") + "</measure>")));
    EXPECT_FALSE(
        LoadXml(Score("<measure>" + Note("1", "<instrument id='missing'/>") + "</measure>")));
    EXPECT_FALSE(LoadXml(Score("<measure>" + Note("1", "<tie type='stop'/>") + "</measure>")));
    EXPECT_FALSE(LoadXml(Score("<measure><attributes><divisions>100000001</divisions>"
                               "</attributes>" +
                               Note("1") + "</measure>")));
}

TEST(ChipMusicXml, MoreThanSixteenPartsHaveIndependentInstrumentPrograms)
{
    std::string definitions, contents;
    for (int i = 0; i < 20; ++i)
    {
        const std::string id = "P" + std::to_string(i);
        definitions += "<score-part id='" + id + "'><part-name>Instrument " + std::to_string(i) +
                       "</part-name><midi-instrument id='" + id +
                       "'><midi-channel>1</midi-channel><midi-program>" +
                       std::to_string(i % 2 ? 81 : 33) +
                       "</midi-program></midi-instrument></score-part>";
        contents += "<part id='" + id +
                    "'><measure><attributes><divisions>1</divisions></attributes>" + Note("1") +
                    "</measure></part>";
    }
    const auto song = LoadXml("<score-partwise><part-list>" + definitions + "</part-list>" +
                              contents + "</score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    ASSERT_TRUE(SDL_Init(0));
    Player actual(Grapple_CreateChipPlayer(song.get(), 8000, 64, false), Grapple_DestroyChipPlayer);
    Player expected(Grapple_CreateChipPlayer(song.get(), 8000, 64, false),
                    Grapple_DestroyChipPlayer);
    ASSERT_TRUE(actual);
    ASSERT_TRUE(expected);
    for (int i = 0; i < 20; ++i)
        ASSERT_TRUE(Grapple_SetChipTrackPreset(
            expected.get(), i, i % 2 ? GRAPPLE_CHIP_PRESET_LEAD : GRAPPLE_CHIP_PRESET_BASS, 1));
    std::vector<float> a(4000 * 2), b(4000 * 2);
    ASSERT_EQ(Grapple_RenderChipPlayer(actual.get(), a.data(), 4000), 4000);
    ASSERT_EQ(Grapple_RenderChipPlayer(expected.get(), b.data(), 4000), 4000);
    EXPECT_EQ(a, b);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(actual.get()), 20);
}

TEST(ChipMusicXml, CompressedAndStoredMxlResolveContainerScoreAndMatchXml)
{
    const Song expected(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml"),
                        Grapple_DestroyChipSong);
    ASSERT_TRUE(expected);
    for (const char *name : {"/c64-composition.mxl", "/c64-composition-stored.mxl"})
    {
        const Song actual(Grapple_LoadChipSong((std::string(MIXER_TEST_ASSETS_DIR) + name).c_str()),
                          Grapple_DestroyChipSong);
        ASSERT_TRUE(actual) << SDL_GetError();
        EXPECT_EQ(actual->info.duration_ticks, expected->info.duration_ticks);
        const auto a = Onsets(actual.get()), b = Onsets(expected.get());
        ASSERT_EQ(a.size(), b.size());
        for (size_t i = 0; i < a.size(); ++i)
        {
            EXPECT_EQ(a[i].tick, b[i].tick);
            EXPECT_EQ(a[i].track, b[i].track);
            EXPECT_EQ(a[i].a, b[i].a);
        }
    }
}

TEST(ChipMusicXml, CorruptMxlFailsWithoutFilesystemExtraction)
{
    size_t size = 0;
    void *input = SDL_LoadFile(MIXER_TEST_ASSETS_DIR "/c64-composition.mxl", &size);
    ASSERT_NE(input, nullptr);
    const std::string bytes(static_cast<const char *>(input), size);
    SDL_free(input);
    EXPECT_FALSE(LoadXml(bytes.substr(0, size - 12)));
    auto changed = bytes;
    changed[14] ^= 0x20; // local-header CRC no longer agrees with the directory
    EXPECT_FALSE(LoadXml(changed));
    changed = bytes;
    changed[6] = 1; // encryption conflicts with central-directory flags
    EXPECT_FALSE(LoadXml(changed));
    changed = bytes;
    const auto path = changed.find("scores/demo.musicxml");
    ASSERT_NE(path, std::string::npos);
    changed.replace(path, 6, "../bad");
    EXPECT_FALSE(LoadXml(changed));
}

TEST(ChipMusicXml, StrictAndPermissiveModesExposeStructuredDiagnostics)
{
    const auto xml = Score("<measure>" +
                           Note("1", "<notations><other-notation type='single'>"
                                     "unknown technique</other-notation></notations>") +
                           "</measure>");
    Grapple_ChipImportOptions options;
    ASSERT_TRUE(Grapple_GetChipImportDefaults(&options));
    Grapple_ChipDiagnostic error;
    Song strict(Grapple_LoadChipSong_IOEx(SDL_IOFromConstMem(xml.data(), xml.size()), true,
                                          &options, &error),
                Grapple_DestroyChipSong);
    EXPECT_FALSE(strict);
    EXPECT_EQ(error.code, GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED);
    EXPECT_EQ(error.part, 0);
    EXPECT_EQ(error.measure, 0);
    EXPECT_GT(error.line, 0u);
    options.strict = false;
    Song permissive(Grapple_LoadChipSong_IOEx(SDL_IOFromConstMem(xml.data(), xml.size()), true,
                                              &options, &error),
                    Grapple_DestroyChipSong);
    ASSERT_TRUE(permissive) << SDL_GetError();
    EXPECT_EQ(error.code, GRAPPLE_CHIP_DIAGNOSTIC_NONE);
    ASSERT_EQ(Grapple_GetChipDiagnosticCount(permissive.get()), 1);
    Grapple_ChipDiagnostic warning;
    ASSERT_TRUE(Grapple_ReadChipDiagnostic(permissive.get(), 0, &warning));
    EXPECT_EQ(warning.severity, GRAPPLE_CHIP_DIAGNOSTIC_WARNING);
    EXPECT_EQ(warning.code, GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED);
    EXPECT_NE(
        std::string(Grapple_GetChipDiagnosticMessage(permissive.get(), 0)).find("other-notation"),
        std::string::npos);
    EXPECT_FALSE(Grapple_ReadChipDiagnostic(permissive.get(), 1, &warning));
}

TEST(ChipMusicXml, StaffSelectionCanKeepMirrorsOrChooseNotationOrTab)
{
    Grapple_ChipImportOptions options;
    ASSERT_TRUE(Grapple_GetChipImportDefaults(&options));
    for (int staff : {-1, 0, 1, 2})
    {
        options.staff = staff;
        Grapple_ChipDiagnostic error;
        const Song song(Grapple_LoadChipSongEx(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml",
                                               &options, &error),
                        Grapple_DestroyChipSong);
        ASSERT_TRUE(song) << SDL_GetError();
        EXPECT_EQ(Onsets(song.get()).size(), staff == -1 ? 142u : 87u);
        EXPECT_EQ(Grapple_GetChipDiagnosticCount(song.get()), staff == 0 ? 3 : 0);
    }
}

TEST(ChipMusicXml, RepeatsRestoreTempoAndExposeSourceMeasureOrder)
{
    const auto song = LoadXml(Score(
        "<measure><attributes><divisions>1</divisions></attributes><sound tempo='90'/>" +
        Note("1") +
        "</measure><measure><barline location='left'><repeat direction='forward'/></barline>" +
        Note("1") + "</measure><measure><sound tempo='60'/>" + Note("1") +
        "<barline><repeat direction='backward'/></barline></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const int expected[] = {0, 1, 2, 1, 2};
    ASSERT_EQ(song->measure_count, 5);
    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(song->measures[i].source, expected[i]);
    EXPECT_EQ(Onsets(song.get()).size(), 5u);
    EXPECT_NEAR(song->info.duration_seconds, 4, 0.000002);
}

TEST(ChipMusicXml, AlternateEndingsAndNestedRepeatsExpandOncePerPass)
{
    const auto endings = LoadXml(Score(
        "<measure><attributes><divisions>1</divisions></attributes>"
        "<barline location='left'><repeat direction='forward'/></barline>" +
        Note("1") +
        "</measure><measure><barline location='left'><ending number='1' type='start'/></barline>" +
        Note("1") +
        "<barline><ending number='1' type='stop'/><repeat "
        "direction='backward'/></barline></measure>"
        "<measure><barline location='left'><ending number='2' type='start'/></barline>" +
        Note("1") + "<barline><ending number='2' type='stop'/></barline></measure>"));
    ASSERT_TRUE(endings) << SDL_GetError();
    ASSERT_EQ(endings->measure_count, 4);
    EXPECT_EQ(endings->measures[0].source, 0);
    EXPECT_EQ(endings->measures[1].source, 1);
    EXPECT_EQ(endings->measures[2].source, 0);
    EXPECT_EQ(endings->measures[3].source, 2);
    const std::string forward = "<barline location='left'><repeat direction='forward'/></barline>";
    const std::string back = "<barline><repeat direction='backward'/></barline>";
    const auto nested =
        LoadXml(Score("<measure>" + forward + Note("1") + "</measure><measure>" + forward +
                      Note("1") + "</measure><measure>" + Note("1") + back + "</measure><measure>" +
                      Note("1") + back + "</measure>"));
    ASSERT_TRUE(nested) << SDL_GetError();
    const int order[] = {0, 1, 2, 1, 2, 3, 0, 1, 2, 1, 2, 3};
    ASSERT_EQ(nested->measure_count, 12);
    for (int i = 0; i < 12; ++i)
        EXPECT_EQ(nested->measures[i].source, order[i]);
}

TEST(ChipMusicXml, DaCapoFineAndDalSegnoCodaUseExplicitNavigation)
{
    const auto dc = LoadXml(Score("<measure>" + Note("1") + "</measure><measure>" + Note("1") +
                                  "<sound fine='yes'/></measure><measure>" + Note("1") +
                                  "<sound dacapo='yes'/></measure>"));
    ASSERT_TRUE(dc) << SDL_GetError();
    ASSERT_EQ(dc->measure_count, 5);
    EXPECT_EQ(dc->measures[4].source, 1);
    const auto ds = LoadXml(Score(
        "<measure><sound segno='S'/>" + Note("1") + "</measure><measure>" + Note("1") +
        "<sound tocoda='C'/></measure><measure>" + Note("1") +
        "<sound dalsegno='S'/></measure><measure><sound coda='C'/>" + Note("1") + "</measure>"));
    ASSERT_TRUE(ds) << SDL_GetError();
    const int order[] = {0, 1, 2, 0, 1, 3};
    ASSERT_EQ(ds->measure_count, 6);
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(ds->measures[i].source, order[i]);
}

TEST(ChipMusicXml, MeasureRepeatAndOctaveShiftDisplayDoNotDuplicateOrTransposeNotes)
{
    const auto song = LoadXml(
        Score("<measure><attributes><measure-style><measure-repeat type='start'>1</measure-repeat>"
              "</measure-style></attributes><direction><direction-type><octave-shift type='down' "
              "size='8'/>"
              "</direction-type></direction>" +
              Note("1") + "</measure><measure>" + Note("1") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].a, 60);
    EXPECT_EQ(notes[1].a, 60);
}

TEST(ChipMusicXml, MissingNavigationTargetsAndUnpairedRepeatStartsFail)
{
    EXPECT_FALSE(LoadXml(Score("<measure>" + Note("1") + "<sound dalsegno='missing'/></measure>")));
    EXPECT_FALSE(LoadXml(Score("<measure><barline location='left'><repeat direction='forward'/>"
                               "</barline>" +
                               Note("1") + "</measure>")));
}

TEST(ChipMusicXml, CommonNavigationWordsAndExplicitJumpPassesAreHonored)
{
    const auto words = LoadXml(Score(
        "<measure>" + Note("1") +
        "<direction><direction-type><words>Fine</words></direction-type></direction></measure>"
        "<measure>" +
        Note("1") +
        "<direction><direction-type><words>D.C. al "
        "Fine</words></direction-type></direction></measure>"));
    ASSERT_TRUE(words) << SDL_GetError();
    EXPECT_EQ(words->measure_count, 3);
    const auto passes =
        LoadXml(Score("<measure>" + Note("1") + "<sound dacapo='yes' time-only='1,2'/></measure>"));
    ASSERT_TRUE(passes) << SDL_GetError();
    EXPECT_EQ(passes->measure_count, 3);
}

TEST(ChipMusicXml, SoundDynamicsUseFortePercentAndExplicitSoundOffsetTakesPriority)
{
    auto note = Note("1");
    note.replace(0, 6, "<note dynamics='50'>");
    const auto song = LoadXml(Score(
        "<measure><attributes><divisions>2</divisions></attributes>"
        "<direction><offset>1</offset><sound tempo='60'><offset>0</offset></sound></direction>" +
        note + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    ASSERT_EQ(Onsets(song.get()).size(), 1u);
    EXPECT_EQ(Onsets(song.get())[0].b, 45);
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.5);
}

TEST(ChipMusicXml, NumberedEndingsDetermineAdditionalRepeatPasses)
{
    const auto song = LoadXml(
        Score("<measure>" + Note("1") +
              "</measure><measure>"
              "<barline location='left'><ending number='1,2' type='start'/></barline>" +
              Note("1") +
              "<barline><ending number='1,2' type='stop'/><repeat "
              "direction='backward'/></barline></measure>"
              "<measure><barline location='left'><ending number='3' type='start'/></barline>" +
              Note("1") + "<barline><ending number='3' type='stop'/></barline></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const int order[] = {0, 1, 0, 1, 0, 2};
    ASSERT_EQ(song->measure_count, 6);
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(song->measures[i].source, order[i]);
}

TEST(ChipMusicXml, MicrotonalBendReleaseAndArticulationAreIndependent)
{
    const auto song = LoadXml(
        Score("<measure><attributes><divisions>1</divisions></attributes>"
              "<note><pitch><step>C</step><alter>0.5</alter><octave>4</octave></pitch><duration>1</"
              "duration>"
              "<notations><technical><bend><bend-alter>2</bend-alter></bend>"
              "<bend><bend-alter>2</bend-alter><release/></bend></technical></notations></note>" +
              Note("1", "<chord/><voice>other</voice><notations><articulations><staccato/><accent/>"
                        "</articulations></notations>") +
              "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    const auto &bend = song->expressions[notes[0].expression - 1];
    const auto &plain = song->expressions[notes[1].expression - 1];
    EXPECT_FLOAT_EQ(bend.tuning, 0.5f);
    EXPECT_FLOAT_EQ(Chip_ExpressionPitch(&bend, 0, 1), 0.5f);
    EXPECT_FLOAT_EQ(Chip_ExpressionPitch(&bend, 0.5, 1), 2.5f);
    EXPECT_FLOAT_EQ(Chip_ExpressionPitch(&bend, 1, 1), 0.5f);
    EXPECT_FLOAT_EQ(Chip_ExpressionPitch(&plain, 0.5, 1), 0);
    EXPECT_GT(notes[1].b, notes[0].b);
    for (size_t i = 0; i < song->count; ++i)
        if ((song->events[i].status >> 4) == 8 && song->events[i].note_id == notes[1].note_id)
            EXPECT_EQ(song->events[i].tick * 2, static_cast<Uint64>(song->info.ticks_per_quarter));
}

TEST(ChipMusicXml, SlidesAndSlursFollowLogicalVoices)
{
    const auto song = LoadXml(
        Score("<measure>" +
              Note("1", "<voice>a</voice><notations><slide type='start'/>"
                        "<slur type='start'/></notations>") +
              "<backup><duration>1</duration></backup>" + Note("2", "<voice>b</voice>") +
              "<backup><duration>1</duration></backup>"
              "<note><pitch><step>E</step><octave>4</octave></pitch><duration>1</"
              "duration><voice>a</voice>"
              "<notations><slide type='stop'/><slur type='stop'/></notations></note></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 3u);
    const auto &slide = song->expressions[notes[0].expression - 1];
    const auto &other = song->expressions[notes[1].expression - 1];
    const auto &target = song->expressions[notes[2].expression - 1];
    EXPECT_FLOAT_EQ(slide.bend_end, 4);
    EXPECT_NE(slide.lane, other.lane);
    EXPECT_EQ(slide.lane, target.lane);
    EXPECT_TRUE(target.legato);
    EXPECT_FALSE(other.legato);
    EXPECT_FALSE(LoadXml(Score(
        "<measure>" + Note("1", "<notations><slide type='stop'/></notations>") + "</measure>")));
}

TEST(ChipMusicXml, ExplicitAttackReleaseOffsetsPreserveFractionalDurations)
{
    auto xml = Score("<measure>" + Note("1") + "</measure>");
    xml.replace(xml.find("<note>"), 6, "<note attack='0.125' release='0.25'>");
    const auto song = LoadXml(xml);
    ASSERT_TRUE(song) << SDL_GetError();
    const auto onsets = Onsets(song.get());
    ASSERT_EQ(onsets.size(), 1u);
    EXPECT_EQ(onsets[0].tick * 8, static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.625);
}

TEST(ChipMusicXml, GraceNotesStealSpecifiedTimeAndKeepTheBarLength)
{
    const std::string grace = "<note><grace steal-time-following='25'/><pitch><step>D</step>"
                              "<octave>4</octave></pitch><voice>1</voice></note>";
    const auto song =
        LoadXml(Score("<measure>" + grace + Note("1", "<voice>1</voice>") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].a, 62);
    EXPECT_EQ(notes[1].a, 60);
    EXPECT_EQ(notes[0].tick, 0u);
    EXPECT_EQ(notes[1].tick * 4, static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_EQ(notes[0].duration * 4, static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.5);

    const auto previous = LoadXml(
        Score("<measure>" + Note("1") +
              "<note><grace steal-time-previous='20'/><pitch><step>D</step><octave>4</octave>"
              "</pitch></note></measure>"));
    ASSERT_TRUE(previous) << SDL_GetError();
    const auto before = Onsets(previous.get());
    ASSERT_EQ(before.size(), 2u);
    EXPECT_EQ(before[1].tick * 5, 4u * static_cast<Uint64>(previous->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(previous->info.duration_seconds, 0.5);
}

TEST(ChipMusicXml, AddedGraceTimeAndFermatasShiftTheEnsembleOnce)
{
    const auto song = LoadXml(
        "<score-partwise><part-list><score-part id='a'><part-name>lead</part-name></score-part>"
        "<score-part id='b'><part-name>bass</part-name></score-part></part-list>"
        "<part id='a'><measure><note><grace make-time='0.25'/><pitch><step>D</step>"
        "<octave>4</octave></pitch></note>" +
        Note("1") +
        "</measure></part>"
        "<part id='b'><measure>" +
        Note("1") + "</measure></part></score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    const auto a = Onsets(song.get(), 0);
    const auto b = Onsets(song.get(), 1);
    ASSERT_EQ(a.size(), 2u);
    ASSERT_EQ(b.size(), 1u);
    EXPECT_EQ(a[0].tick, 0u);
    EXPECT_EQ(a[1].tick, b[0].tick);
    EXPECT_EQ(b[0].tick * 4, static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.625);

    const auto held = LoadXml(Score("<measure>" + Note("1", "<notations><fermata/></notations>") +
                                    Note("1", "<chord/><notations><fermata/></notations>") +
                                    Note("1") + "</measure>"));
    ASSERT_TRUE(held) << SDL_GetError();
    const auto holds = Onsets(held.get());
    ASSERT_EQ(holds.size(), 3u);
    EXPECT_EQ(holds[0].duration * 2, 3u * static_cast<Uint64>(held->info.ticks_per_quarter));
    EXPECT_EQ(holds[2].tick, holds[0].duration);
    EXPECT_DOUBLE_EQ(held->info.duration_seconds, 1.25);
}

TEST(ChipMusicXml, TrillsMordentsTurnsAndTremolosExpandIntoExpectedPitches)
{
    const auto trill = LoadXml(
        Score("<measure><attributes><key><fifths>1</fifths></key></attributes>"
              "<note><pitch><step>E</step><octave>4</octave></pitch><duration>1</duration>"
              "<notations><ornaments><trill-mark/></ornaments></notations></note></measure>"));
    ASSERT_TRUE(trill) << SDL_GetError();
    const auto notes = Onsets(trill.get());
    ASSERT_EQ(notes.size(), 8u);
    for (size_t i = 0; i < notes.size(); ++i)
    {
        EXPECT_EQ(notes[i].a, i % 2 ? 66 : 64);
        EXPECT_EQ(notes[i].tick * 8, i * static_cast<Uint64>(trill->info.ticks_per_quarter));
    }
    const auto turn = LoadXml(Score(
        "<measure>" +
        Note("1",
             "<notations><ornaments><turn/>"
             "<accidental-mark placement='below'>flat</accidental-mark></ornaments></notations>") +
        "</measure>"));
    ASSERT_TRUE(turn) << SDL_GetError();
    const auto turned = Onsets(turn.get());
    ASSERT_EQ(turned.size(), 4u);
    EXPECT_EQ(turned[0].a, 62);
    EXPECT_EQ(turned[1].a, 60);
    EXPECT_EQ(turned[2].a, 58);
    EXPECT_EQ(turned[3].a, 60);
    const auto mordent = LoadXml(Score("<measure>" +
                                       Note("1", "<notations><ornaments><mordent/>"
                                                 "</ornaments></notations>") +
                                       "</measure>"));
    ASSERT_TRUE(mordent) << SDL_GetError();
    const auto mordented = Onsets(mordent.get());
    ASSERT_EQ(mordented.size(), 3u);
    EXPECT_EQ(mordented[0].a, 60);
    EXPECT_EQ(mordented[1].a, 59);
    EXPECT_EQ(mordented[2].a, 60);
    const auto tremolo = LoadXml(
        Score("<measure>" +
              Note("1", "<notations><ornaments>"
                        "<tremolo type='start'>3</tremolo></ornaments></notations>") +
              "<note><pitch><step>E</step><octave>4</octave></pitch><duration>1</duration>"
              "<notations><ornaments><tremolo type='stop'>3</tremolo></ornaments></notations>"
              "</note></measure>"));
    ASSERT_TRUE(tremolo) << SDL_GetError();
    const auto alternated = Onsets(tremolo.get());
    ASSERT_EQ(alternated.size(), 16u);
    for (size_t i = 0; i < alternated.size(); ++i)
        EXPECT_EQ(alternated[i].a, i % 2 ? 64 : 60);
    EXPECT_DOUBLE_EQ(tremolo->info.duration_seconds, 1);
}

TEST(ChipMusicXml, RolledChordsRespectDirectionAndCaesurasLeaveSilence)
{
    const auto roll = LoadXml(
        Score("<measure>" + Note("1", "<notations><arpeggiate direction='down'/></notations>") +
              "<note><chord/><pitch><step>E</step><octave>4</octave></pitch><duration>1</duration>"
              "<notations><arpeggiate direction='down'/></notations></note></measure>"));
    ASSERT_TRUE(roll) << SDL_GetError();
    const auto notes = Onsets(roll.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].a, 64);
    EXPECT_EQ(notes[0].tick, 0u);
    EXPECT_EQ(notes[1].a, 60);
    EXPECT_EQ(notes[1].tick * 8, static_cast<Uint64>(roll->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(roll->info.duration_seconds, 0.5);
    const auto pause = LoadXml(Score("<measure>" +
                                     Note("1", "<notations><articulations><caesura/>"
                                               "</articulations></notations>") +
                                     Note("1") + "</measure>"));
    ASSERT_TRUE(pause) << SDL_GetError();
    const auto paused = Onsets(pause.get());
    ASSERT_EQ(paused.size(), 2u);
    EXPECT_EQ(paused[0].duration, static_cast<Uint64>(pause->info.ticks_per_quarter));
    EXPECT_EQ(paused[1].tick * 4, 5u * static_cast<Uint64>(pause->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(pause->info.duration_seconds, 1.125);
}

TEST(ChipMusicXml, HairpinsCreateContinuousNoteGainAndRespectStaffScope)
{
    const auto song = LoadXml(
        Score("<measure><direction><direction-type><dynamics><p/></dynamics></direction-type>"
              "</direction><direction><direction-type><wedge type='crescendo'/></direction-type>"
              "<staff>1</staff></direction>" +
              Note("2", "<staff>1</staff>") + "<backup><duration>2</duration></backup>" +
              Note("2", "<staff>2</staff>") +
              "<direction><direction-type><wedge type='stop'/><dynamics><f/></dynamics>"
              "</direction-type><staff>1</staff></direction></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].b, 48);
    EXPECT_EQ(notes[1].b, 48);
    bool ramp = false, other = false;
    for (size_t i = 0; i < song->count; ++i)
    {
        const auto &event = song->events[i];
        if (event.status != 0xf1)
            continue;
        if (event.note_id == notes[0].note_id)
        {
            EXPECT_FLOAT_EQ(event.value, 1);
            EXPECT_FLOAT_EQ(event.target, 2);
            ramp = true;
        }
        else
        {
            EXPECT_FLOAT_EQ(event.value, 1);
            EXPECT_FLOAT_EQ(event.target, 1);
            other = true;
        }
    }
    EXPECT_TRUE(ramp && other);
    const Player player(Grapple_CreateChipPlayer(song.get(), 8000, 16, false),
                        Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player) << SDL_GetError();
    std::vector<float> pcm(16000);
    ASSERT_EQ(Grapple_RenderChipPlayer(player.get(), pcm.data(), 8000), 8000);
    double energy = 0;
    for (float sample : pcm)
    {
        ASSERT_TRUE(std::isfinite(sample));
        energy += sample * sample;
    }
    EXPECT_GT(energy, 0.1);
}

TEST(ChipMusicXml, PedalsCaptureAndReleaseOnlyTheIntendedStaff)
{
    const auto song = LoadXml(
        Score("<measure><direction><sound damper-pedal='yes'/><staff>1</staff></direction>" +
              Note("1", "<staff>1</staff>") + "<backup><duration>1</duration></backup>" +
              Note("1", "<staff>2</staff>") +
              "<forward><duration>1</duration></forward>"
              "<direction><sound damper-pedal='no'/><staff>1</staff></direction></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].duration, 2 * notes[1].duration);
    const auto sostenuto = LoadXml(
        Score("<measure>" + Note("2", "<voice>a</voice>") +
              "<backup><duration>1</duration></backup>"
              "<sound sostenuto-pedal='yes'/>" +
              Note("1", "<voice>b</voice>") +
              "<forward><duration>1</duration></forward><sound sostenuto-pedal='no'/></measure>"));
    ASSERT_TRUE(sostenuto) << SDL_GetError();
    const auto captured = Onsets(sostenuto.get());
    ASSERT_EQ(captured.size(), 2u);
    EXPECT_EQ(captured[0].duration, 3 * captured[1].duration);
}

TEST(ChipMusicXml, SwingIsExactAndDoesNotReswingTupletsOrMissingTypes)
{
    const auto song = LoadXml(Score(
        "<measure><attributes><divisions>6</divisions></attributes><sound><swing><first>2</first>"
        "<second>1</second><swing-type>eighth</swing-type></swing></sound>" +
        Note("3", "<type>eighth</type>") + Note("3", "<type>eighth</type>") +
        Note("2", "<type>eighth</type><time-modification><actual-notes>3</actual-notes>"
                  "<normal-notes>2</normal-notes></time-modification>") +
        Note("4") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 4u);
    const auto q = static_cast<Uint64>(song->info.ticks_per_quarter);
    EXPECT_EQ(notes[0].duration * 3, q * 2);
    EXPECT_EQ(notes[1].tick * 3, q * 2);
    EXPECT_EQ(notes[1].duration * 3, q);
    EXPECT_EQ(notes[2].tick, q);
    EXPECT_EQ(notes[2].duration * 3, q);
    EXPECT_EQ(notes[3].tick * 3, q * 4);
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 1);
}

TEST(ChipMusicXml, RepeatsRestoreScopedDynamicsAndTempoRampsSlowPlayback)
{
    const auto repeat = LoadXml(Score(
        "<measure><barline location='left'><repeat direction='forward'/></barline>" + Note("1") +
        "</measure><measure><direction><direction-type><dynamics><p/></dynamics></direction-type>"
        "</direction>" +
        Note("1") + "<barline><repeat direction='backward'/></barline></measure>"));
    ASSERT_TRUE(repeat) << SDL_GetError();
    const auto notes = Onsets(repeat.get());
    ASSERT_EQ(notes.size(), 4u);
    EXPECT_EQ(notes[0].b, 80);
    EXPECT_EQ(notes[1].b, 48);
    EXPECT_EQ(notes[2].b, 80);
    EXPECT_EQ(notes[3].b, 48);
    const auto ramp =
        LoadXml(Score("<measure><sound tempo='120'/><direction><direction-type><words>rit.</words>"
                      "</direction-type></direction>" +
                      Note("4") + "<sound tempo='60'/></measure>"));
    ASSERT_TRUE(ramp) << SDL_GetError();
    EXPECT_GT(ramp->info.duration_seconds, 2.7);
    EXPECT_LT(ramp->info.duration_seconds, 2.8);
    size_t tempos = 0;
    for (size_t i = 0; i < ramp->count; ++i)
        tempos += ramp->events[i].tempo != 0;
    EXPECT_GT(tempos, 100u);
}

TEST(ChipMusicXml, MemoryHelperOwnsItsParsedResultAndReportsInvalidInput)
{
    auto bytes = Score("<measure>" + Note("1") + "</measure>");
    Grapple_ChipDiagnostic error;
    const Song song(Grapple_LoadChipSongMemory(bytes.data(), bytes.size(), nullptr, &error),
                    Grapple_DestroyChipSong);
    ASSERT_TRUE(song) << SDL_GetError();
    bytes.assign(bytes.size(), 'x');
    EXPECT_EQ(Onsets(song.get()).size(), 1u);
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.5);
    EXPECT_EQ(Grapple_LoadChipSongMemory(nullptr, 0, nullptr, &error), nullptr);
    EXPECT_EQ(error.code, GRAPPLE_CHIP_DIAGNOSTIC_INPUT);
}

TEST(ChipMusicXml, SameChannelInstrumentsKeepTheirOwnProgramVolumeAndPan)
{
    const auto song = LoadXml(
        "<score-partwise><part-list><score-part id='P'><part-name>ensemble</part-name>"
        "<score-instrument id='z'/><score-instrument id='a'/>"
        "<midi-instrument id='z'><midi-channel>1</midi-channel><midi-program>33</midi-program>"
        "<volume>25</volume><pan>-90</pan></midi-instrument>"
        "<midi-instrument id='a'><midi-channel>1</midi-channel><midi-program>81</midi-program>"
        "<volume>80</volume><pan>90</pan></midi-instrument></score-part></part-list><part id='P'>"
        "<measure>" +
        Note("1") + Note("1", "<instrument id='a'/>") + "</measure></part></score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[0].program, 32);
    EXPECT_FLOAT_EQ(notes[0].instrument_gain, 0.25f);
    EXPECT_FLOAT_EQ(notes[0].instrument_pan, -90);
    EXPECT_EQ(notes[1].program, 80);
    EXPECT_FLOAT_EQ(notes[1].instrument_gain, 0.8f);
    EXPECT_FLOAT_EQ(notes[1].instrument_pan, 90);
    const Player player(Grapple_CreateChipPlayer(song.get(), 8000, 16, false),
                        Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    std::vector<float> pcm(1600);
    ASSERT_EQ(Grapple_RenderChipPlayer(player.get(), pcm.data(), 800), 800);
    Grapple_ChipMapping mapping{};
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.preset, GRAPPLE_CHIP_PRESET_BASS);
    EXPECT_EQ(mapping.reason, GRAPPLE_CHIP_MAPPING_PROGRAM);
}

TEST(ChipMusicXml, InstrumentChangesRespectOffsetsAndDocumentOrderIndependently)
{
    const auto song =
        LoadXml("<score-partwise><part-list><score-part id='P'><part-name>ensemble</part-name>"
                "<score-instrument id='i'/><midi-instrument id='i'><midi-program>33</midi-program>"
                "</midi-instrument></score-part></part-list><part id='P'><measure>"
                "<direction><offset>2</offset><sound><midi-instrument "
                "id='i'><midi-program>89</midi-program>"
                "<volume>50</volume></midi-instrument></sound></direction>" +
                Note("1") + Note("1") + Note("1") + "<backup><duration>3</duration></backup>" +
                Note("1", "<voice>2</voice>") + "</measure></part></score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 4u);
    for (const auto &note : notes)
    {
        const bool changed = note.tick >= 2u * static_cast<Uint64>(song->info.ticks_per_quarter);
        EXPECT_EQ(note.program, changed ? 88 : 32);
        EXPECT_FLOAT_EQ(note.instrument_gain, changed ? 0.5f : 1);
    }
}

TEST(ChipMusicXml, UnpitchedMetadataChoosesDrumsOnAnyMidiChannel)
{
    const auto song = LoadXml(
        "<score-partwise><part-list><score-part id='P'><part-name>kit</part-name>"
        "<midi-instrument "
        "id='snare'><midi-channel>1</midi-channel><midi-unpitched>39</midi-unpitched>"
        "</midi-instrument></score-part></part-list><part id='P'><measure><note><unpitched/>"
        "<duration>1</duration><instrument id='snare'/></note></measure></part></score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    ASSERT_EQ(Onsets(song.get()).size(), 1u);
    EXPECT_TRUE(Onsets(song.get())[0].unpitched);
    const Player player(Grapple_CreateChipPlayer(song.get(), 8000, 16, false),
                        Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    float pcm[2];
    ASSERT_EQ(Grapple_RenderChipPlayer(player.get(), pcm, 1), 1);
    Grapple_ChipMapping mapping{};
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.preset, GRAPPLE_CHIP_PRESET_DRUMS);
    EXPECT_EQ(mapping.reason, GRAPPLE_CHIP_MAPPING_PERCUSSION);
}

TEST(ChipMusicXml, NamedInstrumentWithoutMidiMetadataStillHasAPitchedDefault)
{
    const auto song = LoadXml(
        "<score-partwise><part-list><score-part id='P'><part-name>ensemble</part-name>"
        "<score-instrument id='i'><instrument-name>Synth</instrument-name></score-instrument>"
        "</score-part></part-list><part id='P'><measure>" +
        Note("1", "<instrument id='i'/>") + "</measure></part></score-partwise>");
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_EQ(Onsets(song.get())[0].program, 0);
}

TEST(ChipMusicXml, DifferentMicrotonalPitchesCannotBeTiedTogether)
{
    EXPECT_FALSE(
        LoadXml(Score("<measure>" + Note("1", "<tie type='start'/>") +
                      "<note><pitch><step>C</step><alter>0.5</alter><octave>4</octave></pitch>"
                      "<duration>1</duration><tie type='stop'/></note></measure>")));
    EXPECT_NE(std::string(SDL_GetError()).find("tie stop"), std::string::npos);
}

TEST(ChipMusicXml, AllMetronomeBeatUnitsAndTiedUnitsUseQuarterBeatTempo)
{
    const std::vector<std::pair<std::string, std::string>> units = {
        {"maxima", "3.75"}, {"long", "7.5"},    {"breve", "15"},   {"whole", "30"},
        {"half", "60"},     {"quarter", "120"}, {"eighth", "240"}, {"16th", "480"},
        {"32nd", "960"},    {"64th", "1920"},   {"128th", "3840"}, {"256th", "7680"},
        {"512th", "15360"}, {"1024th", "30720"}};
    for (const auto &[unit, bpm] : units)
    {
        const auto song = LoadXml(Score("<measure><direction><direction-type><metronome>"
                                        "<beat-unit>" +
                                        unit + "</beat-unit><per-minute>" + bpm +
                                        "</per-minute></metronome></direction-type></direction>" +
                                        Note("1") + "</measure>"));
        ASSERT_TRUE(song) << unit << SDL_GetError();
        EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.5) << unit;
    }
    const auto tied = LoadXml(Score(
        "<measure><direction><direction-type><metronome>"
        "<beat-unit>quarter</beat-unit><beat-unit-tied><beat-unit>eighth</beat-unit>"
        "</beat-unit-tied><per-minute>80</per-minute></metronome></direction-type></direction>" +
        Note("1") + "</measure>"));
    ASSERT_TRUE(tied) << SDL_GetError();
    EXPECT_DOUBLE_EQ(tied->info.duration_seconds, 0.5);
}

TEST(ChipMusicXml, MetricModulationChangesTempoAtItsNotatedPosition)
{
    const auto song =
        LoadXml(Score("<measure>" + Note("1") +
                      "<direction><direction-type><metronome><beat-unit>quarter</beat-unit>"
                      "<beat-unit>eighth</beat-unit></metronome></direction-type></direction>" +
                      Note("1") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 1.5);
}

TEST(ChipMusicXml, MetronomeSwingRelationshipChangesOnlyUntupletedEighths)
{
    const auto song = LoadXml(Score(
        "<measure><direction><direction-type><metronome>"
        "<metronome-note><metronome-type>eighth</metronome-type></metronome-note>"
        "<metronome-note><metronome-type>eighth</metronome-type></metronome-note>"
        "<metronome-relation>equals</metronome-relation>"
        "<metronome-note><metronome-type>quarter</metronome-type><metronome-tuplet type='start'>"
        "<actual-notes>3</actual-notes><normal-notes>2</normal-notes></metronome-tuplet></"
        "metronome-note>"
        "<metronome-note><metronome-type>eighth</metronome-type><metronome-tuplet type='stop'>"
        "<actual-notes>3</actual-notes><normal-notes>2</normal-notes></metronome-tuplet></"
        "metronome-note>"
        "</metronome></direction-type></direction>" +
        Note("0.5", "<type>eighth</type>") + Note("0.5", "<type>eighth</type>") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    EXPECT_EQ(notes[1].tick * 3, 2u * static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.5);
}

TEST(ChipMusicXml, UnmeteredMeasuresUseExplicitDurationsAndRejectEmptyAmbiguity)
{
    const auto song =
        LoadXml(Score("<measure><attributes><time><senza-misura/></time></attributes>" +
                      Note("1.25") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 0.625);
    EXPECT_FALSE(LoadXml(Score("<measure><attributes><time><senza-misura/></time>"
                               "</attributes></measure>")));
    EXPECT_NE(std::string(SDL_GetError()).find("empty unmetered"), std::string::npos);
}

TEST(ChipMusicXml, DiagnosticsIdentifyVoiceElementAndMissingReferences)
{
    const auto xml =
        Score("<measure>" +
              Note("1", "<voice>inner</voice><staff>1</staff>"
                        "<notations><technical><unknown-gesture/></technical></notations>") +
              "</measure>");
    Grapple_ChipDiagnostic error{};
    Song song(Grapple_LoadChipSongMemory(xml.data(), xml.size(), nullptr, &error),
              Grapple_DestroyChipSong);
    EXPECT_FALSE(song);
    EXPECT_EQ(error.code, GRAPPLE_CHIP_DIAGNOSTIC_UNSUPPORTED);
    EXPECT_STREQ(error.voice, "inner");
    EXPECT_STREQ(error.element, "unknown-gesture");
    EXPECT_EQ(error.staff, 1);
    const std::string duplicate = "<score-partwise><part-list><score-part id='p'/>"
                                  "<score-part id='p'/></part-list><part id='p'><measure>" +
                                  Note("1") + "</measure></part></score-partwise>";
    song.reset(Grapple_LoadChipSongMemory(duplicate.data(), duplicate.size(), nullptr, &error));
    EXPECT_FALSE(song);
    EXPECT_EQ(error.code, GRAPPLE_CHIP_DIAGNOSTIC_CROSS_REFERENCE);
    EXPECT_STREQ(error.element, "score-part");
}

TEST(ChipMusicXml, HarmonicBaseAndTouchProduceOneSoundingPartial)
{
    const auto song = LoadXml(Score(
        "<measure>" +
        Note("1", "<notations><technical><harmonic><artificial/><base-pitch/></harmonic></"
                  "technical></notations>") +
        "<note><chord/><pitch><step>F</step><octave>4</octave></pitch><duration>1</duration>"
        "<notations><technical><harmonic><artificial/><touching-pitch/></harmonic></technical>"
        "</notations></note></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 1u);
    EXPECT_EQ(notes[0].a, 84); // Fourth partial: two octaves above the base C4.
}

TEST(ChipMusicXml, LetRingUsesTheNextAttackOnTheSameString)
{
    const auto song = LoadXml(Score(
        "<measure>" +
        Note("1",
             "<notations><technical><string>1</string><other-technical>let ring</other-technical>"
             "</technical></notations>") +
        Note("1", "<notations><technical><string>2</string>"
                  "</technical></notations>") +
        Note("1", "<notations><technical><string>1</string>"
                  "</technical></notations>") +
        "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 3u);
    EXPECT_EQ(notes[0].duration, 2u * static_cast<Uint64>(song->info.ticks_per_quarter));
    EXPECT_EQ(notes[1].duration, static_cast<Uint64>(song->info.ticks_per_quarter));
}

TEST(ChipMusicXml, TrillContinuationAndLongMordentsExpandAcrossNotes)
{
    const auto song = LoadXml(Score(
        "<measure>" +
        Note("1", "<voice>1</voice><notations><ornaments>"
                  "<trill-mark beats='4'/><wavy-line type='start'/></ornaments></notations>") +
        Note("1", "<voice>1</voice><notations><ornaments><wavy-line "
                  "type='stop'/></ornaments></notations>") +
        Note("1", "<voice>1</voice><notations><ornaments><mordent long='yes'/>"
                  "</ornaments></notations>") +
        "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 13u);
    EXPECT_EQ(notes[1].a, 62);
    EXPECT_EQ(notes[5].a, 62);
    EXPECT_EQ(notes[9].a, 59);
}

TEST(ChipMusicXml, WhammyBarBendsUseIndependentAcceleratingCurves)
{
    const auto song = LoadXml(Score(
        "<measure>" +
        Note("1",
             "<notations><technical>"
             "<bend first-beat='0' last-beat='100' accelerate='yes'><bend-alter>-2</bend-alter>"
             "<with-bar>dip</with-bar></bend></technical></notations>") +
        Note("1", "<chord/><voice>other</voice>") + "</measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 2u);
    const auto &bend = song->expressions[notes[0].expression - 1];
    const auto &neutral = song->expressions[notes[1].expression - 1];
    EXPECT_TRUE(bend.bend_accelerate);
    EXPECT_NEAR(Chip_ExpressionPitch(&bend, 0.5, 1), -0.5, 1e-6);
    EXPECT_EQ(Chip_ExpressionPitch(&neutral, 0.5, 1), 0);
}

TEST(ChipMusicXml, MuseScoreFourExportMatchesTheKnownGuitarProPerformance)
{
    const Song expected(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml"),
                        Grapple_DestroyChipSong);
    const Song actual(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-musescore.musicxml"),
                      Grapple_DestroyChipSong);
    ASSERT_TRUE(expected && actual) << SDL_GetError();
    EXPECT_DOUBLE_EQ(actual->info.duration_seconds, 10);
    EXPECT_EQ(actual->info.track_count, 4);
    const int counts[] = {20, 15, 20, 32};
    for (int part = 0; part < 4; ++part)
    {
        auto a = Onsets(actual.get(), part), b = Onsets(expected.get(), part);
        const auto order = [](const ChipEvent &left, const ChipEvent &right) {
            return left.tick != right.tick ? left.tick < right.tick : left.a < right.a;
        };
        std::sort(a.begin(), a.end(), order);
        std::sort(b.begin(), b.end(), order);
        ASSERT_EQ(a.size(), static_cast<size_t>(counts[part]));
        ASSERT_EQ(a.size(), b.size());
        for (size_t i = 0; i < a.size(); ++i)
        {
            EXPECT_EQ(a[i].a, b[i].a);
            EXPECT_EQ(a[i].tick * static_cast<Uint64>(expected->info.ticks_per_quarter),
                      b[i].tick * static_cast<Uint64>(actual->info.ticks_per_quarter));
            EXPECT_EQ(a[i].duration * static_cast<Uint64>(expected->info.ticks_per_quarter),
                      b[i].duration * static_cast<Uint64>(actual->info.ticks_per_quarter));
        }
    }
}

TEST(ChipMusicXml, MuseScoreAlteredTabPitchesAreRetainedUnlessStaffIsSelected)
{
    const char *path = MIXER_TEST_ASSETS_DIR "/c64-musescore-tab-roundtrip.musicxml";
    const Song full(Grapple_LoadChipSong(path), Grapple_DestroyChipSong);
    ASSERT_TRUE(full) << SDL_GetError();
    EXPECT_EQ(Onsets(full.get()).size(), 142u);
    Grapple_ChipImportOptions policy;
    ASSERT_TRUE(Grapple_GetChipImportDefaults(&policy));
    policy.staff = 1;
    const Song selected(Grapple_LoadChipSongEx(path, &policy, nullptr), Grapple_DestroyChipSong);
    ASSERT_TRUE(selected) << SDL_GetError();
    EXPECT_EQ(Onsets(selected.get()).size(), 87u);
    EXPECT_DOUBLE_EQ(selected->info.duration_seconds, 10);
}

TEST(ChipMusicXml, TiesCanApplyOnlyOnSelectedRepeatPasses)
{
    const auto song =
        LoadXml(Score("<measure>" + Note("1", "<tie type='start' time-only='1'/>") +
                      "</measure><measure>" + Note("1", "<tie type='stop' time-only='1'/>") +
                      "<barline><repeat direction='backward'/></barline></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    const auto notes = Onsets(song.get());
    ASSERT_EQ(notes.size(), 3u);
    const auto q = static_cast<Uint64>(song->info.ticks_per_quarter);
    EXPECT_EQ(notes[0].duration, 2 * q);
    EXPECT_EQ(notes[1].tick, 2 * q);
    EXPECT_EQ(notes[2].tick, 3 * q);
}

TEST(ChipMusicXml, RehearsalSectionsRetainSourceAndExpandedLoopBounds)
{
    const auto song = LoadXml(
        Score("<measure><direction><direction-type><rehearsal>Intro</rehearsal></direction-type>"
              "</direction>" +
              Note("1") +
              "</measure><measure><direction><direction-type>"
              "<rehearsal>Loop</rehearsal></direction-type></direction>" +
              Note("1") + "<barline><repeat direction='backward'/></barline></measure>"));
    ASSERT_TRUE(song) << SDL_GetError();
    ASSERT_EQ(Grapple_GetChipSectionCount(song.get()), 4);
    const auto q = static_cast<Uint64>(song->info.ticks_per_quarter);
    Grapple_ChipSection section{};
    ASSERT_TRUE(Grapple_ReadChipSection(song.get(), 2, &section));
    EXPECT_STREQ(section.name, "Intro");
    EXPECT_EQ(section.start_tick, 2 * q);
    EXPECT_EQ(section.end_tick, 3 * q);
    EXPECT_EQ(section.source_measure, 0);
    EXPECT_EQ(section.measure_visit, 2);
    const Player player(Grapple_CreateChipPlayer(song.get(), 8000, 16, false),
                        Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    EXPECT_TRUE(
        Grapple_SetChipPlayerLoop(player.get(), section.start_tick, section.end_tick, true));
}
