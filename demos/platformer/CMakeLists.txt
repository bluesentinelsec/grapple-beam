# The platformer demo, in all four languages: the same level and the same
# player, once per language surface. See platformer.lua for the tour.

add_executable(platformer_c platformer.c)
target_link_libraries(platformer_c PRIVATE Grapple::Platformer Grapple::Engine Grapple::TTF SDL3::SDL3)
set_target_properties(platformer_c PROPERTIES
  OUTPUT_NAME platformer-c
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(GRAPPLE_BUILD_CPP)
  add_executable(platformer_cpp platformer.cpp)
  target_compile_features(platformer_cpp PRIVATE cxx_std_20)
  target_link_libraries(platformer_cpp PRIVATE Grapple::Cpp Grapple::Platformer Grapple::TTF SDL3::SDL3)
  set_target_properties(platformer_cpp PROPERTIES
    OUTPUT_NAME platformer-cpp
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
endif()

# Smoke tests: played headless for a few seconds on autopilot. The player
# narrates its state changes, and a run that never jumped is a failure.
if(GRAPPLE_BEAM_BUILD_TESTS)
  add_test(NAME demo_platformer_c
    COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:platformer_c> -DSECONDS=8
            -DPATTERN=state=jump -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
  set_tests_properties(demo_platformer_c PROPERTIES RUN_SERIAL TRUE)

  if(TARGET platformer_cpp)
    add_test(NAME demo_platformer_cpp
      COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:platformer_cpp> -DSECONDS=8
              -DPATTERN=state=jump -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
    set_tests_properties(demo_platformer_cpp PROPERTIES RUN_SERIAL TRUE)
  endif()

  if(TARGET grapple-beam_app)
    add_test(NAME demo_platformer_slopes_lua
      COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:grapple-beam_app>
              -DSCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/slopes.lua -DSECONDS=8
              "-DPATTERN=angle=-[0-9]" -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
    set_tests_properties(demo_platformer_slopes_lua PROPERTIES RUN_SERIAL TRUE)
    foreach(language lua rb)
      add_test(NAME demo_platformer_${language}
        COMMAND ${CMAKE_COMMAND} -DDEMO=$<TARGET_FILE:grapple-beam_app>
                -DSCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/platformer.${language} -DSECONDS=8
                -DPATTERN=state=jump -P ${CMAKE_SOURCE_DIR}/scripts/run_demo_smoke.cmake)
      set_tests_properties(demo_platformer_${language} PROPERTIES RUN_SERIAL TRUE)
    endforeach()
  endif()
endif()
