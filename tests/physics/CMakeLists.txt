add_executable(grapple_physics_test
  physics_test.cpp
)
target_link_libraries(grapple_physics_test
  PRIVATE
    Grapple::Physics
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_physics_test)
gtest_discover_tests(grapple_physics_test DISCOVERY_TIMEOUT 60)

# Link audit: static Box2D + glue must depend only on OS-built-in libs.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME physics_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_physics_test>
  )
endif()
