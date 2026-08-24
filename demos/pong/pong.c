/**
 * @file pong.c
 * @brief Pong, in C, on the opinionated loop.
 *
 * Original grapple-beam code (zlib). Around 230 lines of code, and nearly all
 * of them are the game rather than the engine — which is the claim this demo
 * exists to check.
 *
 * ### The loop is the engine's, not ours
 *
 * There is no `while (running)` here. Four callbacks are registered and
 * Grapple_RunGame owns the frame:
 *
 *   load          once, before anything is drawn
 *   fixed_update  the simulation, 60 times a second, exactly
 *   render        drawing, at whatever rate the display runs
 *
 * The split matters and is the reason to write a game this way. Ball and
 * paddles move only in fixed_update, so the physics is identical on a 60 Hz
 * laptop and a 144 Hz monitor. render receives `alpha`, how far the frame is
 * between two simulation steps, and draws positions interpolated by it — so
 * motion looks smooth on the 144 Hz screen without the simulation running
 * faster.
 *
 * ### Coordinates are design units
 *
 * Everything below is written against a fixed 1280x720 field. The engine
 * scales that to the real window and letterboxes it, so the game is written
 * once and is correct on a phone, a laptop and a 4K monitor.
 */
#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/debug_text.h>

#include <SDL3/SDL.h>

#include <stdlib.h>

/* The field the game is written against. Not pixels — see the note above. */
#define FIELD_WIDTH 1280
#define FIELD_HEIGHT 720

#define PADDLE_WIDTH 18.0f
#define PADDLE_HEIGHT 120.0f
#define PADDLE_INSET 48.0f
#define PADDLE_SPEED 620.0f   /* design units per second */

#define BALL_SIZE 18.0f
#define BALL_SPEED 520.0f
#define BALL_SPEEDUP 1.04f    /* each paddle hit, so a rally accelerates */
#define BALL_MAX_SPEED 1500.0f

#define WINNING_SCORE 11

/* A moving thing, and where it was one step ago.
 *
 * `previous` exists only so render can interpolate: drawing the current
 * position directly makes motion stutter when the display and the simulation
 * disagree about when a frame happens, which they almost always do. */
typedef struct Mover
{
    float x, y;
    float previous_x, previous_y;
    float velocity_x, velocity_y;
} Mover;

typedef struct Pong
{
    Grapple_Engine *engine;

    Mover left;
    Mover right;
    Mover ball;

    /* Set when there is no window: the match is narrated to the log instead,
       which is how CI can tell a played game from a frozen one. */
    bool narrate;
    int left_score;
    int right_score;

    /* Counts down after a point, so the ball does not reappear mid-rally. */
    float serve_delay;
    /* Which way the next serve goes: toward whoever just conceded. */
    float serve_direction;
} Pong;

/* --- helpers -------------------------------------------------------------- */

static float Clamp(float value, float low, float high)
{
    if (value < low)
    {
        return low;
    }
    return (value > high) ? high : value;
}

/** Where to draw something that moved between the last two steps. */
static float Interpolate(float previous, float current, float alpha)
{
    return previous + (current - previous) * alpha;
}

static void PlaceBallForServe(Pong *game)
{
    game->ball.x = (float)FIELD_WIDTH * 0.5f - BALL_SIZE * 0.5f;
    game->ball.y = (float)FIELD_HEIGHT * 0.5f - BALL_SIZE * 0.5f;
    game->ball.previous_x = game->ball.x;
    game->ball.previous_y = game->ball.y;

    /* Every serve leans up or down, and never by so little that it is a flat
       line both paddles can meet without moving. */
    const float lean = (rand() % 2) ? 1.0f : -1.0f;
    const float steepness = 0.4f + 0.6f * ((float)rand() / (float)RAND_MAX);
    const float drift = lean * steepness;
    game->ball.velocity_x = BALL_SPEED * game->serve_direction;
    game->ball.velocity_y = BALL_SPEED * 0.35f * drift;
    game->serve_delay = 0.75f;
}

static void ResetMatch(Pong *game)
{
    game->left.x = PADDLE_INSET;
    game->right.x = (float)FIELD_WIDTH - PADDLE_INSET - PADDLE_WIDTH;
    game->left.y = game->right.y = (float)FIELD_HEIGHT * 0.5f - PADDLE_HEIGHT * 0.5f;
    game->left.previous_y = game->left.y;
    game->right.previous_y = game->right.y;

    game->left_score = game->right_score = 0;
    game->serve_direction = 1.0f;
    PlaceBallForServe(game);
}

