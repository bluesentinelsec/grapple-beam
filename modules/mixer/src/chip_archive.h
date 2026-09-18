/* Original Grapple code (zlib). In-memory MXL extraction. */
#ifndef GRAPPLE_CHIP_ARCHIVE_H
#define GRAPPLE_CHIP_ARCHIVE_H
#include <SDL3/SDL.h>
#ifdef __cplusplus
extern "C"
{
#endif
    bool Chip_Inflate(const void *data, size_t size, void *output, size_t output_size);
    void *Chip_ReadMxl(const void *data, size_t size, size_t *output_size);
#ifdef __cplusplus
}
#endif
#endif
