-- A playground for slopes and loops, in Lua, on Grapple::Platformer.
--
-- Original grapple-beam code (zlib). Run it with the runner:
--
--     grapple-beam run demos/platformer/slopes.lua
--
-- Left to right: two hills, a 45-degree climb, a plateau with a ramp down
-- into a loop, and a taller ramp into a bigger loop. Hold run (Shift or X)
-- down a ramp: the slope adds speed past the run speed, and that is what
-- carries you up the wall, over the top and out. Try the second loop
-- without the run button and watch the player drop off the wall.
--
-- 1 respawns at the start, 2 at the first ramp, 3 at the second. Esc quits.

local engine = Grapple.engine{
  title = "Slopes and loops — grapple-beam",
  auto_mount = false,
  headless = SDL.getenv("GRAPPLE_HEADLESS") ~= nil,
}

local level = Grapple.create_level(engine, { width = 110, height = 22 })
level:create_floor{ x = 0, y = 20, width = 110 }

-- Two hills: a gentle one, then a steeper one with a flat top.
level:create_slope{ x = 8,  y = 18, width = 6, height = 2, direction = "up" }
level:create_slope{ x = 14, y = 18, width = 6, height = 2, direction = "down" }
level:create_slope{ x = 24, y = 16, width = 4, height = 4, direction = "up" }
level:create_floor{ x = 28, y = 16, width = 3 }
level:create_slope{ x = 31, y = 16, width = 4, height = 4, direction = "down" }

-- A 45-degree climb to a plateau, a ramp down, and a loop of radius 3.
-- The loop's inner bottom sits on the top of row 20, the floor.
level:create_slope{ x = 40, y = 16, width = 4, height = 4, direction = "up" }
level:create_floor{ x = 44, y = 16, width = 3 }
level:create_slope{ x = 47, y = 16, width = 8, height = 4, direction = "down" }
level:create_loop{ x = 58, y = 15, radius = 3 }

-- A taller plateau and ramp into a loop of radius 4: it needs the speed.
level:create_slope{ x = 68, y = 13, width = 7, height = 7, direction = "up" }
level:create_floor{ x = 75, y = 13, width = 3 }
level:create_slope{ x = 78, y = 13, width = 10, height = 7, direction = "down" }
level:create_loop{ x = 91, y = 13, radius = 4 }

local player = level:create_player{ x = 3, y = 19 }

-- Headless, the level plays itself and narrates, so CI can tell it ran.
local headless = SDL.getenv("GRAPPLE_HEADLESS") ~= nil
local last_angle = 0
if headless then player:scripted_input(true) end

local respawns = { [1] = { 3 * 16 + 8, 20 * 16 }, [2] = { 45 * 16, 16 * 16 }, [3] = { 76 * 16, 13 * 16 } }

engine:on_update(function(dt)
  if engine:key_pressed("escape") then engine:quit() end
  for key, at in pairs(respawns) do
    if engine:key_pressed(tostring(key)) then player:respawn(at[1], at[2]) end
  end
  if headless then
    player:input(1, false, true)
    -- Narrate state changes and every ten degrees of slope.
    local angle = math.floor(player:angle() / 10 + 0.5) * 10
    if player:state_changed() or angle ~= last_angle then
      last_angle = angle
      local x, y = player:position()
      print(string.format("state=%s x=%.0f y=%.0f angle=%.0f", player:state(), x, y, player:angle()))
    end
  end
end)

engine:on_post_render(function()
  local renderer = GrappleC.EngineRenderer(engine)
  local x = player:position()
  GrappleC.SetDebugTextSize(8)
  GrappleC.RenderDebugText(renderer, 8, 8,
    string.format("%s  x=%d  angle=%d  speed=%d", player:state(), math.floor(x / 16),
      math.floor(player:angle() + 0.5), math.floor(math.abs(player:ground_speed()) + 0.5)))
  GrappleC.RenderDebugText(renderer, 8, 202, "hold shift down the ramps   1/2/3 respawn   esc quit")
end)
