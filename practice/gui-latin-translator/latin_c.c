/*
 * Latin Translator — C.
 *
 * Three buttons, one per Latin word. Clicking one puts its English
 * translation in a label. Escape closes the window.
 *
 * The widgets are declared once, in Load. Nothing here runs every frame:
 * the engine owns the loop, the UI owns its own input and drawing, and the
 * layout is described in units rather than pixels, so it reflows when the
 * window is resized.
 */
#include <grapple/engine.h>
#include <grapple/widgets.h>

#include <SDL3/SDL.h>

static const struct
{
    const char *latin;
    const char *english;
} kWords[] = {
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
};

typedef struct Translator Translator;

/* What one button means. Handed to the button as its user pointer, so one
   handler serves all three without working out which one was pressed. */
typedef struct WordButton
{
    Translator *app;
    const char *english;
} WordButton;

struct Translator
{
    Grapple_Engine *engine;
    Grapple_Ui *ui;
    Grapple_UiWidget *answer;
    WordButton buttons[SDL_arraysize(kWords)];
};

static void Translate(Grapple_UiWidget *button, void *user)
{
    (void)button;
    const WordButton *word = (const WordButton *)user;
    Grapple_UiSetText(word->app->answer, word->english);
}

static void Clear(Grapple_UiWidget *button, void *user)
{
    (void)button;
    Grapple_UiSetText(((Translator *)user)->answer, "");
}

static bool Load(void *user)
{
    Translator *app = (Translator *)user;

    /* Opens the GUI with the platform's interface font and installs itself
       as the engine's input sink and overlay: no per-frame calls at all. */
    app->ui = Grapple_OpenUi(Grapple_EngineRenderer(app->engine), 15.0f);
    if (app->ui == NULL)
    {
        SDL_Log("could not create the ui: %s", SDL_GetError());
        return false;
    }
    const Grapple_EventSink sink = Grapple_UiEventSink(app->ui);
    Grapple_EngineSetEventSink(app->engine, &sink);
    Grapple_EngineSetOverlay(app->engine, Grapple_UiDrawCallback, app->ui);

    Grapple_UiWidget *panel = Grapple_UiPanel(app->ui, &(Grapple_UiPanelDef){
        .title = "Latin Translator", .fill = true, .padding = 12, .spacing = 8 });

    Grapple_UiLabel(panel, &(Grapple_UiLabelDef){ .text = "Click a Latin word:" });

    app->answer = Grapple_UiLabel(panel, &(Grapple_UiLabelDef){
        .align = GRAPPLE_UI_CENTER, .height = GRAPPLE_UI_EM(2.0f) });

    Grapple_UiWidget *words = Grapple_UiRow(panel, &(Grapple_UiStripDef){
        .height = GRAPPLE_UI_EM(2.4f), .spacing = 8 });
    for (size_t i = 0; i < SDL_arraysize(kWords); ++i)
    {
        app->buttons[i].app = app;
        app->buttons[i].english = kWords[i].english;
        Grapple_UiButton(words, &(Grapple_UiButtonDef){
            .text = kWords[i].latin, .on_click = Translate, .user = &app->buttons[i] });
    }

    Grapple_UiButton(panel, &(Grapple_UiButtonDef){
        .text = "Clear", .width = GRAPPLE_UI_FIT, .align = GRAPPLE_UI_RIGHT,
        .on_click = Clear, .user = app });

    Grapple_UiLabel(panel, &(Grapple_UiLabelDef){
        .text = "Resize the window; the layout reflows.", .align = GRAPPLE_UI_CENTER });
    return true;
}

static void Event(void *user, const SDL_Event *event)
{
    Translator *app = (Translator *)user;
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_ESCAPE)
    {
        Grapple_EngineQuit(app->engine);
    }
}

static void Unload(void *user)
{
    Translator *app = (Translator *)user;
    Grapple_EngineSetEventSink(app->engine, NULL);
    Grapple_EngineSetOverlay(app->engine, NULL, NULL);
    Grapple_DestroyUi(app->ui);
    app->ui = NULL;
}

int main(int argc, char **argv)
{
    Grapple_EngineConfig config = {0};
    config.title = "Latin Translator — C";
    config.window_width = 640;
    config.window_height = 400;
    config.presentation = GRAPPLE_PRESENT_NATIVE; /* pixels, so a resize reflows */
    config.no_auto_mount = true;                  /* nothing to load */
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
    hooks.unload = Unload;

    const bool ok = Grapple_RunGame(engine, &hooks, &app);
    Grapple_DestroyEngine(engine);
    return ok ? 0 : 1;
}
