cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# Single source of truth for the package version: the VERSION file at repo root.
# Edit that file only; CMake, --version, tests, and release checks all consume it.
# ---------------------------------------------------------------------------
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" GRAPPLE_BEAM_VERSION_RAW
  LIMIT_COUNT 1
)
string(STRIP "${GRAPPLE_BEAM_VERSION_RAW}" GRAPPLE_BEAM_VERSION)
# Allow optional leading 'v' and ignore anything after '#' on the line.
string(REGEX REPLACE "^[vV]" "" GRAPPLE_BEAM_VERSION "${GRAPPLE_BEAM_VERSION}")
string(REGEX REPLACE "[ \t]*#.*" "" GRAPPLE_BEAM_VERSION "${GRAPPLE_BEAM_VERSION}")
string(STRIP "${GRAPPLE_BEAM_VERSION}" GRAPPLE_BEAM_VERSION)
if(NOT GRAPPLE_BEAM_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+([.-].*)?$")
  message(FATAL_ERROR
    "VERSION file must contain a semantic version like 0.1.0 "
    "(got '${GRAPPLE_BEAM_VERSION}')"
  )
endif()

project(grapple-beam
  VERSION ${GRAPPLE_BEAM_VERSION}
  DESCRIPTION "A static-first game engine on SDL3, with Lua and Ruby"
  LANGUAGES CXX
)

# True when this project is the top-level CMake project (not add_subdirectory /
# FetchContent). Downstream consumers should not inherit app/tests/bench defaults.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(GRAPPLE_BEAM_IS_TOP_LEVEL ON)
else()
  set(GRAPPLE_BEAM_IS_TOP_LEVEL OFF)
endif()

# Exposed to version templates (configure_file @ONLY).
#
# The C++ namespace stays grapple for now: renaming it changes
# every consumer's `grapple::Version()` for no functional gain,
# and belongs with the rest of the internal rename rather than with the brand.
# See the rename issue for the staging.
set(PROJECT_NAMESPACE "grapple")
set(PROJECT_VERSION_STRING "${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Top-level-only settings so embedding via add_subdirectory/FetchContent stays clean.
if(GRAPPLE_BEAM_IS_TOP_LEVEL)
  # Export a compilation database for clangd and other LSP tools.
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Put binaries under build/<config>/bin (and libs under lib/) so an executable
  # named like a component directory (e.g. project "calc" + src/calc/) never
  # collides with a source/build subdirectory path.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

  # Default to Debug when the user does not pass CMAKE_BUILD_TYPE (single-config).
  if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Absolute paths: generic names like "Dependencies" collide with a consuming
# parent project's identically named modules when this repo is embedded via
# FetchContent/add_subdirectory (CMAKE_MODULE_PATH searches the parent first).
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sanitizers.cmake")

# Preferred third-party libraries (FetchContent). On for top-level apps; off when
# embedded (and for the Android Prefab AAR and iOS XCFramework and web/Emscripten package, whose public
# contract is the dependency-free core C++ API).
set(GRAPPLE_BEAM_DEFAULT_WITH_OPTIONAL_DEPS ${GRAPPLE_BEAM_IS_TOP_LEVEL})
if(ANDROID OR IOS OR EMSCRIPTEN)
  set(GRAPPLE_BEAM_DEFAULT_WITH_OPTIONAL_DEPS OFF)
endif()
option(GRAPPLE_BEAM_WITH_CLI11 "CLI argument parsing via CLI11" ${GRAPPLE_BEAM_DEFAULT_WITH_OPTIONAL_DEPS})
option(GRAPPLE_BEAM_WITH_JSON "JSON parsing via nlohmann/json" ${GRAPPLE_BEAM_DEFAULT_WITH_OPTIONAL_DEPS})
option(GRAPPLE_BEAM_WITH_SPDLOG "Console/file logging via spdlog" ${GRAPPLE_BEAM_DEFAULT_WITH_OPTIONAL_DEPS})

# When embedded, skip app/tests/benchmarks unless the consumer opts in. Platform
# package consumers receive the C++ library as a packaged artifact; repository
# tests run in the platform test application instead of CTest (browser tests
# under tests/web keep BUILD_TESTS meaningful for Emscripten).
set(GRAPPLE_BEAM_DEFAULT_BUILD_APP ${GRAPPLE_BEAM_IS_TOP_LEVEL})
set(GRAPPLE_BEAM_DEFAULT_BUILD_TESTS ${GRAPPLE_BEAM_IS_TOP_LEVEL})
set(GRAPPLE_BEAM_DEFAULT_BUILD_BENCHMARKS ${GRAPPLE_BEAM_IS_TOP_LEVEL})
if(ANDROID OR IOS)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_APP OFF)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_TESTS OFF)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_BENCHMARKS OFF)
endif()
if(EMSCRIPTEN)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_APP OFF)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_BENCHMARKS OFF)
endif()
option(GRAPPLE_BEAM_BUILD_APP "Build the demo application executable" ${GRAPPLE_BEAM_DEFAULT_BUILD_APP})
option(GRAPPLE_BEAM_BUILD_TESTS "Build unit tests" ${GRAPPLE_BEAM_DEFAULT_BUILD_TESTS})
option(GRAPPLE_BEAM_BUILD_BENCHMARKS "Build benchmarks" ${GRAPPLE_BEAM_DEFAULT_BUILD_BENCHMARKS})
# Shared libraries, on the platforms where a shared library is what people
# expect. Not iOS (apps ship static, and a dynamic framework is a different
# packaging format), not Android (the AAR already publishes a .so, built its
# own way), not Emscripten (wasm has no comparable loading model). Those
# platforms get their own idiomatic artifact instead, which is the point.
set(GRAPPLE_BEAM_DEFAULT_SHARED_SDK ${GRAPPLE_BEAM_IS_TOP_LEVEL})
if(ANDROID OR IOS OR EMSCRIPTEN)
  set(GRAPPLE_BEAM_DEFAULT_SHARED_SDK OFF)
