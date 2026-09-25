/*
 * grapple_platformer.c — the level: a tile grid, the dedicated objects that
 * fill it, pixel solids, the camera, the scene that runs it all, and the
 * action map. Original grapple-beam code (zlib).
 *
 * The player's movement lives in grapple_platformer_player.c and the Tiled
 * loader in grapple_platformer_tiled.c.
 */
#include "platformer_internal.h"

#include <grapple/engine_input.h>
#include <grapple/engine_render.h>
#include <grapple/engine_scene.h>
#include <math.h>

/* A cell edge is "touched" within this many pixels. Sweeps snap to exact
   tile edges, so the only floating error left is the one from adding a
   velocity and subtracting it again; a hundredth of a pixel is far above
   that and far below anything the eye sees. */
#define EDGE_EPSILON 0.01f

/* --- scene registry -------------------------------------------------------
 *
 * A scene definition carries callbacks and a state size but no user
 * pointer — by design, so a script can build one — which leaves the scene's
 * load hook with no way to be handed the level it is running. So attached
 * levels register here under the scene's name, and load looks itself up.
 * Main-thread only, like the engine. */

#define REGISTRY_MAX 16

static Grapple_Platformer *g_attached[REGISTRY_MAX];
static unsigned g_scene_serial;

static Grapple_Platformer *FindAttached(Grapple_Engine *engine, const char *name)
{
    for (int i = 0; i < REGISTRY_MAX; ++i)
    {
        Grapple_Platformer *level = g_attached[i];
        if (level != NULL && level->engine == engine && SDL_strcmp(level->scene_name, name) == 0)
        {
            return level;
        }
    }
    return NULL;
}

static bool Register(Grapple_Platformer *level)
{
    for (int i = 0; i < REGISTRY_MAX; ++i)
    {
        if (g_attached[i] == NULL)
        {
            g_attached[i] = level;
            return true;
        }
    }
    return SDL_SetError("too many attached platformer levels (max %d)", REGISTRY_MAX);
}

static void Unregister(Grapple_Platformer *level)
{
    for (int i = 0; i < REGISTRY_MAX; ++i)
    {
        if (g_attached[i] == level)
        {
            g_attached[i] = NULL;
        }
    }
}

/* --- the scene ------------------------------------------------------------ */

static Grapple_Platformer *SceneLevel(Grapple_Scene *scene)
{
    Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_SceneState(scene);
    return (slot != NULL) ? *slot : NULL;
}

static bool SceneLoad(Grapple_Scene *scene)
{
    Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_SceneState(scene);
    Grapple_Platformer *level = FindAttached(Grapple_SceneEngine(scene), Grapple_SceneName(scene));
    if (slot == NULL || level == NULL)
    {
        return SDL_SetError("platformer scene '%s' has no level", Grapple_SceneName(scene));
    }
    *slot = level;
    return true;
}

static void SceneStep(Grapple_Scene *scene, float step)
{
    Grapple_Platformer *level = SceneLevel(scene);
    if (level != NULL)
    {
        Grapple_PlatformerStep(level, step);
    }
}

static void SceneUpdate(Grapple_Scene *scene, float dt)
{
    Grapple_Platformer *level = SceneLevel(scene);
    if (level != NULL)
    {
        Grapple_PlatformerUpdate(level, dt);
    }
}

static void SceneRender(Grapple_Scene *scene, float alpha)
{
    Grapple_Platformer *level = SceneLevel(scene);
    if (level != NULL)
    {
        Grapple_PlatformerRender(level, alpha);
    }
}

static void SceneUnload(Grapple_Scene *scene)
{
    Grapple_Platformer *level = SceneLevel(scene);
    if (level != NULL)
    {
        level->attached = false;
        Unregister(level);
    }
}

/* --- defaults ------------------------------------------------------------- */

