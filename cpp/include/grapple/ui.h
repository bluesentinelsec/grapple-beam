// ui.h — C++ ownership for both Grapple GUI layers.
// Original Grapple code (zlib). Gui wraps the immediate-mode Nuklear backend;
// Ui and Widget wrap the retained productivity layer built over it.

#ifndef GRAPPLE_CPP_UI_H_
#define GRAPPLE_CPP_UI_H_

#include <grapple/gui.h>
#include <grapple/widgets.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "grapple/status.h"

namespace grapple {

class Engine;

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

namespace detail {
struct UiState;
}  // namespace detail

struct PanelOptions;
struct LayoutOptions;
struct LabelOptions;
struct ButtonOptions;
struct CheckOptions;
struct SliderOptions;
struct EntryOptions;
struct ChoiceOptions;
struct ProgressOptions;
struct ImageOptions;
struct SpacerOptions;
struct RawOptions;

/**
 * @brief A layout length for the retained UI.
 *
 * Use the named factories so code says what a size means. The default is
 * Stretch(), which shares the space left by fixed-size siblings.
 */
class UiLength {
 public:
  /**
   * @brief Share the space left by fixed-size siblings.
   * @return A stretching length.
   */
  static UiLength Stretch() { return UiLength(0.0f, GRAPPLE_UI_UNIT_STRETCH); }

  /**
   * @brief Use a fixed number of pixels.
   * @param value Pixel count.
   * @return The requested fixed length.
   */
  static UiLength Pixels(float value) { return UiLength(value, GRAPPLE_UI_UNIT_PX); }

  /**
   * @brief Size relative to the current font's line height.
   * @param value Number of line heights.
   * @return The requested font-relative length.
   */
  static UiLength Em(float value) { return UiLength(value, GRAPPLE_UI_UNIT_EM); }

  /**
   * @brief Size relative to the parent.
   * @param value Fraction of the parent, from 0.0 to 1.0.
   * @return The requested proportional length.
   */
  static UiLength Percent(float value) { return UiLength(value, GRAPPLE_UI_UNIT_PCT); }

  /**
   * @brief Fit the widget's own text or image content.
   * @return A content-fitted length.
   */
  static UiLength Fit() { return UiLength(0.0f, GRAPPLE_UI_UNIT_FIT); }

  /**
   * @brief Return the numeric part of the length.
   * @return Pixels, line heights, or a parent fraction according to unit().
   */
  float value() const { return value_; }

  /**
   * @brief Return the C API unit represented by this value.
   * @return The length's unit.
   */
  Grapple_UiUnit unit() const { return unit_; }

 private:
  UiLength(float value, Grapple_UiUnit unit) : value_(value), unit_(unit) {}

  float value_ = 0.0f;
  Grapple_UiUnit unit_ = GRAPPLE_UI_UNIT_STRETCH;
};

/** @brief Horizontal alignment within the space assigned to a widget. */
enum class UiAlign {
  kLeft,   /**< Place content at the left edge. */
  kCenter, /**< Centre content in its assigned space. */
  kRight,  /**< Place content at the right edge. */
};

/** @brief How an image is fitted into its assigned space. */
enum class UiImageMode {
  kStretch, /**< Fill the slot without preserving aspect ratio. */
  kZoom,    /**< Fit inside the slot while preserving aspect ratio. */
  kCenter,  /**< Keep the native pixel size and centre the image. */
  kFill,    /**< Cover the slot while preserving aspect ratio and cropping. */
};

/** @brief The pixel rectangle occupied by a widget during its last draw. */
struct UiBounds {
  float x = 0.0f;      /**< Left edge in renderer pixels. */
  float y = 0.0f;      /**< Top edge in renderer pixels. */
  float width = 0.0f;  /**< Width in renderer pixels. */
  float height = 0.0f; /**< Height in renderer pixels. */
};

/**
 * @brief A non-owning handle to one retained widget or layout container.
 *
 * Widgets are owned by their parent and ultimately by Ui. A handle remains
 * usable until the widget is removed, an ancestor is cleared or removed, or
 * its Ui is destroyed. Copying a handle does not copy the widget.
 */
class Widget {
 public:
  /**
   * @brief Function called when a widget is clicked or changed.
   *
   * Ui owns a copy of the callback for the tree's lifetime. The argument is
   * the widget that fired.
   */
  using Callback = std::function<void(Widget)>;

  /** @brief Construct an empty handle. */
  Widget() = default;

  /**
   * @brief Return true when this handle refers to a widget.
   * @return Whether the handle is non-empty.
   */
  bool valid() const { return widget_ != nullptr; }

