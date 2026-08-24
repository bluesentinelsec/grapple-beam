// Physics bodies on actors.
//
// These simulate rather than check API shape: things fall, land, stop,
// bounce off each other and report it. The cases worth being careful about
// are the ones where two systems have to agree — the solver and the actor
// transform, and the solver and the fixed tick.
#include <grapple/engine_physics.h>
#include <grapple/engine_render.h>

#include <gtest/gtest.h>

#include <vector>

namespace
{

class PhysicsHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        Grapple_EngineConfig config{};
        config.headless = true;
        config.manual_clock = true;
        config.no_auto_mount = true;
        config.design_width = 960;
        config.design_height = 540;
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
    }
    void TearDown() override
    {
        Grapple_DestroyEngine(engine_);
        SDL_Quit();
    }

    Grapple_ActorId Spawn(float x, float y, const Grapple_BodyDef &def)
    {
        Grapple_ActorDef actor_def{};
        actor_def.type = "body";
        actor_def.x = x;
        actor_def.y = y;
        const Grapple_ActorId id = Grapple_ActorSpawn(engine_, &actor_def);
        Grapple_ActorAddBody(Grapple_ActorGet(engine_, id), &def);
        Frames(1); // admit the actor
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

    float Y(Grapple_ActorId id)
    {
        return Grapple_ActorWorld(Grapple_ActorGet(engine_, id)).y;
    }
    float X(Grapple_ActorId id)
    {
        return Grapple_ActorWorld(Grapple_ActorGet(engine_, id)).x;
    }

    Grapple_Engine *engine_ = nullptr;
};

TEST_F(PhysicsHarness, ADefaultBodyIsUsable)
{
    const Grapple_BodyDef def = Grapple_BodyDefault();
    EXPECT_EQ(def.type, GRAPPLE_BODY_DYNAMIC);
    EXPECT_GT(def.width, 0.0f) << "a zeroed def would fall through the world";
    EXPECT_GT(def.height, 0.0f);
    EXPECT_GT(def.density, 0.0f);
    EXPECT_FLOAT_EQ(def.gravity_scale, 1.0f);
}

TEST_F(PhysicsHarness, ThingsFall)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    const Grapple_ActorId id = Spawn(100.0f, 0.0f, def);
    ASSERT_TRUE(Grapple_ActorHasBody(Grapple_ActorGet(engine_, id)));

    const float start = Y(id);
    Frames(30); // half a second
    EXPECT_GT(Y(id), start) << "positive Y is down, as on screen";

    // Roughly 1g at the default scale: half a second of freefall is about
    // 122 design units. Generously bounded — this is checking the units are
    // not out by a factor of sixty, not the solver's arithmetic.
    EXPECT_GT(Y(id) - start, 60.0f);
    EXPECT_LT(Y(id) - start, 250.0f);
}

TEST_F(PhysicsHarness, GravityIsConfigurable)
{
    Grapple_PhysicsSetGravity(engine_, 0.0f, 0.0f);
    float gx = 1.0f;
    float gy = 1.0f;
    Grapple_PhysicsGravity(engine_, &gx, &gy);
    EXPECT_FLOAT_EQ(gy, 0.0f);

    const Grapple_BodyDef def = Grapple_BodyDefault();
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);
    Frames(30);
    EXPECT_NEAR(Y(id), 100.0f, 1.0f) << "no gravity, no falling";
}

TEST_F(PhysicsHarness, GravityScaleIsPerBody)
{
    Grapple_BodyDef floater = Grapple_BodyDefault();
    floater.gravity_scale = 0.0f;
    const Grapple_ActorId floating = Spawn(100.0f, 100.0f, floater);

    Grapple_BodyDef heavy = Grapple_BodyDefault();
    heavy.gravity_scale = 2.0f;
    const Grapple_ActorId falling = Spawn(300.0f, 100.0f, heavy);

    Frames(30);
    EXPECT_NEAR(Y(floating), 100.0f, 1.0f);
    EXPECT_GT(Y(falling), 200.0f);
}

// The thing physics is for: something falls and something else stops it.
TEST_F(PhysicsHarness, ThingsLandOnStaticGround)
{
    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 800.0f;
    ground_def.height = 40.0f;
    Spawn(400.0f, 400.0f, ground_def);

    Grapple_BodyDef box_def = Grapple_BodyDefault();
    box_def.width = 40.0f;
    box_def.height = 40.0f;
    const Grapple_ActorId box = Spawn(400.0f, 100.0f, box_def);

    Frames(120); // two seconds: plenty of time to fall and settle
    const float resting = Y(box);

    // Sitting on top of the ground: ground centre 400, half-height 20, box
    // half-height 20, so it rests around y = 360.
    EXPECT_NEAR(resting, 360.0f, 6.0f) << "landed on the ground, not through it";

    Frames(60);
    EXPECT_NEAR(Y(box), resting, 1.0f) << "and stayed there";
}

