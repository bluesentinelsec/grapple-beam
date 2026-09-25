# Grapple::Platformer — an opinionated 2D platformer layer over the engine:
# a tile grid with dedicated level objects (floors, walls, blocks, stairs,
# ledges), a tuned player character, a pre-bound action map and a scrolling
# camera, run by a scene so a game has no loop to write.
#
# Original code (zlib). No Box2D: collision is an axis-aligned box swept
# against the grid, the model the NES and SNES platformers it imitates used,
# which is what gives it their feel. Levels also load from Tiled maps.
enable_language(C)

add_library(Grapple_Platformer STATIC
  src/grapple_platformer.c
  src/grapple_platformer_player.c
  src/grapple_platformer_tiled.c
)
add_library(Grapple::Platformer ALIAS Grapple_Platformer)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Platformer PRIVATE c_std_99)
endif()

set_target_properties(Grapple_Platformer PROPERTIES
  OUTPUT_NAME Grapple_Platformer
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Platformer
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(Grapple_Platformer
  PUBLIC Grapple::Engine SDL3::SDL3
  PRIVATE Grapple::Tiled
)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Platformer PRIVATE m)
endif()
