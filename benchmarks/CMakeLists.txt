# Microbenchmarks — one subdirectory per component.
add_subdirectory(version)
if(TARGET Grapple_Mixer)
  add_subdirectory(mixer)
endif()
