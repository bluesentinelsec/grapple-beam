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
    "walk_speed",     "run_speed",         "accel",
    "decel",          "skid_decel",        "air_accel",
    "air_decel",      "jump_speed",        "run_jump_bonus",
    "jump_gravity",   "fall_gravity",      "max_fall",
    "coyote_time",    "jump_buffer",       "wall_slide_speed",
    "wall_jump_x",    "wall_jump_y",       "wall_coyote_time",
    "wall_jump_lock", "wall_return_accel", "wall_return_time",
    "slope_gravity",  "loop_min_speed",    "momentum_decel",
};

/* Defaults in *tiles* per second (and per second squared), so a level with
   32-pixel tiles feels the same as one with 16. They are Super Mario Bros.'
   numbers: 1.5 px/frame walking, 2.5 running, a 4 px/frame take-off, gravity
   of 0.125 px/frame² while the button is held and 0.4375 once it is not,
   all at 60 frames a second and 16-pixel tiles. */
static const float kTuningDefaultsInTiles[TUNE_COUNT] = {
    [TUNE_WALK_SPEED] = 6.0f,
    [TUNE_RUN_SPEED] = 10.0f,
    [TUNE_ACCEL] = 25.0f,
    [TUNE_DECEL] = 37.5f,
    [TUNE_SKID_DECEL] = 56.0f,
    [TUNE_AIR_ACCEL] = 18.75f,
    [TUNE_AIR_DECEL] = 0.0f,
    [TUNE_JUMP_SPEED] = 15.0f,
    [TUNE_RUN_JUMP_BONUS] = 2.0f,
    [TUNE_JUMP_GRAVITY] = 28.125f,
    [TUNE_FALL_GRAVITY] = 98.0f,
    [TUNE_MAX_FALL] = 17.0f,
    /* The wall jump: slide at a quarter of terminal velocity, kick off a
       little slower than a ground jump and well out from the wall. */
    [TUNE_WALL_SLIDE_SPEED] = 4.0f,
    [TUNE_WALL_JUMP_X] = 6.0f,
    [TUNE_WALL_JUMP_Y] = 15.0f,
    /* Steering back toward the wall just left is fast — Super Mario Wonder
       lets a single wall be climbed kick after kick — so a player holding
       into the wall is back on it while still above where they kicked. */
    [TUNE_WALL_RETURN_ACCEL] = 75.0f,
    /* Slopes and loops: the pull along a surface, and how slow a run up a
       wall or across a ceiling can get before falling off. */
    [TUNE_SLOPE_GRAVITY] = 28.125f,
    [TUNE_LOOP_MIN_SPEED] = 6.0f,
    /* How quickly speed gained on a slope bleeds off on the flat: gently,
       so a ramp's momentum carries into a loop a few tiles on. */
    [TUNE_MOMENTUM_DECEL] = 9.375f,
};

