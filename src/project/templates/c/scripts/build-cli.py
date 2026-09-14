"""Build the project's pinned packaging CLI from source using caller toolchains."""
from pathlib import Path
import json
import re
import subprocess
root = Path(__file__).resolve().parents[1]
ref = json.loads((root / "grapple-project.json").read_text())["cli_commit"]
if not re.fullmatch(r"[a-f0-9]{40}", ref):
    raise SystemExit("Set cli_commit to a full commit containing schema-1 C project support")
source = root / "build/cli-source"
build = root / "build/cli"
def run(args):
    subprocess.run(args, check=True, timeout=1800)
if not source.exists():
    run(["git", "clone", "--filter=blob:none", "--no-checkout", "https://github.com/bluesentinelsec/grapple-beam.git", str(source)])
run(["git", "-C", str(source), "fetch", "origin", ref])
run(["git", "-C", str(source), "checkout", "--detach", ref])
run(["cmake", "-S", str(source), "-B", str(build), "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Release",
     "-DGRAPPLE_BEAM_BUILD_TESTS=OFF", "-DGRAPPLE_BEAM_BUILD_BENCHMARKS=OFF", "-DGRAPPLE_BEAM_BUILD_DEMOS=OFF", "-DGRAPPLE_BUILD_SHARED_SDK=OFF"])
run(["cmake", "--build", str(build), "--target", "grapple-beam_app", "--parallel", "4"])