  /**
   * @brief Return the underlying borrowed C widget handle.
   * @return The handle, or nullptr when empty. Ownership is not transferred.
   */
  Grapple_UiWidget* get() const { return widget_; }

  /**
   * @brief Add a horizontal container owned by this widget.
   * @param options Layout settings for the new row.
   * @return A new row handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddRow(const LayoutOptions& options);

  /**
   * @brief Add a vertical container owned by this widget.
   * @param options Layout settings for the new column.
   * @return A new column handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddColumn(const LayoutOptions& options);

  /**
   * @brief Add a text label owned by this widget.
   * @param options Text and layout settings.
   * @return A new label handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddLabel(const LabelOptions& options);

  /**
   * @brief Add a push button owned by this widget.
   * @param options Text, layout, and click callback settings.
   * @return A new button handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddButton(const ButtonOptions& options);

  /**
   * @brief Add a check box owned by this widget.
   * @param options Text, state, layout, and change callback settings.
   * @return A new check-box handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddCheck(const CheckOptions& options);

  /**
   * @brief Add a slider owned by this widget.
   * @param options Range, state, layout, and change callback settings.
   * @return A new slider handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddSlider(const SliderOptions& options);

  /**
   * @brief Add an owned text-entry field.
   * @param options Initial text, capacity, layout, and callback settings.
   * @return A new entry handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddEntry(const EntryOptions& options);

  /**
   * @brief Add a drop-down choice widget.
   * @param options Choices, selection, layout, and callback settings.
   * @return A new select handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddSelect(const ChoiceOptions& options);

  /**
   * @brief Add a visible group of radio choices.
   * @param options Choices, selection, layout, and callback settings.
   * @return A new radio handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddRadio(const ChoiceOptions& options);

  /**
   * @brief Add a progress indicator.
   * @param options Range, state, layout, and optional callback settings.
   * @return A new progress handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddProgress(const ProgressOptions& options);

  /**
   * @brief Add an image, optionally with a click callback.
   * @param options Image source, fitting, layout, and callback settings.
   * @return A new image handle, or an error if it cannot be loaded or created.
   */
  [[nodiscard]] Result<Widget> AddImage(const ImageOptions& options);

  /**
   * @brief Add blank layout space.
   * @param options Width and height settings.
   * @return A new spacer handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddSpacer(const SpacerOptions& options);

  /**
   * @brief Add a place where caller code draws directly with Nuklear.
   * @param options Drawing callback and layout settings.
   * @return A new raw handle, or an error if its callback is empty or creation fails.
   */
  [[nodiscard]] Result<Widget> AddRaw(const RawOptions& options);

  /**
   * @brief Replace this widget's displayed text. The widget copies it.
   * @param text New displayed text.
   */
  void SetText(const std::string& text);

  /**
   * @brief Return this widget's current displayed text.
   * @return A copy of the current text, or an empty string for an empty handle.
   */
  std::string text() const;

  /**
   * @brief Set a check box's checked state.
   * @param checked New checked state.
   */
  void SetChecked(bool checked);

  /**
   * @brief Return a check box's checked state.
   * @return The current state, or false for an empty handle.
   */
  bool checked() const;

  /**
   * @brief Set the numeric value of a slider or progress widget.
   * @param value New value, clamped immediately to the widget's range.
   */
  void SetValue(float value);

  /**
   * @brief Return the numeric value of a slider or progress widget.
   * @return The current value, or zero for an empty handle.
   */
  float value() const;

  /**
   * @brief Show or hide this widget. Hidden widgets take no layout space.
   * @param visible New visibility.
   */
  void SetVisible(bool visible);

  /**
   * @brief Return whether this widget is visible.
   * @return Current visibility, or false for an empty handle.
   */
  bool visible() const;

  /**
   * @brief Enable or disable interaction with this widget.
   * @param disabled New disabled state.
   */
  void SetDisabled(bool disabled);

  /**
   * @brief Return whether this widget is disabled.
   * @return Current disabled state, or false for an empty handle.
   */
  bool disabled() const;

  /**
   * @brief Set the zero-based choice selected by a select or radio widget.
   * @param index Choice index. Out-of-range indices leave the selection unchanged.
   */
  void SetSelected(int index);

  /**
   * @brief Return the zero-based selected choice.
   * @return The choice index, or -1 for an empty handle.
   */
  int selected() const;

  /**
   * @brief Return a copy of the choices owned by a select or radio widget.
   * @return Choices in display order, or an empty vector for an empty handle.
   */
  std::vector<std::string> options() const;

