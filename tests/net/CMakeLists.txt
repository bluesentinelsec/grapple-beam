add_executable(grapple_net_test
  net_test.cpp
)
target_link_libraries(grapple_net_test
  PRIVATE
    Grapple::Net
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_net_test)
gtest_discover_tests(grapple_net_test DISCOVERY_TIMEOUT 60 PROPERTIES TIMEOUT 120)

# Link audit: static Grapple::Net + static SDL3 must depend only on
# OS-built-in shared libraries.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME net_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_net_test>
  )
endif()
