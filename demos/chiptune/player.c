/**
 * @file player.c
 * @brief MusicXML player, per-style mixer, and live instrument editor.
 * Original Grapple code (zlib).
 */
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <grapple/chiptune.h>
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/gui.h>
#include <grapple/widgets.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define PLAYER_MAX_TRACKS 8
#define PLAYER_STYLES 6

typedef struct Playlist
{
    char **paths;
    int count;
    int index;
} Playlist;

typedef struct TrackState
{
    float gain;
    Grapple_ChipEffects fx;
    bool has_fx;
} TrackState;

typedef struct StyleState
{
    bool used;
    TrackState tracks[PLAYER_MAX_TRACKS];
    Grapple_ChipRoleControls roles[GRAPPLE_CHIP_PRESET_LAST + 1];
    bool role_used[GRAPPLE_CHIP_PRESET_LAST + 1];
} StyleState;

typedef struct Bind
{
    struct MusicPlayer *mp;
    int track;
    int field;
} Bind;

typedef struct MusicPlayer
{
    Grapple_Engine *engine;
    Grapple_ChipSong *song;
    Grapple_ChipPlayer *player;
    Grapple_Ui *ui;
    Playlist list;
    StyleState styles[PLAYER_STYLES];
    bool loop;
    bool playing;
    int style_index;
    int selected_track;
    int track_count;
    char status[256];
    Grapple_UiWidget *status_label;
    Grapple_UiWidget *position_label;
    Grapple_UiWidget *style_radio;
    Grapple_UiWidget *loop_check;
    Grapple_UiWidget *tracks_row;
    Grapple_UiWidget *vol_label[PLAYER_MAX_TRACKS];
    Grapple_UiWidget *vol_slider[PLAYER_MAX_TRACKS];
    Grapple_UiWidget *fx_label[PLAYER_MAX_TRACKS][5];
    Grapple_UiWidget *fx_slider[PLAYER_MAX_TRACKS][5];
    Grapple_UiWidget *knob_label[9];
    Grapple_UiWidget *knob_slider[9];
    Bind vol_bind[PLAYER_MAX_TRACKS];
    Bind fx_bind[PLAYER_MAX_TRACKS][5];
    Bind select_bind[PLAYER_MAX_TRACKS];
    Bind knob_bind[9];
} MusicPlayer;

static const char *kStyleIds[] = {"c64",           "nes-smb",    "gb-pokemon",
                                  "genesis-sonic", "snes-zelda", "dos-wolf3d"};
static const char *kStyleNames[] = {"1 C64", "2 NES", "3 GB", "4 Genesis", "5 SNES", "6 DOS", NULL};
static const char *kFxNames[] = {"Reverb", "Chorus", "Delay", "Phaser", "Flanger"};
static const char *kKnobNames[] = {"Level", "Attack ms", "Decay ms", "Sustain",  "Release ms",
                                   "Duty",  "PWM",       "Vibrato",  "Cutoff Hz"};

static void FormatValue(char *out, size_t cap, float value)
{
    SDL_snprintf(out, cap, "%.2f", (double)value);
}

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

static int TrackCount(const MusicPlayer *mp)
{
    if (!mp->song)
        return 0;
    const int n = Grapple_GetChipSongInfo(mp->song)->track_count;
    return n > PLAYER_MAX_TRACKS ? PLAYER_MAX_TRACKS : n;
}

static Grapple_ChipPreset TrackRole(MusicPlayer *mp, int track)
{
    Grapple_ChipMapping mapping = {0};
    if (!mp->player || !Grapple_ReadChipTrackMapping(mp->player, track, 0, &mapping))
        return GRAPPLE_CHIP_PRESET_LEAD;
    if (mapping.preset < GRAPPLE_CHIP_PRESET_FIRST || mapping.preset > GRAPPLE_CHIP_PRESET_LAST)
        return GRAPPLE_CHIP_PRESET_LEAD;
    return mapping.preset;
}

static float *FxField(Grapple_ChipEffects *fx, int field)
{
    switch (field)
    {
    case 0:
        return &fx->reverb;
    case 1:
        return &fx->chorus;
    case 2:
        return &fx->delay;
    case 3:
        return &fx->phaser;
    default:
        return &fx->flanger;
    }
}

