add_executable(grapple_platformer_test
  platformer_test.cpp
)
target_link_libraries(grapple_platformer_test
  PRIVATE
    Grapple::Platformer
    GTest::gtest_main
)
if(GRAPPLE_BUILD_CPP)
  target_link_libraries(grapple_platformer_test PRIVATE Grapple::Cpp)
  target_compile_definitions(grapple_platformer_test PRIVATE PLATFORMER_TEST_CPP=1)
endif()
target_compile_definitions(grapple_platformer_test PRIVATE
  PLATFORMER_TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
grapple_set_project_warnings(grapple_platformer_test)
gtest_discover_tests(grapple_platformer_test DISCOVERY_TIMEOUT 60)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME platformer_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_platformer_test>
  )
endif()
