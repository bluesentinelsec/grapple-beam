# Grapple::Bindings — Lua + Ruby bindings for the game surface (original
# code, zlib). A language-independent core (bindings_core.c) performs every
# operation; the per-language files marshal arguments and register opaque
# handles with GC finalizers.
enable_language(C)

# Generated flat bindings (bindings/generated/, from tools/bindgen). Each
# module's files build only when its CMake option is on; the register
# files skip disabled modules via GRAPPLE_GEN_DISABLE_<LIB>.
set(GRAPPLE_GEN_SOURCES
  generated/gen_lua_register.c
  generated/gen_ruby_register.c
  generated/gen_lua_sdl.c
  generated/gen_ruby_sdl.c
  generated/gen_lua_grapple.c
  generated/gen_ruby_grapple.c
)
set(GRAPPLE_GEN_DEFS "")
foreach(pair IN ITEMS
    "mix:MIXER" "img:IMAGE" "ttf:TTF" "net:NET" "physfs:VFS"
    "b2:PHYSICS" "nk:GUI" "cjson:FORMATS" "gfx:GFX" "toml:FORMATS"
    "yaml:FORMATS" "mog:HTTP")
  string(REPLACE ":" ";" pair "${pair}")
  list(GET pair 0 genkey)
  list(GET pair 1 opt)
  if(GRAPPLE_BUILD_${opt})
    list(APPEND GRAPPLE_GEN_SOURCES
      generated/gen_lua_${genkey}.c generated/gen_ruby_${genkey}.c)
  else()
    string(TOUPPER "${genkey}" genkey_upper)
    list(APPEND GRAPPLE_GEN_DEFS "GRAPPLE_GEN_DISABLE_${genkey_upper}")
  endif()
endforeach()

add_library(Grapple_Bindings STATIC
  src/bindings_core.c
  src/grapple_bindings_lua.c
  src/grapple_engine_hooks_lua.c
  src/grapple_engine_hooks_ruby.c
  src/grapple_bindings_ruby.c
  src/gen_support_lua.c
  src/gen_support_ruby.c
  src/grapple_regexp_ruby.c
  src/grapple_regex_lua.c
  ${GRAPPLE_GEN_SOURCES}
)

if(GRAPPLE_GEN_DEFS)
  target_compile_definitions(Grapple_Bindings PRIVATE ${GRAPPLE_GEN_DEFS})
endif()
add_library(Grapple::Bindings ALIAS Grapple_Bindings)

set_target_properties(Grapple_Bindings PROPERTIES
  OUTPUT_NAME Grapple_Bindings
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Bindings
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(Grapple_Bindings PUBLIC
  Grapple::Lua Grapple::Ruby Grapple::VFS Grapple::Image Grapple::Tiled
  Grapple::Mixer Grapple::Physics Grapple::Gfx Grapple::Extras
  Grapple::TTF Grapple::Formats Grapple::GUI
  SDL3::SDL3
)
if(GRAPPLE_BUILD_REGEX)
  target_link_libraries(Grapple_Bindings PUBLIC Grapple::Regex)
endif()
if(GRAPPLE_BUILD_LIGHT)
  target_link_libraries(Grapple_Bindings PUBLIC Grapple::Light)
endif()
if(GRAPPLE_BUILD_ENGINE)
  target_link_libraries(Grapple_Bindings PUBLIC Grapple::Engine)
endif()
if(GRAPPLE_BUILD_NET)
  target_link_libraries(Grapple_Bindings PUBLIC Grapple::Net)
endif()
if(GRAPPLE_BUILD_HTTP)
  target_link_libraries(Grapple_Bindings PUBLIC Grapple::Http)
endif()
