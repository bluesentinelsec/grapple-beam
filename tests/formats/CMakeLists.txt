add_executable(grapple_formats_test
  formats_test.cpp
)
target_link_libraries(grapple_formats_test
  PRIVATE
    Grapple::Formats
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_formats_test)
gtest_discover_tests(grapple_formats_test DISCOVERY_TIMEOUT 60)

# Link audit: parsers must not pull in shared libs (no libyaml.so, ever).
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME formats_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_formats_test>
  )
endif()