static void ApplyTrackAudio(MusicPlayer *mp, int track)
{
    if (!mp->player || track < 0 || track >= mp->track_count)
        return;
    TrackState *st = &mp->styles[mp->style_index].tracks[track];
    Grapple_ChipTrackMix mix = {0};
    Grapple_ReadChipTrackMix(mp->player, track, &mix);
    mix.gain = st->gain;
    Grapple_SetChipTrackMix(mp->player, track, &mix);
    Grapple_ChipEffects fx = st->fx;
    if (fx.delay > 0 && fx.delay_beats <= 0)
        fx.delay_beats = 0.5;
    Grapple_SetChipTrackEffects(mp->player, track, &fx);
}

static void CaptureStyleDefaults(MusicPlayer *mp)
{
    StyleState *st = &mp->styles[mp->style_index];
    if (st->used)
        return;
    st->used = true;
    for (int i = 0; i < mp->track_count; ++i)
    {
        Grapple_ChipTrackMix mix = {0};
        Grapple_ReadChipTrackMix(mp->player, i, &mix);
        st->tracks[i].gain = mix.gain > 0 ? mix.gain : 1.0f;
        Grapple_ReadChipTrackEffects(mp->player, i, TrackRole(mp, i), &st->tracks[i].fx);
        st->tracks[i].has_fx = true;
    }
    for (int p = GRAPPLE_CHIP_PRESET_FIRST; p <= GRAPPLE_CHIP_PRESET_LAST; ++p)
    {
        if (Grapple_ReadChipRoleControls(mp->player, (Grapple_ChipPreset)p, &st->roles[p]))
            st->role_used[p] = true;
    }
}

static void RestoreStyleEdits(MusicPlayer *mp)
{
    StyleState *st = &mp->styles[mp->style_index];
    if (!st->used)
    {
        CaptureStyleDefaults(mp);
        return;
    }
    for (int i = 0; i < mp->track_count; ++i)
        ApplyTrackAudio(mp, i);
    for (int p = GRAPPLE_CHIP_PRESET_FIRST; p <= GRAPPLE_CHIP_PRESET_LAST; ++p)
        if (st->role_used[p])
            Grapple_SetChipRoleControls(mp->player, (Grapple_ChipPreset)p, &st->roles[p]);
}

static void RefreshKnobLabels(MusicPlayer *mp);
static void RebuildTracks(MusicPlayer *mp);

static void LoadKnobsFromTrack(MusicPlayer *mp)
{
    if (!mp->player)
        return;
    Grapple_ChipRoleControls c = {0};
    if (!Grapple_ReadChipRoleControls(mp->player, TrackRole(mp, mp->selected_track), &c))
        return;
    const float values[] = {c.level, c.attack_ms, c.decay_ms, c.sustain,  c.release_ms,
                            c.duty,  c.pwm,       c.vibrato,  c.cutoff_hz};
    for (int i = 0; i < 9; ++i)
        if (mp->knob_slider[i])
            Grapple_UiSetValue(mp->knob_slider[i], values[i]);
    RefreshKnobLabels(mp);
}

static void ApplyStyle(MusicPlayer *mp, int index)
{
    if (index < 0)
        index = PLAYER_STYLES - 1;
    if (index >= PLAYER_STYLES)
        index = 0;
    mp->style_index = index;
    const Grapple_ChipStyle *style = Grapple_GetChipStyle(kStyleIds[index]);
    if (!mp->player || !style || !Grapple_SetChipPlayerStyle(mp->player, style))
        SDL_snprintf(mp->status, sizeof(mp->status), "style failed: %s", SDL_GetError());
    else
    {
        RestoreStyleEdits(mp);
        Grapple_ChipStyleInfo info;
        Grapple_ReadChipStyleInfo(style, &info);
        SDL_snprintf(mp->status, sizeof(mp->status), "style %d: %s", index + 1, info.name);
        if (mp->style_radio)
            Grapple_UiSetSelected(mp->style_radio, index);
        RebuildTracks(mp);
        LoadKnobsFromTrack(mp);
    }
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
}

static void OnStyle(Grapple_UiWidget *widget, void *user)
{
    MusicPlayer *mp = user;
    ApplyStyle(mp, Grapple_UiSelected(widget));
}

