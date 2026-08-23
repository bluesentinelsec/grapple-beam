/**
 * @file compress.h
 * @brief DEFLATE compression for game data (Grapple Extras).
 *
 * Original Grapple wrapper (zlib) over the vendored sdefl/sinfl
 * single-headers (the same DEFLATE implementation raylib vendors), with
 * raylib's API shape. Output carries a small header ("SSZ1" + original
 * size), so decompression allocates exactly and validates input.
 *
 * Buffers are allocated with SDL_malloc; free results with SDL_free.
 */
#ifndef GRAPPLE_COMPRESS_H
#define GRAPPLE_COMPRESS_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compress a buffer (DEFLATE).
 * \returns a new SDL_malloc'd buffer (*compDataSize bytes), or NULL on error.
 */
extern unsigned char *Grapple_CompressData(const unsigned char *data, int dataSize,
                                             int *compDataSize);

/**
 * Decompress a buffer produced by Grapple_CompressData.
 * \returns a new SDL_malloc'd buffer (*dataSize bytes), or NULL on
 *          malformed/corrupted input (see SDL_GetError()).
 */
extern unsigned char *Grapple_DecompressData(const unsigned char *compData, int compDataSize,
                                               int *dataSize);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_COMPRESS_H */
