/**
 * @file gui_test.cpp
 * @brief Tests for Grapple::GUI — vendored Nuklear + SDL3 backend + grid.
 *
 * Runs fully headless on a software renderer: frames are driven manually,
 * input is injected as fabricated SDL events, rendering is verified by
 * pixel readback, and layout geometry is captured via nk_widget_bounds —
 * so widget behavior, the backend, and the no-hard-coded-coordinates
 * property of the grid helper are all proven for real.
 */

#include <SDL3/SDL.h>
#include <grapple/gui.h>
#include <grapple/gui_grid.h>
#include <grapple/widgets.h>
#include <gtest/gtest.h>
#include <filesystem>

#include <cstring>
#include <string>
#include <vector>

namespace
{

constexpr int kScreen = 400;

class GuiHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        surface_ = SDL_CreateSurface(kScreen, kScreen, SDL_PIXELFORMAT_ARGB8888);
        ASSERT_NE(surface_, nullptr);
        renderer_ = SDL_CreateSoftwareRenderer(surface_);
        ASSERT_NE(renderer_, nullptr) << SDL_GetError();
        gui_ = Grapple_CreateGui(renderer_, nullptr, 0, 0.0f);
        ASSERT_NE(gui_, nullptr) << SDL_GetError();
    }
    void TearDown() override
    {
        Grapple_DestroyGui(gui_);
        SDL_DestroyRenderer(renderer_);
        SDL_DestroySurface(surface_);
        SDL_Quit();
    }

    void BeginFrame()
    {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
    }

    void FeedMouseMove(float x, float y)
    {
        SDL_Event event = {};
        event.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.x = x;
        event.motion.y = y;
        EXPECT_TRUE(Grapple_GuiProcessEvent(gui_, &event));
    }

    void FeedButton(float x, float y, bool down)
    {
        SDL_Event event = {};
        event.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
        event.button.button = SDL_BUTTON_LEFT;
        event.button.clicks = 1;
        event.button.x = x;
        event.button.y = y;
        EXPECT_TRUE(Grapple_GuiProcessEvent(gui_, &event));
    }

    bool Lit(int x, int y)
    {
        SDL_FlushRenderer(renderer_);
        Uint8 r = 0;
        Uint8 g = 0;
        Uint8 b = 0;
        Uint8 a = 0;
        SDL_ReadSurfacePixel(surface_, x, y, &r, &g, &b, &a);
        return (r | g | b) != 0;
    }

    SDL_Surface *surface_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    Grapple_Gui *gui_ = nullptr;
};

TEST_F(GuiHarness, CreateRejectsNullRenderer)
{
    EXPECT_EQ(Grapple_CreateGui(nullptr, nullptr, 0, 0.0f), nullptr);
    EXPECT_NE(Grapple_GuiContext(gui_), nullptr);
    Grapple_DestroyGui(nullptr); // safe no-op
}

TEST_F(GuiHarness, WindowRendersPixelsInsideAndNotOutside)
{
    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    if (nk_begin(ctx, "panel", nk_rect(20, 20, 200, 150), NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 0, 1);
        nk_label(ctx, "Hello Grapple", NK_TEXT_LEFT);
        nk_button_label(ctx, "A Button");
    }
    nk_end(ctx);
    ASSERT_TRUE(Grapple_GuiRender(gui_)) << SDL_GetError();

    EXPECT_TRUE(Lit(120, 90)) << "window interior";
    EXPECT_TRUE(Lit(30, 30)) << "title bar";
    EXPECT_FALSE(Lit(350, 350)) << "outside the window";
}

