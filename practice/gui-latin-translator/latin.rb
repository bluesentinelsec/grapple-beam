# Latin Translator — Ruby.
#
# Same program as latin_c.c: resizable window, the engine's loop, and a
# weighted grid instead of hard-coded coordinates. Escape closes it.
#
# Three places where the script version cannot follow the C one:
#
#   * There is no OnEvent hook. C registers hooks.event and hands each
#     SDL_Event to Grapple_GuiProcessEvent; a script has no way to see an
#     event at all, so the mouse is fed to Nuklear from the engine's polled
#     input instead (NK.input_motion / NK.input_button below). That is enough
#     for buttons, and would not be enough for a text field.
#   * The grid must be the gui-owned one. GuiGridCreate() returns a grid that
#     no bound function can begin, because Grapple_GuiGridBegin takes a float
#     array and did not cross the binding.
#   * **This one runs with the built-in font**, unlike every other version
#     here. mruby has no File class, SDL_LoadFile is not bound, and
#     PHYSFS.readBytes is not bound either, so a Ruby script cannot read the
#     bytes of a system font — or of any other file on disk. Compare the text
#     in this window with the C or Lua one to see what that costs on a Retina
#     panel.
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

config = GrappleC.ConfigCreate()
GrappleC.ConfigSetTitle(config, "Latin Translator - Ruby (built-in font)")
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
  # nil font: the built-in. See the note at the top — a Ruby script has no way
  # to read a better one off disk. 15 is a point size, multiplied by the
  # window's pixel density inside CreateGui.
  $gui = GrappleC.CreateGui(GrappleC.EngineRenderer($engine), nil, 15)
  if $gui.nil?
    false
  else
    $ctx = GrappleC.GuiContext($gui)
    true
  end
end

GrappleC.OnUpdate($engine) do |_dt|
  GrappleC.EngineQuit($engine) if GrappleC.KeyPressed($engine, SDL::SCANCODE_ESCAPE)

  # No OnEvent hook, so Nuklear is fed from polled state. The engine reports
  # the pointer in the same pixel space the GUI hit-tests in, so no scaling is
  # needed here.
  mouse_x, mouse_y = GrappleC.MousePosition($engine)
  NK.input_begin($ctx)
  NK.input_motion($ctx, mouse_x, mouse_y)
  NK.input_button($ctx, NK::NK_BUTTON_LEFT, mouse_x, mouse_y,
                  GrappleC.MouseDown($engine, GrappleC::GRAPPLE_MOUSE_LEFT))
  NK.input_end($ctx)

  # The panel fills the window, in pixels, re-read every frame — which is what
  # makes a resize reflow instead of scale.
  width, height = GrappleC.EnginePixelSize($engine)

  if NK.begin_($ctx, "Latin Translator", NK.rect(0, 0, width, height),
               NK::NK_WINDOW_NO_SCROLLBAR)
    line = GrappleC.GuiFontHeight($gui)

    # One column, so a full-width row. Height 0 means "one line of the current
    # font", which is why nothing here is in pixels.
    GrappleC.GuiGridBeginOwned($gui, 1, 0)
    GrappleC.GuiGridCellOwned($gui)
    NK.label($ctx, "Implementation: Ruby", NK::NK_TEXT_CENTERED)
    GrappleC.GuiGridCellOwned($gui)
    NK.label($ctx, "Click a Latin word:", NK::NK_TEXT_LEFT)
    GrappleC.GuiGridEndOwned($gui)

    # A second grid only because a row height is per-grid, not per-row, and
    # buttons want more than one line.
    GrappleC.GuiGridBeginOwned($gui, WORDS.length, line * 2.4)
    WORDS.each do |word|
      GrappleC.GuiGridCellOwned($gui)
      $translation = word[:english] if NK.button_label($ctx, word[:latin])
    end
    GrappleC.GuiGridEndOwned($gui)

    # Label and answer on one row, the answer twice as wide. Weights are set
    # before Begin and reset by it, which is how the array parameter was
    # avoided for scripts.
    GrappleC.GuiGridWeight($gui, 0, 1.0)
    GrappleC.GuiGridWeight($gui, 1, 2.0)
    GrappleC.GuiGridBeginOwned($gui, 2, line * 2.0)
    GrappleC.GuiGridCellOwned($gui)
    NK.label($ctx, "English:", NK::NK_TEXT_RIGHT)
    GrappleC.GuiGridCellOwned($gui)
    NK.label($ctx, $translation, NK::NK_TEXT_LEFT)
    GrappleC.GuiGridEndOwned($gui)

    GrappleC.GuiGridBeginOwned($gui, 1, 0)
    GrappleC.GuiGridCellOwned($gui)
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
  GrappleC.DestroyGui($gui)
  $gui = nil
end

GrappleC.Run($engine)
GrappleC.DestroyEngine($engine)
