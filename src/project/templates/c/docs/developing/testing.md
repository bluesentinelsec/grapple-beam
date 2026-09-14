# Testing and debugging

`make test` builds prerequisites and runs all CTest cases. `ctest --test-dir
build/debug -R game_movement --output-on-failure` selects the movement unit test.
The rendering integration case launches a real window, loads the supplied SVG,
checks the rendered pixels and movement, and exits after 12 frames. CTest's outer
30-second watchdog handles hangs. It skips player settings and saves. CI reports
its actual renderer; a virtual display/software renderer does not verify physical
GPU behavior. The starter has no audio, so no speaker coverage is claimed.

Add independent C cases under tests/ and register them with CTest timeouts. Assert
actual game state and clean up fixtures. Do not make tests wait for user input.

For Linux sanitizers: `cmake --preset sanitizer`, `cmake --build --preset sanitizer`,
then `ctest --preset sanitizer`. These flags instrument authored game/test code;
source dependencies retain their upstream build flags.

Debug binaries contain symbols. On macOS use
`lldb build/debug/bin/@GAME_ID@.app/Contents/MacOS/@GAME_ID@`, then `breakpoint set
--name main` and `run`. On Linux use `gdb build/debug/bin/@GAME_ID@`, then `break
main` and `run`. On Windows open the generated Debug executable in Visual Studio's
native debugger. OpenGL, software rendering, and a real window require a working
desktop session; Linux CI uses Xvfb.
