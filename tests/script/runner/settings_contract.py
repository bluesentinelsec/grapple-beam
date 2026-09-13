"""Layer precedence, project isolation, script errors and non-destructive recovery."""
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import uuid
import zipfile

executable = str(Path(sys.argv[1]).resolve())

def run(*args, status=0, cwd=None):
    result = subprocess.run([executable, *map(str, args)], text=True, capture_output=True,
                            timeout=20, cwd=cwd)
    assert result.returncode == status, (args, result.returncode, result.stdout, result.stderr)
    return result.stdout + result.stderr

def value(output, key):
    match = re.search(r"^" + re.escape(key) + r" = (.*?)\s+#", output, re.M)
    assert match, output
    return match.group(1)

with tempfile.TemporaryDirectory(prefix="grapple settings '") as directory:
    root = Path(directory)
    for language, extension in [("lua", "lua"), ("ruby", "rb")]:
        project = root / language
        project.mkdir()
        identity = "settings-test-" + uuid.uuid4().hex
        (project / "grapple.toml").write_text(
            '[game]\norganization="grapple-tests"\nid="' + identity + '"\nentry="main.' + extension + '"\n')
        (project / ("main." + extension)).write_text(
            'assert(io.open("asset.txt"):read("*a") == "ROOT"); print("GAME_RAN")' if language == "lua"
            else 'require "asset"; raise "root" unless ASSET == "ROOT"; puts "GAME_RAN"')
        (project / "asset.txt").write_text("ROOT")
        (project / "asset.rb").write_text('ASSET = "ROOT"')
        (project / "config.toml").write_text('[display]\nvsync=false\n[effects]\nbloom=0.2\n')
        script = project / ("config." + extension)
        script.write_text('return {effects={bloom=0.3}}' if language == "lua" else '{effects: {bloom: 0.3}}')
        output = run("--print-settings", project)
        assert abs(float(value(output, "effects.bloom")) - 0.3) < 1e-6
        assert value(output, "display.vsync") == "off"
        pref_line = next(line for line in output.splitlines() if line.startswith("Player configuration directory: "))
        preferences = Path(pref_line.split(": ", 1)[1].strip('"'))
        try:
            (preferences / "config.toml").write_text('[effects]\nbloom=0.4\n[audio]\nmusic_volume=0\n')
            player_script = preferences / ("config." + extension)
            player_script.write_text('return {effects={bloom=0.5}}' if language == "lua" else '{effects: {bloom: 0.5}}')
            extra = root / "overlay.toml"
            extra.write_text('[effects]\nbloom=0.6\n')
            extra_script = root / ("overlay." + extension)
            extra_script.write_text('return {effects={bloom=0.7}}' if language == "lua" else '{effects: {bloom: 0.7}}')
            output = run("--print-settings", project)
            assert value(output, "effects.bloom") == "0.5"
            assert value(output, "audio.music_volume") == "0"
            output = run("--print-settings", "--config-script", extra_script, "--config", extra, project)
            assert abs(float(value(output, "effects.bloom")) - 0.7) < 1e-6
            output = run("--print-settings", "--bloom=0", "--config-script", extra_script, project)
            assert value(output, "effects.bloom") == "0"
            assert "CLI" in next(line for line in output.splitlines() if line.startswith("effects.bloom ="))
            output = run("--print-settings", "--default-settings", project)
            assert abs(float(value(output, "effects.bloom")) - 0.3) < 1e-6
            output = run("--print-settings", "--quality=low", "--shadows=high", project)
            assert value(output, "quality.shadows") == "high"
            assert value(output, "quality.dynamic_lights") == "low"
            output = run("--print-settings", "--shadows=high", "--quality=low", project)
            assert value(output, "quality.shadows") == "high"
            save = preferences / "save.dat"
            save.write_text("PRESERVE")
            run("--reset-settings", "--print-settings", project)
            assert player_script.exists() and not list(preferences.glob("settings-backup-*"))
            run("--reset-settings", "--resolution=1920x1080", project, status=2)
            assert player_script.exists()
            assert "GAME_RAN" in run("--reset-settings", project, cwd=root)
            assert not player_script.exists() and save.read_text() == "PRESERVE"
            backup = list(preferences.glob("settings-backup-*"))
            assert len(backup) == 1 and (backup[0] / "config.toml").exists()
            script.write_text("this is an invalid script")
            run("--print-settings", project, status=2)
            output = run("--safe-mode", "--print-settings", project)
            assert value(output, "engine.backend") == "software"
            assert value(output, "display.window_mode") == "windowed"
            run("--safe-mode", "--config", extra, project, status=2)
            run("--safe-mode", "--reset-settings", project, status=2)
        finally:
            shutil.rmtree(preferences)
    for extension in ("lua", "rb"):
        script = root / ("runtime." + extension)
        script.write_text("""
local c = GrappleC.ConfigCreate()
GrappleC.ConfigSetHeadless(c, true)
GrappleC.ConfigSetAutoMount(c, false)
local e = GrappleC.CreateEngine(c)
local s = GrappleC.EngineRequestedSettings(e)
assert(GrappleC.SettingsGet(s, "engine.tick_rate") == "120")
assert(GrappleC.SettingsSource(s, "engine.tick_rate") == "CLI")
assert(GrappleC.SettingsPlayerPath(s))
assert(GrappleC.GetAudioBusGain(GrappleC.GRAPPLE_AUDIO_SPEECH) == 0)
GrappleC.DestroyEngine(e)
GrappleC.ConfigDestroy(c)
print("RUNTIME_OK")
""" if extension == "lua" else """
c = GrappleC.ConfigCreate
GrappleC.ConfigSetHeadless(c, true)
GrappleC.ConfigSetAutoMount(c, false)
e = GrappleC.CreateEngine(c)
s = GrappleC.EngineRequestedSettings(e)
raise "override" unless GrappleC.SettingsGet(s, "engine.tick_rate") == "120"
raise "source" unless GrappleC.SettingsSource(s, "engine.tick_rate") == "CLI"
raise "player path" unless GrappleC.SettingsPlayerPath(s)
raise "speech" unless GrappleC.GetAudioBusGain(GrappleC::GRAPPLE_AUDIO_SPEECH) == 0
GrappleC.DestroyEngine(e)
GrappleC.ConfigDestroy(c)
puts "RUNTIME_OK"
""")
        assert "RUNTIME_OK" in run("--set=engine.tick_rate=120", "--speech-volume=0", script)
    media = root / "external-media"
    media.mkdir()
    (media / "config.toml").write_text('[effects]\nbloom=0.23\n')
    script = root / "media-test.lua"
    script.write_text('print("media")')
    output = run("--media", media, "--print-settings", script)
    assert abs(float(value(output, "effects.bloom")) - .23) < 1e-6
    archive = root / "assets.zip"
    with zipfile.ZipFile(archive, "w") as zip_file:
        zip_file.writestr("config.toml", '[effects]\nbloom=0.45\n')
    output = run("--media", archive, "--print-settings", script)
    assert abs(float(value(output, "effects.bloom")) - .45) < 1e-6
    run("--media", media, "--auto-mount=off", "--print-settings", script, status=2)
    output = run("--media", archive, "--bloom=0", "--print-settings", script)
    assert value(output, "effects.bloom") == "0"
    (root / "main.lua").write_text('print("lua")')
    (root / "main.rb").write_text('puts "ruby"')
    run(root, status=2)
    assert "lua" in run("--language=lua", root)
print("Settings contract passed")
