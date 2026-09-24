/* Original Grapple code (zlib). Style catalog, C64 recipes, and player apply. */
#include "chip_style.h"

#include "chip_effects.h"
#include "chip_player.h"

#include <SDL3/SDL.h>

static Grapple_ChipStyle g_c64;
static Grapple_ChipStyle *g_catalog[64];
static int g_catalog_count;
static bool g_catalog_ready;
static char g_search_paths[8][512];
static int g_search_count;

static void DryEffects(Grapple_ChipEffects *e)
{
    SDL_zero(*e);
}

static ChipRoleRecipe BasePitched(void)
{
    ChipRoleRecipe r;
    SDL_zero(r);
    r.voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
    r.duty = 0.30f;
    r.pwm = 0.12f;
    r.lfo_hz = 5.2f;
    r.attack_ms = 4.0f;
    r.decay_ms = 160.0f;
    r.sustain = 0.65f;
    r.release_ms = 70.0f;
    r.filter = CHIP_FILTER_SVF_LOW;
    r.cutoff_hz = 6500.0f;
    r.level = 0.23f;
    r.share = GRAPPLE_CHIP_PRESET_AUTO;
    return r;
}

static void InitC64Style(void)
{
    if (g_c64.info.id[0])
        return;
    SDL_zero(g_c64);
    SDL_strlcpy(g_c64.info.id, "c64", sizeof(g_c64.info.id));
    SDL_strlcpy(g_c64.info.name, "C64 (SID-inspired)", sizeof(g_c64.info.name));
    SDL_strlcpy(g_c64.info.description,
                "Original Grapple synthesis inspired by typical use of this era's hardware. "
                "Not a recreation of any copyrighted soundtrack.",
                sizeof(g_c64.info.description));
    g_c64.info.voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
    g_c64.info.builtin = true;
    g_c64.builtin = true;
    g_c64.interned = true;
    g_c64.gain = 1.0f;

    ChipRoleRecipe lead = BasePitched();
    lead.mix_pulse = 1.0f;
    lead.vibrato = 0.0025f;
    lead.vibrato_delay_ms = 120.0f;
    Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_LEAD, &lead.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_LEAD] = lead;

    ChipRoleRecipe bass = BasePitched();
    bass.mix_pulse = 0.65f;
    bass.mix_saw = 0.35f;
    bass.duty = 0.5f;
    bass.pwm = 0.0f;
    bass.vibrato = 0.0f;
    bass.decay_ms = 120.0f;
    bass.sustain = 0.5f;
    bass.cutoff_hz = 1400.0f;
    bass.filter_env = 1.0f;
    bass.level = 0.34f;
    Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_BASS, &bass.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_BASS] = bass;

    ChipRoleRecipe chord = BasePitched();
    chord.mix_pulse = 1.0f;
    chord.attack_ms = 12.0f;
    chord.decay_ms = 280.0f;
    chord.sustain = 0.55f;
    chord.release_ms = 220.0f;
    chord.cutoff_hz = 3800.0f;
    chord.level = 0.14f;
    Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_CHORD, &chord.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_CHORD] = chord;

    ChipRoleRecipe ring = chord;
    ring.mix_pulse = 0.0f;
    ring.mix_triangle = 1.0f;
    ring.triangle = CHIP_TRIANGLE_ANALOG;
    ring.ring_ratio = 2.0f;
    ring.cutoff_hz = 7000.0f;
    ring.level = 0.24f;
    Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_RING, &ring.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_RING] = ring;

    ChipRoleRecipe drums;
    SDL_zero(drums);
    drums.voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
    drums.drums = CHIP_DRUM_GM_ANALOG;
    drums.attack_ms = 1.0f;
    drums.decay_ms = 200.0f;
    drums.sustain = 0.0f;
    drums.release_ms = 70.0f;
    drums.level = 0.45f;
    drums.filter = CHIP_FILTER_SVF_LOW;
    drums.cutoff_hz = 8000.0f;
    drums.lfo_hz = 5.2f;
    drums.duty = 0.5f;
    drums.share = GRAPPLE_CHIP_PRESET_AUTO;
    DryEffects(&drums.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_DRUMS] = drums;

    ChipRoleRecipe noise;
    SDL_zero(noise);
    noise.voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
    noise.mix_noise = 1.0f;
    noise.attack_ms = 1.0f;
    noise.decay_ms = 80.0f;
    noise.sustain = 0.0f;
    noise.release_ms = 50.0f;
    noise.level = 0.20f;
    noise.filter = CHIP_FILTER_OFF;
    noise.lfo_hz = 5.2f;
    noise.duty = 0.5f;
    noise.share = GRAPPLE_CHIP_PRESET_AUTO;
    DryEffects(&noise.effects);
    g_c64.recipes[GRAPPLE_CHIP_PRESET_NOISE] = noise;
}

