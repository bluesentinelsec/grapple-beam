/* Original Grapple code (zlib). Schema-1 chiptune style JSON. */
#include "chip_effects.h"
#include "chip_style.h"

#include <SDL3/SDL.h>
#include <cJSON.h>

#define CHIP_STYLE_JSON_MAX 65536
#define CHIP_STYLE_JSON_DEPTH 32

static bool FailField(const char *field, const char *why)
{
    return SDL_SetError("chiptune style: %s: %s", field, why);
}

static bool RequireObject(const cJSON *node, const char *field)
{
    if (!cJSON_IsObject(node))
        return FailField(field, "expected object");
    (void)CHIP_STYLE_JSON_DEPTH;
    return true;
}

static const cJSON *Get(const cJSON *obj, const char *key)
{
    return cJSON_GetObjectItemCaseSensitive(obj, key);
}

static bool CopyString(const cJSON *obj, const char *key, char *out, size_t cap, bool required)
{
    const cJSON *n = Get(obj, key);
    if (!n)
    {
        if (required)
            return FailField(key, "missing");
        out[0] = 0;
        return true;
    }
    if (!cJSON_IsString(n) || !n->valuestring)
        return FailField(key, "expected string");
    SDL_strlcpy(out, n->valuestring, cap);
    return true;
}

static bool ReadNumber(const cJSON *obj, const char *key, float *out, float lo, float hi,
                       bool present_ok)
{
    const cJSON *n = Get(obj, key);
    if (!n)
        return present_ok;
    if (!cJSON_IsNumber(n))
        return FailField(key, "expected number");
    const float v = (float)n->valuedouble;
    if (v < lo || v > hi)
        return FailField(key, "out of range");
    *out = v;
    return true;
}

static bool ReadInt(const cJSON *obj, const char *key, int *out, int lo, int hi)
{
    const cJSON *n = Get(obj, key);
    if (!n)
        return true;
    if (!cJSON_IsNumber(n))
        return FailField(key, "expected number");
    const int v = (int)n->valuedouble;
    if (v < lo || v > hi)
        return FailField(key, "out of range");
    *out = v;
    return true;
}

static bool ReadBool(const cJSON *obj, const char *key, bool *out)
{
    const cJSON *n = Get(obj, key);
    if (!n)
        return true;
    if (!cJSON_IsBool(n))
        return FailField(key, "expected bool");
    *out = cJSON_IsTrue(n);
    return true;
}

static bool KnownRoleKey(const char *key)
{
    return SDL_strcmp(key, "lead") == 0 || SDL_strcmp(key, "harmony") == 0 ||
           SDL_strcmp(key, "bass") == 0 || SDL_strcmp(key, "drums") == 0 ||
           SDL_strcmp(key, "noise") == 0 || SDL_strcmp(key, "chord") == 0;
}

static Grapple_ChipPreset RoleFromKey(const char *key)
{
    if (SDL_strcmp(key, "lead") == 0)
        return GRAPPLE_CHIP_PRESET_LEAD;
    if (SDL_strcmp(key, "harmony") == 0)
        return GRAPPLE_CHIP_PRESET_HARMONY;
    if (SDL_strcmp(key, "bass") == 0)
        return GRAPPLE_CHIP_PRESET_BASS;
    if (SDL_strcmp(key, "drums") == 0)
        return GRAPPLE_CHIP_PRESET_DRUMS;
    if (SDL_strcmp(key, "noise") == 0)
        return GRAPPLE_CHIP_PRESET_NOISE;
    return GRAPPLE_CHIP_PRESET_CHORD;
}

