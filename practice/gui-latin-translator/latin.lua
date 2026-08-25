-- Latin Translator — Lua.
--
-- Same program as latin_c.c, played by the grapple runner. A GUI-only
-- program, so there is no engine here: the window, the renderer and the loop
-- are ours. Escape closes it.
--
-- Two spellings to note, both of which the C version does not have to think
-- about:
--   NK.end_   `end` is a Lua keyword, so the binding adds a trailing _
--   NK.NK_*   Nuklear constants keep their prefix, unlike SDL.* which drops it

local WINDOW_WIDTH = 520
local WINDOW_HEIGHT = 320

-- The whole dictionary. The buttons draw themselves from it, so adding a word
-- is one line.
local WORDS = {
  { latin = "sinister", english = "left" },
  { latin = "dexter", english = "right" },
  { latin = "medium", english = "center" },
}

assert(SDL.Init(SDL.INIT_VIDEO), "could not start SDL")

local window = assert(SDL.CreateWindow("Latin Translator - Lua", WINDOW_WIDTH, WINDOW_HEIGHT, 0),
  "could not open a window")
local renderer = assert(SDL.CreateRenderer(window, nil), "could not create a renderer")
local gui = assert(GrappleC.CreateGui(renderer, nil, 18), "could not create the gui")
local ctx = GrappleC.GuiContext(gui)

-- One event object, reused every poll, rather than allocating per event.
local event = GrappleC.EventCreate()

-- What the label shows before anything has been clicked.
local translation = ""
local running = true

while running do
  GrappleC.GuiInputBegin(gui)
  while SDL.PollEvent(event) do
    local kind = GrappleC.EventType(event)
    if kind == SDL.EVENT_QUIT then
      running = false
    elseif kind == SDL.EVENT_KEY_DOWN and
        GrappleC.EventKeyScancode(event) == SDL.SCANCODE_ESCAPE then
      running = false
    end
    GrappleC.GuiProcessEvent(gui, event)
  end
  GrappleC.GuiInputEnd(gui)

  if NK.begin(ctx, "Latin Translator", NK.rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
    NK.NK_WINDOW_NO_SCROLLBAR) then
    NK.layout_row_dynamic(ctx, 28, 1)
    NK.label(ctx, "Implementation: Lua", NK.NK_TEXT_CENTERED)

    NK.layout_row_dynamic(ctx, 12, 1)
    NK.label(ctx, "Click a Latin word:", NK.NK_TEXT_LEFT)

    NK.layout_row_dynamic(ctx, 44, #WORDS)
    for _, word in ipairs(WORDS) do
      if NK.button_label(ctx, word.latin) then
        translation = word.english
      end
    end

    NK.layout_row_dynamic(ctx, 20, 1)
    NK.label(ctx, "English:", NK.NK_TEXT_LEFT)

    NK.layout_row_dynamic(ctx, 40, 1)
    NK.label(ctx, translation, NK.NK_TEXT_CENTERED)

    NK.layout_row_dynamic(ctx, 20, 1)
    NK.label(ctx, "Escape closes this window.", NK.NK_TEXT_CENTERED)
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
