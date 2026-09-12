#include "chip_internal.h"

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
