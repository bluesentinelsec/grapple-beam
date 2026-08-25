// GENERATED FILE - DO NOT EDIT.
// Regenerate: python3 -m tools.bindgen (see tools/bindgen/README.md)
// Library: Grapple modules
// clang-format off
#ifndef GRAPPLE_CPP_GEN_GRAPPLE_H_
#define GRAPPLE_CPP_GEN_GRAPPLE_H_

#include <grapple/base64.h>
#include <grapple/bidi.h>
#include <grapple/chiptune.h>
#include <grapple/compress.h>
#include <grapple/crypto.h>
#include <grapple/debug_text.h>
#include <grapple/dialog.h>
#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_assets.h>
#include <grapple/engine_binding.h>
#include <grapple/engine_camera.h>
#include <grapple/engine_config.h>
#include <grapple/engine_graphics.h>
#include <grapple/engine_input.h>
#include <grapple/engine_light.h>
#include <grapple/engine_media.h>
#include <grapple/engine_physics.h>
#include <grapple/engine_render.h>
#include <grapple/engine_save.h>
#include <grapple/engine_scene.h>
#include <grapple/engine_script.h>
#include <grapple/engine_text.h>
#include <grapple/gpu_build.h>
#include <grapple/gui.h>
#include <grapple/gui_grid.h>
#include <grapple/light.h>
#include <grapple/physics_build.h>
#include <grapple/physics_draw.h>
#include <grapple/regex.h>
#include <grapple/signals.h>
#include <grapple/textfile.h>
#include <grapple/tiled.h>
#include <grapple/vfs.h>

#include "grapple/status.h"

namespace grapple {
namespace ext {

// RAII owner for Grapple_TiledMap (destroyed with Grapple_FreeTiledMap).
class TiledMapHandle {
 public:
  static Result<TiledMapHandle> LoadTiledMap(const char *path) {
    Grapple_TiledMap* created_ = ::Grapple_LoadTiledMap(path);
    if (created_ == nullptr) {
      return Status::FromSdl();
    }
    return TiledMapHandle(created_);
  }

  TiledMapHandle() = default;
  ~TiledMapHandle() { reset(); }
  TiledMapHandle(TiledMapHandle&& other) noexcept
      : value_(other.value_), engaged_(other.engaged_) {
    other.value_ = nullptr;
    other.engaged_ = false;
  }
  TiledMapHandle& operator=(TiledMapHandle&& other) noexcept {
    if (this != &other) {
      reset();
      value_ = other.value_;
      engaged_ = other.engaged_;
      other.value_ = nullptr;
      other.engaged_ = false;
    }
    return *this;
  }
  TiledMapHandle(const TiledMapHandle&) = delete;
  TiledMapHandle& operator=(const TiledMapHandle&) = delete;

  Grapple_TiledMap* get() const { return value_; }
  Grapple_TiledMap* release() {
    Grapple_TiledMap* out = value_;
    value_ = nullptr;
    engaged_ = false;
    return out;
  }
  void reset() {
    if (value_ != nullptr) ::Grapple_FreeTiledMap(value_);
    value_ = nullptr;
    engaged_ = false;
  }

  int TiledMapWidth() {
    return ::Grapple_TiledMapWidth(value_);
  }
  int TiledMapHeight() {
    return ::Grapple_TiledMapHeight(value_);
  }
  int TiledTileWidth() {
    return ::Grapple_TiledTileWidth(value_);
  }
  int TiledTileHeight() {
    return ::Grapple_TiledTileHeight(value_);
  }
  int TiledLayerCount() {
    return ::Grapple_TiledLayerCount(value_);
  }
  const char* TiledLayerName(int idx) {
    return ::Grapple_TiledLayerName(value_, idx);
  }
  const char* TiledLayerType(int idx) {
    return ::Grapple_TiledLayerType(value_, idx);
  }
  int TiledTileAt(int layer, int x, int y) {
    return ::Grapple_TiledTileAt(value_, layer, x, y);
  }
  int TiledObjectCount(int layer) {
    return ::Grapple_TiledObjectCount(value_, layer);
  }
  Status TiledObjectAt(int layer, int index, Grapple_TiledObject *out) {
    return ::Grapple_TiledObjectAt(value_, layer, index, out) ? Status() : Status::FromSdl();
  }
  cute_tiled_map_t* TiledRaw() {
    return ::Grapple_TiledRaw(value_);
  }
 private:
  explicit TiledMapHandle(Grapple_TiledMap* value) : value_(value), engaged_(true) {}
  Grapple_TiledMap* value_{};
  bool engaged_ = false;
};

// RAII owner for Grapple_Gui (destroyed with Grapple_DestroyGui).
class GuiHandle {
 public:
  static Result<GuiHandle> CreateGui(SDL_Renderer *renderer, const void *font_data, size_t font_len, float font_size) {
    Grapple_Gui* created_ = ::Grapple_CreateGui(renderer, font_data, font_len, font_size);
    if (created_ == nullptr) {
      return Status::FromSdl();
    }
    return GuiHandle(created_);
  }

  GuiHandle() = default;
  ~GuiHandle() { reset(); }
  GuiHandle(GuiHandle&& other) noexcept
      : value_(other.value_), engaged_(other.engaged_) {
    other.value_ = nullptr;
    other.engaged_ = false;
  }
  GuiHandle& operator=(GuiHandle&& other) noexcept {
    if (this != &other) {
      reset();
      value_ = other.value_;
      engaged_ = other.engaged_;
      other.value_ = nullptr;
      other.engaged_ = false;
    }
    return *this;
  }
  GuiHandle(const GuiHandle&) = delete;
  GuiHandle& operator=(const GuiHandle&) = delete;

  Grapple_Gui* get() const { return value_; }
  Grapple_Gui* release() {
    Grapple_Gui* out = value_;
    value_ = nullptr;
    engaged_ = false;
    return out;
  }
  void reset() {
    if (value_ != nullptr) ::Grapple_DestroyGui(value_);
    value_ = nullptr;
    engaged_ = false;
  }

  SDL_Renderer* GuiRenderer() {
    return ::Grapple_GuiRenderer(value_);
  }
  struct nk_context* GuiContext() {
    return ::Grapple_GuiContext(value_);
  }
  void GuiInputBegin() { ::Grapple_GuiInputBegin(value_); }
  Status GuiProcessEvent(const SDL_Event *event) {
    return ::Grapple_GuiProcessEvent(value_, event) ? Status() : Status::FromSdl();
  }
  void GuiInputEnd() { ::Grapple_GuiInputEnd(value_); }
  Grapple_EventSink GuiEventSink() {
    return ::Grapple_GuiEventSink(value_);
  }
  Status GuiWantsInput() {
    return ::Grapple_GuiWantsInput(value_) ? Status() : Status::FromSdl();
  }
  Status GuiPumpEvents() {
    return ::Grapple_GuiPumpEvents(value_) ? Status() : Status::FromSdl();
  }
  Status GuiKeyPressed(int scancode) {
    return ::Grapple_GuiKeyPressed(value_, scancode) ? Status() : Status::FromSdl();
  }
  Status GuiOpenFileButton(const char *label, const char *filter_name, const char *filter_pattern) {
    return ::Grapple_GuiOpenFileButton(value_, label, filter_name, filter_pattern) ? Status() : Status::FromSdl();
  }
  Status GuiSaveFileButton(const char *label, const char *filename, const void *data, size_t len) {
    return ::Grapple_GuiSaveFileButton(value_, label, filename, data, len) ? Status() : Status::FromSdl();
  }
  const char* GuiSavedPath() {
    return ::Grapple_GuiSavedPath(value_);
  }
  Status GuiTooltip(const char *text) {
    return ::Grapple_GuiTooltip(value_, text) ? Status() : Status::FromSdl();
  }
  void GuiSetTooltipDelay(int delay_ms) { ::Grapple_GuiSetTooltipDelay(value_, delay_ms); }
  int GuiTooltipDelay() {
    return ::Grapple_GuiTooltipDelay(value_);
  }
  Status GuiGridWeight(int column, float weight) {
    return ::Grapple_GuiGridWeight(value_, column, weight) ? Status() : Status::FromSdl();
  }
  Status GuiGridBeginOwned(int columns, float row_height) {
    return ::Grapple_GuiGridBeginOwned(value_, columns, row_height) ? Status() : Status::FromSdl();
  }
  void GuiGridCellOwned() { ::Grapple_GuiGridCellOwned(value_); }
  void GuiGridCellSpanOwned(int span) { ::Grapple_GuiGridCellSpanOwned(value_, span); }
  void GuiGridRowHeightOwned(float height) { ::Grapple_GuiGridRowHeightOwned(value_, height); }
  void GuiGridSpacingOwned(float x, float y) { ::Grapple_GuiGridSpacingOwned(value_, x, y); }
  void GuiGridCellPartOwned(int span, float fraction, Grapple_GuiAlign align) { ::Grapple_GuiGridCellPartOwned(value_, span, fraction, align); }
  void GuiGridNextRowOwned() { ::Grapple_GuiGridNextRowOwned(value_); }
  void GuiGridEndOwned() { ::Grapple_GuiGridEndOwned(value_); }
  Status GuiImage(SDL_Texture *texture, Grapple_GuiImageMode mode) {
    return ::Grapple_GuiImage(value_, texture, mode) ? Status() : Status::FromSdl();
  }
  Status GuiDrawTexture(SDL_Texture *texture, SDL_FRect rect, Grapple_GuiImageMode mode) {
    return ::Grapple_GuiDrawTexture(value_, texture, rect, mode) ? Status() : Status::FromSdl();
  }
  Status GuiDrawTextureOverlay(SDL_Texture *texture, SDL_FRect rect, Grapple_GuiImageMode mode) {
    return ::Grapple_GuiDrawTextureOverlay(value_, texture, rect, mode) ? Status() : Status::FromSdl();
  }
  int GuiDrawCommandCount() {
    return ::Grapple_GuiDrawCommandCount(value_);
  }
  int GuiMemoryUsed() {
    return ::Grapple_GuiMemoryUsed(value_);
  }
  Status GuiSetFont(Grapple_GuiFontSize which) {
    return ::Grapple_GuiSetFont(value_, which) ? Status() : Status::FromSdl();
  }
  Status GuiPushFont(Grapple_GuiFontSize which) {
    return ::Grapple_GuiPushFont(value_, which) ? Status() : Status::FromSdl();
  }
  void GuiPopFont(int count) { ::Grapple_GuiPopFont(value_, count); }
  float GuiFontHeight() {
    return ::Grapple_GuiFontHeight(value_);
  }
  Status GuiPushStyleColor(Grapple_GuiStyleColor which, SDL_Color color) {
    return ::Grapple_GuiPushStyleColor(value_, which, color) ? Status() : Status::FromSdl();
  }
  void GuiPopStyleColor(int count) { ::Grapple_GuiPopStyleColor(value_, count); }
  float GuiScale() {
    return ::Grapple_GuiScale(value_);
  }
  Status GuiRender() {
    return ::Grapple_GuiRender(value_) ? Status() : Status::FromSdl();
  }
 private:
  explicit GuiHandle(Grapple_Gui* value) : value_(value), engaged_(true) {}
  Grapple_Gui* value_{};
  bool engaged_ = false;
};

// RAII owner for Grapple_Regex (destroyed with Grapple_DestroyRegex).
class RegexHandle {
 public:
  static Result<RegexHandle> CompileRegex(const char *pattern, const char *flags) {
    Grapple_Regex* created_ = ::Grapple_CompileRegex(pattern, flags);
    if (created_ == nullptr) {
      return Status::FromSdl();
    }
    return RegexHandle(created_);
  }

