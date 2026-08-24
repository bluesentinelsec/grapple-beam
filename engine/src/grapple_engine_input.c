/*
 * grapple_engine_input.c — the input state table.
 * Original Grapple code (zlib).
 *
 * SDL delivers events; a game wants to ask questions. This file is the
 * table in between, and it exists because every game that works from raw
 * events ends up writing it — usually with the same two bugs.
 *
 * The first is edge timing. "Pressed" has to mean "this frame", and a frame
 * may contain several fixed simulation steps, so an edge cleared per step
 * is an input silently eaten whenever the machine runs two steps in a
 * frame. Here the edges are rebuilt once, at the top of the frame, from the
 * difference between this frame's held set and the last one — so every step
 * in the frame sees the same answer.
 *
 * The second is disconnection. A controller that goes away must read as
 * neutral, not frozen: a player whose battery dies mid-jump should land,
 * not keep holding right into a pit.
 */
#include "engine_internal.h"

#include <grapple/engine_input.h>

#define SCANCODE_COUNT 512

typedef struct GamepadSlot
{
    SDL_Gamepad *pad;
    SDL_JoystickID id;
    char name[128];

    bool buttons[GRAPPLE_PAD_BUTTON_COUNT];
    bool previous[GRAPPLE_PAD_BUTTON_COUNT];
    float axes[GRAPPLE_AXIS_COUNT];

    /* Directions, and when each started being held, for the menu repeat. */
    bool direction[GRAPPLE_DIR_COUNT];
    bool direction_previous[GRAPPLE_DIR_COUNT];
    float direction_held[GRAPPLE_DIR_COUNT];
    bool direction_fired[GRAPPLE_DIR_COUNT];

    bool has_gyro, has_accel, motion_on;
    float gyro[3];
    float accel[3];
} GamepadSlot;

struct Grapple_Input
{
    bool keys[SCANCODE_COUNT];
    bool keys_previous[SCANCODE_COUNT];
    SDL_Keymod modifiers;
    char text[64];
    bool text_enabled;

    float mouse_x, mouse_y;
    float mouse_dx, mouse_dy;
    float wheel_x, wheel_y;
    bool mouse[GRAPPLE_MOUSE_COUNT];
    bool mouse_previous[GRAPPLE_MOUSE_COUNT];
    bool mouse_captured;

    GamepadSlot pads[GRAPPLE_MAX_PLAYERS];
    float deadzone;
    float trigger_threshold;
    float repeat_delay, repeat_interval;

    Grapple_Finger fingers[GRAPPLE_MAX_FINGERS];
    int finger_count;
    float pinch, rotation;

    bool device_motion_on;
    float device_gyro[3];
    float device_accel[3];
    SDL_Sensor *gyro_sensor;
    SDL_Sensor *accel_sensor;

    bool any_input;
    float idle_seconds;
    Grapple_InputDevice last_device;
};

static struct Grapple_Input *Input(Grapple_Engine *engine)
{
    return (engine != NULL) ? engine->input : NULL;
}

/* --- setup --------------------------------------------------------------- */

static Grapple_GamepadButton FromSdlButton(SDL_GamepadButton button)
{
    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        return GRAPPLE_PAD_A;
    case SDL_GAMEPAD_BUTTON_EAST:
        return GRAPPLE_PAD_B;
    case SDL_GAMEPAD_BUTTON_WEST:
        return GRAPPLE_PAD_X;
    case SDL_GAMEPAD_BUTTON_NORTH:
        return GRAPPLE_PAD_Y;
    case SDL_GAMEPAD_BUTTON_BACK:
        return GRAPPLE_PAD_BACK;
    case SDL_GAMEPAD_BUTTON_GUIDE:
        return GRAPPLE_PAD_GUIDE;
    case SDL_GAMEPAD_BUTTON_START:
        return GRAPPLE_PAD_START;
    case SDL_GAMEPAD_BUTTON_LEFT_STICK:
        return GRAPPLE_PAD_LEFT_STICK;
    case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
        return GRAPPLE_PAD_RIGHT_STICK;
    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
        return GRAPPLE_PAD_LEFT_SHOULDER;
    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
        return GRAPPLE_PAD_RIGHT_SHOULDER;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        return GRAPPLE_PAD_DPAD_UP;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        return GRAPPLE_PAD_DPAD_DOWN;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        return GRAPPLE_PAD_DPAD_LEFT;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        return GRAPPLE_PAD_DPAD_RIGHT;
    case SDL_GAMEPAD_BUTTON_MISC1:
        return GRAPPLE_PAD_SHARE;
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:
        return GRAPPLE_PAD_PADDLE1;
    case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:
        return GRAPPLE_PAD_PADDLE2;
    case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:
        return GRAPPLE_PAD_PADDLE3;
    case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:
        return GRAPPLE_PAD_PADDLE4;
    case SDL_GAMEPAD_BUTTON_TOUCHPAD:
        return GRAPPLE_PAD_TOUCHPAD;
    default:
        return GRAPPLE_PAD_BUTTON_COUNT;
    }
}

