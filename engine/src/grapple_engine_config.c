/*
 * grapple_engine_config.c — heap builders for the plain-struct arguments.
 * Original Grapple code (zlib).
 *
 * Mechanical by design. The value is not in the code but in the shape: a
 * create/destroy pair plus scalar setters is exactly what the binding
 * generator already recognises as an owned resource, so every one of these
 * becomes a GC-managed handle in Lua and Ruby without a line of generator
 * work or hand-written marshalling.
 */
#include "engine_internal.h"

#include <grapple/engine_config.h>

/* --- engine config -------------------------------------------------------- */

/* The title and media path are borrowed pointers in the plain struct, which
   is right for a C caller holding a literal. A script's string may be
   collected the moment the setter returns, so the builder owns copies and
   frees them with itself. */
struct ConfigStrings
{
    char *title;
    char *media_path;
    Grapple_GraphicsSettings graphics;
    bool has_graphics;
};

/* The strings live immediately after the config, so one allocation frees
   everything and Grapple_ConfigDestroy stays a single SDL_free. */
typedef struct ConfigBlock
{
    Grapple_EngineConfig config;
    struct ConfigStrings owned;
} ConfigBlock;

static ConfigBlock *Block(Grapple_EngineConfig *config)
{
    return (ConfigBlock *)config;
}

Grapple_EngineConfig *Grapple_ConfigCreate(void)
{
    ConfigBlock *block = (ConfigBlock *)SDL_calloc(1, sizeof(ConfigBlock));
    return (block != NULL) ? &block->config : NULL;
}

void Grapple_ConfigDestroy(Grapple_EngineConfig *config)
{
    if (config == NULL)
    {
        return;
    }
    ConfigBlock *block = Block(config);
    SDL_free(block->owned.title);
    SDL_free(block->owned.media_path);
    SDL_free(block);
}

static void ReplaceString(char **slot, const char **field, const char *value)
{
    SDL_free(*slot);
    *slot = (value != NULL) ? SDL_strdup(value) : NULL;
    *field = *slot;
}

void Grapple_ConfigSetTitle(Grapple_EngineConfig *config, const char *title)
{
    if (config != NULL)
    {
        ReplaceString(&Block(config)->owned.title, &config->title, title);
    }
}

void Grapple_ConfigSetMediaPath(Grapple_EngineConfig *config, const char *path)
{
    if (config != NULL)
    {
        ReplaceString(&Block(config)->owned.media_path, &config->media_path, path);
    }
}

void Grapple_ConfigSetWindowSize(Grapple_EngineConfig *config, int width, int height)
{
    if (config != NULL)
    {
        config->window_width = width;
        config->window_height = height;
    }
}

void Grapple_ConfigSetDesignSize(Grapple_EngineConfig *config, int width, int height)
{
    if (config != NULL)
    {
        config->design_width = width;
        config->design_height = height;
    }
}

void Grapple_ConfigSetPresentation(Grapple_EngineConfig *config,
                                     Grapple_EnginePresentation presentation)
{
    if (config != NULL)
    {
        config->presentation = presentation;
    }
}

void Grapple_ConfigSetFullscreen(Grapple_EngineConfig *config, bool fullscreen)
{
    if (config != NULL)
    {
        config->fullscreen = fullscreen;
    }
}

/* The plain struct spells these as opt-outs so a zeroed config gives the
   documented defaults. A script says what it wants, so these read the
   positive way round and invert here. */
void Grapple_ConfigSetVsync(Grapple_EngineConfig *config, bool vsync)
{
    if (config != NULL)
    {
        config->no_vsync = !vsync;
    }
}

void Grapple_ConfigSetResizable(Grapple_EngineConfig *config, bool resizable)
{
    if (config != NULL)
    {
        config->fixed_size = !resizable;
    }
}

void Grapple_ConfigSetHighDpi(Grapple_EngineConfig *config, bool high_dpi)
{
    if (config != NULL)
    {
        config->low_dpi = !high_dpi;
    }
}

void Grapple_ConfigSetAutoMount(Grapple_EngineConfig *config, bool enabled)
{
    if (config != NULL)
    {
        config->no_auto_mount = !enabled;
    }
}

void Grapple_ConfigSetMaxFps(Grapple_EngineConfig *config, int max_fps)
{
    if (config != NULL)
    {
        config->max_fps = max_fps;
    }
}

void Grapple_ConfigSetTickRate(Grapple_EngineConfig *config, int ticks_per_second)
{
    if (config != NULL)
    {
        config->tick_rate = ticks_per_second;
    }
}

