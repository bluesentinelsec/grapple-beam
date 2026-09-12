/** @file chip_transport_test.cpp @brief Game transport, fractional loops and isolated mixing. */
#include <algorithm>
#include <cmath>
#include <grapple/chiptune.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

namespace
{
using Song = std::unique_ptr<Grapple_ChipSong, decltype(&Grapple_DestroyChipSong)>;
using Player = std::unique_ptr<Grapple_ChipPlayer, decltype(&Grapple_DestroyChipPlayer)>;

Song Compose(int parts = 1, int ppqn = 480, Uint64 length = 1920)
{
    Grapple_ChipComposer *composer = Grapple_CreateChipComposer(parts, ppqn);
    if (!composer)
        return {nullptr, Grapple_DestroyChipSong};
    for (int part = 0; part < parts; ++part)
    {
        EXPECT_TRUE(Grapple_SetChipPart(composer, part, "harmony", GRAPPLE_CHIP_PRESET_AUTO, 1));
        const Grapple_ChipNote note = {part, 60 + part * 4, 90, 0, length};
        EXPECT_TRUE(Grapple_AddChipNote(composer, &note));
    }
    Song song(Grapple_BuildChipSong(composer, length), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    return song;
}

Player Create(const Grapple_ChipSong *song, bool loop = false)
{
    Player player(Grapple_CreateChipPlayer(song, 8000, 32, loop), Grapple_DestroyChipPlayer);
    if (player)
    {
        const Grapple_ChipEffects dry = {};
        for (int i = GRAPPLE_CHIP_PRESET_LEAD; i <= GRAPPLE_CHIP_PRESET_DRUMS; ++i)
            EXPECT_TRUE(Grapple_SetChipPresetEffects(player.get(),
                                                     static_cast<Grapple_ChipPreset>(i), &dry));
    }
    return player;
}

std::vector<float> Render(Grapple_ChipPlayer *player, int frames)
{
    std::vector<float> pcm(static_cast<size_t>(frames) * 2);
    EXPECT_GE(Grapple_RenderChipPlayer(player, pcm.data(), frames), 0);
    return pcm;
}

double Energy(const std::vector<float> &pcm, int channel = -1)
{
    double energy = 0;
    for (size_t i = 0; i < pcm.size(); ++i)
        if (channel < 0 || i % 2 == static_cast<size_t>(channel))
            energy += static_cast<double>(pcm[i]) * pcm[i];
    return energy;
}

TEST(ChipTransport, FractionalFramesCarryAcrossThousandsOfLoops)
{
    const auto song = Compose(1, 3, 1); // 4000/3 sample frames per loop at 8 kHz.
    ASSERT_TRUE(song);
    const auto player = Create(song.get(), true);
    ASSERT_TRUE(player);
    std::vector<float> pcm(2048);
    for (int i = 0; i < 2000; ++i)
        ASSERT_EQ(Grapple_RenderChipPlayer(player.get(), pcm.data(), 1024), 1024);
    Grapple_ChipPosition position{};
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    const double reconstructed_frames =
        static_cast<double>(position.loop_count) * (4000.0 / 3) + position.seconds * 8000;
    EXPECT_NEAR(reconstructed_frames, 2048000, 0.00001);
    EXPECT_LE(Grapple_GetChipPlayerPeakVoices(player.get()), 1);
}

TEST(ChipTransport, LoopHasAnIntroAndUsesOnlyTheSelectedSectionAfterward)
{
    const auto song = Compose();
    const auto player = Create(song.get());
    ASSERT_TRUE(player);
    ASSERT_TRUE(Grapple_SetChipPlayerLoop(player.get(), 480, 960, true));
    Render(player.get(), 18000); // 2.25 seconds: intro 1 s, two 0.5 s loops, 0.25 s.
    Grapple_ChipPosition position{};
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    EXPECT_EQ(position.loop_count, 3u);
    EXPECT_NEAR(position.seconds, 0.75, 0.000001);
    EXPECT_NEAR(position.beat, 1.5, 0.000001);
    EXPECT_FALSE(Grapple_SeekChipPlayer(player.get(), 960));
    EXPECT_FALSE(Grapple_SetChipPlayerLoop(player.get(), 960, 480, true));
    ASSERT_TRUE(Grapple_SetChipPlayerLoop(player.get(), 0, 0, false));
    ASSERT_TRUE(Grapple_SeekChipPlayer(player.get(), 1440));
    EXPECT_GT(Energy(Render(player.get(), 1000)), 0.01); // Reconstructed held chord.
}

TEST(ChipTransport, TempoScalingPreservesPositionPitchAndBlockIndependence)
{
    const auto song = Compose();
    const auto a = Create(song.get());
    const auto b = Create(song.get());
    ASSERT_TRUE(a && b);
    ASSERT_TRUE(Grapple_SetChipPlayerTempo(a.get(), 1.5));
    ASSERT_TRUE(Grapple_SetChipPlayerTempo(b.get(), 1.5));
    auto expected = Render(a.get(), 4000);
    std::vector<float> actual;
    for (int i = 0; i < 40; ++i)
    {
        const auto block = Render(b.get(), 100);
        actual.insert(actual.end(), block.begin(), block.end());
    }
    EXPECT_EQ(actual, expected);
    Grapple_ChipPosition position{};
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(a.get(), &position));
    EXPECT_NEAR(position.beat, 1.5, 0.000001);
    EXPECT_FALSE(Grapple_SetChipPlayerTempo(a.get(), 0));
    ASSERT_TRUE(Grapple_SetChipPlayerTempo(a.get(), 0.5));
    Render(a.get(), 4000);
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(a.get(), &position));
    EXPECT_NEAR(position.beat, 2, 0.000001);
}

