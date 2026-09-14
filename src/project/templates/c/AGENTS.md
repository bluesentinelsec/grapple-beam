# Working on @GAME_ID@

This is a C17 game. `src/main.c` is the thin application entrypoint; reusable
logic belongs under `src/@GAME_ID@/`, public headers under `include/@GAME_ID@/`.
CMake recursively discovers .c files within that game directory. Add a source
file and build; no source-list edit or per-directory CMakeLists is needed.
Keep platform-specific variants, scratch code, dependencies and test entrypoints
outside that portable root. The engine repository's explicit-list rule does not
apply to this generated game.

Use `make build` (the default), `make build-release`, `make test`, `make fmt`, and
`make clean`; Windows uses the corresponding build.bat operations. Keep the five
operation interface. Packaging belongs to `grapple-beam package`.

Write clear C APIs with explicit ownership, checked errors and Doxygen comments
on public symbols. Microsoft clang-format controls layout. Game warnings are
errors; third-party code must not inherit them. Prefer small components and
meaningful C tests. Do not use C++ syntax or introduce C++ test authoring.

Tests exercise real functionality and must finish or time out. Test-mode startup
skips player data; never modify normal preferences or saves in tests. Register
new tests in tests/CMakeLists.txt with external CTest timeouts. Rendering tests
must inspect actual assets/state/pixels instead of only accepting successful init.

The dependency pin is intentional; do not replace it with a moving branch.
Update notices when adding dependencies or media. Keep authored files out of
reserved build/ and dist/ output directories. Review README and player docs when
behavior changes. Do not automatically publish, sign or change repository visibility.