static SDL_FRect PaddleRect(const Mover *paddle, float alpha)
{
    const SDL_FRect rect = {paddle->x,
                            Interpolate(paddle->previous_y, paddle->y, alpha),
                            PADDLE_WIDTH, PADDLE_HEIGHT};
    return rect;
}

/* --- simulation ----------------------------------------------------------- */

/** One paddle's worth of input. The right paddle answers to both the arrow
 *  keys and a gamepad, so two people can play on one keyboard or one can
 *  bring a controller. */
static float PaddleInput(Grapple_Engine *engine, SDL_Scancode up, SDL_Scancode down,
                         int gamepad_player)
{
    float direction = 0.0f;
    if (Grapple_KeyDown(engine, up))
    {
        direction -= 1.0f;
    }
    if (Grapple_KeyDown(engine, down))
    {
        direction += 1.0f;
    }

    if (direction == 0.0f && Grapple_GamepadConnected(engine, gamepad_player))
    {
        float stick_x = 0.0f, stick_y = 0.0f;
        Grapple_GamepadStick(engine, gamepad_player, 0, &stick_x, &stick_y);
        direction = stick_y;
    }
    return direction;
}

static void MovePaddle(Mover *paddle, float direction, float step)
{
    paddle->previous_y = paddle->y;
    paddle->y = Clamp(paddle->y + direction * PADDLE_SPEED * step,
                      0.0f, (float)FIELD_HEIGHT - PADDLE_HEIGHT);
}

/** Send the ball back, steeper the further from the paddle's middle it hit.
 *  That one rule is what makes Pong a game rather than a demo: it hands the
 *  player a way to aim. */
static void BounceOffPaddle(Mover *ball, const Mover *paddle, float direction)
{
    const float paddle_middle = paddle->y + PADDLE_HEIGHT * 0.5f;
    const float ball_middle = ball->y + BALL_SIZE * 0.5f;
    const float offset = (ball_middle - paddle_middle) / (PADDLE_HEIGHT * 0.5f);

    const float speed = SDL_min(BALL_MAX_SPEED,
                                SDL_sqrtf(ball->velocity_x * ball->velocity_x +
                                          ball->velocity_y * ball->velocity_y) * BALL_SPEEDUP);
    ball->velocity_x = speed * direction;
    ball->velocity_y = speed * Clamp(offset, -1.0f, 1.0f);

    /* Nudge clear of the paddle, or a fast ball can register a second hit
       next step and stick to it. */
    ball->x = (direction > 0.0f) ? paddle->x + PADDLE_WIDTH : paddle->x - BALL_SIZE;
}

static bool Overlaps(const SDL_FRect *a, const SDL_FRect *b)
{
    return a->x < b->x + b->w && a->x + a->w > b->x &&
           a->y < b->y + b->h && a->y + a->h > b->y;
}

static void ScorePoint(Pong *game, int *score, float next_serve_direction)
{
    (*score)++;
    if (game->narrate)
    {
        SDL_Log("score %d-%d", game->left_score, game->right_score);
    }
    game->serve_direction = next_serve_direction;
    PlaceBallForServe(game);
}

static void FixedUpdate(void *user, float step)
{
    Pong *game = (Pong *)user;

    MovePaddle(&game->left,
               PaddleInput(game->engine, SDL_SCANCODE_W, SDL_SCANCODE_S, 0), step);
    MovePaddle(&game->right,
               PaddleInput(game->engine, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, 1), step);

    if (game->serve_delay > 0.0f)
    {
        game->serve_delay -= step;
        return;
    }

    game->ball.previous_x = game->ball.x;
    game->ball.previous_y = game->ball.y;
    game->ball.x += game->ball.velocity_x * step;
    game->ball.y += game->ball.velocity_y * step;

    /* Top and bottom walls. */
    if (game->ball.y <= 0.0f)
    {
        game->ball.y = 0.0f;
        game->ball.velocity_y = -game->ball.velocity_y;
    }
    else if (game->ball.y + BALL_SIZE >= (float)FIELD_HEIGHT)
    {
        game->ball.y = (float)FIELD_HEIGHT - BALL_SIZE;
        game->ball.velocity_y = -game->ball.velocity_y;
    }

    const SDL_FRect ball_rect = {game->ball.x, game->ball.y, BALL_SIZE, BALL_SIZE};
    const SDL_FRect left_rect = PaddleRect(&game->left, 1.0f);
    const SDL_FRect right_rect = PaddleRect(&game->right, 1.0f);

    if (game->ball.velocity_x < 0.0f && Overlaps(&ball_rect, &left_rect))
    {
        BounceOffPaddle(&game->ball, &game->left, 1.0f);
    }
    else if (game->ball.velocity_x > 0.0f && Overlaps(&ball_rect, &right_rect))
    {
        BounceOffPaddle(&game->ball, &game->right, -1.0f);
    }

    if (game->ball.x + BALL_SIZE < 0.0f)
    {
        ScorePoint(game, &game->right_score, 1.0f);
    }
    else if (game->ball.x > (float)FIELD_WIDTH)
    {
        ScorePoint(game, &game->left_score, -1.0f);
    }

    if (game->left_score >= WINNING_SCORE || game->right_score >= WINNING_SCORE)
    {
        ResetMatch(game);
    }

    if (Grapple_KeyPressed(game->engine, SDL_SCANCODE_ESCAPE))
    {
        Grapple_EngineQuit(game->engine);
    }
}

