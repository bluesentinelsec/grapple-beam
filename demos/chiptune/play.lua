-- Original Grapple code (zlib). Run with grapple-beam play.lua [score path | code].
local G = GrappleC
assert(SDL.Init(SDL.INIT_AUDIO) and MIX.Init(), SDL.GetError())
local path = arg[1] or 'tests/mixer/assets/c64-composition-named.xml'
local player
if path == 'code' then
  local composer = assert(G.CreateChipComposer(4, 480))
  for part, role in ipairs({'melody', 'harmony', 'bass', 'drums'}) do
    assert(G.SetChipPart(composer, part - 1, role, G.GRAPPLE_CHIP_PRESET_AUTO, 1))
  end
  local notes = {
    {track=0, note=72, velocity=90, start_tick=0, duration_ticks=480},
    {track=0, note=76, velocity=90, start_tick=480, duration_ticks=480},
    {track=1, note=60, velocity=80, start_tick=0, duration_ticks=1920},
    {track=1, note=64, velocity=80, start_tick=0, duration_ticks=1920},
    {track=1, note=67, velocity=80, start_tick=0, duration_ticks=1920},
    {track=2, note=36, velocity=100, start_tick=0, duration_ticks=1920},
    {track=3, note=36, velocity=100, start_tick=0, duration_ticks=120},
    {track=3, note=38, velocity=100, start_tick=480, duration_ticks=120},
  }
  for _, note in ipairs(notes) do assert(G.AddChipNote(composer, note)) end
  assert(G.AddChipTempo(composer, 0, 120))
  local song = assert(G.BuildChipSong(composer, 1920))
  player = assert(G.PlayChipSong(song, false), SDL.GetError())
  G.DestroyChipSong(song)
  G.DestroyChipComposer(composer)
else
  player = assert(G.PlayChipFile(path, false), SDL.GetError())
end
local config = G.ConfigCreate()
G.ConfigSetTitle(config, 'Chiptune from Lua')
G.ConfigSetDesignSize(config, 800, 240)
G.ConfigSetAutoMount(config, false)
G.ConfigSetHeadless(config, SDL.getenv('GRAPPLE_HEADLESS') ~= nil)
local engine = assert(G.CreateEngine(config), SDL.GetError())
G.ConfigDestroy(config)
G.OnFixedUpdate(engine, function(dt)
  if not G.ChipPlayerPlaying(player) then G.EngineQuit(engine) end
end)
G.OnRender(engine, function(alpha)
  local renderer = G.EngineRenderer(engine)
  SDL.SetRenderDrawColor(renderer, 20, 22, 42, 255)
  SDL.RenderClear(renderer)
  SDL.SetRenderDrawColor(renderer, 150, 220, 255, 255)
  G.RenderDebugText(renderer, 24, 40, 'Lua chiptune: ' .. path)
end)
assert(G.Run(engine), SDL.GetError())
G.DestroyChipPlayer(player)
G.DestroyEngine(engine)
MIX.Quit()
SDL.Quit()
