/** @file chiptune_smoke.h @brief Device/browser proof of MusicXML import and CPU synthesis. */
#ifndef GRAPPLE_TEST_CHIPTUNE_SMOKE_H
#define GRAPPLE_TEST_CHIPTUNE_SMOKE_H
#include <cmath>
#include <grapple/chiptune.h>

inline bool ChipMusicXmlSmoke()
{
    const char xml[] =
        "<score-partwise><part-list><score-part id='P'><part-name>harmony</part-name>"
        "</score-part></part-list><part id='P'><measure><attributes><divisions>3</divisions>"
        "</attributes><note><pitch><step>C</step><octave>4</octave></pitch><duration>1</duration>"
        "</note><note><pitch><step>E</step><octave>4</octave></pitch><duration>1</duration></note>"
        "<note><pitch><step>G</step><octave>4</octave></pitch><duration>1</duration></note>"
        "</measure></part></score-partwise>";
    auto *song = Grapple_LoadChipSongMemory(xml, sizeof(xml) - 1, nullptr, nullptr);
    if (!song)
        return false;
    const auto *info = Grapple_GetChipSongInfo(song);
    bool ok = info->track_count == 1 && std::abs(info->duration_seconds - 0.5) < 0.000001;
    auto *player = Grapple_CreateChipPlayer(song, 8000, 8, false);
    if (player)
    {
        float pcm[1024];
        ok &= Grapple_RenderChipPlayer(player, pcm, 512) == 512;
        double energy = 0;
        for (float sample : pcm)
        {
            ok &= std::isfinite(sample);
            energy += static_cast<double>(sample) * sample;
        }
        ok &= energy > 0;
    }
    else
        ok = false;
    Grapple_DestroyChipPlayer(player);
    Grapple_DestroyChipSong(song);
    return ok;
}
#endif
