# Pong, in all four languages.
#
# The C and C++ builds are executables; the Lua and Ruby versions are scripts
# the runner plays, so they need no build at all — which is the point of
# shipping a runner.

add_executable(pong_c pong.c)
target_link_libraries(pong_c PRIVATE Grapple::Engine Grapple::TTF SDL3::SDL3)
set_target_properties(pong_c PROPERTIES
  OUTPUT_NAME pong-c
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(GRAPPLE_BUILD_CPP)
  add_executable(pong_cpp pong.cpp)
  target_compile_features(pong_cpp PRIVATE cxx_std_20)
  target_link_libraries(pong_cpp PRIVATE Grapple::Cpp Grapple::Engine Grapple::TTF SDL3::SDL3)
  set_target_properties(pong_cpp PROPERTIES
    OUTPUT_NAME pong-cpp
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
endif()

# Smoke tests: each version is played headless for a few seconds. See
# scripts/run_demo_smoke.cmake for why "still running" is the passing case.
if(GRAPPLE_BEAM_BUILD_TESTS)
  add_test(NAME demo_pong_c
    COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:pong_c> -DSECONDS=10
            -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)

  if(TARGET pong_cpp)
    add_test(NAME demo_pong_cpp
      COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:pong_cpp> -DSECONDS=10
              -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
  endif()

  # The script versions need no build — the runner plays them, which is the
  # arrangement worth testing as much as the games themselves.
  if(TARGET grapple)
    foreach(language lua rb)
      add_test(NAME demo_pong_${language}
        COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:grapple>
                -DSCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/pong.${language} -DSECONDS=10
                -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
    endforeach()
  endif()
endif()

# Each of these runs a real engine — window, renderer, font atlas — for ten
# seconds. Four at once, alongside everything else ctest is running, is what
# a three-core CI runner does not have the memory for: the macOS runner
# stopped answering the server three times before this was bounded.
set_tests_properties(demo_pong_c PROPERTIES RUN_SERIAL TRUE)
if(TARGET pong_cpp)
  set_tests_properties(demo_pong_cpp PROPERTIES RUN_SERIAL TRUE)
endif()
if(TARGET grapple)
  set_tests_properties(demo_pong_lua demo_pong_rb PROPERTIES RUN_SERIAL TRUE)
endif()