bool Grapple_EngineInputInit(Grapple_Engine *engine)
{
    engine->input = (struct Grapple_Input *)SDL_calloc(1, sizeof(struct Grapple_Input));
    if (engine->input == NULL)
    {
        return false;
    }
    struct Grapple_Input *input = engine->input;
    input->deadzone = 0.2f;
    input->trigger_threshold = 0.5f;
    input->repeat_delay = 0.4f;
    input->repeat_interval = 0.12f;
    input->last_device = GRAPPLE_DEVICE_NONE;
    return true;
}

/* Adopt a controller into the lowest free slot, so player 1 is whoever
   plugged in first and a reconnecting pad lands where it was. */
static void OpenGamepad(struct Grapple_Input *input, SDL_JoystickID id)
{
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        if (input->pads[i].pad != NULL)
        {
            continue;
        }
        SDL_Gamepad *pad = SDL_OpenGamepad(id);
        if (pad == NULL)
        {
            return;
        }
        GamepadSlot *slot = &input->pads[i];
        SDL_zerop(slot);
        slot->pad = pad;
        slot->id = id;
        const char *name = SDL_GetGamepadName(pad);
        SDL_strlcpy(slot->name, (name != NULL) ? name : "gamepad", sizeof(slot->name));
        slot->has_gyro = SDL_GamepadHasSensor(pad, SDL_SENSOR_GYRO);
        slot->has_accel = SDL_GamepadHasSensor(pad, SDL_SENSOR_ACCEL);
        return;
    }
    /* Every slot taken: a fifth controller is simply not a player, which
       is better than silently displacing one of the four already playing. */
}

static void CloseGamepad(struct Grapple_Input *input, SDL_JoystickID id)
{
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        GamepadSlot *slot = &input->pads[i];
        if (slot->pad == NULL || slot->id != id)
        {
            continue;
        }
        SDL_CloseGamepad(slot->pad);
        /* Zeroing is the whole point: a disconnected pad must read neutral,
           not frozen holding whatever it had down when the battery died. */
        SDL_zerop(slot);
        return;
    }
}

void Grapple_EngineInputOpenGamepads(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return;
    }
    /* Controllers already plugged in when the game started do not generate
       an ADDED event on every platform, so ask once. */
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    if (ids == NULL)
    {
        return;
    }
    for (int i = 0; i < count; ++i)
    {
        OpenGamepad(input, ids[i]);
    }
    SDL_free(ids);
}

void Grapple_EngineInputDestroy(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return;
    }
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        if (input->pads[i].pad != NULL)
        {
            SDL_CloseGamepad(input->pads[i].pad);
        }
    }
    if (input->gyro_sensor != NULL)
    {
        SDL_CloseSensor(input->gyro_sensor);
    }
    if (input->accel_sensor != NULL)
    {
        SDL_CloseSensor(input->accel_sensor);
    }
    SDL_free(input);
    engine->input = NULL;
}

/* --- the frame ----------------------------------------------------------- */

void Grapple_EngineInputBeginFrame(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return;
    }
    /* Snapshot, then let the event pump overwrite the live state. Edges are
       the difference between the two, computed once, so every fixed step in
       this frame gets the same answer. */
    SDL_memcpy(input->keys_previous, input->keys, sizeof(input->keys));
    SDL_memcpy(input->mouse_previous, input->mouse, sizeof(input->mouse));
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        GamepadSlot *slot = &input->pads[i];
        SDL_memcpy(slot->previous, slot->buttons, sizeof(slot->buttons));
        SDL_memcpy(slot->direction_previous, slot->direction, sizeof(slot->direction));
    }

    input->text[0] = '\0';
    input->mouse_dx = 0.0f;
    input->mouse_dy = 0.0f;
    input->wheel_x = 0.0f;
    input->wheel_y = 0.0f;
    input->pinch = 1.0f;
    input->rotation = 0.0f;
    input->any_input = false;

    /* Per-finger deltas and edges reset; the fingers themselves persist. */
    for (int i = 0; i < input->finger_count; ++i)
    {
        input->fingers[i].dx = 0.0f;
        input->fingers[i].dy = 0.0f;
        input->fingers[i].pressed = false;
        input->fingers[i].released = false;
    }
}

static void Touched(struct Grapple_Input *input, Grapple_InputDevice device)
{
    input->any_input = true;
    input->idle_seconds = 0.0f;
    input->last_device = device;
}

/* Window coordinates to design coordinates, so a game hit-tests in the
   space it drew in. */
static void ToDesign(Grapple_Engine *engine, float wx, float wy, float *x, float *y)
{
    Grapple_EngineWindowToDesign(engine, wx, wy, x, y);
}

