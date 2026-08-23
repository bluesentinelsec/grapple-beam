# Grapple::Http — the mog HTTP/S client + server (the user's own library,
# github.com/bluesentinelsec/mog) consumed via CMake FetchContent at a
# pinned release (see cmake/Dependencies.cmake and deps/mog.md).
#
# mog's C API normally ships as a shared library for FFI; here its C-API
# sources are compiled straight into this STATIC library against the static
# C++ core (mog_lib), so consumers stay fully self-contained. Use
# <mog/mog_c.h> — client (mog_get/mog_post/mog_perform) and embedded server.

add_library(Grapple_Http STATIC
  ${mog_SOURCE_DIR}/src/c_api/mog_c.cpp
  ${mog_SOURCE_DIR}/src/c_api/mog_c_server.cpp
)
add_library(Grapple::Http ALIAS Grapple_Http)

set_target_properties(Grapple_Http PROPERTIES
  OUTPUT_NAME Grapple_Http
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Http SYSTEM
  PUBLIC
    ${mog_SOURCE_DIR}/include
)

target_link_libraries(Grapple_Http PUBLIC mog_lib)
