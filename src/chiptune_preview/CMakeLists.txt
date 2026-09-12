# Preview behavior belongs in the library; main.cpp only handles CLI startup.
enable_language(C)
target_sources(${PROJECT_NAME}_lib PRIVATE preview.c)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE Grapple::Mixer)
