---
title: VFS
description: "PhysFS virtual filesystem with an encrypted-asset pipeline: pack, encrypt, embed, and mount your game files from one sealed archive."
---

# VFS — `Grapple::VFS`

PhysFS 3.2.0, built static with only the **ZIP** and real-directory
backends (the other archive formats were deleted, not stubbed), plus
original glue for a complete encrypted-asset pipeline.

```cmake
target_link_libraries(your_game PRIVATE Grapple::VFS)
```

## The asset pipeline

```
assets/  ──pack_assets.py──▶  media.zip  ──(--password)──▶  media.bin (SSE1)
                                                              │
                                            ship as a file, or --header
                                            embeds it as a C byte array
```

```bash
python3 scripts/pack_assets.py assets/ media.bin --password "openSesame" \
        --header media.h --symbol game_media
```

The zip is deterministic (sorted entries, fixed timestamps): repacking
unchanged assets yields byte-identical plain ZIPs. Encryption uses new salt/nonce
values, so encrypted output is not byte-identical. Encrypting the whole
zip also hides file names — something classic zip passwords never did.

## Mounting at runtime

```c
#include <physfs.h>
#include <grapple/vfs.h>
#include "media.h"

PHYSFS_init(argv[0]);

/* decrypt + mount the embedded image from memory, one call: */
Grapple_MountEncryptedArchive(game_media, (int)game_media_len,
                                "openSesame", "/assets");

/* plain zips need no glue at all: */
PHYSFS_mount("mods/extra.zip", "/mods", 1);

/* read whole files... */
int size = 0;
unsigned char *level = Grapple_LoadVFSFile("/assets/levels/1.json", &size);

/* ...or stream mounted assets straight into the other modules: */
SDL_Surface *hero = IMG_Load_IO(Grapple_OpenVFSRead("/assets/hero.png"), true);
MIX_Audio *bgm = MIX_LoadAudio_IO(mixer,
    Grapple_OpenVFSRead("/assets/bgm.ogg"), false, true);
```

Check initialization, mount, stream, and load results. Release `level` with
`SDL_free`, surfaces with `SDL_DestroySurface`, and audio with `MIX_DestroyAudio`
after dependent tracks are released. Close open streams and release mounted-file
users before `PHYSFS_deinit`. The snippets assume SDL/SDL_mixer initialization
and the appropriate image/mixer headers.

The runner accepts readable media directories/archives via `--media`. Encrypted
media and passwords are provisioned through the embedding API; see
[asset mounting](engine.md#asset-mounting). Raw filename loaders need an `_IO`
stream adapter to read PhysFS paths.

A wrong password or a tampered blob returns `false` with
`SDL_GetError()` set — the SSE1 container authenticates (encrypt-then-MAC)
*before* mounting, so you never get a half-mounted archive. See
[Extras](extras.md) for the underlying crypto.

Everything else is the native PhysFS API: search-path layering,
`PHYSFS_enumerateFiles`, `PHYSFS_openRead`, write directories, and so on.
The [Tiled](tiled.md) module and both [script runtimes](scripting.md)
(`require`/`load` from mounted archives) are VFS-aware out of the box.

Provenance:
[`deps/physfs.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/physfs.md).
