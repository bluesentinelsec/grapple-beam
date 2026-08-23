// Asset loading: blocking, streamed, cached and reference counted.
//
// Real PNGs are written to a temporary directory and loaded through the
// engine, so the worker threads, the decode and the main-thread upload all
// actually run. The cases worth being careful about are the ones involving
// two threads: an asset released while a worker is still decoding it, and a
// blocking load that must not deadlock against the step that completes it.
#include <grapple/engine_assets.h>

#include <SDL3_image/SDL_image.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{

// A tiny solid-colour PNG on disk, so there is something real to decode.
std::string WritePng(const std::string &name, int size, Uint8 red)
{
    const std::string path = std::string(testing::TempDir()) + name;
    SDL_Surface *surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
    SDL_FillSurfaceRect(surface, nullptr,
                        SDL_MapSurfaceRGBA(surface, red, 0x40, 0x80, 0xFF));
    IMG_SavePNG(surface, path.c_str());
    SDL_DestroySurface(surface);
    return path;
}

class AssetHarness : public ::testing::Test
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
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
    }
    void TearDown() override
    {
        Grapple_DestroyEngine(engine_);
        SDL_Quit();
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
};

TEST_F(AssetHarness, BlockingLoadIsReadyWhenItReturns)
{
    const std::string path = WritePng("grapple_asset_a.png", 32, 0xFF);

    const Grapple_TextureId id = Grapple_LoadTexture(engine_, path.c_str());
    ASSERT_NE(id, GRAPPLE_ASSET_NONE);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_READY)
        << "blocking means blocking: no frames were run";

    SDL_Texture *texture = Grapple_Texture(engine_, id);
    ASSERT_NE(texture, nullptr);
    float w = 0.0f;
    float h = 0.0f;
    SDL_GetTextureSize(texture, &w, &h);
    EXPECT_FLOAT_EQ(w, 32.0f);
    EXPECT_FLOAT_EQ(h, 32.0f);

    SDL_RemovePath(path.c_str());
}

TEST_F(AssetHarness, AsyncLoadReturnsImmediatelyAndLandsLater)
{
    const std::string path = WritePng("grapple_asset_b.png", 16, 0x10);

    const Grapple_TextureId id = Grapple_LoadTextureAsync(engine_, path.c_str());
    ASSERT_NE(id, GRAPPLE_ASSET_NONE);

    // Usable right away: the placeholder stands in until the real thing
    // lands, so a game may draw without checking.
    EXPECT_NE(Grapple_Texture(engine_, id), nullptr);
    EXPECT_EQ(Grapple_AssetsPending(engine_), 1);

    Grapple_AssetsWait(engine_);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_READY);
    EXPECT_EQ(Grapple_AssetsPending(engine_), 0);

    float w = 0.0f;
    SDL_GetTextureSize(Grapple_Texture(engine_, id), &w, nullptr);
    EXPECT_FLOAT_EQ(w, 16.0f) << "the real texture, not the placeholder";

    SDL_RemovePath(path.c_str());
}

// The frame loop is what turns decoded pixels into textures, so an async
// load completes just by running frames.
TEST_F(AssetHarness, TheFrameLoopFinishesAsyncLoads)
{
    const std::string path = WritePng("grapple_asset_c.png", 24, 0x20);
    const Grapple_TextureId id = Grapple_LoadTextureAsync(engine_, path.c_str());

    // Bounded by the wall clock, not by an iteration count. The engine's
    // clock is manual here, so 200 frames run in a few milliseconds — long
    // enough for a worker to have decoded on a fast machine and not on a
    // slower one, which is a race in the test rather than in the engine.
    const Uint64 deadline = SDL_GetTicks() + 5000;
    while (Grapple_AssetStatusOf(engine_, id) != GRAPPLE_ASSET_READY &&
           SDL_GetTicks() < deadline)
    {
        Frames(1);
        SDL_Delay(1); // let the worker actually run
    }
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_READY);
    SDL_RemovePath(path.c_str());
}

