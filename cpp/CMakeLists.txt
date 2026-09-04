# Grapple::Cpp — Google-style C++ bindings over the C surfaces: RAII
# owners for every handle, Status/Result<T> error flow, no exceptions
# (build-enforced via -fno-exceptions on this target), no dependencies
# beyond standard C++20. Disable with -DGRAPPLE_BUILD_CPP=OFF.
#
# Formatting: cpp/.clang-format (Google preset) applies to this directory
# only. Http has no wrapper by decision: mog is already C++.

add_library(Grapple_Cpp STATIC
  src/no_exceptions.cc
  src/ui.cc
)
add_library(Grapple::Cpp ALIAS Grapple_Cpp)

target_compile_features(Grapple_Cpp PUBLIC cxx_std_20)
set_target_properties(Grapple_Cpp PROPERTIES
  OUTPUT_NAME Grapple_Cpp
  POSITION_INDEPENDENT_CODE ON
)
if(NOT MSVC)
  target_compile_options(Grapple_Cpp PRIVATE -fno-exceptions)
endif()

target_include_directories(Grapple_Cpp
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(Grapple_Cpp PUBLIC
  Grapple::VFS Grapple::Extras Grapple::Mixer Grapple::Physics
  Grapple::Image Grapple::GUI Grapple::Lua Grapple::Ruby Grapple::Tiled
  Grapple::Engine
    Grapple::TTF Grapple::Formats Grapple::Gfx
  SDL3::SDL3
)

if(GRAPPLE_BUILD_REGEX)
  target_link_libraries(Grapple_Cpp PUBLIC Grapple::Regex)
endif()
if(GRAPPLE_BUILD_LIGHT)
  target_link_libraries(Grapple_Cpp PUBLIC Grapple::Light)
endif()
if(GRAPPLE_BUILD_NET)
  target_link_libraries(Grapple_Cpp INTERFACE Grapple::Net)
endif()
if(GRAPPLE_BUILD_HTTP)
  target_link_libraries(Grapple_Cpp INTERFACE Grapple::Http)
endif()
