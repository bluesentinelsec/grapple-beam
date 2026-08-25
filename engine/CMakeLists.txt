# Grapple::Engine — an opinionated game engine: the frame loop, scenes,
# actors, rendering, input and assets.
#
#   <grapple/engine.h>           the loop, time, and the drawing surface
#   <grapple/engine_scene.h>     the scene stack
#   <grapple/engine_graphics.h>  graphics settings, config.toml and the CLI
#   <grapple/engine_camera.h>    2D camera, scrolling and split screen
#   <grapple/engine_media.h>     mounting the game's assets
#   <grapple/engine_actor.h>     actors: identity, hierarchy, messages
#   <grapple/engine_render.h>    sprites, z-ordering and culling
#   <grapple/engine_input.h>     polled keyboard, mouse, gamepads, touch
#   <grapple/engine_binding.h>   actions and rebindable controls
#   <grapple/engine_physics.h>   Box2D bodies on actors
#   <grapple/engine_assets.h>    texture loading, blocking and streamed
#   <grapple/engine_light.h>     lighting presets and actor-attached lights
#   <grapple/engine_save.h>      atomic save slots
#   <grapple/engine_text.h>      localisation string tables
#   <grapple/engine_config.h>    heap builders, for script bindings
#   <grapple/engine_script.h>    the loop contract for scripts
#
# Original code (zlib), plain C over SDL3. The loop is a fixed simulation
# tick with interpolated rendering — the arrangement that makes motion look
# smooth when the tick rate and the display's refresh rate disagree.
enable_language(C)

add_library(Grapple_Engine STATIC
  src/grapple_engine.c
  src/grapple_engine_scene.c
  src/grapple_engine_graphics.c
  src/grapple_engine_gfxconfig.c
  src/grapple_engine_target.c
  src/grapple_engine_postfx.c
  src/grapple_engine_camera.c
  src/grapple_engine_media.c
  src/grapple_engine_actor.c
  src/grapple_engine_render.c
  src/grapple_engine_input.c
  src/grapple_engine_binding.c
  src/grapple_engine_physics.c
  src/grapple_engine_assets.c
  src/grapple_engine_light.c
  src/grapple_engine_save.c
  src/grapple_engine_text.c
  src/grapple_engine_config.c
  src/grapple_engine_script.c
  src/grapple_gpu_build.c
)
add_library(Grapple::Engine ALIAS Grapple_Engine)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Engine PRIVATE c_std_99)
endif()

set_target_properties(Grapple_Engine PROPERTIES
  OUTPUT_NAME Grapple_Engine
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # grapple/event_sink.h: shared by modules that must not depend
    # on each other.
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Formats for tomlc99: config.toml is part of the settings contract, not an
# optional extra, so the dependency is unconditional rather than a #ifdef.
# Formats for tomlc99 (config.toml) and VFS for PhysFS (the media archive).
# Both are part of the engine's contract rather than optional extras: an
# opinionated engine that made you mount your own assets would not be one.
target_link_libraries(Grapple_Engine
  PUBLIC SDL3::SDL3
  PRIVATE Grapple::Formats Grapple::VFS Grapple::Physics Grapple::Image Grapple::Light
)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Engine PRIVATE m)
endif()
