// ui.cc — retained C++ widgets over the C productivity layer.

#include "grapple/ui.h"

#include <memory>
#include <utility>
#include <vector>

#include "grapple/game.h"

namespace grapple {
namespace detail {

struct WidgetCallback {
  UiState* state = nullptr;
  Widget::Callback function;
};

struct RawCallback {
  std::function<void(struct nk_context*)> function;
};

struct UiState {
  explicit UiState(Grapple_Ui* raw_ui) : ui(raw_ui) {}

  ~UiState() {
    if (engine != nullptr) {
      Grapple_EngineSetEventSink(engine, nullptr);
      Grapple_EngineSetOverlay(engine, nullptr, nullptr);
    }
    Grapple_DestroyUi(ui);
  }

  WidgetCallback* Keep(Widget::Callback function) {
    if (!function) return nullptr;
    auto callback = std::make_unique<WidgetCallback>();
    callback->state = this;
    callback->function = std::move(function);
    WidgetCallback* raw = callback.get();
    widget_callbacks.push_back(std::move(callback));
    return raw;
  }

  RawCallback* Keep(std::function<void(struct nk_context*)> function) {
    if (!function) return nullptr;
    auto callback = std::make_unique<RawCallback>();
    callback->function = std::move(function);
    RawCallback* raw = callback.get();
    raw_callbacks.push_back(std::move(callback));
    return raw;
  }

  Grapple_Ui* ui = nullptr;
  Grapple_Engine* engine = nullptr;
  std::vector<std::unique_ptr<WidgetCallback>> widget_callbacks;
  std::vector<std::unique_ptr<RawCallback>> raw_callbacks;
};

}  // namespace detail

namespace {

Grapple_UiLength ToC(UiLength length) { return Grapple_UiLength{length.value(), length.unit()}; }

Grapple_UiAlign ToC(UiAlign align) {
  switch (align) {
    case UiAlign::kCenter:
      return GRAPPLE_UI_CENTER;
    case UiAlign::kRight:
      return GRAPPLE_UI_RIGHT;
    case UiAlign::kLeft:
      return GRAPPLE_UI_LEFT;
  }
  return GRAPPLE_UI_LEFT;
}

Grapple_GuiImageMode ToC(UiImageMode mode) {
  switch (mode) {
    case UiImageMode::kZoom:
      return GRAPPLE_GUI_IMAGE_ZOOM;
    case UiImageMode::kCenter:
      return GRAPPLE_GUI_IMAGE_CENTER;
    case UiImageMode::kFill:
      return GRAPPLE_GUI_IMAGE_FILL;
    case UiImageMode::kStretch:
      return GRAPPLE_GUI_IMAGE_STRETCH;
  }
  return GRAPPLE_GUI_IMAGE_STRETCH;
}

Status CreationError(const char* object) {
  const char* error = SDL_GetError();
  if (error != nullptr && error[0] != '\0') return Status::FromSdl();
  return Status::Error(std::string("could not create UI ") + object);
}

}  // namespace

void Widget::RunCallback(Grapple_UiWidget* widget, void* user) {
  auto* callback = static_cast<detail::WidgetCallback*>(user);
  callback->function(Widget(widget, callback->state));
}

void Widget::RunRaw(struct nk_context* context, void* user) {
  auto* callback = static_cast<detail::RawCallback*>(user);
  callback->function(context);
}

Result<Widget> Widget::Wrap(Grapple_UiWidget* widget) const {
  if (widget == nullptr) return CreationError("widget");
  return Widget(widget, state_);
}

Result<Widget> Widget::AddRow(const LayoutOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  Grapple_UiStripDef definition{};
  definition.height = ToC(options.height);
  definition.spacing = options.spacing;
  definition.align = ToC(options.align);
  return Wrap(Grapple_UiRow(widget_, &definition));
}

Result<Widget> Widget::AddColumn(const LayoutOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  Grapple_UiStripDef definition{};
  definition.height = ToC(options.height);
  definition.spacing = options.spacing;
  definition.align = ToC(options.align);
  return Wrap(Grapple_UiColumn(widget_, &definition));
}

Result<Widget> Widget::AddLabel(const LabelOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  Grapple_UiLabelDef definition{};
  definition.text = options.text.c_str();
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.wrap = options.wrap;
  return Wrap(Grapple_UiLabel(widget_, &definition));
}

Result<Widget> Widget::AddButton(const ButtonOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_click);
  Grapple_UiButtonDef definition{};
  definition.text = options.text.c_str();
  definition.value = options.value.empty() ? nullptr : options.value.c_str();
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.disabled = options.disabled;
  definition.on_click = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiButton(widget_, &definition));
}