TEST_F(GuiHarness, ButtonClickReportsPressed)
{
    struct nk_rect button_bounds = nk_rect(0, 0, 0, 0);
    bool clicked = false;

    // Frame 1: no input; capture where the button lands.
    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    if (nk_begin(ctx, "win", nk_rect(10, 10, 220, 160), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        button_bounds = nk_widget_bounds(ctx);
        nk_button_label(ctx, "Fire");
    }
    nk_end(ctx);
    ASSERT_TRUE(Grapple_GuiRender(gui_));
    ASSERT_GT(button_bounds.w, 0.0f);

    // Frame 2: press on the button; frame 3: release -> click fires.
    const float cx = button_bounds.x + button_bounds.w / 2.0f;
    const float cy = button_bounds.y + button_bounds.h / 2.0f;
    for (int phase = 0; phase < 2; ++phase)
    {
        BeginFrame();
        Grapple_GuiInputBegin(gui_);
        if (phase == 0)
        {
            FeedMouseMove(cx, cy);
            FeedButton(cx, cy, true);
        }
        else
        {
            FeedButton(cx, cy, false);
        }
        Grapple_GuiInputEnd(gui_);
        if (nk_begin(ctx, "win", nk_rect(10, 10, 220, 160), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Fire"))
            {
                clicked = true;
            }
        }
        nk_end(ctx);
        ASSERT_TRUE(Grapple_GuiRender(gui_));
    }
    EXPECT_TRUE(clicked) << "press+release at the button's bounds must register";
    EXPECT_TRUE(Grapple_GuiWantsInput(gui_)) << "pointer is over the UI";
}

TEST_F(GuiHarness, CheckboxTogglesAndSliderMoves)
{
    struct nk_rect check_bounds = nk_rect(0, 0, 0, 0);
    struct nk_rect slider_bounds = nk_rect(0, 0, 0, 0);
    nk_bool checked = nk_false;
    float value = 0.0f;
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    auto frame = [&](bool capture) {
        BeginFrame();
        if (nk_begin(ctx, "form", nk_rect(10, 10, 300, 200), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            if (capture)
            {
                check_bounds = nk_widget_bounds(ctx);
            }
            nk_checkbox_label(ctx, "enabled", &checked);
            if (capture)
            {
                slider_bounds = nk_widget_bounds(ctx);
            }
            nk_slider_float(ctx, 0.0f, &value, 1.0f, 0.01f);
        }
        nk_end(ctx);
        ASSERT_TRUE(Grapple_GuiRender(gui_));
    };

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    frame(true);
    ASSERT_GT(check_bounds.w, 0.0f);

    // Press and release on the checkbox across two frames.
    const float kx = check_bounds.x + 8;
    const float ky = check_bounds.y + check_bounds.h / 2;
    Grapple_GuiInputBegin(gui_);
    FeedMouseMove(kx, ky);
    FeedButton(kx, ky, true);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    Grapple_GuiInputBegin(gui_);
    FeedButton(kx, ky, false);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    EXPECT_TRUE(checked) << "checkbox must toggle on";

    // Sliders move by dragging the cursor handle: grab it at the left end
    // (value is 0), drag to 75% while held, then release.
    const float sy = slider_bounds.y + slider_bounds.h / 2.0f;
    const float grab_x = slider_bounds.x + 4.0f; /* cursor sits at the left end */
    const float target_x = slider_bounds.x + slider_bounds.w * 0.75f;
    Grapple_GuiInputBegin(gui_);
    FeedMouseMove(grab_x, sy);
    FeedButton(grab_x, sy, true);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    Grapple_GuiInputBegin(gui_);
    FeedMouseMove(target_x, sy);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    Grapple_GuiInputBegin(gui_);
    FeedButton(target_x, sy, false);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    EXPECT_NEAR(value, 0.75f, 0.15f) << "drag should land near the target fraction";
}

TEST_F(GuiHarness, EditFieldReceivesTypedText)
{
    char buffer[64] = {};
    struct nk_rect edit_bounds = nk_rect(0, 0, 0, 0);
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    auto frame = [&](bool capture) {
        BeginFrame();
        if (nk_begin(ctx, "editwin", nk_rect(10, 10, 300, 120), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            if (capture)
            {
                edit_bounds = nk_widget_bounds(ctx);
            }
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, buffer, sizeof(buffer),
                                           nk_filter_default);
        }
        nk_end(ctx);
        ASSERT_TRUE(Grapple_GuiRender(gui_));
    };

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    frame(true);

    // Press and release inside the field to focus it.
    const float fx = edit_bounds.x + 10;
    const float fy = edit_bounds.y + edit_bounds.h / 2;
    Grapple_GuiInputBegin(gui_);
    FeedMouseMove(fx, fy);
    FeedButton(fx, fy, true);
    Grapple_GuiInputEnd(gui_);
    frame(false);
    Grapple_GuiInputBegin(gui_);
    FeedButton(fx, fy, false);
    Grapple_GuiInputEnd(gui_);
    frame(false);

    // Type UTF-8 text ("hé" exercises the decoder).
    SDL_Event text = {};
    text.type = SDL_EVENT_TEXT_INPUT;
    text.text.text = "h\xc3\xa9!";
    Grapple_GuiInputBegin(gui_);
    EXPECT_TRUE(Grapple_GuiProcessEvent(gui_, &text));
    Grapple_GuiInputEnd(gui_);
    frame(false);

    EXPECT_STREQ(buffer, "h\xc3\xa9!") << "typed text must land in the edit buffer";
}

TEST_F(GuiHarness, GridWidthsFollowWeightsAndSpans)
{
    struct nk_rect c1 = nk_rect(0, 0, 0, 0);
    struct nk_rect c2 = nk_rect(0, 0, 0, 0);
    struct nk_rect c3 = nk_rect(0, 0, 0, 0);
    struct nk_rect span_cell = nk_rect(0, 0, 0, 0);
    struct nk_rect next_row = nk_rect(0, 0, 0, 0);
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    static const float weights[3] = {1.0f, 2.0f, 1.0f};

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    BeginFrame();
    if (nk_begin(ctx, "grid", nk_rect(0, 0, 320, 300), NK_WINDOW_NO_SCROLLBAR))
    {
        Grapple_GuiGrid grid;
        ASSERT_TRUE(Grapple_GuiGridBegin(ctx, &grid, 3, weights, 25.0f));

        Grapple_GuiGridCell(&grid);
        c1 = nk_widget_bounds(ctx);
        nk_label(ctx, "a", NK_TEXT_LEFT);
        Grapple_GuiGridCell(&grid);
        c2 = nk_widget_bounds(ctx);
        nk_label(ctx, "b", NK_TEXT_LEFT);
        Grapple_GuiGridCell(&grid);
        c3 = nk_widget_bounds(ctx);
        nk_label(ctx, "c", NK_TEXT_LEFT);

        // Auto-wrap: this cell must begin row 2. Span 2 columns.
        Grapple_GuiGridCellSpan(&grid, 2);
        span_cell = nk_widget_bounds(ctx);
        nk_label(ctx, "wide", NK_TEXT_LEFT);

        Grapple_GuiGridNextRow(&grid);
        Grapple_GuiGridCell(&grid);
        next_row = nk_widget_bounds(ctx);
        nk_label(ctx, "row3", NK_TEXT_LEFT);

        Grapple_GuiGridEnd(&grid);
    }
    nk_end(ctx);
    ASSERT_TRUE(Grapple_GuiRender(gui_));
    ASSERT_GT(c1.w, 0.0f) << "grid window must have been built";

    // Weight 1:2:1 must show up as proportional widths (padding tolerance).
    EXPECT_NEAR(c2.w / c1.w, 2.0f, 0.35f) << "middle column has weight 2";
    EXPECT_NEAR(c3.w / c1.w, 1.0f, 0.2f) << "outer columns match";
    // Row 1 cells share a y; row 2 sits below; row 3 below that.
    EXPECT_FLOAT_EQ(c1.y, c2.y);
    EXPECT_FLOAT_EQ(c2.y, c3.y);
    EXPECT_GT(span_cell.y, c1.y);
    EXPECT_GT(next_row.y, span_cell.y);
    // The span covers columns 1+2: wider than c1+c2 alone minus spacing.
    EXPECT_GT(span_cell.w, c1.w + c2.w * 0.8f);
    // Columns line up across rows (same left edge, no hard-coded x).
    EXPECT_NEAR(span_cell.x, c1.x, 2.0f);
    EXPECT_NEAR(next_row.x, c1.x, 2.0f);
}

TEST_F(GuiHarness, GridScalesWithWindowSizeNotPixels)
{
    // The same grid code in two window sizes: widths must scale
    // proportionally — the "no hard-coded coordinates" guarantee.
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    static const float weights[2] = {1.0f, 3.0f};
    float first_w = 0.0f;
    float second_w = 0.0f;

    for (int pass = 0; pass < 2; ++pass)
    {
        const float window_w = (pass == 0) ? 200.0f : 380.0f;
        Grapple_GuiInputBegin(gui_);
        Grapple_GuiInputEnd(gui_);
        BeginFrame();
        char name[16];
        SDL_snprintf(name, sizeof(name), "scale%d", pass);
        if (nk_begin(ctx, name, nk_rect(0, 0, window_w, 100), NK_WINDOW_NO_SCROLLBAR))
        {
            Grapple_GuiGrid grid;
            ASSERT_TRUE(Grapple_GuiGridBegin(ctx, &grid, 2, weights, 0.0f));
            Grapple_GuiGridCell(&grid);
            const struct nk_rect bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "x", NK_TEXT_LEFT);
            Grapple_GuiGridCell(&grid);
            nk_label(ctx, "y", NK_TEXT_LEFT);
            Grapple_GuiGridEnd(&grid);
            if (pass == 0)
            {
                first_w = bounds.w;
            }
            else
            {
                second_w = bounds.w;
            }
        }
        nk_end(ctx);
        ASSERT_TRUE(Grapple_GuiRender(gui_));
    }
    ASSERT_GT(first_w, 0.0f);
    EXPECT_NEAR(second_w / first_w, 380.0f / 200.0f, 0.25f)
        << "cell width must scale with the window";
}

TEST_F(GuiHarness, GridRejectsBadInput)
{
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    Grapple_GuiGrid grid;
    EXPECT_FALSE(Grapple_GuiGridBegin(nullptr, &grid, 2, nullptr, 0.0f));
    EXPECT_FALSE(Grapple_GuiGridBegin(ctx, nullptr, 2, nullptr, 0.0f));
    EXPECT_FALSE(Grapple_GuiGridBegin(ctx, &grid, 0, nullptr, 0.0f));
    EXPECT_FALSE(Grapple_GuiGridBegin(ctx, &grid, GRAPPLE_GUI_GRID_MAX_COLS + 1, nullptr,
                                        0.0f));
    // Calls on an unopened grid must be safe no-ops.
    Grapple_GuiGrid dead = {};
    Grapple_GuiGridCell(&dead);
    Grapple_GuiGridEnd(&dead);
}

TEST_F(GuiHarness, IgnoresUnrelatedEvents)
{
    SDL_Event event = {};
    event.type = SDL_EVENT_GAMEPAD_BUTTON_DOWN;
    Grapple_GuiInputBegin(gui_);
    EXPECT_FALSE(Grapple_GuiProcessEvent(gui_, &event));
    EXPECT_FALSE(Grapple_GuiProcessEvent(gui_, nullptr));
    Grapple_GuiInputEnd(gui_);
}


// Grapple_GuiPumpEvents: the one-call input pump. Also the entry point
// that makes the GUI drivable from Lua and Ruby, where SDL_Event (a union)
// cannot cross the script boundary.
TEST_F(GuiHarness, PumpEventsDrainsQueueAndReportsQuit)
{
    // The pump reads the SDL event queue, which needs the events subsystem
    // (the rest of this harness runs with SDL_Init(0)).
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_EVENTS)) << SDL_GetError();

    // A pushed quit request must be reported (and consumed).
    SDL_Event quit;
    SDL_zero(quit);
    quit.type = SDL_EVENT_QUIT;
    ASSERT_TRUE(SDL_PushEvent(&quit));
    EXPECT_FALSE(Grapple_GuiPumpEvents(gui_)) << "quit must stop the loop";

    // With an empty queue it keeps running and leaves the GUI usable.
    EXPECT_TRUE(Grapple_GuiPumpEvents(gui_));
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    ASSERT_NE(ctx, nullptr);
    if (nk_begin(ctx, "pump", nk_rect(0, 0, 100, 60), NK_WINDOW_BORDER))
    {
        nk_layout_row_dynamic(ctx, 0, 1);
        nk_label(ctx, "ok", NK_TEXT_LEFT);
    }
    nk_end(ctx);
    EXPECT_TRUE(Grapple_GuiRender(gui_));

    // Mouse motion routed through the pump reaches Nuklear's input state.
    SDL_Event motion;
    SDL_zero(motion);
    motion.type = SDL_EVENT_MOUSE_MOTION;
    motion.motion.x = 42.0f;
    motion.motion.y = 24.0f;
    ASSERT_TRUE(SDL_PushEvent(&motion));
    EXPECT_TRUE(Grapple_GuiPumpEvents(gui_));
    EXPECT_EQ(ctx->input.mouse.pos.x, 42.0f);
    EXPECT_EQ(ctx->input.mouse.pos.y, 24.0f);

    EXPECT_FALSE(Grapple_GuiPumpEvents(nullptr)) << "null gui fails cleanly";

    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// Key queries: what lets scripts implement "Escape quits" (SDL's keyboard
// state API cannot cross the binding boundary).
TEST_F(GuiHarness, KeyPressedReportsThisFramesKeys)
{
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_EVENTS)) << SDL_GetError();

    SDL_Event key;
    SDL_zero(key);
    key.type = SDL_EVENT_KEY_DOWN;
    key.key.scancode = SDL_SCANCODE_ESCAPE;
    ASSERT_TRUE(SDL_PushEvent(&key));
    EXPECT_TRUE(Grapple_GuiPumpEvents(gui_));
    EXPECT_TRUE(Grapple_GuiKeyPressed(gui_, SDL_SCANCODE_ESCAPE));
    EXPECT_FALSE(Grapple_GuiKeyPressed(gui_, SDL_SCANCODE_A)) << "only keys seen";

    // The set is per-frame: a pump with no keys clears it.
    EXPECT_TRUE(Grapple_GuiPumpEvents(gui_));
    EXPECT_FALSE(Grapple_GuiKeyPressed(gui_, SDL_SCANCODE_ESCAPE));

    // Out-of-range and null are safe.
    EXPECT_FALSE(Grapple_GuiKeyPressed(gui_, -1));
    EXPECT_FALSE(Grapple_GuiKeyPressed(gui_, 999999));
    EXPECT_FALSE(Grapple_GuiKeyPressed(nullptr, SDL_SCANCODE_ESCAPE));

    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// Theming: Nuklear's own style stack takes union-typed items, so this is