void Grapple_ConfigSetBackend(Grapple_EngineConfig *config, Grapple_EngineBackend backend)
{
    if (config != NULL)
    {
        config->backend = backend;
    }
}

void Grapple_ConfigSetHeadless(Grapple_EngineConfig *config, bool headless)
{
    if (config != NULL)
    {
        config->headless = headless;
    }
}

void Grapple_ConfigSetManualClock(Grapple_EngineConfig *config, bool manual)
{
    if (config != NULL)
    {
        config->manual_clock = manual;
    }
}

void Grapple_ConfigSetGraphics(Grapple_EngineConfig *config,
                                 const Grapple_GraphicsSettings *graphics)
{
    if (config == NULL || graphics == NULL)
    {
        return;
    }
    /* Copied into the block, so the caller's settings may be a temporary —
       which they always are from a script. */
    ConfigBlock *block = Block(config);
    block->owned.graphics = *graphics;
    block->owned.has_graphics = true;
    config->graphics = &block->owned.graphics;
}

/* --- actor definitions ---------------------------------------------------- */

typedef struct ActorDefBlock
{
    Grapple_ActorDef def;
    char *type;
    char *name;
} ActorDefBlock;

static ActorDefBlock *ActorBlock(Grapple_ActorDef *def)
{
    return (ActorDefBlock *)def;
}

Grapple_ActorDef *Grapple_ActorDefCreate(void)
{
    ActorDefBlock *block = (ActorDefBlock *)SDL_calloc(1, sizeof(ActorDefBlock));
    return (block != NULL) ? &block->def : NULL;
}

void Grapple_ActorDefDestroy(Grapple_ActorDef *def)
{
    if (def == NULL)
    {
        return;
    }
    ActorDefBlock *block = ActorBlock(def);
    SDL_free(block->type);
    SDL_free(block->name);
    SDL_free(block);
}

void Grapple_ActorDefSetType(Grapple_ActorDef *def, const char *type)
{
    if (def != NULL)
    {
        ReplaceString(&ActorBlock(def)->type, &def->type, type);
    }
}

void Grapple_ActorDefSetName(Grapple_ActorDef *def, const char *name)
{
    if (def != NULL)
    {
        ReplaceString(&ActorBlock(def)->name, &def->name, name);
    }
}

void Grapple_ActorDefSetPosition(Grapple_ActorDef *def, float x, float y)
{
    if (def != NULL)
    {
        def->x = x;
        def->y = y;
    }
}

void Grapple_ActorDefSetRotation(Grapple_ActorDef *def, float degrees)
{
    if (def != NULL)
    {
        def->rotation = degrees;
    }
}

void Grapple_ActorDefSetScale(Grapple_ActorDef *def, float x, float y)
{
    if (def != NULL)
    {
        def->scale_x = x;
        def->scale_y = y;
    }
}

void Grapple_ActorDefSetTags(Grapple_ActorDef *def, Grapple_ActorTags tags)
{
    if (def != NULL)
    {
        def->tags = tags;
    }
}

void Grapple_ActorDefSetParent(Grapple_ActorDef *def, Grapple_ActorId parent)
{
    if (def != NULL)
    {
        def->parent = parent;
    }
}

void Grapple_ActorDefSetStateSize(Grapple_ActorDef *def, int size)
{
    if (def != NULL && size >= 0)
    {
        def->state_size = (size_t)size;
    }
}

/* --- sprites, bodies, lights ---------------------------------------------- */

/* Each starts from its module's default rather than zero: a zeroed sprite is
   invisible and transparent, a zeroed body has no size and no density. */

Grapple_Sprite *Grapple_SpriteCreate(void)
{
    Grapple_Sprite *sprite = (Grapple_Sprite *)SDL_malloc(sizeof(Grapple_Sprite));
    if (sprite != NULL)
    {
        *sprite = Grapple_SpriteDefault();
    }
    return sprite;
}

void Grapple_SpriteDestroy(Grapple_Sprite *sprite)
{
    SDL_free(sprite);
}

void Grapple_SpriteSetTexture(Grapple_Sprite *sprite, SDL_Texture *texture)
{
    if (sprite != NULL)
    {
        sprite->texture = texture;
    }
}

void Grapple_SpriteSetSize(Grapple_Sprite *sprite, float width, float height)
{
    if (sprite != NULL)
    {
        sprite->width = width;
        sprite->height = height;
    }
}