/* --- drawing -------------------------------------------------------------- */

static void DrawNet(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    for (float y = 8.0f; y < (float)FIELD_HEIGHT; y += 32.0f)
    {
        const SDL_FRect dash = {(float)FIELD_WIDTH * 0.5f - 2.0f, y, 4.0f, 16.0f};
        SDL_RenderFillRect(renderer, &dash);
    }
}

static void Render(void *user, float alpha)
{
    Pong *game = (Pong *)user;
    SDL_Renderer *renderer = Grapple_EngineRenderer(game->engine);

    SDL_SetRenderDrawColor(renderer, 16, 16, 22, 255);
    SDL_RenderClear(renderer);
    DrawNet(renderer);

    SDL_SetRenderDrawColor(renderer, 235, 235, 240, 255);
    const SDL_FRect left = PaddleRect(&game->left, alpha);
    const SDL_FRect right = PaddleRect(&game->right, alpha);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    const SDL_FRect ball = {Interpolate(game->ball.previous_x, game->ball.x, alpha),
                            Interpolate(game->ball.previous_y, game->ball.y, alpha),
                            BALL_SIZE, BALL_SIZE};
    SDL_RenderFillRect(renderer, &ball);

    Grapple_SetDebugTextSize(48.0f);
    Grapple_RenderDebugTextFormat(renderer, (float)FIELD_WIDTH * 0.5f - 120.0f, 32.0f,
                                  "%d", game->left_score);
    Grapple_RenderDebugTextFormat(renderer, (float)FIELD_WIDTH * 0.5f + 80.0f, 32.0f,
                                  "%d", game->right_score);
    Grapple_SetDebugTextSize(18.0f);
    Grapple_RenderDebugText(renderer, 24.0f, (float)FIELD_HEIGHT - 40.0f,
                            "W/S and Up/Down — first to 11 — Esc quits");
}

/* --- entry ---------------------------------------------------------------- */

int main(void)
{
    /* A plain struct on the stack: the engine copies what it needs. The heap
       builders in engine_config.h exist for Lua and Ruby, which cannot make
       one of these — see pong.lua for the other side of that. */
    Grapple_EngineConfig config = {0};
    config.title = "Pong — grapple-beam";
    config.design_width = FIELD_WIDTH;
    config.design_height = FIELD_HEIGHT;

    /* Pong draws itself out of rectangles, so there is no media archive to
       look for and no point letting the engine hunt for one. */
    config.no_auto_mount = true;

    /* Headless renders to a surface instead of a window: no display needed.
       CI plays this for a few seconds that way to check the engine survives
       being run rather than merely called. */
    config.headless = SDL_getenv("GRAPPLE_HEADLESS") != NULL;

    Grapple_Engine *engine = Grapple_CreateEngine(&config);
    if (engine == NULL)
    {
        SDL_Log("could not create an engine: %s", SDL_GetError());
        return 1;
    }

    Pong game = {0};
    game.engine = engine;
    game.narrate = config.headless;
    ResetMatch(&game);

    Grapple_GameHooks hooks = {0};
    hooks.fixed_update = FixedUpdate;
    hooks.render = Render;

    const bool ok = Grapple_RunGame(engine, &hooks, &game);
    Grapple_DestroyEngine(engine);
    return ok ? 0 : 1;
}
