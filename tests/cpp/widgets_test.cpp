// widgets_test.cpp — caller-facing tests for the retained C++ UI.

#include "grapple/app.h"
#include "grapple/game.h"
#include "grapple/ui.h"

#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

namespace
{

TEST(CppWidgets, DeclaresAWorkingInterfaceOnce)
{
    grapple::Result<grapple::SdlInit> sdl = grapple::SdlInit::Create(0);
    ASSERT_TRUE(sdl.ok()) << sdl.status().message();
    grapple::Result<grapple::Surface> canvas = grapple::Surface::Create(320, 200);
    ASSERT_TRUE(canvas.ok()) << canvas.status().message();
    grapple::Result<grapple::Renderer> renderer = grapple::Renderer::CreateSoftware(*canvas);
    ASSERT_TRUE(renderer.ok()) << renderer.status().message();
    grapple::Result<grapple::Ui> ui_result = grapple::Ui::Open(renderer->get());
    ASSERT_TRUE(ui_result.ok()) << ui_result.status().message();
    grapple::Ui ui = std::move(ui_result).value();

    grapple::PanelOptions panel_options;
    panel_options.title = "Settings";
    panel_options.padding = 12.0f;
    panel_options.spacing = 8.0f;
    grapple::Result<grapple::Widget> panel_result = ui.AddPanel(panel_options);
    ASSERT_TRUE(panel_result.ok()) << panel_result.status().message();
    grapple::Widget panel = std::move(panel_result).value();

    grapple::LabelOptions label_options;
    label_options.text = "Ready";
    label_options.align = grapple::UiAlign::kCenter;
    grapple::Result<grapple::Widget> label_result = panel.AddLabel(label_options);
    ASSERT_TRUE(label_result.ok()) << label_result.status().message();
    grapple::Widget status = std::move(label_result).value();

    int clicks = 0;
    grapple::ButtonOptions button_options;
    button_options.text = "Save";
    button_options.value = "saved";
    button_options.width = grapple::UiLength::Fit();
    button_options.on_click = [&status, &clicks](grapple::Widget button) {
        status.SetText(button.value_text());
        ++clicks;
    };
    grapple::Result<grapple::Widget> button_result = panel.AddButton(button_options);
    ASSERT_TRUE(button_result.ok()) << button_result.status().message();
    grapple::Widget button = std::move(button_result).value();

    button.Invoke();
    EXPECT_EQ(clicks, 1);
    EXPECT_EQ(status.text(), "saved");

    ui.Draw();
    grapple::Result<grapple::UiBounds> bounds = button.Bounds();
    ASSERT_TRUE(bounds.ok()) << bounds.status().message();
    EXPECT_GT(bounds->width, 0.0f);
    EXPECT_GT(bounds->height, 0.0f);
}

TEST(CppWidgets, OwnsChoiceStateAndSupportsRawNuklearDrawing)
{
    grapple::Result<grapple::SdlInit> sdl = grapple::SdlInit::Create(0);
    ASSERT_TRUE(sdl.ok()) << sdl.status().message();
    grapple::Result<grapple::Surface> canvas = grapple::Surface::Create(320, 200);
    ASSERT_TRUE(canvas.ok()) << canvas.status().message();
    grapple::Result<grapple::Renderer> renderer = grapple::Renderer::CreateSoftware(*canvas);
    ASSERT_TRUE(renderer.ok()) << renderer.status().message();
    grapple::Result<grapple::Ui> ui_result = grapple::Ui::Open(renderer->get());
    ASSERT_TRUE(ui_result.ok()) << ui_result.status().message();
    grapple::Ui ui = std::move(ui_result).value();

    grapple::PanelOptions panel_options;
    grapple::Result<grapple::Widget> panel_result = ui.AddPanel(panel_options);
    ASSERT_TRUE(panel_result.ok()) << panel_result.status().message();
    grapple::Widget panel = std::move(panel_result).value();

    grapple::ChoiceOptions choice_options;
    choice_options.options = {"Easy", "Normal", "Hard"};
    choice_options.selected = 1;
    grapple::Result<grapple::Widget> choice_result = panel.AddSelect(choice_options);
    ASSERT_TRUE(choice_result.ok()) << choice_result.status().message();
    grapple::Widget choice = std::move(choice_result).value();
    EXPECT_EQ(choice.selected(), 1);
    EXPECT_EQ(choice.text(), "Normal");
    EXPECT_EQ(choice.options(), (std::vector<std::string>{"Easy", "Normal", "Hard"}));

    choice.SetSelected(2);
    EXPECT_EQ(choice.text(), "Hard");

    int raw_draws = 0;
    grapple::RawOptions raw_options;
    raw_options.draw = [&raw_draws](struct nk_context *context) {
        EXPECT_NE(context, nullptr);
        ++raw_draws;
    };
    grapple::Result<grapple::Widget> raw = panel.AddRaw(raw_options);
    ASSERT_TRUE(raw.ok()) << raw.status().message();

    ui.Draw();
    EXPECT_EQ(raw_draws, 1);
}

TEST(CppWidgets, AttachesToTheEngineAndSurvivesAMove)
{
    grapple::Result<grapple::SdlInit> sdl = grapple::SdlInit::Create(0);
    ASSERT_TRUE(sdl.ok()) << sdl.status().message();

    Grapple_EngineConfig config{};
    config.headless = true;
    config.manual_clock = true;
    config.no_auto_mount = true;
    config.design_width = 320;
    config.design_height = 200;
    grapple::Result<grapple::Engine> engine_result = grapple::Engine::Create(config);
    ASSERT_TRUE(engine_result.ok()) << engine_result.status().message();
    grapple::Engine engine = std::move(engine_result).value();

    grapple::Result<grapple::Ui> ui_result = grapple::Ui::CreateForEngine(engine);
    ASSERT_TRUE(ui_result.ok()) << ui_result.status().message();
    grapple::Ui ui = std::move(ui_result).value();

    grapple::PanelOptions panel_options;
    grapple::Result<grapple::Widget> panel = ui.AddPanel(panel_options);
    ASSERT_TRUE(panel.ok()) << panel.status().message();

    int clicks = 0;
    grapple::ButtonOptions button_options;
    button_options.text = "Start";
    button_options.on_click = [&clicks](grapple::Widget) { ++clicks; };
    grapple::Result<grapple::Widget> button = panel->AddButton(button_options);
    ASSERT_TRUE(button.ok()) << button.status().message();

    grapple::Ui moved = std::move(ui);
    button->Invoke();
    EXPECT_EQ(clicks, 1);
    EXPECT_NE(moved.event_sink().begin, nullptr);
    moved.Draw();
}

TEST(CppWidgets, RejectsChildrenOnAnEmptyHandle)
{
    grapple::Widget empty;
    grapple::ButtonOptions options;
    options.text = "No parent";
    grapple::Result<grapple::Widget> child = empty.AddButton(options);
    EXPECT_FALSE(child.ok());
    EXPECT_EQ(child.status().message(), "cannot add to an empty UI widget");
}

TEST(CppWidgets, ReplacesAnImageWithoutRebuildingTheWidgetTree)
{
    grapple::Result<grapple::SdlInit> sdl = grapple::SdlInit::Create(0);
    ASSERT_TRUE(sdl.ok()) << sdl.status().message();
    grapple::Result<grapple::Surface> canvas = grapple::Surface::Create(320, 200);
    ASSERT_TRUE(canvas.ok()) << canvas.status().message();
    grapple::Result<grapple::Renderer> renderer = grapple::Renderer::CreateSoftware(*canvas);
    ASSERT_TRUE(renderer.ok()) << renderer.status().message();
    grapple::Result<grapple::Ui> ui_result = grapple::Ui::Open(renderer->get());
    ASSERT_TRUE(ui_result.ok()) << ui_result.status().message();
    grapple::Ui ui = std::move(ui_result).value();

    grapple::PanelOptions panel_options;
    grapple::Result<grapple::Widget> panel = ui.AddPanel(panel_options);
    ASSERT_TRUE(panel.ok()) << panel.status().message();

    SDL_Texture *first = SDL_CreateTexture(renderer->get(), SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_TARGET, 16, 16);
    SDL_Texture *second = SDL_CreateTexture(renderer->get(), SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_TARGET, 32, 32);
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    grapple::ImageOptions image_options;
    image_options.texture = first;
    grapple::Result<grapple::Widget> image = panel->AddImage(image_options);
    ASSERT_TRUE(image.ok()) << image.status().message();
    EXPECT_TRUE(image->SetImage(second).ok());
    EXPECT_TRUE(image->SetImage(nullptr).ok());

    SDL_DestroyTexture(second);
    SDL_DestroyTexture(first);
}

} // namespace
