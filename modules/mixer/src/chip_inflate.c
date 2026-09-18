/* Original Grapple code (zlib). Reuse PhysFS's vendored public-domain inflater. */
#include "chip_archive.h"

#include <stdlib.h>
#include <string.h>

typedef Uint8 PHYSFS_uint8;
typedef Sint16 PHYSFS_sint16;
typedef Uint16 PHYSFS_uint16;
typedef Uint32 PHYSFS_uint32;
typedef Uint64 PHYSFS_uint64;
#define MINIZ_LITTLE_ENDIAN (SDL_BYTEORDER == SDL_LIL_ENDIAN)
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 0
#include <physfs_miniz.h>

bool Chip_Inflate(const void *data, size_t size, void *output, size_t output_size)
{
    tinfl_decompressor *state = SDL_calloc(1, sizeof(*state));
    if (!state)
        return false;
    tinfl_init(state);
    size_t in = size, out = output_size;
    const tinfl_status result = tinfl_decompress(state, data, &in, output, output, &out,
                                                 TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    SDL_free(state);
    if (result != TINFL_STATUS_DONE || in != size || out != output_size)
        return SDL_SetError("MusicXML: invalid deflated MXL entry");
    return true;
}
