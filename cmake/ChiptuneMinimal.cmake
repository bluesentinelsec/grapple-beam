# Initial cache for a source-built C mixer/chiptune SDK. Normal -D options can override it.
# cmake -S . -B build/music -C cmake/ChiptuneMinimal.cmake
foreach(component ENGINE EXTRAS FORMATS GFX GUI HTTP IMAGE LIGHT LUA NET PHYSICS REGEX
                  RUBY TILED TTF VFS CPP)
  set(GRAPPLE_BUILD_${component} OFF CACHE BOOL "Minimal music SDK")
endforeach()
set(GRAPPLE_BUILD_MIXER ON CACHE BOOL "Minimal music SDK")
set(GRAPPLE_BUILD_SHARED_SDK OFF CACHE BOOL "Minimal music SDK")
foreach(feature APP TESTS BENCHMARKS DEMOS PRACTICE WEB_DEMO)
  set(GRAPPLE_BEAM_BUILD_${feature} OFF CACHE BOOL "Minimal music SDK")
endforeach()
foreach(dependency CLI11 JSON SPDLOG)
  set(GRAPPLE_BEAM_WITH_${dependency} OFF CACHE BOOL "Minimal music SDK")
endforeach()
