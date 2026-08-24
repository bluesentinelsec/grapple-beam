// Engine-owned rendering: draw order, culling, origins, interpolation.
//
// These read the framebuffer rather than trusting the draw list, because
// the failure that matters is what ends up on screen. The headless engine
// is a real software renderer, so "which colour is at this pixel" is an
// honest answer to "which sprite won".
#include <grapple/engine_render.h>

#include <gtest/gtest.h>

#include <vector>

namespace
{

// A solid-colour sprite: no texture, so the engine fills a rectangle. That
// is a real feature (it is how a game draws before it has art) and it makes
// these tests independent of image loading.
Grapple_Sprite Solid(float r, float g, float b, float size, int layer = 0)
{
    Grapple_Sprite sprite = Grapple_SpriteDefault();
    sprite.color = SDL_FColor{r, g, b, 1.0f};
    sprite.width = size;
    sprite.height = size;
    sprite.layer = layer;
    return sprite;
}

class RenderHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        Grapple_EngineConfig config{};
        config.headless = true;
        config.manual_clock = true;
        config.no_auto_mount = true;
        config.design_width = 320;
        config.design_height = 240;
        config.window_width = 320;
        config.window_height = 240;
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
        Grapple_CameraInit(&camera_, engine_);
        Grapple_CameraSnap(&camera_, 160.0f, 120.0f);
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
        def.type = "sprite";
        def.x = x;
        def.y = y;
        const Grapple_ActorId id = Grapple_ActorSpawn(engine_, &def);
        // One frame so the actor is admitted; queries and rendering both
        // deliberately ignore actors that are still pending.
        Grapple_EngineAdvance(engine_, 16666667ull);
        Grapple_EngineTick(engine_);
        return id;
    }

    void Draw(const Grapple_Camera *camera = nullptr, float alpha = 1.0f)
    {
        SDL_Renderer *r = Grapple_EngineRenderer(engine_);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColorFloat(r, 0.0f, 0.0f, 0.0f, 1.0f);
        SDL_RenderClear(r);
        Grapple_RenderWorld(engine_, (camera != nullptr) ? camera : &camera_, alpha);
    }

    // The colour at a design-space point, as an (r,g,b) triple 0-255.
    std::vector<int> PixelAt(int x, int y)
    {
        SDL_Renderer *r = Grapple_EngineRenderer(engine_);
        int w = 0;
        int h = 0;
        Grapple_EnginePixelSize(engine_, &w, &h);
        SDL_Rect read{0, 0, w, h};
        SDL_Surface *surface = SDL_RenderReadPixels(r, &read);
        if (surface == nullptr)
        {
            return {-1, -1, -1};
        }
        Uint8 red = 0;
        Uint8 green = 0;
        Uint8 blue = 0;
        Uint8 alpha = 0;
        const SDL_PixelFormatDetails *details = SDL_GetPixelFormatDetails(surface->format);
        const Uint32 *pixels = static_cast<const Uint32 *>(surface->pixels);
        const int pitch = surface->pitch / 4;
        SDL_GetRGBA(pixels[y * pitch + x], details, SDL_GetSurfacePalette(surface), &red, &green,
                    &blue, &alpha);
        SDL_DestroySurface(surface);
        return {red, green, blue};
    }

    Grapple_Engine *engine_ = nullptr;
    Grapple_Camera camera_{};
};

TEST_F(RenderHarness, ADefaultSpriteIsVisibleAndWhite)
{
    const Grapple_Sprite sprite = Grapple_SpriteDefault();
    EXPECT_TRUE(sprite.visible) << "a zeroed sprite would be invisible";
    EXPECT_FLOAT_EQ(sprite.color.a, 1.0f) << "and fully transparent";
    EXPECT_FLOAT_EQ(sprite.origin_x, 0.5f) << "and pinned by its corner";
    EXPECT_FLOAT_EQ(sprite.origin_y, 0.5f);
}

