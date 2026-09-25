// platformer_test.cpp — Grapple::Platformer against a headless engine with an
// injected clock, so every test is a deterministic replay: N frames of a
// scripted controller produce exactly one outcome.
#include <SDL3/SDL.h>
#include <cmath>
#include <grapple/engine_actor.h>
#include <grapple/engine_scene.h>
#include <grapple/engine_script.h>
#include <grapple/platformer.h>
#include <gtest/gtest.h>
#include <string>

#ifdef PLATFORMER_TEST_CPP
#include "grapple/platformer_level.h"
#endif

namespace
{

constexpr int kTile = 16;
constexpr Uint64 kFrameNs = 16666667ull; // one 60 Hz frame

class PlatformerHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        Grapple_EngineConfig config{};
        config.headless = true;
        config.manual_clock = true;
        config.no_auto_mount = true;
        config.design_width = 256;
        config.design_height = 240;
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
    }
    void TearDown() override
    {
        if (level_ != nullptr)
        {
            Grapple_DestroyPlatformer(level_);
        }
        Grapple_DestroyEngine(engine_);
        SDL_Quit();
    }

    // A 20x10 level with a floor along row 8: the player stands at y = 128.
    void MakeLevel()
    {
        level_ = Grapple_CreatePlatformer(engine_, 20, 10, kTile);
        ASSERT_NE(level_, nullptr) << SDL_GetError();
        ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 8, 20, 0));
        ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 2, 7), GRAPPLE_ACTOR_NONE)
            << SDL_GetError();
        Grapple_PlatformerSetScriptedInput(level_, true);
        ASSERT_TRUE(Grapple_PlatformerAttach(level_)) << SDL_GetError();
    }

    void Frames(int count, float move = 0.0f, bool jump = false, bool run = false)
    {
        Grapple_PlatformerScriptInput(level_, move, jump, run);
        for (int i = 0; i < count; ++i)
        {
            Grapple_EngineAdvance(engine_, kFrameNs);
            Grapple_EngineTick(engine_);
        }
    }

    float X() const
    {
        float x = 0.0f;
        Grapple_PlatformerPlayerPosition(level_, &x, nullptr);
        return x;
    }
    float Y() const
    {
        float y = 0.0f;
        Grapple_PlatformerPlayerPosition(level_, nullptr, &y);
        return y;
    }
    float Vx() const
    {
        float v = 0.0f;
        Grapple_PlatformerPlayerVelocity(level_, &v, nullptr);
        return v;
    }
    float Vy() const
    {
        float v = 0.0f;
        Grapple_PlatformerPlayerVelocity(level_, nullptr, &v);
        return v;
    }
    // Land the player from its spawn: a few frames of nothing.
    void Settle()
    {
        Frames(20);
        ASSERT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
        ASSERT_FLOAT_EQ(Y(), 8.0f * kTile);
    }

    Grapple_Engine *engine_ = nullptr;
    Grapple_Platformer *level_ = nullptr;
};

// --- the grid --------------------------------------------------------------

