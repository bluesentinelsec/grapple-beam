cmake_minimum_required(VERSION 3.28)

project(grapple_ios_package_test LANGUAGES CXX OBJCXX)

if(NOT IOS)
  message(FATAL_ERROR "The grapple-beam iOS package test must be configured with CMAKE_SYSTEM_NAME=iOS")
endif()

set(GRAPPLE_BEAM_XCFRAMEWORK "" CACHE PATH "Path to the packaged grapple-beam.xcframework")
set(GRAPPLE_BEAM_EXPECTED_VERSION "" CACHE STRING "Version expected from the packaged library")

if(NOT IS_DIRECTORY "${GRAPPLE_BEAM_XCFRAMEWORK}")
  message(FATAL_ERROR "Set GRAPPLE_BEAM_XCFRAMEWORK to the packaged grapple-beam.xcframework directory")
endif()
if(GRAPPLE_BEAM_EXPECTED_VERSION STREQUAL "")
  message(FATAL_ERROR "Set GRAPPLE_BEAM_EXPECTED_VERSION to the root VERSION value")
endif()

add_executable(grapple_ios_test MACOSX_BUNDLE test_main.mm)
target_compile_features(grapple_ios_test PRIVATE cxx_std_20)
target_compile_options(grapple_ios_test PRIVATE -fobjc-arc)
target_compile_definitions(grapple_ios_test PRIVATE
  GRAPPLE_BEAM_EXPECTED_VERSION="${GRAPPLE_BEAM_EXPECTED_VERSION}"
)
# The XCFramework now carries SDL3's objects as well as ours, so this app
# links what SDL3 needs on iOS. Before, an empty library needed nothing.
target_link_libraries(grapple_ios_test PRIVATE
  "${GRAPPLE_BEAM_XCFRAMEWORK}"
  "-framework AVFoundation"
  "-framework AudioToolbox"
  "-framework CoreAudio"
  "-framework CoreBluetooth"
  "-framework CoreGraphics"
  "-framework CoreMedia"
  "-framework CoreMotion"
  "-framework CoreVideo"
  "-framework Foundation"
  "-framework GameController"
  "-framework Metal"
  "-framework OpenGLES"
  "-framework QuartzCore"
  "-framework UIKit"
  "-weak_framework CoreHaptics"
)

set_target_properties(grapple_ios_test PROPERTIES
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
  MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.sdl3staticextensions.test"
  MACOSX_BUNDLE_BUNDLE_NAME "grapple-beam iOS Tests"
  MACOSX_BUNDLE_BUNDLE_VERSION "1"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${GRAPPLE_BEAM_EXPECTED_VERSION}"
  XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.example.sdl3staticextensions.test"
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
  XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)