TEST_F(PhysicsHarness, StaticBodiesDoNotMove)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    def.type = GRAPPLE_BODY_STATIC;
    const Grapple_ActorId id = Spawn(200.0f, 200.0f, def);
    Frames(60);
    EXPECT_FLOAT_EQ(Y(id), 200.0f);
}

TEST_F(PhysicsHarness, VelocityAndImpulsesMoveThings)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    def.gravity_scale = 0.0f;
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);

    Grapple_ActorSetVelocity(actor, 120.0f, 0.0f);
    float vx = 0.0f;
    float vy = 0.0f;
    Grapple_ActorVelocity(actor, &vx, &vy);
    EXPECT_NEAR(vx, 120.0f, 0.1f) << "in design units, not metres";

    Frames(60); // one second at 120 units/s
    EXPECT_NEAR(X(id), 220.0f, 12.0f);

    // An impulse changes momentum immediately.
    Grapple_ActorSetVelocity(actor, 0.0f, 0.0f);
    Grapple_ActorApplyImpulse(actor, 0.0f, -300.0f);
    Grapple_ActorVelocity(actor, &vx, &vy);
    EXPECT_LT(vy, 0.0f) << "upwards";
}

// The two systems that have to agree: the solver moves the body, and the
// actor transform the renderer reads must follow it.
TEST_F(PhysicsHarness, TheActorTransformFollowsTheBody)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    def.gravity_scale = 0.0f;
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);
    Grapple_ActorSetVelocity(Grapple_ActorGet(engine_, id), 60.0f, 60.0f);

    Frames(30);
    const Grapple_ActorTransform t = Grapple_ActorWorld(Grapple_ActorGet(engine_, id));
    EXPECT_GT(t.x, 120.0f) << "one position, and the solver owns it";
    EXPECT_GT(t.y, 120.0f);
}

// A body offset is how a feet-anchored sprite and a centred body agree.
TEST_F(PhysicsHarness, TheBodyOffsetKeepsFeetOnTheFloor)
{
    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 800.0f;
    ground_def.height = 40.0f;
    Spawn(400.0f, 400.0f, ground_def);

    // The actor's position is at its feet, so the body sits half its height
    // above it.
    Grapple_BodyDef body_def = Grapple_BodyDefault();
    body_def.width = 40.0f;
    body_def.height = 80.0f;
    body_def.offset_y = -40.0f;
    body_def.fixed_rotation = true;
    const Grapple_ActorId walker = Spawn(400.0f, 100.0f, body_def);

    Frames(150);
    // Feet land on the top of the ground, at y = 380.
    EXPECT_NEAR(Y(walker), 380.0f, 6.0f)
        << "the actor's position is where its feet are, and they are on the floor";
}

TEST_F(PhysicsHarness, FixedRotationKeepsACharacterUpright)
{
    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 800.0f;
    ground_def.height = 40.0f;
    Spawn(400.0f, 400.0f, ground_def);

    Grapple_BodyDef def = Grapple_BodyDefault();
    def.shape = GRAPPLE_SHAPE_CAPSULE;
    def.width = 40.0f;
    def.height = 90.0f;
    def.fixed_rotation = true;
    const Grapple_ActorId id = Spawn(400.0f, 100.0f, def);

    // Shove it sideways: without fixed rotation a capsule lies down.
    Grapple_ActorApplyImpulse(Grapple_ActorGet(engine_, id), 400.0f, 0.0f);
    Frames(120);
    const float rotation = Grapple_ActorWorld(Grapple_ActorGet(engine_, id)).rotation;
    EXPECT_NEAR(rotation, 0.0f, 0.001f) << "still standing up";
}

// --- collisions -----------------------------------------------------------

struct Collision
{
    Grapple_ActorId a, b;
    bool began;
};

TEST_F(PhysicsHarness, CollisionsAreReportedAfterTheStep)
{
    static std::vector<Collision> log;
    log.clear();
    Grapple_PhysicsSetCollisionCallback(
        engine_,
        [](Grapple_Engine *, Grapple_ActorId a, Grapple_ActorId b, bool began, void *) {
            log.push_back({a, b, began});
        },
        nullptr);

    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 400.0f;
    ground_def.height = 40.0f;
    const Grapple_ActorId ground = Spawn(400.0f, 400.0f, ground_def);

    Grapple_BodyDef box_def = Grapple_BodyDefault();
    const Grapple_ActorId box = Spawn(400.0f, 100.0f, box_def);

    Frames(120);
    ASSERT_FALSE(log.empty()) << "the box landed on the ground and nobody said so";

    bool found = false;
    for (const Collision &event : log)
    {
        if (event.began && ((event.a == box && event.b == ground) ||
                            (event.a == ground && event.b == box)))
        {
            found = true;
        }
    }
    EXPECT_TRUE(found) << "and the actors are named, not the shapes";
}