TEST(PlatformerGrid, DedicatedObjectsFillTheCellsTheirNamesPromise)
{
    ASSERT_TRUE(SDL_Init(0));
    Grapple_EngineConfig config{};
    config.headless = true;
    config.manual_clock = true;
    config.no_auto_mount = true;
    Grapple_Engine *engine = Grapple_CreateEngine(&config);
    ASSERT_NE(engine, nullptr);
    Grapple_Platformer *level = Grapple_CreatePlatformer(engine, 16, 10, kTile);
    ASSERT_NE(level, nullptr);

    EXPECT_TRUE(Grapple_PlatformerCreateFloor(level, 0, 8, 16, 0));
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 0, 8), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 15, 9), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 0, 7), GRAPPLE_PLATFORMER_EMPTY);

    EXPECT_TRUE(Grapple_PlatformerCreateWall(level, 5, 4, 4));
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 5, 4), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 5, 7), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 5, 3), GRAPPLE_PLATFORMER_EMPTY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 6, 5), GRAPPLE_PLATFORMER_EMPTY);

    EXPECT_TRUE(Grapple_PlatformerCreateBlock(level, 8, 4, 3, 1));
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 10, 4), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 11, 4), GRAPPLE_PLATFORMER_EMPTY);

    // Rising stairs: column i is i + 1 tall, all standing on row 7.
    EXPECT_TRUE(Grapple_PlatformerCreateStairs(level, 12, 7, 3, true));
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 12, 7), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 12, 6), GRAPPLE_PLATFORMER_EMPTY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 13, 6), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 14, 5), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 14, 4), GRAPPLE_PLATFORMER_EMPTY);

    EXPECT_TRUE(Grapple_PlatformerCreatePlatform(level, 1, 5, 3));
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 2, 5), GRAPPLE_PLATFORMER_ONE_WAY);

    // The sides are walls, the top and bottom are open.
    EXPECT_EQ(Grapple_PlatformerTileAt(level, -1, 3), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 16, 3), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 3, -1), GRAPPLE_PLATFORMER_EMPTY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level, 3, 10), GRAPPLE_PLATFORMER_EMPTY);

    // Fills clip rather than fail, and report what they touched.
    EXPECT_EQ(Grapple_PlatformerFillTiles(level, 14, 0, 10, 1, GRAPPLE_PLATFORMER_SOLID), 2);
    EXPECT_FALSE(Grapple_PlatformerSetTile(level, 99, 0, GRAPPLE_PLATFORMER_SOLID));

    const int id = Grapple_PlatformerAddSolid(level, 10.0f, 10.0f, 4.0f, 4.0f);
    EXPECT_GE(id, 0);
    EXPECT_EQ(Grapple_PlatformerSolidCount(level), 1);
    EXPECT_TRUE(Grapple_PlatformerRemoveSolid(level, id));
    EXPECT_FALSE(Grapple_PlatformerRemoveSolid(level, id));
    EXPECT_EQ(Grapple_PlatformerSolidCount(level), 0);

    Grapple_DestroyPlatformer(level);
    Grapple_DestroyEngine(engine);
    SDL_Quit();
}

// --- movement --------------------------------------------------------------

TEST_F(PlatformerHarness, PlayerFallsOntoTheFloorAndIdles)
{
    MakeLevel();
    EXPECT_FLOAT_EQ(X(), 2.5f * kTile);
    Frames(40);
    EXPECT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
    EXPECT_FLOAT_EQ(Y(), 8.0f * kTile);
    EXPECT_FLOAT_EQ(Vy(), 0.0f);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_IDLE);
    EXPECT_STREQ(Grapple_PlatformerPlayerStateName(level_), "idle");
    EXPECT_EQ(Grapple_ActorFindByType(engine_, "player"), Grapple_PlatformerPlayer(level_));
}

TEST_F(PlatformerHarness, WalkingAcceleratesToWalkSpeedAndRunningGoesFaster)
{
    MakeLevel();
    Settle();
    Frames(60, 1.0f);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_WALK);
    EXPECT_NEAR(Vx(), Grapple_PlatformerPlayerTuning(level_, "walk_speed"), 0.01f);
    Frames(60, 1.0f, false, true);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_RUN);
    EXPECT_NEAR(Vx(), Grapple_PlatformerPlayerTuning(level_, "run_speed"), 0.01f);
    EXPECT_EQ(Grapple_PlatformerPlayerFacing(level_), 1);
    Frames(1, -1.0f);
    EXPECT_EQ(Grapple_PlatformerPlayerFacing(level_), -1);
}

TEST_F(PlatformerHarness, WallStopsTheWalkAtItsEdge)
{
    MakeLevel();
    ASSERT_TRUE(Grapple_PlatformerCreateWall(level_, 8, 4, 4));
    Settle();
    Frames(180, 1.0f, false, true);
    float width = 0.0f;
    Grapple_PlatformerPlayerSize(level_, &width, nullptr);
    EXPECT_FLOAT_EQ(X(), 8.0f * kTile - width * 0.5f);
    EXPECT_FLOAT_EQ(Vx(), 0.0f);
    EXPECT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
}