endif()
option(GRAPPLE_BUILD_SHARED_SDK
       "Build the C and C++ APIs as shared libraries as well as static"
       ${GRAPPLE_BEAM_DEFAULT_SHARED_SDK})

# Everything that ends up inside a shared library has to be position
# independent, and that includes SDL3 and every vendored dependency — so this
# is set before they are configured rather than on our own targets after.
if(GRAPPLE_BUILD_SHARED_SDK)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # Precompiled headers are fine again: the __@@_PchSym_ entries that broke
  # the unfiltered export list are dropped by the export filter, which keeps
  # only names matching our public prefixes. See cmake/FilterExportDef.cmake.
endif()

# HTML5 canvas demo page for top-level Emscripten builds (see src/web/).
option(GRAPPLE_BEAM_BUILD_WEB_DEMO "Build the web canvas demo (Emscripten only)" ${GRAPPLE_BEAM_IS_TOP_LEVEL})

# SDL extension libraries (design: default ON; consumers disable what they
# do not want). Each extension is a static library under Grapple::.
option(GRAPPLE_BUILD_IMAGE "Build the Grapple::Image extension" ON)
option(GRAPPLE_BUILD_TTF "Build the Grapple::TTF extension" ON)
option(GRAPPLE_BUILD_MIXER "Build the Grapple::Mixer extension" ON)
# Upstream SDL_net has no real Emscripten support (it ships a stub build);
# per the no-stubs policy, Net simply does not exist on web builds.
if(EMSCRIPTEN)
  set(GRAPPLE_BUILD_NET OFF)
else()
  option(GRAPPLE_BUILD_NET "Build the Grapple::Net extension" ON)