void Grapple_SpriteSetSource(Grapple_Sprite *sprite, float x, float y, float w, float h)
{
    if (sprite != NULL)
    {
        sprite->source = (SDL_FRect){x, y, w, h};
    }
}

void Grapple_SpriteSetOrigin(Grapple_Sprite *sprite, float x, float y)
{
    if (sprite != NULL)
    {
        sprite->origin_x = x;
        sprite->origin_y = y;
    }
}

void Grapple_SpriteSetColor(Grapple_Sprite *sprite, float r, float g, float b, float a)
{
    if (sprite != NULL)
    {
        sprite->color = (SDL_FColor){r, g, b, a};
    }
}

void Grapple_SpriteSetLayer(Grapple_Sprite *sprite, int layer, float order)
{
    if (sprite != NULL)
    {
        sprite->layer = layer;
        sprite->order = order;
    }
}

void Grapple_SpriteSetSortByY(Grapple_Sprite *sprite, bool enabled)
{
    if (sprite != NULL)
    {
        sprite->sort_by_y = enabled;
    }
}

void Grapple_SpriteSetScreenSpace(Grapple_Sprite *sprite, bool enabled)
{
    if (sprite != NULL)
    {
        sprite->screen_space = enabled;
    }
}

void Grapple_SpriteSetVisible(Grapple_Sprite *sprite, bool visible)
{
    if (sprite != NULL)
    {
        sprite->visible = visible;
    }
}

Grapple_BodyDef *Grapple_BodyDefCreate(void)
{
    Grapple_BodyDef *def = (Grapple_BodyDef *)SDL_malloc(sizeof(Grapple_BodyDef));
    if (def != NULL)
    {
        *def = Grapple_BodyDefault();
    }
    return def;
}

void Grapple_BodyDefDestroy(Grapple_BodyDef *def)
{
    SDL_free(def);
}

void Grapple_BodyDefSetType(Grapple_BodyDef *def, Grapple_BodyType type)
{
    if (def != NULL)
    {
        def->type = type;
    }
}

void Grapple_BodyDefSetShape(Grapple_BodyDef *def, Grapple_ShapeType shape)
{
    if (def != NULL)
    {
        def->shape = shape;
    }
}

void Grapple_BodyDefSetSize(Grapple_BodyDef *def, float width, float height)
{
    if (def != NULL)
    {
        def->width = width;
        def->height = height;
    }
}

void Grapple_BodyDefSetOffset(Grapple_BodyDef *def, float x, float y)
{
    if (def != NULL)
    {
        def->offset_x = x;
        def->offset_y = y;
    }
}

void Grapple_BodyDefSetMaterial(Grapple_BodyDef *def, float density, float friction,
                                  float restitution)
{
    if (def != NULL)
    {
        def->density = density;
        def->friction = friction;
        def->restitution = restitution;
    }
}

void Grapple_BodyDefSetFixedRotation(Grapple_BodyDef *def, bool fixed)
{
    if (def != NULL)
    {
        def->fixed_rotation = fixed;
    }
}

void Grapple_BodyDefSetSensor(Grapple_BodyDef *def, bool sensor)
{
    if (def != NULL)
    {
        def->sensor = sensor;
    }
}

void Grapple_BodyDefSetGravityScale(Grapple_BodyDef *def, float scale)
{
    if (def != NULL)
    {
        def->gravity_scale = scale;
    }
}

void Grapple_BodyDefSetFilter(Grapple_BodyDef *def, Uint32 category, Uint32 collides_with)
{
    if (def != NULL)
    {
        def->category = category;
        def->collides_with = collides_with;
    }
}

void Grapple_BodyDefSetBullet(Grapple_BodyDef *def, bool bullet)
{
    if (def != NULL)
    {
        def->bullet = bullet;
    }
}

Grapple_LightDef *Grapple_LightDefCreate(void)
{
    Grapple_LightDef *def = (Grapple_LightDef *)SDL_malloc(sizeof(Grapple_LightDef));
    if (def != NULL)
    {
        *def = Grapple_LightDefault();
    }
    return def;
}

void Grapple_LightDefDestroy(Grapple_LightDef *def)
{
    SDL_free(def);
}

void Grapple_LightDefSetRadius(Grapple_LightDef *def, float radius)
{
    if (def != NULL)
    {
        def->radius = radius;
    }
}

void Grapple_LightDefSetColor(Grapple_LightDef *def, float r, float g, float b, float a)
{
    if (def != NULL)
    {
        def->color = (SDL_FColor){r, g, b, a};
    }
}

