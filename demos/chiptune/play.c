/**
 * @file play.c
 * @brief Load MIDI/MusicXML/MXL and play it from the engine loop, or export a WAV.
 * Original Grapple code (zlib).
 */
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <grapple/chiptune.h>
#include <grapple/debug_text.h>
#include <grapple/engine.h>
#include <stdio.h>

typedef struct MusicDemo
{
    Grapple_Engine *engine;
    Grapple_ChipPlayer *player;
    const char *path;
} MusicDemo;

static void Update(void *user, float dt)
{
    MusicDemo *demo = user;
    (void)dt;
    if (!Grapple_ChipPlayerPlaying(demo->player))
        Grapple_EngineQuit(demo->engine);
}

static void Render(void *user, float alpha)
{
    MusicDemo *demo = user;
    (void)alpha;
    SDL_Renderer *renderer = Grapple_EngineRenderer(demo->engine);
    SDL_SetRenderDrawColor(renderer, 20, 22, 42, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 150, 220, 255, 255);
    Grapple_SetDebugTextSize(24);
    Grapple_RenderDebugText(renderer, 24, 28, "Music notation -> C64-style sound");
    Grapple_SetDebugTextSize(16);
    Grapple_RenderDebugText(renderer, 24, 84, demo->path);
    Grapple_RenderDebugText(renderer, 24, 140, "Melody: delay + reverb   Bass: chorus");
    Grapple_RenderDebugText(renderer, 24, 174, "Harmony: wet eighth-note pulse   Drums: dry");
    Grapple_RenderDebugText(renderer, 24, 242, "Closes after music and effect tails finish.");
}

static void DescribeSong(const Grapple_ChipSong *song)
{
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(song);
    printf("Loaded %d tracks, %.3f seconds, %d ticks per quarter\n", info->track_count,
           info->duration_seconds, info->ticks_per_quarter);
    for (int i = 0; i < info->track_count; ++i)
    {
        const Grapple_ChipTrackInfo *track = Grapple_GetChipTrackInfo(song, i);
        printf("  %d: %s (%u notes)\n", i, track->name, (unsigned)track->note_count);
    }
    for (int i = 0; i < Grapple_GetChipDiagnosticCount(song); ++i)
        printf("  Import: %s\n", Grapple_GetChipDiagnosticMessage(song, i));
    fflush(stdout);
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "usage: chiptune-demo score.mid|score.musicxml|score.mxl [output.wav]\n");
        return 2;
    }

    /* Parsing and score expansion happen before playback, away from the audio callback. */
    Grapple_ChipSong *song = Grapple_LoadChipSong(argv[1]);
    if (!song)
    {
        fprintf(stderr, "load: %s\n", SDL_GetError());
        return 1;
    }
    DescribeSong(song);
    if (argc == 3)
    {
        const bool saved = Grapple_SaveChipSongWav(song, argv[2], 48000, 64);
        if (!saved)
            fprintf(stderr, "WAV export: %s\n", SDL_GetError());
        Grapple_DestroyChipSong(song);
        return saved ? 0 : 1;
    }

    if (!SDL_Init(SDL_INIT_AUDIO) || !MIX_Init())
    {
        fprintf(stderr, "audio: %s\n", SDL_GetError());
        Grapple_DestroyChipSong(song);
        SDL_Quit();
        return 1;
    }
    Grapple_EngineConfig config = {0};
    config.title = "Chiptune library demo";
    config.design_width = 800;
    config.design_height = 300;
    config.no_auto_mount = true;
    config.headless = SDL_getenv("GRAPPLE_HEADLESS") != NULL;
    MusicDemo demo = {0};
    demo.path = argv[1];
    demo.engine = Grapple_CreateEngine(&config);
    if (demo.engine)
        demo.player = Grapple_PlayChipSong(song, false);
    /* The player retains the immutable song. The loader's reference can go now. */
    Grapple_DestroyChipSong(song);

    bool ok = demo.engine && demo.player;
    if (ok)
    {
        Grapple_GameHooks hooks = {0};
        hooks.update = Update;
        hooks.render = Render;
        ok = Grapple_RunGame(demo.engine, &hooks, &demo);
    }
    if (!ok)
        fprintf(stderr, "playback: %s\n", SDL_GetError());
    Grapple_DestroyChipPlayer(demo.player);
    Grapple_DestroyEngine(demo.engine);
    MIX_Quit();
    SDL_Quit();
    return ok ? 0 : 1;
}