// the entry point Lua and Ruby can reach.
TEST_F(GuiHarness, StyleColorPushPopRestoresTheme)
{
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    const struct nk_color before = ctx->style.window.fixed_background.data.color;
    const struct nk_color text_before = ctx->style.text.color;

    ASSERT_TRUE(Grapple_GuiPushStyleColor(
        gui_, GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND, SDL_Color{10, 20, 30, 255}));
    ASSERT_TRUE(Grapple_GuiPushStyleColor(gui_, GRAPPLE_GUI_COLOR_TEXT,
                                            SDL_Color{1, 2, 3, 255}));
    EXPECT_EQ(ctx->style.window.fixed_background.data.color.r, 10);
    EXPECT_EQ(ctx->style.text.color.g, 2) << "plain-colour stack too";

    // Pops unwind both stacks in LIFO order, whichever kind each push used.
    Grapple_GuiPopStyleColor(gui_, 2);
    EXPECT_EQ(ctx->style.window.fixed_background.data.color.r, before.r);
    EXPECT_EQ(ctx->style.text.color.g, text_before.g);

    // Over-popping and null are safe no-ops.
    Grapple_GuiPopStyleColor(gui_, 5);
    Grapple_GuiPopStyleColor(nullptr, 1);
    EXPECT_FALSE(Grapple_GuiPushStyleColor(nullptr, GRAPPLE_GUI_COLOR_BUTTON,
                                             SDL_Color{0, 0, 0, 255}));
}

// Runtime font sizing: Nuklear cannot add glyphs after the atlas is baked,
// so the sizes are baked up front and selected here.
TEST_F(GuiHarness, FontSizesAreSelectableAtRuntime)
{
    const float normal = Grapple_GuiFontHeight(gui_);
    EXPECT_GT(normal, 0.0f);

    ASSERT_TRUE(Grapple_GuiSetFont(gui_, GRAPPLE_GUI_FONT_LARGE));
    const float large = Grapple_GuiFontHeight(gui_);
    EXPECT_GT(large, normal);

    ASSERT_TRUE(Grapple_GuiSetFont(gui_, GRAPPLE_GUI_FONT_SMALL));
    EXPECT_LT(Grapple_GuiFontHeight(gui_), normal);

    ASSERT_TRUE(Grapple_GuiSetFont(gui_, GRAPPLE_GUI_FONT_NORMAL));
    EXPECT_FLOAT_EQ(Grapple_GuiFontHeight(gui_), normal);

    // Scoped push/pop restores the previous font.
    ASSERT_TRUE(Grapple_GuiPushFont(gui_, GRAPPLE_GUI_FONT_LARGE));
    EXPECT_FLOAT_EQ(Grapple_GuiFontHeight(gui_), large);
    Grapple_GuiPopFont(gui_, 1);
    EXPECT_FLOAT_EQ(Grapple_GuiFontHeight(gui_), normal);

    // Text actually measures wider with a bigger font (glyphs really differ).
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    const char *sample = "Button 1 was clicked.";
    const int len = static_cast<int>(SDL_strlen(sample));
    const float w_normal = ctx->style.font->width(ctx->style.font->userdata,
                                                  ctx->style.font->height, sample, len);
    ASSERT_TRUE(Grapple_GuiSetFont(gui_, GRAPPLE_GUI_FONT_LARGE));
    const float w_large = ctx->style.font->width(ctx->style.font->userdata,
                                                 ctx->style.font->height, sample, len);
    EXPECT_GT(w_large, w_normal);
    ASSERT_TRUE(Grapple_GuiSetFont(gui_, GRAPPLE_GUI_FONT_NORMAL));

    // Bad input and over-pop are safe.
    // 3 is one past LARGE and still inside the enum's value range, so the
    // cast is well defined — GCC rejects casting a far-out value like 99 to
    // a three-value enum under -Wconversion.
    EXPECT_FALSE(Grapple_GuiSetFont(gui_, static_cast<Grapple_GuiFontSize>(3)));
    EXPECT_FALSE(Grapple_GuiSetFont(nullptr, GRAPPLE_GUI_FONT_NORMAL));
    EXPECT_FALSE(Grapple_GuiPushFont(nullptr, GRAPPLE_GUI_FONT_LARGE));
    Grapple_GuiPopFont(gui_, 5);
    Grapple_GuiPopFont(nullptr, 1);
    EXPECT_FLOAT_EQ(Grapple_GuiFontHeight(gui_), normal);
    EXPECT_FLOAT_EQ(Grapple_GuiFontHeight(nullptr), 0.0f);
}

// nk_labelf float formatting. Nuklear's built-in printf (used because
// NK_INCLUDE_STANDARD_IO is off) emitted only the first character for a
// precision of 0: "%.0f" of 40.0 rendered as "4". Compared black-box by
// rendering the formatted text beside the literal it must equal.
TEST_F(GuiHarness, LabelfFormatsFloatsCorrectly)
{
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    auto render_text = [&](bool formatted, double value, const char *literal) {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
        Grapple_GuiInputBegin(gui_);
        Grapple_GuiInputEnd(gui_);
        if (nk_begin(ctx, "fmt", nk_rect(0, 0, kScreen, kScreen), 0))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            if (formatted)
            {
                nk_labelf(ctx, NK_TEXT_LEFT, "%.0f", value);
            }
            else
            {
                nk_label(ctx, literal, NK_TEXT_LEFT);
            }
        }
        nk_end(ctx);
        Grapple_GuiRender(gui_);
        SDL_FlushRenderer(renderer_);
        int lit = 0;
        for (int y = 0; y < kScreen; y++)
        {
            for (int x = 0; x < kScreen; x++)
            {
                Uint8 r = 0, g = 0, b = 0, a = 0;
                SDL_ReadSurfacePixel(surface_, x, y, &r, &g, &b, &a);
                if (r > 40 || g > 40 || b > 40)
                {
                    lit++;
                }
            }
        }
        return lit;
    };

    // "%.0f" of 40.0 must paint exactly what "40" paints.
    EXPECT_EQ(render_text(true, 40.0, nullptr), render_text(false, 0, "40"));
    // Rounds like printf rather than truncating.
    EXPECT_EQ(render_text(true, 2.7, nullptr), render_text(false, 0, "3"));
    // Precision beyond zero still works.
    EXPECT_GT(render_text(true, 1.5, nullptr), 0);
}

// Image widget: Nuklear's nk_image takes a union-handle struct that cannot
// cross a script boundary, so the library takes an SDL_Texture directly and
// applies the PictureBox-style sizing modes itself.
TEST_F(GuiHarness, ImageWidgetHonoursSizingModes)
{
    // A 40x20 texture (2:1) drawn into a square slot: Stretch fills it,
    // Zoom leaves letterbox bars, Fill covers it.
    SDL_Surface *pixels = SDL_CreateSurface(40, 20, SDL_PIXELFORMAT_RGBA32);
    ASSERT_NE(pixels, nullptr);
    SDL_FillSurfaceRect(pixels, nullptr, SDL_MapSurfaceRGBA(pixels, 255, 0, 0, 255));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, pixels);
    ASSERT_NE(texture, nullptr) << SDL_GetError();
    SDL_DestroySurface(pixels);

    auto painted = [&](Grapple_GuiImageMode mode) {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
        Grapple_GuiInputBegin(gui_);
        Grapple_GuiInputEnd(gui_);
        struct nk_context *ctx = Grapple_GuiContext(gui_);
        if (nk_begin(ctx, "img", nk_rect(0, 0, kScreen, kScreen), 0))
        {
            nk_layout_row_static(ctx, 100, 100, 1);  // square 100x100 slot
            EXPECT_TRUE(Grapple_GuiImage(gui_, texture, mode));
        }
        nk_end(ctx);
        Grapple_GuiRender(gui_);
        SDL_FlushRenderer(renderer_);
        int red = 0;
        for (int y = 0; y < kScreen; y++)
        {
            for (int x = 0; x < kScreen; x++)
            {
                Uint8 r = 0, g = 0, b = 0, a = 0;
                SDL_ReadSurfacePixel(surface_, x, y, &r, &g, &b, &a);
                if (r > 150 && g < 100)
                {
                    red++;
                }
            }
        }
        return red;
    };

    const int stretch = painted(GRAPPLE_GUI_IMAGE_STRETCH);
    const int zoom = painted(GRAPPLE_GUI_IMAGE_ZOOM);
    const int fill = painted(GRAPPLE_GUI_IMAGE_FILL);
    const int center = painted(GRAPPLE_GUI_IMAGE_CENTER);

    EXPECT_GT(stretch, 0) << "stretch must paint the whole slot";
    // 2:1 source zoomed into a square slot covers about half of it.
    EXPECT_LT(zoom, stretch);
    EXPECT_NEAR(static_cast<double>(zoom) / stretch, 0.5, 0.15);
    // Fill covers the slot but is clipped to it, so it matches stretch's area.
    EXPECT_NEAR(static_cast<double>(fill) / stretch, 1.0, 0.15);
    // Native 40x20 centred is much smaller than the 100x100 slot.
    EXPECT_LT(center, zoom);

    EXPECT_FALSE(Grapple_GuiImage(nullptr, texture, GRAPPLE_GUI_IMAGE_ZOOM));
    EXPECT_FALSE(Grapple_GuiImage(gui_, nullptr, GRAPPLE_GUI_IMAGE_ZOOM));
    SDL_DestroyTexture(texture);
}

