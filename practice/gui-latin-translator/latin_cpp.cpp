// Latin Translator — C++, through the RAII API.
//
// Same program as latin_c.c: resizable window, the engine's loop, the
// platform UI font at display density, and a weighted grid instead of
// hard-coded coordinates. Escape closes it.
//
// What is different from the C version is ownership. grapple::Engine and
// grapple::Gui destroy themselves, the hooks are lambdas that capture this
// object, and there is no void* user pointer to cast back.

#include <grapple/game.h>
#include <grapple/gui_grid.h>
#include <grapple/ui.h>

#include <SDL3/SDL.h>

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace {

struct Word {
  std::string_view latin;
  std::string_view english;
};

// The whole dictionary. The buttons draw themselves from it.
constexpr std::array<Word, 3> kWords{{
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
}};

// Nuklear wants const char*, and these are all literals, so .data() is
// terminated. The helper keeps that reasoning at the call site.
constexpr const char* CStr(std::string_view literal) { return literal.data(); }

// The platform's UI font, empty to fall back to Nuklear's built-in.
//
// Loading the system font rather than embedding one keeps a megabyte of
// glyphs out of the repository and gets each platform's own look. The
// built-in is ProggyClean, a 13px bitmap face that does not survive being
// scaled up on a Retina panel.
std::vector<char> LoadUiFont() {
  static constexpr std::array kCandidates{
#if defined(SDL_PLATFORM_MACOS)
      "/System/Library/Fonts/SFNS.ttf",  // San Francisco
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

  for (const char* path : kCandidates) {
    size_t length = 0;
    void* bytes = SDL_LoadFile(path, &length);
    if (bytes == nullptr) continue;
    const char* first = static_cast<const char*>(bytes);
    std::vector<char> font(first, first + length);
    SDL_free(bytes);
    return font;
  }
  return {};
}

class Translator {
 public:
  explicit Translator(Grapple_Engine* engine) : engine_(engine) {}

  bool Load() {
    font_ = LoadUiFont();
    // 15 is a point size: CreateGui multiplies it by the window's pixel
    // density, so this is 15pt on any display.
    Grapple_Gui* raw = Grapple_CreateGui(Grapple_EngineRenderer(engine_),
                                             font_.empty() ? nullptr : font_.data(),
                                             font_.size(), 15.0f);
    if (raw == nullptr) {
      SDL_Log("could not create the gui: %s", SDL_GetError());
      return false;
    }
    gui_.reset(raw);
    // Hand the GUI to the engine and input handling is done: the engine
    // brackets Nuklear's input around its own event pump, every frame.
    const Grapple_EventSink sink = Grapple_GuiEventSink(gui_.get());
    Grapple_EngineSetEventSink(engine_, &sink);
    return true;
  }

  // Only the program's own business: the GUI is fed by the sink.
  void Event(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
      Grapple_EngineQuit(engine_);
    }
  }

  void Update() {
    nk_context* ctx = Grapple_GuiContext(gui_.get());

    // The panel fills the window, in pixels, re-read every frame — which is
    // what makes a resize reflow instead of scale.
    int width = 0;
    int height = 0;
    Grapple_EnginePixelSize(engine_, &width, &height);

    if (nk_begin(ctx, "Latin Translator",
                 nk_rect(0, 0, static_cast<float>(width), static_cast<float>(height)),
                 NK_WINDOW_NO_SCROLLBAR)) {
      const float line = Grapple_GuiFontHeight(gui_.get());

      // One grid for the whole panel. Three equal columns, so a full-width row
      // is a span of three and "English:" plus its answer fall out as a 1:2
      // split with no weights to declare.
      Grapple_GuiGrid grid;
      Grapple_GuiGridBegin(ctx, &grid, 3, nullptr, 0);
      Grapple_GuiGridSpacing(&grid, 8.0f, 8.0f);

      Grapple_GuiGridCellSpan(&grid, 3);
      nk_label(ctx, "Implementation: C++", NK_TEXT_CENTERED);
      Grapple_GuiGridCellSpan(&grid, 3);
      nk_label(ctx, "Click a Latin word:", NK_TEXT_LEFT);

      // Buttons want more than a line of text — this row only.
      Grapple_GuiGridRowHeight(&grid, line * 2.4f);
      for (const Word& word : kWords) {
        Grapple_GuiGridCell(&grid);
        if (nk_button_label(ctx, CStr(word.latin))) {
          translation_ = word.english;
        }
      }

      Grapple_GuiGridRowHeight(&grid, line * 2.0f);
      Grapple_GuiGridCell(&grid);
      nk_label(ctx, "English:", NK_TEXT_RIGHT);
      Grapple_GuiGridCellSpan(&grid, 2);
      nk_label(ctx, translation_.empty() ? "" : CStr(translation_), NK_TEXT_LEFT);

      // A quarter of the row, hugging the right: what a full-width cell cannot
      // say.
      Grapple_GuiGridRowHeight(&grid, line * 1.8f);
      Grapple_GuiGridCellPart(&grid, 3, 0.25f, GRAPPLE_GUI_ALIGN_RIGHT);
      if (nk_button_label(ctx, "Clear")) {
        translation_ = {};
      }

      Grapple_GuiGridCellSpan(&grid, 3);
      nk_label(ctx, "Resize the window; the layout reflows. Escape closes.", NK_TEXT_CENTERED);

      Grapple_GuiGridEnd(&grid);
    }
    nk_end(ctx);
  }

  // Over the finished frame, above any post-processing.
  void PostRender() { Grapple_GuiRender(gui_.get()); }

  void Unload() {
    // Clear the sink before the GUI it points at goes away.
    Grapple_EngineSetEventSink(engine_, nullptr);
    gui_.reset();
  }

 private:
  struct GuiDeleter {
    void operator()(Grapple_Gui* gui) const { Grapple_DestroyGui(gui); }
  };

  Grapple_Engine* engine_;
  std::unique_ptr<Grapple_Gui, GuiDeleter> gui_;
  std::vector<char> font_;
  std::string_view translation_;
};

}  // namespace

int main(int argc, char** argv) {
  Grapple_EngineConfig config{};
  config.title = "Latin Translator — C++";
  config.window_width = 640;
  config.window_height = 400;

  // Coordinates are pixels and the window is resizable, which together are
  // what make this behave like a desktop program rather than a game: more
  // window means more room, not bigger widgets.
  config.presentation = GRAPPLE_PRESENT_NATIVE;

  // Nothing to mount: this program draws itself out of widgets.
  config.no_auto_mount = true;

  // So the engine sees --window-size and friends.
  config.argc = argc;
  config.argv = argv;

  Grapple_Engine* engine = Grapple_CreateEngine(&config);
  if (engine == nullptr) {
    SDL_Log("could not create an engine: %s", SDL_GetError());
    return 1;
  }

  Translator app(engine);

  Grapple_GameHooks hooks{};
  hooks.load = [](void* user) { return static_cast<Translator*>(user)->Load(); };
  hooks.event = [](void* user, const SDL_Event* event) {
    static_cast<Translator*>(user)->Event(*event);
  };
  hooks.update = [](void* user, float) { static_cast<Translator*>(user)->Update(); };
  hooks.post_render = [](void* user) { static_cast<Translator*>(user)->PostRender(); };
  hooks.unload = [](void* user) { static_cast<Translator*>(user)->Unload(); };

  const bool ok = Grapple_RunGame(engine, &hooks, &app);
  Grapple_DestroyEngine(engine);
  return ok ? 0 : 1;
}