static bool ApplyOscillatorShorthand(ChipRoleRecipe *r, const char *name)
{
    r->mix_pulse = r->mix_saw = r->mix_triangle = r->mix_noise = r->mix_sine = 0;
    r->ring_ratio = 0;
    r->drums = CHIP_DRUM_NONE;
    r->voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
    if (SDL_strcmp(name, "pulse") == 0 || SDL_strcmp(name, "pulse50") == 0)
    {
        r->mix_pulse = 1;
        r->duty = 0.5f;
    }
    else if (SDL_strcmp(name, "pulse12") == 0)
    {
        r->mix_pulse = 1;
        r->duty = 0.125f;
    }
    else if (SDL_strcmp(name, "pulse25") == 0)
    {
        r->mix_pulse = 1;
        r->duty = 0.25f;
    }
    else if (SDL_strcmp(name, "saw") == 0)
        r->mix_saw = 1;
    else if (SDL_strcmp(name, "triangle") == 0)
        r->mix_triangle = 1;
    else if (SDL_strcmp(name, "noise") == 0)
        r->mix_noise = 1;
    else if (SDL_strcmp(name, "sine") == 0)
        r->mix_sine = 1;
    else if (SDL_strcmp(name, "fm") == 0)
    {
        r->voice = GRAPPLE_CHIP_VOICE_FM;
        r->mix_sine = 1;
        r->fm_index = 2.0f;
    }
    else
        return FailField("oscillator", "unknown oscillator name");
    return true;
}

static bool ParseEffects(const cJSON *node, Grapple_ChipEffects *e, Grapple_ChipPreset role)
{
    if (!node)
        return true;
    if (cJSON_IsString(node) && node->valuestring)
    {
        if (SDL_strcmp(node->valuestring, "dry") == 0)
        {
            SDL_zero(*e);
            return true;
        }
        if (SDL_strcmp(node->valuestring, "c64") == 0)
            return Grapple_GetChipPresetEffects(role, e);
        return FailField("effects", "expected dry, c64, or object");
    }
    if (!cJSON_IsObject(node))
        return FailField("effects", "expected object");
    SDL_zero(*e);
    if (!ReadNumber(node, "chorus", &e->chorus, 0, 1, true) ||
        !ReadNumber(node, "delay", &e->delay, 0, 1, true) ||
        !ReadNumber(node, "delay_beats", &e->delay_beats, 0, 4, true) ||
        !ReadNumber(node, "delay_feedback", &e->delay_feedback, 0, 0.85f, true) ||
        !ReadNumber(node, "reverb", &e->reverb, 0, 1, true) ||
        !ReadNumber(node, "motion", &e->motion, 0, 1, true) ||
        !ReadNumber(node, "pulse_depth", &e->pulse_depth, 0, 1, true) ||
        !ReadNumber(node, "phaser", &e->phaser, 0, 1, true) ||
        !ReadNumber(node, "flanger", &e->flanger, 0, 1, true))
        return false;
    const cJSON *pb = Get(node, "pulse_beats");
    if (pb)
    {
        if (!cJSON_IsNumber(pb))
            return FailField("pulse_beats", "expected number");
        e->pulse_beats = pb->valuedouble;
    }
    return Chip_EffectsValid(e) ? true : FailField("effects", "invalid range");
}

