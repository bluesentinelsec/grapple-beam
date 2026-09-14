# Packaging and distribution

Run `grapple-beam package` in the project root with a CLI supporting schema-1 C
projects. The engine pin and CLI pin are separate: v0.9.0 is a supported engine
source baseline but its runner cannot package this template. CI builds the CLI
from the source commit recorded in grapple-project.json. Deliberately update that
pin when upgrading tooling.

Packaging builds/tests Release, invokes the Game CMake install component, checks
runtime imports, verifies assets/rendering from a temporary directory outside the
checkout, and creates versioned OS/architecture game and symbol ZIPs with SHA-256 sidecars in dist/.
Mac output contains an .app; Windows and Linux contain the executable plus runtime
resources/notices. The packaged application runs without this CLI or the source
checkout. Only the starter's static dependency set is supported automatically;
new shared libraries require explicit installation and audit rules.

Use the same command locally and in CI. The release workflow checks a `vX.Y.Z`
tag against root VERSION and attaches tested archives to a GitHub Release.
Update the game's license holder, identity, player docs and download links first.

Signing, notarization, installers and store submission are separate work. These
archives are unsigned and may encounter platform trust restrictions. Linux builds
inherit the host's libc/system baseline; use the oldest intended supported distro
for release builds and document that baseline. No cross-OS build service or
browser/mobile application is produced. Build each desktop payload on its own OS.