TEST_F(RenderHarness, ASpriteIsDrawnWhereTheActorIs)
{
    const Grapple_ActorId id = Spawn(160.0f, 120.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 0.0f, 0.0f, 40.0f);
    ASSERT_TRUE(Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite));

    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 255) << "red, at the actor";
    EXPECT_EQ(PixelAt(10, 10)[0], 0) << "and not everywhere else";
}

// The default origin is the middle, so the sprite straddles the position.
TEST_F(RenderHarness, TheOriginDecidesWhereThePositionSits)
{
    const Grapple_ActorId id = Spawn(160.0f, 120.0f);
    Grapple_Sprite sprite = Solid(0.0f, 1.0f, 0.0f, 40.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);

    Draw();
    EXPECT_EQ(PixelAt(160, 105)[1], 255) << "centred: 20 units above is covered";
    EXPECT_EQ(PixelAt(160, 135)[1], 255) << "and 20 below";

    // Feet-anchored: the position is the bottom edge, which is what you
    // want for anything standing on ground.
    sprite.origin_y = 1.0f;
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);
    Draw();
    EXPECT_EQ(PixelAt(160, 105)[1], 255) << "now entirely above the position";
    EXPECT_EQ(PixelAt(160, 135)[1], 0);
}

// --- ordering -------------------------------------------------------------

TEST_F(RenderHarness, HigherLayersAreDrawnInFront)
{
    const Grapple_ActorId back = Spawn(160.0f, 120.0f);
    const Grapple_ActorId front = Spawn(160.0f, 120.0f);

    const Grapple_Sprite red = Solid(1.0f, 0.0f, 0.0f, 60.0f, 10);
    const Grapple_Sprite blue = Solid(0.0f, 0.0f, 1.0f, 60.0f, 20);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, back), &red);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, front), &blue);

    Draw();
    const std::vector<int> pixel = PixelAt(160, 120);
    EXPECT_EQ(pixel[2], 255) << "the higher layer wins";
    EXPECT_EQ(pixel[0], 0);

    // Swapping the layers swaps the result — the order is the layer, not
    // the spawn order.
    Grapple_ActorSprite(Grapple_ActorGet(engine_, back))->layer = 30;
    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 255);
}

TEST_F(RenderHarness, OrderBreaksTiesWithinALayer)
{
    const Grapple_ActorId first = Spawn(160.0f, 120.0f);
    const Grapple_ActorId second = Spawn(160.0f, 120.0f);

    Grapple_Sprite red = Solid(1.0f, 0.0f, 0.0f, 60.0f, 5);
    red.order = 10.0f;
    Grapple_Sprite blue = Solid(0.0f, 0.0f, 1.0f, 60.0f, 5);
    blue.order = 1.0f;
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, first), &red);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, second), &blue);

    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 255) << "the higher order is in front";
}

// Y-sorting is how a top-down game gets depth: something further down the
// screen is nearer the viewer, so it draws in front.
TEST_F(RenderHarness, YSortingPutsLowerSpritesInFront)
{
    const Grapple_ActorId far_away = Spawn(160.0f, 100.0f);
    const Grapple_ActorId near_by = Spawn(160.0f, 130.0f);

    Grapple_Sprite red = Solid(1.0f, 0.0f, 0.0f, 60.0f, 5);
    red.sort_by_y = true;
    Grapple_Sprite blue = Solid(0.0f, 0.0f, 1.0f, 60.0f, 5);
    blue.sort_by_y = true;
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, far_away), &red);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, near_by), &blue);

    Draw();
    // Where they overlap, the lower (blue, y=130) is in front.
    EXPECT_EQ(PixelAt(160, 115)[2], 255);

    // Move the red one below the blue and it takes over — the sort happens
    // after the actors have moved, which is the point of doing it here.
    Grapple_ActorSetPosition(Grapple_ActorGet(engine_, far_away), 160.0f, 160.0f);
    Draw();
    EXPECT_EQ(PixelAt(160, 140)[0], 255);
}

