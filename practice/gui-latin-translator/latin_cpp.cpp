// Latin Translator — C++.
//
// Three buttons, one per Latin word. Clicking one puts its English
// translation in a label. Escape closes the window.
//
// The widgets are declared once, in Load. Nothing here runs every frame: the
// engine owns the loop, the UI owns its own input and drawing, and the
// layout is described in units rather than pixels, so it reflows when the
// window is resized.

#include <grapple/engine.h>
#include <grapple/widgets.h>

#include <SDL3/SDL.h>

#include <array>
#include <memory>
#include <string_view>

namespace {

struct Word {
  std::string_view latin;
  std::string_view english;
};

constexpr std::array<Word, 3> kWords{{
    {"sinister", "left"},
    {"dexter", "right"},
    {"medium", "center"},
}};

class Translator {
 public:
  explicit Translator(Grapple_Engine* engine) : engine_(engine) {}

  bool Load() {
    // Opens the GUI with the platform's interface font and installs itself as
    // the engine's input sink and overlay: no per-frame calls at all.
    ui_.reset(Grapple_OpenUi(Grapple_EngineRenderer(engine_), 15.0f));
    if (ui_ == nullptr) {
      SDL_Log("could not create the ui: %s", SDL_GetError());
      return false;
    }
    const Grapple_EventSink sink = Grapple_UiEventSink(ui_.get());
    Grapple_EngineSetEventSink(engine_, &sink);
    Grapple_EngineSetOverlay(engine_, Grapple_UiDrawCallback, ui_.get());

    Grapple_UiPanelDef panel_def{};
    panel_def.title = "Latin Translator";
    panel_def.fill = true;
    panel_def.padding = 12;
    panel_def.spacing = 8;
    Grapple_UiWidget* panel = Grapple_UiPanel(ui_.get(), &panel_def);

    Grapple_UiLabelDef prompt{};
    prompt.text = "Click a Latin word:";
    Grapple_UiLabel(panel, &prompt);

    Grapple_UiLabelDef answer{};
    answer.align = GRAPPLE_UI_CENTER;
    answer.height = GRAPPLE_UI_EM(2.0f);
    answer_ = Grapple_UiLabel(panel, &answer);

    Grapple_UiStripDef row{};
    row.height = GRAPPLE_UI_EM(2.4f);
    row.spacing = 8;
    Grapple_UiWidget* words = Grapple_UiRow(panel, &row);

    for (size_t i = 0; i < kWords.size(); ++i) {
      // Each button carries what it means, so one handler serves all three.
      buttons_[i] = {this, kWords[i].english};
      Grapple_UiButtonDef button{};
      button.text = kWords[i].latin.data();
      button.on_click = Translate;
      button.user = &buttons_[i];
      Grapple_UiButton(words, &button);
    }

    Grapple_UiButtonDef clear{};
    clear.text = "Clear";
    clear.width = GRAPPLE_UI_FIT;
    clear.align = GRAPPLE_UI_RIGHT;
    clear.on_click = [](Grapple_UiWidget*, void* user) {
      Grapple_UiSetText(static_cast<Translator*>(user)->answer_, "");
    };
    clear.user = this;
    Grapple_UiButton(panel, &clear);

    Grapple_UiLabelDef hint{};
    hint.text = "Resize the window; the layout reflows.";
    hint.align = GRAPPLE_UI_CENTER;
    Grapple_UiLabel(panel, &hint);
    return true;
  }

  void Event(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
      Grapple_EngineQuit(engine_);
    }
  }

  void Unload() {
    Grapple_EngineSetEventSink(engine_, nullptr);
    Grapple_EngineSetOverlay(engine_, nullptr, nullptr);
    ui_.reset();
  }

 private:
  struct WordButton {
    Translator* app;
    std::string_view english;
  };

  static void Translate(Grapple_UiWidget*, void* user) {
    const auto* word = static_cast<const WordButton*>(user);
    Grapple_UiSetText(word->app->answer_, word->english.data());
  }

  struct UiDeleter {
    void operator()(Grapple_Ui* ui) const { Grapple_DestroyUi(ui); }
  };

  Grapple_Engine* engine_;
  std::unique_ptr<Grapple_Ui, UiDeleter> ui_;
  Grapple_UiWidget* answer_ = nullptr;
  std::array<WordButton, kWords.size()> buttons_{};
};

}  // namespace

int main(int argc, char** argv) {
  Grapple_EngineConfig config{};
  config.title = "Latin Translator — C++";
  config.window_width = 640;
  config.window_height = 400;
  config.presentation = GRAPPLE_PRESENT_NATIVE;  // pixels, so a resize reflows
  config.no_auto_mount = true;                   // nothing to load
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
  hooks.unload = [](void* user) { static_cast<Translator*>(user)->Unload(); };

  const bool ok = Grapple_RunGame(engine, &hooks, &app);
  Grapple_DestroyEngine(engine);
  return ok ? 0 : 1;
}
