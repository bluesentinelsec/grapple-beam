// Scene lighting: presets, the clock, and lights that ride on actors.
//
// The headless engine has a software renderer, so the lighting module's
// geometry path runs for real. What these check is the wiring the engine
// adds: that a light follows its actor, that the quality budget is applied
// without the game asking, and that lighting off costs nothing.
#include <grapple/engine_graphics.h>
#include <grapple/engine_light.h>
#include <grapple/engine_physics.h>

#include <gtest/gtest.h>

namespace
{

class LightHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        Grapple_EngineConfig config{};
        config.headless = true;
        config.manual_clock = true;
        config.no_auto_mount = true;
        config.design_width = 640;
        config.design_height = 360;
        config.window_width = 640;
        config.window_height = 360;
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
        Grapple_CameraInit(&camera_, engine_);
        Grapple_CameraSnap(&camera_, 320.0f, 180.0f);
        Grapple_CameraUpdate(&camera_, engine_, 0.0f);
    }
    void TearDown() override
    {
        Grapple_DestroyEngine(engine_);
        SDL_Quit();
    }

    Grapple_ActorId Spawn(float x, float y)
    {
        Grapple_ActorDef def{};
        def.type = "lamp";
        def.x = x;
        def.y = y;
        const Grapple_ActorId id = Grapple_ActorSpawn(engine_, &def);
        Frames(1);
        return id;
    }

    void Frames(int count)
    {
        for (int i = 0; i < count; ++i)
        {
            Grapple_EngineAdvance(engine_, 16666667ull);
            Grapple_EngineTick(engine_);
        }
    }

    Grapple_Engine *engine_ = nullptr;
    Grapple_Camera camera_{};
};

// --- presets --------------------------------------------------------------

TEST_F(LightHarness, LightingIsOffUntilAskedFor)
{
    EXPECT_EQ(Grapple_LightPreset_(engine_), GRAPPLE_LIGHT_NONE);
    // Nothing rendered, and no complaint: a game may call this every frame
    // whether or not it uses lighting.
    EXPECT_FALSE(Grapple_LightRender(engine_, &camera_, 1.0f));
}

TEST_F(LightHarness, PresetsSetAnAmbientAndAMatchingHour)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);
    EXPECT_EQ(Grapple_LightPreset_(engine_), GRAPPLE_LIGHT_NIGHT);
    const SDL_FColor night = Grapple_LightAmbient(engine_);
    EXPECT_LT(night.r, 0.5f) << "night is dark";
    EXPECT_LT(Grapple_LightSunlight(engine_), 0.2f) << "and the sun is down";

    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_AFTERNOON);
    const SDL_FColor day = Grapple_LightAmbient(engine_);
    EXPECT_GT(day.r, night.r) << "afternoon is brighter than night";
    EXPECT_GT(Grapple_LightSunlight(engine_), 0.8f);

    // Sunset is warm: more red than blue.
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_SUNSET);
    const SDL_FColor dusk = Grapple_LightAmbient(engine_);
    EXPECT_GT(dusk.r, dusk.b);
}

// A cave is dark at noon, so DARK is not on the day/night curve — but not
// pitch black either, or a player with no torch cannot tell the game from a
// crash.
TEST_F(LightHarness, DarkIsDarkButNotInvisible)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_DARK);
    const SDL_FColor dark = Grapple_LightAmbient(engine_);
    EXPECT_LT(dark.r, 0.1f);
    EXPECT_GT(dark.r, 0.0f) << "not absolute black";
}

TEST_F(LightHarness, ACustomAmbientTurnsLightingOnAndSticks)
{
    const SDL_FColor purple{0.3f, 0.1f, 0.4f, 1.0f};
    Grapple_LightSetAmbient(engine_, purple);
    EXPECT_FLOAT_EQ(Grapple_LightAmbient(engine_).b, 0.4f);
    EXPECT_NE(Grapple_LightPreset_(engine_), GRAPPLE_LIGHT_NONE)
        << "asking for an ambient means asking for lighting";

    // The clock must not quietly overwrite what the game asked for.
    Grapple_LightSetClock(engine_, 12.0f, 5.0f);
    Frames(30);
    EXPECT_FLOAT_EQ(Grapple_LightAmbient(engine_).b, 0.4f) << "still the game's colour";
}

// --- the clock ------------------------------------------------------------

TEST_F(LightHarness, TheClockRunsAndWraps)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_AFTERNOON);
    Grapple_LightSetClock(engine_, 23.5f, 60.0f); // an hour a second
    EXPECT_FLOAT_EQ(Grapple_LightHour(engine_), 23.5f);

    Frames(60); // one second: an hour passes, past midnight
    const float hour = Grapple_LightHour(engine_);
    EXPECT_GE(hour, 0.0f);
    EXPECT_LT(hour, 24.0f) << "wrapped rather than running off the end";
    EXPECT_LT(hour, 12.0f) << "and it is now the small hours";
}

