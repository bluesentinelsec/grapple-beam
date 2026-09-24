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
    TUNE_COUNT
} PlatformerTuning;

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

    PlatformerSolid *solids;
    int solid_count;
    int solid_capacity;

    SDL_FColor background;
    SDL_FColor tile_colors[GRAPPLE_PLATFORMER_TILE_COUNT];
    SDL_Texture *tile_textures[GRAPPLE_PLATFORMER_TILE_COUNT];

    Grapple_Camera camera;
    Grapple_PlatformerScroll scroll;
    bool camera_snapped;

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

#endif /* GRAPPLE_PLATFORMER_INTERNAL_H */
