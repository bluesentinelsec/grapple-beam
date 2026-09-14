# The CLI invokes this one install component, never upstream SDK install rules.
if(APPLE)
  set(resource_dest "@GAME_ID@.app/Contents/Resources")
else()
  set(resource_dest ".")
endif()
install(TARGETS @GAME_ID@ RUNTIME DESTINATION . COMPONENT Game BUNDLE DESTINATION . COMPONENT Game)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/media" DESTINATION "${resource_dest}" COMPONENT Game)
install(FILES "${PROJECT_SOURCE_DIR}/config.toml" "${PROJECT_SOURCE_DIR}/LICENSE" "${PROJECT_SOURCE_DIR}/VERSION"
  DESTINATION "${resource_dest}" COMPONENT Game)
set(notices "${CMAKE_BINARY_DIR}/licenses")
file(MAKE_DIRECTORY "${notices}")
file(COPY "${PROJECT_SOURCE_DIR}/deps/" DESTINATION "${notices}/provenance")
foreach(module image gfx vfs physics formats light extras)
  file(GLOB_RECURSE license_files LIST_DIRECTORIES false
    "${grapple_engine_SOURCE_DIR}/${module}/LICENSE*"
    "${grapple_engine_SOURCE_DIR}/${module}/COPYING*"
    "${grapple_engine_SOURCE_DIR}/${module}/NOTICE*")
  foreach(license IN LISTS license_files)
    file(RELATIVE_PATH name "${grapple_engine_SOURCE_DIR}" "${license}")
    get_filename_component(dir "${notices}/${name}" DIRECTORY)
    file(MAKE_DIRECTORY "${dir}")
    file(COPY "${license}" DESTINATION "${dir}")
  endforeach()
endforeach()
# These single-header dependencies carry their complete license in the source.
file(GLOB_RECURSE inline_notices LIST_DIRECTORIES false
  "${grapple_engine_SOURCE_DIR}/extras/*sdefl*.h" "${grapple_engine_SOURCE_DIR}/extras/*sinfl*.h"
  "${grapple_engine_SOURCE_DIR}/image/*stb*.h" "${grapple_engine_SOURCE_DIR}/image/*nanosvg*.h")
foreach(license IN LISTS inline_notices)
  file(RELATIVE_PATH name "${grapple_engine_SOURCE_DIR}" "${license}")
  get_filename_component(dir "${notices}/inline-notices/${name}" DIRECTORY)
  file(MAKE_DIRECTORY "${dir}")
  file(COPY "${license}" DESTINATION "${dir}")
endforeach()
FetchContent_GetProperties(SDL3 SOURCE_DIR sdl_source)
if(NOT EXISTS "${sdl_source}/LICENSE.txt")
  message(FATAL_ERROR "Cannot collect SDL3 license from the selected source dependency")
endif()
file(COPY "${sdl_source}/LICENSE.txt" DESTINATION "${notices}/SDL3")
file(COPY "${grapple_engine_SOURCE_DIR}/LICENSE" DESTINATION "${notices}/grapple-beam")
foreach(info SDL3 SDL3_image SDL3_gfx physfs box2d formats sdefl)
  if(EXISTS "${grapple_engine_SOURCE_DIR}/deps/${info}.md")
    file(COPY "${grapple_engine_SOURCE_DIR}/deps/${info}.md" DESTINATION "${notices}/provenance")
  endif()
endforeach()
install(DIRECTORY "${notices}/" DESTINATION "${resource_dest}/licenses" COMPONENT Game)
# Every engine module selected by the starter is static. Only OS/runtime imports
# are permitted; an added shared dependency requires an explicit packaging rule.
set(GAME_EXECUTABLE "$<TARGET_FILE:@GAME_ID@>")
configure_file("${PROJECT_SOURCE_DIR}/cmake/RuntimeAudit.cmake.in"
  "${CMAKE_BINARY_DIR}/runtime-audit.in" @ONLY)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/runtime-audit-$<CONFIG>.cmake"
  INPUT "${CMAKE_BINARY_DIR}/runtime-audit.in")
install(SCRIPT "${CMAKE_BINARY_DIR}/runtime-audit-\${CMAKE_INSTALL_CONFIG_NAME}.cmake" COMPONENT Game)