void Grapple_LightDefSetOffset(Grapple_LightDef *def, float x, float y)
{
    if (def != NULL)
    {
        def->offset_x = x;
        def->offset_y = y;
    }
}

void Grapple_LightDefSetCone(Grapple_LightDef *def, float direction, float width)
{
    if (def != NULL)
    {
        def->cone_direction = direction;
        def->cone_width = width;
    }
}

void Grapple_LightDefSetFlicker(Grapple_LightDef *def, float flicker)
{
    if (def != NULL)
    {
        def->flicker = flicker;
    }
}

void Grapple_LightDefSetShadows(Grapple_LightDef *def, bool casts_shadows)
{
    if (def != NULL)
    {
        def->no_shadows = !casts_shadows;
    }
}

/* --- SDL events ------------------------------------------------------------ */

/* SDL_PollEvent fills a caller-allocated event; in C that is a local, which
   a script cannot make. One allocation plus accessors turns the whole event
   loop from "bound but uncallable" into something a script can actually
   write. */
SDL_Event *Grapple_EventCreate(void)
{
    return (SDL_Event *)SDL_calloc(1, sizeof(SDL_Event));
}

void Grapple_EventDestroy(SDL_Event *event)
{
    SDL_free(event);
}

Uint32 Grapple_EventType(SDL_Event *event)
{
    return (event != NULL) ? event->type : 0;
}

void Grapple_EventSetType(SDL_Event *event, Uint32 type)
{
    if (event != NULL)
    {
        /* Zero the rest: a reused event still holds the previous type's
           arm of the union, and pushing that would deliver a quit event
           carrying somebody's mouse coordinates. */
        SDL_zerop(event);
        event->type = type;
    }
}

Uint32 Grapple_EventWindowId(SDL_Event *event)
{
    if (event == NULL)
    {
        return 0;
    }
    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        return event->key.windowID;
    case SDL_EVENT_MOUSE_MOTION:
        return event->motion.windowID;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return event->button.windowID;
    case SDL_EVENT_MOUSE_WHEEL:
        return event->wheel.windowID;
    case SDL_EVENT_TEXT_INPUT:
        return event->text.windowID;
    default:
        /* Window events share a layout, and every one of them is in the
           window range, so this covers all of them at once. */
        if (event->type >= SDL_EVENT_WINDOW_FIRST && event->type <= SDL_EVENT_WINDOW_LAST)
        {
            return event->window.windowID;
        }
        return 0;
    }
}

/* Each accessor answers for the events it applies to and returns a neutral
   value otherwise, so a script may read a field without first checking the
   type — reading the wrong arm of a union is the classic way to get
   plausible nonsense out of an event. */
int Grapple_EventKeyScancode(SDL_Event *event)
{
    if (event == NULL || (event->type != SDL_EVENT_KEY_DOWN && event->type != SDL_EVENT_KEY_UP))
    {
        return SDL_SCANCODE_UNKNOWN;
    }
    return (int)event->key.scancode;
}

bool Grapple_EventKeyRepeat(SDL_Event *event)
{
    return event != NULL && event->type == SDL_EVENT_KEY_DOWN && event->key.repeat;
}

Uint16 Grapple_EventKeyModifiers(SDL_Event *event)
{
    if (event == NULL || (event->type != SDL_EVENT_KEY_DOWN && event->type != SDL_EVENT_KEY_UP))
    {
        return 0;
    }
    return event->key.mod;
}

float Grapple_EventMouseX(SDL_Event *event)
{
    if (event == NULL)
    {
        return 0.0f;
    }
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        return event->motion.x;
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        return event->button.x;
    }
    return 0.0f;
}

float Grapple_EventMouseY(SDL_Event *event)
{
    if (event == NULL)
    {
        return 0.0f;
    }
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        return event->motion.y;
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        return event->button.y;
    }
    return 0.0f;
}

float Grapple_EventMouseDeltaX(SDL_Event *event)
{
    return (event != NULL && event->type == SDL_EVENT_MOUSE_MOTION) ? event->motion.xrel : 0.0f;
}

float Grapple_EventMouseDeltaY(SDL_Event *event)
{
    return (event != NULL && event->type == SDL_EVENT_MOUSE_MOTION) ? event->motion.yrel : 0.0f;
}

