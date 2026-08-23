# Grapple::Physics — static-only build of vendored Box2D v3.1.1 (MIT;
# provenance in deps/box2d.md) plus original debug-draw glue that renders
# collision shapes through the Grapple::Gfx GPU primitives.
#
# Box2D v3 is pure C11 with zero external dependencies; SIMD (SSE2/NEON)
# auto-detects with a scalar fallback, so every platform including web
# builds the same sources. The native b2* API is exposed directly.

enable_language(C)

file(GLOB BOX2D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(Grapple_Physics STATIC
  ${BOX2D_SOURCES}
  # Original Grapple code (not vendored): b2DebugDraw -> gpu* primitives.
  glue/grapple_physics_draw.c
  glue/grapple_physics_build.c
)
add_library(Grapple::Physics ALIAS Grapple_Physics)

if("c_std_11" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Physics PRIVATE c_std_11)
endif()

set_target_properties(Grapple_Physics PROPERTIES
  OUTPUT_NAME Grapple_Physics
  POSITION_INDEPENDENT_CODE ON
)

# SYSTEM: Box2D's public headers use C-style casts in inline helpers, which
# consumer TUs built with -Wold-style-cast -Werror would reject.
target_include_directories(Grapple_Physics SYSTEM
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(Grapple_Physics
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Box2D's wasm path uses Emscripten's SSE->WASM-SIMD128 compat intrinsics,
# which require these flags (upstream's own CMake does the same). WASM SIMD
# is supported by all modern browsers.
if(EMSCRIPTEN)
  target_compile_options(Grapple_Physics PRIVATE -msimd128 -msse2)
endif()

target_link_libraries(Grapple_Physics PUBLIC Grapple::Gfx)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Physics PRIVATE m)
endif()
