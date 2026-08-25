---
title: Lua & Ruby
description: "Embedded Lua 5.4 and mruby with require-from-zip, a curated game API, a generated mirror of the whole C API, and a REPL."
---

# Lua &amp; Ruby scripting

Two embedded languages, compiled into the binary like everything else:
**Lua 5.4.8** (`Grapple::Lua`) and **mruby 4.0.0** (`Grapple::Ruby`).
`Grapple::Bindings` adds the game API to both.

```cmake
target_link_libraries(your_game PRIVATE Grapple::Bindings)
```

```c
lua_State *L = Grapple_CreateLuaState();
Grapple_OpenLuaBindings(L);

mrb_state *mrb = Grapple_CreateRubyState();
Grapple_OpenRubyBindings(mrb);
```

## Modules load from your asset archive

Both runtimes resolve `require` (Lua) and `require` (Ruby) through the
[VFS](vfs.html): scripts ship inside the same mounted — optionally
encrypted — zip as the rest of the game. Ruby's `require` is
CRuby-faithful (`$LOAD_PATH`, `$LOADED_FEATURES`, circular-require
guard) and loads both `.rb` source and precompiled `.mrb` bytecode.

## The curated game API

A hand-written `Grapple` module, identical in both languages, covers
the game loop: window/renderer, draw primitives, texture loading, input,
audio, physics world/body, VFS mounting, tile maps, compression, crypto,
base64. This is the layer the Lua and Ruby Pong implementations use:

```lua
Grapple.window("pong", 640, 480)
Grapple.load_texture("/assets/ball.png")
local map = Grapple.load_map("/assets/level.tmj")
local w, h, tw, th = map:size()
```

```ruby
Grapple.window("pong", 640, 480)
map = Grapple.load_map("/assets/level.tmj")
w, h, tw, th = map.size
```

Objects are garbage-collected safely: opaque handles carry their
destructor, and parent references are pinned so a mixer can never be
collected before its tracks.

## The generated flat API — the whole C surface

Beyond the curated layer, generated bindings mirror the **entire C API**
of every module into flat namespaces with C names minus prefixes — the
same 2,300+ functions in each language, plus enum constants:

```lua
local s = SDL.CreateSurface(64, 48, SDL.PIXELFORMAT_RGBA8888)
local d = JSON.Parse('{"hp": 100}')
local w = B2.CreateWorld(B2.DefaultWorldDef())
B2.World_Step(w, 1/60, 4)
local c = NK.rgb(255, 128, 0)      -- POD structs marshal as tables
```

```ruby
s = SDL.CreateSurface(64, 48, SDL::PIXELFORMAT_RGBA8888)
d = JSON.Parse('{"hp": 100}')
w = B2.CreateWorld(B2.DefaultWorldDef)
B2.World_Step(w, 1.0 / 60.0, 4)
```

Modules: `SDL`, `MIX`, `IMG`, `TTF`, `NET`, `PHYSFS`, `B2`, `NK`,
`GFX`, `TOML`, `YAML`, `MOG` (HTTP), `JSON`, `GrappleC`. Ownership is GC-safe by construction: pointers
returned by create functions are destroyed by the GC exactly once, and
calling the explicit destroy function first is also safe (never a
double-free). Out-parameters become extra return values; structs marshal
as tables/hashes.

### Constants, not magic numbers

Enum values *and* integer `#define` constants are registered by name with
the library prefix stripped, so scripts never hardcode numbers:

```lua
SDL.Init(SDL.INIT_VIDEO)
SDL.CreateWindow("game", 1280, 720, SDL.WINDOW_FULLSCREEN | SDL.WINDOW_HIGH_PIXEL_DENSITY)
if GrappleC.GuiKeyPressed(gui, SDL.SCANCODE_ESCAPE) then quit() end
```