TEST(ChipTransport, SeekFindsSourceMeasuresAndResetsTails)
{
    const auto song = Song(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml"),
                           Grapple_DestroyChipSong);
    ASSERT_TRUE(song);
    const auto player = Create(song.get());
    ASSERT_TRUE(player);
    const auto *info = Grapple_GetChipSongInfo(song.get());
    ASSERT_TRUE(
        Grapple_SeekChipPlayer(player.get(), static_cast<Uint64>(info->ticks_per_quarter) * 9));
    Grapple_ChipPosition position{};
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    EXPECT_EQ(position.source_measure, 2);
    EXPECT_EQ(position.measure_visit, 2);
    EXPECT_NEAR(position.measure_beat, 1, 1e-8);
    EXPECT_NEAR(position.seconds, 4.5, 1e-8);
    EXPECT_GT(Energy(Render(player.get(), 800)), 0.01);
    Grapple_ResetChipPlayer(player.get());
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    EXPECT_EQ(position.tick, 0u);
}

TEST(ChipTransport, MappingPrecedenceAndIndependentMuteSoloPan)
{
    const auto song = Compose(2);
    const auto player = Create(song.get());
    ASSERT_TRUE(player);
    Grapple_ChipMapping mapping{};
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.preset, GRAPPLE_CHIP_PRESET_HARMONY);
    EXPECT_EQ(mapping.reason, GRAPPLE_CHIP_MAPPING_NAME);
    ASSERT_TRUE(Grapple_SetChipTrackPreset(player.get(), 0, GRAPPLE_CHIP_PRESET_BASS, 1));
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.reason, GRAPPLE_CHIP_MAPPING_OVERRIDE);
    Grapple_ChipTrackMix mix{1, -1, false, true};
    ASSERT_TRUE(Grapple_SetChipTrackMix(player.get(), 0, &mix));
    const auto pcm = Render(player.get(), 1000);
    EXPECT_GT(Energy(pcm, 0), 0.01);
    EXPECT_EQ(Energy(pcm, 1), 0);
    mix.muted = true;
    ASSERT_TRUE(Grapple_SetChipTrackMix(player.get(), 0, &mix));
    EXPECT_EQ(Energy(Render(player.get(), 500)), 0);
    mix.muted = false;
    ASSERT_TRUE(Grapple_SetChipTrackMix(player.get(), 0, &mix));
    EXPECT_GT(Energy(Render(player.get(), 500)), 0.01);
    ASSERT_TRUE(Grapple_SetChipPlayerGain(player.get(), 0));
    EXPECT_EQ(Energy(Render(player.get(), 500)), 0);
}

