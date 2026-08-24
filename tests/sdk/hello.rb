# The Ruby half of the same check.
cfg = GrappleC.ConfigCreate
GrappleC.ConfigSetHeadless(cfg, true)
GrappleC.ConfigSetManualClock(cfg, true)
GrappleC.ConfigSetAutoMount(cfg, false)
engine = GrappleC.CreateEngine(cfg)
GrappleC.ConfigDestroy(cfg)
raise "the runner could not create an engine" if engine.nil?
GrappleC.EngineAdvance(engine, 16666667)
GrappleC.EngineTick(engine)
raise "no frames" unless GrappleC.EngineFrameCount(engine) >= 1
GrappleC.DestroyEngine(engine)
puts "runner ok: ruby on #{SDL.GetPlatform}"
