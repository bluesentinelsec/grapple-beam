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
-- Press into a wall while falling to slide down it; jump to kick off, and
-- keep alternating to climb.
-- An Xbox-layout pad works the same: left stick or d-pad, X or the right
-- trigger to run, A to jump. Escape quits.

-- 384x216 is a 16:9 frame with 16-pixel tiles, 24 wide by 13.5 tall, and it
-- scales by whole numbers to 1080p (5x) and 4K (10x). The engine scales it
-- to the window — a laptop, a 4K display — and letterboxes the difference,
-- so the game is written once, in tiles, and is right everywhere.
local engine = Grapple.engine{
  title = "Platformer — grapple-beam",
  design = { 384, 216 },
  presentation = "letterbox",
  auto_mount = false,       -- rectangles only: nothing to load
  headless = SDL.getenv("GRAPPLE_HEADLESS") ~= nil,
}

-- 22 rows tall against a 15-row view, so there is somewhere to scroll up to.
-- The camera is the modern kind: it leads the way the player faces, holds
-- the ground level through a jump, and follows anywhere. scroll = "forward"
-- gets 1985 back.
local level = Grapple.create_level(engine, { width = 212, height = 22 })

-- The ground: a long floor with two pits in it.
level:create_floor{ x = 0, y = 20, width = 69 }
level:create_floor{ x = 71, y = 20, width = 15 }
level:create_floor{ x = 89, y = 20, width = 64 }
level:create_floor{ x = 155, y = 20, width = 57 }

-- Blocks to jump on and bump; a tower of floating ones climbs to a high
-- platform, which is where the vertical scrolling shows.
level:create_block{ x = 16, y = 16 }
level:create_block{ x = 20, y = 16, width = 5 }
level:create_block{ x = 22, y = 12 }
level:create_block{ x = 37, y = 15, width = 3 }
level:create_block{ x = 45, y = 17 }
level:create_block{ x = 52, y = 16, width = 2 }
level:create_block{ x = 77, y = 16, width = 3 }
level:create_block{ x = 80, y = 12, width = 8 }
level:create_block{ x = 91, y = 12, width = 3 }
level:create_block{ x = 94, y = 16, width = 2 }
level:create_block{ x = 64, y = 17, width = 2 }
level:create_block{ x = 66, y = 14, width = 2 }
level:create_block{ x = 68, y = 11, width = 2 }
level:create_block{ x = 70, y = 8, width = 2 }
level:create_block{ x = 72, y = 5, width = 6 }
level:create_block{ x = 96, y = 3, width = 4 }
level:create_block{ x = 105, y = 3, width = 6 }
level:create_block{ x = 189, y = 12, height = 8 }

-- Ledges: jump up through them, land on them from above.
level:create_platform{ x = 30, y = 15, width = 4 }
level:create_platform{ x = 58, y = 13, width = 5 }
level:create_platform{ x = 113, y = 6, width = 4 }
level:create_platform{ x = 119, y = 9, width = 4 }

-- Walls: pipes to climb over, and at x = 100 a tall shaft three tiles wide.
-- Press into either side of it while falling to slide, jump to kick off, and
-- keep alternating to climb out onto the ledges at the top.
level:create_wall{ x = 28, y = 18, height = 2 }
level:create_wall{ x = 29, y = 18, height = 2 }
level:create_wall{ x = 38, y = 17, height = 3 }
level:create_wall{ x = 39, y = 17, height = 3 }
level:create_wall{ x = 46, y = 16, height = 4 }
level:create_wall{ x = 47, y = 16, height = 4 }
level:create_wall{ x = 57, y = 16, height = 4 }
level:create_wall{ x = 58, y = 16, height = 4 }
level:create_wall{ x = 100, y = 4, height = 16 }
level:create_wall{ x = 104, y = 4, height = 16 }

-- Stairs up, a gap, stairs down: the staircases near the end of 1-1.
level:create_stairs{ x = 134, y = 19, steps = 4, direction = "up" }
level:create_stairs{ x = 140, y = 19, steps = 4, direction = "down" }
level:create_stairs{ x = 148, y = 19, steps = 4, direction = "up" }
level:create_stairs{ x = 155, y = 19, steps = 4, direction = "down" }
level:create_stairs{ x = 181, y = 19, steps = 8, direction = "up" }

-- The player: a red rectangle standing on the ground three tiles in. The
-- defaults are Super Mario Bros.' numbers; a table of tuning keys —
-- run_speed, jump_height, coyote_time and the rest — changes the feel here.
local player = level:create_player{ x = 3, y = 19 }

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
  GrappleC.RenderDebugText(renderer, 8, 202, "arrows/AD move  shift run  space jump (walls too)  esc quit")
end)