endif()
option(GRAPPLE_BUILD_GFX "Build the Grapple::Gfx extension" ON)
option(GRAPPLE_BUILD_EXTRAS "Build the Grapple::Extras modules" ON)
option(GRAPPLE_BUILD_FORMATS "Build the Grapple::Formats parsers (JSON/TOML/YAML)" ON)
option(GRAPPLE_BUILD_VFS "Build the Grapple::VFS virtual filesystem (PhysFS)" ON)
option(GRAPPLE_BUILD_PHYSICS "Build the Grapple::Physics engine (Box2D)" ON)
option(GRAPPLE_BUILD_GUI "Build the Grapple::GUI toolkit (Nuklear)" ON)
option(GRAPPLE_BUILD_LUA "Build the Grapple::Lua interpreter" ON)
option(GRAPPLE_BUILD_RUBY "Build the Grapple::Ruby interpreter (mruby)" ON)
option(GRAPPLE_BUILD_CPP "Build the C++ bindings (RAII, no exceptions)" ON)
option(GRAPPLE_BUILD_TILED "Build the Grapple::Tiled map parser (cute_tiled)" ON)
option(GRAPPLE_BUILD_REGEX "Build the Grapple::Regex engine (Oniguruma)" ON)
option(GRAPPLE_BUILD_LIGHT "Build the Grapple::Light 2D lighting engine" ON)
option(GRAPPLE_BUILD_ENGINE "Build the Grapple::Engine game engine" ON)
# mog's C API and server are not supported in browsers.
if(EMSCRIPTEN)
  set(GRAPPLE_BUILD_HTTP OFF)
else()
  option(GRAPPLE_BUILD_HTTP "Build the Grapple::Http client/server (mog)" ON)
endif()

if(ANDROID AND (GRAPPLE_BEAM_BUILD_APP OR GRAPPLE_BEAM_BUILD_TESTS OR GRAPPLE_BEAM_BUILD_BENCHMARKS))
  message(FATAL_ERROR
    "Android builds provide the grapple Prefab library; set GRAPPLE_BEAM_BUILD_APP, "
    "GRAPPLE_BEAM_BUILD_TESTS, and GRAPPLE_BEAM_BUILD_BENCHMARKS to OFF. "
    "Use the android Gradle project to build and run device tests."
  )
endif()

if(ANDROID AND (GRAPPLE_BEAM_WITH_CLI11 OR GRAPPLE_BEAM_WITH_JSON OR GRAPPLE_BEAM_WITH_SPDLOG))
  message(FATAL_ERROR
    "The Android Prefab AAR contains the dependency-free core API; set "
    "GRAPPLE_BEAM_WITH_CLI11, GRAPPLE_BEAM_WITH_JSON, and GRAPPLE_BEAM_WITH_SPDLOG to OFF"
  )
endif()
if(IOS AND (GRAPPLE_BEAM_BUILD_APP OR GRAPPLE_BEAM_BUILD_TESTS OR GRAPPLE_BEAM_BUILD_BENCHMARKS))
  message(FATAL_ERROR
    "iOS builds provide the grapple XCFramework library; set GRAPPLE_BEAM_BUILD_APP, "
    "GRAPPLE_BEAM_BUILD_TESTS, and GRAPPLE_BEAM_BUILD_BENCHMARKS to OFF. "
    "Use the iOS package test application for Simulator tests."
  )
endif()

if(IOS AND (GRAPPLE_BEAM_WITH_CLI11 OR GRAPPLE_BEAM_WITH_JSON OR GRAPPLE_BEAM_WITH_SPDLOG))
  message(FATAL_ERROR
    "The iOS XCFramework contains the dependency-free core API; set "
    "GRAPPLE_BEAM_WITH_CLI11, GRAPPLE_BEAM_WITH_JSON, and GRAPPLE_BEAM_WITH_SPDLOG to OFF"
  )