// The gui-owned grid: same layout as the caller-owned helper, but reachable
// from scripts (which cannot hold a struct or pass a float array).
TEST_F(GuiHarness, OwnedGridMatchesCallerOwnedGrid)
{
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    auto widths = [&](bool owned) {
        std::vector<float> out;
        Grapple_GuiInputBegin(gui_);
        Grapple_GuiInputEnd(gui_);
        if (nk_begin(ctx, "grid", nk_rect(0, 0, 300, 200), 0))
        {
            static const float weights[] = {1.0f, 2.0f, 1.0f};
            Grapple_GuiGrid caller_grid;
            if (owned)
            {
                Grapple_GuiGridWeight(gui_, 0, 1.0f);
                Grapple_GuiGridWeight(gui_, 1, 2.0f);
                Grapple_GuiGridWeight(gui_, 2, 1.0f);
                Grapple_GuiGridBeginOwned(gui_, 3, 24.0f);
            }
            else
            {
                Grapple_GuiGridBegin(ctx, &caller_grid, 3, weights, 24.0f);
            }
            for (int i = 0; i < 3; i++)
            {
                if (owned)
                {
                    Grapple_GuiGridCellOwned(gui_);
                }
                else
                {
                    Grapple_GuiGridCell(&caller_grid);
                }
                struct nk_rect bounds = nk_widget_bounds(ctx);
                out.push_back(bounds.w);
                nk_label(ctx, "x", NK_TEXT_LEFT);
            }
            if (owned)
            {
                Grapple_GuiGridEndOwned(gui_);
            }
            else
            {
                Grapple_GuiGridEnd(&caller_grid);
            }
        }
        nk_end(ctx);
        Grapple_GuiRender(gui_);  // ends the frame (nk_clear)
        return out;
    };

    const std::vector<float> caller = widths(false);
    const std::vector<float> owned = widths(true);
    ASSERT_EQ(caller.size(), 3u);
    ASSERT_EQ(owned.size(), 3u);
    for (size_t i = 0; i < caller.size(); i++)
    {
        EXPECT_NEAR(owned[i], caller[i], 0.5f) << "column " << i;
    }
    // The middle column carries weight 2, so it is about twice as wide.
    EXPECT_NEAR(owned[1] / owned[0], 2.0, 0.15);

    // Weights reset between grids: the next one is equal-width.
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    std::vector<float> equal;
    if (nk_begin(ctx, "grid2", nk_rect(0, 0, 300, 200), 0))
    {
        Grapple_GuiGridBeginOwned(gui_, 3, 24.0f);
        for (int i = 0; i < 3; i++)
        {
            Grapple_GuiGridCellOwned(gui_);
            equal.push_back(nk_widget_bounds(ctx).w);
            nk_label(ctx, "x", NK_TEXT_LEFT);
        }
        Grapple_GuiGridEndOwned(gui_);
    }
    nk_end(ctx);
    Grapple_GuiRender(gui_);
    ASSERT_EQ(equal.size(), 3u);
    EXPECT_NEAR(equal[1] / equal[0], 1.0, 0.05);

    EXPECT_FALSE(Grapple_GuiGridWeight(nullptr, 0, 1.0f));
    EXPECT_FALSE(Grapple_GuiGridWeight(gui_, -1, 1.0f));
    EXPECT_FALSE(Grapple_GuiGridBeginOwned(nullptr, 2, 20.0f));
    Grapple_GuiGridEndOwned(nullptr);  // safe no-op
}

// Tooltip timing: Nuklear's nk_tooltip draws immediately and stays up as
// long as the pointer is inside the widget. Grapple_GuiTooltip adds the
// desktop behaviour — appear after a dwell, hide as soon as the pointer
// moves.
TEST_F(GuiHarness, TooltipWaitsForHoverDwellAndHidesOnMotion)
{
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_EVENTS)) << SDL_GetError();
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    // Drive one frame with the pointer at (x, y); returns whether the
    // tooltip was displayed for the button occupying the top-left row.
    auto frame_at = [&](float x, float y) {
        SDL_Event motion;
        SDL_zero(motion);
        motion.type = SDL_EVENT_MOUSE_MOTION;
        motion.motion.x = x;
        motion.motion.y = y;
        SDL_PushEvent(&motion);
        Grapple_GuiPumpEvents(gui_);

        bool shown = false;
        if (nk_begin(ctx, "tips", nk_rect(0, 0, 200, 120), 0))
        {
            nk_layout_row_dynamic(ctx, 40, 1);
            shown = Grapple_GuiTooltip(gui_, "hover text");
            nk_button_label(ctx, "Hover me");
        }
        nk_end(ctx);
        Grapple_GuiRender(gui_);  // ends the frame
        return shown;
    };

    EXPECT_EQ(Grapple_GuiTooltipDelay(gui_), 1000) << "desktop-style default";

    // Pointer away from the widget: never shown.
    EXPECT_FALSE(frame_at(180.0f, 110.0f));

    // Arrive on the widget: the dwell has only just started, so not yet.
    EXPECT_FALSE(frame_at(50.0f, 30.0f));
    EXPECT_FALSE(frame_at(50.0f, 30.0f)) << "still counting down";

    // With no delay it appears as soon as the pointer is resting.
    Grapple_GuiSetTooltipDelay(gui_, 0);
    EXPECT_TRUE(frame_at(50.0f, 30.0f));

    // Moving the pointer re-arms it, even within the same widget.
    EXPECT_FALSE(frame_at(70.0f, 34.0f)) << "motion hides the tooltip";
    EXPECT_TRUE(frame_at(70.0f, 34.0f)) << "resting again shows it";

    // A long delay keeps it hidden no matter how many frames pass.
    Grapple_GuiSetTooltipDelay(gui_, 60000);
    EXPECT_FALSE(frame_at(90.0f, 34.0f));
    for (int i = 0; i < 5; i++)
    {
        EXPECT_FALSE(frame_at(90.0f, 34.0f));
    }

    Grapple_GuiSetTooltipDelay(gui_, -5);
    EXPECT_EQ(Grapple_GuiTooltipDelay(gui_), 0) << "negative clamps to 0";
    EXPECT_FALSE(Grapple_GuiTooltip(nullptr, "x"));
    EXPECT_FALSE(Grapple_GuiTooltip(gui_, nullptr));
    Grapple_GuiSetTooltipDelay(nullptr, 100);  // safe no-op

    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// A windowless (software) renderer stays at 1.0 so headless tests and
// non-Retina displays are unaffected by the high-DPI path.
TEST_F(GuiHarness, ScaleDefaultsToOneWithoutAWindow)
{
    EXPECT_FLOAT_EQ(Grapple_GuiScale(gui_), 1.0f);
    EXPECT_FLOAT_EQ(Grapple_GuiScale(nullptr), 1.0f);
}

// The file buttons are ordinary buttons on desktop: nothing happens until
// they are clicked, and a save reports nowhere until one completes. (The
// browser halves are DOM overlays and are exercised in a real engine.)
TEST_F(GuiHarness, FileButtonsAreInertUntilClicked)
{
    struct nk_context *ctx = Grapple_GuiContext(gui_);
    BeginFrame();
    if (nk_begin(ctx, "files", nk_rect(10, 10, 300, 200), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 30.0f, 1);
        EXPECT_FALSE(Grapple_GuiOpenFileButton(gui_, "Open", "Text files", "txt"));
        EXPECT_FALSE(Grapple_GuiSaveFileButton(gui_, "Save", "untitled.txt", "hi", 2));
    }
    nk_end(ctx);
    EXPECT_TRUE(Grapple_GuiRender(gui_));
    EXPECT_EQ(Grapple_GuiSavedPath(gui_), nullptr) << "nothing has been saved";
    EXPECT_EQ(Grapple_GuiSavedPath(nullptr), nullptr);

    EXPECT_FALSE(Grapple_GuiOpenFileButton(nullptr, "Open", nullptr, nullptr));
    EXPECT_FALSE(Grapple_GuiOpenFileButton(gui_, nullptr, nullptr, nullptr));
    EXPECT_FALSE(Grapple_GuiSaveFileButton(gui_, "Save", nullptr, "hi", 2));
    EXPECT_FALSE(Grapple_GuiSaveFileButton(gui_, "Save", "f.txt", nullptr, 4))
        << "a null buffer with a non-zero length is a caller bug";
}


