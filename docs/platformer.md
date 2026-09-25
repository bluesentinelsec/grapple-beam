---
title: Platformer
description: "An opinionated 2D platformer layer: a tile grid with dedicated level objects, a Mario-tuned player character, pre-bound actions and a scrolling camera — with no loop to write."
---

# Platformer — `Grapple::Platformer`

The games this module is modelled on — Super Mario Bros., Super Mario
World, Donkey Kong Country — predate rigid-body physics engines, and their
feel comes from the model they used instead: an axis-aligned box swept
against a tile grid, one axis at a time, with hand-tuned acceleration and a
jump whose height depends on how long the button is held. That is what
`Grapple::Platformer` implements, over the [engine's](engine.md) actors,
camera, actions and scenes. It does not use Box2D.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Platformer)
```

A complete, playable level in Lua:

```lua
local engine = Grapple.engine{ title = "1-1" }

local level = Grapple.create_level(engine, { width = 212, height = 22 })
level:create_floor{ x = 0,  y = 13, width = 69 }
level:create_floor{ x = 71, y = 13, width = 15 }        -- a pit between them
level:create_wall{  x = 28, y = 11, height = 2 }
level:create_block{ x = 20, y = 9,  width = 5 }
level:create_stairs{ x = 134, y = 12, steps = 4, direction = "up" }
level:create_platform{ x = 30, y = 8, width = 4 }       -- a ledge to jump up through

