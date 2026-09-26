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
    if (tile_size > PLATFORMER_MAX_TILE)
    {
        SDL_SetError("tiles larger than %d px are not supported", PLATFORMER_MAX_TILE);
        SDL_free(level);
        return NULL;
    }
    level->cells = (Uint8 *)SDL_calloc((size_t)width * (size_t)height, sizeof(Uint8));
    level->cell_mask = (Uint16 *)SDL_calloc((size_t)width * (size_t)height, sizeof(Uint16));
    level->actions = Grapple_ActionMapCreate();
    if (level->cells == NULL || level->cell_mask == NULL || level->actions == NULL)
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
    SDL_free(level->swappers);
    SDL_free(level->masks);
    SDL_free(level->cell_mask);
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
    level->cell_mask[(size_t)y * (size_t)level->width + (size_t)x] = 0; /* a whole cell again */
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
            level->cell_mask[(size_t)cy * (size_t)level->width + (size_t)cx] = 0;
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

/* --- masks: cells that are not whole blocks -------------------------------- */

static size_t CellIndex(const Grapple_Platformer *level, int x, int y)
{
    return (size_t)y * (size_t)level->width + (size_t)x;
}

/* The mask of cell (x, y), making one if it has none. A whole solid cell
   gets a full mask; an empty cell an empty one. NULL when out of memory. */
static PlatformerMask *CellMask(Grapple_Platformer *level, int x, int y)
{
    const size_t index = CellIndex(level, x, y);
    if (level->cell_mask[index] != 0)
    {
        return &level->masks[level->cell_mask[index] - 1];
    }
    if (level->mask_count == level->mask_capacity)
    {
        const int grown = (level->mask_capacity == 0) ? 64 : level->mask_capacity * 2;
        PlatformerMask *masks =
            (PlatformerMask *)SDL_realloc(level->masks, (size_t)grown * sizeof(*masks));
        if (masks == NULL)
        {
            return NULL;
        }
        level->masks = masks;
        level->mask_capacity = grown;
    }
    PlatformerMask *mask = &level->masks[level->mask_count];
    const Uint32 full = (level->tile >= 32) ? 0xFFFFFFFFu : ((1u << level->tile) - 1u);
    const bool solid = level->cells[index] == GRAPPLE_PLATFORMER_SOLID;
    for (int r = 0; r < PLATFORMER_MAX_TILE; ++r)
    {
        mask->rows[0][r] = solid ? full : 0u;
        mask->rows[1][r] = solid ? full : 0u;
    }
    level->cell_mask[index] = (Uint16)(level->mask_count + 1);
    level->cells[index] = GRAPPLE_PLATFORMER_SOLID;
    level->mask_count++;
    return mask;
}

/* Make pixel (px, py) solid, on `layers` (0 = both), as part of a shape. A
   cell that becomes wholly solid stays a plain block; one that is partly
   solid gets a mask. Existing solids are kept. */
static void PaintPixel(Grapple_Platformer *level, int px, int py, int layers)
{
    const int cx = px / level->tile;
    const int cy = py / level->tile;
    if (px < 0 || py < 0 || cx >= level->width || cy >= level->height)
    {
        return;
    }
    const size_t index = CellIndex(level, cx, cy);
    if (level->cells[index] == GRAPPLE_PLATFORMER_SOLID && level->cell_mask[index] == 0)
    {
        return; /* already a whole block, on both layers */
    }
    PlatformerMask *mask = CellMask(level, cx, cy);
    if (mask == NULL)
    {
        return;
    }
    const Uint32 bit = 1u << (px % level->tile);
    if (layers == 0 || (layers & PLATFORMER_LAYER_A))
    {
        mask->rows[0][py % level->tile] |= bit;
    }
    if (layers == 0 || (layers & PLATFORMER_LAYER_B))
    {
        mask->rows[1][py % level->tile] |= bit;
    }
}

/* After painting: cells whose mask filled up become whole blocks again, so
   the fast path and TileAt see them as such. */
static void SettleMasks(Grapple_Platformer *level)
{
    const Uint32 full = (level->tile >= 32) ? 0xFFFFFFFFu : ((1u << level->tile) - 1u);
    for (int cy = 0; cy < level->height; ++cy)
    {
        for (int cx = 0; cx < level->width; ++cx)
        {
            const size_t index = CellIndex(level, cx, cy);
            if (level->cell_mask[index] == 0)
            {
                continue;
            }
            const PlatformerMask *mask = &level->masks[level->cell_mask[index] - 1];
            bool whole = true;
            bool empty = true;
            for (int r = 0; r < level->tile; ++r)
            {
                whole = whole && mask->rows[0][r] == full && mask->rows[1][r] == full;
                empty = empty && mask->rows[0][r] == 0u && mask->rows[1][r] == 0u;
            }
            if (whole)
            {
                level->cell_mask[index] = 0; /* the mask stays in the pool, unused */
            }
            else if (empty)
            {
                level->cell_mask[index] = 0;
                level->cells[index] = GRAPPLE_PLATFORMER_EMPTY;
            }
        }
    }
}

