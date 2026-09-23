# Original Grapple code (zlib). Run with grapple-beam play.rb [score path | code].
raise SDL.GetError unless SDL.Init(SDL::INIT_AUDIO) && MIX.Init
path = ARGV[0] || 'tests/mixer/assets/c64-composition-named.xml'
if path == 'code'
  composer = GrappleC.CreateChipComposer(4, 480)
  raise SDL.GetError unless composer
  ['melody', 'harmony', 'bass', 'drums'].each_with_index do |role, part|
    raise SDL.GetError unless GrappleC.SetChipPart(composer, part, role, GrappleC::GRAPPLE_CHIP_PRESET_AUTO, 1)
  end
  notes = [
    {track: 0, note: 72, velocity: 90, start_tick: 0, duration_ticks: 480},
    {track: 0, note: 76, velocity: 90, start_tick: 480, duration_ticks: 480},
    {track: 1, note: 60, velocity: 80, start_tick: 0, duration_ticks: 1920},
    {track: 1, note: 64, velocity: 80, start_tick: 0, duration_ticks: 1920},
    {track: 1, note: 67, velocity: 80, start_tick: 0, duration_ticks: 1920},
    {track: 2, note: 36, velocity: 100, start_tick: 0, duration_ticks: 1920},
    {track: 3, note: 36, velocity: 100, start_tick: 0, duration_ticks: 120},
    {track: 3, note: 38, velocity: 100, start_tick: 480, duration_ticks: 120}
  ]
  notes.each { |note| raise SDL.GetError unless GrappleC.AddChipNote(composer, note) }
  raise SDL.GetError unless GrappleC.AddChipTempo(composer, 0, 120)
  song = GrappleC.BuildChipSong(composer, 1920)
  raise SDL.GetError unless song
  player = GrappleC.PlayChipSong(song, false)
  GrappleC.DestroyChipSong(song)
  GrappleC.DestroyChipComposer(composer)
else
  player = GrappleC.PlayChipFile(path, false)
end
raise SDL.GetError unless player
style_arg = ARGV[1]
if style_arg && !style_arg.empty?
  style = if style_arg.match?(%r{[/\\.]})
            GrappleC.LoadChipStyle(style_arg)
          else
            GrappleC.GetChipStyle(style_arg)
          end
  raise SDL.GetError unless style && GrappleC.SetChipPlayerStyle(player, style)
end
config = GrappleC.ConfigCreate
GrappleC.ConfigSetTitle(config, 'Chiptune from Ruby')
GrappleC.ConfigSetDesignSize(config, 800, 240)
GrappleC.ConfigSetAutoMount(config, false)
GrappleC.ConfigSetHeadless(config, !SDL.getenv('GRAPPLE_HEADLESS').nil?)
engine = GrappleC.CreateEngine(config)
GrappleC.ConfigDestroy(config)
raise SDL.GetError unless engine
GrappleC.OnFixedUpdate(engine) do |dt|
  GrappleC.EngineQuit(engine) unless GrappleC.ChipPlayerPlaying(player)
end
GrappleC.OnRender(engine) do |alpha|
  renderer = GrappleC.EngineRenderer(engine)
  SDL.SetRenderDrawColor(renderer, 20, 22, 42, 255)
  SDL.RenderClear(renderer)
  SDL.SetRenderDrawColor(renderer, 150, 220, 255, 255)
  GrappleC.RenderDebugText(renderer, 24, 40, "Ruby chiptune: #{path}")
end
raise SDL.GetError unless GrappleC.Run(engine)
GrappleC.DestroyChipPlayer(player)
GrappleC.DestroyEngine(engine)
MIX.Quit
SDL.Quit
