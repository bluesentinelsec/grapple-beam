/** @file play.cpp @brief Managed file playback with C++ ownership and the engine loop. */
#include <cstdio>
#include <grapple/debug_text.h>
#include <grapple/gen/grapple.h>
#include <grapple/grapple.h>

namespace
{
int Play(const char *path)
{
    auto player = grapple::ext::ChipPlayer::PlayChipFile(path, false);
    if (!player.ok())
    {
        std::fprintf(stderr, "%s\n", player.status().message().c_str());
        return 1;
    }
    Grapple_EngineConfig config{};
    config.title = "Chiptune from C++";
    config.design_width = 800;
    config.design_height = 240;
    config.no_auto_mount = true;
    config.headless = SDL_getenv("GRAPPLE_HEADLESS") != nullptr;
    auto engine = grapple::Engine::Create(config);
    if (!engine.ok())
        return 1;
    engine->on_fixed_update([&](float) {
        if (!player->ChipPlayerPlaying())
            engine->Quit();
    });
    engine->on_render([&](float) {
        SDL_Renderer *renderer = engine->renderer();
        SDL_SetRenderDrawColor(renderer, 20, 22, 42, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 150, 220, 255, 255);
        Grapple_RenderDebugText(renderer, 24, 40, path);
    });
    return engine->Run() ? 0 : 1;
}
} // namespace

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "usage: chiptune-cpp score.mid|score.musicxml|score.mxl\n");
        return 2;
    }
    if (!SDL_Init(SDL_INIT_AUDIO) || !MIX_Init())
    {
        std::fprintf(stderr, "%s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    const int result = Play(argv[1]);
    MIX_Quit();
    SDL_Quit();
    return result;
}