TEST_F(PlatformerHarness, LevelEdgesAreWalls)
{
    MakeLevel();
    Settle();
    Frames(180, -1.0f, false, true);
    float width = 0.0f;
    Grapple_PlatformerPlayerSize(level_, &width, nullptr);
    EXPECT_FLOAT_EQ(X(), width * 0.5f);
}

TEST_F(PlatformerHarness, HeldJumpReachesFourTilesAndLands)
{
    MakeLevel();
    Settle();
    const float floor_y = Y();
    Frames(1, 0.0f, true);
    EXPECT_TRUE(Grapple_PlatformerPlayerJumped(level_));
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_JUMP);
    float apex = floor_y;
    bool landed = false;
    for (int i = 0; i < 120 && !landed; ++i)
    {
        Frames(1, 0.0f, true);
        apex = std::fmin(apex, Y());
        landed = Grapple_PlatformerPlayerLanded(level_);
    }
    EXPECT_TRUE(landed);
    const float height = floor_y - apex;
    // v²/2g with the defaults is exactly four tiles; the discrete integrator
    // lands within a fraction of a tile of that.
    EXPECT_NEAR(height, 4.0f * kTile, 0.4f * kTile);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_IDLE);
}

TEST_F(PlatformerHarness, TappedJumpIsLowerThanHeldJump)
{
    MakeLevel();
    Settle();
    const float floor_y = Y();
    Frames(2, 0.0f, true);
    float apex = floor_y;
    for (int i = 0; i < 90; ++i)
    {
        Frames(1, 0.0f, false);
        apex = std::fmin(apex, Y());
    }
    const float tapped = floor_y - apex;
    EXPECT_GT(tapped, 0.5f * kTile);
    EXPECT_LT(tapped, 2.0f * kTile);
    EXPECT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
}

TEST_F(PlatformerHarness, RunningJumpGoesHigher)
{
    MakeLevel();
    Settle();
    Frames(60, 1.0f, false, true);
    const float floor_y = Y();
    float apex = floor_y;
    for (int i = 0; i < 90; ++i)
    {
        Frames(1, 1.0f, true, true);
        apex = std::fmin(apex, Y());
    }
    EXPECT_GT(floor_y - apex, 4.4f * kTile);
}

TEST_F(PlatformerHarness, CeilingBumpStopsTheRise)
{
    MakeLevel();
    ASSERT_TRUE(Grapple_PlatformerCreateBlock(level_, 1, 5, 3, 1)); // two tiles of headroom
    Settle();
    bool bumped = false;
    for (int i = 0; i < 30 && !bumped; ++i)
    {
        Frames(1, 0.0f, true);
        bumped = Grapple_PlatformerPlayerBumped(level_);
    }
    EXPECT_TRUE(bumped);
    float height = 0.0f;
    Grapple_PlatformerPlayerSize(level_, nullptr, &height);
    EXPECT_FLOAT_EQ(Y() - height, 6.0f * kTile);
    EXPECT_GE(Vy(), 0.0f);
}

TEST_F(PlatformerHarness, OneWayLedgeIsPassedFromBelowAndStoodOnFromAbove)
{
    MakeLevel();
    ASSERT_TRUE(Grapple_PlatformerCreatePlatform(level_, 0, 6, 6)); // two tiles up
    Settle();
    // Jump up through it...
    bool above = false;
    for (int i = 0; i < 40 && !above; ++i)
    {
        Frames(1, 0.0f, true);
        above = (Y() <= 6.0f * kTile);
    }
    ASSERT_TRUE(above) << "never rose through the ledge";
    // ...and come back down onto it.
    Frames(60, 0.0f, false);
    EXPECT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
    EXPECT_FLOAT_EQ(Y(), 6.0f * kTile);
}

