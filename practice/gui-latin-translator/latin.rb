# Latin Translator — Ruby.
#
# Same program as latin_c.c, played by the grapple runner. A GUI-only program,
# so there is no engine here: the window, the renderer and the loop are ours.
# Escape closes it.
#
# Three spellings to note, none of which the C version has to think about:
#   NK.begin_  `begin` is a Ruby keyword, so the binding adds a trailing _
#   NK.end_    same for `end`
#   NK::NK_*   Nuklear constants keep their prefix, unlike SDL's which drop it
#
# Note that Lua spells the first one `NK.begin` — `begin` is not a keyword
# there. SCRIPT_API.md documents only the Lua spelling, so following the docs
# in Ruby gets you `undefined method 'begin' for Module`.

WINDOW_WIDTH = 520
WINDOW_HEIGHT = 320

# The whole dictionary. The buttons draw themselves from it, so adding a word
# is one line.
WORDS = [
  { latin: "sinister", english: "left" },
  { latin: "dexter", english: "right" },
  { latin: "medium", english: "center" }
].freeze

raise "could not start SDL" unless SDL.Init(SDL::INIT_VIDEO)

window = SDL.CreateWindow("Latin Translator - Ruby", WINDOW_WIDTH, WINDOW_HEIGHT, 0)
raise "could not open a window" if window.nil?

renderer = SDL.CreateRenderer(window, nil)
raise "could not create a renderer" if renderer.nil?

gui = GrappleC.CreateGui(renderer, nil, 18)
raise "could not create the gui" if gui.nil?

ctx = GrappleC.GuiContext(gui)

# One event object, reused every poll, rather than allocating per event.
event = GrappleC.EventCreate()

# What the label shows before anything has been clicked.
translation = ""
running = true

while running
  GrappleC.GuiInputBegin(gui)
  while SDL.PollEvent(event)
    kind = GrappleC.EventType(event)
    if kind == SDL::EVENT_QUIT
      running = false
    elsif kind == SDL::EVENT_KEY_DOWN &&
          GrappleC.EventKeyScancode(event) == SDL::SCANCODE_ESCAPE
      running = false
    end
    GrappleC.GuiProcessEvent(gui, event)
  end
  GrappleC.GuiInputEnd(gui)

  if NK.begin_(ctx, "Latin Translator", NK.rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
               NK::NK_WINDOW_NO_SCROLLBAR)
    NK.layout_row_dynamic(ctx, 28, 1)
    NK.label(ctx, "Implementation: Ruby", NK::NK_TEXT_CENTERED)

    NK.layout_row_dynamic(ctx, 12, 1)
    NK.label(ctx, "Click a Latin word:", NK::NK_TEXT_LEFT)

    NK.layout_row_dynamic(ctx, 44, WORDS.length)
    WORDS.each do |word|
      translation = word[:english] if NK.button_label(ctx, word[:latin])
    end

    NK.layout_row_dynamic(ctx, 20, 1)
    NK.label(ctx, "English:", NK::NK_TEXT_LEFT)

    NK.layout_row_dynamic(ctx, 40, 1)
    NK.label(ctx, translation, NK::NK_TEXT_CENTERED)

    NK.layout_row_dynamic(ctx, 20, 1)
    NK.label(ctx, "Escape closes this window.", NK::NK_TEXT_CENTERED)
  end
  NK.end_(ctx)

  SDL.SetRenderDrawColor(renderer, 32, 32, 40, 255)
  SDL.RenderClear(renderer)
  GrappleC.GuiRender(gui)
  SDL.RenderPresent(renderer)
end

GrappleC.EventDestroy(event)
GrappleC.DestroyGui(gui)
SDL.DestroyRenderer(renderer)
SDL.DestroyWindow(window)
SDL.Quit()
