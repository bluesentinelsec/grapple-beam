/*
 * grapple_tiled.c — Tiled map loading; the single cute_tiled impl TU.
 * Original Grapple code (zlib).
 */
#include <grapple/tiled.h>
#include <grapple/vfs.h>
#include <physfs.h>

#include <cJSON.h>

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

struct Grapple_TiledMap
{
    cute_tiled_map_t *map;
    cute_tiled_layer_t **layers; /* flattened top-level layer list */
    int layer_count;
};

Grapple_TiledMap *Grapple_LoadTiledMap(const char *path)
{
    if (path == NULL)
    {
        SDL_InvalidParamError("path");
        return NULL;
    }
    unsigned char *data = NULL;
    int size = 0;
    if (PHYSFS_isInit() && PHYSFS_exists(path))
    {
        data = Grapple_LoadVFSFile(path, &size);
    }
    else
    {
        size_t fs_size = 0;
        void *raw = SDL_LoadFile(path, &fs_size);
        if (raw != NULL && fs_size <= (size_t)SDL_MAX_SINT32)
        {
            data = (unsigned char *)raw;
            size = (int)fs_size;
        }
        else
        {
            SDL_free(raw);
        }
    }
    if (data == NULL)
    {
        return NULL; /* SDL error already set by the loader */
    }

    /* cute_tiled hard-crashes (deliberate null write) when its cursor runs
     * off the end of the buffer, which truncated or non-JSON input can cause.
     * Validate structure with cJSON first so malformed input fails cleanly. */
    cJSON *doc = cJSON_ParseWithLength((const char *)data, (size_t)size);
    if (doc == NULL)
    {
        SDL_free(data);
        SDL_SetError("not a valid JSON document: %s", path);
        return NULL;
    }
    cJSON_Delete(doc);

    cute_tiled_map_t *parsed = cute_tiled_load_map_from_memory(data, size, NULL);
    SDL_free(data);
    if (parsed == NULL)
    {
        SDL_SetError("cute_tiled: %s", cute_tiled_error_reason ? cute_tiled_error_reason
                                                              : "parse failed");
        return NULL;
    }

    Grapple_TiledMap *map = (Grapple_TiledMap *)SDL_calloc(1, sizeof(Grapple_TiledMap));
    if (map == NULL)
    {
        cute_tiled_free_map(parsed);
        return NULL;
    }
    map->map = parsed;
    for (cute_tiled_layer_t *l = parsed->layers; l != NULL; l = l->next)
    {
        map->layer_count++;
    }
    map->layers = (cute_tiled_layer_t **)SDL_calloc((size_t)map->layer_count,
                                                    sizeof(cute_tiled_layer_t *));
    int at = 0;
    for (cute_tiled_layer_t *l = parsed->layers; l != NULL; l = l->next)
    {
        map->layers[at++] = l;
    }
    return map;
}

void Grapple_FreeTiledMap(Grapple_TiledMap *map)
{
    if (map != NULL)
    {
        cute_tiled_free_map(map->map);
        SDL_free(map->layers);
        SDL_free(map);
    }
}

int Grapple_TiledMapWidth(const Grapple_TiledMap *map)
{
    return (map != NULL) ? map->map->width : 0;
}
int Grapple_TiledMapHeight(const Grapple_TiledMap *map)
{
    return (map != NULL) ? map->map->height : 0;
}
int Grapple_TiledTileWidth(const Grapple_TiledMap *map)
{
    return (map != NULL) ? map->map->tilewidth : 0;
}
int Grapple_TiledTileHeight(const Grapple_TiledMap *map)
{
    return (map != NULL) ? map->map->tileheight : 0;
}
int Grapple_TiledLayerCount(const Grapple_TiledMap *map)
{
    return (map != NULL) ? map->layer_count : 0;
}

static cute_tiled_layer_t *LayerAt(const Grapple_TiledMap *map, int idx)
{
    if (map == NULL || idx < 0 || idx >= map->layer_count)
    {
        return NULL;
    }
    return map->layers[idx];
}

const char *Grapple_TiledLayerName(const Grapple_TiledMap *map, int idx)
{
    cute_tiled_layer_t *layer = LayerAt(map, idx);
    return (layer != NULL) ? layer->name.ptr : NULL;
}
const char *Grapple_TiledLayerType(const Grapple_TiledMap *map, int idx)
{
    cute_tiled_layer_t *layer = LayerAt(map, idx);
    return (layer != NULL) ? layer->type.ptr : NULL;
}

int Grapple_TiledTileAt(const Grapple_TiledMap *map, int layer, int x, int y)
{
    cute_tiled_layer_t *l = LayerAt(map, layer);
    if (l == NULL || l->data == NULL || x < 0 || y < 0 || x >= l->width || y >= l->height)
    {
        return -1;
    }
    return l->data[y * l->width + x];
}

int Grapple_TiledObjectCount(const Grapple_TiledMap *map, int layer)
{
    cute_tiled_layer_t *l = LayerAt(map, layer);
    int count = 0;
    if (l != NULL)
    {
        for (cute_tiled_object_t *o = l->objects; o != NULL; o = o->next)
        {
            count++;
        }
    }
    return count;
}

bool Grapple_TiledObjectAt(const Grapple_TiledMap *map, int layer, int index,
                             Grapple_TiledObject *out)
{
    cute_tiled_layer_t *l = LayerAt(map, layer);
    if (l == NULL || out == NULL || index < 0)
    {
        return false;
    }
    int at = 0;
    for (cute_tiled_object_t *o = l->objects; o != NULL; o = o->next, ++at)
    {
        if (at == index)
        {
            out->name = o->name.ptr;
            out->type = o->type.ptr;
            out->x = o->x;
            out->y = o->y;
            out->w = o->width;
            out->h = o->height;
            return true;
        }
    }
    return false;
}

cute_tiled_map_t *Grapple_TiledRaw(Grapple_TiledMap *map)
{
    return (map != NULL) ? map->map : NULL;
}
