---
title: Tiled
description: "Tiled map (.tmj) parsing via cute_tiled, VFS-aware so levels load from mounted (optionally encrypted) archives."
---

# Tiled — `Grapple::Tiled`

Parses [Tiled](https://www.mapeditor.org/) JSON maps (`.tmj`) via the
vendored single-header cute_tiled, with Formats validation and VFS integration. Loading
is **VFS-first**: if the [virtual filesystem](vfs.md) is initialized
and the path exists there, maps load from mounted (optionally encrypted)
archives; otherwise from disk.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Tiled)
```

```c
#include <grapple/tiled.h>

Grapple_TiledMap *map = Grapple_LoadTiledMap("/assets/level.tmj");

int w  = Grapple_TiledMapWidth(map);       /* in tiles */
int th = Grapple_TiledTileHeight(map);     /* in pixels */

for (int y = 0; y < Grapple_TiledMapHeight(map); y++)
    for (int x = 0; x < w; x++) {
        int gid = Grapple_TiledTileAt(map, 0, x, y);  /* 0 = empty */
        /* draw tile gid at (x, y) */
    }

Grapple_TiledObject obj;
for (int i = 0; Grapple_TiledObjectAt(map, 2, i, &obj); i++)
    spawn_entity(obj.name, obj.type, obj.x, obj.y, obj.w, obj.h);

Grapple_FreeTiledMap(map);
```

The flat accessor API covers dimensions, layers (name/type), tile GIDs,
and objects; the full cute_tiled parse tree (tilesets, properties,
animations) stays reachable via `Grapple_TiledRaw(map)`.

Check `map` for NULL before using it; load failures set `SDL_GetError()`.
Input is validated as JSON before map parsing. Object strings and the raw tree
are borrowed from the map and become invalid after `Grapple_FreeTiledMap`.

C++ gets a RAII `TiledMap` wrapper (`grapple/tilemap.h`); Lua and Ruby
get `Grapple.load_map` with GC-managed map objects — see
[C++](cpp.md) and [Scripting](scripting.md).

Provenance and local fixes:
[`deps/cute_tiled.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/cute_tiled.md).