TEST_F(PlatformerHarness, CoyoteTimeAllowsAJumpJustAfterLeavingALedge)
{
    // Floor only under the first six columns: walk right, fall off, jump late.
    level_ = Grapple_CreatePlatformer(engine_, 20, 10, kTile);
    ASSERT_NE(level_, nullptr);
    ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 8, 6, 0));
    ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 4, 7), GRAPPLE_ACTOR_NONE);
    Grapple_PlatformerSetScriptedInput(level_, true);
    ASSERT_TRUE(Grapple_PlatformerAttach(level_));
    Settle();
    bool airborne = false;
    for (int i = 0; i < 120 && !airborne; ++i)
    {
        Frames(1, 1.0f, false, true);
        airborne = !Grapple_PlatformerPlayerGrounded(level_);
    }
    ASSERT_TRUE(airborne);
    Frames(2, 1.0f, false, true); // two frames late: still within 80 ms
    Frames(1, 1.0f, true, true);
    EXPECT_TRUE(Grapple_PlatformerPlayerJumped(level_));
    EXPECT_LT(Vy(), 0.0f);
}

TEST_F(PlatformerHarness, JumpBufferedBeforeLandingFiresOnLanding)
{
    MakeLevel();
    Settle();
    Frames(1, 0.0f, true);
    Frames(1, 0.0f, false);
    ASSERT_FALSE(Grapple_PlatformerPlayerGrounded(level_));
    // Fall most of the way, press early, release, and land.
    int frame = 0;
    while (!Grapple_PlatformerPlayerGrounded(level_) && frame < 200)
    {
        // Press while still 3 frames from landing: buffered.
        Frames(1, 0.0f, Vy() > 0.0f && Y() > 8.0f * kTile - 12.0f);
        frame++;
    }
    ASSERT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
    // The jump fires within the buffer window even with the button released.
    bool jumped = false;
    for (int i = 0; i < 3 && !jumped; ++i)
    {
        jumped = Grapple_PlatformerPlayerJumped(level_);
        if (!jumped)
        {
            Frames(1, 0.0f, false);
        }
    }
    EXPECT_TRUE(jumped);
}

TEST_F(PlatformerHarness, FallingOutOfTheLevelRespawnsAtTheStart)
{
    level_ = Grapple_CreatePlatformer(engine_, 20, 10, kTile);
    ASSERT_NE(level_, nullptr);
    ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 8, 4, 0));
    ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 2, 7), GRAPPLE_ACTOR_NONE);
    Grapple_PlatformerSetScriptedInput(level_, true);
    ASSERT_TRUE(Grapple_PlatformerAttach(level_));
    Settle();
    bool fell = false;
    for (int i = 0; i < 400 && !fell; ++i)
    {
        Frames(1, 1.0f, false, true);
        fell = Grapple_PlatformerPlayerFell(level_);
    }
    EXPECT_TRUE(fell);
    EXPECT_FLOAT_EQ(X(), 2.5f * kTile);
    EXPECT_FLOAT_EQ(Y(), 8.0f * kTile);
}

TEST_F(PlatformerHarness, PausedPlayerFreezesAndResumes)
{
    MakeLevel();
    Settle();
    Grapple_PlatformerSetPlayerPaused(level_, true);
    EXPECT_STREQ(Grapple_PlatformerPlayerStateName(level_), "paused");
    const float x = X();
    Frames(30, 1.0f, true, true);
    EXPECT_FLOAT_EQ(X(), x);
    Grapple_PlatformerSetPlayerPaused(level_, false);
    Frames(30, 1.0f);
    EXPECT_GT(X(), x);
}

TEST_F(PlatformerHarness, StateChangeIsReportedForTheWholeFrame)
{
    MakeLevel();
    Settle();
    Frames(1, 1.0f);
    EXPECT_TRUE(Grapple_PlatformerPlayerStateChanged(level_));
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_WALK);
    Frames(1, 1.0f);
    EXPECT_FALSE(Grapple_PlatformerPlayerStateChanged(level_));
}

// --- walls -------------------------------------------------------------------