// --- culling --------------------------------------------------------------

TEST_F(RenderHarness, OffScreenSpritesAreCulled)
{
    Spawn(160.0f, 120.0f);
    const Grapple_ActorId visible = Grapple_ActorFindByType(engine_, "sprite");
    Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 20.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, visible), &sprite);

    // A long way outside the camera's view.
    const Grapple_ActorId offscreen = Spawn(9000.0f, 9000.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, offscreen), &sprite);

    Draw();
    const Grapple_RenderStats stats = Grapple_RenderLastStats(engine_);
    EXPECT_EQ(stats.considered, 2);
    EXPECT_EQ(stats.culled, 1);
    EXPECT_EQ(stats.drawn, 1) << "a level is bigger than the screen; that is the point";
}

// A sprite straddling the edge must still be drawn, or things pop in.
TEST_F(RenderHarness, PartiallyVisibleSpritesAreNotCulled)
{
    const Grapple_ActorId id = Spawn(0.0f, 120.0f); // half off the left edge
    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 0.0f, 40.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);

    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 1);
    EXPECT_EQ(PixelAt(5, 120)[0], 255) << "the visible half is on screen";
}

// A rotated sprite sweeps a bigger area than its rectangle, so culling
// against the unrotated one would clip it as it turned near the edge.
TEST_F(RenderHarness, RotationWidensTheCullBounds)
{
    // Just outside the view by the width of the sprite, but its rotated
    // corner reaches back in.
    const Grapple_ActorId id = Spawn(-24.0f, 120.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 40.0f);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);
    Grapple_ActorSetSprite(actor, &sprite);

    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 0) << "square: fully outside";

    Grapple_ActorTransform t = Grapple_ActorLocal(actor);
    t.rotation = 45.0f;
    Grapple_ActorSetLocal(actor, &t);
    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 1) << "turned: its corner reaches in";
}

TEST_F(RenderHarness, WithoutACameraNothingIsCulled)
{
    const Grapple_ActorId id = Spawn(9000.0f, 9000.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 20.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);

    SDL_Renderer *r = Grapple_EngineRenderer(engine_);
    SDL_RenderClear(r);
    Grapple_RenderWorld(engine_, nullptr, 1.0f);
    EXPECT_EQ(Grapple_RenderLastStats(engine_).culled, 0);
}

// --- visibility -----------------------------------------------------------

TEST_F(RenderHarness, HiddenAndClearedSpritesDrawNothing)
{
    const Grapple_ActorId id = Spawn(160.0f, 120.0f);
    Grapple_Sprite sprite = Solid(1.0f, 0.0f, 0.0f, 40.0f);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);
    Grapple_ActorSetSprite(actor, &sprite);

    Draw();
    ASSERT_EQ(PixelAt(160, 120)[0], 255);

    Grapple_ActorSprite(actor)->visible = false;
    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 0);
    EXPECT_EQ(Grapple_RenderLastStats(engine_).considered, 0) << "not even considered";

    Grapple_ActorSprite(actor)->visible = true;
    Grapple_ActorClearSprite(actor);
    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 0);
    EXPECT_EQ(Grapple_ActorSprite(actor), nullptr);
}

// A destroyed actor's sprite must stop drawing on the same frame the actor
// stops existing, not one frame later.
TEST_F(RenderHarness, ADestroyedActorStopsDrawing)
{
    const Grapple_ActorId id = Spawn(160.0f, 120.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 0.0f, 0.0f, 40.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);
    Draw();
    ASSERT_EQ(PixelAt(160, 120)[0], 255);

    Grapple_ActorDestroy(engine_, id);
    Draw();
    EXPECT_EQ(PixelAt(160, 120)[0], 0) << "doomed actors are already gone from queries";
}

// --- transforms -----------------------------------------------------------

