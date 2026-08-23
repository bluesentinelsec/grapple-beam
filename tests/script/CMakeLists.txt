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
cppboot_set_project_warnings(grapple_script_test)
gtest_discover_tests(grapple_script_test DISCOVERY_TIMEOUT 60)

# The runner itself, driven end to end.
if(TARGET grapple)
  add_test(NAME runner_lua_eval COMMAND grapple -l lua -e "print(1 + 2)")
  set_tests_properties(runner_lua_eval PROPERTIES PASS_REGULAR_EXPRESSION "^3")
  add_test(NAME runner_ruby_eval COMMAND grapple -l ruby -e "puts 6 * 7")
  set_tests_properties(runner_ruby_eval PROPERTIES PASS_REGULAR_EXPRESSION "^42")
  add_test(NAME runner_bad_language COMMAND grapple -l cobol)
  set_tests_properties(runner_bad_language PROPERTIES WILL_FAIL TRUE)
endif()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME script_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_script_test>
  )
endif()
