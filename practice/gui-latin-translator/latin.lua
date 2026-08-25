-- Latin Translator — Lua.
--
-- Three buttons, one per Latin word. Clicking one puts its English
-- translation in a label. Escape closes the window.

local WORDS = {
  { latin = "sinister", english = "left" },
  { latin = "dexter", english = "right" },
  { latin = "medium", english = "center" },
}

local engine = Grapple.engine{
  title = "Latin Translator - Lua",
  window = { width = 640, height = 400 },
  presentation = "native",   -- coordinates are pixels, so a resize reflows
  auto_mount = false,        -- nothing to load: this is drawn from widgets
}

local ui = Grapple.ui(engine)
local panel = ui:panel{ title = "Latin Translator", padding = 12, spacing = 8 }

panel:label{ text = "Click a Latin word:" }

local answer = panel:label{ text = "", align = "center", height = "2em" }

local words = panel:row{ height = "2.4em", spacing = 8 }
for _, word in ipairs(WORDS) do
  words:button{
    text = word.latin,
    on_click = function() answer:set(word.english) end,
  }
end

panel:button{
  text = "Clear",
  width = "fit",
  align = "right",
  on_click = function() answer:set("") end,
}

panel:label{ text = "Resize the window; the layout reflows.", align = "center" }

engine:on_update(function()
  if engine:key_pressed("escape") then engine:quit() end
end)

engine:run()