static void DefaultColors(Grapple_Platformer *level)
{
    /* A daytime sky, brown ground, a lighter ledge: enough contrast that a
       level reads with no art at all. */
    level->background = (SDL_FColor){0.36f, 0.58f, 0.99f, 1.0f};
    level->tile_colors[GRAPPLE_PLATFORMER_EMPTY] = (SDL_FColor){0.0f, 0.0f, 0.0f, 0.0f};
    level->tile_colors[GRAPPLE_PLATFORMER_SOLID] = (SDL_FColor){0.78f, 0.45f, 0.20f, 1.0f};
    level->tile_colors[GRAPPLE_PLATFORMER_ONE_WAY] = (SDL_FColor){0.55f, 0.80f, 0.35f, 1.0f};
}

static void DefaultActions(Grapple_Platformer *level)
{
    Grapple_ActionMap *map = level->actions;
    Grapple_Binding binding;

    Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_A, -1);
    Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_D, +1);
    Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_LEFT, -1);
    Grapple_ActionBindKeySigned(map, "move_x", SDL_SCANCODE_RIGHT, +1);
    Grapple_ActionBindAxis(map, "move_x", GRAPPLE_AXIS_LEFT_X, 0);
    /* The d-pad as held buttons, not the menu-style repeat a direction
       binding gives: a platformer wants "is left held", every step. */
    SDL_zero(binding);
    binding.source = GRAPPLE_BIND_PAD_BUTTON;
    binding.code = GRAPPLE_PAD_DPAD_LEFT;
    binding.sign = -1;
    Grapple_ActionBind(map, "move_x", binding);
    binding.code = GRAPPLE_PAD_DPAD_RIGHT;
    binding.sign = +1;
    Grapple_ActionBind(map, "move_x", binding);

    Grapple_ActionBindKey(map, "jump", SDL_SCANCODE_SPACE);
    Grapple_ActionBindKey(map, "jump", SDL_SCANCODE_Z);
    Grapple_ActionBindPad(map, "jump", GRAPPLE_PAD_A);

    Grapple_ActionBindKey(map, "run", SDL_SCANCODE_LSHIFT);
    Grapple_ActionBindKey(map, "run", SDL_SCANCODE_X);
    Grapple_ActionBindPad(map, "run", GRAPPLE_PAD_X);
    Grapple_ActionBindPad(map, "run", GRAPPLE_PAD_RIGHT_TRIGGER);

    Grapple_ActionMapSetKeyboardPlayer(map, 0);
}

/* --- the level ---------------------------------------------------------- */

