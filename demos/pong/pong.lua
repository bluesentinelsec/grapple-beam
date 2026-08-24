-- Pong, in Lua, on the opinionated loop.
--
-- Original grapple-beam code (zlib). Run it with the runner, which carries
-- the engine and the interpreter inside itself:
--
--     grapple demos/pong/pong.lua
--
-- Nothing is compiled and nothing is installed. This is the same game as
-- pong.c, written the same way, so the two can be read together.
--
-- The loop belongs to the engine here too. `on_fixed_update` runs at a fixed
-- rate and `on_render` receives `alpha` — how far this frame sits between two
-- simulation steps — so the ball is drawn between where it was and where it
-- is. That is what keeps motion smooth on a 144 Hz screen without the game
-- running faster on one.

local FIELD_WIDTH, FIELD_HEIGHT = 1280, 720

local PADDLE_WIDTH, PADDLE_HEIGHT = 18, 120
local PADDLE_INSET, PADDLE_SPEED = 48, 620

local BALL_SIZE, BALL_SPEED = 18, 520
local BALL_SPEEDUP, BALL_MAX_SPEED = 1.04, 1500

local WINNING_SCORE = 11

local function clamp(value, low, high)
  if value < low then return low end
  if value > high then return high end
  return value
end

-- Where to draw something that moved between the last two steps.
local function interpolate(previous, current, alpha)
  return previous + (current - previous) * alpha
end

local function mover(x, y)
  return { x = x, y = y, previous_x = x, previous_y = y, velocity_x = 0, velocity_y = 0 }
end

local game = {
  left = mover(PADDLE_INSET, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2),
  right = mover(FIELD_WIDTH - PADDLE_INSET - PADDLE_WIDTH, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2),
  ball = mover(FIELD_WIDTH / 2, FIELD_HEIGHT / 2),
  narrate = SDL.getenv("GRAPPLE_HEADLESS") ~= nil,
  left_score = 0,
  right_score = 0,
  serve_delay = 0,
  serve_direction = 1,
}

local config = GrappleC.ConfigCreate()
GrappleC.ConfigSetTitle(config, "Pong — grapple-beam")
GrappleC.ConfigSetDesignSize(config, FIELD_WIDTH, FIELD_HEIGHT)
-- Pong is drawn from rectangles, so there is no media archive to mount.
GrappleC.ConfigSetAutoMount(config, false)
-- Headless draws to a surface rather than a window: no display required,
-- which is how CI plays this for a few seconds.
GrappleC.ConfigSetHeadless(config, SDL.getenv("GRAPPLE_HEADLESS") ~= nil)
local engine = GrappleC.CreateEngine(config)
GrappleC.ConfigDestroy(config)
assert(engine, "could not create an engine: " .. tostring(SDL.GetError()))

local function place_ball_for_serve()
  local ball = game.ball
  ball.x = FIELD_WIDTH / 2 - BALL_SIZE / 2
  ball.y = FIELD_HEIGHT / 2 - BALL_SIZE / 2
  ball.previous_x, ball.previous_y = ball.x, ball.y

  -- Every serve leans up or down, and never by so little that it is a flat
  -- line both paddles can meet without moving.
  local lean = math.random() < 0.5 and -1 or 1
  local drift = lean * (0.4 + 0.6 * math.random())
  ball.velocity_x = BALL_SPEED * game.serve_direction
  ball.velocity_y = BALL_SPEED * 0.35 * drift
  game.serve_delay = 0.75
end

-- One paddle's worth of input: the keys, or a gamepad stick when one is
-- plugged in and the keys are idle.
local function paddle_input(up_key, down_key, gamepad_player)
  local direction = 0
  if GrappleC.KeyDown(engine, up_key) then direction = direction - 1 end
  if GrappleC.KeyDown(engine, down_key) then direction = direction + 1 end

  if direction == 0 and GrappleC.GamepadConnected(engine, gamepad_player) then
    -- x and y are in/out here, so they are passed as well as returned.
    local _, stick_y = GrappleC.GamepadStick(engine, gamepad_player, 0, 0, 0)
    direction = stick_y or 0
  end
  return direction
end

local function move_paddle(paddle, direction, step)
  paddle.previous_y = paddle.y
  paddle.y = clamp(paddle.y + direction * PADDLE_SPEED * step, 0, FIELD_HEIGHT - PADDLE_HEIGHT)
end

-- Send the ball back, steeper the further from the paddle's middle it hit.
-- That one rule is what makes Pong a game rather than a demo: it hands the
-- player a way to aim.
local function deflect(paddle, direction)
  local ball = game.ball
  local paddle_middle = paddle.y + PADDLE_HEIGHT / 2
  local ball_middle = ball.y + BALL_SIZE / 2
  local offset = (ball_middle - paddle_middle) / (PADDLE_HEIGHT / 2)

  local speed = math.min(BALL_MAX_SPEED,
    math.sqrt(ball.velocity_x ^ 2 + ball.velocity_y ^ 2) * BALL_SPEEDUP)
  ball.velocity_x = speed * direction
  ball.velocity_y = speed * clamp(offset, -1, 1)

  -- Nudge clear, or a fast ball hits again next step and sticks to the paddle.
  ball.x = direction > 0 and paddle.x + PADDLE_WIDTH or paddle.x - BALL_SIZE