// Forty actors of the same type asking for the same sprite is the normal
// case, not an edge case.
TEST_F(AssetHarness, TheSamePathLoadsOnceAndSharesAHandle)
{
    const std::string path = WritePng("grapple_asset_d.png", 8, 0x30);

    const Grapple_TextureId first = Grapple_LoadTexture(engine_, path.c_str());
    const Grapple_TextureId second = Grapple_LoadTexture(engine_, path.c_str());
    EXPECT_EQ(first, second);
    EXPECT_EQ(Grapple_AssetsLoaded(engine_), 1) << "loaded once";
    EXPECT_EQ(Grapple_Texture(engine_, first), Grapple_Texture(engine_, second));

    // Two references, so one release is not enough to free it.
    Grapple_AssetRelease(engine_, first);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, second), GRAPPLE_ASSET_READY);
    Grapple_AssetRelease(engine_, second);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, second), GRAPPLE_ASSET_MISSING);

    SDL_RemovePath(path.c_str());
}

// A sync load of something already loaded asynchronously should be
// instant — one cache, both doors.
TEST_F(AssetHarness, SyncAndAsyncShareOneCache)
{
    const std::string path = WritePng("grapple_asset_e.png", 12, 0x50);

    const Grapple_TextureId async_id = Grapple_LoadTextureAsync(engine_, path.c_str());
    Grapple_AssetsWait(engine_);
    const Grapple_TextureId sync_id = Grapple_LoadTexture(engine_, path.c_str());

    EXPECT_EQ(async_id, sync_id);
    EXPECT_EQ(Grapple_AssetsLoaded(engine_), 1);
    SDL_RemovePath(path.c_str());
}

TEST_F(AssetHarness, RetainKeepsAnAssetPastARelease)
{
    const std::string path = WritePng("grapple_asset_f.png", 8, 0x60);
    const Grapple_TextureId id = Grapple_LoadTexture(engine_, path.c_str());

    Grapple_AssetRetain(engine_, id);
    Grapple_AssetRelease(engine_, id);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_READY) << "still held";
    Grapple_AssetRelease(engine_, id);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_MISSING);

    SDL_RemovePath(path.c_str());
}

// A missing file must cost a wrong-looking sprite, not a crash.
TEST_F(AssetHarness, AMissingFileFailsWithoutTakingTheGameWithIt)
{
    const Grapple_TextureId id =
        Grapple_LoadTexture(engine_, "definitely/not/here/missing.png");
    ASSERT_NE(id, GRAPPLE_ASSET_NONE) << "a handle, even for a file that is not there";
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_FAILED);

    // The placeholder: drawable, and impossible to mistake for art.
    SDL_Texture *texture = Grapple_Texture(engine_, id);
    ASSERT_NE(texture, nullptr);
    EXPECT_TRUE(Grapple_AssetsReady(engine_)) << "a failure still completes the load";
}

TEST_F(AssetHarness, ProgressRunsFromZeroToOne)
{
    std::vector<std::string> paths;
    for (int i = 0; i < 6; ++i)
    {
        paths.push_back(WritePng("grapple_asset_p" + std::to_string(i) + ".png", 8,
                                 static_cast<Uint8>(i * 20)));
    }

    EXPECT_FLOAT_EQ(Grapple_AssetsProgress(engine_), 1.0f) << "nothing asked for yet";

    for (const std::string &path : paths)
    {
        Grapple_LoadTextureAsync(engine_, path.c_str());
    }
    EXPECT_LT(Grapple_AssetsProgress(engine_), 1.0f);
    EXPECT_EQ(Grapple_AssetsPending(engine_), 6);

    Grapple_AssetsWait(engine_);
    EXPECT_FLOAT_EQ(Grapple_AssetsProgress(engine_), 1.0f);
    EXPECT_EQ(Grapple_AssetsLoaded(engine_), 6);

    for (const std::string &path : paths)
    {
        SDL_RemovePath(path.c_str());
    }
}

// The reason the main-thread half is time-sliced at all.
TEST_F(AssetHarness, TheFrameBudgetLimitsUploadsPerFrame)
{
    std::vector<std::string> paths;
    for (int i = 0; i < 8; ++i)
    {
        paths.push_back(WritePng("grapple_asset_q" + std::to_string(i) + ".png", 8,
                                 static_cast<Uint8>(i * 10)));
    }

    // Zero budget: exactly one asset finalised per frame.
    Grapple_AssetsSetFrameBudget(engine_, 0.0f);
    EXPECT_FLOAT_EQ(Grapple_AssetsFrameBudget(engine_), 0.0f);

    for (const std::string &path : paths)
    {
        Grapple_LoadTextureAsync(engine_, path.c_str());
    }

    // Let the workers decode, then check the uploads trickle rather than
    // arriving all at once.
    SDL_Delay(120);
    Frames(1);
    const int after_one = Grapple_AssetsLoaded(engine_);
    EXPECT_LE(after_one, 1) << "a zero budget uploads one per frame";

    Frames(1);
    EXPECT_LE(Grapple_AssetsLoaded(engine_), after_one + 1);

    Grapple_AssetsWait(engine_);
    EXPECT_EQ(Grapple_AssetsLoaded(engine_), 8) << "and they all arrive eventually";

    for (const std::string &path : paths)
    {
        SDL_RemovePath(path.c_str());
    }
}

