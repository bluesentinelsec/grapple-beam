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
    // Open the first frame's input window; every later frame opens its own
    // at the end of Update.
    Grapple_GuiInputBegin(gui_.get());
    return true;
  }

  void Event(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
      Grapple_EngineQuit(engine_);
    }
    Grapple_GuiProcessEvent(gui_.get(), &event);
  }

  void Update() {
    nk_context* ctx = Grapple_GuiContext(gui_.get());

    // Close the input window opened at the end of the last frame: the engine
    // has already drained this frame's events into Event(), and Nuklear needs
    // them all in before any widget is built.
    Grapple_GuiInputEnd(gui_.get());

    // The panel fills the window, in pixels, re-read every frame — which is
    // what makes a resize reflow instead of scale.
    int width = 0;
    int height = 0;
    Grapple_EnginePixelSize(engine_, &width, &height);

    if (nk_begin(ctx, "Latin Translator",
                 nk_rect(0, 0, static_cast<float>(width), static_cast<float>(height)),
                 NK_WINDOW_NO_SCROLLBAR)) {
      const float line = Grapple_GuiFontHeight(gui_.get());

      // One column, so a full-width row. Height 0 means "one line of the
      // current font", which is why nothing here is in pixels.
      Grapple_GuiGrid header;
      Grapple_GuiGridBegin(ctx, &header, 1, nullptr, 0);
      Grapple_GuiGridCell(&header);
      nk_label(ctx, "Implementation: C++", NK_TEXT_CENTERED);
      Grapple_GuiGridCell(&header);
      nk_label(ctx, "Click a Latin word:", NK_TEXT_LEFT);
      Grapple_GuiGridEnd(&header);

      // A second grid only because a row height is per-grid, not per-row, and
      // buttons want more than one line.
      Grapple_GuiGrid buttons;
      Grapple_GuiGridBegin(ctx, &buttons, static_cast<int>(kWords.size()), nullptr, line * 2.4f);
      for (const Word& word : kWords) {
        Grapple_GuiGridCell(&buttons);
        if (nk_button_label(ctx, CStr(word.latin))) {
          translation_ = word.english;
        }
      }
      Grapple_GuiGridEnd(&buttons);

      // Label and answer on one row, the answer twice as wide: column
      // weights, exactly like Tk's grid.
      static constexpr std::array<float, 2> kLabelThenAnswer{1.0f, 2.0f};
      Grapple_GuiGrid result;
      Grapple_GuiGridBegin(ctx, &result, 2, kLabelThenAnswer.data(), line * 2.0f);
      Grapple_GuiGridCell(&result);
      nk_label(ctx, "English:", NK_TEXT_RIGHT);
      Grapple_GuiGridCell(&result);
      nk_label(ctx, translation_.empty() ? "" : CStr(translation_), NK_TEXT_LEFT);
      Grapple_GuiGridEnd(&result);

      Grapple_GuiGrid footer;
      Grapple_GuiGridBegin(ctx, &footer, 1, nullptr, 0);
      Grapple_GuiGridCell(&footer);
      nk_label(ctx, "Resize the window; the layout reflows. Escape closes.", NK_TEXT_CENTERED);
      Grapple_GuiGridEnd(&footer);
    }
    nk_end(ctx);

    // Open the next frame's input window now, because the engine pumps events
    // before it calls any hook: there is no "start of frame" hook to do it in.
    Grapple_GuiInputBegin(gui_.get());
  }

  // Over the finished frame, above any post-processing.
  void PostRender() { Grapple_GuiRender(gui_.get()); }

  void Unload() { gui_.reset(); }

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
