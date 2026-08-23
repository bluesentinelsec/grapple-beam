add_executable(grapple_mixer_test
  mixer_test.cpp
  chiptune_test.cpp
)
target_link_libraries(grapple_mixer_test
  PRIVATE
    Grapple::Mixer
    GTest::gtest_main
)
target_compile_definitions(grapple_mixer_test PRIVATE
  MIXER_TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
  MIXER_GM_PATCHES_DIR="${CMAKE_SOURCE_DIR}/mixer/gm-patches"
)
cppboot_set_project_warnings(grapple_mixer_test)
gtest_discover_tests(grapple_mixer_test DISCOVERY_TIMEOUT 60)

# Link audit: static Grapple::Mixer + static SDL3 must depend only on
# OS-built-in shared libraries — no codec libraries, ever.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME mixer_link_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/link_audit.py
            $<TARGET_FILE:grapple_mixer_test>
  )
endif()
