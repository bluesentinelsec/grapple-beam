/*
 * grapple_platformer_tiled.c — a level from a Tiled map. Original
 * grapple-beam code (zlib).
 *
 * The map is read through Grapple::Tiled's parse tree, so anything Tiled
 * can save is reachable, and the conventions here are the smallest set a
 * designer has to know:
 *
 *   - a tile layer named "solid", "ground", "collision" or "walls", or with
 *     a boolean `solid` property set, is collision: every cell holding a
 *     tile is solid;
 *   - a tileset tile with a `one_way` property is a ledge, and one with
 *     `solid = false` is decoration, in any collision layer;
 *   - an object whose class is "floor", "wall", "block" or "platform" fills
 *     the cells under its rectangle; class "player" puts the player there.
 *
 * Everything else — decoration layers, other objects — is left for the
 * game to read from the same map.
 */
#include "platformer_internal.h"

#include <cute_tiled.h>
#include <grapple/tiled.h>

static const char *Str(cute_tiled_string_t s)
{
    return (s.ptr != NULL) ? s.ptr : "";
}

static bool FindBool(const cute_tiled_property_t *props, int count, const char *name, bool *out)
{
    for (int i = 0; i < count; ++i)
    {
        if (SDL_strcmp(Str(props[i].name), name) != 0)
        {
            continue;
        }
        switch (props[i].type)
        {
        case CUTE_TILED_PROPERTY_BOOL:
            *out = props[i].data.boolean != 0;
            return true;
        case CUTE_TILED_PROPERTY_INT:
            *out = props[i].data.integer != 0;
            return true;
        default:
            return false;
        }
    }
    return false;
}

