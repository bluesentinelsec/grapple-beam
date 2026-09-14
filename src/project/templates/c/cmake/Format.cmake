get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
find_program(FORMATTER clang-format REQUIRED)
file(GLOB_RECURSE files LIST_DIRECTORIES false
  "${ROOT}/src/*.c" "${ROOT}/src/*.h" "${ROOT}/include/*.h"
  "${ROOT}/tests/*.c" "${ROOT}/tests/*.h")
if(CHECK)
  set(args --dry-run --Werror)
else()
  set(args -i)
endif()
foreach(file IN LISTS files)
  execute_process(COMMAND "${FORMATTER}" ${args} "${file}" COMMAND_ERROR_IS_FATAL ANY)
endforeach()
