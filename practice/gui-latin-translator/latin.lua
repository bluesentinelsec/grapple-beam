-- Latin Translator — Lua.
--
-- Same program as latin_c.c: resizable window, the engine's loop, the
-- platform UI font at display density, and a weighted grid instead of
-- hard-coded coordinates. Escape closes it.
--
-- Two places where the script version cannot follow the C one:
--
--   * There is no OnEvent hook. C registers hooks.event and hands each
--     SDL_Event to Grapple_GuiProcessEvent; a script has no way to see an
--     event at all, so the mouse is fed to Nuklear from the engine's polled
--     input instead (NK.input_motion / NK.input_button below). That is
--     enough for buttons, and would not be enough for a text field.
--   * The grid must be the gui-owned one. GuiGridCreate() returns a grid
--     that no bound function can begin, because Grapple_GuiGridBegin takes a
--     float array and did not cross the binding.
--
-- Spellings worth noting: NK.end_ (`end` is a keyword) and NK.NK_* for
-- Nuklear constants, which keep their prefix where SDL's drop it.

local WINDOW_WIDTH = 640
local WINDOW_HEIGHT = 400

-- The whole dictionary. The buttons draw themselves from it, so adding a
-- word is one line.
local WORDS = {
  { latin = "sinister", english = "left" },
  { latin = "dexter", english = "right" },
  { latin = "medium", english = "center" },
}

-- The platform's UI font, or nil to fall back to Nuklear's built-in. Loading
-- the system font rather than embedding one keeps a megabyte of glyphs out of
-- the repository and gets each platform's own look; the built-in is a 13px
-- bitmap face that does not survive being scaled up on a Retina panel.
local FONT_CANDIDATES = {
  "/System/Library/Fonts/SFNS.ttf",
  "/System/Library/Fonts/Helvetica.ttc",
  "C:/Windows/Fonts/segoeui.ttf",
  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
  "/usr/share/fonts/TTF/DejaVuSans.ttf",
}

local function load_ui_font()
  for _, path in ipairs(FONT_CANDIDATES) do
    local file = io.open(path, "rb")
    if file then
      local bytes = file:read("a")
      file:close()
      if bytes and #bytes > 0 then
        return bytes
      end
    end
  end
  return nil
end

local config = GrappleC.ConfigCreate()
GrappleC.ConfigSetTitle(config, "Latin Translator - Lua")
GrappleC.ConfigSetWindowSize(config, WINDOW_WIDTH, WINDOW_HEIGHT)

-- Coordinates are pixels and the window is resizable, which together are what
-- make this behave like a desktop program rather than a game: more window
-- means more room, not bigger widgets.
GrappleC.ConfigSetPresentation(config, GrappleC.GRAPPLE_PRESENT_NATIVE)
GrappleC.ConfigSetResizable(config, true)
GrappleC.ConfigSetHighDpi(config, true)

-- Nothing to mount: this program draws itself out of widgets.
GrappleC.ConfigSetAutoMount(config, false)

local engine = assert(GrappleC.CreateEngine(config), "could not create an engine")

local gui = nil
local ctx = nil
local translation = ""

GrappleC.OnLoad(engine, function()
  -- 15 is a point size: CreateGui multiplies it by the window's pixel
  -- density, so this is 15pt on any display.
  gui = GrappleC.CreateGui(GrappleC.EngineRenderer(engine), load_ui_font(), 15)
  if not gui then
    return false
  end
  ctx = GrappleC.GuiContext(gui)
  return true
end)

GrappleC.OnUpdate(engine, function(_dt)
  if GrappleC.KeyPressed(engine, SDL.SCANCODE_ESCAPE) then
    GrappleC.EngineQuit(engine)
  end

  -- No OnEvent hook, so Nuklear is fed from polled state. The engine reports
  -- the pointer in the same pixel space the GUI hit-tests in, so no scaling
  -- is needed here.
  local mouse_x, mouse_y = GrappleC.MousePosition(engine)
  NK.input_begin(ctx)
  NK.input_motion(ctx, mouse_x, mouse_y)
  NK.input_button(ctx, NK.NK_BUTTON_LEFT, mouse_x, mouse_y,
    GrappleC.MouseDown(engine, GrappleC.GRAPPLE_MOUSE_LEFT))
  NK.input_end(ctx)

  -- The panel fills the window, in pixels, re-read every frame — which is
  -- what makes a resize reflow instead of scale.
  local width, height = GrappleC.EnginePixelSize(engine)

  if NK.begin(ctx, "Latin Translator", NK.rect(0, 0, width, height),
    NK.NK_WINDOW_NO_SCROLLBAR) then
    local line = GrappleC.GuiFontHeight(gui)

    -- One column, so a full-width row. Height 0 means "one line of the
    -- current font", which is why nothing here is in pixels.
    GrappleC.GuiGridBeginOwned(gui, 1, 0)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, "Implementation: Lua", NK.NK_TEXT_CENTERED)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, "Click a Latin word:", NK.NK_TEXT_LEFT)
    GrappleC.GuiGridEndOwned(gui)

    -- A second grid only because a row height is per-grid, not per-row, and
    -- buttons want more than one line.
    GrappleC.GuiGridBeginOwned(gui, #WORDS, line * 2.4)
    for _, word in ipairs(WORDS) do
      GrappleC.GuiGridCellOwned(gui)
      if NK.button_label(ctx, word.latin) then
        translation = word.english
      end
    end
    GrappleC.GuiGridEndOwned(gui)

    -- Label and answer on one row, the answer twice as wide. Weights are set
    -- before Begin and reset by it, which is how the array parameter was
    -- avoided for scripts.
    GrappleC.GuiGridWeight(gui, 0, 1.0)
    GrappleC.GuiGridWeight(gui, 1, 2.0)
    GrappleC.GuiGridBeginOwned(gui, 2, line * 2.0)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, "English:", NK.NK_TEXT_RIGHT)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, translation, NK.NK_TEXT_LEFT)
    GrappleC.GuiGridEndOwned(gui)

    GrappleC.GuiGridBeginOwned(gui, 1, 0)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, "Resize the window; the layout reflows. Escape closes.",
      NK.NK_TEXT_CENTERED)
    GrappleC.GuiGridEndOwned(gui)
  end
  NK.end_(ctx)
end)

-- Over the finished frame, above any post-processing.
GrappleC.OnPostRender(engine, function()
  GrappleC.GuiRender(gui)
end)

GrappleC.OnUnload(engine, function()
  GrappleC.DestroyGui(gui)
  gui = nil
end)

GrappleC.Run(engine)
GrappleC.DestroyEngine(engine)