int Grapple_EventMouseButton(SDL_Event *event)
{
    if (event == NULL ||
        (event->type != SDL_EVENT_MOUSE_BUTTON_DOWN && event->type != SDL_EVENT_MOUSE_BUTTON_UP))
    {
        return 0;
    }
    return event->button.button;
}

float Grapple_EventWheelX(SDL_Event *event)
{
    return (event != NULL && event->type == SDL_EVENT_MOUSE_WHEEL) ? event->wheel.x : 0.0f;
}

float Grapple_EventWheelY(SDL_Event *event)
{
    return (event != NULL && event->type == SDL_EVENT_MOUSE_WHEEL) ? event->wheel.y : 0.0f;
}

Sint32 Grapple_EventGamepadWhich(SDL_Event *event)
{
    if (event == NULL)
    {
        return 0;
    }
    switch (event->type)
    {
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        return (Sint32)event->gbutton.which;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        return (Sint32)event->gaxis.which;
    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
        return (Sint32)event->gdevice.which;
    default:
        return 0;
    }
}

int Grapple_EventGamepadButton(SDL_Event *event)
{
    if (event == NULL || (event->type != SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
                          event->type != SDL_EVENT_GAMEPAD_BUTTON_UP))
    {
        return -1;
    }
    return event->gbutton.button;
}

int Grapple_EventGamepadAxis(SDL_Event *event)
{
    if (event == NULL || event->type != SDL_EVENT_GAMEPAD_AXIS_MOTION)
    {
        return -1;
    }
    return event->gaxis.axis;
}

float Grapple_EventGamepadAxisValue(SDL_Event *event)
{
    if (event == NULL || event->type != SDL_EVENT_GAMEPAD_AXIS_MOTION)
    {
        return 0.0f;
    }
    /* Normalised, because a script comparing against 32767 is a script
       that will be wrong on the next controller. */
    return (float)event->gaxis.value / 32767.0f;
}

float Grapple_EventTouchX(SDL_Event *event)
{
    if (event == NULL || (event->type != SDL_EVENT_FINGER_DOWN &&
                          event->type != SDL_EVENT_FINGER_UP &&
                          event->type != SDL_EVENT_FINGER_MOTION))
    {
        return 0.0f;
    }
    return event->tfinger.x;
}

float Grapple_EventTouchY(SDL_Event *event)
{
    if (event == NULL || (event->type != SDL_EVENT_FINGER_DOWN &&
                          event->type != SDL_EVENT_FINGER_UP &&
                          event->type != SDL_EVENT_FINGER_MOTION))
    {
        return 0.0f;
    }
    return event->tfinger.y;
}

const char *Grapple_EventText(SDL_Event *event)
{
    if (event == NULL || event->type != SDL_EVENT_TEXT_INPUT || event->text.text == NULL)
    {
        return "";
    }
    return event->text.text;
}

/* --- cameras --------------------------------------------------------------- */

Grapple_Camera *Grapple_CameraCreate(Grapple_Engine *engine)
{
    Grapple_Camera *camera = (Grapple_Camera *)SDL_malloc(sizeof(Grapple_Camera));
    if (camera != NULL)
    {
        Grapple_CameraInit(camera, engine);
    }
    return camera;
}

void Grapple_CameraDestroy(Grapple_Camera *camera)
{
    SDL_free(camera);
}

void Grapple_CameraSetZoom(Grapple_Camera *camera, float zoom)
{
    if (camera != NULL)
    {
        camera->zoom = zoom;
    }
}

void Grapple_CameraSetSmoothing(Grapple_Camera *camera, float seconds)
{
    if (camera != NULL)
    {
        camera->smoothing = seconds;
    }
}

void Grapple_CameraSetDeadzone(Grapple_Camera *camera, float width, float height)
{
    if (camera != NULL)
    {
        camera->deadzone_w = width;
        camera->deadzone_h = height;
    }
}

void Grapple_CameraSetBounds(Grapple_Camera *camera, float x, float y, float w, float h)
{
    if (camera != NULL)
    {
        camera->bounds = (SDL_FRect){x, y, w, h};
    }
}

void Grapple_CameraSetViewport(Grapple_Camera *camera, float x, float y, float w, float h)
{
    if (camera != NULL)
    {
        camera->viewport = (SDL_FRect){x, y, w, h};
    }
}

float Grapple_CameraX(Grapple_Camera *camera)
{
    return (camera != NULL) ? camera->x : 0.0f;
}

float Grapple_CameraY(Grapple_Camera *camera)
{
    return (camera != NULL) ? camera->y : 0.0f;
}
