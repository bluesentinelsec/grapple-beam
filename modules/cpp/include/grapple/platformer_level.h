// platformer_level.h — RAII over Grapple::Platformer (named so it cannot
// shadow the C header <grapple/platformer.h> it wraps): a tile-grid level with a
// tuned player character, run by an engine scene. Google style, no
// exceptions; failures come back as Status/Result like the rest of the C++
// surface. Original grapple-beam code (zlib).
//
//   auto level = grapple::Platformer::Create(engine.get(), 212, 15, 16);
//   level->CreateFloor(0, 13, 69);
//   level->CreateWall(28, 9, 4);
//   level->CreatePlayer(3, 12);
//   level->Attach();
//
// Level coordinates are tiles; the player's are pixels. See
// <grapple/platformer.h> for the contract each call follows.
#ifndef GRAPPLE_CPP_PLATFORMER_LEVEL_H_
#define GRAPPLE_CPP_PLATFORMER_LEVEL_H_

#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_binding.h>
#include <grapple/engine_camera.h>
#include <grapple/platformer.h>

#include <string>
#include <utility>

#include "grapple/status.h"

namespace grapple {

// A non-owning view of the level's player. Valid while the level is.
class PlatformerPlayer {
 public:
  PlatformerPlayer() = default;
  explicit PlatformerPlayer(Grapple_Platformer* level) : level_(level) {}

  bool exists() const { return Grapple_PlatformerHasPlayer(level_); }
  explicit operator bool() const { return exists(); }
  Grapple_ActorId actor() const { return Grapple_PlatformerPlayer(level_); }

  Grapple_PlayerState state() const { return Grapple_PlatformerPlayerState(level_); }
  const char* state_name() const { return Grapple_PlatformerPlayerStateName(level_); }
  bool state_changed() const { return Grapple_PlatformerPlayerStateChanged(level_); }
  bool landed() const { return Grapple_PlatformerPlayerLanded(level_); }
  bool jumped() const { return Grapple_PlatformerPlayerJumped(level_); }
  bool bumped() const { return Grapple_PlatformerPlayerBumped(level_); }
  bool fell() const { return Grapple_PlatformerPlayerFell(level_); }
  bool grounded() const { return Grapple_PlatformerPlayerGrounded(level_); }
  int facing() const { return Grapple_PlatformerPlayerFacing(level_); }
  // -1 or +1 while airborne and pressed against a wall on that side.
  int wall() const { return Grapple_PlatformerPlayerWall(level_); }

  std::pair<float, float> position() const {
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerPosition(level_, &x, &y);
    return {x, y};
  }
  std::pair<float, float> velocity() const {
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerPlayerVelocity(level_, &x, &y);
    return {x, y};
  }
  std::pair<float, float> size() const {
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPlayerSize(level_, &w, &h);
    return {w, h};
  }
  void set_size(float width, float height) const {
    Grapple_PlatformerSetPlayerSize(level_, width, height);
  }
  void set_color(float r, float g, float b, float a = 1.0f) const {
    Grapple_PlatformerSetPlayerColor(level_, r, g, b, a);
  }

  Status Set(const std::string& key, float value) const {
    if (!Grapple_PlatformerSetPlayerTuning(level_, key.c_str(), value)) {
      return Status::FromSdl();
    }
    return Status::Ok();
  }
  float Get(const std::string& key) const {
    return Grapple_PlatformerPlayerTuning(level_, key.c_str());
  }

  void Respawn(float x, float y) const { Grapple_PlatformerPlayerRespawn(level_, x, y); }
  void RespawnAtStart() const { Grapple_PlatformerPlayerRespawnAtStart(level_); }
  void set_paused(bool paused) const { Grapple_PlatformerSetPlayerPaused(level_, paused); }
  bool paused() const { return Grapple_PlatformerPlayerPaused(level_); }

