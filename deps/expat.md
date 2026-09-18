# Expat (vendored under `modules/mixer/src/expat/`)

| | |
|---|---|
| Upstream | https://github.com/libexpat/libexpat |
| Version | 2.8.4 (`R_2_8_4`) |
| Commit | `12cf0b1f25f026a022fe728ad8f7e3d017285b80` |
| Source | https://github.com/libexpat/libexpat/releases/download/R_2_8_4/expat-2.8.4.tar.xz |
| SHA-256 | `656ae1cc8da3b4ea513bb4e254f33e6243938084c0ec6239da873376b09985a7` |
| License | MIT; upstream build files retain their own notices (see `COPYING`) |
| Imported | 2026-09-11, direct source copy |

Imported `lib/`, `cmake/`, `win32/`, upstream CMake configuration files,
`COPYING`, `README.md`, `AUTHORS`, and `Changes`. No source modifications.
Tools, examples, tests, documentation generation, installation and shared
outputs are disabled by the parent component. The parser is compiled as C99
from these local sources, with no package-manager or network requirement.
Its objects are included in both aggregate SDK archives. Upstream platform
feature checks are compile/link checks, not execution of target binaries.

MusicXML uses namespace-aware Expat parsing and a bounded private tree. DTD
processing is disabled; common DOCTYPE declarations are accepted without
loading external subsets. Entity declarations and external entity references
are rejected. XML code runs only during import, never in the audio callback.

Upstream entropy detection and `EXPAT_WITH_*` build options remain available
to toolchain files. Desktop builds use upstream defaults; porting to a new
console requires reviewing these OS hooks and the SDL output backend rather
than assuming that desktop binaries or a system XML library are available.

To update, verify a new upstream release/hash, replace the same source subset,
update this ledger, and run XML tests, sanitizers, installed SDK consumer tests
and static-link audits on the supported platform matrix.
