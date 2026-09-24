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

level = Grapple.create_level(engine, width: 212, height: 15, scroll: :forward)

# The ground, with two pits.
level.create_floor(x: 0, y: 13, width: 69)
level.create_floor(x: 71, y: 13, width: 15)
level.create_floor(x: 89, y: 13, width: 64)
level.create_floor(x: 155, y: 13, width: 57)

# Blocks and ledges.
level.create_block(x: 16, y: 9)
level.create_block(x: 20, y: 9, width: 5)
level.create_block(x: 22, y: 5)
level.create_platform(x: 30, y: 8, width: 4)
level.create_block(x: 37, y: 8, width: 3)
level.create_block(x: 45, y: 10)
level.create_block(x: 52, y: 9, width: 2)
level.create_platform(x: 58, y: 6, width: 5)
level.create_block(x: 77, y: 9, width: 3)
level.create_block(x: 80, y: 5, width: 8)
level.create_block(x: 91, y: 5, width: 3)
level.create_block(x: 94, y: 9, width: 2)

# Walls to climb over.
[[28, 11, 2], [29, 11, 2], [38, 10, 3], [39, 10, 3],
 [46, 9, 4], [47, 9, 4], [57, 9, 4], [58, 9, 4]].each do |x, y, height|
  level.create_wall(x: x, y: y, height: height)
end

# Stairs.
level.create_stairs(x: 134, y: 12, steps: 4, direction: :up)
level.create_stairs(x: 140, y: 12, steps: 4, direction: :down)
level.create_stairs(x: 148, y: 12, steps: 4, direction: :up)
level.create_stairs(x: 155, y: 12, steps: 4, direction: :down)
level.create_stairs(x: 181, y: 12, steps: 8, direction: :up)
level.create_block(x: 189, y: 5, width: 1, height: 8)

player = level.create_player(x: 3, y: 12)

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
  GrappleC.RenderDebugText(renderer, 8, 226, "arrows/AD move  shift run  space jump  esc quit")
end
