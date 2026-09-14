get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
# Only these reserved output locations are owned by this project.
file(REMOVE_RECURSE "${ROOT}/build" "${ROOT}/dist")
file(REMOVE "${ROOT}/compile_commands.json" "${ROOT}/tags" "${ROOT}/TAGS")