static int FindFinger(struct Grapple_Input *input, SDL_FingerID id)
{
    for (int i = 0; i < input->finger_count; ++i)
    {
        if (input->fingers[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

void Grapple_EngineInputEvent(Grapple_Engine *engine, const SDL_Event *event)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return;
    }

    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:
        if (event->key.scancode < SCANCODE_COUNT && !event->key.repeat)
        {
            input->keys[event->key.scancode] = true;
            Touched(input, GRAPPLE_DEVICE_KEYBOARD);
        }
        input->modifiers = event->key.mod;
        break;
    case SDL_EVENT_KEY_UP:
        if (event->key.scancode < SCANCODE_COUNT)
        {
            input->keys[event->key.scancode] = false;
        }
        input->modifiers = event->key.mod;
        break;
    case SDL_EVENT_TEXT_INPUT:
        SDL_strlcat(input->text, event->text.text, sizeof(input->text));
        Touched(input, GRAPPLE_DEVICE_KEYBOARD);
        break;

    case SDL_EVENT_MOUSE_MOTION: {
        float x = 0.0f;
        float y = 0.0f;
        ToDesign(engine, event->motion.x, event->motion.y, &x, &y);
        /* The delta comes from SDL's own relative motion rather than from
           differencing positions: in relative mode the position stops
           moving, and differencing would report nothing at all. */
        float rx = 0.0f;
        float ry = 0.0f;
        ToDesign(engine, event->motion.xrel, event->motion.yrel, &rx, &ry);
        float origin_x = 0.0f;
        float origin_y = 0.0f;
        ToDesign(engine, 0.0f, 0.0f, &origin_x, &origin_y);
        input->mouse_x = x;
        input->mouse_y = y;
        input->mouse_dx += rx - origin_x;
        input->mouse_dy += ry - origin_y;
        if (event->motion.xrel != 0.0f || event->motion.yrel != 0.0f)
        {
            Touched(input, GRAPPLE_DEVICE_MOUSE);
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        const bool down = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        int index = -1;
        switch (event->button.button)
        {
        case SDL_BUTTON_LEFT:
            index = GRAPPLE_MOUSE_LEFT;
            break;
        case SDL_BUTTON_MIDDLE:
            index = GRAPPLE_MOUSE_MIDDLE;
            break;
        case SDL_BUTTON_RIGHT:
            index = GRAPPLE_MOUSE_RIGHT;
            break;
        case SDL_BUTTON_X1:
            index = GRAPPLE_MOUSE_X1;
            break;
        case SDL_BUTTON_X2:
            index = GRAPPLE_MOUSE_X2;
            break;
        default:
            break;
        }
        if (index >= 0)
        {
            input->mouse[index] = down;
            if (down)
            {
                Touched(input, GRAPPLE_DEVICE_MOUSE);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_WHEEL:
        /* SDL reports the direction flag separately; folding it in here
           means a game never has to think about natural scrolling. */
        input->wheel_x += (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                              ? -event->wheel.x
                              : event->wheel.x;
        input->wheel_y += (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                              ? -event->wheel.y
                              : event->wheel.y;
        Touched(input, GRAPPLE_DEVICE_MOUSE);
        break;

    case SDL_EVENT_GAMEPAD_ADDED:
        OpenGamepad(input, event->gdevice.which);
        break;
    case SDL_EVENT_GAMEPAD_REMOVED:
        CloseGamepad(input, event->gdevice.which);
        break;

    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP: {
        const bool down = event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;
        for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
        {
            if (input->pads[i].pad == NULL || input->pads[i].id != event->gbutton.which)
            {
                continue;
            }
            const Grapple_GamepadButton button =
                FromSdlButton((SDL_GamepadButton)event->gbutton.button);
            if (button < GRAPPLE_PAD_BUTTON_COUNT)
            {
                input->pads[i].buttons[button] = down;
                if (down)
                {
                    Touched(input, GRAPPLE_DEVICE_GAMEPAD);
                }
            }
            break;
        }
        break;
    }
    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE: {
        for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
        {
            if (input->pads[i].pad == NULL || input->pads[i].id != event->gsensor.which)
            {
                continue;
            }
            float *target = (event->gsensor.sensor == SDL_SENSOR_GYRO) ? input->pads[i].gyro
                                                                       : input->pads[i].accel;
            if (event->gsensor.sensor == SDL_SENSOR_GYRO ||
                event->gsensor.sensor == SDL_SENSOR_ACCEL)
            {
                target[0] = event->gsensor.data[0];
                target[1] = event->gsensor.data[1];
                target[2] = event->gsensor.data[2];
            }
            break;
        }
        break;
    }

    case SDL_EVENT_FINGER_DOWN: {
        if (input->finger_count >= GRAPPLE_MAX_FINGERS)
        {
            break;
        }
        Grapple_Finger *finger = &input->fingers[input->finger_count++];
        SDL_zerop(finger);
        finger->id = event->tfinger.fingerID;
        /* Touch events are normalised 0..1; the design space is what the
           game drew in, so convert once here. */
        finger->x = event->tfinger.x * (float)engine->view_width;
        finger->y = event->tfinger.y * (float)engine->view_height;
        finger->pressure = event->tfinger.pressure;
        finger->pressed = true;
        Touched(input, GRAPPLE_DEVICE_TOUCH);
        break;
    }
    case SDL_EVENT_FINGER_MOTION: {
        const int index = FindFinger(input, event->tfinger.fingerID);
        if (index < 0)
        {
            break;
        }
        Grapple_Finger *finger = &input->fingers[index];
        finger->dx += event->tfinger.dx * (float)engine->view_width;
        finger->dy += event->tfinger.dy * (float)engine->view_height;
        finger->x = event->tfinger.x * (float)engine->view_width;
        finger->y = event->tfinger.y * (float)engine->view_height;
        finger->pressure = event->tfinger.pressure;
        Touched(input, GRAPPLE_DEVICE_TOUCH);
        break;
    }
    case SDL_EVENT_FINGER_UP: {
        const int index = FindFinger(input, event->tfinger.fingerID);
        if (index < 0)
        {
            break;
        }
        /* Keep it for this frame, marked released, so a game polling once a
           frame still sees the lift. It goes away at the next BeginFrame. */
        input->fingers[index].released = true;
        Touched(input, GRAPPLE_DEVICE_TOUCH);
        break;
    }

    case SDL_EVENT_SENSOR_UPDATE: {
        if (input->gyro_sensor != NULL &&
            event->sensor.which == SDL_GetSensorID(input->gyro_sensor))
        {
            input->device_gyro[0] = event->sensor.data[0];
            input->device_gyro[1] = event->sensor.data[1];
            input->device_gyro[2] = event->sensor.data[2];
        }
        else if (input->accel_sensor != NULL &&
                 event->sensor.which == SDL_GetSensorID(input->accel_sensor))
        {
            input->device_accel[0] = event->sensor.data[0];
            input->device_accel[1] = event->sensor.data[1];
            input->device_accel[2] = event->sensor.data[2];
        }
        break;
    }
    default:
        break;
    }
}

/* Axis values are polled rather than taken from events: a stick resting
   off-centre produces no events at all, and a game that only listened to
   them would think the player had let go. */
static void SampleAxes(struct Grapple_Input *input)
{
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        GamepadSlot *slot = &input->pads[i];
        if (slot->pad == NULL)
        {
            continue;
        }
        static const SDL_GamepadAxis kAxes[GRAPPLE_AXIS_COUNT] = {
            SDL_GAMEPAD_AXIS_LEFTX,         SDL_GAMEPAD_AXIS_LEFTY,
            SDL_GAMEPAD_AXIS_RIGHTX,        SDL_GAMEPAD_AXIS_RIGHTY,
            SDL_GAMEPAD_AXIS_LEFT_TRIGGER,  SDL_GAMEPAD_AXIS_RIGHT_TRIGGER};
        for (int a = 0; a < GRAPPLE_AXIS_COUNT; ++a)
        {
            const Sint16 raw = SDL_GetGamepadAxis(slot->pad, kAxes[a]);
            slot->axes[a] = (float)raw / 32767.0f;
            if (slot->axes[a] < -1.0f)
            {
                slot->axes[a] = -1.0f;
            }
        }
        /* Triggers rest at -1 in SDL's signed range; games mean 0..1. */
        slot->axes[GRAPPLE_AXIS_LEFT_TRIGGER] =
            (slot->axes[GRAPPLE_AXIS_LEFT_TRIGGER] + 1.0f) * 0.5f;
        slot->axes[GRAPPLE_AXIS_RIGHT_TRIGGER] =
            (slot->axes[GRAPPLE_AXIS_RIGHT_TRIGGER] + 1.0f) * 0.5f;

        /* Triggers as buttons, so "shoot" is a button press like any other. */
        slot->buttons[GRAPPLE_PAD_LEFT_TRIGGER] =
            slot->axes[GRAPPLE_AXIS_LEFT_TRIGGER] >= input->trigger_threshold;
        slot->buttons[GRAPPLE_PAD_RIGHT_TRIGGER] =
            slot->axes[GRAPPLE_AXIS_RIGHT_TRIGGER] >= input->trigger_threshold;
    }
}

/* Radial deadzone: measure the stick's distance from centre, drop it below
   the threshold, and rescale the rest to a full 0..1. Deadzoning the axes
   separately carves a square hole out of a round stick, and the player
   feels the corners. */
static void ApplyDeadzone(float deadzone, float in_x, float in_y, float *out_x, float *out_y)
{
    const float magnitude = SDL_sqrtf(in_x * in_x + in_y * in_y);
    if (magnitude <= deadzone || magnitude <= 0.0f)
    {
        *out_x = 0.0f;
        *out_y = 0.0f;
        return;
    }
    const float scaled = (magnitude - deadzone) / (1.0f - deadzone);
    const float clamped = (scaled > 1.0f) ? 1.0f : scaled;
    *out_x = (in_x / magnitude) * clamped;
    *out_y = (in_y / magnitude) * clamped;
}

/* Which directions a pad is asking for, from either stick or the d-pad. */
static void SampleDirections(struct Grapple_Input *input, float dt)
{
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        GamepadSlot *slot = &input->pads[i];
        bool wanted[GRAPPLE_DIR_COUNT] = {false, false, false, false};

        if (slot->pad != NULL)
        {
            float x = 0.0f;
            float y = 0.0f;
            ApplyDeadzone(input->deadzone, slot->axes[GRAPPLE_AXIS_LEFT_X],
                          slot->axes[GRAPPLE_AXIS_LEFT_Y], &x, &y);
            /* A generous threshold: a menu should not step twice because a
               thumb wobbled on the way to a diagonal. */
            const float gate = 0.5f;
            wanted[GRAPPLE_DIR_UP] = y < -gate || slot->buttons[GRAPPLE_PAD_DPAD_UP];
            wanted[GRAPPLE_DIR_DOWN] = y > gate || slot->buttons[GRAPPLE_PAD_DPAD_DOWN];
            wanted[GRAPPLE_DIR_LEFT] = x < -gate || slot->buttons[GRAPPLE_PAD_DPAD_LEFT];
            wanted[GRAPPLE_DIR_RIGHT] = x > gate || slot->buttons[GRAPPLE_PAD_DPAD_RIGHT];
        }

        for (int d = 0; d < GRAPPLE_DIR_COUNT; ++d)
        {
            slot->direction_fired[d] = false;
            if (!wanted[d])
            {
                slot->direction[d] = false;
                slot->direction_held[d] = 0.0f;
                continue;
            }
            if (!slot->direction[d])
            {
                /* Just crossed in: fire immediately, then wait out the
                   delay before repeating — what a keyboard does, because
                   that is the behaviour everyone has already learned. */
                slot->direction[d] = true;
                slot->direction_held[d] = 0.0f;
                slot->direction_fired[d] = true;
                continue;
            }
            const float before = slot->direction_held[d];
            slot->direction_held[d] += dt;
            const float after = slot->direction_held[d];
            if (before < input->repeat_delay)
            {
                if (after >= input->repeat_delay)
                {
                    slot->direction_fired[d] = true;
                }
                continue;
            }
            const float since = after - input->repeat_delay;
            const float previous = before - input->repeat_delay;
            if (input->repeat_interval > 0.0f &&
                SDL_floorf(since / input->repeat_interval) >
                    SDL_floorf(previous / input->repeat_interval))
            {
                slot->direction_fired[d] = true;
            }
        }
    }
}

void Grapple_EngineInputEndFrame(Grapple_Engine *engine, float dt)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return;
    }
    SampleAxes(input);
    SampleDirections(input, dt);

    /* Drop lifted fingers now that the frame has had a chance to see them. */
    int kept = 0;
    for (int i = 0; i < input->finger_count; ++i)
    {
        if (!input->fingers[i].released)
        {
            input->fingers[kept++] = input->fingers[i];
        }
    }
    input->finger_count = kept;

    if (!input->any_input)
    {
        input->idle_seconds += dt;
    }
}

/* --- keyboard ------------------------------------------------------------ */

bool Grapple_KeyDown(Grapple_Engine *engine, SDL_Scancode key)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && key < SCANCODE_COUNT && input->keys[key];
}

bool Grapple_KeyPressed(Grapple_Engine *engine, SDL_Scancode key)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && key < SCANCODE_COUNT && input->keys[key] && !input->keys_previous[key];
}

bool Grapple_KeyReleased(Grapple_Engine *engine, SDL_Scancode key)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && key < SCANCODE_COUNT && !input->keys[key] && input->keys_previous[key];
}

