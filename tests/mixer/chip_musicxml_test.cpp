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
