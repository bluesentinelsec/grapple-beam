-- Latin Translator — Lua.
--
-- Same program as latin_c.c: resizable window, the engine's loop, the
-- platform UI font at display density, and a weighted grid instead of
-- hard-coded coordinates. Escape closes it.
--
-- One place where the script version differs from the C one: the grid must
-- be the gui-owned family, because Grapple_GuiGridBegin takes a float array
-- and did not cross the binding.
--
-- Input is not one of those places any more. GrappleC.AttachGui hands the
-- GUI to the engine, which brackets Nuklear's input around its own event
-- pump — the same arrangement C gets, and it covers the keyboard and wheel
-- that polled mouse state could not.
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
    -- SDL.LoadFile rather than io.open: it reads a real filesystem path in
    -- both languages, where Grapple.read_file only sees the VFS.
    local bytes = SDL.LoadFile(path)
    if bytes and #bytes > 0 then
      return bytes
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
  -- The engine now drives the GUI's input for us.
  GrappleC.AttachGui(engine, gui)
  return true
end)

GrappleC.OnUpdate(engine, function(_dt)
  if GrappleC.KeyPressed(engine, SDL.SCANCODE_ESCAPE) then
    GrappleC.EngineQuit(engine)
  end

  -- The panel fills the window, in pixels, re-read every frame — which is
  -- what makes a resize reflow instead of scale.
  local width, height = GrappleC.EnginePixelSize(engine)

  if NK.begin(ctx, "Latin Translator", NK.rect(0, 0, width, height),
    NK.NK_WINDOW_NO_SCROLLBAR) then
    local line = GrappleC.GuiFontHeight(gui)

    -- One grid for the whole panel. Three equal columns, so a full-width row
    -- is a span of three and "English:" plus its answer fall out as a 1:2
    -- split with no weights to declare.
    GrappleC.GuiGridBeginOwned(gui, 3, 0)
    GrappleC.GuiGridSpacingOwned(gui, 8, 8)

    GrappleC.GuiGridCellSpanOwned(gui, 3)
    NK.label(ctx, "Implementation: Lua", NK.NK_TEXT_CENTERED)
    GrappleC.GuiGridCellSpanOwned(gui, 3)
    NK.label(ctx, "Click a Latin word:", NK.NK_TEXT_LEFT)

    -- Buttons want more than a line of text — this row only.
    GrappleC.GuiGridRowHeightOwned(gui, line * 2.4)
    for _, word in ipairs(WORDS) do
      GrappleC.GuiGridCellOwned(gui)
      if NK.button_label(ctx, word.latin) then
        translation = word.english
      end
    end

    GrappleC.GuiGridRowHeightOwned(gui, line * 2.0)
    GrappleC.GuiGridCellOwned(gui)
    NK.label(ctx, "English:", NK.NK_TEXT_RIGHT)
    GrappleC.GuiGridCellSpanOwned(gui, 2)
    NK.label(ctx, translation, NK.NK_TEXT_LEFT)

    -- A quarter of the row, hugging the right.
    GrappleC.GuiGridRowHeightOwned(gui, line * 1.8)
    GrappleC.GuiGridCellPartOwned(gui, 3, 0.25, 2) -- 2 = align right
    if NK.button_label(ctx, "Clear") then
      translation = ""
    end

    GrappleC.GuiGridCellSpanOwned(gui, 3)
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
  -- Detach before the GUI it points at goes away.
  GrappleC.AttachGui(engine, nil)
  GrappleC.DestroyGui(gui)
  gui = nil
end)

GrappleC.Run(engine)
GrappleC.DestroyEngine(engine)