TEST_F(LightHarness, AStoppedClockDoesNotMove)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_AFTERNOON);
    Grapple_LightSetClock(engine_, 9.0f, 0.0f);
    Frames(60);
    EXPECT_FLOAT_EQ(Grapple_LightHour(engine_), 9.0f);
}

TEST_F(LightHarness, TheAmbientFollowsTheClock)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_AFTERNOON);
    Grapple_LightSetClock(engine_, 12.0f, 0.0f);
    const SDL_FColor noon = Grapple_LightAmbient(engine_);

    Grapple_LightSetClock(engine_, 0.0f, 0.0f);
    const SDL_FColor midnight = Grapple_LightAmbient(engine_);
    EXPECT_GT(noon.r, midnight.r);
}

// --- lights on actors -----------------------------------------------------

TEST_F(LightHarness, ADefaultLightIsActuallyALight)
{
    const Grapple_LightDef def = Grapple_LightDefault();
    EXPECT_GT(def.radius, 0.0f) << "a zeroed struct would be nothing at all";
    EXPECT_GT(def.color.r, 0.0f);
    EXPECT_TRUE(def.enabled);
}

TEST_F(LightHarness, LightsAttachToActorsAndAreSubmitted)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);

    const Grapple_ActorId id = Spawn(320.0f, 180.0f);
    Grapple_LightDef torch = Grapple_LightDefault();
    torch.radius = 260.0f;
    ASSERT_TRUE(Grapple_ActorAddLight(Grapple_ActorGet(engine_, id), &torch));

    ASSERT_TRUE(Grapple_LightRender(engine_, &camera_, 1.0f));
    EXPECT_EQ(Grapple_LightCount(engine_), 1);

    // Changed in place, for a torch that burns down.
    Grapple_ActorLight(Grapple_ActorGet(engine_, id))->enabled = false;
    Grapple_LightRender(engine_, &camera_, 1.0f);
    EXPECT_EQ(Grapple_LightCount(engine_), 0);

    Grapple_ActorRemoveLight(Grapple_ActorGet(engine_, id));
    EXPECT_EQ(Grapple_ActorLight(Grapple_ActorGet(engine_, id)), nullptr);
}

// A light on a destroyed actor must stop being submitted, without the game
// having to remember to detach it.
TEST_F(LightHarness, ALightDiesWithItsActor)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);
    const Grapple_ActorId id = Spawn(320.0f, 180.0f);
    const Grapple_LightDef torch = Grapple_LightDefault();
    Grapple_ActorAddLight(Grapple_ActorGet(engine_, id), &torch);

    Grapple_LightRender(engine_, &camera_, 1.0f);
    ASSERT_EQ(Grapple_LightCount(engine_), 1);

    Grapple_ActorDestroy(engine_, id);
    Frames(1);
    Grapple_LightRender(engine_, &camera_, 1.0f);
    EXPECT_EQ(Grapple_LightCount(engine_), 0);
}

// --- the quality budget ---------------------------------------------------

// The whole point of putting this in the engine: the player's slider is
// honoured without the game writing a conditional.
TEST_F(LightHarness, TheQualityBudgetCapsHowManyLightsAreSubmitted)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);

    for (int i = 0; i < 40; ++i)
    {
        const Grapple_ActorId id = Spawn(100.0f + static_cast<float>(i) * 8.0f, 180.0f);
        const Grapple_LightDef torch = Grapple_LightDefault();
        Grapple_ActorAddLight(Grapple_ActorGet(engine_, id), &torch);
    }

    Grapple_GraphicsSettings gfx = *Grapple_EngineGraphics(engine_);
    gfx.dynamic_lights = GRAPPLE_QUALITY_LOW;
    Grapple_EngineSetGraphics(engine_, &gfx);

    Grapple_LightRender(engine_, &camera_, 1.0f);
    const int low = Grapple_LightCount(engine_);
    EXPECT_GT(low, 0);
    EXPECT_LE(low, Grapple_GraphicsMaxDynamicLights(GRAPPLE_QUALITY_LOW));

    gfx.dynamic_lights = GRAPPLE_QUALITY_HIGH;
    Grapple_EngineSetGraphics(engine_, &gfx);
    Grapple_LightRender(engine_, &camera_, 1.0f);
    EXPECT_GT(Grapple_LightCount(engine_), low) << "a higher budget shows more";
}