static void OnVolume(Grapple_UiWidget *widget, void *user)
{
    Bind *bind = user;
    MusicPlayer *mp = bind->mp;
    const float gain = Grapple_UiValue(widget);
    mp->styles[mp->style_index].tracks[bind->track].gain = gain;
    ApplyTrackAudio(mp, bind->track);
    char text[32];
    FormatValue(text, sizeof(text), gain);
    if (mp->vol_label[bind->track])
        Grapple_UiSetText(mp->vol_label[bind->track], text);
    const Grapple_ChipTrackInfo *info =
        mp->song ? Grapple_GetChipTrackInfo(mp->song, bind->track) : NULL;
    SDL_snprintf(mp->status, sizeof(mp->status), "%s volume %s (style %s)",
                 info && info->name[0] ? info->name : "track", text, kStyleIds[mp->style_index]);
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
}

static void OnFx(Grapple_UiWidget *widget, void *user)
{
    Bind *bind = user;
    MusicPlayer *mp = bind->mp;
    TrackState *st = &mp->styles[mp->style_index].tracks[bind->track];
    *FxField(&st->fx, bind->field) = Grapple_UiValue(widget);
    if (bind->field == 2 && st->fx.delay > 0 && st->fx.delay_beats <= 0)
        st->fx.delay_beats = 0.5;
    st->has_fx = true;
    ApplyTrackAudio(mp, bind->track);
    char text[48];
    FormatValue(text, sizeof(text), Grapple_UiValue(widget));
    if (mp->fx_label[bind->track][bind->field])
        Grapple_UiSetText(mp->fx_label[bind->track][bind->field], text);
    SDL_snprintf(mp->status, sizeof(mp->status), "track %d %s %s (style %s)", bind->track + 1,
                 kFxNames[bind->field], text, kStyleIds[mp->style_index]);
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
}

static void OnSelectTrack(Grapple_UiWidget *widget, void *user)
{
    (void)widget;
    Bind *bind = user;
    bind->mp->selected_track = bind->track;
    LoadKnobsFromTrack(bind->mp);
}

static void OnKnob(Grapple_UiWidget *widget, void *user)
{
    Bind *bind = user;
    MusicPlayer *mp = bind->mp;
    Grapple_ChipPreset role = TrackRole(mp, mp->selected_track);
    Grapple_ChipRoleControls c = {0};
    if (!Grapple_ReadChipRoleControls(mp->player, role, &c))
        return;
    const float value = Grapple_UiValue(widget);
    switch (bind->field)
    {
    case 0:
        c.level = value;
        break;
    case 1:
        c.attack_ms = value;
        break;
    case 2:
        c.decay_ms = value;
        break;
    case 3:
        c.sustain = value;
        break;
    case 4:
        c.release_ms = value;
        break;
    case 5:
        c.duty = value;
        break;
    case 6:
        c.pwm = value;
        break;
    case 7:
        c.vibrato = value;
        break;
    default:
        c.cutoff_hz = value;
        break;
    }
    Grapple_SetChipRoleControls(mp->player, role, &c);
    mp->styles[mp->style_index].roles[role] = c;
    mp->styles[mp->style_index].role_used[role] = true;
    char text[48];
    FormatValue(text, sizeof(text), value);
    if (mp->knob_label[bind->field])
        Grapple_UiSetText(mp->knob_label[bind->field], text);
    SDL_snprintf(mp->status, sizeof(mp->status), "role %d %s %s (style %s)", (int)role,
                 kKnobNames[bind->field], text, kStyleIds[mp->style_index]);
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
}

static void RefreshKnobLabels(MusicPlayer *mp)
{
    for (int i = 0; i < 9; ++i)
    {
        if (!mp->knob_slider[i] || !mp->knob_label[i])
            continue;
        char text[32];
        FormatValue(text, sizeof(text), Grapple_UiValue(mp->knob_slider[i]));
        Grapple_UiSetText(mp->knob_label[i], text);
    }
}