  RegexHandle() = default;
  ~RegexHandle() { reset(); }
  RegexHandle(RegexHandle&& other) noexcept
      : value_(other.value_), engaged_(other.engaged_) {
    other.value_ = nullptr;
    other.engaged_ = false;
  }
  RegexHandle& operator=(RegexHandle&& other) noexcept {
    if (this != &other) {
      reset();
      value_ = other.value_;
      engaged_ = other.engaged_;
      other.value_ = nullptr;
      other.engaged_ = false;
    }
    return *this;
  }
  RegexHandle(const RegexHandle&) = delete;
  RegexHandle& operator=(const RegexHandle&) = delete;

  Grapple_Regex* get() const { return value_; }
  Grapple_Regex* release() {
    Grapple_Regex* out = value_;
    value_ = nullptr;
    engaged_ = false;
    return out;
  }
  void reset() {
    if (value_ != nullptr) ::Grapple_DestroyRegex(value_);
    value_ = nullptr;
    engaged_ = false;
  }

  Status RegexSearch(const char *text, int start) {
    return ::Grapple_RegexSearch(value_, text, start) ? Status() : Status::FromSdl();
  }
  Status RegexMatchAt(const char *text, int start) {
    return ::Grapple_RegexMatchAt(value_, text, start) ? Status() : Status::FromSdl();
  }
  int RegexGroupCount() {
    return ::Grapple_RegexGroupCount(value_);
  }
  const char* RegexGroup(int group) {
    return ::Grapple_RegexGroup(value_, group);
  }
  int RegexGroupBegin(int group) {
    return ::Grapple_RegexGroupBegin(value_, group);
  }
  int RegexGroupEnd(int group) {
    return ::Grapple_RegexGroupEnd(value_, group);
  }
  int RegexNamedGroup(const char *name) {
    return ::Grapple_RegexNamedGroup(value_, name);
  }
  int RegexNamedGroupCount() {
    return ::Grapple_RegexNamedGroupCount(value_);
  }
  const char* RegexNamedGroupName(int index) {
    return ::Grapple_RegexNamedGroupName(value_, index);
  }
  const char* RegexPattern() {
    return ::Grapple_RegexPattern(value_);
  }
  const char* RegexFlags() {
    return ::Grapple_RegexFlags(value_);
  }
  const char* RegexReplace(const char *text, const char *replacement, bool all) {
    return ::Grapple_RegexReplace(value_, text, replacement, all);
  }
 private:
  explicit RegexHandle(Grapple_Regex* value) : value_(value), engaged_(true) {}
  Grapple_Regex* value_{};
  bool engaged_ = false;
};

// RAII owner for Grapple_LightScene (destroyed with Grapple_DestroyLightScene).
class LightSceneHandle {
 public:
  static Result<LightSceneHandle> CreateLightScene(SDL_Renderer *renderer) {
    Grapple_LightScene* created_ = ::Grapple_CreateLightScene(renderer);
    if (created_ == nullptr) {
      return Status::FromSdl();
    }
    return LightSceneHandle(created_);
  }

  LightSceneHandle() = default;
  ~LightSceneHandle() { reset(); }
  LightSceneHandle(LightSceneHandle&& other) noexcept
      : value_(other.value_), engaged_(other.engaged_) {
    other.value_ = nullptr;
    other.engaged_ = false;
  }
  LightSceneHandle& operator=(LightSceneHandle&& other) noexcept {
    if (this != &other) {
      reset();
      value_ = other.value_;
      engaged_ = other.engaged_;
      other.value_ = nullptr;
      other.engaged_ = false;
    }
    return *this;
  }
  LightSceneHandle(const LightSceneHandle&) = delete;
  LightSceneHandle& operator=(const LightSceneHandle&) = delete;

  Grapple_LightScene* get() const { return value_; }
  Grapple_LightScene* release() {
    Grapple_LightScene* out = value_;
    value_ = nullptr;
    engaged_ = false;
    return out;
  }
  void reset() {
    if (value_ != nullptr) ::Grapple_DestroyLightScene(value_);
    value_ = nullptr;
    engaged_ = false;
  }

