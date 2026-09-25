# A platformer, in Ruby, on Grapple::Platformer.
#
# Original grapple-beam code (zlib). Run it with the runner:
#
#     grapple-beam run demos/platformer/platformer.rb
#
# The same level as platformer.lua, written the same way: a level built
# from dedicated objects in tile coordinates, a player with Super Mario
# Bros.' defaults, no loop to write. Keyword arguments stand in for Lua's
# tables and predicate methods for its booleans.

engine = Grapple.engine(
  title: "Platformer — grapple-beam",
  design: [256, 240],
  presentation: :letterbox,
  auto_mount: false,
  headless: !SDL.getenv("GRAPPLE_HEADLESS").nil?
)

level = Grapple.create_level(engine, width: 212, height: 22)

# The ground, with two pits.
level.create_floor(x: 0, y: 20, width: 69)
level.create_floor(x: 71, y: 20, width: 15)
level.create_floor(x: 89, y: 20, width: 64)
level.create_floor(x: 155, y: 20, width: 57)

# Blocks, a tower of floating ones up to a high platform.
level.create_block(x: 16, y: 16)
level.create_block(x: 20, y: 16, width: 5)
level.create_block(x: 22, y: 12)
level.create_block(x: 37, y: 15, width: 3)
level.create_block(x: 45, y: 17)
level.create_block(x: 52, y: 16, width: 2)
level.create_block(x: 77, y: 16, width: 3)
level.create_block(x: 80, y: 12, width: 8)
level.create_block(x: 91, y: 12, width: 3)
level.create_block(x: 94, y: 16, width: 2)
level.create_block(x: 64, y: 17, width: 2)
level.create_block(x: 66, y: 14, width: 2)
level.create_block(x: 68, y: 11, width: 2)
level.create_block(x: 70, y: 8, width: 2)
level.create_block(x: 72, y: 5, width: 6)
level.create_block(x: 96, y: 3, width: 4)
level.create_block(x: 105, y: 3, width: 6)
level.create_block(x: 189, y: 12, height: 8)

# Ledges.
level.create_platform(x: 30, y: 15, width: 4)
level.create_platform(x: 58, y: 13, width: 5)
level.create_platform(x: 113, y: 6, width: 4)
level.create_platform(x: 119, y: 9, width: 4)

# Walls, including the wall-jump shaft at x = 100.
[[28, 18, 2], [29, 18, 2], [38, 17, 3], [39, 17, 3], [46, 16, 4], [47, 16, 4], [57, 16, 4], [58, 16, 4], [100, 4, 16], [104, 4, 16]].each do |x, y, height|
  level.create_wall(x: x, y: y, height: height)
end

# Stairs.
level.create_stairs(x: 134, y: 19, steps: 4, direction: :up)
level.create_stairs(x: 140, y: 19, steps: 4, direction: :down)
level.create_stairs(x: 148, y: 19, steps: 4, direction: :up)
level.create_stairs(x: 155, y: 19, steps: 4, direction: :down)
level.create_stairs(x: 181, y: 19, steps: 8, direction: :up)

player = level.create_player(x: 3, y: 19)

# Headless: the level plays itself and narrates, so CI can tell it ran.
headless = !SDL.getenv("GRAPPLE_HEADLESS").nil?
seconds = 0.0
player.scripted_input(true) if headless

engine.on_update do |dt|
  engine.quit if engine.key_pressed?("escape")
  next unless headless

  seconds += dt
  player.input(1.0, (seconds % 1.5) < 0.4, true)
  if player.state_changed?
    x, y = player.position
    puts format("state=%s x=%.0f y=%.0f", player.state, x, y)
  end
end

engine.on_render do |_alpha|
  renderer = GrappleC.EngineRenderer(engine)
  x, = player.position
  GrappleC.SetDebugTextSize(8)
  GrappleC.RenderDebugText(renderer, 8, 8, format("%s  x=%d", player.state, (x / 16).floor))
  GrappleC.RenderDebugText(renderer, 8, 226, "arrows/AD move  shift run  space jump (walls too)  esc quit")
end