static Grapple_UiWidget *AddLabeledSlider(Grapple_UiWidget *parent, const char *name, float min,
                                          float max, float value, Grapple_UiCallback cb, Bind *bind,
                                          Grapple_UiWidget **out_label)
{
    Grapple_UiWidget *row =
        Grapple_UiRow(parent, &(Grapple_UiStripDef){.height = GRAPPLE_UI_EM(1.6f), .spacing = 6});
    Grapple_UiLabel(row, &(Grapple_UiLabelDef){.text = name, .width = GRAPPLE_UI_PCT(0.38f)});
    Grapple_UiWidget *slider =
        Grapple_UiSlider(row, &(Grapple_UiSliderDef){.value = value,
                                                     .min = min,
                                                     .max = max,
                                                     .on_change = cb,
                                                     .user = bind,
                                                     .width = GRAPPLE_UI_STRETCH});
    char text[32];
    FormatValue(text, sizeof(text), value);
    *out_label = Grapple_UiLabel(row, &(Grapple_UiLabelDef){.text = text,
                                                            .width = GRAPPLE_UI_EM(3.2f),
                                                            .align = GRAPPLE_UI_RIGHT});
    return slider;
}

static void RebuildTracks(MusicPlayer *mp)
{
    if (!mp->tracks_row || !mp->song)
        return;
    Grapple_UiClear(mp->tracks_row);
    SDL_zero(mp->vol_label);
    SDL_zero(mp->vol_slider);
    SDL_zero(mp->fx_label);
    SDL_zero(mp->fx_slider);
    mp->track_count = TrackCount(mp);
    for (int i = 0; i < mp->track_count; ++i)
    {
        const Grapple_ChipTrackInfo *info = Grapple_GetChipTrackInfo(mp->song, i);
        TrackState *st = &mp->styles[mp->style_index].tracks[i];
        Grapple_UiWidget *col =
            Grapple_UiColumn(mp->tracks_row, &(Grapple_UiStripDef){.spacing = 4});
        Grapple_UiLabel(col,
                        &(Grapple_UiLabelDef){.text = info && info->name[0] ? info->name : "Track",
                                              .align = GRAPPLE_UI_CENTER});
        mp->vol_bind[i] = (Bind){mp, i, 0};
        mp->vol_slider[i] = AddLabeledSlider(col, "Volume", 0, 2, st->gain, OnVolume,
                                             &mp->vol_bind[i], &mp->vol_label[i]);
        for (int f = 0; f < 5; ++f)
        {
            mp->fx_bind[i][f] = (Bind){mp, i, f};
            mp->fx_slider[i][f] = AddLabeledSlider(col, kFxNames[f], 0, 1, *FxField(&st->fx, f),
                                                   OnFx, &mp->fx_bind[i][f], &mp->fx_label[i][f]);
        }
        mp->select_bind[i] = (Bind){mp, i, 0};
        Grapple_UiButton(col, &(Grapple_UiButtonDef){.text = "Edit instrument",
                                                     .on_click = OnSelectTrack,
                                                     .user = &mp->select_bind[i]});
    }
}

static bool LoadCurrent(MusicPlayer *mp);

static void OnPlay(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    if (mp->player)
    {
        Grapple_PlayChipPlayer(mp->player);
        mp->playing = true;
    }
}

static void OnPause(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    Grapple_PauseChipPlayer(mp->player);
    mp->playing = false;
}

static void OnRestart(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    if (!mp->player)
        return;
    Grapple_ResetChipPlayer(mp->player);
    Grapple_PlayChipPlayer(mp->player);
    mp->playing = true;
}

static void OnNext(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    if (mp->list.count == 0)
        return;
    mp->list.index = (mp->list.index + 1) % mp->list.count;
    LoadCurrent(mp);
}

static void OnLoop(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    if (!mp->player || !mp->song)
        return;
    mp->loop = Grapple_UiChecked(w);
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(mp->song);
    Grapple_SetChipPlayerLoop(mp->player, 0, info->duration_ticks, mp->loop);
}

static const char *RoleName(Grapple_ChipPreset preset)
{
    switch (preset)
    {
    case GRAPPLE_CHIP_PRESET_LEAD:
        return "lead";
    case GRAPPLE_CHIP_PRESET_BASS:
        return "bass";
    case GRAPPLE_CHIP_PRESET_CHORD:
        return "chord";
    case GRAPPLE_CHIP_PRESET_RING:
        return "harmony";
    case GRAPPLE_CHIP_PRESET_DRUMS:
        return "drums";
    case GRAPPLE_CHIP_PRESET_NOISE:
        return "noise";
    default:
        return "lead";
    }
}