Grapple_Platformer *Grapple_CreatePlatformer(Grapple_Engine *engine, int width, int height,
                                             int tile_size)
{
    if (engine == NULL)
    {
        SDL_InvalidParamError("engine");
        return NULL;
    }
    if (width <= 0 || height <= 0 || tile_size <= 0)
    {
        SDL_SetError("a level needs a positive size: %dx%d tiles of %d px", width, height,
                     tile_size);
        return NULL;
    }
    Grapple_Platformer *level = (Grapple_Platformer *)SDL_calloc(1, sizeof(*level));
    if (level == NULL)
    {
        return NULL;
    }
    level->cells = (Uint8 *)SDL_calloc((size_t)width * (size_t)height, sizeof(Uint8));
    level->actions = Grapple_ActionMapCreate();
    if (level->cells == NULL || level->actions == NULL)
    {
        Grapple_DestroyPlatformer(level);
        return NULL;
    }
    level->engine = engine;
    level->width = width;
    level->height = height;
    level->tile = tile_size;
    DefaultColors(level);
    DefaultActions(level);

    /* The display is the level's business, not the game's. A game that gave
       no design size gets a 16:9 frame of 24 x 13.5 tiles — the framing of
       the modern Mario games, and one that 1080p and 4K enlarge by whole
       numbers — and pixel art's presentation: the frame drawn at design
       size and enlarged as a whole, so nothing is scaled per sprite and
       every art pixel is a square block. A game that chose a design size,
       or a presentation other than the engine's letterbox default, keeps
       its choice. */
    if (!Grapple_EngineDesignExplicit(engine))
    {
        Grapple_EngineSetDesignSize(engine, GRAPPLE_PLATFORMER_VIEW_TILES_WIDE * tile_size,
                                    (GRAPPLE_PLATFORMER_VIEW_TILES_HIGH_X2 * tile_size) / 2);
    }
    if (Grapple_EnginePresentation_(engine) == GRAPPLE_PRESENT_LETTERBOX)
    {
        Grapple_EngineSetPresentation(engine, GRAPPLE_PRESENT_PIXEL);
    }

    Grapple_CameraInit(&level->camera, engine);
    level->camera.bounds =
        (SDL_FRect){0.0f, 0.0f, (float)(width * tile_size), (float)(height * tile_size)};
    /* A deadzone a few tiles wide is what stops the screen twitching every
       time the player hops; the smoothing is short so a sprint still feels
       tracked rather than dragged. */
    /* Both deadzones are applied here rather than by the camera, because the
       look-ahead is added on top of the deadzone-tracked position, not
       inside it — the view leads the player, not the box. */
    level->camera.deadzone_w = 0.0f;
    level->camera.deadzone_h = 0.0f;
    level->deadzone_w = 3.0f * (float)tile_size;
    level->look_ahead = 4.0f * (float)tile_size;
    level->vertical_band = 5.5f * (float)tile_size; /* a running jump fits inside it */
    level->camera.smoothing = 0.12f;

    SDL_snprintf(level->scene_name, sizeof(level->scene_name), "platformer#%u", ++g_scene_serial);

    PlatformerPlayerInit(level);
    return level;
}

void Grapple_DestroyPlatformer(Grapple_Platformer *level)
{
    if (level == NULL)
    {
        return;
    }
    if (level->attached)
    {
        /* The scene may outlive the level by a frame; make sure it finds
           nothing rather than freed memory. */
        Grapple_Scene *scene = Grapple_SceneFind(level->engine, level->scene_name);
        if (scene != NULL)
        {
            Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_SceneState(scene);
            if (slot != NULL)
            {
                *slot = NULL;
            }
        }
        Unregister(level);
    }
    Grapple_PlatformerRemovePlayer(level);
    Grapple_ActionMapDestroy(level->actions);
    SDL_free(level->solids);
    SDL_free(level->cells);
    SDL_free(level);
}

Grapple_Engine *Grapple_PlatformerEngine(Grapple_Platformer *level)
{
    return (level != NULL) ? level->engine : NULL;
}

int Grapple_PlatformerWidth(Grapple_Platformer *level)
{
    return (level != NULL) ? level->width : 0;
}

int Grapple_PlatformerHeight(Grapple_Platformer *level)
{
    return (level != NULL) ? level->height : 0;
}

int Grapple_PlatformerTileSize(Grapple_Platformer *level)
{
    return (level != NULL) ? level->tile : 0;
}

void Grapple_PlatformerPixelSize(Grapple_Platformer *level, float *width, float *height)
{
    if (width != NULL)
    {
        *width = (level != NULL) ? (float)(level->width * level->tile) : 0.0f;
    }
    if (height != NULL)
    {
        *height = (level != NULL) ? (float)(level->height * level->tile) : 0.0f;
    }
}

/* --- cells ---------------------------------------------------------------- */

static bool Inside(const Grapple_Platformer *level, int x, int y)
{
    return x >= 0 && y >= 0 && x < level->width && y < level->height;
}

bool Grapple_PlatformerSetTile(Grapple_Platformer *level, int x, int y, Grapple_PlatformerTile tile)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    if (!Inside(level, x, y))
    {
        return SDL_SetError("cell (%d, %d) is outside the %dx%d level", x, y, level->width,
                            level->height);
    }
    if ((int)tile < 0 || tile >= GRAPPLE_PLATFORMER_TILE_COUNT)
    {
        return SDL_SetError("unknown tile kind %d", (int)tile);
    }
    level->cells[(size_t)y * (size_t)level->width + (size_t)x] = (Uint8)tile;
    return true;
}

