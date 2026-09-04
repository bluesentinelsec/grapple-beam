/*
 * Uses the installed C++ SDK the way a C++ game would: the RAII wrapper, one
 * archive, no mention of the C SDK.
 *
 * The C++ API is headers over the C one, so this is as much a test that the
 * headers were installed and agree with the objects as it is a link test.
 */
#include <SDL3/SDL.h>
#include <cstdio>
#include <grapple/grapple.h>

int main()
{
    // A zeroed config is the documented starting point in C, and the C++
    // wrapper takes the same struct rather than inventing a second one.
    Grapple_EngineConfig config{};
    // Headless with a manual clock: no window, no GPU, no waiting on a real
    // clock — this has to run on a CI machine with no display.
    config.headless = true;
    config.manual_clock = true;
    config.no_auto_mount = true;

    grapple::Result<grapple::Engine> engine = grapple::Engine::Create(config);
    if (!engine.ok())
    {
        std::fprintf(stderr, "Engine::Create failed: %s\n", engine.status().message().c_str());
        return 1;
    }

    // Build a retained interface through the installed C++ wrapper. This
    // checks that both the public declarations and the compiled wrapper ship
    // in the C++ SDK archive.
    grapple::Result<grapple::Ui> ui = grapple::Ui::CreateForEngine(*engine);
    if (!ui.ok())
    {
        std::fprintf(stderr, "Ui::CreateForEngine failed: %s\n", ui.status().message().c_str());
        return 1;
    }
    grapple::PanelOptions panel_options;
    grapple::Result<grapple::Widget> panel = ui->AddPanel(panel_options);
    if (!panel.ok())
    {
        std::fprintf(stderr, "Ui::AddPanel failed: %s\n", panel.status().message().c_str());
        return 1;
    }
    grapple::LabelOptions label_options;
    label_options.text = "Installed C++ UI";
    grapple::Result<grapple::Widget> label = panel->AddLabel(label_options);
    if (!label.ok())
    {
        std::fprintf(stderr, "Widget::AddLabel failed: %s\n", label.status().message().c_str());
        return 1;
    }
    engine->on_post_render([&ui]() { ui->Draw(); });

    for (int i = 0; i < 5; ++i)
    {
        engine->Advance(16666667);
        engine->Tick();
    }

    if (engine->frame_count() < 5)
    {
        std::fprintf(stderr, "engine ran %llu frames, expected 5\n",
                     static_cast<unsigned long long>(engine->frame_count()));
        return 1;
    }

    std::printf("C++ SDK consumer ok: %llu frames, SDL %d.%d.%d\n",
                static_cast<unsigned long long>(engine->frame_count()), SDL_MAJOR_VERSION,
                SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    return 0;
}
