#!/usr/bin/env bash
#
# Run the Latin Translator in each language, one after another. Press Escape
# (or close the window) to end one and start the next.
#
#   ./run_all.sh                 # all four, in order
#   ./run_all.sh lua ruby        # only the ones named
#   BUILD_DIR=build/release ./run_all.sh
#
# Each window has a label saying which implementation you are looking at.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
here="$repo_root/practice/gui-latin-translator"
build_dir="${BUILD_DIR:-$repo_root/build/debug}"
bin="$build_dir/bin"

# Which to run, and in what order. Defaults to all four.
if [[ $# -gt 0 ]]; then
  order=("$@")
else
  order=(c cpp lua ruby)
fi

# The C and C++ versions are executables and need building; the scripts do
# not, but they need the runner, which is also a build product.
echo "==> configuring $build_dir"
cmake -S "$repo_root" -B "$build_dir" -DGRAPPLE_BEAM_BUILD_PRACTICE=ON >/dev/null

# Collected as a de-duplicated string rather than an array: lua and ruby both
# want the runner, and macOS still ships bash 3.2, which has no mapfile.
targets=""
want_target() {
  case " $targets " in
    *" $1 "*) ;;
    *) targets="$targets $1" ;;
  esac
}

for language in "${order[@]}"; do
  case "$language" in
    c) want_target latin_c ;;
    cpp) want_target latin_cpp ;;
    lua | ruby) want_target grapple ;;
    *)
      echo "unknown implementation: $language (expected c, cpp, lua or ruby)" >&2
      exit 2
      ;;
  esac
done

echo "==> building:$targets"
# Unquoted on purpose: $targets is a list of target names, not one argument.
cmake --build "$build_dir" --target $targets --parallel

run_one() {
  local label="$1"
  shift
  echo
  echo "==> $label — press Escape to close and continue"
  # A demo that exits non-zero should be visible, but it should not stop the
  # rest of the run: seeing the other three is usually the point. The status
  # is captured rather than read from $? after an `if`, which would report the
  # negation instead of the program's own code.
  local status=0
  "$@" || status=$?
  if [[ $status -ne 0 ]]; then
    echo "!!! $label exited with $status" >&2
  fi
}

for language in "${order[@]}"; do
  case "$language" in
    c) run_one "C" "$bin/latin-c" ;;
    cpp) run_one "C++" "$bin/latin-cpp" ;;
    lua) run_one "Lua" "$bin/grapple" "$here/latin.lua" ;;
    ruby) run_one "Ruby" "$bin/grapple" "$here/latin.rb" ;;
  esac
done

echo
echo "==> done"
