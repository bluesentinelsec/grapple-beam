add_executable(grapple_gui_test
  gui_test.cpp
)
target_link_libraries(grapple_gui_test
  PRIVATE
    Grapple::GUI
    GTest::gtest_main
)
target_compile_definitions(grapple_gui_test PRIVATE
  # Vera carries the typographic punctuation the glyph-range test needs.
  GUI_TEST_FONT="${CMAKE_SOURCE_DIR}/tests/ttf/assets/Vera.ttf"
)
cppboot_set_project_warnings(grapple_gui_test)
gtest_discover_tests(grapple_gui_test DISCOVERY_TIMEOUT 60)

# Link audit: static Nuklear + backend must depend only on OS-built-in libs.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME gui_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_gui_test>
  )
endif()
