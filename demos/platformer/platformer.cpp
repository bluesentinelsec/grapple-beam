// platformer.cpp — a platformer, in C++, on Grapple::Platformer.
//
// Original grapple-beam code (zlib). The same level as platformer.c, through
// the RAII wrappers: the engine, the level and the hooks are members of one
// class, and there is no void* to cast back.
#include <SDL3/SDL.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <grapple/debug_text.h>
#include <grapple/engine_input.h>
#include <grapple/game.h>
#include <grapple/platformer_level.h>
#include <utility>

namespace
{

// Nothing here is about the display: the level frames and presents the view.
constexpr int kTile = 16;

class PlatformerGame
{
  public:
    static int Run()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            return EXIT_FAILURE;
        }
        const int result = PlatformerGame().Play();
        SDL_Quit();
        return result;
    }

  private:
    int Play()
    {
        headless_ = SDL_getenv("GRAPPLE_HEADLESS") != nullptr;

        Grapple_EngineConfig config{};
        config.title = "Platformer — grapple-beam";
        config.no_auto_mount = true;
        config.headless = headless_;
        engine_ = Grapple_CreateEngine(&config);
        if (engine_ == nullptr)
        {
            SDL_Log("could not create an engine: %s", SDL_GetError());
            return EXIT_FAILURE;
        }

        auto level = grapple::Platformer::Create(engine_, 212, 22, kTile);
        if (!level.ok())
        {
            SDL_Log("could not create the level: %s", level.status().message().c_str());
            Grapple_DestroyEngine(engine_);
            return EXIT_FAILURE;
        }
        level_ = std::move(*level);
        Build();
        auto player = level_.CreatePlayer(3, 19);
        if (!player.ok())
        {
            SDL_Log("could not create the player: %s", player.status().message().c_str());
            level_.reset();
            Grapple_DestroyEngine(engine_);
            return EXIT_FAILURE;
        }
        player_ = *player;
        player_.set_scripted_input(headless_);
        if (const grapple::Status status = level_.Attach(); !status.ok())
        {
            SDL_Log("could not attach the level: %s", status.message().c_str());
        }

        Grapple_GameHooks hooks{};
        hooks.update = [](void *user, float dt) {
            static_cast<PlatformerGame *>(user)->Update(dt);
        };
        // post_render: after the frame is enlarged, so the text is drawn at window density.
        hooks.post_render = [](void *user) { static_cast<PlatformerGame *>(user)->Render(); };
        const bool ok = Grapple_RunGame(engine_, &hooks, this);

        level_.reset();
        Grapple_DestroyEngine(engine_);
        return ok ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    void Build()
    {
        level_.CreateFloor(0, 20, 69);
        level_.CreateFloor(71, 20, 15);
        level_.CreateFloor(89, 20, 64);
        level_.CreateFloor(155, 20, 57);

        level_.CreateBlock(16, 16, 1, 1);
        level_.CreateBlock(20, 16, 5, 1);
        level_.CreateBlock(22, 12, 1, 1);
        level_.CreateBlock(37, 15, 3, 1);
        level_.CreateBlock(45, 17, 1, 1);
        level_.CreateBlock(52, 16, 2, 1);
        level_.CreateBlock(77, 16, 3, 1);
        level_.CreateBlock(80, 12, 8, 1);
        level_.CreateBlock(91, 12, 3, 1);
        level_.CreateBlock(94, 16, 2, 1);
        level_.CreateBlock(64, 17, 2, 1);
        level_.CreateBlock(66, 14, 2, 1);
        level_.CreateBlock(68, 11, 2, 1);
        level_.CreateBlock(70, 8, 2, 1);
        level_.CreateBlock(72, 5, 6, 1);
        level_.CreateBlock(96, 3, 4, 1);
        level_.CreateBlock(105, 3, 6, 1);
        level_.CreateBlock(189, 12, 1, 8);

        level_.CreatePlatform(30, 15, 4);
        level_.CreatePlatform(58, 13, 5);
        level_.CreatePlatform(113, 6, 4);
        level_.CreatePlatform(119, 9, 4);

        // Pipes to climb over, and at x = 100 a shaft three tiles wide to wall-jump up.
        for (const auto &[x, y, height] :
             {std::tuple{28, 18, 2}, std::tuple{29, 18, 2}, std::tuple{38, 17, 3},
              std::tuple{39, 17, 3}, std::tuple{46, 16, 4}, std::tuple{47, 16, 4},
              std::tuple{57, 16, 4}, std::tuple{58, 16, 4}, std::tuple{100, 4, 16},
              std::tuple{104, 4, 16}})
        {
            level_.CreateWall(x, y, height);
        }

        level_.CreateStairs(134, 19, 4, true);
        level_.CreateStairs(140, 19, 4, false);
        level_.CreateStairs(148, 19, 4, true);
        level_.CreateStairs(155, 19, 4, false);
        level_.CreateStairs(181, 19, 8, true);
    }

    void Update(float dt)
    {
        if (Grapple_KeyPressed(engine_, SDL_SCANCODE_ESCAPE))
            Grapple_EngineQuit(engine_);
        if (!headless_)
            return;
        // Nobody at the keyboard: run right, jump now and then, and narrate.
        seconds_ += dt;
        player_.Input(1.0f, std::fmod(seconds_, 1.5f) < 0.4f, true);
        if (player_.state_changed())
        {
            const auto [x, y] = player_.position();
            std::printf("state=%s x=%.0f y=%.0f\n", player_.state_name(), static_cast<double>(x),
                        static_cast<double>(y));
            std::fflush(stdout);
        }
    }

    void Render()
    {
        SDL_Renderer *renderer = Grapple_EngineRenderer(engine_);
        const auto [x, y] = player_.position();
        Grapple_SetDebugTextSize(8.0f);
        Grapple_RenderDebugTextFormat(renderer, 8.0f, 8.0f, "%s  x=%d", player_.state_name(),
                                      static_cast<int>(x / kTile));
        Grapple_RenderDebugText(renderer, 8.0f, 202.0f,
                                "arrows/AD move  shift run  space jump (walls too)  esc quit");
    }

    Grapple_Engine *engine_ = nullptr;
    grapple::Platformer level_;
    grapple::PlatformerPlayer player_;
    bool headless_ = false;
    float seconds_ = 0.0f;
};

} // namespace

int main(int, char **)
{
    return PlatformerGame::Run();
}
