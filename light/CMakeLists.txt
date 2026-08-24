# Grapple::Light — dynamic 2D lighting: a day/night ambient cycle,
# coloured lights, and hard shadows cast by the level's geometry.
#
#   <grapple/light.h>  light map compositing, occluders, gameplay queries
#
# Original code (zlib), plain C over SDL3's renderer — no shaders and no
# GPU API, so it runs everywhere the rest of the project does.
enable_language(C)

# The GL path loads every entry point through SDL_GL_GetProcAddress, so
# nothing here links against libGL and the archive stays dependency-free.
add_library(Grapple_Light STATIC
  src/grapple_light.c
  src/grapple_light_gl.c
)
add_library(Grapple::Light ALIAS Grapple_Light)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Light PRIVATE c_std_99)
endif()

set_target_properties(Grapple_Light PROPERTIES
  OUTPUT_NAME Grapple_Light
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Light
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(Grapple_Light PUBLIC SDL3::SDL3)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Light PRIVATE m)
endif()