Result<Widget> Widget::AddCheck(const CheckOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  Grapple_UiCheckDef definition{};
  definition.text = options.text.c_str();
  definition.checked = options.checked;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiCheck(widget_, &definition));
}

Result<Widget> Widget::AddSlider(const SliderOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  Grapple_UiSliderDef definition{};
  definition.value = options.value;
  definition.min = options.min;
  definition.max = options.max;
  definition.step = options.step;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiSlider(widget_, &definition));
}

Result<Widget> Widget::AddEntry(const EntryOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  Grapple_UiEntryDef definition{};
  definition.text = options.text.c_str();
  definition.capacity = options.capacity;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiEntry(widget_, &definition));
}

Result<Widget> Widget::AddSelect(const ChoiceOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  std::vector<const char*> labels;
  labels.reserve(options.options.size() + 1);
  for (const std::string& label : options.options) labels.push_back(label.c_str());
  labels.push_back(nullptr);

  Grapple_UiSelectDef definition{};
  definition.options = labels.data();
  definition.selected = options.selected;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiSelect(widget_, &definition));
}

Result<Widget> Widget::AddRadio(const ChoiceOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  std::vector<const char*> labels;
  labels.reserve(options.options.size() + 1);
  for (const std::string& label : options.options) labels.push_back(label.c_str());
  labels.push_back(nullptr);

  Grapple_UiSelectDef definition{};
  definition.options = labels.data();
  definition.selected = options.selected;
  definition.as_radio = true;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiRadio(widget_, &definition));
}

Result<Widget> Widget::AddProgress(const ProgressOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_change);
  Grapple_UiProgressDef definition{};
  definition.value = options.value;
  definition.max = options.max;
  definition.editable = options.editable;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_change = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiProgress(widget_, &definition));
}

Result<Widget> Widget::AddImage(const ImageOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::WidgetCallback* callback = state_->Keep(options.on_click);
  Grapple_UiImageDef definition{};
  definition.path = options.path.empty() ? nullptr : options.path.c_str();
  definition.value = options.value.empty() ? nullptr : options.value.c_str();
  definition.texture = options.texture;
  definition.mode = ToC(options.mode);
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  definition.align = ToC(options.align);
  definition.on_click = callback == nullptr ? nullptr : &Widget::RunCallback;
  definition.user = callback;
  return Wrap(Grapple_UiImage(widget_, &definition));
}

Result<Widget> Widget::AddSpacer(const SpacerOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  Grapple_UiSpacerDef definition{};
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  return Wrap(Grapple_UiSpacer(widget_, &definition));
}

Result<Widget> Widget::AddRaw(const RawOptions& options) {
  if (!valid() || state_ == nullptr) return Status::Error("cannot add to an empty UI widget");
  detail::RawCallback* callback = state_->Keep(options.draw);
  if (callback == nullptr) return Status::Error("a raw UI widget needs a draw callback");

  Grapple_UiRawDef definition{};
  definition.draw = &Widget::RunRaw;
  definition.user = callback;
  definition.width = ToC(options.width);
  definition.height = ToC(options.height);
  return Wrap(Grapple_UiRaw(widget_, &definition));
}

Status Widget::SetImage(const std::string& path) {
  if (!Grapple_UiSetImagePath(widget_, path.c_str())) return Status::FromSdl();
  return Status::Ok();
}

Status Widget::SetImage(SDL_Texture* texture) {
  if (!Grapple_UiSetImageTexture(widget_, texture)) return Status::FromSdl();
  return Status::Ok();
}

void Widget::SetText(const std::string& text) { Grapple_UiSetText(widget_, text.c_str()); }

std::string Widget::text() const {
  const char* value = Grapple_UiText(widget_);
  return value == nullptr ? std::string() : std::string(value);
}

void Widget::SetChecked(bool checked) { Grapple_UiSetChecked(widget_, checked); }

bool Widget::checked() const { return Grapple_UiChecked(widget_); }

void Widget::SetValue(float value) { Grapple_UiSetValue(widget_, value); }

float Widget::value() const { return Grapple_UiValue(widget_); }

void Widget::SetVisible(bool visible) { Grapple_UiSetVisible(widget_, visible); }

bool Widget::visible() const { return Grapple_UiVisible(widget_); }

