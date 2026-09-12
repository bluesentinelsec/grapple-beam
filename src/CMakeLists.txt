# Library implementation components.
# Each logical subdirectory owns a CMakeLists.txt that lists sources explicitly.
add_subdirectory(version)
# The desktop engine runner is onboarded after its script/binding dependencies.

# Android Prefab shared library (built by the android/ Gradle project).
if(ANDROID)
  add_subdirectory(android)
endif()

# Web canvas demo (Emscripten game loop; see src/web/).
if(EMSCRIPTEN)
  add_subdirectory(web)
endif()