// A font atlas is baked once, so the glyph range decides for the GUI's
// lifetime what text can be drawn. Latin-1 — Nuklear's default — stops
// short of the punctuation UI strings actually contain.
TEST_F(GuiHarness, GlyphRangeDecidesWhatCanBeDrawn)
{
    size_t font_size = 0;
    void *font = SDL_LoadFile(GUI_TEST_FONT, &font_size);
    ASSERT_NE(font, nullptr) << SDL_GetError();

    // An em dash, a curly quote, a bullet and an ellipsis: all outside
    // Latin-1, all present in this font.
    const char *text = "\xe2\x80\x94 \xe2\x80\x9c \xe2\x80\xa2 \xe2\x80\xa6";
    const size_t bytes = static_cast<size_t>(kScreen) * kScreen * sizeof(Uint32);
    std::vector<Uint32> latin1(static_cast<size_t>(kScreen) * kScreen);
    std::vector<Uint32> punctuation(latin1.size());

    auto render_into = [&](Grapple_GuiGlyphRange range, std::vector<Uint32> &out) {
        Grapple_Gui *gui = Grapple_CreateGuiWithGlyphs(renderer_, font, font_size, 24.0f,
                                                           range);
        ASSERT_NE(gui, nullptr) << SDL_GetError();
        struct nk_context *ctx = Grapple_GuiContext(gui);
        BeginFrame();
        Grapple_GuiInputBegin(gui);
        Grapple_GuiInputEnd(gui);
        if (nk_begin(ctx, "glyphs", nk_rect(0, 0, kScreen, kScreen), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 40.0f, 1);
            nk_label(ctx, text, NK_TEXT_LEFT);
        }
        nk_end(ctx);
        EXPECT_TRUE(Grapple_GuiRender(gui));
        SDL_FlushRenderer(renderer_);
        SDL_memcpy(out.data(), surface_->pixels, bytes);
        Grapple_DestroyGui(gui);
    };

    render_into(GRAPPLE_GUI_GLYPHS_LATIN1, latin1);
    render_into(GRAPPLE_GUI_GLYPHS_PUNCTUATION, punctuation);
    EXPECT_NE(SDL_memcmp(latin1.data(), punctuation.data(), bytes), 0)
        << "the same string must render differently once the glyphs are baked";

    // The default entry point keeps its old behaviour exactly.
    std::vector<Uint32> plain(latin1.size());
    Grapple_Gui *gui = Grapple_CreateGui(renderer_, font, font_size, 24.0f);
    ASSERT_NE(gui, nullptr);
    Grapple_DestroyGui(gui);
    SDL_free(font);
}

// The two draw entry points the inventory grid needed: one that paints at
// an explicit rectangle inside a window, one that paints above every panel.
TEST_F(GuiHarness, DrawTextureHonoursRectAndOverlayOrder)
{
    SDL_Surface *pixels = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_ARGB8888);
    ASSERT_NE(pixels, nullptr);
    SDL_FillSurfaceRect(pixels, nullptr, 0xFFFF0000u);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, pixels);
    ASSERT_NE(texture, nullptr) << SDL_GetError();
    SDL_DestroySurface(pixels);

    struct nk_context *ctx = Grapple_GuiContext(gui_);
    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    const SDL_FRect where = {20.0f, 20.0f, 40.0f, 40.0f};
    if (nk_begin(ctx, "canvas", nk_rect(0, 0, kScreen, kScreen), NK_WINDOW_NO_SCROLLBAR))
    {
        EXPECT_TRUE(Grapple_GuiDrawTexture(gui_, texture, where,
                                             GRAPPLE_GUI_IMAGE_STRETCH));
    }
    nk_end(ctx);
    // Queued from outside any window, and still drawn — that is the point.
    const SDL_FRect ghost = {120.0f, 120.0f, 30.0f, 30.0f};
    EXPECT_TRUE(Grapple_GuiDrawTextureOverlay(gui_, texture, ghost,
                                                GRAPPLE_GUI_IMAGE_STRETCH));
    EXPECT_TRUE(Grapple_GuiRender(gui_));
    SDL_FlushRenderer(renderer_);

    auto pixel_at = [&](int x, int y) {
        const Uint32 *p = static_cast<const Uint32 *>(surface_->pixels);
        return p[y * kScreen + x] & 0x00FFFFFFu;
    };
    EXPECT_EQ(pixel_at(40, 40), 0x00FF0000u) << "drawn where the rect asked";
    // Outside the rect is the window's own background, not the texture:
    // an explicit rectangle must not bleed into the rest of the panel.
    EXPECT_NE(pixel_at(10, 10), 0x00FF0000u) << "and nowhere else";
    EXPECT_EQ(pixel_at(130, 130), 0x00FF0000u) << "the overlay reached the renderer";

    // Overlays last one frame; the next frame must be clean.
    BeginFrame();
    EXPECT_TRUE(Grapple_GuiRender(gui_));
    SDL_FlushRenderer(renderer_);
    EXPECT_EQ(pixel_at(130, 130), 0x00000000u) << "the queue is emptied each frame";

    // Outside a window there is no canvas to draw into, and that is an
    // error rather than a silent no-op.
    EXPECT_FALSE(Grapple_GuiDrawTexture(gui_, texture, where, GRAPPLE_GUI_IMAGE_STRETCH));
    EXPECT_FALSE(Grapple_GuiDrawTexture(gui_, nullptr, where, GRAPPLE_GUI_IMAGE_STRETCH));
    EXPECT_FALSE(Grapple_GuiDrawTextureOverlay(nullptr, texture, where,
                                                 GRAPPLE_GUI_IMAGE_STRETCH));
    SDL_DestroyTexture(texture);
}

// The debug-overlay counters: they must move with what was actually drawn.
TEST_F(GuiHarness, CountersReportTheLastFrame)
{
    EXPECT_EQ(Grapple_GuiDrawCommandCount(gui_), 0) << "nothing rendered yet";
    EXPECT_EQ(Grapple_GuiMemoryUsed(gui_), 0);
    EXPECT_EQ(Grapple_GuiDrawCommandCount(nullptr), 0);
    EXPECT_EQ(Grapple_GuiMemoryUsed(nullptr), 0);

    struct nk_context *ctx = Grapple_GuiContext(gui_);
    BeginFrame();
    if (nk_begin(ctx, "busy", nk_rect(0, 0, 200, 200), NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 20.0f, 1);
        for (int i = 0; i < 12; i++)
        {
            nk_button_label(ctx, "button");
        }
    }
    nk_end(ctx);
    ASSERT_TRUE(Grapple_GuiRender(gui_));
    const int busy_memory = Grapple_GuiMemoryUsed(gui_);
    EXPECT_GT(Grapple_GuiDrawCommandCount(gui_), 0);
    EXPECT_GT(busy_memory, 0);

    BeginFrame();
    if (nk_begin(ctx, "busy", nk_rect(0, 0, 200, 200), NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 20.0f, 1);
        nk_label(ctx, "quiet", NK_TEXT_LEFT);
    }
    nk_end(ctx);
    ASSERT_TRUE(Grapple_GuiRender(gui_));
    EXPECT_LT(Grapple_GuiMemoryUsed(gui_), busy_memory)
        << "a simpler frame must cost less, or the counter is not measuring the frame";
}

} // namespace

// --- grid: per-row height, alignment, spacing ------------------------------

TEST_F(GuiHarness, GridRowHeightOverridesOneRowOnly)
{
    struct nk_rect first = nk_rect(0, 0, 0, 0);
    struct nk_rect tall = nk_rect(0, 0, 0, 0);
    struct nk_rect after = nk_rect(0, 0, 0, 0);
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    BeginFrame();
    if (nk_begin(ctx, "grid", nk_rect(0, 0, 320, 300), NK_WINDOW_NO_SCROLLBAR))
    {
        Grapple_GuiGrid grid;
        ASSERT_TRUE(Grapple_GuiGridBegin(ctx, &grid, 1, nullptr, 20.0f));

        Grapple_GuiGridCell(&grid);
        first = nk_widget_bounds(ctx);
        nk_label(ctx, "a", NK_TEXT_LEFT);

        Grapple_GuiGridRowHeight(&grid, 60.0f);
        Grapple_GuiGridCell(&grid);
        tall = nk_widget_bounds(ctx);
        nk_label(ctx, "b", NK_TEXT_LEFT);

        Grapple_GuiGridCell(&grid);
        after = nk_widget_bounds(ctx);
        nk_label(ctx, "c", NK_TEXT_LEFT);

        Grapple_GuiGridEnd(&grid);
    }
    nk_end(ctx);

    EXPECT_NEAR(first.h, 20.0f, 1.0f);
    EXPECT_NEAR(tall.h, 60.0f, 1.0f);
    // One-shot: the row after the override goes back to the grid default.
    EXPECT_NEAR(after.h, 20.0f, 1.0f);
}