SDL_Keymod Grapple_KeyModifiers(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->modifiers : SDL_KMOD_NONE;
}

const char *Grapple_TextTyped(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->text : "";
}

void Grapple_SetTextInput(Grapple_Engine *engine, bool enabled)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL || engine->window == NULL)
    {
        return;
    }
    input->text_enabled = enabled;
    if (enabled)
    {
        SDL_StartTextInput(engine->window);
    }
    else
    {
        SDL_StopTextInput(engine->window);
    }
}

/* --- mouse --------------------------------------------------------------- */

void Grapple_MousePosition(Grapple_Engine *engine, float *x, float *y)
{
    struct Grapple_Input *input = Input(engine);
    if (x != NULL)
    {
        *x = (input != NULL) ? input->mouse_x : 0.0f;
    }
    if (y != NULL)
    {
        *y = (input != NULL) ? input->mouse_y : 0.0f;
    }
}

void Grapple_MouseDelta(Grapple_Engine *engine, float *x, float *y)
{
    struct Grapple_Input *input = Input(engine);
    if (x != NULL)
    {
        *x = (input != NULL) ? input->mouse_dx : 0.0f;
    }
    if (y != NULL)
    {
        *y = (input != NULL) ? input->mouse_dy : 0.0f;
    }
}

bool Grapple_MouseDown(Grapple_Engine *engine, Grapple_MouseButton button)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && button < GRAPPLE_MOUSE_COUNT && input->mouse[button];
}