static const char *TempDirectory(void)
{
#ifdef _WIN32
    const char *dir = SDL_getenv("TEMP");
    if (!dir || !*dir)
        dir = SDL_getenv("TMP");
#else
    const char *dir = SDL_getenv("TMPDIR");
#endif
    if (dir && *dir)
        return dir;
#ifdef _WIN32
    return ".";
#else
    return "/tmp";
#endif
}

static void JoinPath(char *out, size_t cap, const char *dir, const char *file)
{
    const size_t n = SDL_strlen(dir);
    const bool slash = n > 0 && (dir[n - 1] == '/' || dir[n - 1] == '\\');
#ifdef _WIN32
    const char sep = '\\';
#else
    const char sep = '/';
#endif
    if (slash)
        SDL_snprintf(out, cap, "%s%s", dir, file);
    else
        SDL_snprintf(out, cap, "%s%c%s", dir, sep, file);
}

typedef struct DumpBuf
{
    char *data;
    size_t size;
    size_t cap;
    bool ok;
} DumpBuf;

static void DumpAppend(DumpBuf *buf, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    if (!buf->ok)
        return;
    va_list ap;
    va_start(ap, fmt);
    const int need = SDL_vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (need < 0)
    {
        buf->ok = false;
        return;
    }
    const size_t want = buf->size + (size_t)need + 1;
    if (want > buf->cap)
    {
        size_t cap = buf->cap ? buf->cap * 2 : 4096;
        while (cap < want)
            cap *= 2;
        char *grown = (char *)SDL_realloc(buf->data, cap);
        if (!grown)
        {
            buf->ok = false;
            return;
        }
        buf->data = grown;
        buf->cap = cap;
    }
    va_start(ap, fmt);
    SDL_vsnprintf(buf->data + buf->size, buf->cap - buf->size, fmt, ap);
    va_end(ap);
    buf->size += (size_t)need;
}

static void DumpEffects(DumpBuf *buf, const Grapple_ChipEffects *fx, int indent)
{
    DumpAppend(
        buf,
        "%*s\"reverb\": %.4g,\n%*s\"chorus\": %.4g,\n%*s\"delay\": %.4g,\n%*s\"delay_beats\": "
        "%.4g,\n%*s\"delay_feedback\": %.4g,\n%*s\"phaser\": %.4g,\n%*s\"flanger\": %.4g",
        indent, "", (double)fx->reverb, indent, "", (double)fx->chorus, indent, "",
        (double)fx->delay, indent, "", fx->delay_beats, indent, "", (double)fx->delay_feedback,
        indent, "", (double)fx->phaser, indent, "", (double)fx->flanger);
}

static void DumpRole(DumpBuf *buf, const Grapple_ChipRoleControls *c, const char *name)
{
    DumpAppend(buf,
               "      \"%s\": {\n        \"level\": %.4g,\n        \"attack_ms\": %.4g,\n        "
               "\"decay_ms\": %.4g,\n        \"sustain\": %.4g,\n        \"release_ms\": %.4g,\n   "
               "     \"duty\": %.4g,\n        \"pwm\": %.4g,\n        \"vibrato\": %.4g,\n        "
               "\"cutoff_hz\": %.4g,\n        \"effects\": {\n",
               name, (double)c->level, (double)c->attack_ms, (double)c->decay_ms,
               (double)c->sustain, (double)c->release_ms, (double)c->duty, (double)c->pwm,
               (double)c->vibrato, (double)c->cutoff_hz);
    DumpEffects(buf, &c->effects, 10);
    DumpAppend(buf, "\n        }\n      }");
}

static void DumpJsonEscape(DumpBuf *buf, const char *text)
{
    for (const unsigned char *p = (const unsigned char *)(text ? text : ""); *p; ++p)
    {
        if (*p == '\\' || *p == '"')
            DumpAppend(buf, "\\%c", *p);
        else if (*p == '\n')
            DumpAppend(buf, "\\n");
        else if (*p < 32)
            DumpAppend(buf, " ");
        else
            DumpAppend(buf, "%c", *p);
    }
}

