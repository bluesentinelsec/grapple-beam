-- Latin Translator — Lua.
--
-- Three buttons, one per Latin word. Clicking one puts its English
-- translation in a label. Escape closes the window.
--
-- Written the way a Love2D or Godot script is: named callbacks that the
-- engine calls, and no loop of our own. Registering on_load and on_update is
-- enough — the runner starts the engine once this file has finished, so
-- there is no run() at the bottom.

local ENGLISH = {
  sinister = "left",
  dexter = "right",
  medium = "center",
}

-- The order the buttons appear in. A table has no order of its own.
local LATIN = { "sinister", "dexter", "medium" }

local engine = Grapple.engine{
  title = "Latin Translator - Lua",
  window = { width = 640, height = 400 },
  presentation = "native",   -- coordinates are pixels, so a resize reflows
  auto_mount = false,        -- nothing to load: this is drawn from widgets
}

-- Kept here rather than inside on_load because both are garbage collected:
-- a ui that only a local held would be destroyed the moment load returned.
local ui
local answer

-- Handlers take the widget that fired, so one function serves all three
-- buttons: the button knows which word it is.
local function word_clicked(button)
  answer:set(ENGLISH[button:text()])
end

local function clear_clicked()
  answer:set("")
end

local function load()
  ui = Grapple.ui(engine)

  local panel = ui:panel{ title = "Latin Translator", padding = 12, spacing = 8 }
  panel:label{ text = "Click a Latin word:" }

  answer = panel:label{ text = "", align = "center", height = "2em" }

  local words = panel:row{ height = "2.4em", spacing = 8 }
  for _, latin in ipairs(LATIN) do
    words:button{ text = latin, on_click = word_clicked }
  end

  panel:button{ text = "Clear", width = "fit", align = "right",
                on_click = clear_clicked }

  panel:label{ text = "Resize the window; the layout reflows.", align = "center" }
  return true
end

local function update()
  if engine:key_pressed("escape") then
    engine:quit()
  end
end

local function post_render()
  ui:draw()
end

engine:on_load(load)
engine:on_update(update)
engine:on_post_render(post_render)
