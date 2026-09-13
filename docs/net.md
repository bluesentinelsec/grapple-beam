---
title: Net
description: "TCP and UDP sockets via SDL3_net, statically built — the simplest port in the stack."
---

# Net — `Grapple::Net`

SDL3_net 3.2.0 (the `NET_*` API), statically built. One portable C file
over OS sockets with zero third-party dependencies; the full API surface
ships unchanged. For HTTP/S, see the [mog HTTP client](http.md).

SDL3_net connections are asynchronous. Initialize with `NET_Init`, resolve a
hostname with `NET_ResolveHostname`, and inspect `NET_GetAddressStatus` (or wait
with a bounded `NET_WaitUntilResolved` during loading). Pass a resolved address
to `NET_CreateClient(address, game_port, 0)`, then check connection status before
sending. Avoid blocking waits in the game loop.

Release sockets with `NET_DestroyStreamSocket`, address references with
`NET_UnrefAddress`, and call `NET_Quit` after networking work finishes. NULL
results and failed statuses carry diagnostics through `SDL_GetError()`.
TCP here is a byte stream: define framing for game messages. It does not add TLS;
use [HTTP/S](http.md) for HTTPS requests.

## Capability notes

| Area | Status |
|------|--------|
| Address resolution (async, refcounted) | ✅ full |
| TCP stream sockets (client/server) | ✅ full |
| UDP datagram sockets | ✅ full |
| Multi-socket wait (`NET_WaitUntilInputAvailable`) | ✅ full |
| Windows | ✅ links OS-built-in `iphlpapi`/`ws2_32` only |
| Web / Emscripten | ❌ not built — upstream ships only a stub there, and this project does not ship stubs; `GRAPPLE_BUILD_NET` is forced OFF for web builds |

Tests are fully self-contained over 127.0.0.1: TCP echo both directions,
UDP round-trips, multi-socket readiness, clean failure paths.

Provenance:
[`deps/SDL3_net.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/SDL3_net.md).