Grapple_PlatformerTile Grapple_PlatformerTileAt(Grapple_Platformer *level, int x, int y)
{
    if (level == NULL)
    {
        return GRAPPLE_PLATFORMER_EMPTY;
    }
    if (x < 0 || x >= level->width)
    {
        /* The sides of the level are walls: SMB's Mario cannot walk off the
           left of the screen, and nothing good comes of letting him. */
        return GRAPPLE_PLATFORMER_SOLID;
    }
    if (y < 0 || y >= level->height)
    {
        /* Open above, so a high jump is not a head bump, and open below,
           so a pit is a pit. */
        return GRAPPLE_PLATFORMER_EMPTY;
    }
    return (Grapple_PlatformerTile)level->cells[(size_t)y * (size_t)level->width + (size_t)x];
}

int Grapple_PlatformerFillTiles(Grapple_Platformer *level, int x, int y, int width, int height,
                                Grapple_PlatformerTile tile)
{
    if (level == NULL || width <= 0 || height <= 0)
    {
        return 0;
    }
    if ((int)tile < 0 || tile >= GRAPPLE_PLATFORMER_TILE_COUNT)
    {
        SDL_SetError("unknown tile kind %d", (int)tile);
        return 0;
    }
    const int x0 = (x < 0) ? 0 : x;
    const int y0 = (y < 0) ? 0 : y;
    const int x1 = (x + width > level->width) ? level->width : x + width;
    const int y1 = (y + height > level->height) ? level->height : y + height;
    int count = 0;
    for (int cy = y0; cy < y1; ++cy)
    {
        for (int cx = x0; cx < x1; ++cx)
        {
            level->cells[(size_t)cy * (size_t)level->width + (size_t)cx] = (Uint8)tile;
            count++;
        }
    }
    return count;
}

bool Grapple_PlatformerCreateFloor(Grapple_Platformer *level, int x, int y, int width, int depth)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    if (depth <= 0)
    {
        depth = level->height - y;
    }
    return Grapple_PlatformerFillTiles(level, x, y, width, depth, GRAPPLE_PLATFORMER_SOLID) > 0;
}

bool Grapple_PlatformerCreateWall(Grapple_Platformer *level, int x, int y, int height)
{
    return Grapple_PlatformerFillTiles(level, x, y, 1, height, GRAPPLE_PLATFORMER_SOLID) > 0;
}

bool Grapple_PlatformerCreateBlock(Grapple_Platformer *level, int x, int y, int width, int height)
{
    return Grapple_PlatformerFillTiles(level, x, y, width, height, GRAPPLE_PLATFORMER_SOLID) > 0;
}

bool Grapple_PlatformerCreateStairs(Grapple_Platformer *level, int x, int y, int steps, bool rising)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    int count = 0;
    for (int i = 0; i < steps; ++i)
    {
        /* Column i is i + 1 tiles tall when rising, steps - i when falling;
           every column stands on the same base row, which is where the
           first column's single tile sits. */
        const int tall = rising ? (i + 1) : (steps - i);
        count += Grapple_PlatformerFillTiles(level, x + i, y - (tall - 1), 1, tall,
                                             GRAPPLE_PLATFORMER_SOLID);
    }
    return count > 0;
}

bool Grapple_PlatformerCreatePlatform(Grapple_Platformer *level, int x, int y, int width)
{
    return Grapple_PlatformerFillTiles(level, x, y, width, 1, GRAPPLE_PLATFORMER_ONE_WAY) > 0;
}

/* --- pixel solids --------------------------------------------------------- */