const ChipRoleRecipe *Chip_C64Recipe(Grapple_ChipPreset preset)
{
    InitC64Style();
    if (preset < GRAPPLE_CHIP_PRESET_FIRST || preset > GRAPPLE_CHIP_PRESET_LAST)
        preset = GRAPPLE_CHIP_PRESET_LEAD;
    return &g_c64.recipes[preset];
}

const Grapple_ChipStyle *Chip_C64Style(void)
{
    InitC64Style();
    return &g_c64;
}

static bool AddCatalog(Grapple_ChipStyle *style)
{
    if (!style || g_catalog_count == (int)SDL_arraysize(g_catalog))
        return SDL_SetError("chiptune: style catalog is full");
    for (int i = 0; i < g_catalog_count; ++i)
    {
        if (SDL_strcmp(g_catalog[i]->info.id, style->info.id) == 0)
        {
            if (!g_catalog[i]->builtin)
                Grapple_DestroyChipStyle(g_catalog[i]);
            g_catalog[i] = style;
            style->interned = true;
            return true;
        }
    }
    style->interned = true;
    g_catalog[g_catalog_count++] = style;
    return true;
}

static void ScanSearchPath(const char *dir)
{
    int count = 0;
    char **files = SDL_GlobDirectory(dir, "*.json", SDL_GLOB_CASEINSENSITIVE, &count);
    if (!files)
        return;
    for (int i = 0; i < count; ++i)
    {
        char path[1024];
        SDL_snprintf(path, sizeof(path), "%s/%s", dir, files[i]);
        Grapple_ChipStyle *style = Grapple_LoadChipStyle(path);
        if (style)
            AddCatalog(style);
    }
    SDL_free(files);
}

void Chip_EnsureStyleCatalog(void)
{
    InitC64Style();
    if (!g_catalog_ready)
    {
        g_catalog_ready = true;
        AddCatalog(&g_c64);
        const char *env = SDL_getenv("GRAPPLE_CHIP_STYLE_PATH");
        if (env && *env)
            Grapple_AddChipStyleSearchPath(env);
        Chip_RegisterBuiltinStyles();
        for (int i = 0; i < g_search_count; ++i)
            ScanSearchPath(g_search_paths[i]);
    }
}

bool Chip_CopyStyleRecipes(const Grapple_ChipStyle *style, ChipRoleRecipe *out)
{
    if (!style || !out)
        return false;
    SDL_memcpy(out, style->recipes, sizeof(style->recipes));
    return true;
}

bool Chip_ApplyStyleEffects(Grapple_ChipPlayer *p, const Grapple_ChipStyle *style)
{
    if (!p || !style)
        return false;
    for (int bus = GRAPPLE_CHIP_PRESET_FIRST; bus <= GRAPPLE_CHIP_PRESET_LAST; ++bus)
    {
        p->effects[bus].settings = style->recipes[bus].effects;
        Chip_EffectsClear(&p->effects[bus]);
        p->effect_used[bus] = false;
    }
    return true;
}

Grapple_ChipStyle *Grapple_LoadChipStyle(const char *path)
{
    if (!path || !*path)
    {
        SDL_SetError("chiptune: missing style path");
        return NULL;
    }
    size_t size = 0;
    char *bytes = (char *)SDL_LoadFile(path, &size);
    if (!bytes)
        return NULL;
    Grapple_ChipStyle *style = Chip_ParseChipStyleJson(bytes, size, false);
    SDL_free(bytes);
    return style;
}