bool Grapple_MousePressed(Grapple_Engine *engine, Grapple_MouseButton button)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && button < GRAPPLE_MOUSE_COUNT && input->mouse[button] &&
           !input->mouse_previous[button];
}

bool Grapple_MouseReleased(Grapple_Engine *engine, Grapple_MouseButton button)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && button < GRAPPLE_MOUSE_COUNT && !input->mouse[button] &&
           input->mouse_previous[button];
}

void Grapple_MouseWheel(Grapple_Engine *engine, float *x, float *y)
{
    struct Grapple_Input *input = Input(engine);
    if (x != NULL)
    {
        *x = (input != NULL) ? input->wheel_x : 0.0f;
    }
    if (y != NULL)
    {
        *y = (input != NULL) ? input->wheel_y : 0.0f;
    }
}

bool Grapple_SetMouseCapture(Grapple_Engine *engine, bool captured)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL || engine->window == NULL)
    {
        return false;
    }
    if (!SDL_SetWindowRelativeMouseMode(engine->window, captured))
    {
        return false;
    }
    input->mouse_captured = captured;
    return true;
}

bool Grapple_MouseCaptured(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && input->mouse_captured;
}

/* --- gamepads ------------------------------------------------------------ */

static GamepadSlot *Slot(Grapple_Engine *engine, int player)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL || player < 0 || player >= GRAPPLE_MAX_PLAYERS)
    {
        return NULL;
    }
    return &input->pads[player];
}