TEST(ChipTransport, PrivateEffectsDoNotModifyPeersOrPresetSettings)
{
    const auto song = Compose(2);
    const auto player = Create(song.get());
    ASSERT_TRUE(player);
    Grapple_ChipEffects wet{};
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_HARMONY, &wet));
    ASSERT_TRUE(Grapple_SetChipTrackEffects(player.get(), 0, &wet));
    Grapple_ChipEffects first{}, second{};
    ASSERT_TRUE(Grapple_ReadChipTrackEffects(player.get(), 0, GRAPPLE_CHIP_PRESET_HARMONY, &first));
    ASSERT_TRUE(
        Grapple_ReadChipTrackEffects(player.get(), 1, GRAPPLE_CHIP_PRESET_HARMONY, &second));
    EXPECT_EQ(first.pulse_beats, 0.5);
    EXPECT_EQ(second.pulse_beats, 0);
    wet.pulse_beats = 1;
    ASSERT_TRUE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_HARMONY, &wet));
    ASSERT_TRUE(Grapple_ReadChipTrackEffects(player.get(), 0, GRAPPLE_CHIP_PRESET_HARMONY, &first));
    EXPECT_EQ(first.pulse_beats, 0.5);
    ASSERT_TRUE(Grapple_SetChipTrackEffects(player.get(), 0, nullptr));
    ASSERT_TRUE(Grapple_ReadChipTrackEffects(player.get(), 0, GRAPPLE_CHIP_PRESET_HARMONY, &first));
    EXPECT_EQ(first.pulse_beats, 1);
}
TEST(ChipTransport, PrivateBusChangesOnlyItsOwnAudiblePart)
{
    const auto song = Compose(2);
    const auto wet = Create(song.get());
    const auto dry = Create(song.get());
    ASSERT_TRUE(wet && dry);
    Grapple_ChipEffects effects{};
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_HARMONY, &effects));
    ASSERT_TRUE(Grapple_SetChipTrackEffects(wet.get(), 0, &effects));
    const Grapple_ChipTrackMix muted{1, 0, true, false};
    ASSERT_TRUE(Grapple_SetChipTrackMix(wet.get(), 0, &muted));
    ASSERT_TRUE(Grapple_SetChipTrackMix(dry.get(), 0, &muted));
    EXPECT_EQ(Render(wet.get(), 800), Render(dry.get(), 800));
    const Grapple_ChipTrackMix audible{1, 0, false, false};
    ASSERT_TRUE(Grapple_SetChipTrackMix(wet.get(), 0, &audible));
    ASSERT_TRUE(Grapple_SetChipTrackMix(dry.get(), 0, &audible));
    EXPECT_NE(Render(wet.get(), 800), Render(dry.get(), 800));
}

TEST(ChipTransport, SeekReconstructsTempoAndControllersBeforeAnActiveNote)
{
    auto *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_NE(composer, nullptr);
    ASSERT_TRUE(Grapple_AddChipTempo(composer, 480, 60));
    ASSERT_TRUE(Grapple_AddChipControl(composer, 0, 480, 10, 127));
    const Grapple_ChipNote note{0, 60, 100, 0, 1920};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    const Song song(Grapple_BuildChipSong(composer, 1920), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    const auto player = Create(song.get());
    ASSERT_TRUE(player);
    ASSERT_TRUE(Grapple_SeekChipPlayer(player.get(), 960));
    Grapple_ChipPosition position{};
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    EXPECT_NEAR(position.seconds, 1.5, 1e-8);
    const auto pcm = Render(player.get(), 800);
    EXPECT_GT(Energy(pcm, 1), 0.1);
    EXPECT_LT(Energy(pcm, 0), 1e-9);
    ASSERT_TRUE(Grapple_ReadChipPlayerPosition(player.get(), &position));
    EXPECT_NEAR(position.beat, 2.1, 1e-8);
}

} // namespace
