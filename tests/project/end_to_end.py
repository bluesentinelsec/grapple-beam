"""Exercise a newly generated C project through real desktop builds and packaging."""
from pathlib import Path
import argparse
import json
import os
import shutil
import subprocess
import tempfile
import zipfile

parser = argparse.ArgumentParser()
parser.add_argument("--runner", type=Path, required=True)
parser.add_argument("--generator", default="Ninja")
parser.add_argument("--engine-version", default="v0.9.0")
parser.add_argument("--engine-source", type=Path)
parser.add_argument("--work", type=Path)
args = parser.parse_args()
runner = args.runner.resolve()
root = args.work.resolve() if args.work else Path(tempfile.mkdtemp(prefix="grapple-desktop-contract-"))
root.mkdir(parents=True, exist_ok=True)
project = root / "Space Game"

def run(command, cwd=project, timeout=1800, expected=0):
    command = [str(x) for x in command]
    print("RUN", command, flush=True)
    result = subprocess.run(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, timeout=timeout)
    if result.returncode != expected:
        print(result.stdout, flush=True)
        raise AssertionError(f"exit {result.returncode}, expected {expected}: {command}")
    return result.stdout

run([runner, "new", project, "--model=c", "--engine-version", args.engine_version], root, 180)
assert (project / ".git").is_dir()
assert run(["git", "branch", "--show-current"]).strip() == "main"
assert not run(["git", "status", "--porcelain"]).strip() == ""
assert "GIT_TAG main" not in (project / "cmake/GrappleDependency.cmake").read_text()
for config in ("Debug", "Release"):
    build = project / "build" / config.lower()
    command = ["cmake", "-S", project, "-B", build, "-G", args.generator, f"-DCMAKE_BUILD_TYPE={config}"]
    if args.engine_source:
        command += [f"-DFETCHCONTENT_SOURCE_DIR_GRAPPLE_ENGINE={args.engine_source.resolve()}"]
    run(command)
    run(["cmake", "--build", build, "--config", config, "--parallel", "4"])
    run(["ctest", "--test-dir", build, "-C", config, "--output-on-failure", "--timeout", "30"])

# Exercise the supported wrappers as well as direct CMake, using the same build trees.
wrapper = ["cmd", "/d", "/c", "build.bat"] if os.name == "nt" else ["make"]
run(wrapper)
run(wrapper + ["build-release"])
run(wrapper + ["test"])

# A header and nested C implementation must participate in the next incremental build.
source = project / "src/space_game/combat"
header = project / "include/space_game/combat"
source.mkdir(); header.mkdir()
(header / "damage.h").write_text("#define DAMAGE 7\nint Damage(void);\n")
(source / "damage.c").write_text('#include <space_game/combat/damage.h>\nint Damage(void) { return DAMAGE; }\n')
unit = project / "tests/game/movement.c"
original = unit.read_text()
unit.write_text('#include <space_game/combat/damage.h>\n' + original.replace("    GameState state", "    CHECK(Damage() == 7);\n    GameState state"))
debug = project / "build/debug"
run(["cmake", "--build", debug, "--config", "Debug", "--parallel", "4"])
run(["ctest", "--test-dir", debug, "-C", "Debug", "-R", "game_movement", "--output-on-failure"])
(header / "damage.h").write_text("#define DAMAGE 9\nint Damage(void);\n")
run(["cmake", "--build", debug, "--config", "Debug", "--parallel", "4"])
# The changed header must cause recompilation; the old assertion must now fail.
result = subprocess.run(["ctest", "--test-dir", str(debug), "-C", "Debug", "-R", "game_movement"], cwd=project)
assert result.returncode != 0
unit.write_text(unit.read_text().replace("Damage() == 7", "Damage() == 9"))
(source / "damage.c").rename(source / "renamed.c")
run(["cmake", "--build", debug, "--config", "Debug", "--parallel", "4"])
run(["ctest", "--test-dir", debug, "-C", "Debug", "-R", "game_movement", "--output-on-failure"])
unit.write_text(original)
shutil.rmtree(source); shutil.rmtree(header)
(project / "tests/fixtures/never_compile.c").write_text('#error fixture is not a source\n')
run(["cmake", "--build", debug, "--config", "Debug", "--parallel", "4"])
run(["ctest", "--test-dir", debug, "-C", "Debug", "--output-on-failure", "--timeout", "30"])

run(wrapper + ["fmt"])
run(["cmake", "-DCHECK=ON", "-P", "cmake/Format.cmake"])
run(["python3" if os.name != "nt" else "python", "scripts/build-docs.py"])
if shutil.which("doxygen"):
    run(["doxygen", "Doxyfile"])
run([runner, "package", project], root)
archives = [p for p in (project / "dist").glob("*.zip") if not p.name.endswith("-symbols.zip")]
assert list((project / "dist").glob("*-symbols.zip"))
assert archives
unpacked = root / "Unpacked outside source"
with zipfile.ZipFile(archives[0]) as archive:
    archive.extractall(unpacked)
    for entry in archive.infolist():
        path = unpacked / entry.filename
        if path.is_file() and entry.external_attr >> 16:
            path.chmod(entry.external_attr >> 16)
binaries = [p for p in unpacked.rglob("space_game*") if p.is_file() and p.name in ("space_game", "space_game.exe")]
assert len(binaries) == 1
assert "result=pass" in run([binaries[0], "--self-test"], root, 30)
assert list(unpacked.rglob("licenses/SDL3/LICENSE.txt"))
assert not list(unpacked.rglob("grapple-beam*exe"))
# Reset/print cannot touch ordinary player preferences in this integration mode.
assert "window_mode=windowed" in run([binaries[0], "--self-test", "--print-settings"], root, 30)
# Cleaning is complete and idempotent, preserving authored inputs and Git.
run(wrapper + ["clean"])
run(wrapper + ["clean"])
assert not (project / "build").exists() and not (project / "dist").exists()
assert (project / ".git").is_dir() and (project / "media/sprites/player.svg").is_file()
print("PASS: fresh C desktop build, discovery, tests, docs, packaging, relocation and cleanup")