bool PlatformerSolidPixel(Grapple_Platformer *level, int px, int py, int layer)
{
    const int cx = (px >= 0) ? px / level->tile : -1;
    const int cy = (py >= 0) ? py / level->tile : -1;
    if (cx < 0 || cx >= level->width)
    {
        return true; /* the sides of the level are walls */
    }
    if (cy < 0 || cy >= level->height)
    {
        return false; /* open above and below */
    }
    const size_t index = CellIndex(level, cx, cy);
    if (level->cells[index] == GRAPPLE_PLATFORMER_SOLID)
    {
        if (level->cell_mask[index] == 0)
        {
            return true;
        }
        /* A shaped cell keeps a mask per layer; a player on both sees both. */
        const PlatformerMask *mask = &level->masks[level->cell_mask[index] - 1];
        const Uint32 bit = 1u << (px % level->tile);
        const int row = py % level->tile;
        return ((layer & PLATFORMER_LAYER_A) && (mask->rows[0][row] & bit)) ||
               ((layer & PLATFORMER_LAYER_B) && (mask->rows[1][row] & bit));
    }
    for (int i = 0; i < level->solid_count; ++i)
    {
        const SDL_FRect r = level->solids[i].rect;
        if (level->solids[i].alive && (float)px >= r.x && (float)px < r.x + r.w &&
            (float)py >= r.y && (float)py < r.y + r.h)
        {
            return true;
        }
    }
    return false;
}

bool PlatformerLedgePixel(Grapple_Platformer *level, int px, int py, float old_bottom)
{
    const int cx = px / level->tile;
    const int cy = py / level->tile;
    if (px < 0 || py < 0 || cx >= level->width || cy >= level->height)
    {
        return false;
    }
    if (level->cells[CellIndex(level, cx, cy)] != GRAPPLE_PLATFORMER_ONE_WAY)
    {
        return false;
    }
    /* Only the ledge's top row, and only from above it. */
    return py % level->tile == 0 && old_bottom <= (float)py + 0.01f;
}

bool Grapple_PlatformerSolidAt(Grapple_Platformer *level, float x, float y)
{
    if (level == NULL)
    {
        return false;
    }
    const int layer =
        level->player.exists ? level->player.layer : (PLATFORMER_LAYER_A | PLATFORMER_LAYER_B);
    return PlatformerSolidPixel(level, (int)SDL_floorf(x), (int)SDL_floorf(y), layer);
}

bool Grapple_PlatformerCreateSlope(Grapple_Platformer *level, int x, int y, int width, int height,
                                   bool rising)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    if (width <= 0 || height <= 0)
    {
        return SDL_SetError("a slope needs a positive size");
    }
    const int tile = level->tile;
    const int left = x * tile;
    const int top = y * tile;
    const int w = width * tile;
    const int h = height * tile;
    /* Solid below the line; the line runs corner to corner. */
    bool painted = false;
    for (int py = top; py < top + h; ++py)
    {
        for (int px = left; px < left + w; ++px)
        {
            const float u = ((float)(px - left) + 0.5f) / (float)w; /* 0 at the left edge */
            const float surface =
                rising ? (float)(top + h) - u * (float)h : (float)top + u * (float)h;
            if ((float)py + 0.5f >= surface)
            {
                PaintPixel(level, px, py, 0);
                painted = true;
            }
        }
    }
    SettleMasks(level);
    return painted;
}

static bool AddSwapper(Grapple_Platformer *level, float x, float y0, float y1, int moving_right,
                       int moving_left)
{
    if (level->swapper_count == level->swapper_capacity)
    {
        const int grown = (level->swapper_capacity == 0) ? 8 : level->swapper_capacity * 2;
        PlatformerSwapper *swappers =
            (PlatformerSwapper *)SDL_realloc(level->swappers, (size_t)grown * sizeof(*swappers));
        if (swappers == NULL)
        {
            return false;
        }
        level->swappers = swappers;
        level->swapper_capacity = grown;
    }
    level->swappers[level->swapper_count++] =
        (PlatformerSwapper){x, y0, y1, moving_right, moving_left};
    return true;
}

