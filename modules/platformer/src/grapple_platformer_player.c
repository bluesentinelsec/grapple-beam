/*
 * grapple_platformer_player.c — the player character: an axis-aligned box
 * swept against the grid one axis at a time, with the acceleration curve,
 * variable-height jump, coyote time and jump buffer that make a platformer
 * feel like the ones it is modelled on. Original grapple-beam code (zlib).
 *
 * The model is the one every NES and SNES platformer used: no rigid bodies,
 * no solver. Gravity is applied every step and the floor cancels it, which is
 * what makes "am I standing" a fact the collision pass produces rather than a
 * question a raycast has to answer.
 */
#include "platformer_internal.h"

#include <grapple/engine_render.h>
#include <math.h>

#define EDGE_EPSILON 0.01f

/* The tuning keys, in PlatformerTuning order. */
static const char *const kTuningKeys[TUNE_COUNT] = {
    "walk_speed",   "run_speed", "accel",       "decel",          "skid_decel",
    "air_accel",    "air_decel", "jump_speed",  "run_jump_bonus", "jump_gravity",
    "fall_gravity", "max_fall",  "coyote_time", "jump_buffer",
};

/* Defaults in *tiles* per second (and per second squared), so a level with
   32-pixel tiles feels the same as one with 16. They are Super Mario Bros.'
   numbers: 1.5 px/frame walking, 2.5 running, a 4 px/frame take-off, gravity
   of 0.125 px/frame² while the button is held and 0.4375 once it is not,
   all at 60 frames a second and 16-pixel tiles. */
static const float kTuningDefaultsInTiles[TUNE_COUNT] = {
    [TUNE_WALK_SPEED] = 6.0f,      [TUNE_RUN_SPEED] = 10.0f,    [TUNE_ACCEL] = 25.0f,
    [TUNE_DECEL] = 37.5f,          [TUNE_SKID_DECEL] = 56.0f,   [TUNE_AIR_ACCEL] = 18.75f,
    [TUNE_AIR_DECEL] = 0.0f,       [TUNE_JUMP_SPEED] = 15.0f,   [TUNE_RUN_JUMP_BONUS] = 2.0f,
    [TUNE_JUMP_GRAVITY] = 28.125f, [TUNE_FALL_GRAVITY] = 98.0f, [TUNE_MAX_FALL] = 17.0f,
};

static const char *const kStateNames[GRAPPLE_PLAYER_STATE_COUNT] = {
    "idle", "walk", "run", "jump", "fall", "paused",
};

static int TuningIndex(const char *key)
{
    if (key == NULL)
    {
        return -1;
    }
    for (int i = 0; i < TUNE_COUNT; ++i)
    {
        if (SDL_strcmp(kTuningKeys[i], key) == 0)
        {
            return i;
        }
    }
    return -1;
}

void PlatformerPlayerInit(Grapple_Platformer *level)
{
    PlatformerPlayer *p = &level->player;
    SDL_zerop(p);
    const float tile = (float)level->tile;
    for (int i = 0; i < TUNE_COUNT; ++i)
    {
        p->tuning[i] = kTuningDefaultsInTiles[i] * tile;
    }
    p->tuning[TUNE_COYOTE_TIME] = 0.08f;
    p->tuning[TUNE_JUMP_BUFFER] = 0.10f;
    /* A little under a tile wide and just under two tall: SMB's big Mario
       is drawn 16x32 and collides narrower, which is what keeps him from
       catching on the lip of a one-tile gap he visually fits through. */
    p->width = tile * 0.875f;
    p->height = tile * 1.875f;
    p->color = (SDL_FColor){0.90f, 0.25f, 0.20f, 1.0f};
    p->facing = 1;
    p->state = GRAPPLE_PLAYER_IDLE;
}

/* --- the actor -------------------------------------------------------------- */

