---
title: Extras
description: "Password-based asset encryption, DEFLATE compression, base64, and Godot-style signals — the glue every game ends up needing."
---

# Extras — `Grapple::Extras`

Game infrastructure outside the SDL satellite family that most games end
up needing anyway. Plain C over SDL3 with zero external dependencies (no
libcrypto, no zlib) — fully static everywhere, web included. All returned
buffers are `SDL_malloc`'d; free them with `SDL_free`.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Extras)
```

## Crypto — `<grapple/crypto.h>`

Password-based asset encryption: ChaCha20 (RFC 8439) + PBKDF2-HMAC-SHA256
key derivation + encrypt-then-MAC (HMAC-SHA256), implemented from spec
and pinned to published test vectors (`Grapple_CryptoSelfTest()`
re-checks them at runtime). A wrong password or tampered file is
**detected**, not decrypted into garbage.

```c
int n = 0;
unsigned char *enc = Grapple_EncryptData(bytes, size, "password", &n);
unsigned char *dec = Grapple_DecryptData(enc, n, "password", &size);
```

Also exposes `Grapple_SHA256` / `Grapple_HMACSHA256`. Scope honesty:
salts/nonces come from timing entropy, not an OS CSPRNG — right for
protecting shipped assets, not for building a messaging app.

## Compression — `<grapple/compress.h>`

DEFLATE with the raylib API shape, backed by vendored sdefl/sinfl:

```c
int compSize = 0, size = 0;
unsigned char *comp = Grapple_CompressData(data, dataSize, &compSize);
unsigned char *back = Grapple_DecompressData(comp, compSize, &size);
```

The container records the original size and an adler32 checksum, so
decompression allocates exactly and rejects corrupted input.

## Base64 — `<grapple/base64.h>`

```c
char *text = Grapple_EncodeDataBase64(data, dataSize, &outSize);
unsigned char *data2 = Grapple_DecodeDataBase64(text, &outSize);
```

RFC 4648, strict decoding (whitespace skipped, anything else rejected).

## Reading text files — `<grapple/textfile.h>`

`SDL_LoadFile` returns a `void*` plus a size through an out-parameter — a
pair that cannot cross a Lua or Ruby binding boundary, so scripts had no
way to read a file at all. `Grapple_LoadTextFile(path)` returns one
NUL-terminated string instead: scripts receive an ordinary string (freed
for them automatically), and C callers own the allocation and release it
with `SDL_free`.

```lua
local text = GrappleC.LoadTextFile(path)
if text then parse(text) end
```

## File dialogs — `<grapple/dialog.h>`

Native open/save dialogs, polled rather than called back. SDL delivers the
chosen file through a callback and takes an array of filters — neither
crosses a Lua/Ruby binding boundary, and a callback is awkward inside an
immediate-mode frame loop even in C:

```c
if (nk_button_label(ctx, "Open")) {
    Grapple_ShowOpenFileDialog(window, "Text files", "txt", NULL);
}
switch (Grapple_DialogStatus()) {
case GRAPPLE_DIALOG_ACCEPTED:
    load(Grapple_DialogPath());
    Grapple_DialogReset();
    break;
case GRAPPLE_DIALOG_CANCELLED:
case GRAPPLE_DIALOG_ERROR:
    Grapple_DialogReset();
    break;
default:
    break;  /* PENDING: the dialog is still open */
}
```

`Grapple_ShowSaveFileDialog` is the Save As half. One dialog is tracked
at a time, which is all a modal picker can be.

### On the web

SDL ships cocoa, windows, unix (portal) and android backends but none for
Emscripten, so this module implements one over browser APIs. The state
machine above is identical everywhere; two things differ by necessity:

- **User activation** gates both halves. Browsers only open a picker or
  start a download from inside the real click handler, and an SDL app
  handles clicks a frame later — so calling these functions directly from a
  Nuklear button works in Firefox (which allows a few seconds of grace) and
  silently does nothing in Safari. For GUI apps use
  `Grapple_GuiOpenFileButton` / `Grapple_GuiSaveFileButton`, which park
  a transparent DOM element over the button so the user clicks the browser's
  own element; see [gui.md](gui.md). The functions below remain the right
  tool on desktop and for non-GUI callers.
- **Opening** uses a hidden `<input type="file">`. The chosen file is copied
  into the Emscripten filesystem and reported as a normal path, so
  `SDL_LoadFile(Grapple_DialogPath())` works unchanged. Browsers report a
  chosen file but never a dismissal, so a cancelled picker simply leaves the
  dialog `PENDING` — offer the user another way out rather than blocking on
  it. The picker also needs user activation, so call it from a real click.
- **Saving** has no picker at all, because a page cannot write to the user's
  disk. The save dialog resolves immediately to a path in the virtual
  filesystem; write your file there, then call
  `Grapple_DialogDeliverSave(path)` to hand it to the user as a browser
  download. On desktop that call is a no-op returning true, so one code path
  serves both:

```c
if (SDL_SaveFile(path, data, size)) {
    Grapple_DialogDeliverSave(path);   /* no-op on desktop */
}
```

## Signals — `<grapple/signals.h>`

Godot-style events:

```c
Grapple_SignalEmitter *door = Grapple_CreateSignalEmitter();
Uint64 id = Grapple_ConnectSignal(door, "opened", on_opened, hud, 0);
Grapple_EmitSignal(door, "opened", &args);
Grapple_DisconnectSignal(door, id);
```

Godot-matching semantics: in-order delivery, connections made during an
emission don't fire in it, disconnect-during-emit (including self) is
safe, `GRAPPLE_SIGNAL_ONESHOT` auto-disconnects, emission is
reentrant. Single-threaded by design, like a game loop.

Crypto primitives are pinned to published vectors (FIPS 180-4, RFC 4231,
RFC 8439) in the test suite, with an independent Python implementation of
the SSE1 container cross-validated against the C one.

Provenance for the vendored DEFLATE:
[`deps/sdefl.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/sdefl.md).
