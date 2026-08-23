cmake_minimum_required(VERSION 3.22.1)
project(grapple_android_test LANGUAGES CXX)

find_package(grapple REQUIRED CONFIG)

add_library(grapple_android_test SHARED android_test.cpp)
target_compile_features(grapple_android_test PRIVATE cxx_std_20)
target_compile_options(grapple_android_test PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Werror
)
target_link_libraries(grapple_android_test PRIVATE grapple::grapple log)
