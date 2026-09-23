/**
 * @file player.c
 * @brief MusicXML/MIDI/MXL player with transport, looping, and runtime styles.
 * Original Grapple code (zlib).
 */
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <grapple/chiptune.h>
#include <grapple/debug_text.h>
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <stdio.h>
#include <string.h>

typedef struct Playlist
{
    char **paths;
    int count;
    int index;
} Playlist;

typedef struct MusicPlayer
{
    Grapple_Engine *engine;
    Grapple_ChipSong *song;
    Grapple_ChipPlayer *player;
    Playlist list;
    bool loop;
    bool playing;
    int style_index;
    char status[256];
} MusicPlayer;

static const char *kStyleIds[] = {"c64",           "nes-smb",    "gb-pokemon",
                                  "genesis-sonic", "snes-zelda", "dos-wolf3d"};

static bool HasScoreExt(const char *name)
{
    const char *dot = SDL_strrchr(name, '.');
    if (!dot)
        return false;
    return SDL_strcasecmp(dot, ".xml") == 0 || SDL_strcasecmp(dot, ".musicxml") == 0 ||
           SDL_strcasecmp(dot, ".mxl") == 0 || SDL_strcasecmp(dot, ".mid") == 0 ||
           SDL_strcasecmp(dot, ".midi") == 0;
}

static int ComparePath(const void *a, const void *b)
{
    return SDL_strcasecmp(*(char *const *)a, *(char *const *)b);
}

static void FreePlaylist(Playlist *list)
{
    for (int i = 0; i < list->count; ++i)
        SDL_free(list->paths[i]);
    SDL_free(list->paths);
    SDL_zero(*list);
}

static bool BuildPlaylist(Playlist *list, const char *path)
{
    FreePlaylist(list);
    char *parent = SDL_strdup(path);
    if (!parent)
        return false;
    char *slash = SDL_strrchr(parent, '/');
#if defined(_WIN32)
    char *bslash = SDL_strrchr(parent, '\\');
    if (!slash || (bslash && bslash > slash))
        slash = bslash;
#endif
    if (slash)
        *slash = 0;
    else
        SDL_strlcpy(parent, ".", SDL_strlen(parent) + 1);
    int count = 0;
    char **files = SDL_GlobDirectory(parent, "*", 0, &count);
    if (!files)
    {
        list->paths = SDL_malloc(sizeof(char *));
        list->paths[0] = SDL_strdup(path);
        list->count = 1;
        list->index = 0;
        SDL_free(parent);
        return true;
    }
    for (int i = 0; i < count; ++i)
    {
        if (!HasScoreExt(files[i]))
            continue;
        char full[1024];
        SDL_snprintf(full, sizeof(full), "%s/%s", parent, files[i]);
        char **grown = SDL_realloc(list->paths, (size_t)(list->count + 1) * sizeof(char *));
        if (!grown)
            break;
        list->paths = grown;
        list->paths[list->count++] = SDL_strdup(full);
    }
    SDL_free(files);
    SDL_free(parent);
    if (list->count == 0)
    {
        list->paths = SDL_malloc(sizeof(char *));
        list->paths[0] = SDL_strdup(path);
        list->count = 1;
    }
    SDL_qsort(list->paths, (size_t)list->count, sizeof(char *), ComparePath);
    list->index = 0;
    for (int i = 0; i < list->count; ++i)
        if (SDL_strcmp(list->paths[i], path) == 0)
            list->index = i;
    return true;
}

static void ApplyStyle(MusicPlayer *mp, int index)
{
    if (index < 0)
        index = (int)SDL_arraysize(kStyleIds) - 1;
    if (index >= (int)SDL_arraysize(kStyleIds))
        index = 0;
    mp->style_index = index;
    const Grapple_ChipStyle *style = Grapple_GetChipStyle(kStyleIds[index]);
    if (!mp->player || !style || !Grapple_SetChipPlayerStyle(mp->player, style))
        SDL_snprintf(mp->status, sizeof(mp->status), "style failed: %s", SDL_GetError());
    else
    {
        Grapple_ChipStyleInfo info;
        Grapple_ReadChipStyleInfo(style, &info);
        SDL_snprintf(mp->status, sizeof(mp->status), "style %d: %s", index + 1, info.name);
    }
}

static bool LoadCurrent(MusicPlayer *mp)
{
    const char *path = mp->list.paths[mp->list.index];
    Grapple_DestroyChipPlayer(mp->player);
    Grapple_DestroyChipSong(mp->song);
    mp->player = NULL;
    mp->song = Grapple_LoadChipSong(path);
    if (!mp->song)
    {
        SDL_snprintf(mp->status, sizeof(mp->status), "load: %s", SDL_GetError());
        return false;
    }
    mp->player = Grapple_CreateChipPlayer(mp->song, 48000, 64, mp->loop);
    if (!mp->player)
    {
        SDL_snprintf(mp->status, sizeof(mp->status), "player: %s", SDL_GetError());
        return false;
    }
    ApplyStyle(mp, mp->style_index);
    if (!Grapple_PlayChipPlayer(mp->player))
        return false;
    mp->playing = true;
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(mp->song);
    SDL_snprintf(mp->status, sizeof(mp->status), "%s  (%.1fs, %d tracks)", path,
                 info->duration_seconds, info->track_count);
    return true;
}