  /**
   * @brief Set the semantic value associated with this widget.
   * @param value New value text, copied by the widget.
   */
  void SetValueText(const std::string& value);

  /**
   * @brief Return the semantic value associated with this widget.
   * @return A copy of the value text, or an empty string when unset.
   */
  std::string value_text() const;

  /**
   * @brief Return the rectangle occupied during the last draw.
   * @return Bounds, or an error before the first draw or while hidden.
   */
  [[nodiscard]] Result<UiBounds> Bounds() const;

  /** @brief Run this widget's callback without pointer input. */
  void Invoke();

  /**
   * @brief Remove this widget and all of its children.
   *
   * This handle becomes empty. Other handles to the removed subtree must no
   * longer be used.
   */
  void Remove();

  /**
   * @brief Remove every child while keeping this container.
   *
   * Existing handles to the former children must no longer be used.
   */
  void Clear();

 private:
  friend class Ui;
  Widget(Grapple_UiWidget* widget, detail::UiState* state) : widget_(widget), state_(state) {}

  static void RunCallback(Grapple_UiWidget* widget, void* user);
  static void RunRaw(struct nk_context* context, void* user);
  [[nodiscard]] Result<Widget> Wrap(Grapple_UiWidget* widget) const;

  Grapple_UiWidget* widget_ = nullptr;
  detail::UiState* state_ = nullptr;
};

/**
 * @brief Settings for a top-level panel.
 *
 * Panels fill the renderer by default. Set fill to false to use the explicit
 * pixel rectangle.
 */
struct PanelOptions {
  std::string title;       /**< Optional title-bar text. */
  bool fill = true;        /**< Whether to cover the renderer. */
  float x = 0.0f;          /**< Left edge when fill is false. */
  float y = 0.0f;          /**< Top edge when fill is false. */
  float width = 0.0f;      /**< Pixel width when fill is false. */
  float height = 0.0f;     /**< Pixel height when fill is false. */
  float padding = 0.0f;    /**< Space inside the panel edges. */
  float spacing = 0.0f;    /**< Default space between children. */
  bool movable = false;    /**< Whether the player can drag the panel. */
  bool scrollable = false; /**< Whether overflowing content scrolls. */
  bool no_border = false;  /**< Whether to omit the panel border. */
};

/** @brief Settings shared by rows and columns. */
struct LayoutOptions {
  UiLength height = UiLength::Stretch(); /**< Row or child height. */
  float spacing = -1.0f;                 /**< Child spacing; negative inherits. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment of narrower children. */
};

/** @brief Settings for a text label. */
struct LabelOptions {
  std::string text;                      /**< Displayed text, copied. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Text alignment. */
  bool wrap = false;                     /**< Whether long text wraps. */
};

/** @brief Settings for a push button. */
struct ButtonOptions {
  std::string text;                      /**< Player-visible text. */
  std::string value;                     /**< Semantic value for callbacks. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  bool disabled = false;                 /**< Initial disabled state. */
  Widget::Callback on_click;             /**< Optional click callback. */
};

/** @brief Settings for a check box. */
struct CheckOptions {
  std::string text;                      /**< Text beside the check box. */
  bool checked = false;                  /**< Initial checked state. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  Widget::Callback on_change;            /**< Optional change callback. */
};

/** @brief Settings for a slider. */
struct SliderOptions {
  float value = 0.0f;                    /**< Initial value. */
  float min = 0.0f;                      /**< Minimum value. */
  float max = 1.0f;                      /**< Maximum value. */
  float step = 0.0f;                     /**< Step, or zero for automatic. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  Widget::Callback on_change;            /**< Optional change callback. */
};

/** @brief Settings for a text-entry field whose buffer is owned by the widget. */
struct EntryOptions {
  std::string text;                      /**< Initial text, copied. */
  int capacity = 256;                    /**< Maximum bytes including NUL. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  Widget::Callback on_change;            /**< Optional change callback. */
};

/** @brief Settings shared by drop-down and radio choices. Indices are zero-based. */
struct ChoiceOptions {
  std::vector<std::string> options;      /**< Labels, copied by the widget. */
  int selected = 0;                      /**< Zero-based initial selection. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  Widget::Callback on_change;            /**< Optional change callback. */
};

/** @brief Settings for a progress indicator. */
struct ProgressOptions {
  float value = 0.0f;                    /**< Initial value. */
  float max = 1.0f;                      /**< Maximum value. */
  bool editable = false;                 /**< Whether dragging changes it. */
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
  UiAlign align = UiAlign::kLeft;        /**< Alignment within the parent. */
  Widget::Callback on_change;            /**< Optional change callback. */
};

/**
 * @brief Settings for an image.
 *
 * Set either path or texture. A supplied texture remains owned by the caller
 * and must outlive the widget.
 */
struct ImageOptions {
  std::string path;                         /**< Image path, copied and loaded. */
  std::string value;                        /**< Semantic value for callbacks. */
  SDL_Texture* texture = nullptr;           /**< Borrowed texture; overrides path. */
  UiImageMode mode = UiImageMode::kStretch; /**< Fitting behavior. */
  UiLength width = UiLength::Stretch();     /**< Requested width. */
  UiLength height = UiLength::Stretch();    /**< Requested height. */
  UiAlign align = UiAlign::kLeft;           /**< Alignment within the parent. */
  Widget::Callback on_click;                /**< Optional click callback. */
};

/** @brief Settings for blank layout space. */
struct SpacerOptions {
  UiLength width = UiLength::Stretch();  /**< Requested width. */
  UiLength height = UiLength::Stretch(); /**< Requested height. */
};

/** @brief Settings for a direct Nuklear drawing callback inside the retained layout. */
struct RawOptions {
  std::function<void(struct nk_context*)> draw; /**< Required drawing callback. */
  UiLength width = UiLength::Stretch();         /**< Requested width. */
  UiLength height = UiLength::Stretch();        /**< Requested height. */
};

/**
 * @brief An owning retained UI: declare a Tk-like widget tree once and draw it each frame.
 *
 * Ui owns every panel and widget in the tree. When created for an Engine it
 * also installs and later removes the UI event sink. The renderer, Engine, or
 * Gui supplied at creation must outlive Ui.
 */
class Ui {
 public:
  /**
   * @brief Create a UI attached to an engine's input loop.
   * @param engine Engine that owns the renderer and pumps events.
   * @param font_points Interface font size in points; zero selects the default.
   * @return An owning UI, or an error if its GUI cannot be created.
   */
  static Result<Ui> CreateForEngine(Engine& engine, float font_points = 0.0f);

