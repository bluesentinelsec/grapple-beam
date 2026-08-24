# Pong, in Ruby, on the opinionated loop.
#
# Original grapple-beam code (zlib). Run it with the runner, which carries the
# engine and the interpreter inside itself:
#
#     grapple demos/pong/pong.rb
#
# The same game as pong.c and pong.lua, written the same way. Ruby blocks stand
# in for the C function pointers; the loop is still the engine's, with
# +on_fixed_update+ at a fixed rate and +on_render+ receiving the interpolation
# alpha so motion is smooth on a display that does not match it.

FIELD_WIDTH = 1280
FIELD_HEIGHT = 720

PADDLE_WIDTH = 18
PADDLE_HEIGHT = 120
PADDLE_INSET = 48
PADDLE_SPEED = 620

BALL_SIZE = 18
BALL_SPEED = 520
BALL_SPEEDUP = 1.04
BALL_MAX_SPEED = 1500

WINNING_SCORE = 11

def clamp(value, low, high)
  return low if value < low
  return high if value > high

  value
end

# Where to draw something that moved between the last two steps.
def interpolate(previous, current, alpha)
  previous + (current - previous) * alpha
end

def mover(x, y)
  { x: x, y: y, previous_x: x, previous_y: y, velocity_x: 0.0, velocity_y: 0.0 }
end

$game = {
  left: mover(PADDLE_INSET, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2),
  right: mover(FIELD_WIDTH - PADDLE_INSET - PADDLE_WIDTH, FIELD_HEIGHT / 2 - PADDLE_HEIGHT / 2),
  ball: mover(FIELD_WIDTH / 2, FIELD_HEIGHT / 2),
  narrate: !SDL.getenv("GRAPPLE_HEADLESS").nil?,
  left_score: 0,
  right_score: 0,
  serve_delay: 0.0,
  serve_direction: 1.0
}

config = GrappleC.ConfigCreate
GrappleC.ConfigSetTitle(config, "Pong — grapple-beam")
GrappleC.ConfigSetDesignSize(config, FIELD_WIDTH, FIELD_HEIGHT)
# Pong is drawn from rectangles, so there is no media archive to mount.
GrappleC.ConfigSetAutoMount(config, false)
# Headless draws to a surface rather than a window: no display required, which
# is how CI plays this for a few seconds.
GrappleC.ConfigSetHeadless(config, !SDL.getenv("GRAPPLE_HEADLESS").nil?)
$engine = GrappleC.CreateEngine(config)
GrappleC.ConfigDestroy(config)
raise "could not create an engine: #{SDL.GetError}" if $engine.nil?

def place_ball_for_serve
  ball = $game[:ball]
  ball[:x] = FIELD_WIDTH / 2 - BALL_SIZE / 2
  ball[:y] = FIELD_HEIGHT / 2 - BALL_SIZE / 2
  ball[:previous_x] = ball[:x]
  ball[:previous_y] = ball[:y]

  # Every serve leans up or down, and never by so little that it is a flat
  # line both paddles can meet without moving.
  lean = rand < 0.5 ? -1.0 : 1.0
  drift = lean * (0.4 + 0.6 * rand)
  ball[:velocity_x] = BALL_SPEED * $game[:serve_direction]
  ball[:velocity_y] = BALL_SPEED * 0.35 * drift
  $game[:serve_delay] = 0.75
end

# One paddle's worth of input: the keys, or a gamepad stick when one is plugged
# in and the keys are idle.
def paddle_input(up_key, down_key, gamepad_player)
  direction = 0.0
  direction -= 1.0 if GrappleC.KeyDown($engine, up_key)
  direction += 1.0 if GrappleC.KeyDown($engine, down_key)

  if direction.zero? && GrappleC.GamepadConnected($engine, gamepad_player)
    # x and y are in/out here, so they are passed as well as returned.
    _, stick_y = GrappleC.GamepadStick($engine, gamepad_player, 0, 0, 0)
    direction = stick_y || 0.0
  end
  direction
end

def move_paddle(paddle, direction, step)
  paddle[:previous_y] = paddle[:y]
  paddle[:y] = clamp(paddle[:y] + direction * PADDLE_SPEED * step, 0, FIELD_HEIGHT - PADDLE_HEIGHT)
end

# Send the ball back, steeper the further from the paddle's middle it hit. That
# one rule is what makes Pong a game rather than a demo: it hands the player a
# way to aim.
def deflect(paddle, direction)
  ball = $game[:ball]
  paddle_middle = paddle[:y] + PADDLE_HEIGHT / 2
  ball_middle = ball[:y] + BALL_SIZE / 2
  offset = (ball_middle - paddle_middle) / (PADDLE_HEIGHT / 2.0)

  speed = [BALL_MAX_SPEED,
           Math.sqrt(ball[:velocity_x]**2 + ball[:velocity_y]**2) * BALL_SPEEDUP].min
  ball[:velocity_x] = speed * direction
  ball[:velocity_y] = speed * clamp(offset, -1.0, 1.0)

  # Nudge clear, or a fast ball hits again next step and sticks to the paddle.
  ball[:x] = direction > 0 ? paddle[:x] + PADDLE_WIDTH : paddle[:x] - BALL_SIZE
