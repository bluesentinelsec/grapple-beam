include(FetchContent)
# Deliberately update this immutable pin, then rebuild and run the complete suite.
set(GAME_ENGINE_COMMIT "@ENGINE_COMMIT@")
foreach(option BUILD_APP BUILD_TESTS BUILD_BENCHMARKS BUILD_DEMOS BUILD_PRACTICE WITH_CLI11 WITH_JSON WITH_SPDLOG)
  set(GRAPPLE_BEAM_${option} OFF CACHE BOOL "" FORCE)
endforeach()
foreach(module SHARED_SDK TTF MIXER NET GUI LUA RUBY HTTP TILED REGEX CPP)
  set(GRAPPLE_BUILD_${module} OFF CACHE BOOL "" FORCE)
endforeach()
foreach(module ENGINE IMAGE EXTRAS GFX VFS FORMATS PHYSICS LIGHT)
  set(GRAPPLE_BUILD_${module} ON CACHE BOOL "" FORCE)
endforeach()
FetchContent_Declare(grapple_engine
  GIT_REPOSITORY https://github.com/bluesentinelsec/grapple-beam.git
  GIT_TAG "${GAME_ENGINE_COMMIT}"
  GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(grapple_engine)
if(NOT TARGET Grapple::Engine)
  message(FATAL_ERROR "Selected engine ref does not supply Grapple::Engine; use v0.9.0 or a compatible ref")
endif()
# v0.9.0's Engine target assumed the engine was the top-level project.
target_include_directories(Grapple_Engine PUBLIC "${grapple_engine_SOURCE_DIR}/include")
set_property(DIRECTORY "${grapple_engine_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)
