add_executable(grapple_regex_test
  regex_test.cpp
)
target_link_libraries(grapple_regex_test
  PRIVATE
    Grapple::Regex
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_regex_test)
gtest_discover_tests(grapple_regex_test DISCOVERY_TIMEOUT 60)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME regex_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_regex_test>
  )
endif()