static void ActorStep(Grapple_Actor *actor, float step)
{
    Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_ActorState(actor);
    if (slot != NULL && *slot != NULL)
    {
        PlatformerPlayerStep(*slot, step);
    }
}

void PlatformerPlayerSyncSprite(Grapple_Platformer *level)
{
    PlatformerPlayer *p = &level->player;
    Grapple_Actor *actor = Grapple_ActorGet(level->engine, p->id);
    if (actor == NULL)
    {
        return;
    }
    Grapple_Sprite *sprite = Grapple_ActorSprite(actor);
    if (sprite == NULL)
    {
        Grapple_Sprite fresh = Grapple_SpriteDefault();
        fresh.origin_x = 0.5f;
        fresh.origin_y = 1.0f; /* the position is the feet */
        fresh.layer = 1;
        Grapple_ActorSetSprite(actor, &fresh);
        sprite = Grapple_ActorSprite(actor);
        if (sprite == NULL)
        {
            return;
        }
    }
    if (sprite->texture == NULL)
    {
        /* Still the placeholder: keep it the size and colour of the box.
           Once the game has given it a texture the game owns its look. */
        sprite->width = p->width;
        sprite->height = p->height;
        sprite->color = p->color;
    }
    sprite->flip = (p->facing < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}

bool PlatformerPlayerSpawnActor(Grapple_Platformer *level)
{
    PlatformerPlayer *p = &level->player;
    Grapple_ActorDef def;
    SDL_zero(def);
    def.type = "player";
    def.name = "player";
    def.state_size = sizeof(Grapple_Platformer *);
    def.x = p->x;
    def.y = p->y;
    def.scale_x = 1.0f;
    def.scale_y = 1.0f;
    def.fixed_update = ActorStep;
    p->id = Grapple_ActorSpawn(level->engine, &def);
    if (p->id == GRAPPLE_ACTOR_NONE)
    {
        return false;
    }
    Grapple_Actor *actor = Grapple_ActorGet(level->engine, p->id);
    Grapple_Platformer **slot =
        (actor != NULL) ? (Grapple_Platformer **)Grapple_ActorState(actor) : NULL;
    if (slot == NULL)
    {
        Grapple_ActorDestroy(level->engine, p->id);
        p->id = GRAPPLE_ACTOR_NONE;
        return SDL_SetError("could not attach the level to the player actor");
    }
    *slot = level;
    PlatformerPlayerSyncSprite(level);
    return true;
}

/* --- collision -------------------------------------------------------------- */

static int FloorDiv(float v, float size)
{
    return (int)SDL_floorf(v / size);
}

static bool SolidOverlaps(const Grapple_Platformer *level, float left, float top, float right,
                          float bottom, SDL_FRect *hit)
{
    for (int i = 0; i < level->solid_count; ++i)
    {
        if (!level->solids[i].alive)
        {
            continue;
        }
        const SDL_FRect r = level->solids[i].rect;
        if (left < r.x + r.w - EDGE_EPSILON && right > r.x + EDGE_EPSILON &&
            top < r.y + r.h - EDGE_EPSILON && bottom > r.y + EDGE_EPSILON)
        {
            *hit = r;
            return true;
        }
    }
    return false;
}

/* Move horizontally by dx, stopping at the first solid the leading edge
   would enter. Returns true if something was hit. */
static bool MoveX(Grapple_Platformer *level, PlatformerPlayer *p, float dx)
{
    if (dx == 0.0f)
    {
        return false;
    }
    const float size = (float)level->tile;
    const float half = p->width * 0.5f;
    const float top = p->y - p->height;
    const float bottom = p->y;
    const int r0 = FloorDiv(top + EDGE_EPSILON, size);
    const int r1 = FloorDiv(bottom - EDGE_EPSILON, size);

    float x = p->x + dx;
    bool hit = false;
    if (dx > 0.0f)
    {
        const int col = FloorDiv(x + half - EDGE_EPSILON, size);
        for (int r = r0; r <= r1; ++r)
        {
            if (PlatformerCellBlocks(level, col, r, false, bottom))
            {
                x = (float)col * size - half;
                hit = true;
                break;
            }
        }
    }
    else
    {
        const int col = FloorDiv(x - half + EDGE_EPSILON, size);
        for (int r = r0; r <= r1; ++r)
        {
            if (PlatformerCellBlocks(level, col, r, false, bottom))
            {
                x = (float)(col + 1) * size + half;
                hit = true;
                break;
            }
        }
    }
    SDL_FRect solid;
    if (SolidOverlaps(level, x - half, top, x + half, bottom, &solid))
    {
        x = (dx > 0.0f) ? solid.x - half : solid.x + solid.w + half;
        hit = true;
    }
    p->x = x;
    return hit;
}

/* Move vertically by dy. Sets grounded when landing, bumped when hitting a
   ceiling. Returns true if something was hit. */
static bool MoveY(Grapple_Platformer *level, PlatformerPlayer *p, float dy)
{
    if (dy == 0.0f)
    {
        return false;
    }
    const float size = (float)level->tile;
    const float half = p->width * 0.5f;
    const float left = p->x - half;
    const float right = p->x + half;
    const int c0 = FloorDiv(left + EDGE_EPSILON, size);
    const int c1 = FloorDiv(right - EDGE_EPSILON, size);
    const float old_bottom = p->y;

    float y = p->y + dy;
    bool hit = false;
    if (dy > 0.0f)
    {
        const int row = FloorDiv(y - EDGE_EPSILON, size);
        for (int c = c0; c <= c1; ++c)
        {
            if (PlatformerCellBlocks(level, c, row, true, old_bottom))
            {
                y = (float)row * size;
                hit = true;
                p->grounded = true;
                break;
            }
        }
    }
    else
    {
        const int row = FloorDiv(y - p->height + EDGE_EPSILON, size);
        for (int c = c0; c <= c1; ++c)
        {
            if (PlatformerCellBlocks(level, c, row, false, old_bottom))
            {
                y = (float)(row + 1) * size + p->height;
                hit = true;
                p->bumped = true;
                break;
            }
        }
    }
    SDL_FRect solid;
    if (SolidOverlaps(level, left, y - p->height, right, y, &solid))
    {
        if (dy > 0.0f)
        {
            y = solid.y;
            p->grounded = true;
        }
        else
        {
            y = solid.y + solid.h + p->height;
            p->bumped = true;
        }
        hit = true;
    }
    p->y = y;
    return hit;
}

/* Sweep in pieces no larger than half a tile, so a fast fall cannot pass
   through a floor between two positions. */
static void Sweep(Grapple_Platformer *level, PlatformerPlayer *p, float dx, float dy)
{
    const float limit = (float)level->tile * 0.5f;
    const float longest = SDL_max(SDL_fabsf(dx), SDL_fabsf(dy));
    int pieces = (int)SDL_ceilf(longest / limit);
    if (pieces < 1)
    {
        pieces = 1;
    }
    const float px = dx / (float)pieces;
    const float py = dy / (float)pieces;
    bool stop_x = false;
    bool stop_y = false;
    for (int i = 0; i < pieces; ++i)
    {
        if (!stop_x && MoveX(level, p, px))
        {
            stop_x = true;
            p->vx = 0.0f;
        }
        if (!stop_y && MoveY(level, p, py))
        {
            stop_y = true;
        }
    }
}

/* --- the step --------------------------------------------------------------- */

static float Approach(float value, float target, float amount)
{
    if (value < target)
    {
        return SDL_min(value + amount, target);
    }
    if (value > target)
    {
        return SDL_max(value - amount, target);
    }
    return value;
}

static void SetState(PlatformerPlayer *p, Grapple_PlayerState state)
{
    if (p->state != state)
    {
        p->state = state;
        p->state_changed = true;
    }
}

static void BeginFrameEvents(Grapple_Platformer *level, PlatformerPlayer *p)
{
    const Uint64 frame = Grapple_EngineFrameCount(level->engine);
    if (p->event_frame != frame)
    {
        p->event_frame = frame;
        p->state_changed = false;
        p->landed = false;
        p->jumped = false;
        p->bumped = false;
        p->fell = false;
    }
}

static void ReadInput(Grapple_Platformer *level, float *move, bool *jump, bool *run)
{
    if (level->scripted)
    {
        *move = level->script_move;
        *jump = level->script_jump;
        *run = level->script_run;
        return;
    }
    *move = Grapple_ActionValue(level->engine, level->actions, 0, "move_x");
    *jump = Grapple_ActionDown(level->engine, level->actions, 0, "jump");
    *run = Grapple_ActionDown(level->engine, level->actions, 0, "run");
}

void PlatformerPlayerStep(Grapple_Platformer *level, float step)
{
    PlatformerPlayer *p = &level->player;
    const float *t = p->tuning;
    BeginFrameEvents(level, p);

    if (p->paused)
    {
        SetState(p, GRAPPLE_PLAYER_PAUSED);
        return;
    }

    float move = 0.0f;
    bool jump_down = false;
    bool run_down = false;
    ReadInput(level, &move, &jump_down, &run_down);
    const bool jump_pressed = jump_down && !p->jump_was_down;
    p->jump_was_down = jump_down;

    /* Horizontal: accelerate toward the speed the stick and the run button
       ask for. Turning around on the ground uses the skid rate, which is
       what makes a reversal feel decisive rather than icy. */
    const float top_speed = run_down ? t[TUNE_RUN_SPEED] : t[TUNE_WALK_SPEED];
    const float target = move * top_speed;
    float rate;
    if (p->grounded)
    {
        if (move != 0.0f && p->vx != 0.0f && (move > 0.0f) != (p->vx > 0.0f))
        {
            rate = t[TUNE_SKID_DECEL];
        }
        else
        {
            rate = (move != 0.0f) ? t[TUNE_ACCEL] : t[TUNE_DECEL];
        }
    }
    else
    {
        rate = (move != 0.0f) ? t[TUNE_AIR_ACCEL] : t[TUNE_AIR_DECEL];
    }
    p->vx = Approach(p->vx, target, rate * step);
    if (move > 0.05f)
    {
        p->facing = 1;
    }
    else if (move < -0.05f)
    {
        p->facing = -1;
    }

    /* Jumping. Coyote time lets a jump pressed just after walking off a
       ledge still happen; the buffer lets one pressed just before landing
       happen on landing. Both are the difference between "the game ate my
       jump" and a controller that feels read. */
    if (p->grounded)
    {
        p->coyote = t[TUNE_COYOTE_TIME];
    }
    else
    {
        p->coyote = SDL_max(0.0f, p->coyote - step);
    }
    if (jump_pressed)
    {
        p->buffer = t[TUNE_JUMP_BUFFER];
    }
    else
    {
        p->buffer = SDL_max(0.0f, p->buffer - step);
    }
    if (p->buffer > 0.0f && (p->grounded || p->coyote > 0.0f))
    {
        /* A running jump goes higher: the bonus scales with how much of the
           run speed the player has, so a standing jump gets none of it. */
        const float run_fraction =
            (t[TUNE_RUN_SPEED] > 0.0f) ? SDL_min(1.0f, SDL_fabsf(p->vx) / t[TUNE_RUN_SPEED]) : 0.0f;
        p->vy = -(t[TUNE_JUMP_SPEED] + t[TUNE_RUN_JUMP_BONUS] * run_fraction);
        p->grounded = false;
        p->rising = true;
        p->coyote = 0.0f;
        p->buffer = 0.0f;
        p->jumped = true;
    }

    /* Gravity: light while the button is held on the way up, heavy
       otherwise. Letting go early ends the light phase, which is the whole
       of the variable-height jump. */
    if (!jump_down || p->vy >= 0.0f)
    {
        p->rising = false;
    }
    const float gravity = p->rising ? t[TUNE_JUMP_GRAVITY] : t[TUNE_FALL_GRAVITY];
    p->vy = SDL_min(p->vy + gravity * step, t[TUNE_MAX_FALL]);

    /* Move and collide, x then y. Gravity is applied every step and the
       floor cancels it, so grounded is decided fresh each step by whether
       the downward move was stopped. */
    const bool was_grounded = p->grounded;
    p->grounded = false;
    Sweep(level, p, p->vx * step, p->vy * step);
    if (p->grounded && p->vy > 0.0f)
    {
        p->vy = 0.0f;
    }
    if (p->bumped && p->vy < 0.0f)
    {
        p->vy = 0.0f;
        p->rising = false;
    }
    if (p->grounded && !was_grounded)
    {
        p->landed = true;
    }

    /* Forward scrolling: what has scrolled off the left is gone, and the
       edge of the screen is a wall, exactly as in the game this copies. */
    if (level->scroll == GRAPPLE_PLATFORMER_SCROLL_FORWARD)
    {
        const float edge = level->camera.bounds.x + p->width * 0.5f;
        if (p->x < edge)
        {
            p->x = edge;
            if (p->vx < 0.0f)
            {
                p->vx = 0.0f;
            }
        }
    }

    /* Out of the bottom of the level: back to the start. A game that wants
       a death animation reads the event and takes over. */
    const float floor_y = (float)(level->height * level->tile);
    if (p->y - p->height > floor_y + (float)level->tile * 2.0f)
    {
        p->fell = true;
        Grapple_PlatformerPlayerRespawnAtStart(level);
        return;
    }

    if (!p->grounded)
    {
        SetState(p, (p->vy < 0.0f) ? GRAPPLE_PLAYER_JUMP : GRAPPLE_PLAYER_FALL);
    }
    else if (SDL_fabsf(p->vx) < 1.0f)
    {
        SetState(p, GRAPPLE_PLAYER_IDLE);
    }
    else if (SDL_fabsf(p->vx) > t[TUNE_WALK_SPEED] + 1.0f)
    {
        SetState(p, GRAPPLE_PLAYER_RUN);
    }
    else
    {
        SetState(p, GRAPPLE_PLAYER_WALK);
    }

    Grapple_Actor *actor = Grapple_ActorGet(level->engine, p->id);
    if (actor != NULL)
    {
        Grapple_ActorSetPosition(actor, p->x, p->y);
        Grapple_Sprite *sprite = Grapple_ActorSprite(actor);
        if (sprite != NULL)
        {
            sprite->flip = (p->facing < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        }
    }
}

/* --- public --------------------------------------------------------------- */

Grapple_ActorId Grapple_PlatformerCreatePlayer(Grapple_Platformer *level, int tile_x, int tile_y)
{
    if (level == NULL)
    {
        SDL_InvalidParamError("level");
        return GRAPPLE_ACTOR_NONE;
    }
    Grapple_PlatformerRemovePlayer(level);
    PlatformerPlayer *p = &level->player;
    const float tile = (float)level->tile;
    p->x = ((float)tile_x + 0.5f) * tile;
    p->y = (float)(tile_y + 1) * tile;
    p->start_x = p->x;
    p->start_y = p->y;
    p->vx = 0.0f;
    p->vy = 0.0f;
    p->grounded = false;
    p->state = GRAPPLE_PLAYER_IDLE;
    p->facing = 1;
    p->exists = true;
    if (!PlatformerPlayerSpawnActor(level))
    {
        p->exists = false;
        return GRAPPLE_ACTOR_NONE;
    }
    level->camera_snapped = false;
    return p->id;
}

Grapple_ActorId Grapple_PlatformerPlayer(Grapple_Platformer *level)
{
    return (level != NULL && level->player.exists) ? level->player.id : GRAPPLE_ACTOR_NONE;
}

bool Grapple_PlatformerHasPlayer(Grapple_Platformer *level)
{
    return level != NULL && level->player.exists;
}

void Grapple_PlatformerRemovePlayer(Grapple_Platformer *level)
{
    if (level == NULL || !level->player.exists)
    {
        return;
    }
    Grapple_Actor *actor = Grapple_ActorGet(level->engine, level->player.id);
    if (actor != NULL)
    {
        /* Cut the actor's link first: destruction is deferred, and its
           fixed update must not run against a player that is gone. */
        Grapple_Platformer **slot = (Grapple_Platformer **)Grapple_ActorState(actor);
        if (slot != NULL)
        {
            *slot = NULL;
        }
        Grapple_ActorDestroy(level->engine, level->player.id);
    }
    level->player.exists = false;
    level->player.id = GRAPPLE_ACTOR_NONE;
}

void Grapple_PlatformerSetPlayerSize(Grapple_Platformer *level, float width, float height)
{
    if (level == NULL || width <= 0.0f || height <= 0.0f)
    {
        return;
    }
    level->player.width = width;
    level->player.height = height;
    if (level->player.exists)
    {
        PlatformerPlayerSyncSprite(level);
    }
}

void Grapple_PlatformerPlayerSize(Grapple_Platformer *level, float *width, float *height)
{
    if (width != NULL)
    {
        *width = (level != NULL) ? level->player.width : 0.0f;
    }
    if (height != NULL)
    {
        *height = (level != NULL) ? level->player.height : 0.0f;
    }
}

void Grapple_PlatformerSetPlayerColor(Grapple_Platformer *level, float r, float g, float b, float a)
{
    if (level == NULL)
    {
        return;
    }
    level->player.color = (SDL_FColor){r, g, b, a};
    if (level->player.exists)
    {
        PlatformerPlayerSyncSprite(level);
    }
}

bool Grapple_PlatformerSetPlayerTuning(Grapple_Platformer *level, const char *key, float value)
{
    if (level == NULL)
    {
        return SDL_InvalidParamError("level");
    }
    PlatformerPlayer *p = &level->player;
    if (key != NULL && SDL_strcmp(key, "jump_height") == 0)
    {
        /* Height under the held-button gravity: v² = 2 g h. */
        if (value < 0.0f)
        {
            return SDL_SetError("jump_height cannot be negative");
        }
        p->tuning[TUNE_JUMP_SPEED] = SDL_sqrtf(2.0f * p->tuning[TUNE_JUMP_GRAVITY] * value);
        return true;
    }
    const int index = TuningIndex(key);
    if (index < 0)
    {
        return SDL_SetError("unknown player tuning '%s'", key ? key : "(null)");
    }
    p->tuning[index] = value;
    return true;
}

float Grapple_PlatformerPlayerTuning(Grapple_Platformer *level, const char *key)
{
    if (level == NULL)
    {
        return 0.0f;
    }
    const PlatformerPlayer *p = &level->player;
    if (key != NULL && SDL_strcmp(key, "jump_height") == 0)
    {
        const float g = p->tuning[TUNE_JUMP_GRAVITY];
        const float v = p->tuning[TUNE_JUMP_SPEED];
        return (g > 0.0f) ? (v * v) / (2.0f * g) : 0.0f;
    }
    const int index = TuningIndex(key);
    if (index < 0)
    {
        SDL_SetError("unknown player tuning '%s'", key ? key : "(null)");
        return 0.0f;
    }
    return p->tuning[index];
}

int Grapple_PlatformerPlayerTuningCount(void)
{
    return TUNE_COUNT + 1; /* the keys, plus jump_height */
}

const char *Grapple_PlatformerPlayerTuningKey(int index)
{
    if (index >= 0 && index < TUNE_COUNT)
    {
        return kTuningKeys[index];
    }
    if (index == TUNE_COUNT)
    {
        return "jump_height";
    }
    return NULL;
}

Grapple_PlayerState Grapple_PlatformerPlayerState(Grapple_Platformer *level)
{
    return (level != NULL) ? level->player.state : GRAPPLE_PLAYER_IDLE;
}

const char *Grapple_PlayerStateName(Grapple_PlayerState state)
{
    if ((int)state < 0 || state >= GRAPPLE_PLAYER_STATE_COUNT)
    {
        return "unknown";
    }
    return kStateNames[state];
}

const char *Grapple_PlatformerPlayerStateName(Grapple_Platformer *level)
{
    return Grapple_PlayerStateName(Grapple_PlatformerPlayerState(level));
}

bool Grapple_PlatformerPlayerStateChanged(Grapple_Platformer *level)
{
    return level != NULL && level->player.state_changed;
}

bool Grapple_PlatformerPlayerLanded(Grapple_Platformer *level)
{
    return level != NULL && level->player.landed;
}

bool Grapple_PlatformerPlayerJumped(Grapple_Platformer *level)
{
    return level != NULL && level->player.jumped;
}

bool Grapple_PlatformerPlayerBumped(Grapple_Platformer *level)
{
    return level != NULL && level->player.bumped;
}

bool Grapple_PlatformerPlayerFell(Grapple_Platformer *level)
{
    return level != NULL && level->player.fell;
}

bool Grapple_PlatformerPlayerGrounded(Grapple_Platformer *level)
{
    return level != NULL && level->player.grounded;
}

int Grapple_PlatformerPlayerFacing(Grapple_Platformer *level)
{
    return (level != NULL) ? level->player.facing : 1;
}

void Grapple_PlatformerPlayerPosition(Grapple_Platformer *level, float *x, float *y)
{
    if (x != NULL)
    {
        *x = (level != NULL) ? level->player.x : 0.0f;
    }
    if (y != NULL)
    {
        *y = (level != NULL) ? level->player.y : 0.0f;
    }
}

void Grapple_PlatformerPlayerVelocity(Grapple_Platformer *level, float *vx, float *vy)
{
    if (vx != NULL)
    {
        *vx = (level != NULL) ? level->player.vx : 0.0f;
    }
    if (vy != NULL)
    {
        *vy = (level != NULL) ? level->player.vy : 0.0f;
    }
}

void Grapple_PlatformerPlayerRespawn(Grapple_Platformer *level, float x, float y)
{
    if (level == NULL)
    {
        return;
    }
    PlatformerPlayer *p = &level->player;
    p->x = x;
    p->y = y;
    p->vx = 0.0f;
    p->vy = 0.0f;
    p->grounded = false;
    p->rising = false;
    p->coyote = 0.0f;
    p->buffer = 0.0f;
    Grapple_Actor *actor = Grapple_ActorGet(level->engine, p->id);
    if (actor != NULL)
    {
        Grapple_ActorTeleport(actor, x, y); /* no smear across the screen */
    }
    level->camera_snapped = false;
}

void Grapple_PlatformerPlayerRespawnAtStart(Grapple_Platformer *level)
{
    if (level != NULL)
    {
        Grapple_PlatformerPlayerRespawn(level, level->player.start_x, level->player.start_y);
    }
}

void Grapple_PlatformerSetPlayerPaused(Grapple_Platformer *level, bool paused)
{
    if (level == NULL)
    {
        return;
    }
    PlatformerPlayer *p = &level->player;
    p->paused = paused;
    if (paused)
    {
        SetState(p, GRAPPLE_PLAYER_PAUSED);
    }
    else if (p->state == GRAPPLE_PLAYER_PAUSED)
    {
        SetState(p, p->grounded ? GRAPPLE_PLAYER_IDLE : GRAPPLE_PLAYER_FALL);
    }
}

bool Grapple_PlatformerPlayerPaused(Grapple_Platformer *level)
{
    return level != NULL && level->player.paused;
}