```ruby
SDL.Init(SDL::INIT_VIDEO)
SDL.CreateWindow('game', 1280, 720, SDL::WINDOW_FULLSCREEN | SDL::WINDOW_HIGH_PIXEL_DENSITY)
```

Nuklear's constants keep their `NK_` spelling (`NK.NK_WINDOW_TITLE`)
because its prefix is lowercase `nk_`. Each constant is emitted behind an
`#ifdef`, so anything a platform doesn't define simply isn't registered.

### The script signature is not always the C signature

Check
[`SCRIPT_API.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/bindings/generated/SCRIPT_API.md)
— it lists every bound function with the signature the *script* sees.
Three rules make it differ from C:

- A `(const void *data, size_t len)` pair collapses into **one** string
  argument, so everything after it shifts left:
  `Grapple_CreateGui(renderer, data, len, font_size)` is
  `GrappleC.CreateGui(renderer, data, font_size)`. Passing a spurious
  length lands in `font_size` — the call still succeeds, just with the
  wrong value.
- Pure out-parameters are not arguments; they come back as extra returns.
- In/out parameters are passed *and* returned.

Functions that cannot cross a script boundary (callbacks, varargs,
threading) are skipped **with the reason recorded** in
[`COVERAGE.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/bindings/generated/COVERAGE.md).


### The short way to open an engine

`GrappleC.ConfigCreate` and its two dozen `ConfigSet*` functions still work,
but a table says the same thing in one call, in any order, and complains
about a key it does not recognise:

```lua
local engine = Grapple.engine{
  title = "My Game",
  window = { width = 1280, height = 720 },
  presentation = "letterbox",   -- or "native", "expand", "integer", ...
  tick_rate = 120,
}
```

```ruby
engine = Grapple.engine(
  title: "My Game",
  window: { width: 1280, height: 720 },
  presentation: :letterbox,
  tick_rate: 120
)
```

`Grapple.engine{ titel = "..." }` answers *"unknown engine option 'titel' —
did you mean 'title'?"*, which the setter form could not: a misspelled
`ConfigSetTitel` is a nil call that fails much later, or never.

The engine it returns carries the hooks as methods — `engine:on_update(fn)`,
`engine:on_event(fn)`, `engine:run()`, `engine:quit()` — and takes key names
rather than scancodes: `engine:key_pressed("escape")`. The keys the table
accepts are `title`, `window`, `design`, `presentation`, `resizable`,
`high_dpi`, `fullscreen`, `vsync`, `max_fps`, `tick_rate`, `auto_mount`,
`headless`, `media`, `font_size` and `backend`.

