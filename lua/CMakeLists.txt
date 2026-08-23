# Grapple::Lua — static-only build of vendored Lua 5.4.8 (MIT; provenance
# in deps/lua.md) plus original glue: state creation and a package searcher
# that resolves require() through the PhysFS VFS (scripts can live inside a
# mounted — optionally encrypted — zip archive).

enable_language(C)

file(GLOB LUA_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(Grapple_Lua STATIC
  ${LUA_SOURCES}
  glue/grapple_lua.c
)
add_library(Grapple::Lua ALIAS Grapple_Lua)

set_target_properties(Grapple_Lua PROPERTIES
  OUTPUT_NAME Grapple_Lua
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Lua SYSTEM
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# iOS has no system(3) — the symbol exists in the SDK and is marked
# unavailable, so os.execute fails to compile rather than at runtime. Lua
# upstream anticipates this: LUA_USE_IOS turns l_system into a stub that
# reports "no shell available", which is the truthful answer on a platform
# where a game cannot spawn one anyway.
if(IOS)
  target_compile_definitions(Grapple_Lua PRIVATE LUA_USE_IOS)
endif()

target_link_libraries(Grapple_Lua PUBLIC SDL3::SDL3 Grapple::VFS)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_Lua PRIVATE m)
endif()