static bool WriteConfigDump(MusicPlayer *mp, char *path, size_t path_cap)
{
    DumpBuf buf = {.ok = true};
    const char *score = (mp->list.count && mp->list.paths) ? mp->list.paths[mp->list.index] : "";
    Grapple_ChipStyleInfo style = {0};
    if (mp->player)
        Grapple_GetChipPlayerStyleInfo(mp->player, &style);
    DumpAppend(&buf, "{\n  \"app\": \"chiptune-player\",\n  \"score\": \"");
    DumpJsonEscape(&buf, score);
    DumpAppend(
        &buf, "\",\n  \"loop\": %s,\n  \"current_style\": {\n    \"id\": \"%s\",\n    \"name\": \"",
        mp->loop ? "true" : "false", kStyleIds[mp->style_index]);
    DumpJsonEscape(&buf, style.name);
    DumpAppend(&buf, "\"\n  },\n  \"tracks\": [\n");
    for (int i = 0; i < mp->track_count; ++i)
    {
        const Grapple_ChipTrackInfo *info = mp->song ? Grapple_GetChipTrackInfo(mp->song, i) : NULL;
        const TrackState *st = &mp->styles[mp->style_index].tracks[i];
        const Grapple_ChipPreset role = TrackRole(mp, i);
        DumpAppend(&buf, "    {\n      \"index\": %d,\n      \"name\": \"", i);
        DumpJsonEscape(&buf, info && info->name[0] ? info->name : "Track");
        DumpAppend(&buf,
                   "\",\n      \"role\": \"%s\",\n      \"volume\": %.4g,\n      \"effects\": {\n",
                   RoleName(role), (double)st->gain);
        DumpEffects(&buf, &st->fx, 8);
        DumpAppend(&buf, "\n      }\n    }%s\n", i + 1 < mp->track_count ? "," : "");
    }
    DumpAppend(&buf, "  ],\n  \"roles\": {\n");
    bool first_role = true;
    for (int p = GRAPPLE_CHIP_PRESET_FIRST; p <= GRAPPLE_CHIP_PRESET_LAST; ++p)
    {
        Grapple_ChipRoleControls c = {0};
        if (!mp->player || !Grapple_ReadChipRoleControls(mp->player, (Grapple_ChipPreset)p, &c))
            continue;
        if (!first_role)
            DumpAppend(&buf, ",\n");
        first_role = false;
        DumpRole(&buf, &c, RoleName((Grapple_ChipPreset)p));
    }
    DumpAppend(&buf, "\n  },\n  \"remembered_styles\": {\n");
    bool first_style = true;
    for (int s = 0; s < PLAYER_STYLES; ++s)
    {
        if (!mp->styles[s].used)
            continue;
        if (!first_style)
            DumpAppend(&buf, ",\n");
        first_style = false;
        DumpAppend(&buf, "    \"%s\": {\n      \"tracks\": [\n", kStyleIds[s]);
        const int n = mp->track_count;
        for (int i = 0; i < n; ++i)
        {
            const TrackState *st = &mp->styles[s].tracks[i];
            DumpAppend(&buf, "        {\"index\": %d, \"volume\": %.4g}%s\n", i, (double)st->gain,
                       i + 1 < n ? "," : "");
        }
        DumpAppend(&buf, "      ]\n    }");
    }
    DumpAppend(&buf, "\n  }\n}\n");
    if (!buf.ok || !buf.data)
    {
        SDL_free(buf.data);
        return false;
    }

    char dir[1024];
    JoinPath(dir, sizeof(dir), TempDirectory(), "chiptune-player");
    SDL_CreateDirectory(dir);
    const Uint64 now = SDL_GetTicks();
    char file[128];
    SDL_snprintf(file, sizeof(file), "config-%s-%" SDL_PRIu64 ".json", kStyleIds[mp->style_index],
                 now);
    JoinPath(path, path_cap, dir, file);
    const bool saved = SDL_SaveFile(path, buf.data, buf.size);
    SDL_free(buf.data);
    return saved;
}

static void SetStatus(MusicPlayer *mp, const char *text)
{
    SDL_strlcpy(mp->status, text, sizeof(mp->status));
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
}

static void OnDumpConfig(Grapple_UiWidget *w, void *user)
{
    (void)w;
    MusicPlayer *mp = user;
    char path[1400];
    if (!WriteConfigDump(mp, path, sizeof(path)))
    {
        const char *err = SDL_GetError();
        char msg[512];
        SDL_snprintf(msg, sizeof(msg), "dump failed: %s", err && *err ? err : "write error");
        SetStatus(mp, msg);
        fprintf(stderr, "%s\n", msg);
        if (mp->ui)
            Grapple_UiMessage(mp->ui, "Dump config", msg);
        return;
    }
    char msg[1600];
    SDL_snprintf(msg, sizeof(msg), "Wrote config to %s", path);
    SetStatus(mp, msg);
    printf("%s\n", msg);
    fflush(stdout);
    if (mp->ui)
        Grapple_UiMessage(mp->ui, "Dump config", msg);
}

