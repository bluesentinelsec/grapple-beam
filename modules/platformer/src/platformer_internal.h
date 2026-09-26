/*
 * platformer_internal.h — the level structure shared by the module's
 * translation units. Original grapple-beam code (zlib). Not installed.
 */
#ifndef GRAPPLE_PLATFORMER_INTERNAL_H
#define GRAPPLE_PLATFORMER_INTERNAL_H

#include <grapple/platformer.h>

/* Every tuning value the player reads, indexed so the step loop never does
   a string compare. The public API spells them as keys; kTuningKeys in
   grapple_platformer_player.c is the one place both are listed. */
typedef enum PlatformerTuning
{
    TUNE_WALK_SPEED = 0,
    TUNE_RUN_SPEED,
    TUNE_ACCEL,
    TUNE_DECEL,
    TUNE_SKID_DECEL,
    TUNE_AIR_ACCEL,
    TUNE_AIR_DECEL,
    TUNE_JUMP_SPEED,
    TUNE_RUN_JUMP_BONUS,
    TUNE_JUMP_GRAVITY,
    TUNE_FALL_GRAVITY,
    TUNE_MAX_FALL,
    TUNE_COYOTE_TIME,
    TUNE_JUMP_BUFFER,
    TUNE_WALL_SLIDE_SPEED,
    TUNE_WALL_JUMP_X,
    TUNE_WALL_JUMP_Y,
    TUNE_WALL_COYOTE_TIME,
    TUNE_WALL_JUMP_LOCK,
    TUNE_WALL_RETURN_ACCEL,
    TUNE_WALL_RETURN_TIME,
    TUNE_SLOPE_GRAVITY,
    TUNE_LOOP_MIN_SPEED,
    TUNE_MOMENTUM_DECEL,
    TUNE_COUNT
} PlatformerTuning;

#define PLATFORMER_MAX_TILE 32
#define PLATFORMER_LAYER_A 1
#define PLATFORMER_LAYER_B 2

typedef struct PlatformerMask
{
    Uint32 rows[PLATFORMER_MAX_TILE];
} PlatformerMask;

typedef struct PlatformerSwapper
{
    float x, y0, y1;
    int moving_right; /* layer to set when crossed left-to-right, 0 = keep */
    int moving_left;
} PlatformerSwapper;

typedef struct PlatformerSolid
{
    SDL_FRect rect;
    bool alive;
} PlatformerSolid;

typedef struct PlatformerPlayer
{
    Grapple_ActorId id;
    bool exists;

    /* The feet, in pixels; the box extends up by height and to each side by
       half the width. */
    float x, y;
    float vx, vy;
    float width, height;
    float start_x, start_y;
    SDL_FColor color;

    Grapple_PlayerState state;
    bool paused;
    bool grounded;
    int facing;

    float coyote;       /* seconds left in which a jump still counts */
    float buffer;       /* seconds left in which a landing still jumps */
    bool rising;        /* holding the button still shortens gravity */
    bool jump_was_down; /* edge detection, per step */

    /* On the ground the player moves along the surface: gsp is the speed
       along it, angle its direction, in radians with y down — 0 is flat
       ground going right, negative rises to the right, +-pi is a ceiling.
       The tangent is (cos a, sin a), the outward normal (sin a, -cos a). */
    float angle;
    float gsp;
    int layer; /* PLATFORMER_LAYER_A or _B: which collision layer is solid */

    int land_column;   /* which of the feet's three columns touched down last */
    int wall;          /* -1/+1: airborne and pressed against a wall that side */
    int last_wall;     /* the wall most recently slid on, for the kick */
    float wall_coyote; /* seconds left in which a wall jump still counts */
    float wall_lock;   /* seconds left with the stick ignored after a kick */
    float wall_return; /* seconds left of strong steering back to the wall left */
    float ground_y;    /* the feet's y the last time they stood on ground */

    /* Frame events: set during the steps of a frame, cleared at the first
       step of the next one. event_frame is the frame they belong to. */
    Uint64 event_frame;
    bool state_changed;
    bool landed;
    bool jumped;
    bool bumped;
    bool fell;

    float tuning[TUNE_COUNT];
} PlatformerPlayer;

struct Grapple_Platformer
{
    Grapple_Engine *engine;
    int width, height, tile;
    Uint8 *cells; /* Grapple_PlatformerTile, row-major */
    /* A cell that is not a whole block — a slope, a piece of a loop — has a
       pixel mask: one row per pixel row, bit c set where column c is solid.
       cell_mask holds 1 + the mask's index, 0 for a whole cell. */
    Uint16 *cell_mask;
    Uint8 *cell_layers; /* PLATFORMER_LAYER_* bits; 0 means both */
    PlatformerMask *masks;
    int mask_count, mask_capacity;
    /* Path swappers: crossing the vertical line x between y0 and y1 sets the
       player's layer, by direction. 0 leaves it alone. */
    PlatformerSwapper *swappers;
    int swapper_count, swapper_capacity;

    PlatformerSolid *solids;
    int solid_count;
    int solid_capacity;

    SDL_FColor background;
    SDL_FColor tile_colors[GRAPPLE_PLATFORMER_TILE_COUNT];
    SDL_Texture *tile_textures[GRAPPLE_PLATFORMER_TILE_COUNT];

    Grapple_Camera camera;
    Grapple_PlatformerScroll scroll;
    bool camera_snapped;
    float deadzone_w;             /* pixels the player may move before the view follows */
    float follow_x;               /* the deadzone's anchor: where the view follows from */
    float look_ahead;             /* pixels, in the facing direction */
    float look;                   /* where the look-ahead currently is, eased */
    float vertical_band;          /* pixels above and below the ground level before following */
    float last_cam_x, last_cam_y; /* to tell a following camera from a still one */

    char scene_name[32];
    bool attached;

    Grapple_ActionMap *actions;
    bool scripted;
    float script_move;
    bool script_jump;
    bool script_run;

    PlatformerPlayer player;
};

/* grapple_platformer_player.c */
void PlatformerPlayerInit(Grapple_Platformer *level);
void PlatformerPlayerStep(Grapple_Platformer *level, float step);
bool PlatformerPlayerSpawnActor(Grapple_Platformer *level);
void PlatformerPlayerSyncSprite(Grapple_Platformer *level);

/* grapple_platformer.c */
bool PlatformerCellBlocks(Grapple_Platformer *level, int cx, int cy, bool moving_down,
                          float old_bottom);
/* Is the pixel (px, py) solid for a player on `layer`? Whole cells, masked
   cells, pixel solids and the level's side walls; one-way ledges are not. */
bool PlatformerSolidPixel(Grapple_Platformer *level, int px, int py, int layer);
/* The one-way rule for a pixel: the top row of a ledge, landed on from above. */
bool PlatformerLedgePixel(Grapple_Platformer *level, int px, int py, float old_bottom);
/* Run every swapper against a move from (x0, y0) to (x1, y1). */
void PlatformerApplySwappers(Grapple_Platformer *level, float x0, float y0, float x1, float y1,
                             int *layer);

#endif /* GRAPPLE_PLATFORMER_INTERNAL_H */
