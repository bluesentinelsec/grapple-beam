/**
 * @file tiled.h
 * @brief Tiled map (.tmj JSON) loading over vendored cute_tiled.
 *
 * Original Grapple code (zlib) around cute_tiled (single dependency-free
 * C header, dual zlib/public-domain; provenance in deps/cute_tiled.md).
 * Loads through the PhysFS VFS when initialized (maps can live in mounted,
 * optionally encrypted, zip archives) with filesystem fallback. This flat
 * accessor API is what the Lua/Ruby/C++ bindings wrap; the full parsed
 * cute_tiled_map_t is reachable via Grapple_TiledRaw for C users.
 */
#ifndef GRAPPLE_TILED_H
#define GRAPPLE_TILED_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_TiledMap Grapple_TiledMap;
typedef struct cute_tiled_map_t cute_tiled_map_t;

typedef struct Grapple_TiledObject
{
    const char *name; /* borrowed from the map; valid until free */
    const char *type;
    float x, y, w, h;
} Grapple_TiledObject;

/** Load a .tmj map (VFS first, then filesystem). NULL + SDL_GetError on
 *  parse failure. Free with Grapple_FreeTiledMap. */
extern Grapple_TiledMap *Grapple_LoadTiledMap(const char *path);
extern void Grapple_FreeTiledMap(Grapple_TiledMap *map);

extern int Grapple_TiledMapWidth(const Grapple_TiledMap *map);   /* in tiles */
extern int Grapple_TiledMapHeight(const Grapple_TiledMap *map);  /* in tiles */
extern int Grapple_TiledTileWidth(const Grapple_TiledMap *map);  /* in pixels */
extern int Grapple_TiledTileHeight(const Grapple_TiledMap *map); /* in pixels */

extern int Grapple_TiledLayerCount(const Grapple_TiledMap *map);
/** NULL when idx is out of range. */
extern const char *Grapple_TiledLayerName(const Grapple_TiledMap *map, int idx);
/** "tilelayer", "objectgroup", ... — NULL when out of range. */
extern const char *Grapple_TiledLayerType(const Grapple_TiledMap *map, int idx);

/** Tile GID at (x, y) of a tilelayer: 0 = empty, -1 = bad layer/coords. */
extern int Grapple_TiledTileAt(const Grapple_TiledMap *map, int layer, int x, int y);

/** Number of objects in an objectgroup layer (0 for tile layers). */
extern int Grapple_TiledObjectCount(const Grapple_TiledMap *map, int layer);
/** False when indices are out of range. */
extern bool Grapple_TiledObjectAt(const Grapple_TiledMap *map, int layer, int index,
                                    Grapple_TiledObject *out);

/** The full cute_tiled parse tree (tilesets, properties, animations...). */
extern cute_tiled_map_t *Grapple_TiledRaw(Grapple_TiledMap *map);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_TILED_H */
