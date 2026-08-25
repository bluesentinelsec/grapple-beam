# Latin Translator — Ruby.
#
# Same program as latin_c.c: resizable window, the engine's loop, and a
# weighted grid instead of hard-coded coordinates. Escape closes it.
#
# One place where the script version differs from the C one: the grid must be
# the gui-owned family, because Grapple_GuiGridBegin takes a float array and
# did not cross the binding.
#
# The other two differences are gone. GrappleC.AttachGui hands the GUI to the
# engine, which brackets Nuklear's input around its own event pump; and
# SDL.LoadFile reads a real filesystem path, which mruby could not do at all
# before — no File class, and Grapple.read_file only sees the VFS.
#
# Spellings worth noting: NK.begin_ and NK.end_ (both are Ruby keywords) and
# NK::NK_* for Nuklear constants, which keep their prefix where SDL's drop it.

WINDOW_WIDTH = 640
WINDOW_HEIGHT = 400

# The whole dictionary. The buttons draw themselves from it, so adding a word
# is one line.
WORDS = [
  { latin: "sinister", english: "left" },
  { latin: "dexter", english: "right" },
  { latin: "medium", english: "center" }
].freeze

# The platform's UI font, or nil to fall back to Nuklear's built-in. The
# built-in is a 13px bitmap face that does not survive being scaled up on a
# Retina panel.
FONT_CANDIDATES = [
  "/System/Library/Fonts/SFNS.ttf",
  "/System/Library/Fonts/Helvetica.ttc",
  "C:/Windows/Fonts/segoeui.ttf",
  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
  "/usr/share/fonts/TTF/DejaVuSans.ttf"
].freeze

def load_ui_font
  FONT_CANDIDATES.each do |path|
    bytes = SDL.LoadFile(path)
    return bytes if bytes && bytes.length.positive?
  end
  nil
end

config = GrappleC.ConfigCreate()
GrappleC.ConfigSetTitle(config, "Latin Translator - Ruby")
GrappleC.ConfigSetWindowSize(config, WINDOW_WIDTH, WINDOW_HEIGHT)

# Coordinates are pixels and the window is resizable, which together are what
# make this behave like a desktop program rather than a game: more window
# means more room, not bigger widgets.
GrappleC.ConfigSetPresentation(config, GrappleC::GRAPPLE_PRESENT_NATIVE)
GrappleC.ConfigSetResizable(config, true)
GrappleC.ConfigSetHighDpi(config, true)

# Nothing to mount: this program draws itself out of widgets.
GrappleC.ConfigSetAutoMount(config, false)

$engine = GrappleC.CreateEngine(config)
raise "could not create an engine" if $engine.nil?

$gui = nil
$ctx = nil
$translation = ""

GrappleC.OnLoad($engine) do
  # 15 is a point size, multiplied by the window's pixel density inside
  # CreateGui, so this is 15pt on any display.
  $gui = GrappleC.CreateGui(GrappleC.EngineRenderer($engine), load_ui_font, 15)
  if $gui.nil?
    false
  else
    $ctx = GrappleC.GuiContext($gui)
    # The engine now drives the GUI's input for us.
    GrappleC.AttachGui($engine, $gui)
    true
  end
end

GrappleC.OnUpdate($engine) do |_dt|
  GrappleC.EngineQuit($engine) if GrappleC.KeyPressed($engine, SDL::SCANCODE_ESCAPE)

  # The panel fills the window, in pixels, re-read every frame — which is what
  # makes a resize reflow instead of scale.
  width, height = GrappleC.EnginePixelSize($engine)

  if NK.begin_($ctx, "Latin Translator", NK.rect(0, 0, width, height),
               NK::NK_WINDOW_NO_SCROLLBAR)
    line = GrappleC.GuiFontHeight($gui)

    # One grid for the whole panel. Three equal columns, so a full-width row is
    # a span of three and "English:" plus its answer fall out as a 1:2 split
    # with no weights to declare.
    GrappleC.GuiGridBeginOwned($gui, 3, 0)
    GrappleC.GuiGridSpacingOwned($gui, 8, 8)

    GrappleC.GuiGridCellSpanOwned($gui, 3)
    NK.label($ctx, "Implementation: Ruby", NK::NK_TEXT_CENTERED)
    GrappleC.GuiGridCellSpanOwned($gui, 3)
    NK.label($ctx, "Click a Latin word:", NK::NK_TEXT_LEFT)

    # Buttons want more than a line of text — this row only.
    GrappleC.GuiGridRowHeightOwned($gui, line * 2.4)
    WORDS.each do |word|
      GrappleC.GuiGridCellOwned($gui)
      $translation = word[:english] if NK.button_label($ctx, word[:latin])
    end

    GrappleC.GuiGridRowHeightOwned($gui, line * 2.0)
    GrappleC.GuiGridCellOwned($gui)
    NK.label($ctx, "English:", NK::NK_TEXT_RIGHT)
    GrappleC.GuiGridCellSpanOwned($gui, 2)
    NK.label($ctx, $translation, NK::NK_TEXT_LEFT)

    # A quarter of the row, hugging the right.
    GrappleC.GuiGridRowHeightOwned($gui, line * 1.8)
    GrappleC.GuiGridCellPartOwned($gui, 3, 0.25, 2) # 2 = align right
    $translation = "" if NK.button_label($ctx, "Clear")

    GrappleC.GuiGridCellSpanOwned($gui, 3)
    NK.label($ctx, "Resize the window; the layout reflows. Escape closes.",
             NK::NK_TEXT_CENTERED)

    GrappleC.GuiGridEndOwned($gui)
  end
  NK.end_($ctx)
end

# Over the finished frame, above any post-processing.
GrappleC.OnPostRender($engine) do
  GrappleC.GuiRender($gui)
end

GrappleC.OnUnload($engine) do
  # Detach before the GUI it points at goes away.
  GrappleC.AttachGui($engine, nil)
  GrappleC.DestroyGui($gui)
  $gui = nil
end

GrappleC.Run($engine)
GrappleC.DestroyEngine($engine)
