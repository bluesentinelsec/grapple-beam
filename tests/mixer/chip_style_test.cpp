#include <grapple/chiptune.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace
{
using Song = std::unique_ptr<Grapple_ChipSong, decltype(&Grapple_DestroyChipSong)>;
using Player = std::unique_ptr<Grapple_ChipPlayer, decltype(&Grapple_DestroyChipPlayer)>;

Song LoadNamed(void)
{
    return Song(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml"),
                Grapple_DestroyChipSong);
}
} // namespace

TEST(ChipStyle, CatalogListsBuiltinPalettes)
{
    EXPECT_GE(Grapple_GetChipStyleCount(), 6);
    EXPECT_TRUE(Grapple_GetChipStyle("c64"));
    EXPECT_TRUE(Grapple_GetChipStyle("nes-smb"));
    EXPECT_TRUE(Grapple_GetChipStyle("gb-pokemon"));
    EXPECT_TRUE(Grapple_GetChipStyle("genesis-sonic"));
    EXPECT_TRUE(Grapple_GetChipStyle("snes-zelda"));
    EXPECT_TRUE(Grapple_GetChipStyle("dos-wolf3d"));
    Grapple_ChipStyleInfo info{};
    ASSERT_TRUE(Grapple_ReadChipStyleInfo(Grapple_GetChipStyle("nes-smb"), &info));
    EXPECT_STREQ(info.id, "nes-smb");
    EXPECT_EQ(info.polyphony, 4);
    EXPECT_FALSE(info.approximation);
    ASSERT_TRUE(Grapple_ReadChipStyleInfo(Grapple_GetChipStyle("genesis-sonic"), &info));
    EXPECT_TRUE(info.approximation);
}

TEST(ChipStyle, UserJsonLoadsAndRegisters)
{
    const char *json = R"({
      "schema": 1,
      "id": "test-pulse",
      "name": "Test pulse",
      "roles": {
        "lead": { "oscillator": "pulse25", "filter": "off", "pwm": 0, "vibrato": 0 },
        "harmony": { "oscillator": "pulse50", "filter": "off" },
        "bass": { "oscillator": "triangle", "triangle": "nes", "filter": "off" },
        "drums": { "oscillator": "noise", "filter": "off", "drums": "none" }
      }
    })";
    Grapple_ChipStyle *style = Grapple_LoadChipStyleMemory(json, SDL_strlen(json));
    ASSERT_TRUE(style) << SDL_GetError();
    ASSERT_TRUE(Grapple_RegisterChipStyle(style));
    EXPECT_TRUE(Grapple_GetChipStyle("test-pulse"));
}

TEST(ChipStyle, MissingRequiredRoleFails)
{
    const char *json =
        R"({"schema":1,"id":"x","name":"x","roles":{"lead":{"oscillator":"pulse"}}})";
    EXPECT_FALSE(Grapple_LoadChipStyleMemory(json, SDL_strlen(json)));
}

TEST(ChipStyle, ApplyNesCapsPeakVoices)
{
    auto song = LoadNamed();
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 8000, 64, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    ASSERT_TRUE(Grapple_SetChipPlayerStyle(player.get(), Grapple_GetChipStyle("nes-smb")))
        << SDL_GetError();
    std::vector<float> pcm(8000 * 2);
    ASSERT_GT(Grapple_RenderChipPlayer(player.get(), pcm.data(), 8000), 0);
    EXPECT_LE(Grapple_GetChipPlayerPeakVoices(player.get()), 4);
}

TEST(ChipStyle, NoiseRoleWordMapsBeforeLead)
{
    Grapple_ChipComposer *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_TRUE(composer);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 0, "Sound Effects", GRAPPLE_CHIP_PRESET_AUTO, 1));
    Grapple_ChipNote note{0, 60, 90, 0, 480};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    Song song(Grapple_BuildChipSong(composer, 480), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 8000, 8, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    Grapple_ChipMapping mapping{};
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.preset, GRAPPLE_CHIP_PRESET_NOISE);
}

TEST(ChipStyle, AffectionDoesNotMatchEffect)
{
    Grapple_ChipComposer *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_TRUE(composer);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 0, "Affection", GRAPPLE_CHIP_PRESET_AUTO, 1));
    Grapple_ChipNote note{0, 60, 90, 0, 480};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    Song song(Grapple_BuildChipSong(composer, 480), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 8000, 8, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    Grapple_ChipMapping mapping{};
    ASSERT_TRUE(Grapple_ReadChipTrackMapping(player.get(), 0, 0, &mapping));
    EXPECT_EQ(mapping.preset, GRAPPLE_CHIP_PRESET_LEAD);
}

TEST(ChipStyle, SetChipPartNoiseSucceeds)
{
    Grapple_ChipComposer *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_TRUE(composer);
    EXPECT_TRUE(Grapple_SetChipPart(composer, 0, "fx", GRAPPLE_CHIP_PRESET_NOISE, 1));
    Grapple_DestroyChipComposer(composer);
}

TEST(ChipStyle, RoleControlsAndPhaserRoundTrip)
{
    Grapple_ChipComposer *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_TRUE(composer);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 0, "lead", GRAPPLE_CHIP_PRESET_LEAD, 1));
    Grapple_ChipNote note{0, 60, 90, 0, 480};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    Song song(Grapple_BuildChipSong(composer, 480), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 8000, 8, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    Grapple_ChipRoleControls controls{};
    ASSERT_TRUE(Grapple_ReadChipRoleControls(player.get(), GRAPPLE_CHIP_PRESET_LEAD, &controls));
    controls.level = 1.25f;
    controls.duty = 0.25f;
    controls.effects.phaser = 0.4f;
    controls.effects.flanger = 0.3f;
    ASSERT_TRUE(Grapple_SetChipRoleControls(player.get(), GRAPPLE_CHIP_PRESET_LEAD, &controls))
        << SDL_GetError();
    Grapple_ChipRoleControls again{};
    ASSERT_TRUE(Grapple_ReadChipRoleControls(player.get(), GRAPPLE_CHIP_PRESET_LEAD, &again));
    EXPECT_FLOAT_EQ(again.level, 1.25f);
    EXPECT_FLOAT_EQ(again.duty, 0.25f);
    EXPECT_FLOAT_EQ(again.effects.phaser, 0.4f);
    EXPECT_FLOAT_EQ(again.effects.flanger, 0.3f);
    std::vector<float> pcm(256);
    EXPECT_GT(Grapple_RenderChipPlayer(player.get(), pcm.data(), 128), 0);
}