// A shaft: walls at columns 4 and 8 from the top of the level down to the
// floor at row 22, a lid across the top, three tiles of air between them,
// the player at the bottom.
class WallHarness : public PlatformerHarness
{
  protected:
    void MakeShaft()
    {
        level_ = Grapple_CreatePlatformer(engine_, 20, 24, kTile);
        ASSERT_NE(level_, nullptr);
        ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 22, 20, 0));
        ASSERT_TRUE(Grapple_PlatformerCreateWall(level_, 4, 0, 22));
        ASSERT_TRUE(Grapple_PlatformerCreateWall(level_, 8, 0, 22));
        ASSERT_TRUE(Grapple_PlatformerCreateBlock(level_, 4, 0, 5, 1));
        ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 6, 21), GRAPPLE_ACTOR_NONE);
        Grapple_PlatformerSetScriptedInput(level_, true);
        ASSERT_TRUE(Grapple_PlatformerAttach(level_));
        Frames(20);
        ASSERT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
    }

    bool Sliding() const
    {
        return Grapple_PlatformerPlayerState(level_) == GRAPPLE_PLAYER_WALL_SLIDE;
    }

    // A full jump toward the right wall, holding right, until the slide
    // starts; the jump button is then released while still sliding.
    void JumpOntoRightWall()
    {
        Frames(1, 1.0f, true);
        for (int i = 0; i < 90 && !Sliding(); ++i)
        {
            Frames(1, 1.0f, true);
        }
        ASSERT_TRUE(Sliding()) << "never reached the wall";
        Frames(1, 1.0f, false);
        ASSERT_TRUE(Sliding());
    }
};

TEST_F(WallHarness, HoldingIntoAWallWhileFallingSlidesDownIt)
{
    MakeShaft();
    JumpOntoRightWall();
    EXPECT_EQ(Grapple_PlatformerPlayerWall(level_), 1);
    EXPECT_STREQ(Grapple_PlatformerPlayerStateName(level_), "wall_slide");
    Frames(10, 1.0f, false);
    EXPECT_LE(Vy(), Grapple_PlatformerPlayerTuning(level_, "wall_slide_speed") + 0.01f);
    EXPECT_GT(Vy(), 0.0f);
    // Letting go of the wall drops off it.
    Frames(2, 0.0f, false);
    EXPECT_EQ(Grapple_PlatformerPlayerWall(level_), 0);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_FALL);
}

TEST_F(WallHarness, WallJumpKicksAwayAndUpAndIgnoresTheStickBriefly)
{
    MakeShaft();
    JumpOntoRightWall();
    const float x_on_wall = X();
    Frames(1, 1.0f, true); // a fresh press while still holding into the wall
    EXPECT_TRUE(Grapple_PlatformerPlayerJumped(level_));
    EXPECT_LT(Vx(), 0.0f);
    EXPECT_LT(Vy(), 0.0f);
    EXPECT_EQ(Grapple_PlatformerPlayerFacing(level_), -1);
    EXPECT_EQ(Grapple_PlatformerPlayerState(level_), GRAPPLE_PLAYER_JUMP);
    // Still holding right through the lock: the kick carries anyway.
    Frames(5, 1.0f, true);
    EXPECT_LT(X(), x_on_wall - 8.0f);
}

TEST_F(WallHarness, RepeatedWallJumpsClimbTheShaft)
{
    MakeShaft();
    const float start_y = Y();
    Frames(1, 1.0f, true);
    // hold counts frames the button stays down after a press, then goes
    // negative for frames it has been up: a kick needs a fresh press, so
    // the button must have been released at least once in between.
    int hold = 12;
    for (int i = 0; i < 200; ++i)
    {
        // Steer into the nearer wall; when touching it and the button has
        // been up, press jump and hold it so the kick gets its full height.
        const float toward = (X() < 6.5f * kTile) ? -1.0f : 1.0f;
        if (Grapple_PlatformerPlayerWall(level_) != 0 && hold < 0)
        {
            hold = 12;
        }
        Frames(1, toward, hold > 0);
        hold--;
    }
    EXPECT_LT(Y(), start_y - 8.0f * kTile);
    EXPECT_FALSE(Grapple_PlatformerPlayerGrounded(level_));
}

