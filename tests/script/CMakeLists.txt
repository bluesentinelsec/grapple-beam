add_executable(grapple_script_test
  script_test.cpp
)
target_link_libraries(grapple_script_test
  PRIVATE
    Grapple::Bindings
    GTest::gtest_main
)
target_compile_definitions(grapple_script_test PRIVATE
  SCRIPT_TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
grapple_set_project_warnings(grapple_script_test)
gtest_discover_tests(grapple_script_test DISCOVERY_TIMEOUT 60)

# The runner itself, driven end to end.
if(TARGET grapple-beam_app)
  add_test(NAME runner_lua_eval COMMAND grapple-beam_app -l lua -e "print(1 + 2)")
  set_tests_properties(runner_lua_eval PROPERTIES PASS_REGULAR_EXPRESSION "^3")
  add_test(NAME runner_ruby_eval COMMAND grapple-beam_app -l ruby -e "puts 6 * 7")
  set_tests_properties(runner_ruby_eval PROPERTIES PASS_REGULAR_EXPRESSION "^42")
  add_test(NAME runner_bad_language COMMAND grapple-beam_app -l cobol)
  set_tests_properties(runner_bad_language PROPERTIES WILL_FAIL TRUE)
endif()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME script_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_script_test>
  )
endif()

if(TARGET grapple-beam_app)
  add_test(NAME runner_version COMMAND grapple-beam_app --version)
  set_tests_properties(runner_version PROPERTIES PASS_REGULAR_EXPRESSION "grapple-beam [0-9]")
  add_test(NAME runner_help COMMAND grapple-beam_app --help)
  set_tests_properties(runner_help PROPERTIES
    PASS_REGULAR_EXPRESSION "usage: grapple-beam"
    FAIL_REGULAR_EXPRESSION "chip-midi|chip-wav")
  add_test(NAME runner_missing_eval COMMAND grapple-beam_app -l lua -e)
  set_tests_properties(runner_missing_eval PROPERTIES WILL_FAIL TRUE)
  foreach(language lua rb)
    foreach(option --fullscreen --vsync --with-safe-mode --with-default-settings)
      add_test(NAME runner_${language}_${option}
        COMMAND grapple-beam_app ${option}
                ${CMAKE_CURRENT_SOURCE_DIR}/runner/arguments.${language} -- --level 3)
      set_tests_properties(runner_${language}_${option} PROPERTIES
        PASS_REGULAR_EXPRESSION "runner arguments preserved")
    endforeach()
  endforeach()
  configure_file(runner/arguments.rb "runner's arguments.rb" COPYONLY)
  add_test(NAME runner_ruby_quoted_path
    COMMAND grapple-beam_app "${CMAKE_CURRENT_BINARY_DIR}/runner's arguments.rb" --level 3)
  set_tests_properties(runner_ruby_quoted_path PROPERTIES PASS_REGULAR_EXPRESSION "runner arguments preserved")
endif()
