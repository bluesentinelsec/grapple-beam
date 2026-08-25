# Grapple::GUI — vendored Nuklear (single header, v4.13.3, dual
# MIT/public-domain; provenance in deps/nuklear.md) plus the original SDL3
# backend and the weighted grid layout helper.
#
# Nuklear is vendored as-is (no delete-down: it is one dependency-free
# header; its embedded single-file pieces — stb_truetype/stb_rect_pack —
# are part of that header). All platform specificity lives in our backend,
# so the GUI runs wherever SDL3 runs: desktop, web, and mobile.

enable_language(C)

add_library(Grapple_GUI STATIC
  # Original Grapple code; grapple_gui.c is the single TU that compiles
  # the Nuklear implementation.
  src/grapple_gui.c
  src/grapple_gui_grid.c
)
add_library(Grapple::GUI ALIAS Grapple_GUI)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_GUI PRIVATE c_std_99)
endif()

set_target_properties(Grapple_GUI PROPERTIES
  OUTPUT_NAME Grapple_GUI
  POSITION_INDEPENDENT_CODE ON
)

# SYSTEM: the vendored header's declarations trip strict consumer warnings.
target_include_directories(Grapple_GUI SYSTEM
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # grapple/event_sink.h: shared by modules that must not depend
    # on each other.
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Grapple_GuiOpenFileButton drives the file dialogs in Grapple::Extras.
target_link_libraries(Grapple_GUI PUBLIC SDL3::SDL3 Grapple::Extras)
if(UNIX AND NOT APPLE)
  target_link_libraries(Grapple_GUI PRIVATE m)
endif()
