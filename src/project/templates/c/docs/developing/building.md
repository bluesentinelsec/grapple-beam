# Building

See [the root README](../../README.md) for the five Make operations and Windows wrapper.
Use CMake 3.25+, Git and a C17/C++20 compiler. Presets use Ninja; direct CMake
also supports Unix Makefiles and Visual Studio generators. Debug and Release
have separate build directories. A first build fetches the pinned engine source
and its pinned dependencies. Subsequent builds do not query for a newer release.

On Ubuntu install build-essential, cmake, ninja-build and SDL development libraries:
libasound2-dev libpulse-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev
libxfixes-dev libxi-dev libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev libgbm-dev
libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev libudev-dev
libwayland-dev libdecor-0-dev libpipewire-0.3-dev. The CI workflow is the tested list.

Authored game sources are compiled as C17. The engine's selected modules link
statically; platform system libraries retain their normal runtime requirements.
To add a module, enable its supported engine CMake option and link its Grapple::
target, then update notice collection and package validation. Do not add source
files to build/ or dist/: clean owns these directories.