int Grapple_PlatformerAddSolid(Grapple_Platformer *level, float x, float y, float w, float h)
{
    if (level == NULL)
    {
        SDL_InvalidParamError("level");
        return -1;
    }
    if (w <= 0.0f || h <= 0.0f)
    {
        SDL_SetError("a solid needs a positive size");
        return -1;
    }
    for (int i = 0; i < level->solid_count; ++i)
    {
        if (!level->solids[i].alive)
        {
            level->solids[i].rect = (SDL_FRect){x, y, w, h};
            level->solids[i].alive = true;
            return i;
        }
    }
    if (level->solid_count == level->solid_capacity)
    {
        const int grown = (level->solid_capacity == 0) ? 16 : level->solid_capacity * 2;
        PlatformerSolid *solids =
            (PlatformerSolid *)SDL_realloc(level->solids, (size_t)grown * sizeof(*solids));
        if (solids == NULL)
        {
            return -1;
        }
        level->solids = solids;
        level->solid_capacity = grown;
    }
    level->solids[level->solid_count].rect = (SDL_FRect){x, y, w, h};
    level->solids[level->solid_count].alive = true;
    return level->solid_count++;
}

bool Grapple_PlatformerRemoveSolid(Grapple_Platformer *level, int id)
{
    if (level == NULL || id < 0 || id >= level->solid_count || !level->solids[id].alive)
    {
        return SDL_SetError("no solid with id %d", id);
    }
    level->solids[id].alive = false;
    return true;
}

int Grapple_PlatformerSolidCount(Grapple_Platformer *level)
{
    int count = 0;
    for (int i = 0; level != NULL && i < level->solid_count; ++i)
    {
        count += level->solids[i].alive ? 1 : 0;
    }
    return count;
}

bool PlatformerCellBlocks(Grapple_Platformer *level, int cx, int cy, bool moving_down,
                          float old_bottom)
{
    switch (Grapple_PlatformerTileAt(level, cx, cy))
    {
    case GRAPPLE_PLATFORMER_SOLID:
        return true;
    case GRAPPLE_PLATFORMER_ONE_WAY:
        /* Only from above: the feet were at or above the ledge's top edge
           before this move. */
        return moving_down && old_bottom <= (float)(cy * level->tile) + EDGE_EPSILON;
    case GRAPPLE_PLATFORMER_EMPTY:
    default:
        return false;
    }
}

/* --- look ----------------------------------------------------------------- */

void Grapple_PlatformerSetBackgroundColor(Grapple_Platformer *level, float r, float g, float b,
                                          float a)
{
    if (level != NULL)
    {
        level->background = (SDL_FColor){r, g, b, a};
    }
}

void Grapple_PlatformerSetTileColor(Grapple_Platformer *level, Grapple_PlatformerTile tile, float r,
                                    float g, float b, float a)
{
    if (level != NULL && (int)tile >= 0 && tile < GRAPPLE_PLATFORMER_TILE_COUNT)
    {
        level->tile_colors[tile] = (SDL_FColor){r, g, b, a};
    }
}

void Grapple_PlatformerSetTileTexture(Grapple_Platformer *level, Grapple_PlatformerTile tile,
                                      SDL_Texture *texture)
{
    if (level != NULL && (int)tile >= 0 && tile < GRAPPLE_PLATFORMER_TILE_COUNT)
    {
        level->tile_textures[tile] = texture;
    }
}

/* --- camera --------------------------------------------------------------- */

void Grapple_PlatformerSetScroll(Grapple_Platformer *level, Grapple_PlatformerScroll mode)
{
    if (level != NULL)
    {
        level->scroll = mode;
    }
}

Grapple_PlatformerScroll Grapple_PlatformerScrollMode(Grapple_Platformer *level)
{
    return (level != NULL) ? level->scroll : GRAPPLE_PLATFORMER_SCROLL_FREE;
}

void Grapple_PlatformerSetCameraSmoothing(Grapple_Platformer *level, float seconds)
{
    if (level != NULL)
    {
        level->camera.smoothing = (seconds > 0.0f) ? seconds : 0.0f;
    }
}

void Grapple_PlatformerSetCameraLookAhead(Grapple_Platformer *level, float pixels)
{
    if (level != NULL)
    {
        level->look_ahead = (pixels > 0.0f) ? pixels : 0.0f;
    }
}

