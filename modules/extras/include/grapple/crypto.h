/**
 * @file crypto.h
 * @brief Password-based encryption for game assets (Grapple Extras).
 *
 * Original Grapple code (zlib). Self-contained implementations of
 * well-specified primitives — no OS crypto libraries, fully static:
 *   ChaCha20 (RFC 8439) for encryption, PBKDF2-HMAC-SHA256 for key
 *   derivation, and an encrypt-then-MAC HMAC-SHA256 tag for integrity —
 * so a wrong password or a tampered file is *detected*, not decrypted
 * into garbage.
 *
 * Intended for encrypting/obfuscating media assets with a password.
 * Honest scope note: salts/nonces come from timing entropy, not an OS
 * CSPRNG, which is fine for asset protection but don't build a messaging
 * app on this.
 *
 * Buffers are allocated with SDL_malloc; free results with SDL_free.
 *
 *   int size = 0;
 *   unsigned char *enc = Grapple_EncryptData(data, dataSize, "hunter2", &size);
 *   ...
 *   int plainSize = 0;
 *   unsigned char *plain = Grapple_DecryptData(enc, size, "hunter2", &plainSize);
 *   // plain == NULL on wrong password or corruption (SDL_GetError says which)
 */
#ifndef GRAPPLE_CRYPTO_H
#define GRAPPLE_CRYPTO_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encrypt a buffer with a password.
 * Output layout: magic "SSE1" | salt(16) | nonce(12) | ciphertext | tag(32).
 * \returns a new SDL_malloc'd buffer (*outputSize bytes), or NULL on error.
 */
extern unsigned char *Grapple_EncryptData(const unsigned char *data, int dataSize,
                                            const char *password, int *outputSize);

/**
 * Decrypt a buffer produced by Grapple_EncryptData.
 * \returns a new SDL_malloc'd buffer (*outputSize bytes), or NULL if the
 *          password is wrong, the data was tampered with, or input is
 *          malformed (see SDL_GetError()).
 */
extern unsigned char *Grapple_DecryptData(const unsigned char *data, int dataSize,
                                            const char *password, int *outputSize);

/** SHA-256 of a buffer (also handy for content addressing / integrity). */
extern bool Grapple_SHA256(const void *data, size_t dataSize, Uint8 digest[32]);

/** HMAC-SHA256 keyed hash. */
extern bool Grapple_HMACSHA256(const void *key, size_t keySize, const void *data,
                                 size_t dataSize, Uint8 digest[32]);

/**
 * Run the internal primitives against their published test vectors
 * (ChaCha20 RFC 8439 §2.4.2, PBKDF2-HMAC-SHA256 standard vectors).
 * \returns true if every vector matches. Cheap; suitable for a startup
 * sanity check on exotic platforms.
 */
extern bool Grapple_CryptoSelfTest(void);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_CRYPTO_H */
