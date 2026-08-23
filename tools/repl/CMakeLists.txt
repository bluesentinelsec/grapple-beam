# grapple — the runner: plays a Lua or Ruby game, and opens a shell over the SDLStatic
# interpreters (with VFS-aware require and the game bindings preloaded).
enable_language(C)
add_executable(grapple main.c)
target_link_libraries(grapple PRIVATE SDLStatic::Lua SDLStatic::Ruby SDLStatic::Bindings)
set_target_properties(grapple PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Shipped alongside the library: the SDK is what you build a game against,
# and this is what runs one written in Lua or Ruby without building anything
# at all. It is statically linked like everything else here, so it carries
# its own interpreters and needs nothing installed on the machine.
include(GNUInstallDirs)
install(TARGETS grapple RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