void Grapple_PlatformerSetCameraDeadzone(Grapple_Platformer *level, float width, float height)
{
    if (level != NULL)
    {
        level->deadzone_w = (width > 0.0f) ? width : 0.0f;
        level->vertical_band = (height > 0.0f) ? height : 0.0f;
    }
}

void Grapple_PlatformerCameraPosition(Grapple_Platformer *level, float *x, float *y)
{
    if (x != NULL)
    {
        *x = (level != NULL) ? level->camera.x : 0.0f;
    }
    if (y != NULL)
    {
        *y = (level != NULL) ? level->camera.y : 0.0f;
    }
}

Grapple_Camera *Grapple_PlatformerCamera(Grapple_Platformer *level)
{
    return (level != NULL) ? &level->camera : NULL;
}

/* --- the loop ------------------------------------------------------------- */

bool Grapple_PlatformerAttach(Grapple_Platformer *level)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    if (level->attached)
    {
        return true;
    }
    if (!Register(level))
    {
        return false;
    }
    Grapple_SceneDef def;
    SDL_zero(def);
    def.name = level->scene_name;
    def.state_size = sizeof(Grapple_Platformer *);
    def.load = SceneLoad;
    def.fixed_update = SceneStep;
    def.update = SceneUpdate;
    def.render = SceneRender;
    def.unload = SceneUnload;
    if (!Grapple_ScenePush(level->engine, &def))
    {
        Unregister(level);
        return false;
    }
    level->attached = true;
    return true;
}

void Grapple_PlatformerDetach(Grapple_Platformer *level)
{
    if (level == NULL || !level->attached)
    {
        return;
    }
    Grapple_Scene *scene = Grapple_SceneFind(level->engine, level->scene_name);
    if (scene != NULL && scene == Grapple_SceneCurrent(level->engine))
    {
        Grapple_ScenePop(level->engine); /* lands at the end of the frame */
    }
    else if (scene != NULL)
    {
        /* Covered by something else: leave it on the stack, but inert. */
        Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_SceneState(scene);
        if (slot != NULL)
        {
            *slot = NULL;
        }
    }
    Unregister(level);
    level->attached = false;
}

bool Grapple_PlatformerAttached(Grapple_Platformer *level)
{
    return level != NULL && level->attached;
}

void Grapple_PlatformerStep(Grapple_Platformer *level, float step)
{
    if (level == NULL || step <= 0.0f)
    {
        return;
    }
    /* The player moves from its actor's own fixed update, so the engine's
       transform snapshot (taken just before the actors run) still holds
       where it was — which is what lets the render interpolate. When the
       player has no actor to be moved from, this is where it steps. */
    if (level->player.exists && !Grapple_ActorAlive(level->engine, level->player.id))
    {
        PlatformerPlayerStep(level, step);
    }
}