bool Grapple_GamepadConnected(Grapple_Engine *engine, int player)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL;
}

int Grapple_GamepadCount(Grapple_Engine *engine)
{
    int count = 0;
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        count += Grapple_GamepadConnected(engine, i) ? 1 : 0;
    }
    return count;
}

const char *Grapple_GamepadName(Grapple_Engine *engine, int player)
{
    const GamepadSlot *slot = Slot(engine, player);
    return (slot != NULL && slot->pad != NULL) ? slot->name : NULL;
}

bool Grapple_GamepadButtonDown(Grapple_Engine *engine, int player,
                                 Grapple_GamepadButton button)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && button < GRAPPLE_PAD_BUTTON_COUNT &&
           slot->buttons[button];
}

bool Grapple_GamepadButtonPressed(Grapple_Engine *engine, int player,
                                    Grapple_GamepadButton button)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && button < GRAPPLE_PAD_BUTTON_COUNT &&
           slot->buttons[button] && !slot->previous[button];
}

bool Grapple_GamepadButtonReleased(Grapple_Engine *engine, int player,
                                     Grapple_GamepadButton button)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && button < GRAPPLE_PAD_BUTTON_COUNT &&
           !slot->buttons[button] && slot->previous[button];
}

float Grapple_GamepadAxisValue(Grapple_Engine *engine, int player, Grapple_GamepadAxis axis)
{
    const GamepadSlot *slot = Slot(engine, player);
    struct Grapple_Input *input = Input(engine);
    if (slot == NULL || slot->pad == NULL || input == NULL || axis >= GRAPPLE_AXIS_COUNT)
    {
        return 0.0f;
    }
    const float value = slot->axes[axis];
    if (axis == GRAPPLE_AXIS_LEFT_TRIGGER || axis == GRAPPLE_AXIS_RIGHT_TRIGGER)
    {
        return value;
    }
    /* A single axis gets a one-dimensional version of the same deadzone, so
       that reading one axis alone agrees with reading the stick. */
    float out_x = 0.0f;
    float out_y = 0.0f;
    ApplyDeadzone(input->deadzone, value, 0.0f, &out_x, &out_y);
    return out_x;
}

void Grapple_GamepadStick(Grapple_Engine *engine, int player, int side, float *x, float *y)
{
    const GamepadSlot *slot = Slot(engine, player);
    struct Grapple_Input *input = Input(engine);
    float out_x = 0.0f;
    float out_y = 0.0f;
    if (slot != NULL && slot->pad != NULL && input != NULL)
    {
        const int base = (side == 0) ? GRAPPLE_AXIS_LEFT_X : GRAPPLE_AXIS_RIGHT_X;
        ApplyDeadzone(input->deadzone, slot->axes[base], slot->axes[base + 1], &out_x, &out_y);
    }
    if (x != NULL)
    {
        *x = out_x;
    }
    if (y != NULL)
    {
        *y = out_y;
    }
}

void Grapple_SetGamepadDeadzone(Grapple_Engine *engine, float deadzone)
{
    struct Grapple_Input *input = Input(engine);
    if (input != NULL)
    {
        input->deadzone = SDL_clamp(deadzone, 0.0f, 0.9f);
    }
}

float Grapple_GamepadDeadzone(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->deadzone : 0.0f;
}

