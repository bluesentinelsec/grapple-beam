# Grapple::Tiled — Tiled (.tmj JSON) map parsing via vendored cute_tiled
# (single dependency-free C header, dual zlib/public-domain; provenance in
# deps/cute_tiled.md) with VFS-aware loading and a flat accessor API the
# Lua/Ruby/C++ bindings wrap. Chosen over libtmx, whose libxml2 dependency
# fails the static-first rule.
enable_language(C)

add_library(Grapple_Tiled STATIC
  src/grapple_tiled.c
)
add_library(Grapple::Tiled ALIAS Grapple_Tiled)

set_target_properties(Grapple_Tiled PROPERTIES
  OUTPUT_NAME Grapple_Tiled
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Tiled SYSTEM
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(Grapple_Tiled PUBLIC SDL3::SDL3 Grapple::VFS PRIVATE Grapple::Formats)
