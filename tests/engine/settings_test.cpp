#include <grapple/engine_camera.h>
#include <grapple/engine_settings.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
namespace
{
using Settings = std::unique_ptr<Grapple_Settings, decltype(&Grapple_DestroySettings)>;
Settings Defaults()
{
    return Settings(Grapple_CreateSettings(), Grapple_DestroySettings);
}
} // namespace
TEST(Settings, OverlaysPreserveFalseZeroAndOrigins)
{
    auto settings = Defaults(), overlay = Defaults();
    ASSERT_TRUE(Grapple_SettingsSet(settings.get(), "bloom", ".8", "game"));
    ASSERT_TRUE(Grapple_SettingsSet(overlay.get(), "vsync", "off", "player"));
    ASSERT_TRUE(Grapple_SettingsSet(overlay.get(), "bloom", "0", "player"));
    ASSERT_TRUE(Grapple_SettingsOverlay(settings.get(), overlay.get()));
    EXPECT_STREQ(Grapple_SettingsGet(settings.get(), "vsync"), "off");
    EXPECT_STREQ(Grapple_SettingsGet(settings.get(), "bloom"), "0");
    EXPECT_STREQ(Grapple_SettingsSource(settings.get(), "bloom"), "player");
    EXPECT_STREQ(Grapple_SettingsSource(settings.get(), "crt"), "engine defaults");
}
TEST(Settings, InvalidTomlIsAtomicAndUnknownKeysFail)
{
    auto s = Defaults();
    EXPECT_FALSE(
        Grapple_SettingsLoadToml(s.get(), "[display]\nvsync=false\nmax_fps=\"oops\"", "bad.toml"));
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "vsync"), "on");
    EXPECT_FALSE(Grapple_SettingsLoadToml(s.get(), "[audio]\nmisspelled=0", "bad.toml"));
    EXPECT_FALSE(Grapple_SettingsLoadToml(s.get(), "[effects]\nbloom=nan", "bad.toml"));
    EXPECT_TRUE(Grapple_SettingsLoadToml(
        s.get(), "[audio]\nmute=true\nmusic_volume=0\n[display]\nvsync=false", "valid.toml"));
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "music-volume"), "0");
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "vsync"), "off");
}
TEST(Settings, SparseTomlRoundTripDoesNotSaveDefaults)
{
    auto s = Defaults(), restored = Defaults();
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "engine.title", "A \"quoted\" title", "player"));
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "effects", "off", "player"));
    char *text = Grapple_SettingsToToml(s.get(), true);
    ASSERT_NE(text, nullptr);
    EXPECT_EQ(std::string(text).find("vsync"), std::string::npos);
    ASSERT_TRUE(Grapple_SettingsLoadToml(restored.get(), text, "saved")) << SDL_GetError();
    SDL_free(text);
    EXPECT_STREQ(Grapple_SettingsGet(restored.get(), "engine.title"), "A \"quoted\" title");
    EXPECT_STREQ(Grapple_SettingsGet(restored.get(), "effects"), "off");
}
TEST(Settings, LaunchOverridesReachEngineAndSnapshotsSurviveLaunchScope)
{
    auto s = Defaults();
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "engine.tick_rate", "120", "CLI"));
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "window-size", "640x360", "CLI"));
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "bloom", "1", "CLI"));
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "reduced-flashing", "on", "CLI"));
    Grapple_EngineConfig config{};
    config.headless = true;
    config.no_auto_mount = true;
    config.tick_rate = 30;
    Grapple_SetLaunchSettings(s.get());
    auto *engine = Grapple_CreateEngine(&config);
    Grapple_SetLaunchSettings(nullptr);
    s.reset();
    ASSERT_NE(engine, nullptr) << SDL_GetError();
    auto *requested = Grapple_EngineRequestedSettings(engine);
    EXPECT_STREQ(Grapple_SettingsGet(requested, "engine.tick_rate"), "120");
    EXPECT_STREQ(Grapple_SettingsSource(requested, "engine.tick_rate"), "CLI");
    Settings actual(Grapple_EngineActualSettings(engine), Grapple_DestroySettings);
    ASSERT_NE(actual, nullptr);
    EXPECT_STREQ(Grapple_SettingsGet(actual.get(), "engine.tick_rate"), "120");
    EXPECT_STREQ(Grapple_SettingsGet(actual.get(), "backend"), "software");
    EXPECT_LT(Grapple_EngineGraphics(engine)->bloom, 1.0f);
    EXPECT_STREQ(Grapple_SettingsGet(requested, "bloom"), "1");
    Grapple_DestroyEngine(engine);
}
TEST(Settings, QualityExpansionCanBeOverriddenWithoutChangingOtherBudgets)
{
    auto s = Defaults();
    ASSERT_TRUE(Grapple_SettingsQuality(s.get(), "low", "preset"));
    ASSERT_TRUE(Grapple_SettingsSet(s.get(), "shadows", "high", "CLI"));
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "lights"), "low");
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "shadows"), "high");
    EXPECT_STREQ(Grapple_SettingPolicy("backend"), "restart");
    EXPECT_STREQ(Grapple_SettingPolicy("vsync"), "graphics");
    EXPECT_STREQ(Grapple_SettingPolicy("music-volume"), "audio");
}