For the widget tree that goes with it, see
[the GUI page](gui.html#widgets-you-declare-once).

### Some functions are hand-written, and not in `SCRIPT_API.md`

The generator writes `SCRIPT_API.md` from the C headers, so anything a
generator *cannot* produce is missing from it — including the engine hooks
every game uses. The complete hand-written list:

| Call | What it does |
| --- | --- |
| `GrappleC.OnLoad(engine, fn)` | once, before the loop; return `false` to abort start-up |
| `GrappleC.OnFixedUpdate(engine, fn)` | `fn(step)` — the simulation, at a fixed rate |
| `GrappleC.OnUpdate(engine, fn)` | `fn(dt)` — per-frame work |
| `GrappleC.OnRender(engine, fn)` | `fn(alpha)` — drawing, interpolated |
| `GrappleC.OnPostRender(engine, fn)` | drawing above the effect chain |
| `GrappleC.OnEvent(engine, fn)` | `fn(event)` — one SDL_Event, borrowed for the call |
| `GrappleC.OnResize(engine, fn)` | `fn(width, height)` — the new size in pixels |
| `GrappleC.OnUnload(engine, fn)` | once, after the loop |
| `GrappleC.AttachGui(engine, gui)` | let the engine drive a GUI's input; `nil` detaches |
| `GrappleC.Run(engine)` | hand the loop over |
| `GrappleC.SceneDefine` / `SceneOn` | see [Scenes from a script](#scenes-from-a-script) |
| `SDL.LoadFile(path)` | file bytes from a real filesystem path, or `nil` |
| `Grapple.read_file(path)` | file bytes **through the VFS** |

Two of these are worth dwelling on.

**`OnEvent` hands you a borrowed event.** It is the same `SDL_Event` handle
the generated `SDL.*` and `GrappleC.Event*` accessors take, but it points at
a value the engine owns and it stops being valid the moment your function
returns. Read what you need; do not stash it.

```lua
GrappleC.OnEvent(engine, function(event)
  if GrappleC.EventType(event) == SDL.EVENT_KEY_DOWN then
    print(GrappleC.EventKeyScancode(event))
  end
end)
```

**`AttachGui` is how a GUI gets its input**, and it exists because the C
equivalent — filling a `Grapple_EventSink` and calling
`Grapple_EngineSetEventSink` — is a struct of function pointers, which does
not cross a binding. With it attached, the engine brackets Nuklear's input
around its own event pump and a script never calls `GuiInputBegin` at all.

```lua
GrappleC.AttachGui(engine, gui)   -- in OnLoad
GrappleC.AttachGui(engine, nil)   -- in OnUnload, before destroying the gui
```

**`SDL.LoadFile` versus `Grapple.read_file`.** `read_file` goes through the
VFS, which is what you want for game assets: it reads out of the mounted
archive whether or not that archive is a directory today. `SDL.LoadFile`
reads a real filesystem path that was never mounted — a system font, a file
a dialog returned. Both return `nil` rather than raising when the file is
not there, so walking a list of candidates needs no `pcall`.

## Scenes from a script

Every other definition in this engine is a struct a script can build —
an actor, a body, a light. A scene is not, because a scene *is* its
callbacks: what makes a title screen different from a level is what it
does on load and render, not any field. A builder alone would hand back
a scene that does nothing.

So scenes get the same kind of bridge the game hooks got. A script
defines a scene **by name** and pushes it by that name:

```lua
GrappleC.SceneDefine(engine, "title")

GrappleC.SceneOn(engine, "title", "load", function(scene)
  title_font = GrappleC.LoadTexture(engine, "ui/title.png")
  return true            -- returning false aborts the push
end)
GrappleC.SceneOn(engine, "title", "render", function(scene, alpha)
  draw_title(alpha)
end)
GrappleC.SceneOn(engine, "title", "event", function(scene, event)
  if GrappleC.EventType(event) == SDL.EVENT_KEY_DOWN then
    GrappleC.ScriptSceneReplace(engine, "level")
  end
end)

GrappleC.ScriptScenePush(engine, "title")
```

In Ruby the handler is a block:

```ruby
GrappleC.SceneDefine(engine, "title")
GrappleC.SceneOn(engine, "title", "render") { |scene, alpha| draw_title(alpha) }
GrappleC.ScriptScenePush(engine, "title")
```

The hooks are `load`, `enter`, `fixed_update`, `update`, `render`,
`event`, `exit` and `unload` — the lifecycle documented in
[Scenes](engine.html#scenes), in that order. They are named rather than
numbered so that a typo is an error naming the hooks that exist, instead
of a scene that quietly never draws.

**The name is the identity**, not a pointer, and that is what makes this
work from a script at all. The engine copies a definition when it pushes
one, so a C game passes a local and forgets it; a script has nowhere to
keep a struct in the first place. A name outlives both, and it is what a
callback can be traced back from.

The stack operations take that name: `ScriptScenePush`,
`ScriptSceneReplace`, `ScriptSceneReset`, `ScriptSceneTransitionTo`. The
rest of the scene API — `ScenePop`, `SceneDepth`, `SceneCurrent`,
`SceneName` — is the ordinary bound C surface and needs no wrapper.

### Per-scene state: key by `SceneKey`, not by the scene

One definition can back several live scenes, so every callback is given
its scene. To keep state per scene, index by `GrappleC.SceneKey(scene)`:

```lua
local state = {}
GrappleC.SceneOn(engine, "room", "update", function(scene, dt)
  local key = GrappleC.SceneKey(scene)
  state[key] = (state[key] or 0) + dt
end)
```

Using the scene *itself* as the key is the obvious thing and it fails
quietly: a handle is boxed fresh each time it crosses into a script, so
the same scene arrives as a different key every frame — the table grows
without bound and never finds what it stored. `SceneKey` is the identity
underneath the box. It is an address, so it is good for the scene's
lifetime and means nothing across a run; do not put it in a save.

`ScriptSceneSetStateSize` is there for the other direction: a script
scene that C code will also read with `Grapple_SceneState`.

### Redefining a name replaces it

Defining a name that already exists releases its handlers and starts
over, so reloading a script during development gets the new callbacks
rather than shadowing them with closures the old definition still holds.

## The GPU API from a script

SDL's GPU API is bound in full — device, pipelines, passes, buffers,
shaders. Getting there took more than binding the functions, because most
of them take a *descriptor struct* that a C caller fills in on the stack,
and a script has no stack to put one on. Every one of those calls was
reachable and inert.

So the descriptors get heap builders, in the `GrappleC` module, the same
shape used for `SDL_Event` and the Box2D joint definitions: create, set,
pass to SDL, destroy.

```lua
local target = GrappleC.GPUColorTargetInfoCreate()
GrappleC.GPUColorTargetInfoSetTexture(target, swapchain)
GrappleC.GPUColorTargetInfoSetClearColor(target, 0.1, 0.1, 0.15, 1.0)
GrappleC.GPUColorTargetInfoSetOps(target, SDL.GPU_LOADOP_CLEAR,
                                    SDL.GPU_STOREOP_STORE)

local pass = SDL.BeginGPURenderPass(cmd, target, 1, nil)
SDL.BindGPUGraphicsPipeline(pass, pipeline)
SDL.DrawGPUPrimitives(pass, 3, 1, 0, 0)
SDL.EndGPURenderPass(pass)

GrappleC.GPUColorTargetInfoDestroy(target)
```

Where a descriptor holds an **array** — the vertex buffers, vertex
attributes and colour targets of a pipeline — the builder appends instead
of setting, and owns the storage it grows:

```lua
local pipeline = GrappleC.GPUPipelineInfoCreate()
GrappleC.GPUPipelineInfoSetShaders(pipeline, vertex_shader, fragment_shader)
GrappleC.GPUPipelineInfoAddVertexBuffer(pipeline, 0, 20,
                                          SDL.GPU_VERTEXINPUTRATE_VERTEX)
GrappleC.GPUPipelineInfoAddVertexAttribute(pipeline, 0, 0,
                                             SDL.GPU_VERTEXELEMENTFORMAT_FLOAT3, 0)
GrappleC.GPUPipelineInfoAddVertexAttribute(pipeline, 1, 0,
                                             SDL.GPU_VERTEXELEMENTFORMAT_FLOAT2, 12)
GrappleC.GPUPipelineInfoAddColorTarget(pipeline,
                                         SDL.GetGPUSwapchainTextureFormat(device, window))
local handle = SDL.CreateGPUGraphicsPipeline(device, pipeline)
GrappleC.GPUPipelineInfoDestroy(pipeline)   -- the descriptor, not the pipeline
```

### Four calls that needed wrappers

A handful of GPU functions cannot be bound as they stand, because they
pass data through pointers a script cannot make. Each has a wrapper that
does the same work in one call:

| Instead of | Call | Why |
|---|---|---|
| `SDL_AcquireGPUSwapchainTexture` | `GrappleC.GPUAcquireSwapchain(cmd, window)` | returns the texture rather than filling an `SDL_GPUTexture **`; size via `GPUSwapchainWidth/Height`. A `nil` texture means *not ready this frame* — skip the frame, do not stop. There is a `GPUWaitAndAcquireSwapchain` too. |
| `SDL_MapGPUTransferBuffer` | `GrappleC.GPUUploadToTransferBuffer(device, buffer, offset, bytes, cycle)` | maps, copies and unmaps together. The map returns a raw `void *`, which is the one thing a script cannot hold. `GPUReadTransferBuffer` reads back. |
| `SDL_BindGPU*StorageBuffers` | `GrappleC.GPUBindVertexStorageBuffer(pass, slot, buffer)` and friends | SDL takes an array of pointers; these bind one slot, which is what nearly every call site does. |
| `SDL_BeginGPUComputePass` | `GrappleC.GPUBeginComputePass(cmd, bindings)` | its two read-write binding arrays come from `GPUComputeBindingsCreate` + `AddBuffer`/`AddTexture`. |
| `SDL_WaitForGPUFences` | `GrappleC.GPUWaitForFence(device, fence)` | the array form, for the single fence almost everyone has. |

Vertex data crosses as a string of bytes, which is what `(const void *,
int)` collapses to everywhere else in the bindings — `string.pack("fff",
x, y, z)` in Lua, `[x, y, z].pack("f*")` in Ruby.

Shaders are your problem, as they are in C: SDL takes compiled bytecode,
so ship SPIR-V, DXIL and MSL for the backends you support and pick with
`SDL_GetGPUShaderFormats`.

## Regular expressions

Neither language brings usable regular expressions of its own: mruby ships
no engine at all, so `Regexp` simply does not exist in stock mruby, and
Lua has patterns, which have no alternation, quantified groups or
lookaround. [`Grapple::Regex`](regex.html) supplies one engine for both.

In Ruby it arrives as the real class, so literals, `$1` and `$~` work —
mruby's compiler already emits code for them, it was only the class that
was missing:

```ruby
"on 2026-08-14".match(/(?<year>\d{4})/)["year"]   # => "2026"
"a1b2".gsub(/\d/) { |d| d.to_i * 2 }              # => "a2b4"
```

In Lua it is an added `Regex` module; Lua's own patterns are untouched:

```lua
for m in Regex.new("\\d+"):gmatch("a1b22") do print(m[0]) end
```

Both are linked in by `Grapple::Bindings`, so nothing extra is needed.
See [Regex](regex.html) for the full surface and its limits.

## Completions in your editor

An embedded API that your editor knows nothing about is an API you read in
a browser tab. The runner ships definitions for both languages, generated
from the same source as the bindings — so they describe exactly what your
build exposes, and a function that is not bound does not appear.

**Lua**, for lua-language-server. In `.luarc.json`:

```json
{ "workspace": { "library": ["editor/grapple.lua"] } }
```

Then `GrappleC.` completes, hovering shows the signature, and a typo is
a red squiggle rather than a runtime error three minutes in.

**Ruby**, as RBS, for Steep and Solargraph: put `editor/grapple.rbs` on
your RBS path.

They are in `editor/` beside the runner, and in
`share/grapple-beam/editor/` in the SDK.

The signatures are the ones a *script* sees, not the C ones — a
`(data, len)` pair is one string, and out-parameters come back rather than
being passed. That distinction is the main reason to generate these rather
than hand-write them.

### C and C++

The SDK ships a `tags` file at its root, covering every public function,
struct, enum and field across all the headers — including SDL3's. Vim,
Emacs and anything else that reads ctags will jump to a declaration
without a language server:

```vim
:set tags=/path/to/sdk/tags
```

For clangd, no extra file is needed: point it at your own project's
`compile_commands.json` and it reads the SDK's headers directly.

## The REPL

`tools/repl` builds an interactive shell for both languages with all
bindings loaded:

```bash
./build/debug/bin/grapple -l lua
./build/debug/bin/grapple -l ruby
```