void Grapple_SetTriggerThreshold(Grapple_Engine *engine, float threshold)
{
    struct Grapple_Input *input = Input(engine);
    if (input != NULL)
    {
        input->trigger_threshold = SDL_clamp(threshold, 0.05f, 1.0f);
    }
}

bool Grapple_GamepadDirectionRepeat(Grapple_Engine *engine, int player,
                                      Grapple_Direction direction)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && direction < GRAPPLE_DIR_COUNT &&
           slot->direction_fired[direction];
}

bool Grapple_GamepadDirectionPressed(Grapple_Engine *engine, int player,
                                       Grapple_Direction direction)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && direction < GRAPPLE_DIR_COUNT &&
           slot->direction[direction] && !slot->direction_previous[direction];
}

void Grapple_SetDirectionRepeat(Grapple_Engine *engine, float delay_seconds,
                                  float interval_seconds)
{
    struct Grapple_Input *input = Input(engine);
    if (input != NULL)
    {
        input->repeat_delay = (delay_seconds > 0.0f) ? delay_seconds : 0.0f;
        input->repeat_interval = (interval_seconds > 0.0f) ? interval_seconds : 0.0f;
    }
}

/* --- rumble -------------------------------------------------------------- */

static Uint16 ToMotor(float value)
{
    return (Uint16)(SDL_clamp(value, 0.0f, 1.0f) * 65535.0f);
}

bool Grapple_GamepadRumble(Grapple_Engine *engine, int player, float low, float high,
                             Uint32 milliseconds)
{
    const GamepadSlot *slot = Slot(engine, player);
    if (slot == NULL || slot->pad == NULL)
    {
        return false;
    }
    return SDL_RumbleGamepad(slot->pad, ToMotor(low), ToMotor(high), milliseconds);
}

bool Grapple_GamepadRumbleTriggers(Grapple_Engine *engine, int player, float left,
                                     float right, Uint32 milliseconds)
{
    const GamepadSlot *slot = Slot(engine, player);
    if (slot == NULL || slot->pad == NULL)
    {
        return false;
    }
    return SDL_RumbleGamepadTriggers(slot->pad, ToMotor(left), ToMotor(right), milliseconds);
}

void Grapple_GamepadStopRumble(Grapple_Engine *engine, int player)
{
    for (int i = 0; i < GRAPPLE_MAX_PLAYERS; ++i)
    {
        if (player >= 0 && i != player)
        {
            continue;
        }
        const GamepadSlot *slot = Slot(engine, i);
        if (slot != NULL && slot->pad != NULL)
        {
            SDL_RumbleGamepad(slot->pad, 0, 0, 0);
            SDL_RumbleGamepadTriggers(slot->pad, 0, 0, 0);
        }
    }
}

bool Grapple_GamepadSetLED(Grapple_Engine *engine, int player, Uint8 red, Uint8 green,
                             Uint8 blue)
{
    const GamepadSlot *slot = Slot(engine, player);
    if (slot == NULL || slot->pad == NULL)
    {
        return false;
    }
    return SDL_SetGamepadLED(slot->pad, red, green, blue);
}

/* --- motion -------------------------------------------------------------- */

bool Grapple_GamepadHasGyro(Grapple_Engine *engine, int player)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && slot->has_gyro;
}

bool Grapple_GamepadHasAccelerometer(Grapple_Engine *engine, int player)
{
    const GamepadSlot *slot = Slot(engine, player);
    return slot != NULL && slot->pad != NULL && slot->has_accel;
}

bool Grapple_SetGamepadMotion(Grapple_Engine *engine, int player, bool enabled)
{
    GamepadSlot *slot = Slot(engine, player);
    if (slot == NULL || slot->pad == NULL)
    {
        return false;
    }
    bool ok = true;
    if (slot->has_gyro)
    {
        ok = SDL_SetGamepadSensorEnabled(slot->pad, SDL_SENSOR_GYRO, enabled) && ok;
    }
    if (slot->has_accel)
    {
        ok = SDL_SetGamepadSensorEnabled(slot->pad, SDL_SENSOR_ACCEL, enabled) && ok;
    }
    slot->motion_on = enabled && ok;
    if (!enabled)
    {
        SDL_zeroa(slot->gyro);
        SDL_zeroa(slot->accel);
    }
    return ok;
}

static void CopyVector(const float *source, float *x, float *y, float *z)
{
    if (x != NULL)
    {
        *x = source[0];
    }
    if (y != NULL)
    {
        *y = source[1];
    }
    if (z != NULL)
    {
        *z = source[2];
    }
}

void Grapple_GamepadGyro(Grapple_Engine *engine, int player, float *x, float *y, float *z)
{
    const GamepadSlot *slot = Slot(engine, player);
    static const float zero[3] = {0.0f, 0.0f, 0.0f};
    CopyVector((slot != NULL && slot->pad != NULL) ? slot->gyro : zero, x, y, z);
}

