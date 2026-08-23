// ui.h — RAII over the Nuklear SDL3 backend.
// Original Grapple code (zlib). The full immediate-mode nk_* API stays
// available through context(); this wrapper owns setup/teardown and the
// per-frame input/render calls.

#ifndef GRAPPLE_CPP_UI_H_
#define GRAPPLE_CPP_UI_H_

#include <grapple/gui.h>

#include <utility>

#include "grapple/status.h"

namespace grapple {

class Gui {
 public:
  static Result<Gui> Create(SDL_Renderer* renderer, float font_size = 0.0f) {
    Grapple_Gui* raw = Grapple_CreateGui(renderer, nullptr, 0, font_size);
    if (raw == nullptr) return Status::FromSdl();
    return Gui(raw);
  }

  ~Gui() { Destroy(); }
  Gui(Gui&& other) noexcept : gui_(std::exchange(other.gui_, nullptr)) {}
  Gui& operator=(Gui&& other) noexcept {
    if (this != &other) {
      Destroy();
      gui_ = std::exchange(other.gui_, nullptr);
    }
    return *this;
  }
  Gui(const Gui&) = delete;
  Gui& operator=(const Gui&) = delete;

  void InputBegin() { Grapple_GuiInputBegin(gui_); }
  bool ProcessEvent(const SDL_Event& event) { return Grapple_GuiProcessEvent(gui_, &event); }
  void InputEnd() { Grapple_GuiInputEnd(gui_); }
  bool wants_input() const { return Grapple_GuiWantsInput(gui_); }

  Status Render() {
    if (!Grapple_GuiRender(gui_)) return Status::FromSdl();
    return Status::Ok();
  }

  struct nk_context* context() const { return Grapple_GuiContext(gui_); }
  Grapple_Gui* get() const { return gui_; }

 private:
  explicit Gui(Grapple_Gui* gui) : gui_(gui) {}
  void Destroy() {
    if (gui_ != nullptr) Grapple_DestroyGui(gui_);
    gui_ = nullptr;
  }
  Grapple_Gui* gui_ = nullptr;
};

}  // namespace grapple

#endif  // GRAPPLE_CPP_UI_H_