static const char *const kStateNames[GRAPPLE_PLAYER_STATE_COUNT] = {
    "idle", "walk", "run", "jump", "fall", "wall_slide", "paused",
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
    p->tuning[TUNE_WALL_COYOTE_TIME] = 0.10f;
    p->tuning[TUNE_WALL_JUMP_LOCK] = 0.05f;
    p->tuning[TUNE_WALL_RETURN_TIME] = 0.6f;
    /* A little under a tile wide and just under two tall: SMB's big Mario
       is drawn 16x32 and collides narrower, which is what keeps him from
       catching on the lip of a one-tile gap he visually fits through. */
    p->width = tile * 0.875f;
    p->height = tile * 1.875f;
    p->color = (SDL_FColor){0.90f, 0.25f, 0.20f, 1.0f};
    p->facing = 1;
    p->state = GRAPPLE_PLAYER_IDLE;
    p->layer = PLATFORMER_LAYER_A;
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

/* --- collision ----------------------------------------------------------------
 *
 * Everything below tests the world pixel by pixel through
 * PlatformerSolidPixel, which is what lets a cell be a slope or a piece of a
 * loop as easily as a block. Movement is in steps of at most one pixel, so
 * nothing is ever inside anything: a wall stops the player at its face, a
 * floor at its top, a slope where the line is.
 */

#define NO_GROUND (-1.0e9f)

static bool Solid(Grapple_Platformer *level, const PlatformerPlayer *p, float x, float y)
{
    return PlatformerSolidPixel(level, (int)SDL_floorf(x), (int)SDL_floorf(y), p->layer);
}

static bool InsideLevel(const Grapple_Platformer *level, float x)
{
    return x >= 0.0f && x < (float)(level->width * level->tile);
}

/* Airborne: move horizontally by dx, one pixel at a time, stopping at the
   first solid the leading column of the box would enter. */
static bool MoveX(Grapple_Platformer *level, PlatformerPlayer *p, float dx)
{
    if (dx == 0.0f)
    {
        return false;
    }
    const float s = (dx > 0.0f) ? 1.0f : -1.0f;
    const float half = p->width * 0.5f;
    float remaining = SDL_fabsf(dx);
    while (remaining > 0.0f)
    {
        const float piece = SDL_min(1.0f, remaining);
        const float nx = p->x + s * piece;
        const float edge = (s > 0.0f) ? nx + half - EDGE_EPSILON : nx - half + EDGE_EPSILON;
        const float top = p->y - p->height + EDGE_EPSILON;
        const float bottom = p->y - EDGE_EPSILON;
        bool blocked = false;
        for (float yy = top; !blocked; yy += 1.0f)
        {
            if (yy > bottom)
            {
                yy = bottom;
            }
            blocked = Solid(level, p, edge, yy);
            if (yy >= bottom)
            {
                break;
            }
        }
        if (blocked)
        {
            const float face = SDL_floorf(edge);
            p->x = (s > 0.0f) ? face - half : face + 1.0f + half;
            if (InsideLevel(level, edge))
            {
                /* The invisible wall at the edge of the level stops the
                   player but is nothing to kick off. */
                p->wall = (s > 0.0f) ? 1 : -1;
            }
            return true;
        }
        p->x = nx;
        remaining -= piece;
    }
    return false;
}

/* Airborne: move vertically by dy, one pixel at a time. Landing sets
   grounded; hitting a ceiling sets bumped. The feet's three columns are
   what land, so a slope catches the corner and the sensors sort it out. */
static bool MoveY(Grapple_Platformer *level, PlatformerPlayer *p, float dy)
{
    if (dy == 0.0f)
    {
        return false;
    }
    const float s = (dy > 0.0f) ? 1.0f : -1.0f;
    const float half = p->width * 0.5f;
    const float columns[3] = {p->x - half + EDGE_EPSILON, p->x, p->x + half - EDGE_EPSILON};
    const float old_bottom = p->y;
    float remaining = SDL_fabsf(dy);
    while (remaining > 0.0f)
    {
        const float piece = SDL_min(1.0f, remaining);
        const float ny = p->y + s * piece;
        const float edge = (s > 0.0f) ? ny - EDGE_EPSILON : ny - p->height + EDGE_EPSILON;
        bool blocked = false;
        int column = 1;
        for (int i = 0; i < 3 && !blocked; ++i)
        {
            blocked = Solid(level, p, columns[i], edge);
            if (!blocked && s > 0.0f)
            {
                blocked = PlatformerLedgePixel(level, (int)SDL_floorf(columns[i]),
                                               (int)SDL_floorf(edge), old_bottom);
            }
            column = i;
        }
        if (blocked)
        {
            const float face = SDL_floorf(edge);
            if (s > 0.0f)
            {
                p->y = face;
                p->grounded = true;
                p->land_column = column;
            }
            else
            {
                p->y = face + 1.0f + p->height;
                p->bumped = true;
            }
            return true;
        }
        p->y = ny;
        remaining -= piece;
    }
    return false;
}

/* --- ground sensors -----------------------------------------------------------
 *
 * On the ground the player is a point — the feet — with a direction along
 * the surface, and two sensors either side of the feet that look "down" into
 * the ground, where down is whichever of the four directions the surface
 * normal is closest to. That is Sonic's model, and it is what lets the same
 * code walk a floor, climb a slope, and run up a wall and across a ceiling.
 */

/* The direction into the ground for the current angle, and which axis the
   sensors spread along (the other one). */
static void GroundDirection(float angle, int *dx, int *dy)
{
    const float deg = angle * 180.0f / SDL_PI_F;
    if (deg > -45.0f && deg <= 45.0f)
    {
        *dx = 0;
        *dy = 1; /* floor */
    }
    else if (deg > -135.0f && deg <= -45.0f)
    {
        *dx = 1;
        *dy = 0; /* running up a wall on the right */
    }
    else if (deg > 45.0f && deg <= 135.0f)
    {
        *dx = -1;
        *dy = 0; /* running down a wall on the left */
    }
    else
    {
        *dx = 0;
        *dy = -1; /* a ceiling */
    }
}

/* Look along (dx, dy) from the boundary `f` (the feet's coordinate on that
   axis) at the sensor's position `c` on the other axis. Returns where the
   surface boundary is on the probe axis — below the feet if the ground is
   further away, above if the feet are inside it — or NO_GROUND. */
static float Probe(Grapple_Platformer *level, const PlatformerPlayer *p, int dx, int dy, float f,
                   float c, int max_in, int max_out)
{
    const int sign = (dx + dy > 0) ? 1 : -1;
    /* The pixel just ahead of the boundary. */
    const int first = (sign > 0) ? (int)SDL_floorf(f) : (int)SDL_ceilf(f) - 1;
    const int cc = (int)SDL_floorf(c);
    const bool vertical = dy != 0;
    /* Looking down, a ledge's top row is ground too — from above it. */
#define PIXEL_SOLID(k)                                                                             \
    (vertical ? (PlatformerSolidPixel(level, cc, first + (k) * sign, p->layer) ||                  \
                 (sign > 0 && (k) >= 0 && PlatformerLedgePixel(level, cc, first + (k) * sign, f))) \
              : PlatformerSolidPixel(level, first + (k) * sign, cc, p->layer))
    if (PIXEL_SOLID(0))
    {
        /* Inside the ground: back out until a free pixel, then stand on
           the last solid one's outer edge. */
        for (int k = 1; k <= max_out; ++k)
        {
            if (!PIXEL_SOLID(-k))
            {
                const int last_solid = first - (k - 1) * sign;
                return (sign > 0) ? (float)last_solid : (float)(last_solid + 1);
            }
        }
        return NO_GROUND;
    }
    for (int k = 1; k <= max_in; ++k)
    {
        if (PIXEL_SOLID(k))
        {
            const int hit = first + k * sign;
            return (sign > 0) ? (float)hit : (float)(hit + 1);
        }
    }
#undef PIXEL_SOLID
    return NO_GROUND;
}

static float WrapAngle(float a)
{
    while (a > SDL_PI_F)
    {
        a -= 2.0f * SDL_PI_F;
    }
    while (a <= -SDL_PI_F)
    {
        a += 2.0f * SDL_PI_F;
    }
    return a;
}

/* Find the ground under the feet for the current angle, put the feet on it,
   and take the surface's angle from the two side sensors. The feet stand
   where the centre sensor finds ground — on a tight concave curve like the
   inside of a loop the side sensors find the surface much nearer or
   further than the centre does, and standing on either drags the feet off
   the curve — and a side sensor holds the player up only at a ledge, where
   the centre finds nothing. False if there is no ground within reach,
   which is how walking off a ledge becomes falling. */
static bool FindGround(Grapple_Platformer *level, PlatformerPlayer *p, int max_in)
{
    int dx;
    int dy;
    GroundDirection(p->angle, &dx, &dy);
    const bool vertical = dy != 0;
    const float spread = SDL_max(1.0f, SDL_min(4.0f, p->width * 0.5f - 1.0f));
    const float f = vertical ? p->y : p->x; /* the feet, on the probe axis */
    const float c = vertical ? p->x : p->y; /* the feet, on the other axis */
    const int max_out = level->tile * 2;
    const float fa = Probe(level, p, dx, dy, f, c - spread, max_in, max_out);
    const float fm = Probe(level, p, dx, dy, f, c, max_in, max_out);
    const float fb = Probe(level, p, dx, dy, f, c + spread, max_in, max_out);
    if (fa == NO_GROUND && fm == NO_GROUND && fb == NO_GROUND)
    {
        return false;
    }

    float on = fm;
    if (on == NO_GROUND)
    {
        /* Only an edge under one side: stand on it. */
        const bool toward_plus = (dx + dy) > 0;
        if (fa == NO_GROUND)
        {
            on = fb;
        }
        else if (fb == NO_GROUND)
        {
            on = fa;
        }
        else
        {
            on = toward_plus ? SDL_min(fa, fb) : SDL_max(fa, fb);
        }
    }
    if (vertical)
    {
        p->y = on;
    }
    else
    {
        p->x = on;
    }

    /* The angle, from the line between two surface points — the two sides
       if both found ground, else the centre and the side that did — kept
       pointing the way we were already going. */
    float ca = c - spread;
    float cb = c + spread;
    float sa = fa;
    float sb = fb;
    if (sa == NO_GROUND && fm != NO_GROUND)
    {
        ca = c;
        sa = fm;
    }
    if (sb == NO_GROUND && fm != NO_GROUND)
    {
        cb = c;
        sb = fm;
    }
    if (sa != NO_GROUND && sb != NO_GROUND && ca != cb)
    {
        const float ax = vertical ? ca : sa;
        const float ay = vertical ? sa : ca;
        const float bx = vertical ? cb : sb;
        const float by = vertical ? sb : cb;
        float angle = SDL_atan2f(by - ay, bx - ax);
        if (SDL_cosf(angle) * SDL_cosf(p->angle) + SDL_sinf(angle) * SDL_sinf(p->angle) < 0.0f)
        {
            angle = WrapAngle(angle + SDL_PI_F);
        }
        p->angle = angle;
    }
    return true;
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

/* Off the ground: the speed along the surface becomes a velocity. */
static void LeaveGround(PlatformerPlayer *p)
{
    p->vx = SDL_cosf(p->angle) * p->gsp;
    p->vy = SDL_sinf(p->angle) * p->gsp;
    p->grounded = false;
    p->gsp = 0.0f;
    p->angle = 0.0f;
}

/* Onto the ground: the velocity becomes a speed along the surface. */
static void LandOn(PlatformerPlayer *p)
{
    p->gsp = p->vx * SDL_cosf(p->angle) + p->vy * SDL_sinf(p->angle);
    p->grounded = true;
    p->rising = false;
}

/* A step on the ground: accelerate along the surface, feel the slope, move
   along the tangent a pixel at a time keeping the feet on the ground. */
static void GroundStep(Grapple_Platformer *level, PlatformerPlayer *p, float step, float move,
                       bool run_down)
{
    const float *t = p->tuning;
    const float top_speed = run_down ? t[TUNE_RUN_SPEED] : t[TUNE_WALK_SPEED];

    /* Gravity's pull along the surface: uphill slows, downhill speeds, and
       downhill is not capped, which is how a loop gets entered fast. */
    p->gsp += t[TUNE_SLOPE_GRAVITY] * SDL_sinf(p->angle) * step;

    /* Traction: the feet push only as hard as the ground pushes back, so
       acceleration, braking and skidding fade with the slope and are gone
       on a wall or a ceiling — which is what makes a run up a loop's wall
       a climb that slows, not a run that keeps its pace. */
    const float traction = SDL_max(0.0f, SDL_cosf(p->angle));

    if (move != 0.0f)
    {
        const bool reversing = p->gsp != 0.0f && (move > 0.0f) != (p->gsp > 0.0f);
        if (reversing)
        {
            p->gsp = Approach(p->gsp, move * top_speed, t[TUNE_SKID_DECEL] * traction * step);
        }
        else if (SDL_fabsf(p->gsp) < top_speed)
        {
            p->gsp = Approach(p->gsp, move * top_speed, t[TUNE_ACCEL] * traction * step);
        }
        else if (SDL_sinf(p->angle) * move <= 0.0f)
        {
            /* Faster than the run, thanks to a slope, and no longer going
               down one: ease back to the run rather than keep the speed
               forever — Mario's rule, not Sonic's. Downhill it is kept. */
            p->gsp = Approach(p->gsp, move * top_speed, t[TUNE_MOMENTUM_DECEL] * traction * step);
        }
    }
    else
    {
        p->gsp = Approach(p->gsp, 0.0f, t[TUNE_DECEL] * traction * step);
    }
    if (move > 0.05f)
    {
        p->facing = 1;
    }
    else if (move < -0.05f)
    {
        p->facing = -1;
    }

    /* Too slow for a wall or a ceiling: fall off it. */
    const float normal_y = -SDL_cosf(p->angle);
    if (normal_y > -0.5f && SDL_fabsf(p->gsp) < t[TUNE_LOOP_MIN_SPEED])
    {
        LeaveGround(p);
        return;
    }

    const float distance = p->gsp * step;
    const float s = (distance >= 0.0f) ? 1.0f : -1.0f;
    float remaining = SDL_fabsf(distance);
    const float half = p->width * 0.5f;
    while (remaining > 0.0f)
    {
        const float tx = SDL_cosf(p->angle);
        const float ty = SDL_sinf(p->angle);
        const float nx = SDL_sinf(p->angle);
        const float ny = -SDL_cosf(p->angle);
        /* Something ahead at mid height stops the run: a wall, a step. On
           flat ground the player is put flush against it. */
        const float piece = SDL_min(1.0f, remaining);
        const float qx = p->x + tx * s * (half + piece) + nx * p->height * 0.5f;
        const float qy = p->y + ty * s * (half + piece) + ny * p->height * 0.5f;
        if (Solid(level, p, qx, qy))
        {
            if (SDL_fabsf(ty) < 0.01f)
            {
                const float face = SDL_floorf(qx);
                const float flush = (s > 0.0f) ? face - half : face + 1.0f + half;
                if ((s > 0.0f && flush >= p->x) || (s < 0.0f && flush <= p->x))
                {
                    p->x = flush;
                }
            }
            p->gsp = 0.0f;
            break;
        }
        p->x += tx * s * piece;
        p->y += ty * s * piece;
        remaining -= piece;
        if (!FindGround(level, p, level->tile))
        {
            LeaveGround(p);
            return;
        }
    }
    if (remaining <= 0.0f && distance == 0.0f)
    {
        /* Standing still: the ground can still go away underneath. */
        if (!FindGround(level, p, 2))
        {
            LeaveGround(p);
            return;
        }
    }
    p->vx = SDL_cosf(p->angle) * p->gsp;
    p->vy = SDL_sinf(p->angle) * p->gsp;
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
    const float x0 = p->x;
    const float y0 = p->y;
    const bool was_grounded = p->grounded;

    /* Jump timers. Coyote time lets a jump pressed just after walking off
       a ledge still happen; the buffer lets one pressed just before landing
       happen on landing. */
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
        /* Off the surface along its normal, keeping the speed along it: on
           flat ground that is straight up; on a slope, away from the slope;
           inside a loop, toward its middle. A running jump goes higher. */
        const float run_fraction = (t[TUNE_RUN_SPEED] > 0.0f)
                                       ? SDL_min(1.0f, SDL_fabsf(p->gsp) / t[TUNE_RUN_SPEED])
                                       : 0.0f;
        const float take_off = t[TUNE_JUMP_SPEED] + t[TUNE_RUN_JUMP_BONUS] * run_fraction;
        const float angle = p->grounded ? p->angle : 0.0f;
        const float along = p->grounded ? p->gsp : p->vx;
        p->vx = SDL_sinf(angle) * take_off + SDL_cosf(angle) * along;
        p->vy = -SDL_cosf(angle) * take_off + SDL_sinf(angle) * along;
        p->grounded = false;
        p->gsp = 0.0f;
        p->angle = 0.0f;
        p->rising = true;
        p->coyote = 0.0f;
        p->buffer = 0.0f;
        p->jumped = true;
    }
    else if (p->buffer > 0.0f && p->wall_coyote > 0.0f && t[TUNE_WALL_JUMP_Y] > 0.0f)
    {
        /* Off the wall: away from it and up, facing the way we go. */
        p->vx = (float)-p->last_wall * t[TUNE_WALL_JUMP_X];
        p->vy = -t[TUNE_WALL_JUMP_Y];
        p->facing = -p->last_wall;
        p->rising = true;
        p->wall_lock = t[TUNE_WALL_JUMP_LOCK];
        p->wall_return = t[TUNE_WALL_RETURN_TIME];
        p->wall_coyote = 0.0f;
        p->buffer = 0.0f;
        p->jumped = true;
    }

    bool sliding = false;
    if (p->grounded)
    {
        p->wall = 0;
        p->wall_coyote = 0.0f;
        p->wall_return = 0.0f;
        GroundStep(level, p, step, move, run_down);
    }
    else
    {
        /* In the air: steer, fall, and sweep against the world. */
        const float top_speed = run_down ? t[TUNE_RUN_SPEED] : t[TUNE_WALK_SPEED];
        const float target = move * top_speed;
        float rate;
        if (p->wall_lock > 0.0f)
        {
            /* Just kicked off a wall: the stick is ignored for a moment. */
            p->wall_lock = SDL_max(0.0f, p->wall_lock - step);
            rate = 0.0f;
        }
        else
        {
            rate = (move != 0.0f) ? t[TUNE_AIR_ACCEL] : t[TUNE_AIR_DECEL];
            if (p->wall_return > 0.0f)
            {
                p->wall_return = SDL_max(0.0f, p->wall_return - step);
                if (move * (float)p->last_wall > 0.3f)
                {
                    rate = t[TUNE_WALL_RETURN_ACCEL];
                }
            }
        }
        /* Steer toward the stick's speed, but never slow a faster launch. */
        if (SDL_fabsf(p->vx) <= top_speed || (move != 0.0f && (move > 0.0f) != (p->vx > 0.0f)))
        {
            p->vx = Approach(p->vx, target, rate * step);
        }
        else if (move == 0.0f)
        {
            p->vx = Approach(p->vx, 0.0f, rate * step);
        }
        if (p->wall_lock <= 0.0f)
        {
            if (move > 0.05f)
            {
                p->facing = 1;
            }
            else if (move < -0.05f)
            {
                p->facing = -1;
            }
        }

        /* Gravity: light while the button is held on the way up, heavy
           otherwise — the whole of the variable-height jump. */
        if (!jump_down || p->vy >= 0.0f)
        {
            p->rising = false;
        }
        const float gravity = p->rising ? t[TUNE_JUMP_GRAVITY] : t[TUNE_FALL_GRAVITY];
        p->vy = SDL_min(p->vy + gravity * step, t[TUNE_MAX_FALL]);

        p->wall = 0;
        p->bumped = false;
        if (MoveX(level, p, p->vx * step))
        {
            p->vx = 0.0f;
        }
        MoveY(level, p, p->vy * step);
        if (p->bumped && p->vy < 0.0f)
        {
            p->vy = 0.0f;
            p->rising = false;
        }
        if (p->grounded)
        {
            /* Landed: take the surface's angle and carry the speed along
               it. A corner of the box catching a steep surface with nothing
               under the feet is not a landing: slide off it sideways. */
            p->angle = 0.0f;
            if (FindGround(level, p, level->tile))
            {
                LandOn(p);
            }
            else
            {
                p->grounded = false;
                p->x += (p->land_column == 2) ? -1.0f : (p->land_column == 0) ? 1.0f : 0.0f;
            }
        }

        /* Against a wall, holding toward it: a wall slide. */
        const bool holding_wall = !p->grounded && p->wall != 0 && move * (float)p->wall > 0.3f;
        if (holding_wall)
        {
            p->last_wall = p->wall;
            p->wall_coyote = t[TUNE_WALL_COYOTE_TIME];
            if (p->vy > t[TUNE_WALL_SLIDE_SPEED])
            {
                p->vy = t[TUNE_WALL_SLIDE_SPEED];
            }
        }
        else
        {
            p->wall = 0;
            p->wall_coyote = p->grounded ? 0.0f : SDL_max(0.0f, p->wall_coyote - step);
        }
        sliding = !p->grounded && holding_wall && p->vy >= 0.0f;
    }

    if (p->grounded && !was_grounded)
    {
        p->landed = true;
    }
    if (p->grounded)
    {
        p->ground_y = p->y;
    }

    /* A loop's path swappers watch the feet cross their lines. */
    PlatformerApplySwappers(level, x0, y0, p->x, p->y, &p->layer);

    /* Forward scrolling: what has scrolled off the left is gone, and the
       edge of the screen is a wall. */
    if (level->scroll == GRAPPLE_PLATFORMER_SCROLL_FORWARD)
    {
        const float edge = level->camera.bounds.x + p->width * 0.5f;
        if (p->x < edge)
        {
            p->x = edge;
            if (p->grounded)
            {
                p->gsp = SDL_max(p->gsp, 0.0f);
            }
            else if (p->vx < 0.0f)
            {
                p->vx = 0.0f;
            }
        }
    }

    /* Out of the bottom of the level: back to the start. */
    const float floor_y = (float)(level->height * level->tile);
    if (p->y - p->height > floor_y + (float)level->tile * 2.0f)
    {
        p->fell = true;
        Grapple_PlatformerPlayerRespawnAtStart(level);
        return;
    }

    if (sliding)
    {
        SetState(p, GRAPPLE_PLAYER_WALL_SLIDE);
    }
    else if (!p->grounded)
    {
        SetState(p, (p->vy < 0.0f) ? GRAPPLE_PLAYER_JUMP : GRAPPLE_PLAYER_FALL);
    }
    else if (SDL_fabsf(p->gsp) < 1.0f)
    {
        SetState(p, GRAPPLE_PLAYER_IDLE);
    }
    else if (SDL_fabsf(p->gsp) > t[TUNE_WALK_SPEED] + 1.0f)
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
        /* Lean into the surface: the sprite turns with the ground, about
           its feet, all the way round a loop. */
        Grapple_ActorTransform local = Grapple_ActorLocal(actor);
        local.rotation = p->grounded ? p->angle * 180.0f / SDL_PI_F : 0.0f;
        Grapple_ActorSetLocal(actor, &local);
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
    p->wall = 0;
    p->wall_coyote = 0.0f;
    p->wall_lock = 0.0f;
    p->wall_return = 0.0f;
    p->angle = 0.0f;
    p->gsp = 0.0f;
    p->layer = PLATFORMER_LAYER_A;
    p->ground_y = p->y;
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

int Grapple_PlatformerPlayerWall(Grapple_Platformer *level)
{
    return (level != NULL) ? level->player.wall : 0;
}

float Grapple_PlatformerPlayerAngle(Grapple_Platformer *level)
{
    return (level != NULL && level->player.grounded) ? level->player.angle * 180.0f / SDL_PI_F
                                                     : 0.0f;
}

float Grapple_PlatformerPlayerGroundSpeed(Grapple_Platformer *level)
{
    return (level != NULL && level->player.grounded) ? level->player.gsp : 0.0f;
}

int Grapple_PlatformerPlayerLayer(Grapple_Platformer *level)
{
    return (level != NULL) ? level->player.layer : PLATFORMER_LAYER_A;
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
    p->wall = 0;
    p->wall_coyote = 0.0f;
    p->wall_lock = 0.0f;
    p->wall_return = 0.0f;
    p->angle = 0.0f;
    p->gsp = 0.0f;
    p->layer = PLATFORMER_LAYER_A;
    p->ground_y = y;
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