void Widget::SetDisabled(bool disabled) { Grapple_UiSetDisabled(widget_, disabled); }

bool Widget::disabled() const { return Grapple_UiDisabled(widget_); }

void Widget::SetSelected(int index) { Grapple_UiSetSelected(widget_, index); }

int Widget::selected() const { return Grapple_UiSelected(widget_); }

std::vector<std::string> Widget::options() const {
  std::vector<std::string> result;
  const int count = Grapple_UiOptionCount(widget_);
  result.reserve(static_cast<std::size_t>(count));
  for (int index = 0; index < count; ++index) {
    result.emplace_back(Grapple_UiOption(widget_, index));
  }
  return result;
}

void Widget::SetValueText(const std::string& value) {
  Grapple_UiSetValueText(widget_, value.c_str());
}

std::string Widget::value_text() const {
  const char* value = Grapple_UiValueText(widget_);
  return value == nullptr ? std::string() : std::string(value);
}

Result<UiBounds> Widget::Bounds() const {
  UiBounds bounds;
  if (!Grapple_UiBounds(widget_, &bounds.x, &bounds.y, &bounds.width, &bounds.height)) {
    return Status::Error("UI widget has not been drawn or is hidden");
  }
  return bounds;
}

void Widget::Invoke() { Grapple_UiInvoke(widget_); }

void Widget::Remove() {
  Grapple_UiRemove(widget_);
  widget_ = nullptr;
  state_ = nullptr;
}

void Widget::Clear() { Grapple_UiClear(widget_); }

Result<Ui> Ui::CreateForEngine(Engine& engine, float font_points) {
  Grapple_Ui* raw = Grapple_OpenUi(engine.renderer(), font_points);
  if (raw == nullptr) return CreationError("for engine");

  auto state = std::make_unique<detail::UiState>(raw);
  state->engine = engine.get();
  const Grapple_EventSink sink = Grapple_UiEventSink(raw);
  Grapple_EngineSetEventSink(engine.get(), &sink);
  Grapple_EngineSetOverlay(engine.get(), Grapple_UiNoteFrameCallback, raw);
  return Ui(std::move(state));
}

Result<Ui> Ui::Open(SDL_Renderer* renderer, float font_points) {
  Grapple_Ui* raw = Grapple_OpenUi(renderer, font_points);
  if (raw == nullptr) return CreationError("for renderer");
  return Ui(std::make_unique<detail::UiState>(raw));
}

Result<Ui> Ui::Create(Gui& gui) {
  Grapple_Ui* raw = Grapple_CreateUi(gui.get());
  if (raw == nullptr) return CreationError("widget tree");
  return Ui(std::make_unique<detail::UiState>(raw));
}

Ui::Ui(std::unique_ptr<detail::UiState> state) : state_(std::move(state)) {}

Ui::~Ui() = default;

Ui::Ui(Ui&& other) noexcept = default;

Ui& Ui::operator=(Ui&& other) noexcept = default;

Result<Widget> Ui::AddPanel(const PanelOptions& options) {
  Grapple_UiPanelDef definition{};
  definition.title = options.title.empty() ? nullptr : options.title.c_str();
  definition.fill = options.fill;
  definition.x = options.x;
  definition.y = options.y;
  definition.width = options.width;
  definition.height = options.height;
  definition.padding = options.padding;
  definition.spacing = options.spacing;
  definition.movable = options.movable;
  definition.scrollable = options.scrollable;
  definition.no_border = options.no_border;
  Grapple_UiWidget* panel = Grapple_UiPanel(state_->ui, &definition);
  if (panel == nullptr) return CreationError("panel");
  return Widget(panel, state_.get());
}

void Ui::Draw() { Grapple_UiDraw(state_->ui); }

void Ui::NoteFrame() { Grapple_UiNoteFrame(state_->ui); }

bool Ui::wants_input() const { return Grapple_UiWantsInput(state_->ui); }

Grapple_EventSink Ui::event_sink() const { return Grapple_UiEventSink(state_->ui); }

void Ui::ShowMessage(const std::string& text) { Grapple_UiMessage(state_->ui, "", text.c_str()); }

void Ui::ShowMessage(const std::string& title, const std::string& text) {
  Grapple_UiMessage(state_->ui, title.c_str(), text.c_str());
}

Grapple_Gui* Ui::gui() const { return Grapple_UiGui(state_->ui); }

Grapple_Ui* Ui::get() const { return state_->ui; }

}  // namespace grapple
