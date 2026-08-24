add_executable(grapple_cpp_test
  cpp_test.cpp
)
target_link_libraries(grapple_cpp_test
  PRIVATE
    Grapple::Cpp
    Grapple::GUI
    GTest::gtest_main
)
target_compile_definitions(grapple_cpp_test PRIVATE
  CPP_TEST_ASSETS_DIR="${CMAKE_SOURCE_DIR}/tests/vfs/assets"
)
cppboot_set_project_warnings(grapple_cpp_test)
gtest_discover_tests(grapple_cpp_test DISCOVERY_TIMEOUT 60)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME cpp_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_cpp_test>
  )
endif()
