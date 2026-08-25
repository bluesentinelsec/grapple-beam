# Latin Translator, in all four languages.
#
# The C and C++ builds are executables; the Lua and Ruby versions are scripts
# the runner plays, so they need no build at all.

add_executable(latin_c latin_c.c)
target_link_libraries(latin_c PRIVATE Grapple::Engine Grapple::GUI Grapple::TTF SDL3::SDL3)
set_target_properties(latin_c PROPERTIES
  OUTPUT_NAME latin-c
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(GRAPPLE_BUILD_CPP)
  add_executable(latin_cpp latin_cpp.cpp)
  target_compile_features(latin_cpp PRIVATE cxx_std_20)
  target_link_libraries(latin_cpp PRIVATE Grapple::Cpp Grapple::Engine Grapple::GUI Grapple::TTF SDL3::SDL3)
  set_target_properties(latin_cpp PROPERTIES
    OUTPUT_NAME latin-cpp
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
endif()

