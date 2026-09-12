#include <grapple/engine_backend.h>
#include <grapple/engine_config.h>
#include <grapple/engine_graphics.h>
#include <gtest/gtest.h>
#include <set>
#include <string>

TEST(RenderBackend, CanonicalNamesAreConcreteAndUnique)
{
    std::set<std::string> names;
    for (int i = 0; i < Grapple_RenderBackendCount(); ++i)
    {
        const char *name = Grapple_RenderBackendName(i);
        ASSERT_NE(name, nullptr);
        EXPECT_TRUE(names.insert(name).second);
        EXPECT_TRUE(Grapple_RenderBackendValid(name));
    }
    EXPECT_EQ(names, (std::set<std::string>{"directx11", "directx12", "vulkan", "metal", "opengl",
                                            "opengles2", "software"}));
    EXPECT_TRUE(Grapple_RenderBackendValid("auto"));
    EXPECT_FALSE(Grapple_RenderBackendValid("native"));
    EXPECT_FALSE(Grapple_RenderBackendValid("opengl,software"));
    EXPECT_FALSE(Grapple_RenderBackendValid(nullptr));
    EXPECT_EQ(Grapple_RenderBackendName(-1), nullptr);
    EXPECT_EQ(Grapple_RenderBackendName(Grapple_RenderBackendCount()), nullptr);
}

TEST(RenderBackend, ConfigBuilderOwnsAndValidatesTheSelection)
{
    auto *config = Grapple_ConfigCreate();
    ASSERT_NE(config, nullptr);
    char name[] = "software";
    ASSERT_TRUE(Grapple_ConfigSetRendererBackend(config, name));
    name[0] = 'x';
    EXPECT_STREQ(config->renderer_backend, "software");
    EXPECT_FALSE(Grapple_ConfigSetRendererBackend(config, "native"));
    EXPECT_STREQ(config->renderer_backend, "software");
    Grapple_ConfigDestroy(config);
}

TEST(RenderBackend, InvalidSelectionFailsBeforeOpeningAWindow)
{
    Grapple_EngineConfig config{};
    config.renderer_backend = "imaginary";
    EXPECT_EQ(Grapple_CreateEngine(&config), nullptr);
    EXPECT_NE(std::string(SDL_GetError()).find("unknown renderer backend"), std::string::npos);
}

TEST(RenderBackend, HeadlessCannotSilentlyIgnoreAnExplicitHardwareBackend)
{
    Grapple_EngineConfig config{};
    config.headless = true;
    config.renderer_backend = "metal";
    EXPECT_EQ(Grapple_CreateEngine(&config), nullptr);
    EXPECT_NE(std::string(SDL_GetError()).find("headless"), std::string::npos);
}

TEST(RenderBackend, UnknownProbeDoesNotPretendToSucceed)
{
    Grapple_RenderBackendInfo info{};
    EXPECT_FALSE(Grapple_ProbeRenderBackend("auto", &info));
    EXPECT_FALSE(Grapple_ProbeRenderBackend("software", nullptr));
}

TEST(RenderBackend, SoftwareProbeReportsActualRendererWithoutInventingVersions)
{
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_VIDEO));
    Grapple_RenderBackendInfo info{};
    ASSERT_TRUE(Grapple_ProbeRenderBackend("software", &info));
    EXPECT_TRUE(info.compiled);
    EXPECT_TRUE(info.available) << info.error;
    EXPECT_STREQ(info.renderer, "software");
    EXPECT_STREQ(info.api_version, "");
    EXPECT_FALSE(info.opengl_effects);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

TEST(RenderBackend, CliOverridesBackendDefaultAndWindowModeAtCreation)
{
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_VIDEO));
    Grapple_EngineConfig config{};
    config.no_auto_mount = true;
    config.start_hidden = true;
    config.fullscreen = true;
    config.renderer_backend = "imaginary";
    char app[] = "game";
    char backend[] = "--backend=software";
    char window[] = "--window-mode=windowed";
    char *args[] = {app, backend, window};
    config.argc = 3;
    config.argv = args;
    auto *engine = Grapple_CreateEngine(&config);
    ASSERT_NE(engine, nullptr) << SDL_GetError();
    EXPECT_STREQ(SDL_GetRendererName(Grapple_EngineRenderer(engine)), "software");
    EXPECT_EQ(SDL_GetWindowFlags(Grapple_EngineWindow(engine)) & SDL_WINDOW_FULLSCREEN, 0u);
    Grapple_DestroyEngine(engine);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

TEST(GraphicsArgs, CanonicalModesAndFrameCapsAreApplied)
{
    auto graphics = Grapple_GraphicsDefaults();
    char app[] = "game";
    char mode[] = "--window-mode=fullscreen-exclusive";
    char fps[] = "--max-fps=unlimited";
    char *args[] = {app, mode, fps};
    EXPECT_EQ(Grapple_GraphicsLoadArgs(&graphics, 3, args), 2);
    EXPECT_EQ(graphics.window_mode, GRAPPLE_WINDOW_EXCLUSIVE);
    EXPECT_EQ(graphics.max_fps, -1);
    char *toml = Grapple_GraphicsToToml(&graphics);
    ASSERT_NE(toml, nullptr);
    EXPECT_NE(std::string(toml).find("fullscreen-exclusive"), std::string::npos);
    SDL_free(toml);
}
