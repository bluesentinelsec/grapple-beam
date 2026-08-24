# Grapple::TTF — static-only build of the vendored SDL_ttf sources
# (upstream release-3.2.2; provenance in deps/SDL3_ttf.md).
#
# Backend: minimal static FreeType via pinned FetchContent (deps/FreeType.md —
# internal zlib, no harfbuzz/png/brotli/bzip2, so the link-time dependency set
# stays empty). HarfBuzz shaping and plutosvg color emoji are disabled and
# their nine dependent public APIs are deleted (no-stubs; deps/SDL3_ttf.md).

# The root project is CXX-only; the vendored SDL_ttf sources are C.
# (HarfBuzz is C++ with a C API; it needs no exceptions or RTTI.)
enable_language(C)

add_library(Grapple_TTF STATIC
  # Vendored HarfBuzz (Old MIT; deps/harfbuzz.md): official single-TU
  # amalgamation, C++ but built with no exceptions/RTTI required.
  harfbuzz/harfbuzz.cc
  # Vendored SheenBidi (Apache-2.0; deps/sheenbidi.md): unity build.
  sheenbidi/Source/SheenBidi.c
  # Grapple addition (ours, zlib): UBA paragraph itemization helper.
  src/grapple_bidi.c
  src/SDL_ttf.c
  src/SDL_hashtable.c
  src/SDL_hashtable_ttf.c
  src/SDL_gpu_textengine.c
  src/SDL_renderer_textengine.c
  src/SDL_surface_textengine.c
  # Grapple additions (ours, zlib): embedded-font debug text overlay.
  src/grapple_debug_text.c
)
add_library(Grapple::TTF ALIAS Grapple_TTF)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_TTF PRIVATE c_std_99)
endif()

set_target_properties(Grapple_TTF PROPERTIES
  OUTPUT_NAME Grapple_TTF
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_TTF
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/harfbuzz
    ${CMAKE_CURRENT_SOURCE_DIR}/sheenbidi/Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/sheenbidi/Source
)

target_compile_definitions(Grapple_TTF PRIVATE
  TTF_USE_HARFBUZZ=1
  HAVE_FREETYPE=1        # harfbuzz.cc: enable hb-ft
  HB_NO_MMAP=1           # no platform mmap paths; keep the TU portable
  SB_CONFIG_UNITY=1      # SheenBidi single-TU build
)

target_link_libraries(Grapple_TTF
  PUBLIC SDL3::SDL3
  PRIVATE freetype
)

# Vendored upstream C: build quietly rather than under the project's
# warnings-as-errors regime; our own code around it stays strict.
