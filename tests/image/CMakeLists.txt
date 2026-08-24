add_executable(grapple_image_test
  image_test.cpp
)
target_link_libraries(grapple_image_test
  PRIVATE
    Grapple::Image
    GTest::gtest_main
)
target_compile_definitions(grapple_image_test PRIVATE
  IMAGE_TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
cppboot_set_project_warnings(grapple_image_test)
gtest_discover_tests(grapple_image_test DISCOVERY_TIMEOUT 60)

# Link audit: the test binary (static Grapple::Image + static SDL3) must
# depend only on OS-built-in shared libraries — the project's core promise.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME image_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_image_test>
  )
endif()