// The first thing every collision handler wants to do.
TEST_F(PhysicsHarness, DestroyingAnActorFromACollisionHandlerIsSafe)
{
    static Grapple_Engine *engine_ptr;
    static Grapple_ActorId doomed;
    static int destroyed_calls;
    engine_ptr = engine_;
    destroyed_calls = 0;

    Grapple_PhysicsSetCollisionCallback(
        engine_,
        [](Grapple_Engine *engine, Grapple_ActorId a, Grapple_ActorId b, bool began,
           void *) {
            if (!began)
            {
                return;
            }
            if (a == doomed || b == doomed)
            {
                Grapple_ActorDestroy(engine, doomed);
                destroyed_calls++;
            }
        },
        nullptr);

    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 400.0f;
    ground_def.height = 40.0f;
    Spawn(400.0f, 400.0f, ground_def);

    Grapple_BodyDef box_def = Grapple_BodyDefault();
    doomed = Spawn(400.0f, 100.0f, box_def);

    Frames(150);
    EXPECT_GT(destroyed_calls, 0);
    EXPECT_FALSE(Grapple_ActorAlive(engine_ptr, doomed)) << "and it is actually gone";
}

TEST_F(PhysicsHarness, SensorsDetectWithoutColliding)
{
    static int touches;
    touches = 0;
    Grapple_PhysicsSetCollisionCallback(
        engine_,
        [](Grapple_Engine *, Grapple_ActorId, Grapple_ActorId, bool began, void *) {
            if (began)
            {
                touches++;
            }
        },
        nullptr);

    Grapple_BodyDef trigger_def = Grapple_BodyDefault();
    trigger_def.type = GRAPPLE_BODY_STATIC;
    trigger_def.sensor = true;
    trigger_def.width = 400.0f;
    trigger_def.height = 40.0f;
    Spawn(400.0f, 300.0f, trigger_def);

    Grapple_BodyDef box_def = Grapple_BodyDefault();
    const Grapple_ActorId box = Spawn(400.0f, 100.0f, box_def);

    Frames(90);
    EXPECT_GT(Y(box), 400.0f) << "fell straight through the sensor";
    EXPECT_GT(touches, 0) << "but was still detected: a trigger nobody can "
                             "detect is not a trigger";
}

// --- collision filtering --------------------------------------------------

TEST_F(PhysicsHarness, CategoriesAndMasksDecideWhatHitsWhat)
{
    constexpr Uint32 kGround = 1u << 0;
    constexpr Uint32 kGhost = 1u << 1;

    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 800.0f;
    ground_def.height = 40.0f;
    ground_def.category = kGround;
    ground_def.collides_with = kGround; // not ghosts
    Spawn(400.0f, 400.0f, ground_def);

    Grapple_BodyDef ghost_def = Grapple_BodyDefault();
    ghost_def.category = kGhost;
    ghost_def.collides_with = kGround;
    const Grapple_ActorId ghost = Spawn(400.0f, 100.0f, ghost_def);

    Frames(120);
    EXPECT_GT(Y(ghost), 450.0f) << "the ground does not collide with ghosts";
}

// --- queries --------------------------------------------------------------

TEST_F(PhysicsHarness, RaycastsFindTheNearestThing)
{
    Grapple_BodyDef ground_def = Grapple_BodyDefault();
    ground_def.type = GRAPPLE_BODY_STATIC;
    ground_def.width = 800.0f;
    ground_def.height = 40.0f;
    const Grapple_ActorId ground = Spawn(400.0f, 400.0f, ground_def);
    Frames(2);

    // Straight down from above the ground: the classic ground check.
    const Grapple_RayHit hit =
        Grapple_PhysicsRaycast(engine_, 400.0f, 100.0f, 0.0f, 400.0f, 0);
    ASSERT_TRUE(hit.hit);
    EXPECT_EQ(hit.actor, ground);
    EXPECT_NEAR(hit.y, 380.0f, 4.0f) << "the top surface, in design units";
    EXPECT_LT(hit.normal_y, 0.0f) << "pointing back up at the ray";

    // And a ray into nothing finds nothing.
    const Grapple_RayHit miss =
        Grapple_PhysicsRaycast(engine_, -5000.0f, -5000.0f, 0.0f, 10.0f, 0);
    EXPECT_FALSE(miss.hit);
    EXPECT_EQ(miss.actor, GRAPPLE_ACTOR_NONE);
}