static void SeekBy(MusicPlayer *mp, Sint64 delta_ticks)
{
    if (!mp->player || !mp->song)
        return;
    Grapple_ChipPosition pos;
    if (!Grapple_ReadChipPlayerPosition(mp->player, &pos))
        return;
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(mp->song);
    Sint64 tick = (Sint64)pos.tick + delta_ticks;
    if (tick < 0)
        tick = 0;
    if ((Uint64)tick > info->duration_ticks)
        tick = (Sint64)info->duration_ticks;
    Grapple_SeekChipPlayer(mp->player, (Uint64)tick);
}

static void ToggleLoop(MusicPlayer *mp)
{
    if (!mp->player || !mp->song)
        return;
    mp->loop = !mp->loop;
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(mp->song);
    Grapple_SetChipPlayerLoop(mp->player, 0, info->duration_ticks, mp->loop);
    SDL_snprintf(mp->status, sizeof(mp->status), "loop %s", mp->loop ? "on" : "off");
}

static void NextSong(MusicPlayer *mp)
{
    if (mp->list.count == 0)
        return;
    mp->list.index = (mp->list.index + 1) % mp->list.count;
    LoadCurrent(mp);
}

static void Update(void *user, float dt)
{
    MusicPlayer *mp = user;
    (void)dt;
    Grapple_Engine *engine = mp->engine;
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_ESCAPE))
        Grapple_EngineQuit(engine);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_SPACE))
    {
        if (mp->playing)
        {
            Grapple_PauseChipPlayer(mp->player);
            mp->playing = false;
        }
        else if (mp->player)
        {
            Grapple_PlayChipPlayer(mp->player);
            mp->playing = true;
        }
    }
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_LEFT))
        SeekBy(mp, -(Sint64)Grapple_GetChipSongInfo(mp->song)->ticks_per_quarter * 4);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_RIGHT))
        SeekBy(mp, (Sint64)Grapple_GetChipSongInfo(mp->song)->ticks_per_quarter * 4);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_HOME) || Grapple_KeyPressed(engine, SDL_SCANCODE_R))
    {
        Grapple_ResetChipPlayer(mp->player);
        Grapple_PlayChipPlayer(mp->player);
        mp->playing = true;
    }
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_N))
        NextSong(mp);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_L))
        ToggleLoop(mp);
    const SDL_Scancode numbers[] = {SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                                    SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6};
    for (int i = 0; i < (int)SDL_arraysize(numbers); ++i)
        if (Grapple_KeyPressed(engine, numbers[i]))
            ApplyStyle(mp, i);
}

static void Render(void *user, float alpha)
{
    MusicPlayer *mp = user;
    (void)alpha;
    SDL_Renderer *renderer = Grapple_EngineRenderer(mp->engine);
    SDL_SetRenderDrawColor(renderer, 16, 18, 32, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 180, 230, 255, 255);
    Grapple_SetDebugTextSize(22);
    Grapple_RenderDebugText(renderer, 20, 18, "Grapple chiptune player");
    Grapple_SetDebugTextSize(14);
    const char *path = mp->list.count ? mp->list.paths[mp->list.index] : "";
    Grapple_RenderDebugText(renderer, 20, 56, path);
    Grapple_RenderDebugText(renderer, 20, 80, mp->status);
    Grapple_ChipStyleInfo info;
    if (mp->player && Grapple_GetChipPlayerStyleInfo(mp->player, &info))
    {
        char line[192];
        SDL_snprintf(line, sizeof(line), "Preset: %s%s  loop:%s  %s", info.name,
                     info.approximation ? " (approx)" : "", mp->loop ? "on" : "off",
                     mp->playing ? "playing" : "paused");
        Grapple_RenderDebugText(renderer, 20, 108, line);
    }
    Grapple_RenderDebugText(renderer, 20, 150,
                            "Space play/pause   Left/Right seek   R/Home restart");
    Grapple_RenderDebugText(renderer, 20, 172, "N next file   L loop   1-6 styles   Esc quit");
    Grapple_RenderDebugText(renderer, 20, 204,
                            "1 C64  2 NES  3 Game Boy  4 Genesis  5 SNES  6 MS-DOS");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: chiptune-player score.xml|mid|mxl\n");
        return 2;
    }
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) || !MIX_Init())
    {
        fprintf(stderr, "audio: %s\n", SDL_GetError());
        return 1;
    }
    MusicPlayer mp = {0};
    mp.loop = true;
    if (!BuildPlaylist(&mp.list, argv[1]))
    {
        fprintf(stderr, "playlist: %s\n", SDL_GetError());
        return 1;
    }
    Grapple_EngineConfig config = {0};
    config.title = "Chiptune player";
    config.design_width = 900;
    config.design_height = 280;
    config.no_auto_mount = true;
    config.headless = SDL_getenv("GRAPPLE_HEADLESS") != NULL;
    mp.engine = Grapple_CreateEngine(&config);
    bool ok = mp.engine && LoadCurrent(&mp);
    if (ok)
    {
        Grapple_GameHooks hooks = {0};
        hooks.update = Update;
        hooks.render = Render;
        ok = Grapple_RunGame(mp.engine, &hooks, &mp);
    }
    if (!ok)
        fprintf(stderr, "playback: %s\n", SDL_GetError());
    Grapple_DestroyChipPlayer(mp.player);
    Grapple_DestroyChipSong(mp.song);
    Grapple_DestroyEngine(mp.engine);
    FreePlaylist(&mp.list);
    MIX_Quit();
    SDL_Quit();
    return ok ? 0 : 1;
}
