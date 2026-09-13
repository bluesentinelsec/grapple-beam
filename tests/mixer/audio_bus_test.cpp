#include <array>
#include <grapple/audio_bus.h>
#include <gtest/gtest.h>
#include <limits>
namespace
{
class AudioBuses : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0));
        ASSERT_TRUE(MIX_Init());
        Grapple_SetAudioMuted(false);
        for (int i = 0; i < GRAPPLE_AUDIO_BUS_COUNT; ++i)
            ASSERT_TRUE(Grapple_SetAudioBusGain(static_cast<Grapple_AudioBus>(i), 1));
    }
    void TearDown() override
    {
        Grapple_SetAudioMuted(false);
        for (int i = 0; i < GRAPPLE_AUDIO_BUS_COUNT; ++i)
            Grapple_SetAudioBusGain(static_cast<Grapple_AudioBus>(i), 1);
        MIX_Quit();
        SDL_Quit();
    }
};
} // namespace
TEST_F(AudioBuses, MasterMultipliesCategoryAndMuteRetainsGains)
{
    ASSERT_TRUE(Grapple_SetAudioBusGain(GRAPPLE_AUDIO_MASTER, .5f));
    ASSERT_TRUE(Grapple_SetAudioBusGain(GRAPPLE_AUDIO_SPEECH, .25f));
    float pcm[] = {1, -1};
    ASSERT_TRUE(Grapple_ApplyAudioBus(GRAPPLE_AUDIO_SPEECH, pcm, 2));
    EXPECT_FLOAT_EQ(pcm[0], .125f);
    EXPECT_FLOAT_EQ(pcm[1], -.125f);
    Grapple_SetAudioMuted(true);
    ASSERT_TRUE(Grapple_ApplyAudioBus(GRAPPLE_AUDIO_AMBIENT, pcm, 2));
    EXPECT_FLOAT_EQ(pcm[0], 0);
    EXPECT_FLOAT_EQ(Grapple_GetAudioBusGain(GRAPPLE_AUDIO_SPEECH), .25f);
    EXPECT_FALSE(
        Grapple_SetAudioBusGain(GRAPPLE_AUDIO_MUSIC, std::numeric_limits<float>::quiet_NaN()));
}
TEST_F(AudioBuses, MixerGroupsApplyIndependentCategoriesAndMasterOnce)
{
    SDL_AudioSpec spec{SDL_AUDIO_F32, 1, 48000};
    auto *mixer = MIX_CreateMixer(&spec);
    ASSERT_NE(mixer, nullptr);
    ASSERT_TRUE(Grapple_AttachAudioBuses(mixer));
    ASSERT_TRUE(Grapple_AttachAudioBuses(mixer));
    std::array<float, 4096> raw{};
    raw.fill(.25f);
    auto *speech = MIX_CreateTrack(mixer), *ambient = MIX_CreateTrack(mixer);
    ASSERT_NE(speech, nullptr);
    ASSERT_NE(ambient, nullptr);
    ASSERT_TRUE(
        MIX_SetTrackRawIOStream(speech, SDL_IOFromConstMem(raw.data(), sizeof(raw)), &spec, true));
    ASSERT_TRUE(
        MIX_SetTrackRawIOStream(ambient, SDL_IOFromConstMem(raw.data(), sizeof(raw)), &spec, true));
    ASSERT_TRUE(Grapple_RouteAudioTrack(speech, GRAPPLE_AUDIO_SPEECH));
    ASSERT_TRUE(Grapple_RouteAudioTrack(ambient, GRAPPLE_AUDIO_AMBIENT));
    ASSERT_TRUE(Grapple_SetAudioBusGain(GRAPPLE_AUDIO_MASTER, .5f));
    ASSERT_TRUE(Grapple_SetAudioBusGain(GRAPPLE_AUDIO_SPEECH, 0));
    ASSERT_TRUE(MIX_PlayTrack(speech, 0));
    ASSERT_TRUE(MIX_PlayTrack(ambient, 0));
    std::array<float, 128> output{};
    ASSERT_EQ(MIX_Generate(mixer, output.data(), static_cast<int>(sizeof(output))),
              static_cast<int>(sizeof(output)));
    EXPECT_NEAR(output.back(), .125f, .0001f);
    Grapple_SetAudioMuted(true);
    ASSERT_EQ(MIX_Generate(mixer, output.data(), static_cast<int>(sizeof(output))),
              static_cast<int>(sizeof(output)));
    EXPECT_FLOAT_EQ(output.back(), 0);
    MIX_DestroyMixer(mixer);
}