static bool ParseRole(const cJSON *obj, Grapple_ChipPreset role, ChipRoleRecipe *r)
{
    *r = *Chip_C64Recipe(role);
    SDL_zero(r->effects);
    if (!RequireObject(obj, "role"))
        return false;
    const cJSON *osc = Get(obj, "oscillator");
    const cJSON *wave = Get(obj, "wave");
    const cJSON *mix = Get(obj, "mix");
    if (osc || wave || mix)
    {
        const char *name = NULL;
        if (osc && cJSON_IsString(osc))
            name = osc->valuestring;
        else if (wave && cJSON_IsString(wave))
            name = wave->valuestring;
        if (name && !ApplyOscillatorShorthand(r, name))
            return false;
        else if (mix)
        {
            if (!cJSON_IsObject(mix))
                return FailField("mix", "expected object");
            r->mix_pulse = r->mix_saw = r->mix_triangle = r->mix_noise = r->mix_sine = 0;
            r->ring_ratio = 0;
            r->drums = CHIP_DRUM_NONE;
            if (!ReadNumber(mix, "pulse", &r->mix_pulse, 0, 1, true) ||
                !ReadNumber(mix, "saw", &r->mix_saw, 0, 1, true) ||
                !ReadNumber(mix, "triangle", &r->mix_triangle, 0, 1, true) ||
                !ReadNumber(mix, "noise", &r->mix_noise, 0, 1, true) ||
                !ReadNumber(mix, "sine", &r->mix_sine, 0, 1, true))
                return false;
        }
        else if (!name)
            return FailField("oscillator", "expected string");
    }
    char voice[16] = {0};
    if (!CopyString(obj, "voice", voice, sizeof(voice), false))
        return false;
    if (voice[0])
    {
        if (SDL_strcmp(voice, "oscillator") == 0)
            r->voice = GRAPPLE_CHIP_VOICE_OSCILLATOR;
        else if (SDL_strcmp(voice, "fm") == 0)
            r->voice = GRAPPLE_CHIP_VOICE_FM;
        else if (SDL_strcmp(voice, "sample") == 0)
            return FailField("voice", "sample engine is not available");
        else
            return FailField("voice", "unknown voice kind");
    }
    char triangle[16] = {0};
    if (!CopyString(obj, "triangle", triangle, sizeof(triangle), false))
        return false;
    if (triangle[0])
        r->triangle = SDL_strcmp(triangle, "nes") == 0 ? CHIP_TRIANGLE_NES : CHIP_TRIANGLE_ANALOG;
    char filter[16] = {0};
    if (!CopyString(obj, "filter", filter, sizeof(filter), false))
        return false;
    if (filter[0])
    {
        if (SDL_strcmp(filter, "off") == 0)
            r->filter = CHIP_FILTER_OFF;
        else if (SDL_strcmp(filter, "svf_high") == 0)
            r->filter = CHIP_FILTER_SVF_HIGH;
        else
            r->filter = CHIP_FILTER_SVF_LOW;
    }
    char drums[16] = {0};
    if (!CopyString(obj, "drums", drums, sizeof(drums), false))
        return false;
    if (drums[0])
        r->drums = SDL_strcmp(drums, "gm-analog") == 0 ? CHIP_DRUM_GM_ANALOG : CHIP_DRUM_NONE;
    char share[16] = {0};
    if (!CopyString(obj, "share", share, sizeof(share), false))
        return false;
    if (share[0])
        r->share = RoleFromKey(share);
    if (!ReadNumber(obj, "duty", &r->duty, 0.05f, 0.95f, true) ||
        !ReadNumber(obj, "pwm", &r->pwm, 0, 1, true) ||
        !ReadNumber(obj, "vibrato", &r->vibrato, 0, 1, true) ||
        !ReadNumber(obj, "vibrato_delay_ms", &r->vibrato_delay_ms, 0, 2000, true) ||
        !ReadNumber(obj, "lfo_hz", &r->lfo_hz, 0, 32, true) ||
        !ReadNumber(obj, "attack_ms", &r->attack_ms, 0, 2000, true) ||
        !ReadNumber(obj, "decay_ms", &r->decay_ms, 0, 4000, true) ||
        !ReadNumber(obj, "sustain", &r->sustain, 0, 1, true) ||
        !ReadNumber(obj, "release_ms", &r->release_ms, 0, 4000, true) ||
        !ReadNumber(obj, "cutoff_hz", &r->cutoff_hz, 0, 20000, true) ||
        !ReadNumber(obj, "filter_env", &r->filter_env, 0, 1, true) ||
        !ReadNumber(obj, "ring_ratio", &r->ring_ratio, 0, 16, true) ||
        !ReadNumber(obj, "fm_index", &r->fm_index, 0, 16, true) ||
        !ReadNumber(obj, "level", &r->level, 0, 2, true) ||
        !ReadInt(obj, "channels", &r->channels, 0, 64) ||
        !ReadBool(obj, "noise_metallic", &r->noise_metallic))
        return false;
    const cJSON *effects = Get(obj, "effects");
    if (effects)
        return ParseEffects(effects, &r->effects, role);
    return true;
}

static bool UnknownKeys(const cJSON *obj, const char *const *allowed, const char *field)
{
    for (const cJSON *c = obj->child; c; c = c->next)
    {
        bool ok = false;
        for (int i = 0; allowed[i]; ++i)
            if (SDL_strcmp(c->string, allowed[i]) == 0)
                ok = true;
        if (!ok)
            return FailField(field, c->string);
    }
    return true;
}

