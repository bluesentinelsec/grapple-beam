/**
 * @file main.cpp
 * @brief Program entrypoint (always src/main.cpp in cppboot projects).
 *
 * Keep this file thin: parse args / wire dependencies, then call library code.
 */

#include "grapple/version.hpp"
#ifdef GRAPPLE_APP_CHIPTUNE
#include "chiptune_preview/preview.h"
#endif

#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

/**
 * @brief Program entry.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit status.
 */
int main(int argc, char **argv)
{
    CLI::App app{"grapple-beam — cppboot project"};
    app.set_version_flag("-V,--version", std::string{grapple::Version()});
#ifdef GRAPPLE_APP_CHIPTUNE
    std::string chip_midi;
    std::string chip_wav;
    int ring_track = -1;
    int solo_track = -1;
    int voices = 64;
    bool loop = false;
    bool dry = false;
    double pulse_beats = 0.5;
    float pulse_depth = 0.7F;
    auto *midi_option =
        app.add_option("--chip-midi", chip_midi, "Play a MIDI file with SID-style instruments")
            ->check(CLI::ExistingFile);
    app.add_option("--chip-wav", chip_wav, "Render MIDI to a stereo WAV instead of playing")
        ->needs(midi_option);
    app.add_option("--chip-harmony-track,--chip-ring-track", ring_track,
                   "Zero-based file track for the wet pulsing harmony preset")
        ->needs(midi_option);
    app.add_option("--chip-solo-track", solo_track, "Audition only this zero-based file track")
        ->needs(midi_option);
    app.add_option("--chip-voices", voices, "Polyphony (default 64)")
        ->check(CLI::Range(1, 1024))
        ->needs(midi_option);
    app.add_option("--chip-pulse-beats", pulse_beats,
                   "Beats per harmony pulse: 0.5=eighths, 1=quarters, 0=off")
        ->check(CLI::Range(0.0, 16.0))
        ->needs(midi_option);
    app.add_option("--chip-pulse-depth", pulse_depth, "Harmony pulse depth, 0..1")
        ->check(CLI::Range(0.0, 1.0))
        ->needs(midi_option);
    app.add_flag("--chip-dry", dry, "Disable modern effects and harmony motion")
        ->needs(midi_option);
    app.add_flag("--chip-loop", loop, "Loop MIDI playback until interrupted")->needs(midi_option);
#endif
    CLI11_PARSE(app, argc, argv);
#ifdef GRAPPLE_APP_CHIPTUNE
    if (!chip_midi.empty())
        return ChipPreview_Run(chip_midi.c_str(), chip_wav.c_str(), ring_track, solo_track, voices,
                               loop, pulse_beats, pulse_depth, dry);
#endif

    std::cout << "grapple-beam " << grapple::Version()
              << " — add components under src/ (see README.md / AGENTS.md)\n";
    return 0;
}