static void DrawUi(void *user)
{
    MusicPlayer *mp = user;
    Grapple_Gui *gui = Grapple_UiGui(mp->ui);
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND,
                              (SDL_Color){18, 20, 28, 255});
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_TEXT, (SDL_Color){226, 230, 240, 255});
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_BUTTON, (SDL_Color){42, 48, 64, 255});
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_BUTTON_HOVER, (SDL_Color){64, 92, 128, 255});
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_BUTTON_TEXT, (SDL_Color){236, 240, 248, 255});
    Grapple_GuiPushStyleColor(gui, GRAPPLE_GUI_COLOR_HEADER, (SDL_Color){28, 32, 44, 255});
    Grapple_UiDraw(mp->ui);
    Grapple_GuiPopStyleColor(gui, 6);
}

static bool BuildUi(MusicPlayer *mp)
{
    mp->ui = Grapple_OpenUi(Grapple_EngineRenderer(mp->engine), 15.0f);
    if (!mp->ui)
        return false;
    const Grapple_EventSink sink = Grapple_UiEventSink(mp->ui);
    Grapple_EngineSetEventSink(mp->engine, &sink);
    Grapple_EngineSetOverlay(mp->engine, DrawUi, mp);

    Grapple_UiWidget *panel = Grapple_UiPanel(
        mp->ui, &(Grapple_UiPanelDef){
                    .title = "Chiptune mixer", .fill = true, .padding = 14, .spacing = 8});
    Grapple_UiSetPanelColors(panel, (SDL_Color){18, 20, 28, 255}, (SDL_Color){226, 230, 240, 255});

    Grapple_UiLabel(panel, &(Grapple_UiLabelDef){.text = "MusicXML instrument editor"});
    mp->status_label = Grapple_UiLabel(panel, &(Grapple_UiLabelDef){.text = mp->status});
    mp->position_label = Grapple_UiLabel(panel, &(Grapple_UiLabelDef){.text = ""});

    Grapple_UiWidget *transport =
        Grapple_UiRow(panel, &(Grapple_UiStripDef){.height = GRAPPLE_UI_EM(2.2f), .spacing = 8});
    Grapple_UiButton(transport,
                     &(Grapple_UiButtonDef){.text = "Play", .on_click = OnPlay, .user = mp});
    Grapple_UiButton(transport,
                     &(Grapple_UiButtonDef){.text = "Pause", .on_click = OnPause, .user = mp});
    Grapple_UiButton(transport,
                     &(Grapple_UiButtonDef){.text = "Restart", .on_click = OnRestart, .user = mp});
    Grapple_UiButton(transport,
                     &(Grapple_UiButtonDef){.text = "Next", .on_click = OnNext, .user = mp});
    mp->loop_check = Grapple_UiCheck(
        transport, &(Grapple_UiCheckDef){
                       .text = "Loop", .checked = mp->loop, .on_change = OnLoop, .user = mp});
    Grapple_UiButton(transport, &(Grapple_UiButtonDef){
                                    .text = "Dump config", .on_click = OnDumpConfig, .user = mp});

    mp->style_radio =
        Grapple_UiSelect(panel, &(Grapple_UiSelectDef){.options = kStyleNames,
                                                       .selected = mp->style_index,
                                                       .on_change = OnStyle,
                                                       .user = mp,
                                                       .height = GRAPPLE_UI_EM(2.0f)});

    Grapple_UiWidget *body =
        Grapple_UiRow(panel, &(Grapple_UiStripDef){.height = GRAPPLE_UI_STRETCH, .spacing = 12});
    mp->tracks_row = Grapple_UiRow(body, &(Grapple_UiStripDef){.spacing = 10});
    Grapple_UiWidget *inst = Grapple_UiColumn(body, &(Grapple_UiStripDef){.spacing = 4});
    Grapple_UiLabel(inst, &(Grapple_UiLabelDef){.text = "Instrument (selected track)"});
    const float knob_min[] = {0, 0, 0, 0, 0, 0.05f, 0, 0, 0};
    const float knob_max[] = {2, 400, 800, 1, 800, 0.95f, 1, 0.02f, 8000};
    const float knob_def[] = {1, 8, 160, 0.65f, 70, 0.3f, 0, 0, 6500};
    for (int i = 0; i < 9; ++i)
    {
        mp->knob_bind[i] = (Bind){mp, 0, i};
        mp->knob_slider[i] =
            AddLabeledSlider(inst, kKnobNames[i], knob_min[i], knob_max[i], knob_def[i], OnKnob,
                             &mp->knob_bind[i], &mp->knob_label[i]);
    }
    RebuildTracks(mp);
    LoadKnobsFromTrack(mp);
    return true;
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
    mp->track_count = TrackCount(mp);
    ApplyStyle(mp, mp->style_index);
    if (!Grapple_PlayChipPlayer(mp->player))
        return false;
    mp->playing = true;
    const Grapple_ChipSongInfo *info = Grapple_GetChipSongInfo(mp->song);
    SDL_snprintf(mp->status, sizeof(mp->status), "%s  (%.1fs, %d tracks)", path,
                 info->duration_seconds, info->track_count);
    if (mp->status_label)
        Grapple_UiSetText(mp->status_label, mp->status);
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
            OnPause(NULL, mp);
        else
            OnPlay(NULL, mp);
    }
    if (mp->song && Grapple_KeyPressed(engine, SDL_SCANCODE_LEFT))
        SeekBy(mp, -(Sint64)Grapple_GetChipSongInfo(mp->song)->ticks_per_quarter * 4);
    if (mp->song && Grapple_KeyPressed(engine, SDL_SCANCODE_RIGHT))
        SeekBy(mp, (Sint64)Grapple_GetChipSongInfo(mp->song)->ticks_per_quarter * 4);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_HOME) || Grapple_KeyPressed(engine, SDL_SCANCODE_R))
        OnRestart(NULL, mp);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_N))
        OnNext(NULL, mp);
    if (Grapple_KeyPressed(engine, SDL_SCANCODE_L) && mp->loop_check)
    {
        Grapple_UiSetChecked(mp->loop_check, !mp->loop);
        Grapple_UiInvoke(mp->loop_check);
    }
    const SDL_Scancode numbers[] = {SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                                    SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6};
    for (int i = 0; i < PLAYER_STYLES; ++i)
        if (Grapple_KeyPressed(engine, numbers[i]))
            ApplyStyle(mp, i);
    if (mp->player && mp->position_label)
    {
        Grapple_ChipPosition pos = {0};
        Grapple_ReadChipPlayerPosition(mp->player, &pos);
        char line[160];
        SDL_snprintf(line, sizeof(line), "%.1fs  beat %.1f  %s  loop %s", pos.seconds, pos.beat,
                     mp->playing ? "playing" : "paused", mp->loop ? "on" : "off");
        Grapple_UiSetText(mp->position_label, line);
    }
}