endif()
if(EMSCRIPTEN AND (GRAPPLE_BEAM_BUILD_APP OR GRAPPLE_BEAM_BUILD_BENCHMARKS))
  message(FATAL_ERROR
    "Browser builds provide the static grapple library, the web demo, and "
    "browser tests; set GRAPPLE_BEAM_BUILD_APP and GRAPPLE_BEAM_BUILD_BENCHMARKS to OFF"
  )
endif()
# ASan + UBSan for project targets (intended for Linux GCC/Clang; see make sanitizer).
option(GRAPPLE_BEAM_ENABLE_SANITIZERS "Enable Address+UBSan on project targets" OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake")

# Apply sanitizer flags only to targets created after this point (not FetchContent deps).
if(GRAPPLE_BEAM_ENABLE_SANITIZERS)
  cppboot_enable_sanitizers()
endif()

# Generate the version API from cmake/version.{hpp,cpp}.in (values from VERSION).
set(GRAPPLE_BEAM_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GRAPPLE_BEAM_GENERATED_INCLUDE_DIR "${GRAPPLE_BEAM_GENERATED_DIR}/include")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
  "${GRAPPLE_BEAM_GENERATED_INCLUDE_DIR}/grapple/version.hpp"
  @ONLY
)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cpp.in"
  "${GRAPPLE_BEAM_GENERATED_DIR}/version.cpp"
  @ONLY
)

# The Android Prefab AAR and iOS XCFramework and web/Emscripten package ships the core as a static
# archive, so force the core static there (even with --shared). Android
# additionally needs PIC: the archive folds into libgrapple.so (src/android/).
set(GRAPPLE_BEAM_CORE_LIB_TYPE STATIC)
if(ANDROID OR IOS OR EMSCRIPTEN)
  set(GRAPPLE_BEAM_CORE_LIB_TYPE STATIC)
endif()
if(ANDROID)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
add_library(${PROJECT_NAME}_lib ${GRAPPLE_BEAM_CORE_LIB_TYPE})
# Namespaced aliases for consumers (add_subdirectory / FetchContent / find_package).
add_library(${PROJECT_NAME}::lib ALIAS ${PROJECT_NAME}_lib)
add_library(${PROJECT_NAME}::grapple ALIAS ${PROJECT_NAME}_lib)

