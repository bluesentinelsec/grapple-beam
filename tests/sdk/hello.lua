-- The smallest thing that proves the runner has the bindings loaded and can
-- reach the engine: no -l on the command line, so this also checks that the
-- language was inferred from the extension.
local cfg = GrappleC.ConfigCreate()
GrappleC.ConfigSetHeadless(cfg, true)
GrappleC.ConfigSetManualClock(cfg, true)
GrappleC.ConfigSetAutoMount(cfg, false)
local engine = GrappleC.CreateEngine(cfg)
GrappleC.ConfigDestroy(cfg)
assert(engine ~= nil, "the runner could not create an engine")
GrappleC.EngineAdvance(engine, 16666667)
GrappleC.EngineTick(engine)
assert(GrappleC.EngineFrameCount(engine) >= 1)
GrappleC.DestroyEngine(engine)
print("runner ok: lua on " .. SDL.GetPlatform())