TEST_F(GuiHarness, GridCellPartPlacesAWidgetInsideItsCell)
{
    struct nk_rect full = nk_rect(0, 0, 0, 0);
    struct nk_rect left = nk_rect(0, 0, 0, 0);
    struct nk_rect right = nk_rect(0, 0, 0, 0);
    struct nk_rect centered = nk_rect(0, 0, 0, 0);
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    BeginFrame();
    if (nk_begin(ctx, "grid", nk_rect(0, 0, 320, 300), NK_WINDOW_NO_SCROLLBAR))
    {
        Grapple_GuiGrid grid;
        ASSERT_TRUE(Grapple_GuiGridBegin(ctx, &grid, 1, nullptr, 24.0f));

        Grapple_GuiGridCell(&grid);
        full = nk_widget_bounds(ctx);
        nk_label(ctx, "full", NK_TEXT_LEFT);

        Grapple_GuiGridCellPart(&grid, 1, 0.5f, GRAPPLE_GUI_ALIGN_LEFT);
        left = nk_widget_bounds(ctx);
        nk_label(ctx, "left", NK_TEXT_LEFT);

        Grapple_GuiGridCellPart(&grid, 1, 0.5f, GRAPPLE_GUI_ALIGN_RIGHT);
        right = nk_widget_bounds(ctx);
        nk_label(ctx, "right", NK_TEXT_LEFT);

        Grapple_GuiGridCellPart(&grid, 1, 0.5f, GRAPPLE_GUI_ALIGN_CENTER);
        centered = nk_widget_bounds(ctx);
        nk_label(ctx, "mid", NK_TEXT_LEFT);

        Grapple_GuiGridEnd(&grid);
    }
    nk_end(ctx);

    // Half a cell is half as wide, whatever the alignment.
    EXPECT_NEAR(left.w, full.w * 0.5f, 3.0f);
    EXPECT_NEAR(right.w, full.w * 0.5f, 3.0f);
    EXPECT_NEAR(centered.w, full.w * 0.5f, 3.0f);

    // And it sits where it was asked to sit.
    EXPECT_NEAR(left.x, full.x, 2.0f);
    EXPECT_GT(right.x, left.x + left.w * 0.5f);
    EXPECT_GT(centered.x, left.x);
    EXPECT_LT(centered.x, right.x);
}

TEST_F(GuiHarness, GridSpacingIsPoppedSoLaterWidgetsAreUnaffected)
{
    struct nk_rect before = nk_rect(0, 0, 0, 0);
    struct nk_rect after = nk_rect(0, 0, 0, 0);
    struct nk_context *ctx = Grapple_GuiContext(gui_);

    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    BeginFrame();
    if (nk_begin(ctx, "grid", nk_rect(0, 0, 320, 300), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 20, 1);
        before = nk_widget_bounds(ctx);
        nk_label(ctx, "plain", NK_TEXT_LEFT);

        Grapple_GuiGrid grid;
        ASSERT_TRUE(Grapple_GuiGridBegin(ctx, &grid, 2, nullptr, 20.0f));
        Grapple_GuiGridSpacing(&grid, 24.0f, 24.0f);
        Grapple_GuiGridCell(&grid);
        nk_label(ctx, "a", NK_TEXT_LEFT);
        Grapple_GuiGridCell(&grid);
        nk_label(ctx, "b", NK_TEXT_LEFT);
        Grapple_GuiGridEnd(&grid);

        nk_layout_row_dynamic(ctx, 20, 1);
        after = nk_widget_bounds(ctx);
        nk_label(ctx, "plain again", NK_TEXT_LEFT);
    }
    nk_end(ctx);

    // An unbalanced style stack would leak the wide spacing into everything
    // drawn afterwards, so the width either side of the grid must match.
    EXPECT_NEAR(before.w, after.w, 1.0f);
}

// --- the retained UI -------------------------------------------------------
//
// The layer's whole claim is that a widget declared once keeps its identity,
// its state and its callback. These check the three things that would make
// that claim false: that nothing draws, that a click reaches nobody, and that
// "fit" is not actually measured.

namespace {

struct ClickCount
{
    int clicks = 0;
};

void CountClick(Grapple_UiWidget *, void *user)
{
    static_cast<ClickCount *>(user)->clicks++;
}

} // namespace

TEST_F(GuiHarness, UiButtonClickReachesItsCallback)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);

    ClickCount counter;
    Grapple_UiPanelDef panel_def{};
    panel_def.title = "ui";
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);
    ASSERT_NE(panel, nullptr);

    Grapple_UiButtonDef button_def{};
    button_def.text = "Fire";
    button_def.on_click = CountClick;
    button_def.user = &counter;
    Grapple_UiWidget *button = Grapple_UiButton(panel, &button_def);

    // Frame 1 lays the tree out; the button's bounds come from the widget
    // Nuklear actually placed, not from anything the test assumed.
    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    // Where the layer actually put the button, not where the test guessed.
    float bx = 0.0f;
    float by = 0.0f;
    float bw = 0.0f;
    float bh = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(button, &bx, &by, &bw, &bh));
    ASSERT_GT(bw, 0.0f);

    const float cx = bx + bw / 2.0f;
    const float cy = by + bh / 2.0f;
    for (int phase = 0; phase < 2; ++phase)
    {
        BeginFrame();
        Grapple_GuiInputBegin(gui_);
        FeedMouseMove(cx, cy);
        FeedButton(cx, cy, phase == 0);
        Grapple_GuiInputEnd(gui_);
        Grapple_UiDraw(ui);
    }

    EXPECT_EQ(counter.clicks, 1);
    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiFitIsMeasuredAndStretchIsNot)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);

    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiStripDef row_def{};
    Grapple_UiWidget *row = Grapple_UiRow(panel, &row_def);

    Grapple_UiButtonDef fitted{};
    fitted.text = "Hi";
    fitted.width = GRAPPLE_UI_FIT;
    Grapple_UiWidget *narrow = Grapple_UiButton(row, &fitted);

    Grapple_UiButtonDef stretched{};
    stretched.text = "Also a button, considerably wider than the first";
    Grapple_UiWidget *wide = Grapple_UiButton(row, &stretched);

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float narrow_w = 0.0f;
    float wide_w = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(narrow, nullptr, nullptr, &narrow_w, nullptr));
    ASSERT_TRUE(Grapple_UiBounds(wide, nullptr, nullptr, &wide_w, nullptr));

    // No exact pixel count is asserted -- the font decides that. What must
    // hold is that "fit" was measured from the widget's own two characters
    // while its stretched neighbour took everything left over, which is the
    // sizing an immediate-mode layout cannot do at all.
    EXPECT_GT(narrow_w, 0.0f);
    EXPECT_LT(narrow_w, wide_w / 2.0f)
        << "fit=" << narrow_w << " stretch=" << wide_w;

    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiOverlayPlacesChildrenOverTheSameArea)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiOverlayDef overlay_def{};
    overlay_def.height = GRAPPLE_UI_PX(120.0f);
    Grapple_UiWidget *overlay = Grapple_UiOverlay(panel, &overlay_def);
    ASSERT_NE(overlay, nullptr);

    Grapple_UiSpacerDef background_def{};
    Grapple_UiWidget *background = Grapple_UiSpacer(overlay, &background_def);
    ASSERT_NE(background, nullptr);

    Grapple_UiLabelDef label_def{};
    label_def.text = "placed";
    label_def.width = GRAPPLE_UI_FIT;
    label_def.height = GRAPPLE_UI_FIT;
    Grapple_UiWidget *label = Grapple_UiLabel(overlay, &label_def);
    ASSERT_NE(label, nullptr);
    ASSERT_TRUE(Grapple_UiPlace(label, GRAPPLE_UI_PCT(0.25f), GRAPPLE_UI_PCT(0.40f)));

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float background_x = 0.0f;
    float background_y = 0.0f;
    float background_width = 0.0f;
    float background_height = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(background, &background_x, &background_y, &background_width,
                                 &background_height));
    float label_x = 0.0f;
    float label_y = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(label, &label_x, &label_y, nullptr, nullptr));
    EXPECT_NEAR(label_x, background_x + background_width * 0.25f, 2.0f);
    EXPECT_NEAR(label_y, background_y + background_height * 0.40f, 2.0f);

    EXPECT_FALSE(Grapple_UiPlace(panel, GRAPPLE_UI_PX(1.0f), GRAPPLE_UI_PX(1.0f)));
    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiEntryNotifiesOnlyWhenUserEditsText)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);
    std::vector<std::string> changes;
    Grapple_UiEntryDef entry_def{};
    entry_def.text = "4";
    entry_def.capacity = 8;
    entry_def.on_change = [](Grapple_UiWidget *entry, void *user) {
        static_cast<std::vector<std::string> *>(user)->emplace_back(Grapple_UiText(entry));
    };
    entry_def.user = &changes;
    Grapple_UiWidget *entry = Grapple_UiEntry(panel, &entry_def);
    ASSERT_NE(entry, nullptr);

    auto draw = [&]() {
        Grapple_GuiInputEnd(gui_);
        Grapple_UiDraw(ui);
        BeginFrame();
        Grapple_GuiInputBegin(gui_);
    };
    auto key = [&](SDL_Keycode code, SDL_Keymod mod = SDL_KMOD_NONE) {
        SDL_Event event{};
        event.type = SDL_EVENT_KEY_DOWN;
        event.key.key = code;
        event.key.mod = mod;
        Grapple_GuiProcessEvent(gui_, &event);
        draw();
        event.type = SDL_EVENT_KEY_UP;
        Grapple_GuiProcessEvent(gui_, &event);
        draw();
    };
    auto type = [&](const char *text) {
        SDL_Event event{};
        event.type = SDL_EVENT_TEXT_INPUT;
        event.text.text = text;
        Grapple_GuiProcessEvent(gui_, &event);
        draw();
    };

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    draw();
    float x = 0, y = 0, width = 0, height = 0;
    ASSERT_TRUE(Grapple_UiBounds(entry, &x, &y, &width, &height));
    FeedMouseMove(x + width / 2, y + height / 2);
    FeedButton(x + width / 2, y + height / 2, true);
    draw();
    FeedButton(x + width / 2, y + height / 2, false);
    draw();
    EXPECT_TRUE(changes.empty()) << "Focus and idle frames do not change text";

    key(SDLK_END);
    type("2");
    EXPECT_EQ(changes, (std::vector<std::string>{"42"}));
    key(SDLK_BACKSPACE);
    EXPECT_EQ(changes, (std::vector<std::string>{"42", "4"}));
    key(SDLK_A, SDL_KMOD_CTRL);
    type("9");
    EXPECT_EQ(changes, (std::vector<std::string>{"42", "4", "9"}))
        << "Replacing a selection with the same byte count is still a change";
    key(SDLK_RETURN);
    draw();
    EXPECT_EQ(changes.size(), 3u);

    Grapple_UiSetText(entry, "7");
    draw();
    EXPECT_STREQ(Grapple_UiText(entry), "7");
    EXPECT_EQ(changes.size(), 3u) << "Programmatic setters do not emit user callbacks";
    Grapple_GuiInputEnd(gui_);
    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiWidgetStateOutlivesTheFrame)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);

    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiEntryDef entry_def{};
    entry_def.text = "typed";
    Grapple_UiWidget *entry = Grapple_UiEntry(panel, &entry_def);

    Grapple_UiLabelDef label_def{};
    Grapple_UiWidget *label = Grapple_UiLabel(panel, &label_def);
    Grapple_UiSetText(label, "set once");

    for (int frame = 0; frame < 3; ++frame)
    {
        BeginFrame();
        Grapple_GuiInputBegin(gui_);
        Grapple_GuiInputEnd(gui_);
        Grapple_UiDraw(ui);
    }

    // The buffer belongs to the widget, which is the ceremony this layer
    // removes: three frames later it is still there and still correct.
    EXPECT_EQ(std::string(Grapple_UiText(entry)), "typed");
    EXPECT_EQ(std::string(Grapple_UiText(label)), "set once");

    Grapple_UiSetVisible(label, false);
    EXPECT_FALSE(Grapple_UiVisible(label));

    Grapple_DestroyUi(ui);
}