TEST_F(PhysicsHarness, OverlapFindsEverythingInARectangle)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    def.gravity_scale = 0.0f;
    def.width = 40.0f;
    def.height = 40.0f;
    const Grapple_ActorId a = Spawn(100.0f, 100.0f, def);
    const Grapple_ActorId b = Spawn(160.0f, 100.0f, def);
    Spawn(900.0f, 900.0f, def);
    Frames(2);

    Grapple_ActorId found[8];
    const int count =
        Grapple_PhysicsOverlap(engine_, SDL_FRect{60.0f, 60.0f, 160.0f, 80.0f}, 0, found, 8);
    EXPECT_EQ(count, 2);

    bool has_a = false;
    bool has_b = false;
    for (int i = 0; i < count; ++i)
    {
        has_a = has_a || found[i] == a;
        has_b = has_b || found[i] == b;
    }
    EXPECT_TRUE(has_a);
    EXPECT_TRUE(has_b);
}

// --- lifecycle ------------------------------------------------------------

TEST_F(PhysicsHarness, PausingStopsTheWorldWithoutLosingIt)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);

    Grapple_PhysicsSetPaused(engine_, true);
    EXPECT_TRUE(Grapple_PhysicsPaused(engine_));
    Frames(60);
    EXPECT_NEAR(Y(id), 100.0f, 1.0f) << "nothing moved";

    Grapple_PhysicsSetPaused(engine_, false);
    Frames(60);
    EXPECT_GT(Y(id), 150.0f) << "and carried on where it left off";
}

TEST_F(PhysicsHarness, BodiesGoAwayWithTheirActors)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);
    EXPECT_EQ(Grapple_PhysicsBodyCount(engine_), 1);

    Grapple_ActorRemoveBody(Grapple_ActorGet(engine_, id));
    EXPECT_EQ(Grapple_PhysicsBodyCount(engine_), 0);
    EXPECT_FALSE(Grapple_ActorHasBody(Grapple_ActorGet(engine_, id)));

    // The actor is still there, just no longer simulated.
    Frames(30);
    EXPECT_TRUE(Grapple_ActorAlive(engine_, id));
    EXPECT_NEAR(Y(id), 100.0f, 1.0f);
}

TEST_F(PhysicsHarness, TeleportingMovesBodyAndActorTogether)
{
    Grapple_BodyDef def = Grapple_BodyDefault();
    def.gravity_scale = 0.0f;
    const Grapple_ActorId id = Spawn(100.0f, 100.0f, def);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);

    Grapple_ActorSetVelocity(actor, 200.0f, 0.0f);
    Frames(10);
    Grapple_ActorTeleportBody(actor, 700.0f, 300.0f);

    EXPECT_NEAR(X(id), 700.0f, 0.1f);
    // Not interpolated, or it smears across the screen from where it was.
    EXPECT_NEAR(Grapple_ActorRenderTransform(actor, 0.0f).x, 700.0f, 0.1f);

    Frames(5);
    EXPECT_NEAR(X(id), 700.0f, 20.0f) << "and the velocity was cleared with it";
}

TEST_F(PhysicsHarness, PixelsPerMetreIsConfigurable)
{
    EXPECT_FLOAT_EQ(Grapple_PhysicsPixelsPerMetre(engine_), 64.0f);
    Grapple_PhysicsSetPixelsPerMetre(engine_, 100.0f);
    EXPECT_FLOAT_EQ(Grapple_PhysicsPixelsPerMetre(engine_), 100.0f);
    Grapple_PhysicsSetPixelsPerMetre(engine_, 0.0f);
    EXPECT_FLOAT_EQ(Grapple_PhysicsPixelsPerMetre(engine_), 100.0f) << "zero is refused";
}

TEST_F(PhysicsHarness, NullsAreHandled)
{
    EXPECT_FALSE(Grapple_ActorAddBody(nullptr, nullptr));
    EXPECT_FALSE(Grapple_ActorHasBody(nullptr));
    Grapple_ActorRemoveBody(nullptr);
    Grapple_ActorSetVelocity(nullptr, 1.0f, 1.0f);
    Grapple_ActorApplyImpulse(nullptr, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(Grapple_ActorAngularVelocity(nullptr), 0.0f);
    EXPECT_EQ(Grapple_PhysicsBodyCount(nullptr), 0);
    EXPECT_FALSE(Grapple_PhysicsRaycast(nullptr, 0, 0, 1, 1, 0).hit);
    EXPECT_EQ(Grapple_PhysicsOverlap(nullptr, SDL_FRect{0, 0, 1, 1}, 0, nullptr, 0), 0);

    // A body on an actor with no physics world yet is still fine.
    float vx = 9.0f;
    Grapple_ActorVelocity(nullptr, &vx, nullptr);
    EXPECT_FLOAT_EQ(vx, 0.0f);
}

} // namespace
