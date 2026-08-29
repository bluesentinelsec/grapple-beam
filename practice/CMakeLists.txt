# Practice problems for exercising the GUI toolkit.
#
# These are scratch programs used to find rough edges in the API, not part of
# the shipped product. Nothing here installs, and nothing here registers a
# test: they are run by hand, or by each problem's own run_all.sh.
#
# Subdirectories are discovered rather than listed. Most of these are never
# committed — they exist to be written, run once and thrown away — so a fixed
# list would name directories that are not there and break the build for
# everyone else.
file(GLOB practice_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(entry IN LISTS practice_dirs)
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${entry}
     AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${entry}/CMakeLists.txt)
    add_subdirectory(${entry})
  endif()
endforeach()
