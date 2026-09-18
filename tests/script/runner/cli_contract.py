"""Exercise the installed command boundary without opening game windows."""
from pathlib import Path
import subprocess
import sys
import tempfile


def run(*args, status=0, contains=None, stdin=""):
    result = subprocess.run([sys.argv[1], *args], input=stdin, text=True,
                            capture_output=True, timeout=15)
    assert result.returncode == status, (args, result.returncode, result.stdout, result.stderr)
    output = result.stdout + result.stderr
    if contains:
        assert contains in output, (args, output)
    return output


with tempfile.TemporaryDirectory(prefix="grapple cli '") as directory:
    lua = Path(directory) / "game.lua"
    ruby = Path(directory) / "game.rb"
    lua.write_text('print("GAME_RAN")\nfor _, v in ipairs(arg) do print(v) end\n')
    ruby.write_text('puts "GAME_RAN"\nARGV.each { |v| puts v }\n')
    for script in (lua, ruby):
        for name, value in [("window-mode", "fullscreen-exclusive"), ("vsync", "off"),
                            ("reduced-flashing", "on"), ("max-fps", "unlimited"),
                            ("backend", "software"), ("window-size", "640x360")]:
            run("run", "--" + name, value, str(script), contains="GAME_RAN")
            run("run", "--" + name + "=" + value, str(script), contains="GAME_RAN")
        run("run", str(script), "--", "--vsnyc", "on", contains="--vsnyc\non")
        run("run", str(script), "--level", "3", contains="--level\n3")
        run("run", "--crt", "0", "--crt", "1", str(script), contains="GAME_RAN")
        for invalid in [("--vsnyc=off",), ("--fullscreen",), ("--windowed",),
                        ("--window-mode", "borderless"), ("--vsync", "false"),
                        ("--max-fps", "-1"), ("--max-fps", "0"),
                        ("--bloom", "nan"), ("--crt", "1.1"),
                        ("--max-fps", "60fps"), ("--window-size", "640x360garbage"),
                        ("--backend", "native"), ("--backend", "imaginary")]:
            output = run("run", *invalid, str(script), status=2)
            assert "GAME_RAN" not in output
        run("run", "--reduced-flashing", str(script), status=2, contains="on")
    run("run", "--max-fps", status=2, contains="requires a value")
    run("run", "-l", "ruby", str(lua), status=2, contains="language must agree")
    help_text = run("--help", contains="new")
    for command in ("new", "package", "run", "repl", "eval"):
        assert command in help_text, (command, help_text)
    run("run", "--help", contains="--window-mode")
    run("--help-all", contains="--list-backends")
    run("--version", contains="grapple-beam")
    run("new", "--help", contains="destination")
    run("eval", "--language", "lua", "--code", "print(6*7)", contains="42")
    run("eval", "--language", "ruby", "--code", "puts ARGV[0]", "--", "--game-arg", contains="--game-arg")
    run("eval", "--code", "print(1)", status=2)
    run("repl", "--language", "lua", stdin="exit\n", contains="Lua")
    run("-l", "lua", status=2)
    run("run", "--list-backends", str(lua), status=2)
    run(status=2)
print("CLI contract passed")
