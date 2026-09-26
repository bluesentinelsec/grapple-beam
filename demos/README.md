# Demos

Complete little games, written the way the engine wants to be used. Each one
exists to be read: the point is not that grapple-beam can draw a rectangle, but
what a whole game looks like when the loop belongs to the engine.

Every demo is built on the opinionated loop — `Grapple_RunGame` and its hooks.
None of them contains a `while (running)`.

## Chiptune playback

The [C chiptune demo](chiptune/README.md) loads and plays MIDI, MusicXML and MXL
through the library and engine loop. It also demonstrates offline WAV export.

## Platformer

The opening stretch of a Super Mario Bros. style level — floor, blocks, walls,
stairs, pits, hills, and near the end a ramp into a Sonic loop-de-loop — and a
character that runs and jumps around it, on
[`Grapple::Platformer`](../docs/platformer.md). Everything on screen is a
filled rectangle; the shapes are where the art goes. Same game four times:

| File | Language | How it runs |
| --- | --- | --- |
| [`platformer/platformer.c`](platformer/platformer.c) | C | compiled to `platformer-c` |
| [`platformer/platformer.cpp`](platformer/platformer.cpp) | C++ | compiled to `platformer-cpp` |
| [`platformer/platformer.lua`](platformer/platformer.lua) | Lua | `grapple-beam run platformer.lua` |
| [`platformer/platformer.rb`](platformer/platformer.rb) | Ruby | `grapple-beam run platformer.rb` |
| [`platformer/slopes.lua`](platformer/slopes.lua) | Lua | `grapple-beam run slopes.lua` — a playground of hills, ramps and two loops |

Controls: **A**/**D** or the arrows walk, **Shift** or **X** runs, **Space**
or **Z** jumps. An Xbox-layout pad works the same — left stick or d-pad,
**X** or the right trigger to run, **A** to jump. **Escape** quits. Headless,
the level plays itself and narrates its state changes, which is what CI checks.

## Pong

The same game four times, once per supported language. They are deliberately
line-for-line comparable: the same constants, the same function names, the same
order. Read one, and you can read the other three.

| File | Language | How it runs |
| --- | --- | --- |
| [`pong/pong.c`](pong/pong.c) | C | compiled to `pong-c` |
| [`pong/pong.cpp`](pong/pong.cpp) | C++ | compiled to `pong-cpp` |
| [`pong/pong.lua`](pong/pong.lua) | Lua | `grapple-beam run pong.lua` |
| [`pong/pong.rb`](pong/pong.rb) | Ruby | `grapple-beam run pong.rb` |

Controls: **W**/**S** for the left paddle, **↑**/**↓** for the right. A gamepad
moves the right paddle if one is plugged in. First to 11 wins, then the match
restarts.

### Running them

The compiled demos are built with the normal build, into `build/<config>/bin`:

```sh
cmake --build build/debug --target pong_c pong_cpp
./build/debug/bin/pong-c
```

The script versions need no build at all. The runner plays them:

```sh
./build/debug/bin/grapple-beam run demos/pong/pong.lua
./build/debug/bin/grapple-beam run demos/pong/pong.rb
```

Set `GRAPPLE_BEAM_BUILD_DEMOS=OFF` to leave the compiled demos out of a build.
They are off by default on Android and iOS, which have no place to put a
desktop executable.

### What each one is showing

- **C** — the baseline. A plain `Grapple_EngineConfig` on the stack, four
  function pointers, `Grapple_RunGame`.
- **C++** — the same game as a class, with the hooks as lambdas capturing it,
  so there is no `void *user` to cast back.
- **Lua and Ruby** — the same game again through the generated `GrappleC`
  surface. These use the heap config builders (`ConfigCreate`, `SetTitle`,
  `SetDesignSize`), which exist precisely because a script has no stack struct
  to hand the engine.

### Why the physics is in `fixed_update`

Every demo moves the ball and paddles in `fixed_update` and *only* draws in
`render`. `fixed_update` runs exactly 60 times a second no matter what the
display is doing, so the game plays identically on a 60 Hz laptop and a 144 Hz
monitor. `render` is handed `alpha` — how far the current frame sits between
two simulation steps — and interpolates positions with it, which is what makes
motion look smooth on the 144 Hz screen without speeding the game up.

This split is the whole argument for the opinionated loop, and Pong is small
enough that you can see it working.

## Testing

Each demo is played for ten seconds by `ctest`:

```sh
ctest --test-dir build/debug -R demo_pong
```

There is no display on a CI machine, so the demos check `GRAPPLE_HEADLESS` and
ask the engine for its headless mode, which renders to a surface instead of a
window. Surviving the clock is not enough to pass — a black screen would do
that too — so a headless run narrates each point to the log, and a run that
scored nothing fails. A pass therefore means the loop ticked, the ball moved,
and it bounced off something.