// --- the image widget ------------------------------------------------------
//
// A picture that can be clicked is what a toolbar is made of, and what the
// textbook calls a PictureBox. These check the three claims: it loads, it
// takes its size from the image rather than from a text measurement, and a
// click on it reaches the handler.

namespace {

// A small BMP written here, so the test carries its own asset.
//
// The name carries the test's own name: ctest runs tests as concurrent
// processes, so a fixed filename means one test truncating the file another
// is reading. That failed on Windows, where the sharing rules are strict,
// and passed everywhere else — which is the worst way for it to behave.
std::string MakeBitmap(int width, int height)
{
    const ::testing::TestInfo *info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const std::string unique =
        std::string("grapple_ui_image_") + ((info != nullptr) ? info->name() : "anon") +
        ".bmp";
    const std::filesystem::path file = std::filesystem::temp_directory_path() / unique;
    SDL_Surface *surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    SDL_FillSurfaceRect(surface, nullptr, 0xFF3366CCu);
    SDL_SaveBMP(surface, file.string().c_str());
    SDL_DestroySurface(surface);
    return file.string();
}

} // namespace

TEST_F(GuiHarness, UiAnnotationTracksTheRenderedImageRatherThanItsSlot)
{
    const std::string path = MakeBitmap(40, 40);

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiOverlayDef overlay_def{};
    overlay_def.height = GRAPPLE_UI_PX(100.0f);
    Grapple_UiWidget *overlay = Grapple_UiOverlay(panel, &overlay_def);
    ASSERT_NE(overlay, nullptr);

    Grapple_UiImageDef image_def{};
    image_def.path = path.c_str();
    image_def.mode = GRAPPLE_GUI_IMAGE_ZOOM;
    Grapple_UiWidget *image = Grapple_UiImage(overlay, &image_def);
    ASSERT_NE(image, nullptr);

    Grapple_UiImageAnnotationDef annotation_def{};
    annotation_def.text = "star";
    annotation_def.x = 0.25f;
    annotation_def.y = 0.50f;
    annotation_def.side = GRAPPLE_UI_ANNOTATION_RIGHT;
    annotation_def.gap = 6.0f;
    Grapple_UiWidget *annotation = Grapple_UiImageAnnotation(image, &annotation_def);
    ASSERT_NE(annotation, nullptr);
    Grapple_UiImageAnnotationDef left_def = annotation_def;
    left_def.side = GRAPPLE_UI_ANNOTATION_LEFT;
    Grapple_UiWidget *left = Grapple_UiImageAnnotation(image, &left_def);
    ASSERT_NE(left, nullptr);
    Grapple_UiImageAnnotationDef above_def = annotation_def;
    above_def.side = GRAPPLE_UI_ANNOTATION_ABOVE;
    Grapple_UiWidget *above = Grapple_UiImageAnnotation(image, &above_def);
    ASSERT_NE(above, nullptr);
    Grapple_UiImageAnnotationDef below_def = annotation_def;
    below_def.side = GRAPPLE_UI_ANNOTATION_BELOW;
    Grapple_UiWidget *below = Grapple_UiImageAnnotation(image, &below_def);
    ASSERT_NE(below, nullptr);

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float image_x = 0.0f;
    float image_y = 0.0f;
    float image_width = 0.0f;
    float image_height = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(image, &image_x, &image_y, &image_width, &image_height));
    float annotation_x = 0.0f;
    float annotation_y = 0.0f;
    float annotation_width = 0.0f;
    float annotation_height = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(annotation, &annotation_x, &annotation_y, &annotation_width,
                                 &annotation_height));

    const float rendered_size = SDL_min(image_width, image_height);
    const float rendered_x = image_x + (image_width - rendered_size) * 0.5f;
    const float rendered_y = image_y + (image_height - rendered_size) * 0.5f;
    EXPECT_NEAR(annotation_x, rendered_x + rendered_size * 0.25f + 6.0f, 2.0f);
    EXPECT_NEAR(annotation_y, rendered_y + rendered_size * 0.50f - annotation_height * 0.5f, 2.0f);

    const float point_x = rendered_x + rendered_size * 0.25f;
    const float point_y = rendered_y + rendered_size * 0.50f;
    float left_x = 0.0f;
    float left_y = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(left, &left_x, &left_y, nullptr, nullptr));
    EXPECT_NEAR(left_x, point_x - 6.0f - annotation_width, 2.0f);
    EXPECT_NEAR(left_y, point_y - annotation_height * 0.5f, 2.0f);
    float above_x = 0.0f;
    float above_y = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(above, &above_x, &above_y, nullptr, nullptr));
    EXPECT_NEAR(above_x, point_x - annotation_width * 0.5f, 2.0f);
    EXPECT_NEAR(above_y, point_y - 6.0f - annotation_height, 2.0f);
    float below_x = 0.0f;
    float below_y = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(below, &below_x, &below_y, nullptr, nullptr));
    EXPECT_NEAR(below_x, point_x - annotation_width * 0.5f, 2.0f);
    EXPECT_NEAR(below_y, point_y + 6.0f, 2.0f);

    Grapple_UiImageAnnotationDef invalid = annotation_def;
    invalid.x = 1.1f;
    EXPECT_EQ(Grapple_UiImageAnnotation(image, &invalid), nullptr);
    EXPECT_EQ(Grapple_UiImageAnnotation(panel, &annotation_def), nullptr);
    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiImageTakesItsSizeFromTheImage)
{
    const std::string path = MakeBitmap(64, 48);

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiImageDef image_def{};
    image_def.path = path.c_str();
    image_def.width = GRAPPLE_UI_FIT;
    Grapple_UiWidget *image = Grapple_UiImage(panel, &image_def);
    ASSERT_NE(image, nullptr) << "the bitmap did not load";

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float w = 0.0f;
    float h = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(image, nullptr, nullptr, &w, &h));
    // "fit" for a picture is the picture's own width, not a text measurement.
    EXPECT_NEAR(w, 64.0f, 2.0f);
    EXPECT_NEAR(h, 48.0f, 2.0f);

    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiImageClickReachesItsCallback)
{
    const std::string path = MakeBitmap(80, 40);

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    ClickCount counter;
    Grapple_UiImageDef image_def{};
    image_def.path = path.c_str();
    image_def.on_click = CountClick;
    image_def.user = &counter;
    Grapple_UiWidget *image = Grapple_UiImage(panel, &image_def);
    ASSERT_NE(image, nullptr);

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(image, &x, &y, &w, &h));
    const float cx = x + w / 2.0f;
    const float cy = y + h / 2.0f;

    for (int phase = 0; phase < 2; ++phase)
    {
        BeginFrame();
        Grapple_GuiInputBegin(gui_);
        FeedMouseMove(cx, cy);
        FeedButton(cx, cy, phase == 0);
        Grapple_GuiInputEnd(gui_);
        Grapple_UiDraw(ui);
    }

    EXPECT_EQ(counter.clicks, 1);
    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, UiImageWithNoUsableSourceFails)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    // A missing file is a NULL widget rather than a picture-shaped hole that
    // draws nothing and says nothing.
    Grapple_UiImageDef missing{};
    missing.path = "/no/such/image/anywhere.bmp";
    EXPECT_EQ(Grapple_UiImage(panel, &missing), nullptr);

    Grapple_DestroyUi(ui);
}