void Grapple_PlatformerUpdate(Grapple_Platformer *level, float dt)
{
    if (level == NULL)
    {
        return;
    }
    Grapple_Camera *camera = &level->camera;
    if (level->player.exists)
    {
        const PlatformerPlayer *p = &level->player;
        const float half_height = p->height * 0.5f;

        /* Look ahead of the player in the direction faced, easing there so
           a turn swings the view over rather than snapping it. */
        const float want_look = (float)p->facing * level->look_ahead;
        const float ease = (dt > 0.0f) ? SDL_min(1.0f, dt * 4.0f) : 1.0f;
        level->look += (want_look - level->look) * ease;
        /* The player moves inside a box before the view follows; the box is
           what stops a hop or a half-step from twitching the screen. */
        const float half_zone = level->deadzone_w * 0.5f;
        if (p->x > level->follow_x + half_zone)
        {
            level->follow_x = p->x - half_zone;
        }
        else if (p->x < level->follow_x - half_zone)
        {
            level->follow_x = p->x + half_zone;
        }
        const float target_x = level->follow_x + level->look;

        /* Vertically, hold the last ground level through a jump — a camera
           that chases every hop makes a platformer nauseating — and only
           follow when the player leaves a band around it: climbing out of
           the top of the view, or falling out of the bottom. Grounded, the
           feet's level is the ground level, so stairs and ledges scroll. */
        float target_y = p->ground_y - half_height;
        const float mid_y = p->y - half_height;
        if (!p->grounded && level->vertical_band > 0.0f)
        {
            if (mid_y < target_y - level->vertical_band)
            {
                target_y = mid_y + level->vertical_band;
            }
            else if (mid_y > target_y + level->vertical_band)
            {
                target_y = mid_y - level->vertical_band;
            }
        }
        if (!level->camera_snapped)
        {
            /* A fresh start: the whole level is in bounds again, whatever
               forward scrolling had cut off, and the camera lands on the
               player instead of sliding there. */
            camera->bounds = (SDL_FRect){0.0f, 0.0f, (float)(level->width * level->tile),
                                         (float)(level->height * level->tile)};
            level->look = want_look;
            level->follow_x = p->x;
            Grapple_CameraSnap(camera, p->x + level->look, target_y);
            level->camera_snapped = true;
        }
        else
        {
            Grapple_CameraFollow(camera, target_x, target_y);
        }
    }
    Grapple_CameraUpdate(camera, level->engine, dt);

    if (level->scroll == GRAPPLE_PLATFORMER_SCROLL_FORWARD)
    {
        /* The level's left edge advances with the camera and never goes
           back. Doing it through the bounds means CameraUpdate's own
           clamping enforces it, and the visible rect stays honest. */
        const float left = camera->visible.x;
        if (left > camera->bounds.x)
        {
            camera->bounds.w -= left - camera->bounds.x;
            camera->bounds.x = left;
        }
    }
}

static void DrawCell(Grapple_Platformer *level, SDL_Renderer *renderer, int cx, int cy,
                     Grapple_PlatformerTile tile)
{
    const float size = (float)level->tile;
    const SDL_FRect world = {(float)cx * size, (float)cy * size, size, size};
    const SDL_FRect dst = Grapple_CameraRect(&level->camera, world);
    if (level->tile_textures[tile] != NULL)
    {
        SDL_RenderTexture(renderer, level->tile_textures[tile], NULL, &dst);
        return;
    }
    const SDL_FColor color = level->tile_colors[tile];
    SDL_SetRenderDrawColorFloat(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &dst);
    /* A darker rim on two sides makes each cell read as a block rather than
       a run of cells merging into one slab — a level built of rectangles
       still shows where its tiles are. */
    SDL_SetRenderDrawColorFloat(renderer, color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, color.a);
    const SDL_FRect bottom = {dst.x, dst.y + dst.h - 1.0f, dst.w, 1.0f};
    const SDL_FRect right = {dst.x + dst.w - 1.0f, dst.y, 1.0f, dst.h};
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &right);
}