void PlatformerApplySwappers(Grapple_Platformer *level, float x0, float y0, float x1, float y1,
                             int *layer)
{
    for (int i = 0; i < level->swapper_count; ++i)
    {
        const PlatformerSwapper *s = &level->swappers[i];
        if (x0 == x1)
        {
            continue;
        }
        const bool crossed = (x0 < s->x && x1 >= s->x) || (x0 >= s->x && x1 < s->x);
        if (!crossed)
        {
            continue;
        }
        /* Where the move crossed the line, vertically. */
        const float t = (s->x - x0) / (x1 - x0);
        const float y = y0 + (y1 - y0) * t;
        if (y < s->y0 || y > s->y1)
        {
            continue;
        }
        const int set = (x1 > x0) ? s->moving_right : s->moving_left;
        if (set != 0)
        {
            *layer = set;
        }
    }
}

bool Grapple_PlatformerCreateLoop(Grapple_Platformer *level, int x, int y, int radius)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    if (radius < 2)
    {
        return SDL_SetError("a loop needs a radius of at least 2 tiles");
    }
    const int tile = level->tile;
    const float outer = (float)(radius * tile);
    const float inner = outer - (float)tile;
    const float cx = (float)(x * tile) + outer;
    const float cy = (float)(y * tile) + outer;

    /* The ring, painted pixel by pixel. Its two bottom quarters are on
       different layers: the entry side passes through the far quarter,
       the exit side through the near one. Angles are y-down: 0 is right,
       +90 is straight down, 180 left, -90 up. */
    for (int py = (int)(cy - outer); py < (int)(cy + outer) + 1; ++py)
    {
        for (int px = (int)(cx - outer); px < (int)(cx + outer) + 1; ++px)
        {
            const float dx = (float)px + 0.5f - cx;
            const float dy = (float)py + 0.5f - cy;
            const float dist = SDL_sqrtf(dx * dx + dy * dy);
            if (dist < inner || dist >= outer)
            {
                continue;
            }
            const float deg = SDL_atan2f(dy, dx) * 180.0f / SDL_PI_F;
            int layers = 0;
            if (deg > 0.0f && deg < 90.0f)
            {
                layers = PLATFORMER_LAYER_A; /* bottom-right quarter */
            }
            else if (deg >= 90.0f && deg < 180.0f)
            {
                layers = PLATFORMER_LAYER_B; /* bottom-left quarter */
            }
            PaintPixel(level, px, py, layers);
        }
    }
    SettleMasks(level);

    /* Entrance and exit lines on either side, the ring's full height, and
       the top switch. Entering from the left puts the player on A: the bottom-left
       quarter is not there, the bottom-right is, and the run goes up the
       right side. Crossing the top leftward switches to B: the bottom-left
       quarter catches the way down and the bottom-right is gone, so the
       run continues out to the right. The mirror image from the right. */
    const bool ok =
        AddSwapper(level, cx - outer, cy, cy + outer, PLATFORMER_LAYER_A, PLATFORMER_LAYER_A) &&
        AddSwapper(level, cx + outer, cy, cy + outer, PLATFORMER_LAYER_B, PLATFORMER_LAYER_B) &&
        AddSwapper(level, cx, cy - outer, cy, PLATFORMER_LAYER_A, PLATFORMER_LAYER_B);
    return ok;
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
    /* Where the player is drawn this frame; the snap below anchors to it. */
    float anchor_x = 0.0f;
    float anchor_y = 0.0f;
    if (level->player.exists)
    {
        const PlatformerPlayer *p = &level->player;
        const float half_height = p->height * 0.5f;

        /* Follow the player where it is *drawn* — between its last two
           steps, by this frame's alpha — not where the simulation has it.
           The sprite is interpolated; a camera aimed at the un-interpolated
           position is up to a step out from it, by an amount that changes
           every frame, and the player visibly wobbles against the world. */
        float px = p->x;
        float py = p->y;
        Grapple_Actor *actor = Grapple_ActorGet(level->engine, p->id);
        if (actor != NULL)
        {
            const Grapple_ActorTransform drawn =
                Grapple_ActorRenderTransform(actor, Grapple_EngineAlpha(level->engine));
            px = drawn.x;
            py = drawn.y;
        }
        /* The sprite's corner is what the renderer rounds: half a width
           left of the feet, and a height above. */
        anchor_x = px - p->width * 0.5f;
        anchor_y = py - p->height;

        /* Look ahead of the player in the direction faced, easing there so
           a turn swings the view over rather than snapping it. */
        const float want_look = (float)p->facing * level->look_ahead;
        const float ease = (dt > 0.0f) ? SDL_min(1.0f, dt * 4.0f) : 1.0f;
        level->look += (want_look - level->look) * ease;
        /* The player moves inside a box before the view follows; the box is
           what stops a hop or a half-step from twitching the screen. */
        const float half_zone = level->deadzone_w * 0.5f;
        if (px > level->follow_x + half_zone)
        {
            level->follow_x = px - half_zone;
        }
        else if (px < level->follow_x - half_zone)
        {
            level->follow_x = px + half_zone;
        }
        const float target_x = level->follow_x + level->look;

        /* Vertically, hold the last ground level through a jump — a camera
           that chases every hop makes a platformer nauseating — and only
           follow when the player leaves a band around it: climbing out of
           the top of the view, or falling out of the bottom. Grounded, the
           feet's level is the ground level, so stairs and ledges scroll. */
        float target_y = p->ground_y - half_height;
        const float mid_y = py - half_height;
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
            level->follow_x = px;
            Grapple_CameraSnap(camera, px + level->look, target_y);
            level->camera_snapped = true;
        }
        else
        {
            Grapple_CameraFollow(camera, target_x, target_y);
        }
    }
    Grapple_CameraUpdate(camera, level->engine, dt);

    /* Under the pixel presentations the frame has a whole number of texels
       per design unit, and everything drawn is rounded to that grid. Two
       things round — the view's origin and the player — and whichever is
       meant to hold still on screen must be the one that lands exactly on
       the grid, or their difference flickers by a texel as their fractional
       parts round different ways. While the camera is following, the
       player is what stays put on screen: the origin is placed so the
       player is on the grid and the world scrolls by whole texels. While
       the camera is still — clamped at the level's edge, or the player
       inside the deadzone — the world is what stays put: the origin itself
       goes on the grid and the player steps across it. */
    const int grid = Grapple_EngineFrameScale(level->engine);
    if (grid > 0)
    {
        const float unit = 1.0f / (float)grid;
        const bool moving_x = SDL_fabsf(camera->x - level->last_cam_x) > 0.0001f;
        const bool moving_y = SDL_fabsf(camera->y - level->last_cam_y) > 0.0001f;
        level->last_cam_x = camera->x;
        level->last_cam_y = camera->y;

        float snapped_x;
        float snapped_y;
        if (moving_x && level->player.exists)
        {
            const float gap = SDL_floorf((anchor_x - camera->visible.x) / unit + 0.5f) * unit;
            snapped_x = anchor_x - gap;
        }
        else
        {
            snapped_x = SDL_floorf(camera->visible.x / unit + 0.5f) * unit;
        }
        if (moving_y && level->player.exists)
        {
            const float gap = SDL_floorf((anchor_y - camera->visible.y) / unit + 0.5f) * unit;
            snapped_y = anchor_y - gap;
        }
        else
        {
            snapped_y = SDL_floorf(camera->visible.y / unit + 0.5f) * unit;
        }
        camera->x += snapped_x - camera->visible.x;
        camera->y += snapped_y - camera->visible.y;
        camera->visible.x = snapped_x;
        camera->visible.y = snapped_y;
    }

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
    const size_t index = CellIndex(level, cx, cy);
    if (tile == GRAPPLE_PLATFORMER_SOLID && level->cell_mask[index] != 0)
    {
        /* A shaped cell: its solid pixels, row by row, as runs. Pixels on
           one layer only draw a little paler, so a loop reads as a ring with
           a path through it rather than a wall. */
        const PlatformerMask *mask = &level->masks[level->cell_mask[index] - 1];
        const SDL_FColor color = level->tile_colors[tile];
        for (int pass = 0; pass < 2; ++pass)
        {
            /* Pass 0: pixels on both layers; pass 1: on one layer only. */
            if (pass == 0)
            {
                SDL_SetRenderDrawColorFloat(renderer, color.r, color.g, color.b, color.a);
            }
            else
            {
                SDL_SetRenderDrawColorFloat(renderer, color.r * 0.75f + 0.25f,
                                            color.g * 0.75f + 0.25f, color.b * 0.75f + 0.25f,
                                            color.a);
            }
            for (int r = 0; r < level->tile; ++r)
            {
                const Uint32 both = mask->rows[0][r] & mask->rows[1][r];
                const Uint32 row =
                    (pass == 0) ? both : ((mask->rows[0][r] | mask->rows[1][r]) & ~both);
                int c = 0;
                while (c < level->tile)
                {
                    if (((row >> c) & 1u) == 0u)
                    {
                        c++;
                        continue;
                    }
                    int end = c;
                    while (end < level->tile && ((row >> end) & 1u))
                    {
                        end++;
                    }
                    const SDL_FRect run = {dst.x + (float)c, dst.y + (float)r, (float)(end - c),
                                           1.0f};
                    SDL_RenderFillRect(renderer, &run);
                    c = end;
                }
            }
        }
        return;
    }
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