static void Render(void *user, float alpha)
{
    MusicPlayer *mp = user;
    (void)alpha;
    SDL_Renderer *renderer = Grapple_EngineRenderer(mp->engine);
    SDL_SetRenderDrawColor(renderer, 12, 14, 20, 255);
    SDL_RenderClear(renderer);
}

static bool Load(void *user)
{
    MusicPlayer *mp = user;
    return BuildUi(mp);
}

static void Unload(void *user)
{
    MusicPlayer *mp = user;
    Grapple_EngineSetEventSink(mp->engine, NULL);
    Grapple_EngineSetOverlay(mp->engine, NULL, NULL);
    Grapple_DestroyUi(mp->ui);
    mp->ui = NULL;
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
    config.title = "Chiptune mixer";
    config.design_width = 1920;
    config.design_height = 1080;
    config.window_width = 1920;
    config.window_height = 1080;
    config.no_auto_mount = true;
    config.headless = SDL_getenv("GRAPPLE_HEADLESS") != NULL;
    mp.engine = Grapple_CreateEngine(&config);
    bool ok = mp.engine && LoadCurrent(&mp);
    if (ok)
    {
        Grapple_GameHooks hooks = {0};
        hooks.load = config.headless ? NULL : Load;
        hooks.update = Update;
        hooks.render = Render;
        hooks.unload = config.headless ? NULL : Unload;
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