end

def overlaps?(ax, ay, aw, ah, bx, by, bw, bh)
  ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by
end

def score_point(scorer, next_serve_direction)
  $game[scorer] += 1
  if $game[:narrate]
    # With no window there is nothing to watch, so the match is narrated: that
    # is how CI tells a played game from a frozen one.
    puts format("score %d-%d", $game[:left_score], $game[:right_score])
  end
  $game[:serve_direction] = next_serve_direction
  place_ball_for_serve
end

GrappleC.OnFixedUpdate($engine) do |step|
  move_paddle($game[:left], paddle_input(SDL::SCANCODE_W, SDL::SCANCODE_S, 0), step)
  move_paddle($game[:right], paddle_input(SDL::SCANCODE_UP, SDL::SCANCODE_DOWN, 1), step)

  if $game[:serve_delay] > 0
    $game[:serve_delay] -= step
  else
    ball = $game[:ball]
    ball[:previous_x] = ball[:x]
    ball[:previous_y] = ball[:y]
    ball[:x] += ball[:velocity_x] * step
    ball[:y] += ball[:velocity_y] * step

    if ball[:y] <= 0
      ball[:y] = 0
      ball[:velocity_y] = -ball[:velocity_y]
    elsif ball[:y] + BALL_SIZE >= FIELD_HEIGHT
      ball[:y] = FIELD_HEIGHT - BALL_SIZE
      ball[:velocity_y] = -ball[:velocity_y]
    end

    left = $game[:left]
    right = $game[:right]
    if ball[:velocity_x] < 0 && overlaps?(ball[:x], ball[:y], BALL_SIZE, BALL_SIZE,
                                          left[:x], left[:y], PADDLE_WIDTH, PADDLE_HEIGHT)
      deflect(left, 1)
    elsif ball[:velocity_x] > 0 && overlaps?(ball[:x], ball[:y], BALL_SIZE, BALL_SIZE,
                                             right[:x], right[:y], PADDLE_WIDTH, PADDLE_HEIGHT)
      deflect(right, -1)
    end

    if ball[:x] + BALL_SIZE < 0
      score_point(:right_score, 1.0)
    elsif ball[:x] > FIELD_WIDTH
      score_point(:left_score, -1.0)
    end

    if $game[:left_score] >= WINNING_SCORE || $game[:right_score] >= WINNING_SCORE
      $game[:left_score] = 0
      $game[:right_score] = 0
      place_ball_for_serve
    end
  end

  GrappleC.EngineQuit($engine) if GrappleC.KeyPressed($engine, SDL::SCANCODE_ESCAPE)
end

GrappleC.OnRender($engine) do |alpha|
  renderer = GrappleC.EngineRenderer($engine)

  SDL.SetRenderDrawColor(renderer, 16, 16, 22, 255)
  SDL.RenderClear(renderer)

  SDL.SetRenderDrawColor(renderer, 60, 60, 70, 255)
  y = 8
  while y < FIELD_HEIGHT
    SDL.RenderFillRect(renderer, { x: FIELD_WIDTH / 2 - 2, y: y, w: 4, h: 16 })
    y += 32
  end

  SDL.SetRenderDrawColor(renderer, 235, 235, 240, 255)
  left = $game[:left]
  right = $game[:right]
  ball = $game[:ball]
  SDL.RenderFillRect(renderer, { x: left[:x], y: interpolate(left[:previous_y], left[:y], alpha),
                                 w: PADDLE_WIDTH, h: PADDLE_HEIGHT })
  SDL.RenderFillRect(renderer, { x: right[:x], y: interpolate(right[:previous_y], right[:y], alpha),
                                 w: PADDLE_WIDTH, h: PADDLE_HEIGHT })
  SDL.RenderFillRect(renderer, { x: interpolate(ball[:previous_x], ball[:x], alpha),
                                 y: interpolate(ball[:previous_y], ball[:y], alpha),
                                 w: BALL_SIZE, h: BALL_SIZE })

  GrappleC.SetDebugTextSize(48)
  GrappleC.RenderDebugText(renderer, FIELD_WIDTH / 2 - 120, 32, $game[:left_score].to_s)
  GrappleC.RenderDebugText(renderer, FIELD_WIDTH / 2 + 80, 32, $game[:right_score].to_s)
  GrappleC.SetDebugTextSize(18)
  GrappleC.RenderDebugText(renderer, 24, FIELD_HEIGHT - 40,
                           "W/S and Up/Down — first to 11 — Esc quits")
end

place_ball_for_serve
GrappleC.Run($engine)
GrappleC.DestroyEngine($engine)