void Grapple_GamepadAccelerometer(Grapple_Engine *engine, int player, float *x, float *y,
                                    float *z)
{
    const GamepadSlot *slot = Slot(engine, player);
    static const float zero[3] = {0.0f, 0.0f, 0.0f};
    CopyVector((slot != NULL && slot->pad != NULL) ? slot->accel : zero, x, y, z);
}

/* --- touch --------------------------------------------------------------- */

int Grapple_FingerCount(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->finger_count : 0;
}

const Grapple_Finger *Grapple_GetFinger(Grapple_Engine *engine, int index)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL || index < 0 || index >= input->finger_count)
    {
        return NULL;
    }
    return &input->fingers[index];
}

static bool Inside(const Grapple_Finger *finger, SDL_FRect area)
{
    const SDL_FPoint point = {finger->x, finger->y};
    return SDL_PointInRectFloat(&point, &area);
}

const Grapple_Finger *Grapple_FingerInRect(Grapple_Engine *engine, SDL_FRect area)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < input->finger_count; ++i)
    {
        if (input->fingers[i].pressed && Inside(&input->fingers[i], area))
        {
            return &input->fingers[i];
        }
    }
    return NULL;
}

const Grapple_Finger *Grapple_FingerHeldInRect(Grapple_Engine *engine, SDL_FRect area)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < input->finger_count; ++i)
    {
        if (!input->fingers[i].released && Inside(&input->fingers[i], area))
        {
            return &input->fingers[i];
        }
    }
    return NULL;
}

float Grapple_TouchPinch(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->pinch : 1.0f;
}

float Grapple_TouchRotation(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->rotation : 0.0f;
}

/* --- device motion ------------------------------------------------------- */

bool Grapple_HasDeviceMotion(Grapple_Engine *engine)
{
    (void)engine;
    int count = 0;
    SDL_SensorID *ids = SDL_GetSensors(&count);
    SDL_free(ids);
    return count > 0;
}

bool Grapple_SetDeviceMotion(Grapple_Engine *engine, bool enabled)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return false;
    }
    if (!enabled)
    {
        if (input->gyro_sensor != NULL)
        {
            SDL_CloseSensor(input->gyro_sensor);
            input->gyro_sensor = NULL;
        }
        if (input->accel_sensor != NULL)
        {
            SDL_CloseSensor(input->accel_sensor);
            input->accel_sensor = NULL;
        }
        SDL_zeroa(input->device_gyro);
        SDL_zeroa(input->device_accel);
        input->device_motion_on = false;
        return true;
    }

    int count = 0;
    SDL_SensorID *ids = SDL_GetSensors(&count);
    if (ids == NULL)
    {
        return false;
    }
    for (int i = 0; i < count; ++i)
    {
        const SDL_SensorType type = SDL_GetSensorTypeForID(ids[i]);
        if (type == SDL_SENSOR_GYRO && input->gyro_sensor == NULL)
        {
            input->gyro_sensor = SDL_OpenSensor(ids[i]);
        }
        else if (type == SDL_SENSOR_ACCEL && input->accel_sensor == NULL)
        {
            input->accel_sensor = SDL_OpenSensor(ids[i]);
        }
    }
    SDL_free(ids);
    input->device_motion_on = input->gyro_sensor != NULL || input->accel_sensor != NULL;
    return input->device_motion_on;
}

void Grapple_DeviceGyro(Grapple_Engine *engine, float *x, float *y, float *z)
{
    struct Grapple_Input *input = Input(engine);
    static const float zero[3] = {0.0f, 0.0f, 0.0f};
    CopyVector((input != NULL) ? input->device_gyro : zero, x, y, z);
}

void Grapple_DeviceAccelerometer(Grapple_Engine *engine, float *x, float *y, float *z)
{
    struct Grapple_Input *input = Input(engine);
    static const float zero[3] = {0.0f, 0.0f, 0.0f};
    CopyVector((input != NULL) ? input->device_accel : zero, x, y, z);
}

/* --- anything at all ----------------------------------------------------- */

bool Grapple_AnyInput(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return input != NULL && input->any_input;
}

float Grapple_IdleSeconds(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->idle_seconds : 0.0f;
}

Grapple_InputDevice Grapple_LastInputDevice(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    return (input != NULL) ? input->last_device : GRAPPLE_DEVICE_NONE;
}

/* --- for the binding layer ----------------------------------------------- */

bool Grapple_InputKeyDownRaw(Grapple_Engine *engine, int scancode)
{
    return Grapple_KeyDown(engine, (SDL_Scancode)scancode);
}

int Grapple_InputFirstPressedKey(Grapple_Engine *engine)
{
    struct Grapple_Input *input = Input(engine);
    if (input == NULL)
    {
        return -1;
    }
    for (int i = 0; i < SCANCODE_COUNT; ++i)
    {
        if (input->keys[i] && !input->keys_previous[i])
        {
            return i;
        }
    }
    return -1;
}
