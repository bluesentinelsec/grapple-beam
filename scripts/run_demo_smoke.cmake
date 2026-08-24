# Play a demo for a few seconds with no display, and fail if it dies.
#
#   cmake -DDEMO=<executable> [-DSCRIPT=<file>] -DSECONDS=5 -P scripts/run_demo_smoke.cmake
#
# What this checks is the thing a demo is uniquely placed to catch — that the
# engine survives being *run*, frame after frame, rather than being called once
# by a unit test.
#
# Surviving the clock is necessary but not sufficient: a game that renders a
# black screen and moves nothing would also survive it. So the demos narrate
# each point when they run without a window, and a run that scored nothing is a
# failure. That makes a pass mean the loop ticked, the ball moved, and it
# bounced off something.
cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED DEMO)
  message(FATAL_ERROR "usage: -DDEMO=<executable> [-DSCRIPT=<file>] [-DSECONDS=n]")
endif()
if(NOT DEFINED SECONDS)
  set(SECONDS 5)
endif()

set(command "${DEMO}")
if(DEFINED SCRIPT)
  list(APPEND command "${SCRIPT}")
endif()

# The dummy drivers let this run on a CI machine with no display and no sound
# card, which is every CI machine.
# The demos read this and ask the engine for its headless mode, which draws
# to a surface instead of a window. The dummy *video driver* is not enough on
# its own: the engine asks for an OpenGL renderer, and dummy has no GL.
set(ENV{GRAPPLE_HEADLESS} "1")
set(ENV{SDL_AUDIODRIVER} "dummy")

execute_process(
  COMMAND ${command}
  TIMEOUT ${SECONDS}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE output
)

if(result MATCHES "timeout" OR result EQUAL 0)
  if(NOT output MATCHES "score [0-9]+-[0-9]+")
    message(FATAL_ERROR
      "the demo ran but never scored a point, so the simulation is not "
      "advancing:\n${output}")
  endif()
endif()

if(result MATCHES "timeout")
  message(STATUS "still playing after ${SECONDS}s, points scored — ok")
  return()
endif()

if(result EQUAL 0)
  # Exiting cleanly is unusual for a game with no input, but it is not a
  # failure — say so rather than hiding it.
  message(STATUS "exited cleanly before the clock ran out:\n${output}")
  return()
endif()

message(FATAL_ERROR "the demo stopped with ${result}:\n${output}")
