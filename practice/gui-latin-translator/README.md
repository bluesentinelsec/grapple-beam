# Latin Translator

The same small program four times: three buttons, one per Latin word, each
putting its English translation in a label.

```sh
./run_all.sh          # all four in turn; Escape closes one and starts the next
./run_all.sh lua ruby # only the ones named
```

| File | Language | Lines |
| --- | --- | --- |
| [`latin.lua`](latin.lua) | Lua | 69 |
| [`latin.rb`](latin.rb) | Ruby | 70 |
| [`latin_c.c`](latin_c.c) | C | 148 |
| [`latin_cpp.cpp`](latin_cpp.cpp) | C++ | 160 |

These are practice problems for finding rough edges in the API, not shipped
examples. They are built behind `GRAPPLE_BEAM_BUILD_PRACTICE` (off by
default) and nothing here is installed.

## What they exercise

Every version declares its widgets once and never touches them again: the
engine owns the loop, the UI owns its own input and drawing, and the layout
is written in font-relative units so resizing the window reflows it rather
than scaling it. Between them they cover the declarative engine
(`Grapple.engine{...}`), the retained widget tree, `fit` widths that are
measured rather than estimated, alignment, and callbacks per widget.

The two scripts are written the way a Love2D or Godot script is: named
callbacks the engine calls, no loop, and no `run()` — registering handlers
is enough, and the runner starts the engine once the file has finished.
Handlers are named functions rather than anonymous ones, and each is given
the widget that fired, so one function serves the whole row of buttons.

They are a few lines longer than the closure-based version they replaced,
which is the trade: naming a handler costs a line and buys a name.
