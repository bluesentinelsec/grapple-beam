// tilemap.h — RAII over the Tiled map parser. (Named tilemap.h, not
// tiled.h: the C header grapple/tiled.h would collide on
// case-insensitive filesystems.)
// Original Grapple code (zlib).

#ifndef GRAPPLE_CPP_TILEMAP_H_
#define GRAPPLE_CPP_TILEMAP_H_

#include <grapple/tiled.h>

#include <optional>
#include <string>
#include <utility>

#include "grapple/status.h"

namespace grapple {

struct TiledObject {
  std::string name;
  std::string type;
  float x = 0, y = 0, w = 0, h = 0;
};

// Owns a parsed Tiled map (.tmj), loaded VFS-first.
class TiledMap {
 public:
  static Result<TiledMap> Load(const std::string& path) {
    Grapple_TiledMap* raw = Grapple_LoadTiledMap(path.c_str());
    if (raw == nullptr) return Status::FromSdl();
    return TiledMap(raw);
  }

  ~TiledMap() { Destroy(); }
  TiledMap(TiledMap&& other) noexcept : map_(std::exchange(other.map_, nullptr)) {}
  TiledMap& operator=(TiledMap&& other) noexcept {
    if (this != &other) {
      Destroy();
      map_ = std::exchange(other.map_, nullptr);
    }
    return *this;
  }
  TiledMap(const TiledMap&) = delete;
  TiledMap& operator=(const TiledMap&) = delete;

  int width() const { return Grapple_TiledMapWidth(map_); }
  int height() const { return Grapple_TiledMapHeight(map_); }
  int tile_width() const { return Grapple_TiledTileWidth(map_); }
  int tile_height() const { return Grapple_TiledTileHeight(map_); }
  int layer_count() const { return Grapple_TiledLayerCount(map_); }

  std::optional<std::string> LayerName(int index) const {
    const char* name = Grapple_TiledLayerName(map_, index);
    if (name == nullptr) return std::nullopt;
    return std::string(name);
  }

  // GID at (x, y): 0 = empty; error status on bad layer/coords.
  Result<int> TileAt(int layer, int x, int y) const {
    const int gid = Grapple_TiledTileAt(map_, layer, x, y);
    if (gid < 0) return Status::Error("bad layer index or coordinates");
    return gid;
  }

  int ObjectCount(int layer) const { return Grapple_TiledObjectCount(map_, layer); }

  Result<TiledObject> ObjectAt(int layer, int index) const {
    Grapple_TiledObject raw;
    if (!Grapple_TiledObjectAt(map_, layer, index, &raw)) {
      return Status::Error("bad layer or object index");
    }
    TiledObject object;
    object.name = (raw.name != nullptr) ? raw.name : "";
    object.type = (raw.type != nullptr) ? raw.type : "";
    object.x = raw.x;
    object.y = raw.y;
    object.w = raw.w;
    object.h = raw.h;
    return object;
  }

  cute_tiled_map_t* raw() const { return Grapple_TiledRaw(map_); }

 private:
  explicit TiledMap(Grapple_TiledMap* map) : map_(map) {}
  void Destroy() {
    if (map_ != nullptr) Grapple_FreeTiledMap(map_);
    map_ = nullptr;
  }
  Grapple_TiledMap* map_ = nullptr;
};

}  // namespace grapple

#endif  // GRAPPLE_CPP_TILEMAP_H_