TEST_F(PlatformerHarness, TheEdgeOfTheLevelIsNotAWallToJumpFrom)
{
    MakeLevel();
    Settle();
    // Run into the left edge, jump, and hold into it: no slide, no kick.
    Frames(120, -1.0f, false, true);
    Frames(1, -1.0f, true);
    bool slid = false;
    for (int i = 0; i < 25; ++i)
    {
        Frames(1, -1.0f, i < 10);
        slid = slid || Grapple_PlatformerPlayerWall(level_) != 0;
    }
    EXPECT_FALSE(slid);
    ASSERT_FALSE(Grapple_PlatformerPlayerGrounded(level_));
    Frames(1, -1.0f, true); // a fresh press in the air, against the edge
    EXPECT_FALSE(Grapple_PlatformerPlayerJumped(level_));
}

TEST_F(WallHarness, ZeroWallJumpHeightTurnsItOff)
{
    MakeShaft();
    ASSERT_TRUE(Grapple_PlatformerSetPlayerTuning(level_, "wall_jump_y", 0.0f));
    JumpOntoRightWall();
    Frames(1, 1.0f, true);
    EXPECT_FALSE(Grapple_PlatformerPlayerJumped(level_));
    EXPECT_GE(Vx(), 0.0f);
}

// --- camera ------------------------------------------------------------------

TEST_F(PlatformerHarness, CameraHoldsGroundLevelThroughAJumpAndFollowsALedge)
{
    // A big level with the floor mid-way, so the view can move every way.
    level_ = Grapple_CreatePlatformer(engine_, 80, 40, kTile);
    ASSERT_NE(level_, nullptr);
    ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 20, 80, 0));
    ASSERT_TRUE(Grapple_PlatformerCreatePlatform(level_, 40, 12, 4));
    ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 40, 19), GRAPPLE_ACTOR_NONE);
    Grapple_PlatformerSetScriptedInput(level_, true);
    ASSERT_TRUE(Grapple_PlatformerAttach(level_));
    Frames(30);
    float cam_y0 = 0.0f;
    Grapple_PlatformerCameraPosition(level_, nullptr, &cam_y0);

    // A full standing jump, four tiles up and back: the view holds still.
    Frames(1, 0.0f, true);
    for (int i = 0; i < 60; ++i)
    {
        Frames(1, 0.0f, true);
        float cam_y = 0.0f;
        Grapple_PlatformerCameraPosition(level_, nullptr, &cam_y);
        EXPECT_NEAR(cam_y, cam_y0, 0.01f) << "frame " << i;
    }

    // The view leads the way the player faces: ahead to the right while
    // walking right, ahead to the left once walking left.
    Frames(90, 1.0f);
    float cam_x = 0.0f;
    Grapple_PlatformerCameraPosition(level_, &cam_x, nullptr);
    EXPECT_GT(cam_x - X(), 8.0f);
    Frames(180, -1.0f);
    Grapple_PlatformerCameraPosition(level_, &cam_x, nullptr);
    EXPECT_LT(cam_x - X(), -8.0f);

    // Standing on a ledge eight tiles up is a new ground level: the view climbs.
    Grapple_PlatformerPlayerRespawn(level_, 41.5f * kTile, 12.0f * kTile);
    Frames(60);
    ASSERT_TRUE(Grapple_PlatformerPlayerGrounded(level_));
    float cam_y = 0.0f;
    Grapple_PlatformerCameraPosition(level_, nullptr, &cam_y);
    EXPECT_LT(cam_y, cam_y0 - 100.0f);
}

// --- tuning and input --------------------------------------------------------

TEST_F(PlatformerHarness, TuningKeysRoundTripAndUnknownKeysAreNamed)
{
    MakeLevel();
    EXPECT_TRUE(Grapple_PlatformerSetPlayerTuning(level_, "walk_speed", 123.0f));
    EXPECT_FLOAT_EQ(Grapple_PlatformerPlayerTuning(level_, "walk_speed"), 123.0f);
    EXPECT_FALSE(Grapple_PlatformerSetPlayerTuning(level_, "wlak_speed", 1.0f));
    EXPECT_NE(std::string(SDL_GetError()).find("wlak_speed"), std::string::npos);

    // jump_height is jump_speed in other units: 4 tiles by default.
    EXPECT_NEAR(Grapple_PlatformerPlayerTuning(level_, "jump_height"), 4.0f * kTile, 0.01f);
    EXPECT_TRUE(Grapple_PlatformerSetPlayerTuning(level_, "jump_height", 5.0f * kTile));
    EXPECT_NEAR(Grapple_PlatformerPlayerTuning(level_, "jump_height"), 5.0f * kTile, 0.01f);

    // Every key can be enumerated for a tuning screen.
    bool saw_gravity = false;
    for (int i = 0; i < Grapple_PlatformerPlayerTuningCount(); ++i)
    {
        const char *key = Grapple_PlatformerPlayerTuningKey(i);
        ASSERT_NE(key, nullptr);
        saw_gravity = saw_gravity || std::string(key) == "fall_gravity";
    }
    EXPECT_TRUE(saw_gravity);
    EXPECT_EQ(Grapple_PlatformerPlayerTuningKey(999), nullptr);
}

