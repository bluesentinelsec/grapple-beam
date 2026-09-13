---
title: Image
description: "Supported image formats, file and VFS loading, and surface/texture ownership."
---

# Image — `Grapple::Image`

A static-link-first port of SDL3_image (upstream base 3.4.4). The API is
SDL3_image-compatible for supported features. Format-specific APIs for removed
codecs are absent; generic loaders still report unsupported or malformed files
at runtime through a NULL result and `SDL_GetError()`.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Image)
```

```c
#include <SDL3_image/SDL_image.h>

SDL_Surface *hero = IMG_Load("hero.png");
SDL_Texture *tex  = IMG_LoadTexture(renderer, "tiles.png");
IMG_SavePNG(screenshot, "shot.png");

/* from a VFS-mounted archive: */
SDL_Surface *s = IMG_Load_IO(Grapple_OpenVFSRead("/assets/hero.png"), true);
```

Check returned pointers and save results. Free surfaces with `SDL_DestroySurface`,
textures with `SDL_DestroyTexture` before their renderer, and animations with the
matching animation destructor. VFS examples also include `<grapple/vfs.h>`;
`closeio=true` transfers stream ownership to the loader.

## Format support

| Format | Load | Save | Animation |
|--------|------|------|-----------|
| BMP | ✅ | ✅ | — |
| PNG | ✅ | ✅ | — |
| JPG | ✅ | ✅ | — |
| GIF | ✅ | ✅ | ✅ load + save |
| ANI | — | — | ✅ load + save |
| ICO / CUR | ✅ | ✅ | — |
| TGA | ✅ | ✅ | — |
| QOI | ✅ | ❌ | — |
| SVG | ✅ (incl. `IMG_LoadSizedSVG_IO`) | ❌ | — |
| PNM (PBM/PGM/PPM) | ✅ | ❌ | — |
| PCX | ✅ | ❌ | — |
| XPM / XV | ✅ | ❌ | — |

All codecs are vendored single-file C (stb_image, nanosvg, tiny_jpeg,
original encoders) — no libpng, no libjpeg, no zlib dependency. Formats
that required shared libraries upstream (AVIF, JXL, TIFF, WebP) are
removed, headers included.

TGA has no magic bytes: loading TGA from memory needs
`IMG_LoadTyped_IO(io, true, "TGA")` (file loads by `.tga` extension work
directly).

Provenance and change ledger:
[`deps/SDL3_image.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/SDL3_image.md).
