/*
 * Latin Translator — C.
 *
 * Three buttons, one per Latin word. Clicking one puts its English
 * translation in a label.
 *
 * Four things this version is exercising, beyond the game itself:
 *
 *   1. The window is resizable, and the layout reflows rather than scaling.
 *      GRAPPLE_PRESENT_NATIVE means coordinates are pixels, so widening the
 *      window gives the widgets more room instead of stretching them.
 *   2. The loop belongs to the engine — Grapple_RunGame and its hooks. There
 *      is no `while (running)` here.
 *   3. The UI font is the platform's own, baked at the display's pixel
 *      density, so it is sharp on a Retina panel.
 *   4. Nothing places a widget at a hard-coded x/y. Grapple_GuiGrid takes
 *      column weights and the widgets follow, Tkinter-style.
 *
 * Escape closes it.
 */
#include <grapple/engine.h>
#include <grapple/gui.h>
#include <grapple/gui_grid.h>

#include <SDL3/SDL.h>

/* The whole dictionary. The buttons draw themselves from it, so adding a
   word is one line. */
static const struct
{
    const char *latin;
    const char *english;
} kWords[] = {
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
};

typedef struct Translator
{
    Grapple_Engine *engine;
    Grapple_Gui *gui;
    void *font_bytes;
    const char *translation; /* what the label shows; empty until a click */
} Translator;

/*
 * The platform's UI font, or NULL to fall back to Nuklear's built-in.
 *
 * Loading the system font rather than embedding one keeps a megabyte of
 * glyphs out of the repository and gets each platform's own look. The
 * built-in default is ProggyClean, a 13px bitmap face that does not survive
 * being scaled up on a Retina panel.
 */
static void *LoadUiFont(size_t *length)
{
    static const char *kCandidates[] = {
#if defined(SDL_PLATFORM_MACOS)
        "/System/Library/Fonts/SFNS.ttf",      /* San Francisco */
        "/System/Library/Fonts/Helvetica.ttc",
#elif defined(SDL_PLATFORM_WIN32)
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
#endif
    };

    for (size_t i = 0; i < SDL_arraysize(kCandidates); ++i)
    {
        void *bytes = SDL_LoadFile(kCandidates[i], length);
        if (bytes != NULL)
        {
            return bytes;
        }
    }
    return NULL;
}

static bool Load(void *user)
{
    Translator *app = (Translator *)user;

    size_t font_length = 0;
    app->font_bytes = LoadUiFont(&font_length);

    /* 15 is a point size, not a pixel size: CreateGui multiplies it by the
       window's pixel density, so this is 15pt on any display. */
    app->gui = Grapple_CreateGui(Grapple_EngineRenderer(app->engine), app->font_bytes,
                                   font_length, 15.0f);
    if (app->gui == NULL)
    {
        SDL_Log("could not create the gui: %s", SDL_GetError());
        return false;
    }

    /* Open the first frame's input window. Every later frame opens its own
       at the end of Update — see the note there. */
    Grapple_GuiInputBegin(app->gui);
    app->translation = "";
    return true;
}

static void Event(void *user, const SDL_Event *event)
{
    Translator *app = (Translator *)user;
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_ESCAPE)
    {
        Grapple_EngineQuit(app->engine);
    }
    Grapple_GuiProcessEvent(app->gui, event);
}

