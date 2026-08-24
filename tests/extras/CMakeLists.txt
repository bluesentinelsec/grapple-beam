add_executable(grapple_extras_test
  extras_test.cpp
)
target_link_libraries(grapple_extras_test
  PRIVATE
    Grapple::Extras
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_extras_test)
gtest_discover_tests(grapple_extras_test DISCOVERY_TIMEOUT 60)

# Link audit: static Grapple::Extras + static SDL3 must depend only on
# OS-built-in shared libraries — no libcrypto, no libz, ever.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME extras_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_extras_test>
  )
endif()