  // Drive the player from code: a cutscene, a replay, a test.
  void set_scripted_input(bool enabled) const {
    Grapple_PlatformerSetScriptedInput(level_, enabled);
  }
  void Input(float move_x, bool jump, bool run) const {
    Grapple_PlatformerScriptInput(level_, move_x, jump, run);
  }

 private:
  Grapple_Platformer* level_ = nullptr;
};

// Owns a level. Destroy it after the engine has stopped running it.
class Platformer {
 public:
  static Result<Platformer> Create(Grapple_Engine* engine, int width, int height, int tile_size) {
    Grapple_Platformer* raw = Grapple_CreatePlatformer(engine, width, height, tile_size);
    if (raw == nullptr) return Status::FromSdl();
    return Platformer(raw);
  }
  // From a Tiled .tmj map, VFS-first.
  static Result<Platformer> Load(Grapple_Engine* engine, const std::string& path) {
    Grapple_Platformer* raw = Grapple_LoadPlatformer(engine, path.c_str());
    if (raw == nullptr) return Status::FromSdl();
    return Platformer(raw);
  }

  Platformer() = default;
  ~Platformer() { reset(); }
  Platformer(Platformer&& other) noexcept : level_(std::exchange(other.level_, nullptr)) {}
  Platformer& operator=(Platformer&& other) noexcept {
    if (this != &other) {
      reset();
      level_ = std::exchange(other.level_, nullptr);
    }
    return *this;
  }
  Platformer(const Platformer&) = delete;
  Platformer& operator=(const Platformer&) = delete;

  Grapple_Platformer* get() const { return level_; }
  explicit operator bool() const { return level_ != nullptr; }
  void reset() {
    if (level_ != nullptr) Grapple_DestroyPlatformer(level_);
    level_ = nullptr;
  }

  Grapple_Engine* engine() const { return Grapple_PlatformerEngine(level_); }
  int width() const { return Grapple_PlatformerWidth(level_); }
  int height() const { return Grapple_PlatformerHeight(level_); }
  int tile_size() const { return Grapple_PlatformerTileSize(level_); }
  std::pair<float, float> pixel_size() const {
    float w = 0.0f;
    float h = 0.0f;
    Grapple_PlatformerPixelSize(level_, &w, &h);
    return {w, h};
  }

  // --- cells, in tiles ----------------------------------------------------
  Status SetTile(int x, int y, Grapple_PlatformerTile tile) const {
    if (!Grapple_PlatformerSetTile(level_, x, y, tile)) return Status::FromSdl();
    return Status::Ok();
  }
  Grapple_PlatformerTile TileAt(int x, int y) const {
    return Grapple_PlatformerTileAt(level_, x, y);
  }
  int Fill(int x, int y, int width, int height, Grapple_PlatformerTile tile) const {
    return Grapple_PlatformerFillTiles(level_, x, y, width, height, tile);
  }
  bool CreateFloor(int x, int y, int width, int depth = 0) const {
    return Grapple_PlatformerCreateFloor(level_, x, y, width, depth);
  }
  bool CreateWall(int x, int y, int height) const {
    return Grapple_PlatformerCreateWall(level_, x, y, height);
  }
  bool CreateBlock(int x, int y, int width = 1, int height = 1) const {
    return Grapple_PlatformerCreateBlock(level_, x, y, width, height);
  }
  bool CreateStairs(int x, int y, int steps, bool rising = true) const {
    return Grapple_PlatformerCreateStairs(level_, x, y, steps, rising);
  }
  bool CreatePlatform(int x, int y, int width) const {
    return Grapple_PlatformerCreatePlatform(level_, x, y, width);
  }
  // A generic solid rectangle in pixels; -1 on failure.
  int AddSolid(float x, float y, float w, float h) const {
    return Grapple_PlatformerAddSolid(level_, x, y, w, h);
  }
  bool RemoveSolid(int id) const { return Grapple_PlatformerRemoveSolid(level_, id); }
  int solid_count() const { return Grapple_PlatformerSolidCount(level_); }

