/** @file preview.h @brief Command-line MIDI audition and WAV rendering. */
#ifndef GRAPPLE_CHIP_PREVIEW_H
#define GRAPPLE_CHIP_PREVIEW_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Play a MIDI composition or render it to a stereo WAV file.
     * @param midi Input MIDI path.
     * @param wav Output WAV path, or an empty string for real-time playback.
     * @param ring_track File track receiving the ring preset, or -1 for none.
     * @param solo_track Only audible file track, or -1 for the full mix.
     * @param voices Maximum simultaneous notes.
     * @param loop Repeat real-time playback until interrupted; incompatible with WAV.
     * @param pulse_beats Quarter-note beats per harmony pulse, 0 disables pulses.
     * @param pulse_depth Harmony pulse depth, 0..1.
     * @param dry Disable all modern effects and harmony motion.
     * @return Zero on success, one on failure (details logged through SDL).
     * @pre Called before other application SDL/MIX initialization.
     */
    int ChipPreview_Run(const char *midi, const char *wav, int ring_track, int solo_track,
                        int voices, bool loop, double pulse_beats, float pulse_depth, bool dry);

#ifdef __cplusplus
}
#endif
#endif
