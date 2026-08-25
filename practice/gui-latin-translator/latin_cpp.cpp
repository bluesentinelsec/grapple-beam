// Latin Translator — C++, through the RAII wrappers.
//
// Same program as latin_c.c. The difference worth reading for is ownership:
// grapple::Gui destroys itself, and Create returns a Result that carries the
// error rather than a null pointer plus SDL_GetError().
//
// Escape closes the window.

#include <grapple/ui.h>

#include <SDL3/SDL.h>

#include <array>
#include <string_view>

namespace {

constexpr int kWindowWidth = 520;
constexpr int kWindowHeight = 320;

struct Word {
  std::string_view latin;
  std::string_view english;
};

// The whole dictionary. The buttons draw themselves from it, so adding a word
// is one line.
constexpr std::array<Word, 3> kWords{{
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
}};

// Nuklear takes const char*, and string_view is not guaranteed terminated —
// but these are all literals, so a tiny helper keeps the call sites honest
// about why .data() is safe here.
constexpr const char* CStr(std::string_view literal) { return literal.data(); }

}  // namespace

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("could not start SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Latin Translator — C++", kWindowWidth, kWindowHeight, 0);
  SDL_Renderer* renderer = window ? SDL_CreateRenderer(window, nullptr) : nullptr;
  if (renderer == nullptr) {
    SDL_Log("could not open a window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Scoped, so the Gui destroys itself before the renderer it draws through.
  // That ordering is the reason this is a block and not a bare declaration.
  {
    auto gui = grapple::Gui::Create(renderer, 18.0f);
    if (!gui.ok()) {
      SDL_Log("could not create the gui: %s", gui.status().message().c_str());
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    // What the label shows before anything has been clicked.
    std::string_view translation;
    bool running = true;

    while (running) {
      gui->InputBegin();
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
          running = false;
        } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
          running = false;
        }
        gui->ProcessEvent(event);
      }
      gui->InputEnd();

      nk_context* ctx = gui->context();
      if (nk_begin(ctx, "Latin Translator", nk_rect(0, 0, kWindowWidth, kWindowHeight),
                   NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(ctx, 28, 1);
        nk_label(ctx, "Implementation: C++", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 12, 1);
        nk_label(ctx, "Click a Latin word:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 44, static_cast<int>(kWords.size()));
        for (const Word& word : kWords) {
          if (nk_button_label(ctx, CStr(word.latin))) {
            translation = word.english;
          }
        }

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "English:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 40, 1);
        nk_label(ctx, translation.empty() ? "" : CStr(translation), NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Escape closes this window.", NK_TEXT_CENTERED);
      }
      nk_end(ctx);

      SDL_SetRenderDrawColor(renderer, 32, 32, 40, 255);
      SDL_RenderClear(renderer);
      static_cast<void>(gui->Render());
      SDL_RenderPresent(renderer);
    }

  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
