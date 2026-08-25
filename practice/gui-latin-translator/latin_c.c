/*
 * Latin Translator — C.
 *
 * Three buttons, one per Latin word. Clicking one puts its English
 * translation in a label.
 *
 * A GUI-only program, so there is no engine and no game loop here: the
 * window, the renderer and the loop are ours, which is the standalone
 * shape gui.h documents. Escape closes it.
 */
#include <grapple/gui.h>

#include <SDL3/SDL.h>

#define WINDOW_WIDTH 520
#define WINDOW_HEIGHT 320

/* The whole dictionary. A table rather than three if-blocks, so adding a
   word is one line and the buttons draw themselves from it. */
static const struct
{
    const char *latin;
    const char *english;
} kWords[] = {
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
};

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("could not start SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Latin Translator — C", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = window ? SDL_CreateRenderer(window, NULL) : NULL;
    if (renderer == NULL)
    {
        SDL_Log("could not open a window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Grapple_Gui *gui = Grapple_CreateGui(renderer, NULL, 0, 18.0f);
    if (gui == NULL)
    {
        SDL_Log("could not create the gui: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* What the label shows before anything has been clicked. */
    const char *translation = "";
    bool running = true;

    while (running)
    {
        Grapple_GuiInputBegin(gui);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN &&
                     event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                running = false;
            }
            Grapple_GuiProcessEvent(gui, &event);
        }
        Grapple_GuiInputEnd(gui);

        struct nk_context *ctx = Grapple_GuiContext(gui);
        if (nk_begin(ctx, "Latin Translator", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, 28, 1);
            nk_label(ctx, "Implementation: C", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 12, 1);
            nk_label(ctx, "Click a Latin word:", NK_TEXT_LEFT);

            /* One row, one button per word, evenly divided. */
            nk_layout_row_dynamic(ctx, 44, (int)SDL_arraysize(kWords));
            for (size_t i = 0; i < SDL_arraysize(kWords); ++i)
            {
                if (nk_button_label(ctx, kWords[i].latin))
                {
                    translation = kWords[i].english;
                }
            }

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "English:", NK_TEXT_LEFT);

            nk_layout_row_dynamic(ctx, 40, 1);
            nk_label(ctx, translation, NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Escape closes this window.", NK_TEXT_CENTERED);
        }
        nk_end(ctx);

        SDL_SetRenderDrawColor(renderer, 32, 32, 40, 255);
        SDL_RenderClear(renderer);
        Grapple_GuiRender(gui);
        SDL_RenderPresent(renderer);
    }

    Grapple_DestroyGui(gui);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