local mario = level:create_player{ x = 3, y = 12 }
```

Nothing else is needed. The level attaches itself to the engine, so the
player steps, the camera scrolls and the cells draw with no hooks
registered, and the runner starts the engine when the script finishes. The
[platformer demos](https://github.com/bluesentinelsec/grapple-beam/blob/main/demos/platformer/)
are that file in all four languages.

| Task | Sections |
| --- | --- |
| Lay out a level | [Levels](#levels), [dedicated objects](#dedicated-objects), [Tiled](#levels-from-tiled) |
| Move a character | [The player](#the-player), [tuning](#tuning), [events](#reading-the-player) |
| Take input | [Actions](#actions) |
| Draw and scroll | [Look](#look), [camera](#the-camera), [the loop](#the-loop) |

## Levels

A level is a grid of cells, `width` by `height` tiles of `tile` pixels
(16 by default), plus a list of pixel-sized solids for anything the grid
cannot say. **Every coordinate a level takes is in tiles**, with (0, 0) the
top-left cell, because that is how a level is designed; **everything the
player reports is in pixels**, because that is how the engine draws.

```c
Grapple_Platformer *level = Grapple_CreatePlatformer(engine, 212, 15, 16);
Grapple_PlatformerSetTile(level, 4, 12, GRAPPLE_PLATFORMER_SOLID);
Grapple_PlatformerFillTiles(level, 0, 13, 212, 2, GRAPPLE_PLATFORMER_SOLID);
```

A cell is `EMPTY`, `SOLID`, or `ONE_WAY` — a ledge that is solid from above
and passable from below and the sides. The left and right edges of the
level read as solid, so the player cannot walk out of it; the top and
bottom are open, so a high jump is not a head bump and a pit is a pit.

The display is the level's business, not the game's. A level frames the
view at **24 by 13.5 tiles** — the framing of the modern Mario games, and
at 16-pixel tiles a 384×216 frame that 1080p and 4K enlarge by exactly 5×
and 10× — and presents it with the engine's [`pixel`
mode](engine.md#presentation-modes): the whole frame is drawn once at that
size and enlarged as one picture, by the largest whole number that fits the
display, with any remainder a stretch of less than an art pixel. Nothing is
scaled per sprite or per tile, so every art pixel is a square block on a
laptop and on a 4K monitor alike, and a game never has to think about it.

To take over, say so at the engine: a `design` size changes the frame (the
level leaves an explicit one alone), and any `presentation` other than the
default letterbox is kept. For art drawn for 4K rather than scaled to it,
use 32-pixel tiles: the same 24×13.5 framing becomes 768×432, which 4K
enlarges 5× and 1080p 2× with a quarter-pixel stretch.

### Dedicated objects

Almost every level is made of the same few things, and a level reads better
when it says which:

| Call | Makes |
| --- | --- |
| `create_floor{ x, y, width, depth }` | ground from row `y` down, to the bottom of the level unless `depth` says otherwise |
| `create_wall{ x, y, height }` | a one-tile column, growing down from row `y` |
| `create_block{ x, y, width, height }` | a solid rectangle; a single block is the default 1×1 |
| `create_stairs{ x, y, steps, direction }` | a staircase standing on row `y`, starting at column `x`; each column is one taller than the last going `"up"`, one shorter going `"down"` |
| `create_platform{ x, y, width }` | a one-way ledge |
| `solid{ x, y, w, h }` | a generic solid rectangle in **pixels**, for anything else; returns an id for `remove_solid` |

Each is sugar over `FillTiles`, and `set_tile`/`tile_at`/`fill` are there
for the case the names do not cover.

### Levels from Tiled

```lua
local level = Grapple.load_level(engine, "levels/1-1.tmj")
local mario = level:player()          -- from an object of class "player"
```

The map is read through [Grapple::Tiled](tiled.md), VFS-first, with the
smallest set of conventions a designer has to know:

- a tile layer named `solid`, `ground`, `collision` or `walls`, or with a
  boolean `solid` property set, is collision: every cell holding a tile is
  solid;
- a tileset tile with a `one_way` property is a ledge, and one with
  `solid = false` is decoration, wherever it appears in a collision layer;
- an object whose class is `floor`, `wall`, `block` or `platform` fills the
  cells under its rectangle, and one of class `player` places the player
  with its feet at the object's bottom edge.

Everything else in the map — decoration layers, the game's own objects — is
left for the game to read from the same file. Square tiles are required,
and the map should be Tiled's own export: the parser reads the compact
`"key":value` layout Tiled writes, not a pretty-printed copy.

## The player

One per level, spawned standing at a cell:

```lua
local mario = level:create_player{ x = 3, y = 12 }     -- feet at the bottom of cell (3, 12)
```

The player is an ordinary engine actor of type `"player"` whose movement the
level owns. That is what makes swapping the rectangle for art one line: give
the actor a sprite and the placeholder is gone, and the sprite flips with
the player's facing.

```lua
local sprite = GrappleC.ActorSprite(GrappleC.ActorGet(engine, mario:actor()))
GrappleC.SpriteSetTexture(sprite, GrappleC.Texture(engine, GrappleC.LoadTexture(engine, "mario.png")))
```

Its movement is the classic one, with the modern additions. Gravity is applied every step and the
floor cancels it, so "am I standing" is something the collision pass
produces rather than a question a raycast answers. Movement is swept one
axis at a time against the grid, which is what makes a head bump stop the
rise, a wall stop the walk and a landing snap the feet to the tile. Holding
the jump button keeps gravity light on the way up; letting go makes it
heavy, which is the whole of the variable-height jump. **Coyote time** lets
a jump pressed just after walking off a ledge still happen, and the **jump
buffer** lets one pressed just before landing happen on landing — the
difference between "the game ate my jump" and a controller that feels read.

Walls work the way the modern Mario games and Mega Man X do them. Press
into a wall while falling and the player **wall slides** down it at a
fraction of falling speed; jump from there and it **kicks off**, away from
the wall and up, facing the way it goes, with the stick ignored for a
moment so a player still holding toward the wall does not cancel the kick.
Then, as in Super Mario Wonder, steering back toward that wall turns the
player around hard, so they catch the same wall again higher up: a single
wall is climbed kick after kick, with no second wall needed. Letting go of
the stick drops off the wall.

### Tuning

The defaults are Super Mario Bros.' numbers, converted to pixels per second
at the level's tile size, so a fresh level already feels right. Every number
is a key:

| Key | Default at 16 px tiles | What it is |
| --- | --- | --- |
| `walk_speed`, `run_speed` | 96, 160 px/s | top speeds, walking and with `run` held |
| `accel`, `decel`, `skid_decel` | 400, 600, 900 px/s² | on the ground: toward the stick, toward rest, and turning around |
| `air_accel`, `air_decel` | 300, 0 | in the air; zero decel keeps momentum through a jump |
| `jump_speed` | 240 px/s | take-off |
| `jump_height` | 64 px | `jump_speed` spelled as the standing jump it produces; set either |
| `run_jump_bonus` | 32 px/s | extra take-off at full run speed, scaled by how much of it the player has |
| `jump_gravity`, `fall_gravity` | 450, 1575 px/s² | while the button is held on the way up, and otherwise |
| `max_fall` | 272 px/s | terminal velocity |
| `coyote_time`, `jump_buffer` | 0.08, 0.10 s | the two forgivenesses above |
| `wall_slide_speed` | 64 px/s | how fast a wall slide falls |
| `wall_jump_x`, `wall_jump_y` | 96, 240 px/s | the kick off a wall, as high as a ground jump; a `wall_jump_y` of 0 turns wall jumping off |
| `wall_coyote_time`, `wall_jump_lock` | 0.10, 0.05 s | how long after leaving a wall a jump still kicks, and how long the stick is ignored after one |
| `wall_return_accel`, `wall_return_time` | 1200 px/s², 0.6 s | how hard, and for how long after a kick, steering back toward the wall just left turns the player around |

```lua
local mario = level:create_player{ x = 3, y = 12, run_speed = 200, jump_height = 80 }
mario:set("coyote_time", 0.12)
mario:set{ accel = 500, decel = 700 }
print(mario:get("jump_height"))
```

The box is `width` × `height` pixels anchored at the feet (14×30 by default,
a little under a tile wide and two tall, which keeps a character from
catching on the lip of a gap it visibly fits through); `color` is the
placeholder's colour.

### Reading the player

Polling, like the engine's input, and the events are true for the whole of
the frame in which they happened, whichever hook asks:

```lua
engine:on_update(function(dt)
  if mario:state_changed() then sprite:play(mario:state()) end   -- "idle" "walk" "run" "jump" "fall" "wall_slide" "paused"
  if mario:landed() then Grapple.sfx("land") end
  if mario:bumped() then bump_the_block_above(mario:position()) end
  if mario:fell()   then lives = lives - 1 end
end)
```

`position` is the feet in pixels, `velocity` in pixels per second,
`grounded` and `facing` (−1 or +1) are the obvious things, and `wall` is
−1 or +1 while pressed against a wall in the air, else 0. A player that
falls out of the bottom of the level is put back where it spawned in the
same step and reports `fell`; a game that wants a death animation pauses
the player there and takes over. `respawn(x, y)` and `respawn()` teleport,
so the sprite does not smear; `set_paused(true)` freezes the player over a
live world, independent of the engine's time scale.

## Actions

The level owns an action map with what a platformer needs, bound to the
keyboard and a modern Xbox-layout pad:

| Action | Keyboard | Pad |
| --- | --- | --- |
| `move_x` (−1..1) | A/D, ←/→ | left stick, d-pad |
| `jump` | Space, Z | A |
| `run` | Left Shift, X | X, right trigger |

Rebind in the text form the engine's [action maps](engine.md#actions) use,
add the game's own actions to the same map so one `bindings.toml` covers
everything, and read them the same way:

```lua
level:bind("jump", "pad:b")
level:bind("attack", "pad:y")
if level:action_pressed("attack") then swing() end
local map = level:actions()      -- the generated Grapple_ActionMap, for everything else
```

For a cutscene, a replay, an attract mode or a test, drive the player from
code instead: `mario:scripted_input(true)` then
`mario:input(move_x, jump, run)` each frame. The map is not read while
scripted input is on. The demos use this to play themselves headless in CI.

## Look

Until a game has art, cells draw as filled rectangles with a darker rim on
two sides — enough that a level built from nothing but shapes still reads as
tiles — and the player as one in its own colour. `background{ r, g, b }`,
`tile_color("solid", { r, g, b })` and the player's `color` change them,
in 0–255 or 0–1. `Grapple_PlatformerSetTileTexture` gives a kind of cell a
texture to draw instead.

## The camera

The camera is the modern kind — Super Mario World, Donkey Kong Country, the
SNES Zeldas — and follows anywhere the level goes:

- **It leads.** The view sits a couple of tiles ahead of the player in the
  direction faced, easing across when the player turns, so there is more
  screen in front than behind.
- **It ignores hops.** Horizontally, the player moves inside a deadzone a
  few tiles wide before the view follows. Vertically, the view holds the
  *last ground level* through a jump, however high, and only climbs or
  drops when the player leaves a band around it — climbing out of the top
  of the view, or falling out of the bottom. Land on a ledge and that is the
  new ground level, so stairs and towers scroll.
- **It stays in the level**, and a level smaller than the view is centred.

`camera_look_ahead` (pixels), `camera_deadzone{ width, height }` (the
horizontal box and the vertical band, pixels) and `camera_smoothing`
(seconds) tune it, as constructor options or methods. `scroll = "forward"`
gets 1985 back: the view never scrolls left, and what has scrolled off is a
wall to the player. `camera_position` is there for a HUD; C and C++ get the
`Grapple_Camera` itself to draw through.

## The loop

`attach` — done for you by the Lua and Ruby constructors unless
`attach = false` — pushes a [scene](engine.md#scenes) that runs the level:
the player steps in the fixed tick, the camera follows in the per-frame
update, and the render draws the cells then the actors through the camera,
sorted, culled and interpolated by the engine. The game's own `on_update`
and `on_post_render` hooks run around it. Draw a HUD in `on_post_render`:
it runs after the pixel-art frame has been enlarged to the window, so text
is rasterised at the window's density rather than drawn into the small
frame and blown up. A game with scenes of its own — a title screen, a pause menu — calls
`step`, `update` and `render` from them instead.

Because the push is a scene, a script that builds a level and registers no
hook at all is still describing a game, and the runner starts it.

## C, C++ and Ruby

The C surface is `<grapple/platformer.h>`: `Grapple_CreatePlatformer` /
`Grapple_LoadPlatformer`, the `Grapple_PlatformerCreate*` objects,
`Grapple_PlatformerCreatePlayer` and the `Grapple_PlatformerPlayer*`
readers, all in the units above. C++ gets `grapple::Platformer` and a
`grapple::PlatformerPlayer` view in `<grapple/platformer_level.h>`, with
`Status`/`Result` error flow. Ruby mirrors Lua with keyword arguments and
predicate methods:

```ruby
level = Grapple.create_level(engine, width: 212, height: 15, scroll: :forward)
level.create_floor(x: 0, y: 13, width: 69)
mario = level.create_player(x: 3, y: 12, jump_height: 80)
engine.on_update { |dt| puts mario.state if mario.state_changed? }
```

The generated `GrappleC.Platformer*` functions remain available in both
scripting languages for anything the objects do not spell.

## What is next

Ducking, sliding, ladders, slopes and loops are the next states
of the same machine and the same collision pass — slopes as height-mask
cells, the way Super Mario World and Sonic did them, so the mover keeps its
shape. Animation clips from Aseprite exports plug into `state_changed`.