static void Update(void *user, float dt)
{
    (void)dt;
    Translator *app = (Translator *)user;
    struct nk_context *ctx = Grapple_GuiContext(app->gui);

    /* Close the input window opened at the end of the last frame. The engine
       drained this frame's events into Event() above, and Nuklear needs them
       all in before any widget is built. */
    Grapple_GuiInputEnd(app->gui);

    /* The panel fills the window, in pixels, re-read every frame — which is
       what makes a resize reflow instead of scale. */
    int width = 0;
    int height = 0;
    Grapple_EnginePixelSize(app->engine, &width, &height);

    if (nk_begin(ctx, "Latin Translator", nk_rect(0, 0, (float)width, (float)height),
                 NK_WINDOW_NO_SCROLLBAR))
    {
        const float line = Grapple_GuiFontHeight(app->gui);

        /* One column, so a full-width row. Height 0 means "one line of the
           current font", which is why nothing here is in pixels. */
        Grapple_GuiGrid header;
        Grapple_GuiGridBegin(ctx, &header, 1, NULL, 0);
        Grapple_GuiGridCell(&header);
        nk_label(ctx, "Implementation: C", NK_TEXT_CENTERED);
        Grapple_GuiGridCell(&header);
        nk_label(ctx, "Click a Latin word:", NK_TEXT_LEFT);
        Grapple_GuiGridEnd(&header);

        /* A second grid only because a row height is per-grid, not per-row,
           and buttons want more than one line. */
        Grapple_GuiGrid buttons;
        Grapple_GuiGridBegin(ctx, &buttons, (int)SDL_arraysize(kWords), NULL, line * 2.4f);
        for (size_t i = 0; i < SDL_arraysize(kWords); ++i)
        {
            Grapple_GuiGridCell(&buttons);
            if (nk_button_label(ctx, kWords[i].latin))
            {
                app->translation = kWords[i].english;
            }
        }
        Grapple_GuiGridEnd(&buttons);

        /* Label and answer on one row, the answer twice as wide: column
           weights, exactly like Tk's grid. */
        static const float kLabelThenAnswer[] = {1.0f, 2.0f};
        Grapple_GuiGrid result;
        Grapple_GuiGridBegin(ctx, &result, 2, kLabelThenAnswer, line * 2.0f);
        Grapple_GuiGridCell(&result);
        nk_label(ctx, "English:", NK_TEXT_RIGHT);
        Grapple_GuiGridCell(&result);
        nk_label(ctx, app->translation, NK_TEXT_LEFT);
        Grapple_GuiGridEnd(&result);

        Grapple_GuiGrid footer;
        Grapple_GuiGridBegin(ctx, &footer, 1, NULL, 0);
        Grapple_GuiGridCell(&footer);
        nk_label(ctx, "Resize the window; the layout reflows. Escape closes.",
                 NK_TEXT_CENTERED);
        Grapple_GuiGridEnd(&footer);
    }
    nk_end(ctx);

    /* Open the next frame's input window now, because the engine pumps
       events before it calls any hook: there is no "start of frame" hook to
       do it in. */
    Grapple_GuiInputBegin(app->gui);
}

static void PostRender(void *user)
{
    /* Over the finished frame, above any post-processing. */
    Translator *app = (Translator *)user;
    Grapple_GuiRender(app->gui);
}

static void Unload(void *user)
{
    Translator *app = (Translator *)user;
    Grapple_DestroyGui(app->gui);
    SDL_free(app->font_bytes);
    app->gui = NULL;
    app->font_bytes = NULL;
}

int main(int argc, char **argv)
{
    Grapple_EngineConfig config = {0};
    config.title = "Latin Translator — C";
    config.window_width = 640;
    config.window_height = 400;

    /* Coordinates are pixels and the window is resizable, which together are
       what make this behave like a desktop program rather than a game: more
       window means more room, not bigger widgets. */
    config.presentation = GRAPPLE_PRESENT_NATIVE;

    /* Nothing to mount: this program draws itself out of widgets. */
    config.no_auto_mount = true;

    /* So the engine sees --window-size and friends. */
    config.argc = argc;
    config.argv = argv;

    Grapple_Engine *engine = Grapple_CreateEngine(&config);
    if (engine == NULL)
    {
        SDL_Log("could not create an engine: %s", SDL_GetError());
        return 1;
    }

    Translator app = {0};
    app.engine = engine;

    Grapple_GameHooks hooks = {0};
    hooks.load = Load;
    hooks.event = Event;
    hooks.update = Update;
    hooks.post_render = PostRender;
    hooks.unload = Unload;

    const bool ok = Grapple_RunGame(engine, &hooks, &app);
    Grapple_DestroyEngine(engine);
    return ok ? 0 : 1;
}
