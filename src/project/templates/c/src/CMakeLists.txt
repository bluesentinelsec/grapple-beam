file(GLOB_RECURSE game_sources CONFIGURE_DEPENDS LIST_DIRECTORIES false
  "${CMAKE_CURRENT_SOURCE_DIR}/@GAME_ID@/*.c")
file(GLOB_RECURSE game_headers CONFIGURE_DEPENDS LIST_DIRECTORIES false
  "${PROJECT_SOURCE_DIR}/include/@GAME_ID@/*.h")
add_library(game STATIC ${game_sources} ${game_headers})
target_include_directories(game PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(game PUBLIC Grapple::Engine Grapple::VFS Grapple::Formats)
target_compile_definitions(game PRIVATE GAME_RELEASE=$<CONFIG:Release>)
game_warnings(game)
add_executable(@GAME_ID@ main.c)
target_link_libraries(@GAME_ID@ PRIVATE game)
game_warnings(@GAME_ID@)
set_target_properties(@GAME_ID@ PROPERTIES MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_GUI_IDENTIFIER "games.@GAME_ID@"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
if(APPLE)
  set(game_resources "$<TARGET_BUNDLE_DIR:@GAME_ID@>/Contents/Resources")
else()
  set(game_resources "$<TARGET_FILE_DIR:@GAME_ID@>")
endif()
add_custom_target(game_resources ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${game_resources}"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/media" "${game_resources}/media"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/config.toml" "${game_resources}/config.toml"
  DEPENDS @GAME_ID@ VERBATIM)
