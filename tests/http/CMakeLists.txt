add_executable(grapple_http_test
  http_test.cpp
)
target_link_libraries(grapple_http_test
  PRIVATE
    Grapple::Http
    SDL3::SDL3
    GTest::gtest_main
)
cppboot_set_project_warnings(grapple_http_test)
gtest_discover_tests(grapple_http_test DISCOVERY_TIMEOUT 60 PROPERTIES TIMEOUT 120)

# Link audit: mog transports are OS-native or dlopen'd; nothing shared links.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME http_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_http_test>
  )
endif()