Grapple_ChipStyle *Chip_ParseChipStyleJson(const char *json, size_t size, bool builtin)
{
    if (size == 0 || size > CHIP_STYLE_JSON_MAX)
    {
        SDL_SetError("chiptune style: document must be 1..64 KiB");
        return NULL;
    }
    char *copy = (char *)SDL_malloc(size + 1);
    if (!copy)
        return NULL;
    SDL_memcpy(copy, json, size);
    copy[size] = 0;
    cJSON *root = cJSON_ParseWithLength(copy, size);
    SDL_free(copy);
    if (!root)
    {
        SDL_SetError("chiptune style: JSON parse failed");
        return NULL;
    }
    Grapple_ChipStyle *style = SDL_calloc(1, sizeof(*style));
    bool ok = style && RequireObject(root, "root");
    static const char *root_keys[] = {"schema",        "id",        "name",    "description",
                                      "voice",         "polyphony", "steal",   "gain",
                                      "approximation", "roles",     "samples", NULL};
    ok = ok && UnknownKeys(root, root_keys, "root");
    const cJSON *schema = Get(root, "schema");
    if (ok && (!schema || !cJSON_IsNumber(schema) || schema->valuedouble != 1))
        ok = FailField("schema", "must be 1");
    ok = ok && CopyString(root, "id", style->info.id, sizeof(style->info.id), true);
    ok = ok && CopyString(root, "name", style->info.name, sizeof(style->info.name), true);
    ok = ok && CopyString(root, "description", style->info.description,
                          sizeof(style->info.description), false);
    if (ok && !style->info.description[0])
        SDL_strlcpy(style->info.description,
                    "Original Grapple synthesis inspired by typical use of this era's hardware.",
                    sizeof(style->info.description));
    ok = ok && ReadInt(root, "polyphony", &style->info.polyphony, 0, 1024);
    ok = ok && ReadNumber(root, "gain", &style->gain, 0, 2, true);
    if (ok && style->gain == 0)
        style->gain = 1;
    ok = ok && ReadBool(root, "approximation", &style->info.approximation);
    char steal[32] = {0};
    ok = ok && CopyString(root, "steal", steal, sizeof(steal), false);
    if (ok && SDL_strcmp(steal, "oldest") == 0)
        style->steal = CHIP_STEAL_OLDEST;
    else if (ok && SDL_strcmp(steal, "role-then-global") == 0)
        style->steal = CHIP_STEAL_ROLE_THEN_GLOBAL;
    char samples[256] = {0};
    ok = ok && CopyString(root, "samples", samples, sizeof(samples), false);
    if (ok && samples[0])
        ok = FailField("samples", "sample packs are not available");
    char voice[16] = {0};
    ok = ok && CopyString(root, "voice", voice, sizeof(voice), false);
    if (ok && SDL_strcmp(voice, "fm") == 0)
        style->info.voice = GRAPPLE_CHIP_VOICE_FM;
    else if (ok && SDL_strcmp(voice, "sample") == 0)
        ok = FailField("voice", "sample engine is not available");
    const cJSON *roles = Get(root, "roles");
    ok = ok && roles && RequireObject(roles, "roles");
    if (ok)
    {
        if (!Get(roles, "lead") || !Get(roles, "harmony") || !Get(roles, "bass") ||
            !Get(roles, "drums"))
            ok = FailField("roles", "lead, harmony, bass, and drums are required");
        for (const cJSON *c = roles->child; ok && c; c = c->next)
        {
            if (!KnownRoleKey(c->string))
                ok = FailField("roles", "unknown role key");
            else
                ok = ParseRole(c, RoleFromKey(c->string), &style->recipes[RoleFromKey(c->string)]);
        }
        if (ok && !Get(roles, "noise"))
            style->recipes[GRAPPLE_CHIP_PRESET_NOISE] = *Chip_C64Recipe(GRAPPLE_CHIP_PRESET_NOISE);
        if (ok && !Get(roles, "chord"))
        {
            style->recipes[GRAPPLE_CHIP_PRESET_CHORD] = style->recipes[GRAPPLE_CHIP_PRESET_HARMONY];
            style->recipes[GRAPPLE_CHIP_PRESET_CHORD].ring_ratio = 0;
            style->recipes[GRAPPLE_CHIP_PRESET_CHORD].channels = 0;
        }
    }
    style->info.builtin = builtin;
    style->builtin = builtin;
    if (!ok)
    {
        SDL_free(style);
        style = NULL;
    }
    cJSON_Delete(root);
    return style;
}