TEST(Settings, PlayerSaveMergesOnlyChangesAndPreservesExistingPreferences)
{
    auto launch = Defaults(), changes = Defaults(), saved = Defaults();
    char *pref = SDL_GetPrefPath("grapple-tests", "settings-save");
    ASSERT_NE(pref, nullptr);
    std::string path = std::string(pref) + "test-" + std::to_string(SDL_GetTicksNS()) + ".toml";
    SDL_free(pref);
    ASSERT_TRUE(Grapple_SettingsSet(changes.get(), "music-volume", ".5", "player"));
    ASSERT_TRUE(Grapple_SettingsSaveChanges(changes.get(), path.c_str())) << SDL_GetError();
    ASSERT_TRUE(Grapple_SettingsSet(launch.get(), "vsync", "off", "CLI"));
    changes = Defaults();
    ASSERT_TRUE(Grapple_SettingsSet(changes.get(), "bloom", ".4", "player"));
    ASSERT_TRUE(Grapple_SettingsSaveChanges(changes.get(), path.c_str())) << SDL_GetError();
    ASSERT_TRUE(Grapple_SettingsLoadFile(saved.get(), path.c_str()));
    EXPECT_STREQ(Grapple_SettingsGet(saved.get(), "music-volume"), ".5");
    EXPECT_STREQ(Grapple_SettingsGet(saved.get(), "bloom"), ".4");
    EXPECT_STREQ(Grapple_SettingsSource(saved.get(), "vsync"), "engine defaults");
    EXPECT_TRUE(SDL_RemovePath(path.c_str()));
}

TEST(Settings, LegacyGraphicsTomlMigratesToTypedKeys)
{
    auto s = Defaults();
    ASSERT_TRUE(
        Grapple_SettingsLoadToml(s.get(),
                                 "[display]\nwindow_mode=\"borderless\"\nmax_fps=-1\nwindow_width="
                                 "960\nwindow_height=540\n[image]\nbrightness=1.2",
                                 "legacy"))
        << SDL_GetError();
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "window-mode"), "fullscreen-borderless");
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "window-size"), "960x540");
    EXPECT_STREQ(Grapple_SettingsGet(s.get(), "max-fps"), "unlimited");
}

TEST(Settings, CameraAndUiHelpersHonorAccessibility)
{
    Grapple_EngineConfig config{};
    config.headless = true;
    config.no_auto_mount = true;
    config.design_width = config.design_height = 64;
    auto *engine = Grapple_CreateEngine(&config);
    ASSERT_NE(engine, nullptr);
    auto graphics = *Grapple_EngineGraphics(engine);
    graphics.screen_shake = 0;
    graphics.ui_scale = 2;
    ASSERT_TRUE(Grapple_EngineSetGraphics(engine, &graphics));
    Grapple_Camera camera{};
    Grapple_CameraInit(&camera, engine);
    Grapple_CameraShake(&camera, 100, 1);
    Grapple_CameraUpdate(&camera, engine, .1f);
    EXPECT_FLOAT_EQ(camera.shake_scale, 0);
    EXPECT_FLOAT_EQ(Grapple_EngineUiPoints(engine, 12), 24);
    Grapple_DestroyEngine(engine);
}

TEST(Settings, EffectsGatePreservesStyleAndReportsUnsupportedCorrections)
{
    Grapple_EngineConfig config{};
    config.headless = true;
    config.no_auto_mount = true;
    config.design_width = config.design_height = 64;
    auto *engine = Grapple_CreateEngine(&config);
    ASSERT_NE(engine, nullptr);
    auto graphics = *Grapple_EngineGraphics(engine);
    graphics.effects_disabled = true;
    graphics.bloom = .8f;
    graphics.brightness = 1.4f;
    ASSERT_TRUE(Grapple_EngineSetGraphics(engine, &graphics));
    EXPECT_FLOAT_EQ(Grapple_EngineGraphics(engine)->bloom, .8f);
    EXPECT_FLOAT_EQ(Grapple_EngineGraphics(engine)->brightness, 1.4f);
    Settings actual(Grapple_EngineActualSettings(engine), Grapple_DestroySettings);
    ASSERT_NE(actual, nullptr);
    EXPECT_STREQ(Grapple_SettingsGet(actual.get(), "bloom"), "0");
    EXPECT_STREQ(Grapple_SettingsGet(actual.get(), "brightness"), "1");
    Grapple_DestroyEngine(engine);
}

TEST(Settings, EveryRegisteredDefaultValidatesAndRoundTrips)
{
    auto s = Defaults(), restored = Defaults();
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        const auto *key = Grapple_SettingKey(i);
        const std::string value = Grapple_SettingsGet(s.get(), key);
        if (value.empty())
            continue;
        ASSERT_TRUE(Grapple_SettingsSet(restored.get(), key, value.c_str(), "registry")) << key;
        EXPECT_STREQ(Grapple_SettingsGet(restored.get(), key), value.c_str());
        ASSERT_NE(Grapple_SettingPolicy(key), nullptr);
    }
    EXPECT_FALSE(Grapple_SettingsSet(s.get(), "vsync", "false", "CLI"));
    EXPECT_FALSE(Grapple_SettingsSet(s.get(), "engine.tick_rate", "0", "CLI"));
    EXPECT_FALSE(Grapple_SettingsSet(s.get(), "render-scale", "1garbage", "CLI"));
    EXPECT_FALSE(Grapple_SettingsSet(s.get(), "engine.headless", "", "CLI"));
}
