# Dependency notices

The game uses grapple-beam's Engine, Image, Extras, Gfx, VFS, Formats, Physics and Light
modules and their static source dependencies. The engine pin is authoritative in
`cmake/GrappleDependency.cmake`. `grapple-beam.md` records its origin at creation.
CMake collects selected module licenses, inline image decoder notices, SDL3's
license, and upstream provenance into `build/<configuration>/licenses`.
`grapple-beam package` installs them into the application payload. Review this
inventory whenever adding modules, assets, shared libraries, or changing pins.
OS system libraries are not redistributed. The game's root LICENSE does not
relicense dependencies. No dependency update happens automatically during builds.