end

local function overlaps(ax, ay, aw, ah, bx, by, bw, bh)
  return ax < bx + bw and ax + aw > bx and ay < by + bh and ay + ah > by
end

local function score_point(scorer, next_serve_direction)
  game[scorer] = game[scorer] + 1
  if game.narrate then
    -- With no window there is nothing to watch, so the match is narrated:
    -- that is how CI tells a played game from a frozen one.
    print(string.format("score %d-%d", game.left_score, game.right_score))
  end
  game.serve_direction = next_serve_direction
  place_ball_for_serve()
end

GrappleC.OnFixedUpdate(engine, function(step)
  move_paddle(game.left, paddle_input(SDL.SCANCODE_W, SDL.SCANCODE_S, 0), step)
  move_paddle(game.right, paddle_input(SDL.SCANCODE_UP, SDL.SCANCODE_DOWN, 1), step)

  if game.serve_delay > 0 then
    game.serve_delay = game.serve_delay - step
    return
  end

  local ball = game.ball
  ball.previous_x, ball.previous_y = ball.x, ball.y
  ball.x = ball.x + ball.velocity_x * step
  ball.y = ball.y + ball.velocity_y * step

  if ball.y <= 0 then
    ball.y = 0
    ball.velocity_y = -ball.velocity_y
  elseif ball.y + BALL_SIZE >= FIELD_HEIGHT then
    ball.y = FIELD_HEIGHT - BALL_SIZE
    ball.velocity_y = -ball.velocity_y
  end

  if ball.velocity_x < 0 and overlaps(ball.x, ball.y, BALL_SIZE, BALL_SIZE,
      game.left.x, game.left.y, PADDLE_WIDTH, PADDLE_HEIGHT) then
    deflect(game.left, 1)
  elseif ball.velocity_x > 0 and overlaps(ball.x, ball.y, BALL_SIZE, BALL_SIZE,
      game.right.x, game.right.y, PADDLE_WIDTH, PADDLE_HEIGHT) then
    deflect(game.right, -1)
  end

  if ball.x + BALL_SIZE < 0 then
    score_point("right_score", 1)
  elseif ball.x > FIELD_WIDTH then
    score_point("left_score", -1)
  end

  if game.left_score >= WINNING_SCORE or game.right_score >= WINNING_SCORE then
    game.left_score, game.right_score = 0, 0
    place_ball_for_serve()
  end

  if GrappleC.KeyPressed(engine, SDL.SCANCODE_ESCAPE) then
    GrappleC.EngineQuit(engine)
  end
end)

GrappleC.OnRender(engine, function(alpha)
  local renderer = GrappleC.EngineRenderer(engine)

  SDL.SetRenderDrawColor(renderer, 16, 16, 22, 255)
  SDL.RenderClear(renderer)

  SDL.SetRenderDrawColor(renderer, 60, 60, 70, 255)
  for y = 8, FIELD_HEIGHT, 32 do
    SDL.RenderFillRect(renderer, { x = FIELD_WIDTH / 2 - 2, y = y, w = 4, h = 16 })
  end

  SDL.SetRenderDrawColor(renderer, 235, 235, 240, 255)
  SDL.RenderFillRect(renderer, { x = game.left.x,
    y = interpolate(game.left.previous_y, game.left.y, alpha),
    w = PADDLE_WIDTH, h = PADDLE_HEIGHT })
  SDL.RenderFillRect(renderer, { x = game.right.x,
    y = interpolate(game.right.previous_y, game.right.y, alpha),
    w = PADDLE_WIDTH, h = PADDLE_HEIGHT })
  SDL.RenderFillRect(renderer, {
    x = interpolate(game.ball.previous_x, game.ball.x, alpha),
    y = interpolate(game.ball.previous_y, game.ball.y, alpha),
    w = BALL_SIZE, h = BALL_SIZE })

  GrappleC.SetDebugTextSize(48)
  GrappleC.RenderDebugText(renderer, FIELD_WIDTH / 2 - 120, 32, tostring(game.left_score))
  GrappleC.RenderDebugText(renderer, FIELD_WIDTH / 2 + 80, 32, tostring(game.right_score))
  GrappleC.SetDebugTextSize(18)
  GrappleC.RenderDebugText(renderer, 24, FIELD_HEIGHT - 40,
    "W/S and Up/Down — first to 11 — Esc quits")
end)

place_ball_for_serve()
GrappleC.Run(engine)
GrappleC.DestroyEngine(engine)
