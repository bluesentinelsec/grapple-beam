/**
 * @file vfs.h
 * @brief Encrypted-asset glue over PhysFS (Grapple VFS).
 *
 * Original Grapple code (zlib) on top of the vendored PhysFS — use the
 * native PhysFS API (<physfs.h>) for everything else (init/deinit, mounts,
 * enumeration, reads). This header adds the pieces the asset pipeline
 * needs:
 *
 *   pack:  scripts/pack_assets.py assets/ media.zip --password "pw"
 *          (deterministic zip -> SSE1 encryption; --header embeds as C)
 *   ship:  the encrypted blob, as a file or embedded byte array
 *   load:  Grapple_MountEncryptedArchive(blob, size, "pw", "/assets")
 *          ...then read via PhysFS, or hand any mounted file straight to
 *          the Image/TTF/Mixer loaders through Grapple_OpenVFSRead().
 *
 * Plain unencrypted zips don't need this header at all: mount them with
 * PHYSFS_mount / PHYSFS_mountMemory directly.
 */
#ifndef GRAPPLE_VFS_H
#define GRAPPLE_VFS_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decrypt an SSE1 container (see grapple/crypto.h) holding a zip archive
 * and mount the plaintext image from memory at `mountPoint` (NULL or "/"
 * for the root). The decrypted image stays in memory until unmounted;
 * PhysFS must already be initialized.
 * \returns true on success; false on wrong password, tampered data, or a
 *          payload PhysFS cannot mount (see SDL_GetError()).
 */
extern bool Grapple_MountEncryptedArchive(const void *data, int dataSize,
                                            const char *password, const char *mountPoint);

/**
 * Convenience: load `path` from the real filesystem, then behave exactly
 * like Grapple_MountEncryptedArchive.
 */
extern bool Grapple_MountEncryptedArchiveFile(const char *path, const char *password,
                                                const char *mountPoint);

/**
 * Open a read-only SDL_IOStream over a file inside the mounted VFS —
 * size/seek/read all work, so the result feeds directly into
 * IMG_Load_IO, TTF_OpenFontIO, MIX_LoadAudio_IO, and friends.
 * Closing the stream closes the underlying PhysFS handle.
 * \returns a new SDL_IOStream (close with SDL_CloseIO), or NULL on error.
 */
extern SDL_IOStream *Grapple_OpenVFSRead(const char *vfsPath);

/**
 * Read an entire file from the mounted VFS into one buffer.
 * \returns a new SDL_malloc'd buffer (*size bytes; free with SDL_free),
 *          or NULL on error.
 */
extern unsigned char *Grapple_LoadVFSFile(const char *vfsPath, int *size);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_VFS_H */
