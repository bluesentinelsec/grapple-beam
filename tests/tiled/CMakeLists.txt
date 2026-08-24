add_executable(grapple_tiled_test
  tiled_test.cpp
)
target_link_libraries(grapple_tiled_test
  PRIVATE
    Grapple::Tiled
    Grapple::Bindings
    Grapple::Cpp
    GTest::gtest_main
)
target_compile_definitions(grapple_tiled_test PRIVATE
  TILED_TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
cppboot_set_project_warnings(grapple_tiled_test)
gtest_discover_tests(grapple_tiled_test DISCOVERY_TIMEOUT 60)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME tiled_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_tiled_test>
  )
endif()
