-- A platformer, in Lua, on Grapple::Platformer.
--
-- Original grapple-beam code (zlib). Run it with the runner:
--
--     grapple-beam run demos/platformer/platformer.lua
--
-- The first screens of a Super Mario Bros. style level — floor, blocks,
-- walls, a staircase, a pit — and a character that runs and jumps around
-- them. Everything on screen is a filled rectangle; the shapes are where
-- the art goes later, and the feel is already here.
--
-- There is no loop in this file. The level attaches itself to the engine
-- and runs the player, the camera and the drawing from a scene; the render
-- hook below only adds a HUD on top. The runner starts the engine once the
-- script has finished, the same bargain Love2D and Godot make.
--
-- Controls: A/D or the arrow keys walk, Shift or X runs, Space or Z jumps.
-- An Xbox-layout pad works the same: left stick or d-pad, X or the right
-- trigger to run, A to jump. Escape quits.

-- The NES drew 256x240 and its tiles were 16 pixels. The engine scales that
-- to the window — a laptop, a 4K display — and letterboxes the difference,
-- so the game is written once, in tiles, and is right everywhere.
local engine = Grapple.engine{
  title = "Platformer — grapple-beam",
  design = { 256, 240 },
  presentation = "letterbox",
  auto_mount = false,       -- rectangles only: nothing to load
  headless = SDL.getenv("GRAPPLE_HEADLESS") ~= nil,
}

-- 1-1 is 212 tiles long; this is its opening stretch.
local level = Grapple.create_level(engine, {
  width = 212, height = 15,
  scroll = "forward",       -- the screen never scrolls back, like 1985
})

-- The ground: a long floor with two pits in it.
level:create_floor{ x = 0,   y = 13, width = 69 }
level:create_floor{ x = 71,  y = 13, width = 15 }
level:create_floor{ x = 89,  y = 13, width = 64 }
level:create_floor{ x = 155, y = 13, width = 57 }

-- Blocks to jump on and bump: singles, rows, and a couple stacked high.
level:create_block{ x = 16, y = 9 }
level:create_block{ x = 20, y = 9, width = 5 }
level:create_block{ x = 22, y = 5 }
level:create_platform{ x = 30, y = 8, width = 4 }     -- a ledge: jump up through it
level:create_block{ x = 37, y = 8, width = 3 }
level:create_block{ x = 45, y = 10 }
level:create_block{ x = 52, y = 9, width = 2 }
level:create_platform{ x = 58, y = 6, width = 5 }
level:create_block{ x = 77, y = 9, width = 3 }
level:create_block{ x = 80, y = 5, width = 8 }
level:create_block{ x = 91, y = 5, width = 3 }
level:create_block{ x = 94, y = 9, width = 2 }

-- Walls, pipes, whatever the game will call them: columns to climb over.
level:create_wall{ x = 28, y = 11, height = 2 }
level:create_wall{ x = 29, y = 11, height = 2 }
level:create_wall{ x = 38, y = 10, height = 3 }
level:create_wall{ x = 39, y = 10, height = 3 }
level:create_wall{ x = 46, y = 9, height = 4 }
level:create_wall{ x = 47, y = 9, height = 4 }
level:create_wall{ x = 57, y = 9, height = 4 }
level:create_wall{ x = 58, y = 9, height = 4 }

-- Stairs up, a gap, stairs down: the staircase near the end of 1-1.
level:create_stairs{ x = 134, y = 12, steps = 4, direction = "up" }
level:create_stairs{ x = 140, y = 12, steps = 4, direction = "down" }
level:create_stairs{ x = 148, y = 12, steps = 4, direction = "up" }
level:create_stairs{ x = 155, y = 12, steps = 4, direction = "down" }
level:create_stairs{ x = 181, y = 12, steps = 8, direction = "up" }
level:create_block{ x = 189, y = 5, width = 1, height = 8 }   -- the top of the flagpole hill

-- The player: a red rectangle standing on the ground three tiles in. The
-- defaults are Super Mario Bros.' numbers; a table of tuning keys —
-- run_speed, jump_height, coyote_time and the rest — changes the feel here.
local player = level:create_player{ x = 3, y = 12 }

-- Without a window there is nobody to press the buttons, so the level plays
-- itself: run right, jump every so often, and narrate. That is how CI tells
-- a game that runs from a window that only draws.
local headless = SDL.getenv("GRAPPLE_HEADLESS") ~= nil
local autopilot = { seconds = 0 }
if headless then
  player:scripted_input(true)
end

engine:on_update(function(dt)
  if engine:key_pressed("escape") then engine:quit() end

  if headless then
    autopilot.seconds = autopilot.seconds + dt
    local jump = (autopilot.seconds % 1.5) < 0.4
    player:input(1, jump, true)
    if player:state_changed() then
      local x, y = player:position()
      print(string.format("state=%s x=%.0f y=%.0f", player:state(), x, y))
    end
  end
end)

-- The scene drew the level; this draws over it, in design coordinates.
engine:on_render(function(alpha)
  local renderer = GrappleC.EngineRenderer(engine)
  local x = player:position()
  GrappleC.SetDebugTextSize(8)
  GrappleC.RenderDebugText(renderer, 8, 8,
    string.format("%s  x=%d", player:state(), math.floor(x / 16)))
  GrappleC.RenderDebugText(renderer, 8, 226, "arrows/AD move  shift run  space jump  esc quit")
end)