static bool IsCollisionLayer(const cute_tiled_layer_t *layer)
{
    bool solid = false;
    if (FindBool(layer->properties, layer->property_count, "solid", &solid))
    {
        return solid;
    }
    static const char *const kNames[] = {"solid", "ground", "collision", "walls"};
    for (size_t i = 0; i < SDL_arraysize(kNames); ++i)
    {
        if (SDL_strcasecmp(Str(layer->name), kNames[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

/* What a GID means, by its tileset tile's properties. */
static Grapple_PlatformerTile KindOfGid(const cute_tiled_map_t *map, int gid)
{
    if (gid <= 0)
    {
        return GRAPPLE_PLATFORMER_EMPTY;
    }
    for (const cute_tiled_tileset_t *set = map->tilesets; set != NULL; set = set->next)
    {
        if (gid < set->firstgid || gid >= set->firstgid + set->tilecount)
        {
            continue;
        }
        const int local = gid - set->firstgid;
        for (const cute_tiled_tile_descriptor_t *tile = set->tiles; tile != NULL; tile = tile->next)
        {
            if (tile->tile_index != local)
            {
                continue;
            }
            bool flag = false;
            if (FindBool(tile->properties, tile->property_count, "one_way", &flag) && flag)
            {
                return GRAPPLE_PLATFORMER_ONE_WAY;
            }
            if (FindBool(tile->properties, tile->property_count, "solid", &flag) && !flag)
            {
                return GRAPPLE_PLATFORMER_EMPTY;
            }
            return GRAPPLE_PLATFORMER_SOLID;
        }
        return GRAPPLE_PLATFORMER_SOLID;
    }
    return GRAPPLE_PLATFORMER_SOLID;
}

static void ReadTileLayer(Grapple_Platformer *level, const cute_tiled_map_t *map,
                          const cute_tiled_layer_t *layer)
{
    if (layer->data == NULL)
    {
        return;
    }
    const int w = (layer->width > 0) ? layer->width : level->width;
    for (int i = 0; i < layer->data_count; ++i)
    {
        const int gid = cute_tiled_unset_flags(layer->data[i]);
        const Grapple_PlatformerTile kind = KindOfGid(map, gid);
        if (kind != GRAPPLE_PLATFORMER_EMPTY)
        {
            Grapple_PlatformerSetTile(level, i % w, i / w, kind);
        }
    }
}

/* An object rectangle in pixels, snapped to the cells it covers. */
static void ObjectCells(const Grapple_Platformer *level, const cute_tiled_object_t *object, int *x,
                        int *y, int *w, int *h)
{
    const float tile = (float)level->tile;
    const int x0 = (int)SDL_floorf(object->x / tile + 0.001f);
    const int y0 = (int)SDL_floorf(object->y / tile + 0.001f);
    const int x1 = (int)SDL_ceilf((object->x + object->width) / tile - 0.001f);
    const int y1 = (int)SDL_ceilf((object->y + object->height) / tile - 0.001f);
    *x = x0;
    *y = y0;
    *w = (x1 > x0) ? x1 - x0 : 1;
    *h = (y1 > y0) ? y1 - y0 : 1;
}

static void ReadObjectLayer(Grapple_Platformer *level, const cute_tiled_layer_t *layer,
                            bool *placed_player)
{
    for (const cute_tiled_object_t *object = layer->objects; object != NULL; object = object->next)
    {
        const char *kind = Str(object->type);
        int x, y, w, h;
        ObjectCells(level, object, &x, &y, &w, &h);
        if (SDL_strcasecmp(kind, "floor") == 0)
        {
            Grapple_PlatformerCreateFloor(level, x, y, w, h);
        }
        else if (SDL_strcasecmp(kind, "wall") == 0 || SDL_strcasecmp(kind, "block") == 0)
        {
            Grapple_PlatformerCreateBlock(level, x, y, w, h);
        }
        else if (SDL_strcasecmp(kind, "platform") == 0)
        {
            Grapple_PlatformerCreatePlatform(level, x, y, w);
        }
        else if (SDL_strcasecmp(kind, "player") == 0 && !*placed_player)
        {
            /* The object's bottom edge is where the feet go; a point object
               has no height, so its position is the feet. */
            const float tile = (float)level->tile;
            const float feet_x = object->x + object->width * 0.5f;
            const float feet_y = object->y + object->height;
            const int tile_x = (int)SDL_floorf(feet_x / tile);
            const int tile_y = (int)SDL_ceilf(feet_y / tile) - 1;
            if (Grapple_PlatformerCreatePlayer(level, tile_x, tile_y) != GRAPPLE_ACTOR_NONE)
            {
                *placed_player = true;
            }
        }
    }
}

static void ReadLayers(Grapple_Platformer *level, const cute_tiled_map_t *map,
                       const cute_tiled_layer_t *first, bool *placed_player)
{
    for (const cute_tiled_layer_t *layer = first; layer != NULL; layer = layer->next)
    {
        const char *type = Str(layer->type);
        if (SDL_strcmp(type, "tilelayer") == 0)
        {
            if (IsCollisionLayer(layer))
            {
                ReadTileLayer(level, map, layer);
            }
        }
        else if (SDL_strcmp(type, "objectgroup") == 0)
        {
            ReadObjectLayer(level, layer, placed_player);
        }
        else if (SDL_strcmp(type, "group") == 0)
        {
            ReadLayers(level, map, layer->layers, placed_player);
        }
    }
}

Grapple_Platformer *Grapple_LoadPlatformer(Grapple_Engine *engine, const char *path)
{
    Grapple_TiledMap *tiled = Grapple_LoadTiledMap(path);
    if (tiled == NULL)
    {
        return NULL;
    }
    const int tw = Grapple_TiledTileWidth(tiled);
    const int th = Grapple_TiledTileHeight(tiled);
    if (tw != th)
    {
        SDL_SetError("%s: tiles are %dx%d; a platformer grid needs square tiles", path, tw, th);
        Grapple_FreeTiledMap(tiled);
        return NULL;
    }
    Grapple_Platformer *level = Grapple_CreatePlatformer(engine, Grapple_TiledMapWidth(tiled),
                                                         Grapple_TiledMapHeight(tiled), tw);
    if (level == NULL)
    {
        Grapple_FreeTiledMap(tiled);
        return NULL;
    }
    const cute_tiled_map_t *map = Grapple_TiledRaw(tiled);
    bool placed_player = false;
    ReadLayers(level, map, map->layers, &placed_player);
    Grapple_FreeTiledMap(tiled);
    return level;
}