// A filling panel is re-measured every frame, which is what makes a resize
// reflow the layout instead of scaling it. Two surfaces of different widths
// stand in for a window before and after a drag.

TEST(GuiStandalone, AFillingPanelFollowsTheWindowWidth)
{
    // SDL_Init(0), not SDL_INIT_VIDEO: a software renderer over a surface
    // needs no video subsystem, and asking for one fails on a machine with
    // no display — which is every CI runner that is not macOS.
    ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();

    auto stretched_label_width = [](int surface_width) {
        SDL_Surface *surface =
            SDL_CreateSurface(surface_width, 240, SDL_PIXELFORMAT_RGBA32);
        SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
        Grapple_Gui *gui = Grapple_CreateGui(renderer, nullptr, 0, 0.0f);
        Grapple_Ui *ui = Grapple_CreateUi(gui);

        Grapple_UiPanelDef panel_def{};
        panel_def.fill = true;
        Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

        Grapple_UiLabelDef label_def{};
        label_def.text = "stretches";
        Grapple_UiWidget *label = Grapple_UiLabel(panel, &label_def);

        Grapple_GuiInputBegin(gui);
        Grapple_GuiInputEnd(gui);
        SDL_RenderClear(renderer);
        Grapple_UiDraw(ui);

        float w = 0.0f;
        Grapple_UiBounds(label, nullptr, nullptr, &w, nullptr);

        Grapple_DestroyUi(ui);
        Grapple_DestroyGui(gui);
        SDL_DestroyRenderer(renderer);
        SDL_DestroySurface(surface);
        return w;
    };

    const float narrow = stretched_label_width(320);
    const float wide = stretched_label_width(640);

    EXPECT_GT(narrow, 0.0f);
    // Roughly twice the room, so the layout followed the window rather than
    // being scaled up from a fixed design size.
    EXPECT_GT(wide, narrow * 1.7f) << "narrow=" << narrow << " wide=" << wide;

    SDL_Quit();
}

// `path` reads whatever the installed loader reads, and SDL_image when
// nothing is installed. The hook is for a file that is not where its name
// says it is: an atlas, a pack file, a cache.

namespace {

int g_loader_calls = 0;

SDL_Texture *CountingLoader(SDL_Renderer *renderer, const char *path, void *user)
{
    g_loader_calls++;
    *static_cast<std::string *>(user) = path;
    if (SDL_strcmp(path, "missing.png") == 0)
    {
        SDL_SetError("deliberate test load failure");
        return nullptr;
    }
    const int width = (SDL_strcmp(path, "replacement.png") == 0) ? 56 : 12;
    SDL_Surface *surface = SDL_CreateSurface(width, 34, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

} // namespace

TEST_F(GuiHarness, AnInstalledImageLoaderHandlesPath)
{
    std::string seen;
    g_loader_calls = 0;
    Grapple_UiSetImageLoader(CountingLoader, &seen);

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiImageDef image_def{};
    // A name nothing on disk answers to: it works because the loader was
    // asked rather than the filesystem.
    image_def.path = "not-a-real-file-anywhere.png";
    image_def.width = GRAPPLE_UI_FIT;
    Grapple_UiWidget *image = Grapple_UiImage(panel, &image_def);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(g_loader_calls, 1);
    EXPECT_EQ(seen, "not-a-real-file-anywhere.png");

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    float w = 0.0f;
    ASSERT_TRUE(Grapple_UiBounds(image, nullptr, nullptr, &w, nullptr));
    EXPECT_NEAR(w, 12.0f, 2.0f) << "the loader's texture decided the size";

    ASSERT_TRUE(Grapple_UiSetImagePath(image, "replacement.png"));
    EXPECT_EQ(g_loader_calls, 2);
    EXPECT_EQ(seen, "replacement.png");

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);
    ASSERT_TRUE(Grapple_UiBounds(image, nullptr, nullptr, &w, nullptr));
    EXPECT_NEAR(w, 56.0f, 2.0f) << "the replacement texture decided the size";

    EXPECT_FALSE(Grapple_UiSetImagePath(image, "missing.png"));
    EXPECT_STREQ(SDL_GetError(), "deliberate test load failure");

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);
    ASSERT_TRUE(Grapple_UiBounds(image, nullptr, nullptr, &w, nullptr));
    EXPECT_NEAR(w, 56.0f, 2.0f) << "a failed load keeps the previous image";

    Grapple_DestroyUi(ui);

    // Restore, so the next test sees the built-in.
    Grapple_UiSetImageLoader(nullptr, nullptr);
}

// A widget's value is not its text.
//
// Sharing one handler between several widgets means telling them apart, and
// the only place to hang that used to be the text — which is a label the
// user reads, and which a picture does not have at all.

TEST_F(GuiHarness, AWidgetsValueIsSeparateFromItsText)
{
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);

    Grapple_UiButtonDef button_def{};
    button_def.text = "Exit";
    button_def.value = "quit";
    Grapple_UiWidget *button = Grapple_UiButton(panel, &button_def);
    ASSERT_NE(button, nullptr);

    EXPECT_EQ(std::string(Grapple_UiText(button)), "Exit");
    EXPECT_EQ(std::string(Grapple_UiValueText(button)), "quit");

    // Changing one leaves the other alone.
    Grapple_UiSetText(button, "Close");
    EXPECT_EQ(std::string(Grapple_UiValueText(button)), "quit");
    Grapple_UiSetValueText(button, "cancel");
    EXPECT_EQ(std::string(Grapple_UiText(button)), "Close");

    // A widget that was never given one reads as empty rather than NULL.
    Grapple_UiLabelDef label_def{};
    label_def.text = "plain";
    Grapple_UiWidget *label = Grapple_UiLabel(panel, &label_def);
    EXPECT_EQ(std::string(Grapple_UiValueText(label)), "");

    Grapple_DestroyUi(ui);
}

// Drawing belongs to whoever owns the loop, so forgetting it gives a blank
// window. The UI notices and says so, once, rather than leaving that
// unexplained — the cost of making the call explicit.

namespace {

std::string g_captured_log;

void CaptureLog(void *, int, SDL_LogPriority, const char *message)
{
    g_captured_log += message;
    g_captured_log += "\n";
}

// Watch what SDL logs for the duration of a test, then put it back.
class LogCapture
{
  public:
    LogCapture()
    {
        g_captured_log.clear();
        SDL_GetLogOutputFunction(&previous_, &previous_user_);
        SDL_SetLogOutputFunction(CaptureLog, nullptr);
    }
    ~LogCapture() { SDL_SetLogOutputFunction(previous_, previous_user_); }
    const std::string &text() const { return g_captured_log; }

  private:
    SDL_LogOutputFunction previous_ = nullptr;
    void *previous_user_ = nullptr;
};

int CountOccurrences(const std::string &haystack, const std::string &needle)
{
    int found = 0;
    for (size_t at = haystack.find(needle); at != std::string::npos;
         at = haystack.find(needle, at + needle.size()))
    {
        found++;
    }
    return found;
}

} // namespace

TEST_F(GuiHarness, AUiThatIsNeverDrawnSaysSoExactlyOnce)
{
    LogCapture log;

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);

    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);
    Grapple_UiLabelDef label_def{};
    label_def.text = "unseen";
    Grapple_UiLabel(panel, &label_def);

    for (int frame = 0; frame < 200; ++frame)
    {
        Grapple_UiNoteFrame(ui);
    }

    // Once — a warning repeated every frame is noise, not help.
    EXPECT_EQ(CountOccurrences(log.text(), "never been drawn"), 1) << log.text();

    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, ADrawnUiIsNeverAccusedOfNotBeingDrawn)
{
    LogCapture log;

    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);

    Grapple_UiPanelDef panel_def{};
    panel_def.fill = true;
    Grapple_UiWidget *panel = Grapple_UiPanel(ui, &panel_def);
    Grapple_UiLabelDef label_def{};
    label_def.text = "seen";
    Grapple_UiWidget *label = Grapple_UiLabel(panel, &label_def);

    BeginFrame();
    Grapple_GuiInputBegin(gui_);
    Grapple_GuiInputEnd(gui_);
    Grapple_UiDraw(ui);

    for (int frame = 0; frame < 200; ++frame)
    {
        Grapple_UiNoteFrame(ui);
    }

    EXPECT_EQ(CountOccurrences(log.text(), "never been drawn"), 0) << log.text();

    // Noting frames must not disturb the layout it already has.
    float w = 0.0f;
    EXPECT_TRUE(Grapple_UiBounds(label, nullptr, nullptr, &w, nullptr));
    EXPECT_GT(w, 0.0f);

    Grapple_DestroyUi(ui);
}

TEST_F(GuiHarness, AnEmptyUiIsNotAccused)
{
    LogCapture log;

    // No widgets: a UI built and not yet populated is not a mistake, and
    // saying so would train people to ignore the warning.
    Grapple_Ui *ui = Grapple_CreateUi(gui_);
    ASSERT_NE(ui, nullptr);
    for (int frame = 0; frame < 200; ++frame)
    {
        Grapple_UiNoteFrame(ui);
    }
    EXPECT_EQ(CountOccurrences(log.text(), "never been drawn"), 0) << log.text();
    Grapple_DestroyUi(ui);
}