TEST_F(PlatformerHarness, DefaultActionsCoverKeyboardAndPadAndCanBeRebound)
{
    MakeLevel();
    Grapple_ActionMap *map = Grapple_PlatformerActions(level_);
    ASSERT_NE(map, nullptr);
    EXPECT_GE(Grapple_ActionBindingCount(map, "move_x"), 6);
    EXPECT_GE(Grapple_ActionBindingCount(map, "jump"), 3);
    EXPECT_GE(Grapple_ActionBindingCount(map, "run"), 3);
    const int before = Grapple_ActionBindingCount(map, "jump");
    EXPECT_TRUE(Grapple_PlatformerBind(level_, "jump", "pad:b")) << SDL_GetError();
    EXPECT_EQ(Grapple_ActionBindingCount(map, "jump"), before + 1);
    EXPECT_FALSE(Grapple_PlatformerBind(level_, "jump", "pad:teapot"));
    // Nothing is held on a headless engine.
    EXPECT_FALSE(Grapple_PlatformerActionDown(level_, "jump"));
    EXPECT_FLOAT_EQ(Grapple_PlatformerActionValue(level_, "move_x"), 0.0f);
}

// --- the scene -----------------------------------------------------------------

TEST_F(PlatformerHarness, AttachingCountsAsAGameForTheScriptRunner)
{
    level_ = Grapple_CreatePlatformer(engine_, 8, 8, kTile);
    ASSERT_NE(level_, nullptr);
    EXPECT_FALSE(Grapple_ScriptHasHandlers(engine_));
    ASSERT_TRUE(Grapple_PlatformerAttach(level_));
    EXPECT_TRUE(Grapple_PlatformerAttached(level_));
    EXPECT_TRUE(Grapple_ScenePending(engine_));
    EXPECT_TRUE(Grapple_ScriptHasHandlers(engine_));
    Frames(1);
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);
    EXPECT_FALSE(Grapple_ScenePending(engine_));
    Grapple_PlatformerDetach(level_);
    EXPECT_FALSE(Grapple_PlatformerAttached(level_));
    Frames(1);
    EXPECT_EQ(Grapple_SceneDepth(engine_), 0);
}

TEST_F(PlatformerHarness, DestroyingAnAttachedLevelLeavesTheSceneInert)
{
    MakeLevel();
    Frames(5);
    Grapple_DestroyPlatformer(level_);
    level_ = nullptr;
    Frames(5); // the scene is still on the stack and must not touch freed memory
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);
}

TEST_F(PlatformerHarness, CameraFollowsAndForwardScrollNeverGoesBack)
{
    level_ = Grapple_CreatePlatformer(engine_, 60, 15, kTile);
    ASSERT_NE(level_, nullptr);
    ASSERT_TRUE(Grapple_PlatformerCreateFloor(level_, 0, 13, 60, 0));
    ASSERT_NE(Grapple_PlatformerCreatePlayer(level_, 2, 12), GRAPPLE_ACTOR_NONE);
    Grapple_PlatformerSetScriptedInput(level_, true);
    Grapple_PlatformerSetScroll(level_, GRAPPLE_PLATFORMER_SCROLL_FORWARD);
    ASSERT_TRUE(Grapple_PlatformerAttach(level_));
    Frames(20);
    float start_x = 0.0f;
    Grapple_PlatformerCameraPosition(level_, &start_x, nullptr);
    EXPECT_FLOAT_EQ(start_x, 128.0f); // clamped to the left edge of a 256-wide view
    Frames(240, 1.0f, false, true);
    float far_x = 0.0f;
    Grapple_PlatformerCameraPosition(level_, &far_x, nullptr);
    EXPECT_GT(far_x, start_x + 100.0f);
    Frames(240, -1.0f, false, true);
    float back_x = 0.0f;
    Grapple_PlatformerCameraPosition(level_, &back_x, nullptr);
    EXPECT_GE(back_x, far_x - 0.01f);
    // Left of the scrolled edge is a wall to the player too.
    EXPECT_GE(X(), back_x - 128.0f);
}

