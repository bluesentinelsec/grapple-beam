# Latin Translator — Ruby.
#
# Three buttons, one per Latin word. Clicking one puts its English
# translation in a label. Escape closes the window.

WORDS = [
  { latin: "sinister", english: "left" },
  { latin: "dexter", english: "right" },
  { latin: "medium", english: "center" }
].freeze

engine = Grapple.engine(
  title: "Latin Translator - Ruby",
  window: { width: 640, height: 400 },
  presentation: :native,   # coordinates are pixels, so a resize reflows
  auto_mount: false        # nothing to load: this is drawn from widgets
)

ui = Grapple.ui(engine)
panel = ui.panel(title: "Latin Translator", padding: 12, spacing: 8)

panel.label(text: "Click a Latin word:")

answer = panel.label(text: "", align: :center, height: "2em")

words = panel.row(height: "2.4em", spacing: 8)
WORDS.each do |word|
  words.button(text: word[:latin]) { answer.set(word[:english]) }
end

panel.button(text: "Clear", width: :fit, align: :right) { answer.set("") }

panel.label(text: "Resize the window; the layout reflows.", align: :center)

engine.on_update { engine.quit if engine.key_pressed?("escape") }

engine.run