Grapple_ChipStyle *Grapple_LoadChipStyleMemory(const void *json, size_t size)
{
    if (!json || size == 0)
    {
        SDL_SetError("chiptune: empty style document");
        return NULL;
    }
    return Chip_ParseChipStyleJson((const char *)json, size, false);
}

const Grapple_ChipStyle *Grapple_GetChipStyle(const char *id)
{
    Chip_EnsureStyleCatalog();
    if (!id || !*id)
        return NULL;
    for (int i = 0; i < g_catalog_count; ++i)
        if (SDL_strcmp(g_catalog[i]->info.id, id) == 0)
            return g_catalog[i];
    return NULL;
}

bool Grapple_RegisterChipStyle(Grapple_ChipStyle *style)
{
    Chip_EnsureStyleCatalog();
    if (!style || !style->info.id[0])
        return SDL_SetError("chiptune: invalid style");
    return AddCatalog(style);
}

void Grapple_DestroyChipStyle(Grapple_ChipStyle *style)
{
    if (!style || style->builtin || style->interned)
        return;
    SDL_free(style);
}

int Grapple_GetChipStyleCount(void)
{
    Chip_EnsureStyleCatalog();
    return g_catalog_count;
}

bool Grapple_ReadChipStyleInfo(const Grapple_ChipStyle *style, Grapple_ChipStyleInfo *info)
{
    if (!style || !info)
        return SDL_SetError("chiptune: invalid style info output");
    *info = style->info;
    return true;
}

bool Grapple_ReadChipStyleInfoAt(int index, Grapple_ChipStyleInfo *info)
{
    Chip_EnsureStyleCatalog();
    if (!info || index < 0 || index >= g_catalog_count)
        return SDL_SetError("chiptune: invalid style index");
    *info = g_catalog[index]->info;
    return true;
}

bool Grapple_AddChipStyleSearchPath(const char *path)
{
    if (!path || !*path)
        return SDL_SetError("chiptune: empty style search path");
    if (g_search_count == (int)SDL_arraysize(g_search_paths))
        return SDL_SetError("chiptune: style search path limit exceeded");
    SDL_strlcpy(g_search_paths[g_search_count++], path, sizeof(g_search_paths[0]));
    if (g_catalog_ready)
        ScanSearchPath(path);
    return true;
}

bool Grapple_GetChipPlayerStyleInfo(Grapple_ChipPlayer *p, Grapple_ChipStyleInfo *info)
{
    if (!p || !info)
        return SDL_SetError("chiptune: invalid style info output");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    *info = p->style_info;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_SetChipPlayerStyle(Grapple_ChipPlayer *p, const Grapple_ChipStyle *style)
{
    Chip_EnsureStyleCatalog();
    if (!p)
        return SDL_SetError("chiptune: NULL player");
    if (!style)
        style = Chip_C64Style();
    if (!SDL_LockAudioStream(p->stream))
        return false;
    ChipRoleRecipe prior_recipes[GRAPPLE_CHIP_PRESET_LAST + 1];
    Grapple_ChipStyleInfo prior_info = p->style_info;
    ChipStealMode prior_steal = p->steal;
    float prior_style_gain = p->style_gain;
    int prior_polyphony = p->style_polyphony;
    SDL_memcpy(prior_recipes, p->recipes, sizeof(prior_recipes));
    SDL_memcpy(p->recipes, style->recipes, sizeof(p->recipes));
    p->style_info = style->info;
    p->steal = style->steal;
    p->style_gain = style->gain > 0 ? style->gain : 1.0f;
    p->style_polyphony = style->info.polyphony;
    if (!Chip_RefreshCheckpoint(p))
    {
        SDL_memcpy(p->recipes, prior_recipes, sizeof(prior_recipes));
        p->style_info = prior_info;
        p->steal = prior_steal;
        p->style_gain = prior_style_gain;
        p->style_polyphony = prior_polyphony;
        SDL_UnlockAudioStream(p->stream);
        return false;
    }
    Chip_ApplyStyleEffects(p, style);
    for (int i = 0; i < p->voice_count; ++i)
        if (p->voices[i].active)
            Chip_VoiceRelease(&p->voices[i], p->sample_rate);
    SDL_UnlockAudioStream(p->stream);
    return true;
}