  // --- look ----------------------------------------------------------------
  void set_background(float r, float g, float b, float a = 1.0f) const {
    Grapple_PlatformerSetBackgroundColor(level_, r, g, b, a);
  }
  void set_tile_color(Grapple_PlatformerTile tile, float r, float g, float b,
                      float a = 1.0f) const {
    Grapple_PlatformerSetTileColor(level_, tile, r, g, b, a);
  }
  void set_tile_texture(Grapple_PlatformerTile tile, SDL_Texture* texture) const {
    Grapple_PlatformerSetTileTexture(level_, tile, texture);
  }

  // --- camera --------------------------------------------------------------
  void set_scroll(Grapple_PlatformerScroll mode) const {
    Grapple_PlatformerSetScroll(level_, mode);
  }
  Grapple_PlatformerScroll scroll() const { return Grapple_PlatformerScrollMode(level_); }
  void set_camera_smoothing(float seconds) const {
    Grapple_PlatformerSetCameraSmoothing(level_, seconds);
  }
  void set_camera_look_ahead(float pixels) const {
    Grapple_PlatformerSetCameraLookAhead(level_, pixels);
  }
  void set_camera_deadzone(float width, float height) const {
    Grapple_PlatformerSetCameraDeadzone(level_, width, height);
  }
  std::pair<float, float> camera_position() const {
    float x = 0.0f;
    float y = 0.0f;
    Grapple_PlatformerCameraPosition(level_, &x, &y);
    return {x, y};
  }
  Grapple_Camera* camera() const { return Grapple_PlatformerCamera(level_); }

  // --- the loop ------------------------------------------------------------
  Status Attach() const {
    if (!Grapple_PlatformerAttach(level_)) return Status::FromSdl();
    return Status::Ok();
  }
  void Detach() const { Grapple_PlatformerDetach(level_); }
  bool attached() const { return Grapple_PlatformerAttached(level_); }
  void Step(float step) const { Grapple_PlatformerStep(level_, step); }
  void Update(float dt) const { Grapple_PlatformerUpdate(level_, dt); }
  void Render(float alpha) const { Grapple_PlatformerRender(level_, alpha); }

  // --- the player ----------------------------------------------------------
  Result<PlatformerPlayer> CreatePlayer(int tile_x, int tile_y) const {
    if (Grapple_PlatformerCreatePlayer(level_, tile_x, tile_y) == GRAPPLE_ACTOR_NONE) {
      return Status::FromSdl();
    }
    return PlatformerPlayer(level_);
  }
  PlatformerPlayer player() const { return PlatformerPlayer(level_); }
  void RemovePlayer() const { Grapple_PlatformerRemovePlayer(level_); }

  // --- input ---------------------------------------------------------------
  Grapple_ActionMap* actions() const { return Grapple_PlatformerActions(level_); }
  Status Bind(const std::string& action, const std::string& binding) const {
    if (!Grapple_PlatformerBind(level_, action.c_str(), binding.c_str())) {
      return Status::FromSdl();
    }
    return Status::Ok();
  }
  bool ActionDown(const std::string& action) const {
    return Grapple_PlatformerActionDown(level_, action.c_str());
  }
  bool ActionPressed(const std::string& action) const {
    return Grapple_PlatformerActionPressed(level_, action.c_str());
  }
  bool ActionReleased(const std::string& action) const {
    return Grapple_PlatformerActionReleased(level_, action.c_str());
  }
  float ActionValue(const std::string& action) const {
    return Grapple_PlatformerActionValue(level_, action.c_str());
  }

 private:
  explicit Platformer(Grapple_Platformer* level) : level_(level) {}
  Grapple_Platformer* level_ = nullptr;
};

}  // namespace grapple

#endif  // GRAPPLE_CPP_PLATFORMER_LEVEL_H_