// The threading case that corrupts memory if it is wrong: the handle goes
// away while a worker is still decoding into its slot.
TEST_F(AssetHarness, ReleasingWhileAWorkerIsStillDecodingIsSafe)
{
    std::vector<std::string> paths;
    std::vector<Grapple_TextureId> ids;
    for (int i = 0; i < 12; ++i)
    {
        paths.push_back(WritePng("grapple_asset_r" + std::to_string(i) + ".png", 64,
                                 static_cast<Uint8>(i * 5)));
    }
    for (const std::string &path : paths)
    {
        ids.push_back(Grapple_LoadTextureAsync(engine_, path.c_str()));
    }

    // Release immediately, while decoding is very likely still in flight.
    for (const Grapple_TextureId id : ids)
    {
        Grapple_AssetRelease(engine_, id);
    }

    // Nothing should be outstanding, and nothing should explode.
    Grapple_AssetsWait(engine_);
    EXPECT_EQ(Grapple_AssetsPending(engine_), 0);
    for (const Grapple_TextureId id : ids)
    {
        EXPECT_EQ(Grapple_AssetStatusOf(engine_, id), GRAPPLE_ASSET_MISSING);
    }

    for (const std::string &path : paths)
    {
        SDL_RemovePath(path.c_str());
    }
}

// A released slot is reused; the old handle must not resolve to whatever
// took its place.
TEST_F(AssetHarness, AReusedSlotDoesNotResurrectAnOldHandle)
{
    const std::string first_path = WritePng("grapple_asset_s1.png", 8, 0x11);
    const std::string second_path = WritePng("grapple_asset_s2.png", 16, 0x22);

    const Grapple_TextureId first = Grapple_LoadTexture(engine_, first_path.c_str());
    Grapple_AssetRelease(engine_, first);
    const Grapple_TextureId second = Grapple_LoadTexture(engine_, second_path.c_str());

    EXPECT_NE(first, second);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, first), GRAPPLE_ASSET_MISSING);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, second), GRAPPLE_ASSET_READY);

    SDL_RemovePath(first_path.c_str());
    SDL_RemovePath(second_path.c_str());
}

TEST_F(AssetHarness, WorkerCountIsClamped)
{
    Grapple_AssetsSetWorkers(engine_, 99);
    Grapple_AssetsSetWorkers(engine_, 0);
    // No crash and loading still works, which is all a caller can observe.
    const std::string path = WritePng("grapple_asset_w.png", 8, 0x70);
    EXPECT_NE(Grapple_LoadTexture(engine_, path.c_str()), GRAPPLE_ASSET_NONE);
    SDL_RemovePath(path.c_str());
}

TEST_F(AssetHarness, NullsAreHandled)
{
    EXPECT_EQ(Grapple_LoadTexture(nullptr, "x.png"), GRAPPLE_ASSET_NONE);
    EXPECT_EQ(Grapple_LoadTexture(engine_, nullptr), GRAPPLE_ASSET_NONE);
    EXPECT_EQ(Grapple_Texture(nullptr, 1), nullptr);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, 0), GRAPPLE_ASSET_MISSING);
    EXPECT_EQ(Grapple_AssetStatusOf(engine_, 0xDEADBEEF), GRAPPLE_ASSET_MISSING);
    EXPECT_EQ(Grapple_AssetPath(engine_, 0), nullptr);
    Grapple_AssetRelease(engine_, 0);
    Grapple_AssetRetain(engine_, 0);
    EXPECT_TRUE(Grapple_AssetsReady(nullptr));
    EXPECT_FLOAT_EQ(Grapple_AssetsProgress(nullptr), 1.0f);
    Grapple_AssetsWait(nullptr);
}

} // namespace
