# Latin Translator — Ruby.
#
# Three buttons, one per Latin word. Clicking one puts its English
# translation in a label. Escape closes the window.
#
# Written the way a Love2D or Godot script is: named callbacks that the
# engine calls, and no loop of our own. Registering on_load and on_update is
# enough — the runner starts the engine once this file has finished, so there
# is no run at the bottom.

ENGLISH = {
  "sinister" => "left",
  "dexter" => "right",
  "medium" => "center"
}.freeze

# The order the buttons appear in. A hash has no order worth relying on.
LATIN = %w[sinister dexter medium].freeze

$engine = Grapple.engine(
  title: "Latin Translator - Ruby",
  window: { width: 640, height: 400 },
  presentation: :native,   # coordinates are pixels, so a resize reflows
  auto_mount: false        # nothing to load: this is drawn from widgets
)

# Kept at the top level rather than inside load because both are garbage
# collected: a ui that only a local held would be destroyed the moment load
# returned.
$ui = nil
$answer = nil

# Handlers take the widget that fired, so one method serves all three
# buttons: the button knows which word it is.
def word_clicked(button)
  $answer.set(ENGLISH[button.text])
end

def clear_clicked(_button)
  $answer.set("")
end

def load_ui
  $ui = Grapple.ui($engine)

  panel = $ui.panel(title: "Latin Translator", padding: 12, spacing: 8)
  panel.label(text: "Click a Latin word:")

  $answer = panel.label(text: "", align: :center, height: "2em")

  words = panel.row(height: "2.4em", spacing: 8)
  LATIN.each do |latin|
    words.button(text: latin, on_click: method(:word_clicked))
  end

  panel.button(text: "Clear", width: :fit, align: :right,
               on_click: method(:clear_clicked))

  panel.label(text: "Resize the window; the layout reflows.", align: :center)
  true
end

# The delta is passed whether or not it is wanted, and Ruby counts
# arguments strictly where Lua ignores the extra one.
def update(_delta)
  $engine.quit if $engine.key_pressed?("escape")
end

$engine.on_load(&method(:load_ui))
$engine.on_update(&method(:update))
