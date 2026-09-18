# Readable desktop example; never installed as an engine/SDK output.
add_executable(chiptune_demo play.c)
set_target_properties(chiptune_demo PROPERTIES OUTPUT_NAME chiptune-demo)
target_link_libraries(chiptune_demo PRIVATE Grapple::Engine Grapple::Mixer Grapple::TTF)
grapple_set_project_warnings(chiptune_demo)

if(GRAPPLE_BEAM_BUILD_TESTS)
  foreach(extension mid xml)
    add_test(NAME demo_chiptune_${extension}
      COMMAND chiptune_demo
        ${PROJECT_SOURCE_DIR}/tests/mixer/assets/c64-composition-named.${extension})
    set_tests_properties(demo_chiptune_${extension} PROPERTIES
      ENVIRONMENT "GRAPPLE_HEADLESS=1;SDL_AUDIODRIVER=dummy"
      PASS_REGULAR_EXPRESSION "Loaded [0-9]+ tracks, 10.000 seconds"
      TIMEOUT 35)
    add_test(NAME demo_chiptune_wav_${extension}
      COMMAND chiptune_demo
        ${PROJECT_SOURCE_DIR}/tests/mixer/assets/c64-composition-named.${extension}
        ${CMAKE_CURRENT_BINARY_DIR}/demo-${extension}.wav)
    set_tests_properties(demo_chiptune_wav_${extension} PROPERTIES TIMEOUT 35)
  endforeach()
endif()

if(GRAPPLE_BUILD_CPP)
  add_executable(chiptune_cpp play.cpp)
  target_compile_features(chiptune_cpp PRIVATE cxx_std_20)
  set_target_properties(chiptune_cpp PROPERTIES OUTPUT_NAME chiptune-cpp)
  target_link_libraries(chiptune_cpp PRIVATE Grapple::Cpp Grapple::Engine Grapple::Mixer Grapple::TTF)
  grapple_set_project_warnings(chiptune_cpp)
  if(GRAPPLE_BEAM_BUILD_TESTS)
    add_test(NAME demo_chiptune_cpp COMMAND chiptune_cpp
      ${PROJECT_SOURCE_DIR}/tests/mixer/assets/c64-composition.mxl)
    set_tests_properties(demo_chiptune_cpp PROPERTIES
      ENVIRONMENT "GRAPPLE_HEADLESS=1;SDL_AUDIODRIVER=dummy" TIMEOUT 40)
  endif()
endif()
if(GRAPPLE_BEAM_BUILD_TESTS AND TARGET grapple-beam_app)
  foreach(language lua rb)
    foreach(mode file code)
      set(score "${PROJECT_SOURCE_DIR}/tests/mixer/assets/c64-composition-named.xml")
      if(mode STREQUAL "code")
        set(score code)
      endif()
      add_test(NAME demo_chiptune_${language}_${mode} COMMAND grapple-beam_app
        run ${CMAKE_CURRENT_SOURCE_DIR}/play.${language} ${score})
      set_tests_properties(demo_chiptune_${language}_${mode} PROPERTIES
        ENVIRONMENT "GRAPPLE_HEADLESS=1;SDL_AUDIODRIVER=dummy" TIMEOUT 40)
    endforeach()
  endforeach()
endif()
