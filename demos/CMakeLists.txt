# Demos: complete little games, one per language surface.
#
# They are built by default at top level and never installed. A game here is
# not a test — it is the thing the tests are for — but CI still runs each one
# headless for a few hundred frames, because a demo that stopped compiling or
# started crashing is a bug report the engine should file against itself.
#
# Every demo drives the opinionated loop rather than its own `while`, which is
# the arrangement the engine exists to provide and therefore the one worth
# demonstrating.
add_subdirectory(pong)
