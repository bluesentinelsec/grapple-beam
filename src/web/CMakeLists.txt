# HTML5 canvas demo: a minimal Emscripten game loop around the project library.
# Built only for top-level web builds (option GRAPPLE_BEAM_BUILD_WEB_DEMO).
if(NOT GRAPPLE_BEAM_BUILD_WEB_DEMO)
  return()
endif()

add_executable(grapple_web_demo main_web.cpp)
set_target_properties(grapple_web_demo PROPERTIES
  OUTPUT_NAME grapple-beam
  SUFFIX ".html"
)
target_link_libraries(grapple_web_demo PRIVATE ${PROJECT_NAME}_lib)
target_link_options(grapple_web_demo
  PRIVATE
    "SHELL:-sALLOW_MEMORY_GROWTH=1"
    "SHELL:-sUSE_WEBGL2=1"
    "SHELL:-sEXIT_RUNTIME=0"
    "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
)
set_property(TARGET grapple_web_demo APPEND PROPERTY
  LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
)
# Game assets: uncomment to bundle a directory into the virtual filesystem
# (readable in C++ as /assets/...):
#   target_link_options(grapple_web_demo PRIVATE
#     "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
# SDL2 (input/audio/textures): add to BOTH compile and link options:
#   target_compile_options(grapple_web_demo PRIVATE "SHELL:-sUSE_SDL=2")
#   target_link_options(grapple_web_demo PRIVATE "SHELL:-sUSE_SDL=2")
cppboot_set_project_warnings(grapple_web_demo)