TEST_F(RenderHarness, ChildSpritesFollowTheirParent)
{
    const Grapple_ActorId parent = Spawn(100.0f, 120.0f);

    Grapple_ActorDef def{};
    def.type = "child";
    def.x = 40.0f;
    def.parent = parent;
    const Grapple_ActorId child = Grapple_ActorSpawn(engine_, &def);
    Grapple_EngineAdvance(engine_, 16666667ull);
    Grapple_EngineTick(engine_);

    const Grapple_Sprite sprite = Solid(0.0f, 1.0f, 1.0f, 20.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, child), &sprite);

    Draw();
    EXPECT_EQ(PixelAt(140, 120)[1], 255) << "drawn at parent + local";

    Grapple_ActorMove(Grapple_ActorGet(engine_, parent), 40.0f, 0.0f);
    Draw();
    EXPECT_EQ(PixelAt(180, 120)[1], 255) << "and follows when the parent moves";
    EXPECT_EQ(PixelAt(140, 120)[1], 0);
}

TEST_F(RenderHarness, ActorScaleScalesTheSprite)
{
    const Grapple_ActorId id = Spawn(160.0f, 120.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 0.0f, 1.0f, 20.0f);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);
    Grapple_ActorSetSprite(actor, &sprite);

    Draw();
    EXPECT_EQ(PixelAt(160, 105)[0], 0) << "20 units tall: 15 above is outside";

    Grapple_ActorTransform t = Grapple_ActorLocal(actor);
    t.scale_x = 3.0f;
    t.scale_y = 3.0f;
    Grapple_ActorSetLocal(actor, &t);
    Draw();
    EXPECT_EQ(PixelAt(160, 105)[0], 255) << "60 units tall: now it reaches";
}

// Sprites are drawn at the interpolated transform, so a game gets smooth
// motion without writing its own previous_x.
TEST_F(RenderHarness, SpritesAreDrawnInterpolated)
{
    Grapple_ActorDef def{};
    def.type = "mover";
    def.x = 100.0f;
    def.y = 120.0f;
    def.fixed_update = [](Grapple_Actor *actor, float) {
        Grapple_ActorMove(actor, 60.0f, 0.0f);
    };
    const Grapple_ActorId id = Grapple_ActorSpawn(engine_, &def);
    for (int i = 0; i < 3; ++i)
    {
        Grapple_EngineAdvance(engine_, 16666667ull);
        Grapple_EngineTick(engine_);
    }

    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 10.0f);
    Grapple_Actor *actor = Grapple_ActorGet(engine_, id);
    Grapple_ActorSetSprite(actor, &sprite);

    const float now = Grapple_ActorWorld(actor).x;
    Draw(nullptr, 1.0f);
    EXPECT_EQ(PixelAt(static_cast<int>(now), 120)[0], 255);

    Draw(nullptr, 0.0f);
    EXPECT_EQ(PixelAt(static_cast<int>(now) - 60, 120)[0], 255) << "a whole step behind";
    EXPECT_EQ(PixelAt(static_cast<int>(now), 120)[0], 0);
}

// --- overlay --------------------------------------------------------------

// A HUD belongs to the player, not to a viewport: it is drawn once, in
// design coordinates, whatever the camera is doing.
TEST_F(RenderHarness, ScreenSpaceSpritesIgnoreTheCamera)
{
    const Grapple_ActorId world_actor = Spawn(160.0f, 120.0f);
    const Grapple_ActorId hud = Spawn(40.0f, 40.0f);

    Grapple_Sprite world_sprite = Solid(1.0f, 0.0f, 0.0f, 30.0f);
    Grapple_Sprite hud_sprite = Solid(0.0f, 1.0f, 0.0f, 30.0f);
    hud_sprite.screen_space = true;
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, world_actor), &world_sprite);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, hud), &hud_sprite);

    // The world pass ignores the HUD...
    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 1);
    EXPECT_EQ(PixelAt(40, 40)[1], 0);

    // ...and the overlay pass draws it, unmoved by a camera that has
    // scrolled a long way away.
    Grapple_CameraSnap(&camera_, 5000.0f, 5000.0f);
    Grapple_CameraUpdate(&camera_, engine_, 0.0f);
    Draw();
    EXPECT_EQ(Grapple_RenderOverlay(engine_, 1.0f), 1);
    EXPECT_EQ(PixelAt(40, 40)[1], 255) << "still exactly where it was put";
}