set_target_properties(${PROJECT_NAME}_lib PROPERTIES
  # Not `grapple`: that name belongs to the shared library we ship, whose
  # Windows import library would otherwise be generated by two rules at once.
  # This target is internal — the app and the tests link it, nothing installs
  # it.
  OUTPUT_NAME grapple_core
  EXPORT_NAME lib
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${GRAPPLE_BEAM_GENERATED_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

cppboot_set_project_warnings(${PROJECT_NAME}_lib)

# Preferred third-party deps are linked on the *app* (and optionally tests), not
# the library. Static libraries propagate private deps into install(EXPORT);
# keeping the library free of FetchContent targets makes find_package clean.
# Add target_link_libraries(... PUBLIC/PRIVATE ...) in component CMakeLists when
# library code needs these deps.

add_subdirectory(src)

if(GRAPPLE_BUILD_IMAGE)
  add_subdirectory(image)
endif()

if(GRAPPLE_BUILD_TTF)
  add_subdirectory(ttf)
endif()

if(GRAPPLE_BUILD_MIXER)
  add_subdirectory(mixer)
endif()

if(GRAPPLE_BUILD_NET)
  add_subdirectory(net)
endif()

if(GRAPPLE_BUILD_GFX)
  add_subdirectory(gfx)
endif()

if(GRAPPLE_BUILD_EXTRAS)
  add_subdirectory(extras)
endif()

if(GRAPPLE_BUILD_FORMATS)
  add_subdirectory(formats)
endif()

if(GRAPPLE_BUILD_VFS)
  if(NOT GRAPPLE_BUILD_EXTRAS)
    message(FATAL_ERROR "GRAPPLE_BUILD_VFS requires GRAPPLE_BUILD_EXTRAS (encrypted mounts use Grapple crypto)")
  endif()
  add_subdirectory(vfs)
endif()

if(GRAPPLE_BUILD_PHYSICS)
  if(NOT GRAPPLE_BUILD_GFX)
    message(FATAL_ERROR "GRAPPLE_BUILD_PHYSICS requires GRAPPLE_BUILD_GFX (debug draw uses the GPU primitives)")
  endif()
  add_subdirectory(physics)
endif()

if(GRAPPLE_BUILD_GUI)
  if(NOT GRAPPLE_BUILD_EXTRAS)
    message(FATAL_ERROR "GRAPPLE_BUILD_GUI requires GRAPPLE_BUILD_EXTRAS (file dialogs)")
  endif()
  add_subdirectory(gui)
endif()

if(GRAPPLE_BUILD_LUA OR GRAPPLE_BUILD_RUBY)
  if(NOT GRAPPLE_BUILD_VFS)
    message(FATAL_ERROR "script interpreters require GRAPPLE_BUILD_VFS (require() loads modules through the VFS)")
  endif()
  if(NOT GRAPPLE_BUILD_REGEX)
    message(FATAL_ERROR "script interpreters require GRAPPLE_BUILD_REGEX (Ruby's Regexp class is built on it)")
  endif()
endif()
if(GRAPPLE_BUILD_HTTP)
  add_subdirectory(http)
endif()
if(GRAPPLE_BUILD_TILED)
  if(NOT GRAPPLE_BUILD_VFS)
    message(FATAL_ERROR "GRAPPLE_BUILD_TILED requires GRAPPLE_BUILD_VFS")
  endif()
  if(NOT GRAPPLE_BUILD_FORMATS)
    message(FATAL_ERROR "GRAPPLE_BUILD_TILED requires GRAPPLE_BUILD_FORMATS")
  endif()
  add_subdirectory(tiled)
endif()
if(GRAPPLE_BUILD_REGEX)
  add_subdirectory(regex)
endif()
if(GRAPPLE_BUILD_LIGHT)
  add_subdirectory(light)
endif()
if(GRAPPLE_BUILD_ENGINE)
  if(NOT GRAPPLE_BUILD_FORMATS)
    message(FATAL_ERROR "GRAPPLE_BUILD_ENGINE requires GRAPPLE_BUILD_FORMATS (graphics settings parse media/config.toml)")
  endif()
  if(NOT GRAPPLE_BUILD_VFS)
    message(FATAL_ERROR "GRAPPLE_BUILD_ENGINE requires GRAPPLE_BUILD_VFS (the engine mounts the game's media archive)")
  endif()
  if(NOT GRAPPLE_BUILD_PHYSICS)
    message(FATAL_ERROR "GRAPPLE_BUILD_ENGINE requires GRAPPLE_BUILD_PHYSICS (actors carry Box2D bodies)")
  endif()
  if(NOT GRAPPLE_BUILD_IMAGE)
    message(FATAL_ERROR "GRAPPLE_BUILD_ENGINE requires GRAPPLE_BUILD_IMAGE (the asset loader decodes textures)")
  endif()
  if(NOT GRAPPLE_BUILD_LIGHT)
    message(FATAL_ERROR "GRAPPLE_BUILD_ENGINE requires GRAPPLE_BUILD_LIGHT (scene lighting presets)")
  endif()
  add_subdirectory(engine)
endif()
if(GRAPPLE_BUILD_LUA)
  add_subdirectory(lua)
endif()
if(GRAPPLE_BUILD_RUBY)
  add_subdirectory(mruby)
endif()
if(GRAPPLE_BUILD_CPP)
  foreach(dep IN ITEMS VFS EXTRAS MIXER PHYSICS IMAGE GUI LUA RUBY TILED TTF FORMATS GFX)
    if(NOT GRAPPLE_BUILD_${dep})
      message(FATAL_ERROR "GRAPPLE_BUILD_CPP requires GRAPPLE_BUILD_${dep} (or set GRAPPLE_BUILD_CPP=OFF)")
    endif()
  endforeach()
  add_subdirectory(cpp)
endif()
if(GRAPPLE_BUILD_LUA AND GRAPPLE_BUILD_RUBY)
  foreach(dep IN ITEMS VFS IMAGE TILED MIXER PHYSICS GFX EXTRAS TTF FORMATS GUI)
    if(NOT GRAPPLE_BUILD_${dep})
      message(FATAL_ERROR "the bindings require GRAPPLE_BUILD_${dep} (or disable LUA/RUBY)")
    endif()
  endforeach()
  add_subdirectory(bindings)
  # The script runner. On by default when this is the project being built —
  # a static library and a way to run a game are both part of "the default
  # build" — and off when we are a subdirectory of someone else's game,
  # which does not want our executable in its bin/.
  #
  # Off on iOS and Android regardless: there is no command line to run it
  # from, and CMake treats every executable there as an app bundle, so even
  # installing it wants a BUNDLE DESTINATION for a binary nobody can launch.
  set(_runner_default ${GRAPPLE_BEAM_IS_TOP_LEVEL})
  if(IOS OR ANDROID)
    set(_runner_default OFF)
  endif()
  option(GRAPPLE_BUILD_RUNNER "Build the Lua/Ruby script runner" ${_runner_default})
  if(GRAPPLE_BUILD_RUNNER)
    add_subdirectory(tools/repl)  # builds `grapple`
  endif()
endif()

# The libraries we ship — two static, two shared — assembled from every
# component. Defined before the tests, which check the artifacts and so need
# the targets to exist.
include(SDK)

# Playable demos, one per language surface. On by default at top level and
# never installed: they are what the engine is for, not part of what it ships.
set(GRAPPLE_BEAM_DEFAULT_BUILD_DEMOS ${GRAPPLE_BEAM_IS_TOP_LEVEL})
if(ANDROID OR IOS)
  set(GRAPPLE_BEAM_DEFAULT_BUILD_DEMOS OFF)
endif()
option(GRAPPLE_BEAM_BUILD_DEMOS "Build the playable demos"
       ${GRAPPLE_BEAM_DEFAULT_BUILD_DEMOS})

# Before the demos, not after: they register smoke tests of their own, and
# add_test does nothing until testing is enabled.
if(GRAPPLE_BEAM_BUILD_TESTS)
  enable_testing()
endif()

if(GRAPPLE_BEAM_BUILD_DEMOS AND GRAPPLE_BUILD_ENGINE AND GRAPPLE_BUILD_TTF)
  add_subdirectory(demos)
endif()

if(GRAPPLE_BEAM_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(GRAPPLE_BEAM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

# Keep a source-root compile_commands.json for clangd / VS Code IntelliSense.
# Only when this project is top-level (do not rewrite a parent project's link).
if(GRAPPLE_BEAM_IS_TOP_LEVEL AND CMAKE_EXPORT_COMPILE_COMMANDS)
  if(WIN32)
    add_custom_target(cppboot_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
      COMMENT "Copying compile_commands.json to source root for clangd"
      VERBATIM
    )
  else()
    add_custom_target(cppboot_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E rm -f
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
      COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
      COMMENT "Linking compile_commands.json to source root for clangd"
      VERBATIM
    )
  endif()
endif()


include(GNUInstallDirs)
install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
# Generated version API header (from VERSION + cmake/version.hpp.in).
install(DIRECTORY ${GRAPPLE_BEAM_GENERATED_INCLUDE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
# The scaffold library is deliberately *not* installed. It holds version.cpp
# and nothing else, its objects are inside both SDKs already, and shipping it
# beside them offers a consumer a 950-byte archive that links and does
# nothing — which is the exact failure this project spent a week removing.
# It stays as an internal target for the app and the tests.

# CMake package config so consumers can find_package(grapple-beam).
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PackageConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

