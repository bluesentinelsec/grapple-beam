# Grapple::Extras — original game-infrastructure modules (zlib), plus the
# vendored sdefl/sinfl DEFLATE single-headers (dual MIT/public-domain;
# provenance in deps/sdefl.md). Everything is plain C over SDL3 with zero
# external dependencies — fully static on every platform, web included.
#
#   <grapple/crypto.h>    password-based asset encryption (ChaCha20 +
#                           PBKDF2-HMAC-SHA256, encrypt-then-MAC)
#   <grapple/compress.h>  DEFLATE compress/decompress (raylib API shape)
#   <grapple/base64.h>    Base64 encode/decode (raylib API shape)
#   <grapple/signals.h>   Godot-style signal/observer system

enable_language(C)

add_library(Grapple_Extras STATIC
  src/grapple_crypto.c
  src/grapple_compress.c
  src/grapple_base64.c
  src/grapple_signals.c
  src/grapple_dialog.c
  src/grapple_textfile.c
)
add_library(Grapple::Extras ALIAS Grapple_Extras)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Extras PRIVATE c_std_99)
endif()

set_target_properties(Grapple_Extras PROPERTIES
  OUTPUT_NAME Grapple_Extras
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(Grapple_Extras
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Grapple_Extras PUBLIC SDL3::SDL3)
