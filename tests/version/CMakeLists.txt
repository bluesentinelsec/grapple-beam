add_executable(grapple_version_test
  version_test.cpp
)
target_link_libraries(grapple_version_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_version_test)
gtest_discover_tests(grapple_version_test DISCOVERY_TIMEOUT 60)