// --- split screen ---------------------------------------------------------

// Each pass culls to its own viewport, which is most of what makes four
// views affordable.
TEST_F(RenderHarness, EachCameraCullsToItsOwnView)
{
    Grapple_Camera cameras[GRAPPLE_SPLIT_MAX];
    for (Grapple_Camera &c : cameras)
    {
        Grapple_CameraInit(&c, engine_);
    }
    ASSERT_EQ(Grapple_CameraSplit(engine_, GRAPPLE_SPLIT_VERTICAL, 2, 0.0f, cameras), 2);

    const Grapple_ActorId left = Spawn(100.0f, 120.0f);
    const Grapple_ActorId right = Spawn(4000.0f, 120.0f);
    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 20.0f);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, left), &sprite);
    Grapple_ActorSetSprite(Grapple_ActorGet(engine_, right), &sprite);

    Grapple_CameraSnap(&cameras[0], 100.0f, 120.0f);
    Grapple_CameraSnap(&cameras[1], 4000.0f, 120.0f);
    Grapple_CameraUpdate(&cameras[0], engine_, 0.0f);
    Grapple_CameraUpdate(&cameras[1], engine_, 0.0f);

    EXPECT_EQ(Grapple_RenderWorld(engine_, &cameras[0], 1.0f), 1);
    EXPECT_EQ(Grapple_RenderLastStats(engine_).culled, 1);
    EXPECT_EQ(Grapple_RenderWorld(engine_, &cameras[1], 1.0f), 1);
    EXPECT_EQ(Grapple_RenderLastStats(engine_).culled, 1);
}

TEST_F(RenderHarness, NullsAreHandled)
{
    EXPECT_FALSE(Grapple_ActorSetSprite(nullptr, nullptr));
    EXPECT_EQ(Grapple_ActorSprite(nullptr), nullptr);
    Grapple_ActorClearSprite(nullptr);
    EXPECT_EQ(Grapple_RenderWorld(nullptr, nullptr, 0.0f), 0);
    EXPECT_EQ(Grapple_RenderOverlay(nullptr, 0.0f), 0);
    EXPECT_EQ(Grapple_RenderLastStats(nullptr).drawn, 0);
}

// The draw list is reused between frames; make sure growing it and
// shrinking back does not lose or duplicate anything.
TEST_F(RenderHarness, TheDrawListSurvivesGrowingAndShrinking)
{
    std::vector<Grapple_ActorId> ids;
    for (int i = 0; i < 200; ++i)
    {
        Grapple_ActorDef def{};
        def.type = "many";
        def.x = 160.0f;
        def.y = 120.0f;
        ids.push_back(Grapple_ActorSpawn(engine_, &def));
    }
    Grapple_EngineAdvance(engine_, 16666667ull);
    Grapple_EngineTick(engine_);

    const Grapple_Sprite sprite = Solid(1.0f, 1.0f, 1.0f, 8.0f);
    for (const Grapple_ActorId id : ids)
    {
        Grapple_ActorSetSprite(Grapple_ActorGet(engine_, id), &sprite);
    }

    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 200);

    for (size_t i = 0; i < ids.size(); i += 2)
    {
        Grapple_ActorDestroy(engine_, ids[i]);
    }
    Grapple_EngineAdvance(engine_, 16666667ull);
    Grapple_EngineTick(engine_);

    Draw();
    EXPECT_EQ(Grapple_RenderLastStats(engine_).drawn, 100);
}

} // namespace