  void LightBeginFrame(float camera_x, float camera_y) { ::Grapple_LightBeginFrame(value_, camera_x, camera_y); }
  void SetLightAmbient(SDL_FColor ambient) { ::Grapple_SetLightAmbient(value_, ambient); }
  Status AddLight(const Grapple_Light *light) {
    return ::Grapple_AddLight(value_, light) ? Status() : Status::FromSdl();
  }
  Status AddDarkZone(SDL_FRect rect, SDL_FColor ambient) {
    return ::Grapple_AddDarkZone(value_, rect, ambient) ? Status() : Status::FromSdl();
  }
  Status AddOccluderRect(SDL_FRect rect) {
    return ::Grapple_AddOccluderRect(value_, rect) ? Status() : Status::FromSdl();
  }
  Status AddOccluderSegment(float x1, float y1, float x2, float y2) {
    return ::Grapple_AddOccluderSegment(value_, x1, y1, x2, y2) ? Status() : Status::FromSdl();
  }
  Status RenderLighting() {
    return ::Grapple_RenderLighting(value_) ? Status() : Status::FromSdl();
  }
  float SampleLight(float x, float y) {
    return ::Grapple_SampleLight(value_, x, y);
  }
  Status LightLineOfSight(float x1, float y1, float x2, float y2) {
    return ::Grapple_LightLineOfSight(value_, x1, y1, x2, y2) ? Status() : Status::FromSdl();
  }
  Status LightUsesShaders() {
    return ::Grapple_LightUsesShaders(value_) ? Status() : Status::FromSdl();
  }
  void SetLightUseShaders(bool enabled) { ::Grapple_SetLightUseShaders(value_, enabled); }
  void SetLightMapScale(float scale) { ::Grapple_SetLightMapScale(value_, scale); }
  void SetLightShadowSoftness(float softness) { ::Grapple_SetLightShadowSoftness(value_, softness); }
  void SetLightRayCount(int rays) { ::Grapple_SetLightRayCount(value_, rays); }
  void SetLightRings(int rings) { ::Grapple_SetLightRings(value_, rings); }
  void SetLightDebugDraw(bool enabled) { ::Grapple_SetLightDebugDraw(value_, enabled); }
 private:
  explicit LightSceneHandle(Grapple_LightScene* value) : value_(value), engaged_(true) {}
  Grapple_LightScene* value_{};
  bool engaged_ = false;
};

// bool-returning C functions surfaced as Status.
inline Status ActionBind(Grapple_ActionMap *map, const char *action, Grapple_Binding binding) {
  return ::Grapple_ActionBind(map, action, binding) ? Status() : Status::FromSdl();
}
inline Status ActionBindAxis(Grapple_ActionMap *map, const char *action, Grapple_GamepadAxis axis, int half) {
  return ::Grapple_ActionBindAxis(map, action, axis, half) ? Status() : Status::FromSdl();
}
inline Status ActionBindDirection(Grapple_ActionMap *map, const char *action, Grapple_Direction direction) {
  return ::Grapple_ActionBindDirection(map, action, direction) ? Status() : Status::FromSdl();
}
inline Status ActionBindKey(Grapple_ActionMap *map, const char *action, SDL_Scancode key) {
  return ::Grapple_ActionBindKey(map, action, key) ? Status() : Status::FromSdl();
}
inline Status ActionBindKeySigned(Grapple_ActionMap *map, const char *action, SDL_Scancode key, int sign) {
  return ::Grapple_ActionBindKeySigned(map, action, key, sign) ? Status() : Status::FromSdl();
}
inline Status ActionBindMouse(Grapple_ActionMap *map, const char *action, Grapple_MouseButton button) {
  return ::Grapple_ActionBindMouse(map, action, button) ? Status() : Status::FromSdl();
}
inline Status ActionBindPad(Grapple_ActionMap *map, const char *action, Grapple_GamepadButton button) {
  return ::Grapple_ActionBindPad(map, action, button) ? Status() : Status::FromSdl();
}
inline Status ActionBindingAt(Grapple_ActionMap *map, const char *action, int index, Grapple_Binding *out) {
  return ::Grapple_ActionBindingAt(map, action, index, out) ? Status() : Status::FromSdl();
}
inline Status ActionCapture(Grapple_Engine *engine, int player, Grapple_Binding *out) {
  return ::Grapple_ActionCapture(engine, player, out) ? Status() : Status::FromSdl();
}
inline Status ActionDown(Grapple_Engine *engine, Grapple_ActionMap *map, int player, const char *action) {
  return ::Grapple_ActionDown(engine, map, player, action) ? Status() : Status::FromSdl();
}
inline Status ActionMapLoad(Grapple_ActionMap *map, const char *org, const char *app) {
  return ::Grapple_ActionMapLoad(map, org, app) ? Status() : Status::FromSdl();
}
inline Status ActionMapLoadToml(Grapple_ActionMap *map, const char *toml) {
  return ::Grapple_ActionMapLoadToml(map, toml) ? Status() : Status::FromSdl();
}
inline Status ActionMapSave(Grapple_ActionMap *map, const char *org, const char *app) {
  return ::Grapple_ActionMapSave(map, org, app) ? Status() : Status::FromSdl();
}
inline Status ActionPressed(Grapple_Engine *engine, Grapple_ActionMap *map, int player, const char *action) {
  return ::Grapple_ActionPressed(engine, map, player, action) ? Status() : Status::FromSdl();
}
inline Status ActionReleased(Grapple_Engine *engine, Grapple_ActionMap *map, int player, const char *action) {
  return ::Grapple_ActionReleased(engine, map, player, action) ? Status() : Status::FromSdl();
}
inline Status ActorAddBody(Grapple_Actor *actor, const Grapple_BodyDef *def) {
  return ::Grapple_ActorAddBody(actor, def) ? Status() : Status::FromSdl();
}
inline Status ActorAddLight(Grapple_Actor *actor, const Grapple_LightDef *light) {
  return ::Grapple_ActorAddLight(actor, light) ? Status() : Status::FromSdl();
}
inline Status ActorAlive(Grapple_Engine *engine, Grapple_ActorId id) {
  return ::Grapple_ActorAlive(engine, id) ? Status() : Status::FromSdl();
}
inline Status ActorBodyBounds(Grapple_Actor *actor, SDL_FRect *out) {
  return ::Grapple_ActorBodyBounds(actor, out) ? Status() : Status::FromSdl();
}
inline Status ActorEnabled(Grapple_Actor *actor) {
  return ::Grapple_ActorEnabled(actor) ? Status() : Status::FromSdl();
}
inline Status ActorHasBody(Grapple_Actor *actor) {
  return ::Grapple_ActorHasBody(actor) ? Status() : Status::FromSdl();
}
inline Status ActorHasTags(Grapple_Actor *actor, Grapple_ActorTags any) {
  return ::Grapple_ActorHasTags(actor, any) ? Status() : Status::FromSdl();
}
inline Status ActorSend(Grapple_Engine *engine, Grapple_ActorId target, const Grapple_ActorMessage *message) {
  return ::Grapple_ActorSend(engine, target, message) ? Status() : Status::FromSdl();
}
inline Status ActorSetParent(Grapple_Actor *actor, Grapple_ActorId parent) {
  return ::Grapple_ActorSetParent(actor, parent) ? Status() : Status::FromSdl();
}
inline Status ActorSetSprite(Grapple_Actor *actor, const Grapple_Sprite *sprite) {
  return ::Grapple_ActorSetSprite(actor, sprite) ? Status() : Status::FromSdl();
}
inline Status AnyInput(Grapple_Engine *engine) {
  return ::Grapple_AnyInput(engine) ? Status() : Status::FromSdl();
}
inline Status AssetsReady(Grapple_Engine *engine) {
  return ::Grapple_AssetsReady(engine) ? Status() : Status::FromSdl();
}
inline Status BidiBaseIsRTL(const char *utf8, int length) {
  return ::Grapple_BidiBaseIsRTL(utf8, length) ? Status() : Status::FromSdl();
}
inline Status BindingFromString(const char *text, Grapple_Binding *out) {
  return ::Grapple_BindingFromString(text, out) ? Status() : Status::FromSdl();
}
inline Status CameraBegin(Grapple_Engine *engine, const Grapple_Camera *camera) {
  return ::Grapple_CameraBegin(engine, camera) ? Status() : Status::FromSdl();
}
inline Status CameraScreenToWorld(const Grapple_Camera *camera, float screen_x, float screen_y, float *world_x, float *world_y) {
  return ::Grapple_CameraScreenToWorld(camera, screen_x, screen_y, world_x, world_y) ? Status() : Status::FromSdl();
}
inline Status CameraVisible(const Grapple_Camera *camera, SDL_FRect world) {
  return ::Grapple_CameraVisible(camera, world) ? Status() : Status::FromSdl();
}
inline Status CryptoSelfTest() {
  return ::Grapple_CryptoSelfTest() ? Status() : Status::FromSdl();
}
inline Status DialogDeliverSave(const char *path) {
  return ::Grapple_DialogDeliverSave(path) ? Status() : Status::FromSdl();
}
inline Status DisconnectSignal(Grapple_SignalEmitter *emitter, Uint64 connection) {
  return ::Grapple_DisconnectSignal(emitter, connection) ? Status() : Status::FromSdl();
}
inline Status DrawPhysicsWorld(b2WorldId world, SDL_Renderer *renderer, const Grapple_PhysicsDrawConfig *config) {
  return ::Grapple_DrawPhysicsWorld(world, renderer, config) ? Status() : Status::FromSdl();
}
inline Status EngineEffectsAvailable(Grapple_Engine *engine) {
  return ::Grapple_EngineEffectsAvailable(engine) ? Status() : Status::FromSdl();
}
inline Status EngineSetDisplay(Grapple_Engine *engine, int index) {
  return ::Grapple_EngineSetDisplay(engine, index) ? Status() : Status::FromSdl();
}
inline Status EngineSetGraphics(Grapple_Engine *engine, const Grapple_GraphicsSettings *settings) {
  return ::Grapple_EngineSetGraphics(engine, settings) ? Status() : Status::FromSdl();
}
inline Status EngineSetPresentation(Grapple_Engine *engine, Grapple_EnginePresentation mode) {
  return ::Grapple_EngineSetPresentation(engine, mode) ? Status() : Status::FromSdl();
}
inline Status EngineSetTickRate(Grapple_Engine *engine, int ticks_per_second) {
  return ::Grapple_EngineSetTickRate(engine, ticks_per_second) ? Status() : Status::FromSdl();
}
inline Status EngineTick(Grapple_Engine *engine) {
  return ::Grapple_EngineTick(engine) ? Status() : Status::FromSdl();
}
inline Status EventKeyRepeat(SDL_Event *event) {
  return ::Grapple_EventKeyRepeat(event) ? Status() : Status::FromSdl();
}
inline Status GPUComputeBindingsAddBuffer(Grapple_GPUComputeBindings *bindings, SDL_GPUBuffer *buffer, bool cycle) {
  return ::Grapple_GPUComputeBindingsAddBuffer(bindings, buffer, cycle) ? Status() : Status::FromSdl();
}
inline Status GPUComputeBindingsAddTexture(Grapple_GPUComputeBindings *bindings, SDL_GPUTexture *texture, Uint32 mip_level, Uint32 layer, bool cycle) {
  return ::Grapple_GPUComputeBindingsAddTexture(bindings, texture, mip_level, layer, cycle) ? Status() : Status::FromSdl();
}
inline Status GPUPipelineInfoAddColorTarget(SDL_GPUGraphicsPipelineCreateInfo *info, SDL_GPUTextureFormat format) {
  return ::Grapple_GPUPipelineInfoAddColorTarget(info, format) ? Status() : Status::FromSdl();
}
inline Status GPUPipelineInfoAddVertexAttribute(SDL_GPUGraphicsPipelineCreateInfo *info, Uint32 location, Uint32 buffer_slot, SDL_GPUVertexElementFormat format, Uint32 offset) {
  return ::Grapple_GPUPipelineInfoAddVertexAttribute(info, location, buffer_slot, format, offset) ? Status() : Status::FromSdl();
}
inline Status GPUPipelineInfoAddVertexBuffer(SDL_GPUGraphicsPipelineCreateInfo *info, Uint32 slot, Uint32 pitch, SDL_GPUVertexInputRate input_rate) {
  return ::Grapple_GPUPipelineInfoAddVertexBuffer(info, slot, pitch, input_rate) ? Status() : Status::FromSdl();
}
inline Status GPURenderStateInfoAddSampler(SDL_GPURenderStateCreateInfo *info, SDL_GPUTexture *texture, SDL_GPUSampler *sampler) {
  return ::Grapple_GPURenderStateInfoAddSampler(info, texture, sampler) ? Status() : Status::FromSdl();
}
inline Status GPUUploadToTransferBuffer(SDL_GPUDevice *device, SDL_GPUTransferBuffer *buffer, Uint32 offset, const void *data, int size, bool cycle) {
  return ::Grapple_GPUUploadToTransferBuffer(device, buffer, offset, data, size, cycle) ? Status() : Status::FromSdl();
}
inline Status GPUWaitForFence(SDL_GPUDevice *device, SDL_GPUFence *fence) {
  return ::Grapple_GPUWaitForFence(device, fence) ? Status() : Status::FromSdl();
}
inline Status GamepadButtonDown(Grapple_Engine *engine, int player, Grapple_GamepadButton button) {
  return ::Grapple_GamepadButtonDown(engine, player, button) ? Status() : Status::FromSdl();
}
inline Status GamepadButtonPressed(Grapple_Engine *engine, int player, Grapple_GamepadButton button) {
  return ::Grapple_GamepadButtonPressed(engine, player, button) ? Status() : Status::FromSdl();
}
inline Status GamepadButtonReleased(Grapple_Engine *engine, int player, Grapple_GamepadButton button) {
  return ::Grapple_GamepadButtonReleased(engine, player, button) ? Status() : Status::FromSdl();
}
inline Status GamepadConnected(Grapple_Engine *engine, int player) {
  return ::Grapple_GamepadConnected(engine, player) ? Status() : Status::FromSdl();
}
inline Status GamepadDirectionPressed(Grapple_Engine *engine, int player, Grapple_Direction direction) {
  return ::Grapple_GamepadDirectionPressed(engine, player, direction) ? Status() : Status::FromSdl();
}
inline Status GamepadDirectionRepeat(Grapple_Engine *engine, int player, Grapple_Direction direction) {
  return ::Grapple_GamepadDirectionRepeat(engine, player, direction) ? Status() : Status::FromSdl();
}
inline Status GamepadHasAccelerometer(Grapple_Engine *engine, int player) {
  return ::Grapple_GamepadHasAccelerometer(engine, player) ? Status() : Status::FromSdl();
}
inline Status GamepadHasGyro(Grapple_Engine *engine, int player) {
  return ::Grapple_GamepadHasGyro(engine, player) ? Status() : Status::FromSdl();
}
inline Status GamepadRumble(Grapple_Engine *engine, int player, float low, float high, Uint32 milliseconds) {
  return ::Grapple_GamepadRumble(engine, player, low, high, milliseconds) ? Status() : Status::FromSdl();
}
inline Status GamepadRumbleTriggers(Grapple_Engine *engine, int player, float left, float right, Uint32 milliseconds) {
  return ::Grapple_GamepadRumbleTriggers(engine, player, left, right, milliseconds) ? Status() : Status::FromSdl();
}
inline Status GamepadSetLED(Grapple_Engine *engine, int player, Uint8 red, Uint8 green, Uint8 blue) {
  return ::Grapple_GamepadSetLED(engine, player, red, green, blue) ? Status() : Status::FromSdl();
}
inline Status GraphicsEqual(const Grapple_GraphicsSettings *a, const Grapple_GraphicsSettings *b) {
  return ::Grapple_GraphicsEqual(a, b) ? Status() : Status::FromSdl();
}
inline Status GraphicsLoadTomlFile(Grapple_GraphicsSettings *settings, const char *path) {
  return ::Grapple_GraphicsLoadTomlFile(settings, path) ? Status() : Status::FromSdl();
}
inline Status GraphicsLoadTomlString(Grapple_GraphicsSettings *settings, const char *toml) {
  return ::Grapple_GraphicsLoadTomlString(settings, toml) ? Status() : Status::FromSdl();
}
inline Status GraphicsQualityFromName(const char *name, Grapple_GraphicsQuality *out) {
  return ::Grapple_GraphicsQualityFromName(name, out) ? Status() : Status::FromSdl();
}
inline Status GraphicsSave(const Grapple_GraphicsSettings *settings, const char *org, const char *app) {
  return ::Grapple_GraphicsSave(settings, org, app) ? Status() : Status::FromSdl();
}
inline Status GuiGridBegin(struct nk_context *ctx, Grapple_GuiGrid *grid, int columns, const float *weights, float row_height) {
  return ::Grapple_GuiGridBegin(ctx, grid, columns, weights, row_height) ? Status() : Status::FromSdl();
}
inline Status HMACSHA256(const void *key, size_t keySize, const void *data, size_t dataSize, Uint8 digest[32]) {
  return ::Grapple_HMACSHA256(key, keySize, data, dataSize, digest) ? Status() : Status::FromSdl();
}
inline Status HasDeviceMotion(Grapple_Engine *engine) {
  return ::Grapple_HasDeviceMotion(engine) ? Status() : Status::FromSdl();
}
inline Status KeyDown(Grapple_Engine *engine, SDL_Scancode key) {
  return ::Grapple_KeyDown(engine, key) ? Status() : Status::FromSdl();
}
inline Status KeyPressed(Grapple_Engine *engine, SDL_Scancode key) {
  return ::Grapple_KeyPressed(engine, key) ? Status() : Status::FromSdl();
}
inline Status KeyReleased(Grapple_Engine *engine, SDL_Scancode key) {
  return ::Grapple_KeyReleased(engine, key) ? Status() : Status::FromSdl();
}
inline Status LightRender(Grapple_Engine *engine, const Grapple_Camera *camera, float alpha) {
  return ::Grapple_LightRender(engine, camera, alpha) ? Status() : Status::FromSdl();
}
inline Status MountEncryptedArchive(const void *data, int dataSize, const char *password, const char *mountPoint) {
  return ::Grapple_MountEncryptedArchive(data, dataSize, password, mountPoint) ? Status() : Status::FromSdl();
}
inline Status MountEncryptedArchiveFile(const char *path, const char *password, const char *mountPoint) {
  return ::Grapple_MountEncryptedArchiveFile(path, password, mountPoint) ? Status() : Status::FromSdl();
}
inline Status MouseCaptured(Grapple_Engine *engine) {
  return ::Grapple_MouseCaptured(engine) ? Status() : Status::FromSdl();
}
inline Status MouseDown(Grapple_Engine *engine, Grapple_MouseButton button) {
  return ::Grapple_MouseDown(engine, button) ? Status() : Status::FromSdl();
}
inline Status MousePressed(Grapple_Engine *engine, Grapple_MouseButton button) {
  return ::Grapple_MousePressed(engine, button) ? Status() : Status::FromSdl();
}
inline Status MouseReleased(Grapple_Engine *engine, Grapple_MouseButton button) {
  return ::Grapple_MouseReleased(engine, button) ? Status() : Status::FromSdl();
}
inline Status PhysicsPaused(Grapple_Engine *engine) {
  return ::Grapple_PhysicsPaused(engine) ? Status() : Status::FromSdl();
}
inline Status RenderDebugText(SDL_Renderer *renderer, float x, float y, const char *text) {
  return ::Grapple_RenderDebugText(renderer, x, y, text) ? Status() : Status::FromSdl();
}
inline Status RunGame(Grapple_Engine *engine, const Grapple_GameHooks *hooks, void *user) {
  return ::Grapple_RunGame(engine, hooks, user) ? Status() : Status::FromSdl();
}
inline Status SHA256(const void *data, size_t dataSize, Uint8 digest[32]) {
  return ::Grapple_SHA256(data, dataSize, digest) ? Status() : Status::FromSdl();
}
inline Status SaveDelete(Grapple_Engine *engine, int slot) {
  return ::Grapple_SaveDelete(engine, slot) ? Status() : Status::FromSdl();
}
inline Status SaveExists(Grapple_Engine *engine, int slot) {
  return ::Grapple_SaveExists(engine, slot) ? Status() : Status::FromSdl();
}
inline Status SaveWrite(Grapple_Engine *engine, int slot, const void *data, size_t size, const char *label) {
  return ::Grapple_SaveWrite(engine, slot, data, size, label) ? Status() : Status::FromSdl();
}
inline Status SceneIsActive(Grapple_Scene *scene) {
  return ::Grapple_SceneIsActive(scene) ? Status() : Status::FromSdl();
}
inline Status ScenePop(Grapple_Engine *engine) {
  return ::Grapple_ScenePop(engine) ? Status() : Status::FromSdl();
}
inline Status ScenePush(Grapple_Engine *engine, const Grapple_SceneDef *def) {
  return ::Grapple_ScenePush(engine, def) ? Status() : Status::FromSdl();
}
inline Status SceneReplace(Grapple_Engine *engine, const Grapple_SceneDef *def) {
  return ::Grapple_SceneReplace(engine, def) ? Status() : Status::FromSdl();
}
inline Status SceneReset(Grapple_Engine *engine, const Grapple_SceneDef *def) {
  return ::Grapple_SceneReset(engine, def) ? Status() : Status::FromSdl();
}
inline Status SceneTransitionTo(Grapple_Engine *engine, const Grapple_SceneDef *def, Grapple_SceneTransition transition, float seconds) {
  return ::Grapple_SceneTransitionTo(engine, def, transition, seconds) ? Status() : Status::FromSdl();
}
inline Status SceneTransitioning(Grapple_Engine *engine) {
  return ::Grapple_SceneTransitioning(engine) ? Status() : Status::FromSdl();
}
inline Status ScriptBind(Grapple_Engine *engine, void *language_state, Grapple_ScriptDispatch dispatch, Grapple_ScriptRelease release) {
  return ::Grapple_ScriptBind(engine, language_state, dispatch, release) ? Status() : Status::FromSdl();
}
inline Status ScriptHasHandlers(Grapple_Engine *engine) {
  return ::Grapple_ScriptHasHandlers(engine) ? Status() : Status::FromSdl();
}
inline Status ScriptRun(Grapple_Engine *engine) {
  return ::Grapple_ScriptRun(engine) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneDefine(Grapple_Engine *engine, const char *name) {
  return ::Grapple_ScriptSceneDefine(engine, name) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneDefined(Grapple_Engine *engine, const char *name) {
  return ::Grapple_ScriptSceneDefined(engine, name) ? Status() : Status::FromSdl();
}
inline Status ScriptScenePush(Grapple_Engine *engine, const char *name) {
  return ::Grapple_ScriptScenePush(engine, name) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneReplace(Grapple_Engine *engine, const char *name) {
  return ::Grapple_ScriptSceneReplace(engine, name) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneReset(Grapple_Engine *engine, const char *name) {
  return ::Grapple_ScriptSceneReset(engine, name) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneSetFlags(Grapple_Engine *engine, const char *name, Grapple_SceneFlags flags) {
  return ::Grapple_ScriptSceneSetFlags(engine, name, flags) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneSetHook(Grapple_Engine *engine, const char *name, Grapple_SceneHook hook, Sint64 handle) {
  return ::Grapple_ScriptSceneSetHook(engine, name, hook, handle) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneSetStateSize(Grapple_Engine *engine, const char *name, int state_size) {
  return ::Grapple_ScriptSceneSetStateSize(engine, name, state_size) ? Status() : Status::FromSdl();
}
inline Status ScriptSceneTransitionTo(Grapple_Engine *engine, const char *name, Grapple_SceneTransition transition, float seconds) {
  return ::Grapple_ScriptSceneTransitionTo(engine, name, transition, seconds) ? Status() : Status::FromSdl();
}
inline Status ScriptSetHook(Grapple_Engine *engine, Grapple_ScriptHook hook, Sint64 handle) {
  return ::Grapple_ScriptSetHook(engine, hook, handle) ? Status() : Status::FromSdl();
}
inline Status ScriptSetSceneDispatch(Grapple_Engine *engine, Grapple_ScriptSceneDispatch dispatch) {
  return ::Grapple_ScriptSetSceneDispatch(engine, dispatch) ? Status() : Status::FromSdl();
}
inline Status SetDeviceMotion(Grapple_Engine *engine, bool enabled) {
  return ::Grapple_SetDeviceMotion(engine, enabled) ? Status() : Status::FromSdl();
}
inline Status SetGamepadMotion(Grapple_Engine *engine, int player, bool enabled) {
  return ::Grapple_SetGamepadMotion(engine, player, enabled) ? Status() : Status::FromSdl();
}
inline Status SetMouseCapture(Grapple_Engine *engine, bool captured) {
  return ::Grapple_SetMouseCapture(engine, captured) ? Status() : Status::FromSdl();
}
inline Status ShowOpenFileDialog(SDL_Window *window, const char *filter_name, const char *filter_pattern, const char *default_location) {
  return ::Grapple_ShowOpenFileDialog(window, filter_name, filter_pattern, default_location) ? Status() : Status::FromSdl();
}
inline Status ShowSaveFileDialog(SDL_Window *window, const char *filter_name, const char *filter_pattern, const char *default_location) {
  return ::Grapple_ShowSaveFileDialog(window, filter_name, filter_pattern, default_location) ? Status() : Status::FromSdl();
}
inline Status TextHas(Grapple_Engine *engine, const char *key) {
  return ::Grapple_TextHas(engine, key) ? Status() : Status::FromSdl();
}
inline Status TextLoad(Grapple_Engine *engine, const char *language, const char *toml) {
  return ::Grapple_TextLoad(engine, language, toml) ? Status() : Status::FromSdl();
}
inline Status TextLoadFile(Grapple_Engine *engine, const char *language) {
  return ::Grapple_TextLoadFile(engine, language) ? Status() : Status::FromSdl();
}

// Everything else, aliased into the namespace unchanged.
inline constexpr auto& ActionBindingCount = ::Grapple_ActionBindingCount;
inline constexpr auto& ActionClear = ::Grapple_ActionClear;
inline constexpr auto& ActionCount = ::Grapple_ActionCount;
inline constexpr auto& ActionMapCreate = ::Grapple_ActionMapCreate;
inline constexpr auto& ActionMapDestroy = ::Grapple_ActionMapDestroy;
inline constexpr auto& ActionMapKeyboardPlayer = ::Grapple_ActionMapKeyboardPlayer;
inline constexpr auto& ActionMapSetKeyboardPlayer = ::Grapple_ActionMapSetKeyboardPlayer;
inline constexpr auto& ActionMapToToml = ::Grapple_ActionMapToToml;
inline constexpr auto& ActionName = ::Grapple_ActionName;
inline constexpr auto& ActionValue = ::Grapple_ActionValue;
inline constexpr auto& ActionVector = ::Grapple_ActionVector;
inline constexpr auto& ActorAngularVelocity = ::Grapple_ActorAngularVelocity;
inline constexpr auto& ActorApplyForce = ::Grapple_ActorApplyForce;
inline constexpr auto& ActorApplyImpulse = ::Grapple_ActorApplyImpulse;
inline constexpr auto& ActorBroadcast = ::Grapple_ActorBroadcast;
inline constexpr auto& ActorChild = ::Grapple_ActorChild;
inline constexpr auto& ActorChildCount = ::Grapple_ActorChildCount;
inline constexpr auto& ActorClear = ::Grapple_ActorClear;
inline constexpr auto& ActorClearSprite = ::Grapple_ActorClearSprite;
inline constexpr auto& ActorCount = ::Grapple_ActorCount;
inline constexpr auto& ActorDefCreate = ::Grapple_ActorDefCreate;
inline constexpr auto& ActorDefDestroy = ::Grapple_ActorDefDestroy;
inline constexpr auto& ActorDefSetName = ::Grapple_ActorDefSetName;
inline constexpr auto& ActorDefSetParent = ::Grapple_ActorDefSetParent;
inline constexpr auto& ActorDefSetPosition = ::Grapple_ActorDefSetPosition;
inline constexpr auto& ActorDefSetRotation = ::Grapple_ActorDefSetRotation;
inline constexpr auto& ActorDefSetScale = ::Grapple_ActorDefSetScale;
inline constexpr auto& ActorDefSetStateSize = ::Grapple_ActorDefSetStateSize;
inline constexpr auto& ActorDefSetTags = ::Grapple_ActorDefSetTags;
inline constexpr auto& ActorDefSetType = ::Grapple_ActorDefSetType;
inline constexpr auto& ActorDestroy = ::Grapple_ActorDestroy;
inline constexpr auto& ActorEach = ::Grapple_ActorEach;
inline constexpr auto& ActorEngine = ::Grapple_ActorEngine;
inline constexpr auto& ActorFindByName = ::Grapple_ActorFindByName;
inline constexpr auto& ActorFindByType = ::Grapple_ActorFindByType;
inline constexpr auto& ActorGet = ::Grapple_ActorGet;
inline constexpr auto& ActorGetId = ::Grapple_ActorGetId;
inline constexpr auto& ActorGetTags = ::Grapple_ActorGetTags;
inline constexpr auto& ActorLight = ::Grapple_ActorLight;
inline constexpr auto& ActorLocal = ::Grapple_ActorLocal;
inline constexpr auto& ActorMove = ::Grapple_ActorMove;
inline constexpr auto& ActorName = ::Grapple_ActorName;
inline constexpr auto& ActorParent = ::Grapple_ActorParent;
inline constexpr auto& ActorQuery = ::Grapple_ActorQuery;
inline constexpr auto& ActorRemoveBody = ::Grapple_ActorRemoveBody;
inline constexpr auto& ActorRemoveLight = ::Grapple_ActorRemoveLight;
inline constexpr auto& ActorRenderTransform = ::Grapple_ActorRenderTransform;
inline constexpr auto& ActorSetAngularVelocity = ::Grapple_ActorSetAngularVelocity;
inline constexpr auto& ActorSetEnabled = ::Grapple_ActorSetEnabled;
inline constexpr auto& ActorSetLocal = ::Grapple_ActorSetLocal;
inline constexpr auto& ActorSetPosition = ::Grapple_ActorSetPosition;
inline constexpr auto& ActorSetTags = ::Grapple_ActorSetTags;
inline constexpr auto& ActorSetVelocity = ::Grapple_ActorSetVelocity;
inline constexpr auto& ActorSpawn = ::Grapple_ActorSpawn;
inline constexpr auto& ActorSprite = ::Grapple_ActorSprite;
inline constexpr auto& ActorState = ::Grapple_ActorState;
inline constexpr auto& ActorTeleport = ::Grapple_ActorTeleport;
inline constexpr auto& ActorTeleportBody = ::Grapple_ActorTeleportBody;
inline constexpr auto& ActorType = ::Grapple_ActorType;
inline constexpr auto& ActorVelocity = ::Grapple_ActorVelocity;
inline constexpr auto& ActorWakeBody = ::Grapple_ActorWakeBody;
inline constexpr auto& ActorWorld = ::Grapple_ActorWorld;
inline constexpr auto& AssetPath = ::Grapple_AssetPath;
inline constexpr auto& AssetRelease = ::Grapple_AssetRelease;
inline constexpr auto& AssetRetain = ::Grapple_AssetRetain;
inline constexpr auto& AssetStatusOf = ::Grapple_AssetStatusOf;
inline constexpr auto& AssetsFrameBudget = ::Grapple_AssetsFrameBudget;
inline constexpr auto& AssetsLoaded = ::Grapple_AssetsLoaded;
inline constexpr auto& AssetsPending = ::Grapple_AssetsPending;
inline constexpr auto& AssetsProgress = ::Grapple_AssetsProgress;
inline constexpr auto& AssetsSetFrameBudget = ::Grapple_AssetsSetFrameBudget;
inline constexpr auto& AssetsSetWorkers = ::Grapple_AssetsSetWorkers;
inline constexpr auto& AssetsWait = ::Grapple_AssetsWait;
inline constexpr auto& BidiItemize = ::Grapple_BidiItemize;
inline constexpr auto& BindingToString = ::Grapple_BindingToString;
inline constexpr auto& BodyDefCreate = ::Grapple_BodyDefCreate;
inline constexpr auto& BodyDefDestroy = ::Grapple_BodyDefDestroy;
inline constexpr auto& BodyDefSetBullet = ::Grapple_BodyDefSetBullet;
inline constexpr auto& BodyDefSetFilter = ::Grapple_BodyDefSetFilter;
inline constexpr auto& BodyDefSetFixedRotation = ::Grapple_BodyDefSetFixedRotation;
inline constexpr auto& BodyDefSetGravityScale = ::Grapple_BodyDefSetGravityScale;
inline constexpr auto& BodyDefSetMaterial = ::Grapple_BodyDefSetMaterial;
inline constexpr auto& BodyDefSetOffset = ::Grapple_BodyDefSetOffset;
inline constexpr auto& BodyDefSetSensor = ::Grapple_BodyDefSetSensor;
inline constexpr auto& BodyDefSetShape = ::Grapple_BodyDefSetShape;
inline constexpr auto& BodyDefSetSize = ::Grapple_BodyDefSetSize;
inline constexpr auto& BodyDefSetType = ::Grapple_BodyDefSetType;
inline constexpr auto& BodyDefault = ::Grapple_BodyDefault;
inline constexpr auto& CameraCreate = ::Grapple_CameraCreate;
inline constexpr auto& CameraDestroy = ::Grapple_CameraDestroy;
inline constexpr auto& CameraEnd = ::Grapple_CameraEnd;
inline constexpr auto& CameraFollow = ::Grapple_CameraFollow;
inline constexpr auto& CameraInit = ::Grapple_CameraInit;
inline constexpr auto& CameraPoint = ::Grapple_CameraPoint;
inline constexpr auto& CameraRect = ::Grapple_CameraRect;
inline constexpr auto& CameraSetBounds = ::Grapple_CameraSetBounds;
inline constexpr auto& CameraSetDeadzone = ::Grapple_CameraSetDeadzone;
inline constexpr auto& CameraSetSmoothing = ::Grapple_CameraSetSmoothing;
inline constexpr auto& CameraSetViewport = ::Grapple_CameraSetViewport;
inline constexpr auto& CameraSetZoom = ::Grapple_CameraSetZoom;
inline constexpr auto& CameraShake = ::Grapple_CameraShake;
inline constexpr auto& CameraSnap = ::Grapple_CameraSnap;
inline constexpr auto& CameraSplit = ::Grapple_CameraSplit;
inline constexpr auto& CameraUpdate = ::Grapple_CameraUpdate;
inline constexpr auto& CameraX = ::Grapple_CameraX;
inline constexpr auto& CameraY = ::Grapple_CameraY;
inline constexpr auto& CompressData = ::Grapple_CompressData;
inline constexpr auto& ConfigCreate = ::Grapple_ConfigCreate;
inline constexpr auto& ConfigDestroy = ::Grapple_ConfigDestroy;
inline constexpr auto& ConfigSetAutoMount = ::Grapple_ConfigSetAutoMount;
inline constexpr auto& ConfigSetBackend = ::Grapple_ConfigSetBackend;
inline constexpr auto& ConfigSetDesignSize = ::Grapple_ConfigSetDesignSize;
inline constexpr auto& ConfigSetFullscreen = ::Grapple_ConfigSetFullscreen;
inline constexpr auto& ConfigSetGraphics = ::Grapple_ConfigSetGraphics;
inline constexpr auto& ConfigSetHeadless = ::Grapple_ConfigSetHeadless;
inline constexpr auto& ConfigSetHighDpi = ::Grapple_ConfigSetHighDpi;
inline constexpr auto& ConfigSetManualClock = ::Grapple_ConfigSetManualClock;
inline constexpr auto& ConfigSetMaxFps = ::Grapple_ConfigSetMaxFps;
inline constexpr auto& ConfigSetMediaPath = ::Grapple_ConfigSetMediaPath;
inline constexpr auto& ConfigSetPresentation = ::Grapple_ConfigSetPresentation;
inline constexpr auto& ConfigSetResizable = ::Grapple_ConfigSetResizable;
inline constexpr auto& ConfigSetTickRate = ::Grapple_ConfigSetTickRate;
inline constexpr auto& ConfigSetTitle = ::Grapple_ConfigSetTitle;
inline constexpr auto& ConfigSetVsync = ::Grapple_ConfigSetVsync;
inline constexpr auto& ConfigSetWindowSize = ::Grapple_ConfigSetWindowSize;
inline constexpr auto& ConnectSignal = ::Grapple_ConnectSignal;
inline constexpr auto& CountSignalConnections = ::Grapple_CountSignalConnections;
inline constexpr auto& CreateChipSFX = ::Grapple_CreateChipSFX;
inline constexpr auto& CreateChipTone = ::Grapple_CreateChipTone;
inline constexpr auto& CreateChipTune = ::Grapple_CreateChipTune;
inline constexpr auto& CreateEngine = ::Grapple_CreateEngine;
inline constexpr auto& CreateGuiWithGlyphs = ::Grapple_CreateGuiWithGlyphs;
inline constexpr auto& CreateSignalEmitter = ::Grapple_CreateSignalEmitter;
inline constexpr auto& DayNightAmbient = ::Grapple_DayNightAmbient;
inline constexpr auto& DayNightSunlight = ::Grapple_DayNightSunlight;
inline constexpr auto& DecodeDataBase64 = ::Grapple_DecodeDataBase64;
inline constexpr auto& DecompressData = ::Grapple_DecompressData;
inline constexpr auto& DecryptData = ::Grapple_DecryptData;
inline constexpr auto& DestroyEngine = ::Grapple_DestroyEngine;
inline constexpr auto& DestroySignalEmitter = ::Grapple_DestroySignalEmitter;
inline constexpr auto& DeviceAccelerometer = ::Grapple_DeviceAccelerometer;
inline constexpr auto& DeviceGyro = ::Grapple_DeviceGyro;
inline constexpr auto& DialogPath = ::Grapple_DialogPath;
inline constexpr auto& DialogReset = ::Grapple_DialogReset;
inline constexpr auto& DialogStatus = ::Grapple_DialogStatus;
inline constexpr auto& DistanceJointDefCreate = ::Grapple_DistanceJointDefCreate;
inline constexpr auto& DistanceJointDefDestroy = ::Grapple_DistanceJointDefDestroy;
inline constexpr auto& DistanceJointDefSetAnchors = ::Grapple_DistanceJointDefSetAnchors;
inline constexpr auto& DistanceJointDefSetBodies = ::Grapple_DistanceJointDefSetBodies;
inline constexpr auto& DistanceJointDefSetCollideConnected = ::Grapple_DistanceJointDefSetCollideConnected;
inline constexpr auto& DistanceJointDefSetLength = ::Grapple_DistanceJointDefSetLength;
inline constexpr auto& DistanceJointDefSetLimit = ::Grapple_DistanceJointDefSetLimit;
inline constexpr auto& DistanceJointDefSetSpring = ::Grapple_DistanceJointDefSetSpring;
inline constexpr auto& EmitSignal = ::Grapple_EmitSignal;
inline constexpr auto& EncodeDataBase64 = ::Grapple_EncodeDataBase64;
inline constexpr auto& EncryptData = ::Grapple_EncryptData;
inline constexpr auto& EngineAdvance = ::Grapple_EngineAdvance;
inline constexpr auto& EngineAlpha = ::Grapple_EngineAlpha;
inline constexpr auto& EngineAssetScale = ::Grapple_EngineAssetScale;
inline constexpr auto& EngineDelta = ::Grapple_EngineDelta;
inline constexpr auto& EngineDesignSize = ::Grapple_EngineDesignSize;
inline constexpr auto& EngineDisplay = ::Grapple_EngineDisplay;
inline constexpr auto& EngineDisplayCount = ::Grapple_EngineDisplayCount;
inline constexpr auto& EngineDisplayName = ::Grapple_EngineDisplayName;
inline constexpr auto& EngineEmbedMedia = ::Grapple_EngineEmbedMedia;
inline constexpr auto& EngineFps = ::Grapple_EngineFps;
inline constexpr auto& EngineFrameCount = ::Grapple_EngineFrameCount;
inline constexpr auto& EngineGraphics = ::Grapple_EngineGraphics;
inline constexpr auto& EngineMaxFps = ::Grapple_EngineMaxFps;
inline constexpr auto& EngineMediaPath = ::Grapple_EngineMediaPath;
inline constexpr auto& EngineMediaSource = ::Grapple_EngineMediaSource;
inline constexpr auto& EngineOverloadFrames = ::Grapple_EngineOverloadFrames;
inline constexpr auto& EnginePixelSize = ::Grapple_EnginePixelSize;
inline constexpr auto& EnginePresentation_ = ::Grapple_EnginePresentation_;
inline constexpr auto& EngineQuit = ::Grapple_EngineQuit;
inline constexpr auto& EngineRenderScale = ::Grapple_EngineRenderScale;
inline constexpr auto& EngineRenderer = ::Grapple_EngineRenderer;
inline constexpr auto& EngineSafeRect = ::Grapple_EngineSafeRect;
inline constexpr auto& EngineSetClearColor = ::Grapple_EngineSetClearColor;
inline constexpr auto& EngineSetEventSink = ::Grapple_EngineSetEventSink;
inline constexpr auto& EngineSetHooks = ::Grapple_EngineSetHooks;
inline constexpr auto& EngineSetMaxFps = ::Grapple_EngineSetMaxFps;
inline constexpr auto& EngineSetMediaPassword = ::Grapple_EngineSetMediaPassword;
inline constexpr auto& EngineSetOverlay = ::Grapple_EngineSetOverlay;
inline constexpr auto& EngineSetRefreshRate = ::Grapple_EngineSetRefreshRate;
inline constexpr auto& EngineSetTimeScale = ::Grapple_EngineSetTimeScale;
inline constexpr auto& EngineStep = ::Grapple_EngineStep;
inline constexpr auto& EngineStepsLastFrame = ::Grapple_EngineStepsLastFrame;
inline constexpr auto& EngineTickRate = ::Grapple_EngineTickRate;
inline constexpr auto& EngineTimeScale = ::Grapple_EngineTimeScale;
inline constexpr auto& EngineViewRect = ::Grapple_EngineViewRect;
inline constexpr auto& EngineWindow = ::Grapple_EngineWindow;
inline constexpr auto& EngineWindowToDesign = ::Grapple_EngineWindowToDesign;
inline constexpr auto& EventCreate = ::Grapple_EventCreate;
inline constexpr auto& EventDestroy = ::Grapple_EventDestroy;
inline constexpr auto& EventGamepadAxis = ::Grapple_EventGamepadAxis;
inline constexpr auto& EventGamepadAxisValue = ::Grapple_EventGamepadAxisValue;
inline constexpr auto& EventGamepadButton = ::Grapple_EventGamepadButton;
inline constexpr auto& EventGamepadWhich = ::Grapple_EventGamepadWhich;
inline constexpr auto& EventKeyModifiers = ::Grapple_EventKeyModifiers;
inline constexpr auto& EventKeyScancode = ::Grapple_EventKeyScancode;
inline constexpr auto& EventMouseButton = ::Grapple_EventMouseButton;
inline constexpr auto& EventMouseDeltaX = ::Grapple_EventMouseDeltaX;
inline constexpr auto& EventMouseDeltaY = ::Grapple_EventMouseDeltaY;
inline constexpr auto& EventMouseX = ::Grapple_EventMouseX;
inline constexpr auto& EventMouseY = ::Grapple_EventMouseY;
inline constexpr auto& EventSetType = ::Grapple_EventSetType;
inline constexpr auto& EventText = ::Grapple_EventText;
inline constexpr auto& EventTouchX = ::Grapple_EventTouchX;
inline constexpr auto& EventTouchY = ::Grapple_EventTouchY;
inline constexpr auto& EventType = ::Grapple_EventType;
inline constexpr auto& EventWheelX = ::Grapple_EventWheelX;
inline constexpr auto& EventWheelY = ::Grapple_EventWheelY;
inline constexpr auto& EventWindowId = ::Grapple_EventWindowId;
inline constexpr auto& FilterJointDefCreate = ::Grapple_FilterJointDefCreate;
inline constexpr auto& FilterJointDefDestroy = ::Grapple_FilterJointDefDestroy;
inline constexpr auto& FilterJointDefSetBodies = ::Grapple_FilterJointDefSetBodies;
inline constexpr auto& FingerCount = ::Grapple_FingerCount;
inline constexpr auto& FingerHeldInRect = ::Grapple_FingerHeldInRect;
inline constexpr auto& FingerInRect = ::Grapple_FingerInRect;
inline constexpr auto& GPUAcquireSwapchain = ::Grapple_GPUAcquireSwapchain;
inline constexpr auto& GPUBeginComputePass = ::Grapple_GPUBeginComputePass;
inline constexpr auto& GPUBindComputeStorageBuffer = ::Grapple_GPUBindComputeStorageBuffer;
inline constexpr auto& GPUBindComputeStorageTexture = ::Grapple_GPUBindComputeStorageTexture;
inline constexpr auto& GPUBindFragmentStorageBuffer = ::Grapple_GPUBindFragmentStorageBuffer;
inline constexpr auto& GPUBindFragmentStorageTexture = ::Grapple_GPUBindFragmentStorageTexture;
inline constexpr auto& GPUBindVertexStorageBuffer = ::Grapple_GPUBindVertexStorageBuffer;
inline constexpr auto& GPUBindVertexStorageTexture = ::Grapple_GPUBindVertexStorageTexture;
inline constexpr auto& GPUBlitInfoCreate = ::Grapple_GPUBlitInfoCreate;
inline constexpr auto& GPUBlitInfoDestroy = ::Grapple_GPUBlitInfoDestroy;
inline constexpr auto& GPUBlitInfoSetDestination = ::Grapple_GPUBlitInfoSetDestination;
inline constexpr auto& GPUBlitInfoSetFilter = ::Grapple_GPUBlitInfoSetFilter;
inline constexpr auto& GPUBlitInfoSetSource = ::Grapple_GPUBlitInfoSetSource;
inline constexpr auto& GPUBufferBindingCreate = ::Grapple_GPUBufferBindingCreate;
inline constexpr auto& GPUBufferBindingDestroy = ::Grapple_GPUBufferBindingDestroy;
inline constexpr auto& GPUBufferBindingSet = ::Grapple_GPUBufferBindingSet;
inline constexpr auto& GPUBufferLocationCreate = ::Grapple_GPUBufferLocationCreate;
inline constexpr auto& GPUBufferLocationDestroy = ::Grapple_GPUBufferLocationDestroy;
inline constexpr auto& GPUBufferLocationSet = ::Grapple_GPUBufferLocationSet;
inline constexpr auto& GPUBufferRegionCreate = ::Grapple_GPUBufferRegionCreate;
inline constexpr auto& GPUBufferRegionDestroy = ::Grapple_GPUBufferRegionDestroy;
inline constexpr auto& GPUBufferRegionSet = ::Grapple_GPUBufferRegionSet;
inline constexpr auto& GPUColorTargetInfoCreate = ::Grapple_GPUColorTargetInfoCreate;
inline constexpr auto& GPUColorTargetInfoDestroy = ::Grapple_GPUColorTargetInfoDestroy;
inline constexpr auto& GPUColorTargetInfoSetClearColor = ::Grapple_GPUColorTargetInfoSetClearColor;
inline constexpr auto& GPUColorTargetInfoSetCycle = ::Grapple_GPUColorTargetInfoSetCycle;
inline constexpr auto& GPUColorTargetInfoSetMipLayer = ::Grapple_GPUColorTargetInfoSetMipLayer;
inline constexpr auto& GPUColorTargetInfoSetOps = ::Grapple_GPUColorTargetInfoSetOps;
inline constexpr auto& GPUColorTargetInfoSetTexture = ::Grapple_GPUColorTargetInfoSetTexture;
inline constexpr auto& GPUComputeBindingsCreate = ::Grapple_GPUComputeBindingsCreate;
inline constexpr auto& GPUComputeBindingsDestroy = ::Grapple_GPUComputeBindingsDestroy;
inline constexpr auto& GPUComputePipelineInfoCreate = ::Grapple_GPUComputePipelineInfoCreate;
inline constexpr auto& GPUComputePipelineInfoDestroy = ::Grapple_GPUComputePipelineInfoDestroy;
inline constexpr auto& GPUComputePipelineInfoSetCode = ::Grapple_GPUComputePipelineInfoSetCode;
inline constexpr auto& GPUComputePipelineInfoSetEntrypoint = ::Grapple_GPUComputePipelineInfoSetEntrypoint;
inline constexpr auto& GPUComputePipelineInfoSetFormat = ::Grapple_GPUComputePipelineInfoSetFormat;
inline constexpr auto& GPUComputePipelineInfoSetThreadCount = ::Grapple_GPUComputePipelineInfoSetThreadCount;
inline constexpr auto& GPUDepthStencilTargetInfoCreate = ::Grapple_GPUDepthStencilTargetInfoCreate;
inline constexpr auto& GPUDepthStencilTargetInfoDestroy = ::Grapple_GPUDepthStencilTargetInfoDestroy;
inline constexpr auto& GPUDepthStencilTargetInfoSetClear = ::Grapple_GPUDepthStencilTargetInfoSetClear;
inline constexpr auto& GPUDepthStencilTargetInfoSetOps = ::Grapple_GPUDepthStencilTargetInfoSetOps;
inline constexpr auto& GPUDepthStencilTargetInfoSetTexture = ::Grapple_GPUDepthStencilTargetInfoSetTexture;
inline constexpr auto& GPUPipelineInfoCreate = ::Grapple_GPUPipelineInfoCreate;
inline constexpr auto& GPUPipelineInfoDestroy = ::Grapple_GPUPipelineInfoDestroy;
inline constexpr auto& GPUPipelineInfoSetDepthStencil = ::Grapple_GPUPipelineInfoSetDepthStencil;
inline constexpr auto& GPUPipelineInfoSetFillMode = ::Grapple_GPUPipelineInfoSetFillMode;
inline constexpr auto& GPUPipelineInfoSetPrimitive = ::Grapple_GPUPipelineInfoSetPrimitive;
inline constexpr auto& GPUPipelineInfoSetShaders = ::Grapple_GPUPipelineInfoSetShaders;
inline constexpr auto& GPUReadTransferBuffer = ::Grapple_GPUReadTransferBuffer;
inline constexpr auto& GPURenderStateInfoCreate = ::Grapple_GPURenderStateInfoCreate;
inline constexpr auto& GPURenderStateInfoDestroy = ::Grapple_GPURenderStateInfoDestroy;
inline constexpr auto& GPURenderStateInfoSetShader = ::Grapple_GPURenderStateInfoSetShader;
inline constexpr auto& GPUShaderCreateInfoCreate = ::Grapple_GPUShaderCreateInfoCreate;
inline constexpr auto& GPUShaderCreateInfoDestroy = ::Grapple_GPUShaderCreateInfoDestroy;
inline constexpr auto& GPUShaderCreateInfoSetCode = ::Grapple_GPUShaderCreateInfoSetCode;
inline constexpr auto& GPUShaderCreateInfoSetCounts = ::Grapple_GPUShaderCreateInfoSetCounts;
inline constexpr auto& GPUShaderCreateInfoSetEntrypoint = ::Grapple_GPUShaderCreateInfoSetEntrypoint;
inline constexpr auto& GPUShaderCreateInfoSetFormat = ::Grapple_GPUShaderCreateInfoSetFormat;
inline constexpr auto& GPUSwapchainHeight = ::Grapple_GPUSwapchainHeight;
inline constexpr auto& GPUSwapchainWidth = ::Grapple_GPUSwapchainWidth;
inline constexpr auto& GPUTextureLocationCreate = ::Grapple_GPUTextureLocationCreate;
inline constexpr auto& GPUTextureLocationDestroy = ::Grapple_GPUTextureLocationDestroy;
inline constexpr auto& GPUTextureLocationSet = ::Grapple_GPUTextureLocationSet;
inline constexpr auto& GPUTextureRegionCreate = ::Grapple_GPUTextureRegionCreate;
inline constexpr auto& GPUTextureRegionDestroy = ::Grapple_GPUTextureRegionDestroy;
inline constexpr auto& GPUTextureRegionSet = ::Grapple_GPUTextureRegionSet;
inline constexpr auto& GPUTextureSamplerBindingCreate = ::Grapple_GPUTextureSamplerBindingCreate;
inline constexpr auto& GPUTextureSamplerBindingDestroy = ::Grapple_GPUTextureSamplerBindingDestroy;
inline constexpr auto& GPUTextureSamplerBindingSet = ::Grapple_GPUTextureSamplerBindingSet;
inline constexpr auto& GPUTextureTransferInfoCreate = ::Grapple_GPUTextureTransferInfoCreate;
inline constexpr auto& GPUTextureTransferInfoDestroy = ::Grapple_GPUTextureTransferInfoDestroy;
inline constexpr auto& GPUTextureTransferInfoSet = ::Grapple_GPUTextureTransferInfoSet;
inline constexpr auto& GPUTransferBufferLocationCreate = ::Grapple_GPUTransferBufferLocationCreate;
inline constexpr auto& GPUTransferBufferLocationDestroy = ::Grapple_GPUTransferBufferLocationDestroy;
inline constexpr auto& GPUTransferBufferLocationSet = ::Grapple_GPUTransferBufferLocationSet;
inline constexpr auto& GPUWaitAndAcquireSwapchain = ::Grapple_GPUWaitAndAcquireSwapchain;
inline constexpr auto& GamepadAccelerometer = ::Grapple_GamepadAccelerometer;
inline constexpr auto& GamepadAxisValue = ::Grapple_GamepadAxisValue;
inline constexpr auto& GamepadCount = ::Grapple_GamepadCount;
inline constexpr auto& GamepadDeadzone = ::Grapple_GamepadDeadzone;
inline constexpr auto& GamepadGyro = ::Grapple_GamepadGyro;
inline constexpr auto& GamepadName = ::Grapple_GamepadName;
inline constexpr auto& GamepadStick = ::Grapple_GamepadStick;
inline constexpr auto& GamepadStopRumble = ::Grapple_GamepadStopRumble;
inline constexpr auto& GetFinger = ::Grapple_GetFinger;
inline constexpr auto& GraphicsArgsConfigPath = ::Grapple_GraphicsArgsConfigPath;
inline constexpr auto& GraphicsClamp = ::Grapple_GraphicsClamp;
inline constexpr auto& GraphicsConfigError = ::Grapple_GraphicsConfigError;
inline constexpr auto& GraphicsConfigPath = ::Grapple_GraphicsConfigPath;
inline constexpr auto& GraphicsDefaults = ::Grapple_GraphicsDefaults;
inline constexpr auto& GraphicsLightMapScale = ::Grapple_GraphicsLightMapScale;
inline constexpr auto& GraphicsLoadArgs = ::Grapple_GraphicsLoadArgs;
inline constexpr auto& GraphicsMaxDynamicLights = ::Grapple_GraphicsMaxDynamicLights;
inline constexpr auto& GraphicsParticleDensity = ::Grapple_GraphicsParticleDensity;
inline constexpr auto& GraphicsQualityName = ::Grapple_GraphicsQualityName;
inline constexpr auto& GraphicsResolve = ::Grapple_GraphicsResolve;
inline constexpr auto& GraphicsSafeMode = ::Grapple_GraphicsSafeMode;
inline constexpr auto& GraphicsSavePath = ::Grapple_GraphicsSavePath;
inline constexpr auto& GraphicsSetArchiveReader = ::Grapple_GraphicsSetArchiveReader;
inline constexpr auto& GraphicsShadowRays = ::Grapple_GraphicsShadowRays;
inline constexpr auto& GraphicsShadowSoftness = ::Grapple_GraphicsShadowSoftness;
inline constexpr auto& GraphicsToToml = ::Grapple_GraphicsToToml;
inline constexpr auto& GuiGridCell = ::Grapple_GuiGridCell;
inline constexpr auto& GuiGridCellPart = ::Grapple_GuiGridCellPart;
inline constexpr auto& GuiGridCellSpan = ::Grapple_GuiGridCellSpan;
inline constexpr auto& GuiGridCreate = ::Grapple_GuiGridCreate;
inline constexpr auto& GuiGridDestroy = ::Grapple_GuiGridDestroy;
inline constexpr auto& GuiGridEnd = ::Grapple_GuiGridEnd;
inline constexpr auto& GuiGridNextRow = ::Grapple_GuiGridNextRow;
inline constexpr auto& GuiGridRowHeight = ::Grapple_GuiGridRowHeight;
inline constexpr auto& GuiGridSpacing = ::Grapple_GuiGridSpacing;
inline constexpr auto& IdleSeconds = ::Grapple_IdleSeconds;
inline constexpr auto& KeyModifiers = ::Grapple_KeyModifiers;
inline constexpr auto& LastInputDevice = ::Grapple_LastInputDevice;
inline constexpr auto& LightAddDarkZone = ::Grapple_LightAddDarkZone;
inline constexpr auto& LightAddOccluder = ::Grapple_LightAddOccluder;
inline constexpr auto& LightAddOccluderLine = ::Grapple_LightAddOccluderLine;
inline constexpr auto& LightAmbient = ::Grapple_LightAmbient;
inline constexpr auto& LightAt = ::Grapple_LightAt;
inline constexpr auto& LightCount = ::Grapple_LightCount;
inline constexpr auto& LightDefCreate = ::Grapple_LightDefCreate;
inline constexpr auto& LightDefDestroy = ::Grapple_LightDefDestroy;
inline constexpr auto& LightDefSetColor = ::Grapple_LightDefSetColor;
inline constexpr auto& LightDefSetCone = ::Grapple_LightDefSetCone;
inline constexpr auto& LightDefSetFlicker = ::Grapple_LightDefSetFlicker;
inline constexpr auto& LightDefSetOffset = ::Grapple_LightDefSetOffset;
inline constexpr auto& LightDefSetRadius = ::Grapple_LightDefSetRadius;
inline constexpr auto& LightDefSetShadows = ::Grapple_LightDefSetShadows;
inline constexpr auto& LightDefault = ::Grapple_LightDefault;
inline constexpr auto& LightHour = ::Grapple_LightHour;
inline constexpr auto& LightPreset_ = ::Grapple_LightPreset_;
inline constexpr auto& LightSetAmbient = ::Grapple_LightSetAmbient;
inline constexpr auto& LightSetAutoOccluders = ::Grapple_LightSetAutoOccluders;
inline constexpr auto& LightSetClock = ::Grapple_LightSetClock;
inline constexpr auto& LightSetPreset = ::Grapple_LightSetPreset;
inline constexpr auto& LightSunlight = ::Grapple_LightSunlight;
inline constexpr auto& LoadTextFile = ::Grapple_LoadTextFile;
inline constexpr auto& LoadTexture = ::Grapple_LoadTexture;
inline constexpr auto& LoadTextureAsync = ::Grapple_LoadTextureAsync;
inline constexpr auto& LoadVFSFile = ::Grapple_LoadVFSFile;
inline constexpr auto& MotorJointDefCreate = ::Grapple_MotorJointDefCreate;
inline constexpr auto& MotorJointDefDestroy = ::Grapple_MotorJointDefDestroy;
inline constexpr auto& MotorJointDefSetBodies = ::Grapple_MotorJointDefSetBodies;
inline constexpr auto& MountMedia = ::Grapple_MountMedia;
inline constexpr auto& MouseDelta = ::Grapple_MouseDelta;
inline constexpr auto& MouseJointDefCreate = ::Grapple_MouseJointDefCreate;
inline constexpr auto& MouseJointDefDestroy = ::Grapple_MouseJointDefDestroy;
inline constexpr auto& MouseJointDefSetBodies = ::Grapple_MouseJointDefSetBodies;
inline constexpr auto& MouseJointDefSetMaxForce = ::Grapple_MouseJointDefSetMaxForce;
inline constexpr auto& MouseJointDefSetSpring = ::Grapple_MouseJointDefSetSpring;
inline constexpr auto& MousePosition = ::Grapple_MousePosition;
inline constexpr auto& MouseWheel = ::Grapple_MouseWheel;
inline constexpr auto& OpenVFSRead = ::Grapple_OpenVFSRead;
inline constexpr auto& PhysicsBodyCount = ::Grapple_PhysicsBodyCount;
inline constexpr auto& PhysicsGravity = ::Grapple_PhysicsGravity;
inline constexpr auto& PhysicsOverlap = ::Grapple_PhysicsOverlap;
inline constexpr auto& PhysicsPixelsPerMetre = ::Grapple_PhysicsPixelsPerMetre;
inline constexpr auto& PhysicsRaycast = ::Grapple_PhysicsRaycast;
inline constexpr auto& PhysicsSetCollisionCallback = ::Grapple_PhysicsSetCollisionCallback;
inline constexpr auto& PhysicsSetGravity = ::Grapple_PhysicsSetGravity;
inline constexpr auto& PhysicsSetPaused = ::Grapple_PhysicsSetPaused;
inline constexpr auto& PhysicsSetPixelsPerMetre = ::Grapple_PhysicsSetPixelsPerMetre;
inline constexpr auto& PhysicsSetSubSteps = ::Grapple_PhysicsSetSubSteps;
inline constexpr auto& PrismaticJointDefCreate = ::Grapple_PrismaticJointDefCreate;
inline constexpr auto& PrismaticJointDefDestroy = ::Grapple_PrismaticJointDefDestroy;
inline constexpr auto& PrismaticJointDefSetAnchors = ::Grapple_PrismaticJointDefSetAnchors;
inline constexpr auto& PrismaticJointDefSetAxis = ::Grapple_PrismaticJointDefSetAxis;
inline constexpr auto& PrismaticJointDefSetBodies = ::Grapple_PrismaticJointDefSetBodies;
inline constexpr auto& PrismaticJointDefSetLimit = ::Grapple_PrismaticJointDefSetLimit;
inline constexpr auto& PrismaticJointDefSetMotor = ::Grapple_PrismaticJointDefSetMotor;
inline constexpr auto& QuitDebugText = ::Grapple_QuitDebugText;
inline constexpr auto& RegexEscape = ::Grapple_RegexEscape;
inline constexpr auto& RenderLastStats = ::Grapple_RenderLastStats;
inline constexpr auto& RenderOverlay = ::Grapple_RenderOverlay;
inline constexpr auto& RenderWorld = ::Grapple_RenderWorld;
inline constexpr auto& RevoluteJointDefCreate = ::Grapple_RevoluteJointDefCreate;
inline constexpr auto& RevoluteJointDefDestroy = ::Grapple_RevoluteJointDefDestroy;
inline constexpr auto& RevoluteJointDefSetAnchors = ::Grapple_RevoluteJointDefSetAnchors;
inline constexpr auto& RevoluteJointDefSetBodies = ::Grapple_RevoluteJointDefSetBodies;
inline constexpr auto& RevoluteJointDefSetCollideConnected = ::Grapple_RevoluteJointDefSetCollideConnected;
inline constexpr auto& RevoluteJointDefSetLimit = ::Grapple_RevoluteJointDefSetLimit;
inline constexpr auto& RevoluteJointDefSetMotor = ::Grapple_RevoluteJointDefSetMotor;
inline constexpr auto& RevoluteJointDefSetSpring = ::Grapple_RevoluteJointDefSetSpring;
inline constexpr auto& SaveInfoOf = ::Grapple_SaveInfoOf;
inline constexpr auto& SavePath = ::Grapple_SavePath;
inline constexpr auto& SaveRead = ::Grapple_SaveRead;
inline constexpr auto& SaveSetIdentity = ::Grapple_SaveSetIdentity;
inline constexpr auto& SceneCurrent = ::Grapple_SceneCurrent;
inline constexpr auto& SceneDepth = ::Grapple_SceneDepth;
inline constexpr auto& SceneEngine = ::Grapple_SceneEngine;
inline constexpr auto& SceneFind = ::Grapple_SceneFind;
inline constexpr auto& SceneKey = ::Grapple_SceneKey;
inline constexpr auto& SceneName = ::Grapple_SceneName;
inline constexpr auto& SceneSetTransitionColor = ::Grapple_SceneSetTransitionColor;
inline constexpr auto& SceneState = ::Grapple_SceneState;
inline constexpr auto& ScriptUnbind = ::Grapple_ScriptUnbind;
inline constexpr auto& SetDebugTextSize = ::Grapple_SetDebugTextSize;
inline constexpr auto& SetDirectionRepeat = ::Grapple_SetDirectionRepeat;
inline constexpr auto& SetGamepadDeadzone = ::Grapple_SetGamepadDeadzone;
inline constexpr auto& SetTextInput = ::Grapple_SetTextInput;
inline constexpr auto& SetTriggerThreshold = ::Grapple_SetTriggerThreshold;
inline constexpr auto& SpriteCreate = ::Grapple_SpriteCreate;
inline constexpr auto& SpriteDefault = ::Grapple_SpriteDefault;
inline constexpr auto& SpriteDestroy = ::Grapple_SpriteDestroy;
inline constexpr auto& SpriteSetColor = ::Grapple_SpriteSetColor;
inline constexpr auto& SpriteSetLayer = ::Grapple_SpriteSetLayer;
inline constexpr auto& SpriteSetOrigin = ::Grapple_SpriteSetOrigin;
inline constexpr auto& SpriteSetScreenSpace = ::Grapple_SpriteSetScreenSpace;
inline constexpr auto& SpriteSetSize = ::Grapple_SpriteSetSize;
inline constexpr auto& SpriteSetSortByY = ::Grapple_SpriteSetSortByY;
inline constexpr auto& SpriteSetSource = ::Grapple_SpriteSetSource;
inline constexpr auto& SpriteSetTexture = ::Grapple_SpriteSetTexture;
inline constexpr auto& SpriteSetVisible = ::Grapple_SpriteSetVisible;
inline constexpr auto& Text = ::Grapple_Text;
inline constexpr auto& TextCount = ::Grapple_TextCount;
inline constexpr auto& TextFormat = ::Grapple_TextFormat;
inline constexpr auto& TextLanguage = ::Grapple_TextLanguage;
inline constexpr auto& TextSetLanguage = ::Grapple_TextSetLanguage;
inline constexpr auto& TextSystemLanguages = ::Grapple_TextSystemLanguages;
inline constexpr auto& TextTyped = ::Grapple_TextTyped;
inline constexpr auto& Texture = ::Grapple_Texture;
inline constexpr auto& TouchPinch = ::Grapple_TouchPinch;
inline constexpr auto& TouchRotation = ::Grapple_TouchRotation;
inline constexpr auto& WeldJointDefCreate = ::Grapple_WeldJointDefCreate;
inline constexpr auto& WeldJointDefDestroy = ::Grapple_WeldJointDefDestroy;
inline constexpr auto& WeldJointDefSetAnchors = ::Grapple_WeldJointDefSetAnchors;
inline constexpr auto& WeldJointDefSetBodies = ::Grapple_WeldJointDefSetBodies;
inline constexpr auto& WeldJointDefSetSpring = ::Grapple_WeldJointDefSetSpring;
inline constexpr auto& WheelJointDefCreate = ::Grapple_WheelJointDefCreate;
inline constexpr auto& WheelJointDefDestroy = ::Grapple_WheelJointDefDestroy;
inline constexpr auto& WheelJointDefSetAnchors = ::Grapple_WheelJointDefSetAnchors;
inline constexpr auto& WheelJointDefSetAxis = ::Grapple_WheelJointDefSetAxis;
inline constexpr auto& WheelJointDefSetBodies = ::Grapple_WheelJointDefSetBodies;
inline constexpr auto& WheelJointDefSetMotor = ::Grapple_WheelJointDefSetMotor;
inline constexpr auto& WheelJointDefSetSpring = ::Grapple_WheelJointDefSetSpring;

}  // namespace ext
}  // namespace grapple

#endif  // GRAPPLE_CPP_GEN_GRAPPLE_H_
// clang-format on
