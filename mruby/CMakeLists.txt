# Grapple::Ruby — static-only build of mruby 4.0.0 (MIT; provenance in
# deps/mruby.md). mruby normally requires a host Ruby + rake to GENERATE
# code before compiling; this tree commits that generated output (option
# "b"), harvested by scripts/harvest_mruby.py, so plain CMake builds it
# everywhere with no host Ruby. Original glue adds Kernel#require /
# #require_relative / #load (mruby has none) with $LOAD_PATH,
# $LOADED_FEATURES, .rb + .mrb resolution, circular-require guard, and
# PhysFS VFS integration — plus Kernel#puts/print/p (no IO gem needed).

enable_language(C)

file(GLOB MRUBY_CORE ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c)
file(GLOB MRUBY_GEMS ${CMAKE_CURRENT_SOURCE_DIR}/src/gems/*.c)
add_library(Grapple_Ruby STATIC
  ${MRUBY_CORE}
  ${MRUBY_GEMS}
  glue/grapple_ruby.c
)
add_library(Grapple::Ruby ALIAS Grapple_Ruby)

set_target_properties(Grapple_Ruby PROPERTIES
  OUTPUT_NAME Grapple_Ruby
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Ruby SYSTEM
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(Grapple_Ruby PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gems
)

target_link_libraries(Grapple_Ruby PUBLIC SDL3::SDL3 Grapple::VFS)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Ruby PRIVATE m)
endif()