void Grapple_PlatformerRender(Grapple_Platformer *level, float alpha)
{
    if (level == NULL)
    {
        return;
    }
    SDL_Renderer *renderer = Grapple_EngineRenderer(level->engine);
    if (renderer == NULL)
    {
        return;
    }
    Grapple_Camera *camera = &level->camera;
    if (camera->visible.w <= 0.0f)
    {
        /* Rendering before the first update: settle the camera so the
           first frame is not drawn from the origin. */
        Grapple_PlatformerUpdate(level, 0.0f);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const SDL_FRect view = Grapple_EngineViewRect(level->engine);
    SDL_SetRenderDrawColorFloat(renderer, level->background.r, level->background.g,
                                level->background.b, level->background.a);
    SDL_RenderFillRect(renderer, &view);

    if (Grapple_CameraBegin(level->engine, camera))
    {
        const float size = (float)level->tile;
        const int x0 = (int)SDL_floorf(camera->visible.x / size) - 1;
        const int y0 = (int)SDL_floorf(camera->visible.y / size) - 1;
        const int x1 = (int)SDL_ceilf((camera->visible.x + camera->visible.w) / size) + 1;
        const int y1 = (int)SDL_ceilf((camera->visible.y + camera->visible.h) / size) + 1;
        for (int cy = (y0 < 0 ? 0 : y0); cy < y1 && cy < level->height; ++cy)
        {
            for (int cx = (x0 < 0 ? 0 : x0); cx < x1 && cx < level->width; ++cx)
            {
                const Grapple_PlatformerTile tile = Grapple_PlatformerTileAt(level, cx, cy);
                if (tile != GRAPPLE_PLATFORMER_EMPTY)
                {
                    DrawCell(level, renderer, cx, cy, tile);
                }
            }
        }
        const SDL_FColor solid = level->tile_colors[GRAPPLE_PLATFORMER_SOLID];
        SDL_SetRenderDrawColorFloat(renderer, solid.r * 0.8f, solid.g * 0.8f, solid.b * 0.8f,
                                    solid.a);
        for (int i = 0; i < level->solid_count; ++i)
        {
            if (level->solids[i].alive && Grapple_CameraVisible(camera, level->solids[i].rect))
            {
                const SDL_FRect dst = Grapple_CameraRect(camera, level->solids[i].rect);
                SDL_RenderFillRect(renderer, &dst);
            }
        }
        Grapple_CameraEnd(level->engine);
    }

    /* The actors — the player, and anything the game added — drawn sorted,
       culled and interpolated by the engine. */
    Grapple_RenderWorld(level->engine, camera, alpha);
}

/* --- input ---------------------------------------------------------------- */

Grapple_ActionMap *Grapple_PlatformerActions(Grapple_Platformer *level)
{
    return (level != NULL) ? level->actions : NULL;
}

bool Grapple_PlatformerBind(Grapple_Platformer *level, const char *action, const char *binding)
{
    if (level == NULL || action == NULL || binding == NULL)
    {
        return SDL_InvalidParamError("level/action/binding");
    }
    Grapple_Binding parsed;
    if (!Grapple_BindingFromString(binding, &parsed))
    {
        return SDL_SetError("unknown binding '%s' (try \"space\", \"pad:a\", \"-a\", "
                            "\"axis:left_x\")",
                            binding);
    }
    return Grapple_ActionBind(level->actions, action, parsed);
}

bool Grapple_PlatformerActionDown(Grapple_Platformer *level, const char *action)
{
    return level != NULL && Grapple_ActionDown(level->engine, level->actions, 0, action);
}

bool Grapple_PlatformerActionPressed(Grapple_Platformer *level, const char *action)
{
    return level != NULL && Grapple_ActionPressed(level->engine, level->actions, 0, action);
}

bool Grapple_PlatformerActionReleased(Grapple_Platformer *level, const char *action)
{
    return level != NULL && Grapple_ActionReleased(level->engine, level->actions, 0, action);
}

float Grapple_PlatformerActionValue(Grapple_Platformer *level, const char *action)
{
    return (level != NULL) ? Grapple_ActionValue(level->engine, level->actions, 0, action) : 0.0f;
}

void Grapple_PlatformerSetScriptedInput(Grapple_Platformer *level, bool enabled)
{
    if (level != NULL)
    {
        level->scripted = enabled;
        if (!enabled)
        {
            level->script_move = 0.0f;
            level->script_jump = false;
            level->script_run = false;
        }
    }
}

bool Grapple_PlatformerScriptedInput(Grapple_Platformer *level)
{
    return level != NULL && level->scripted;
}

void Grapple_PlatformerScriptInput(Grapple_Platformer *level, float move_x, bool jump, bool run)
{
    if (level != NULL)
    {
        level->script_move = SDL_clamp(move_x, -1.0f, 1.0f);
        level->script_jump = jump;
        level->script_run = run;
    }
}