TEST_F(LightHarness, TurningDynamicLightsOffSubmitsNothing)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);
    const Grapple_ActorId id = Spawn(320.0f, 180.0f);
    const Grapple_LightDef torch = Grapple_LightDefault();
    Grapple_ActorAddLight(Grapple_ActorGet(engine_, id), &torch);

    Grapple_GraphicsSettings gfx = *Grapple_EngineGraphics(engine_);
    gfx.dynamic_lights = GRAPPLE_QUALITY_OFF;
    Grapple_EngineSetGraphics(engine_, &gfx);

    Grapple_LightRender(engine_, &camera_, 1.0f);
    EXPECT_EQ(Grapple_LightCount(engine_), 0) << "ambient only, and it costs nothing";
}

// --- occluders ------------------------------------------------------------

// Occluders are buffered, so submitting them anywhere in the frame works
// rather than only inside an invisible window.
TEST_F(LightHarness, OccludersMayBeSubmittedBeforeTheRender)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);
    Grapple_LightAddOccluder(engine_, SDL_FRect{100.0f, 100.0f, 40.0f, 200.0f});
    Grapple_LightAddOccluderLine(engine_, 0.0f, 0.0f, 100.0f, 100.0f);
    Grapple_LightAddDarkZone(engine_, SDL_FRect{0.0f, 0.0f, 50.0f, 50.0f},
                               SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f});
    EXPECT_TRUE(Grapple_LightRender(engine_, &camera_, 1.0f));
}

// Submitting walls while lighting is off must not pile them up forever.
TEST_F(LightHarness, BufferedOccludersAreDroppedWhenLightingIsOff)
{
    for (int i = 0; i < 500; ++i)
    {
        Grapple_LightAddOccluder(engine_, SDL_FRect{0.0f, 0.0f, 10.0f, 10.0f});
    }
    EXPECT_FALSE(Grapple_LightRender(engine_, &camera_, 1.0f));
    // And again, to show the buffer was emptied rather than growing.
    for (int i = 0; i < 500; ++i)
    {
        Grapple_LightAddOccluder(engine_, SDL_FRect{0.0f, 0.0f, 10.0f, 10.0f});
    }
    EXPECT_FALSE(Grapple_LightRender(engine_, &camera_, 1.0f));
}

// Static bodies double as walls, because a level's collision is usually
// exactly what should block light.
TEST_F(LightHarness, StaticBodiesBecomeOccludersOnTheirOwn)
{
    Grapple_LightSetPreset(engine_, GRAPPLE_LIGHT_NIGHT);

    Grapple_ActorDef wall_def{};
    wall_def.type = "wall";
    wall_def.x = 320.0f;
    wall_def.y = 180.0f;
    const Grapple_ActorId wall = Grapple_ActorSpawn(engine_, &wall_def);
    Grapple_BodyDef body = Grapple_BodyDefault();
    body.type = GRAPPLE_BODY_STATIC;
    body.width = 40.0f;
    body.height = 200.0f;
    ASSERT_TRUE(Grapple_ActorAddBody(Grapple_ActorGet(engine_, wall), &body));
    Frames(1);

    SDL_FRect bounds{};
    ASSERT_TRUE(Grapple_ActorBodyBounds(Grapple_ActorGet(engine_, wall), &bounds));
    EXPECT_NEAR(bounds.w, 40.0f, 4.0f) << "in design units, not metres";
    EXPECT_NEAR(bounds.h, 200.0f, 4.0f);

    EXPECT_TRUE(Grapple_LightRender(engine_, &camera_, 1.0f));

    // And a game that disagrees can turn it off.
    Grapple_LightSetAutoOccluders(engine_, false);
    EXPECT_TRUE(Grapple_LightRender(engine_, &camera_, 1.0f));
}

TEST_F(LightHarness, SamplingLightIsSafeWithoutAScene)
{
    // An unlit world is fully lit, which is what "no lighting" means.
    EXPECT_FLOAT_EQ(Grapple_LightAt(engine_, 10.0f, 10.0f), 1.0f);
}

TEST_F(LightHarness, NullsAreHandled)
{
    Grapple_LightSetPreset(nullptr, GRAPPLE_LIGHT_NIGHT);
    EXPECT_EQ(Grapple_LightPreset_(nullptr), GRAPPLE_LIGHT_NONE);
    EXPECT_FLOAT_EQ(Grapple_LightAmbient(nullptr).r, 1.0f);
    EXPECT_FLOAT_EQ(Grapple_LightHour(nullptr), 0.0f);
    EXPECT_FALSE(Grapple_ActorAddLight(nullptr, nullptr));
    EXPECT_EQ(Grapple_ActorLight(nullptr), nullptr);
    Grapple_ActorRemoveLight(nullptr);
    EXPECT_FALSE(Grapple_LightRender(nullptr, nullptr, 0.0f));
    EXPECT_EQ(Grapple_LightCount(nullptr), 0);
    SDL_FRect bounds{};
    EXPECT_FALSE(Grapple_ActorBodyBounds(nullptr, &bounds));
}

} // namespace
