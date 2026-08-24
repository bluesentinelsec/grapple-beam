# Android Prefab library. Gradle publishes this target as the `grapple`
# module in the AAR. Whole-archive linking keeps every public API symbol while
# folding the whole static SDK — every component, SDL3, and the vendored
# dependencies — into one shared object.
add_library(grapple SHARED android_library.cpp)

target_compile_features(grapple PUBLIC cxx_std_20)
target_include_directories(grapple
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${GRAPPLE_BEAM_GENERATED_INCLUDE_DIR}>
)

# Whole-archive the SDK, not the version library. Linking only the latter is
# how the AAR came to hold a version string and nothing else while the ABI,
# slice and version checks all passed — they were checking the shape of
# something empty.
add_dependencies(grapple Grapple_SDK)
target_link_options(grapple
  PRIVATE
    "LINKER:--whole-archive,$<TARGET_FILE:Grapple_SDK>,--no-whole-archive"
)

# The system libraries SDL3 needs on Android — liblog for SDL_Log, GLESv2 and
# EGL for its renderer, and the rest. They arrive through SDL3's own link
# interface rather than a list here, so they stay right as SDL changes: the
# archive was linked with nothing but -latomic -lm before, which was enough
# only because it contained no SDL3 to speak of.
target_link_libraries(grapple PRIVATE SDL3::SDL3)

set_target_properties(grapple PROPERTIES
  OUTPUT_NAME grapple
)

cppboot_set_project_warnings(grapple)