// --- Tiled -------------------------------------------------------------------

TEST_F(PlatformerHarness, LoadsCollisionAndObjectsFromATiledMap)
{
    const std::string path = std::string(PLATFORMER_TEST_ASSETS_DIR) + "/level.tmj";
    level_ = Grapple_LoadPlatformer(engine_, path.c_str());
    ASSERT_NE(level_, nullptr) << SDL_GetError();
    EXPECT_EQ(Grapple_PlatformerWidth(level_), 12);
    EXPECT_EQ(Grapple_PlatformerHeight(level_), 6);
    EXPECT_EQ(Grapple_PlatformerTileSize(level_), 16);

    // The "ground" layer is collision; "decoration" is not.
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 0, 4), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 11, 5), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 0, 0), GRAPPLE_PLATFORMER_EMPTY);
    // Tile properties: gid 2 is one_way, gid 3 is solid = false.
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 4, 2), GRAPPLE_PLATFORMER_ONE_WAY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 6, 2), GRAPPLE_PLATFORMER_ONE_WAY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 8, 3), GRAPPLE_PLATFORMER_EMPTY);
    // Objects by class.
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 10, 2), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 10, 3), GRAPPLE_PLATFORMER_SOLID);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 10, 1), GRAPPLE_PLATFORMER_EMPTY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 0, 2), GRAPPLE_PLATFORMER_ONE_WAY);
    EXPECT_EQ(Grapple_PlatformerTileAt(level_, 1, 2), GRAPPLE_PLATFORMER_ONE_WAY);
    // The player object: its bottom edge is the feet.
    ASSERT_TRUE(Grapple_PlatformerHasPlayer(level_));
    EXPECT_FLOAT_EQ(X(), 24.0f);
    EXPECT_FLOAT_EQ(Y(), 64.0f);
}

TEST_F(PlatformerHarness, MissingMapFailsWithAnError)
{
    EXPECT_EQ(Grapple_LoadPlatformer(engine_, "nowhere/level.tmj"), nullptr);
    EXPECT_STRNE(SDL_GetError(), "");
}

#ifdef PLATFORMER_TEST_CPP
TEST_F(PlatformerHarness, CppWrapperOwnsAndDrivesALevel)
{
    auto level = grapple::Platformer::Create(engine_, 20, 10, kTile);
    ASSERT_TRUE(level.ok()) << level.status().message();
    EXPECT_TRUE(level->CreateFloor(0, 8, 20));
    auto player = level->CreatePlayer(2, 7);
    ASSERT_TRUE(player.ok());
    player->set_scripted_input(true);
    ASSERT_TRUE(level->Attach().ok());
    player->Input(1.0f, false, false);
    for (int i = 0; i < 60; ++i)
    {
        Grapple_EngineAdvance(engine_, kFrameNs);
        Grapple_EngineTick(engine_);
    }
    EXPECT_TRUE(player->grounded());
    EXPECT_GT(player->position().first, 2.5f * kTile);
    EXPECT_EQ(std::string(player->state_name()), "walk");
    EXPECT_TRUE(player->Set("walk_speed", 50.0f).ok());
    EXPECT_FALSE(player->Set("nope", 1.0f).ok());
    EXPECT_TRUE(level->Bind("jump", "pad:y").ok());
    grapple::Platformer moved = std::move(*level);
    EXPECT_TRUE(static_cast<bool>(moved));
    EXPECT_FALSE(static_cast<bool>(*level));
}
#endif

} // namespace