  /**
   * @brief Create a standalone UI and GUI for a renderer.
   * @param renderer Renderer borrowed for the UI's lifetime.
   * @param font_points Interface font size in points; zero selects the default.
   * @return An owning UI, or an error if its GUI cannot be created.
   */
  static Result<Ui> Open(SDL_Renderer* renderer, float font_points = 0.0f);

  /**
   * @brief Create a retained tree over an existing immediate-mode GUI.
   * @param gui GUI borrowed for the UI's lifetime.
   * @return An owning widget tree, or an allocation error.
   */
  static Result<Ui> Create(Gui& gui);

  ~Ui();
  Ui(Ui&& other) noexcept;
  Ui& operator=(Ui&& other) noexcept;
  Ui(const Ui&) = delete;
  Ui& operator=(const Ui&) = delete;

  /**
   * @brief Add a top-level panel owned by this UI.
   * @param options Panel appearance, placement, and layout settings.
   * @return A new panel handle, or an error if it cannot be created.
   */
  [[nodiscard]] Result<Widget> AddPanel(const PanelOptions& options);

  /** @brief Draw the complete retained tree over the frame rendered so far. */
  void Draw();

  /** @brief Tell the UI that a frame passed, for the missing-draw warning. */
  void NoteFrame();

  /**
   * @brief Return true while the pointer is over the UI.
   * @return Whether the UI currently wants pointer input.
   */
  bool wants_input() const;

  /**
   * @brief Return this UI as an event sink for a caller-owned event loop.
   * @return A borrowed sink that remains valid for this UI's lifetime.
   */
  Grapple_EventSink event_sink() const;

  /**
   * @brief Show a blocking native message box containing text.
   * @param text Message shown to the player.
   */
  void ShowMessage(const std::string& text);

  /**
   * @brief Show a blocking native message box with a title and text.
   * @param title Message-box title.
   * @param text Message shown to the player.
   */
  void ShowMessage(const std::string& title, const std::string& text);

  /**
   * @brief Return the borrowed immediate-mode GUI under this retained tree.
   * @return The GUI handle. Ownership is not transferred.
   */
  Grapple_Gui* gui() const;

  /**
   * @brief Return the underlying retained C UI handle.
   * @return The UI handle. Ownership is not transferred.
   */
  Grapple_Ui* get() const;

 private:
  explicit Ui(std::unique_ptr<detail::UiState> state);
  std::unique_ptr<detail::UiState> state_;
};

}  // namespace grapple

#endif  // GRAPPLE_CPP_UI_H_
