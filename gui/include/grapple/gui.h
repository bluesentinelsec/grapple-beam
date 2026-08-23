/**
 * @file gui.h
 * @brief SDL3 backend for Nuklear (Grapple GUI).
 *
 * Original Grapple code (zlib) around the vendored Nuklear. The backend
 * owns the nk_context, the baked font atlas (uploaded once as an
 * SDL_Texture), clipboard wiring, and the render path: each frame the
 * Nuklear draw list is converted to vertex buffers and issued through
 * batched SDL_RenderGeometry calls with scissor clipping.
 *
 * Frame shape (works standalone or inside a game loop):
 *
 *   Grapple_GuiInputBegin(gui);
 *   while (SDL_PollEvent(&ev)) { Grapple_GuiProcessEvent(gui, &ev); }
 *   Grapple_GuiInputEnd(gui);
 *
 *   struct nk_context *ctx = Grapple_GuiContext(gui);
 *   if (nk_begin(ctx, "Tools", nk_rect(20, 20, 260, 300),
 *                NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
 *       nk_layout_row_dynamic(ctx, 0, 1);
 *       if (nk_button_label(ctx, "Save")) { save(); }
 *   }
 *   nk_end(ctx);
 *
 *   SDL_RenderClear(renderer);
 *   ...draw your game...
 *   Grapple_GuiRender(gui);        // composites the UI over the frame
 *   SDL_RenderPresent(renderer);
 *
 * For layout without hard-coded coordinates, see grapple/gui_grid.h.
 */
#ifndef GRAPPLE_GUI_H
#define GRAPPLE_GUI_H

#include <SDL3/SDL.h>
#include <grapple/nuklear.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_Gui Grapple_Gui;

/** Which glyphs to bake into the font atlas.
 *
 *  A font atlas is rasterised once, at creation, so this decides for the
 *  lifetime of the GUI what text can be drawn: anything outside the range
 *  renders as the font's missing-glyph box, whatever font you supply.
 *  Nuklear's own default is LATIN1, which is why an em dash or a curly
 *  quote pasted into a label comes out wrong.
 *
 *  Wider ranges cost atlas space and baking time — CJK is thousands of
 *  glyphs — so pick the narrowest that covers your strings. The ranges
 *  above LATIN1 need a font that actually contains those glyphs; the
 *  built-in default font has only ASCII. */
typedef enum Grapple_GuiGlyphRange
{
    GRAPPLE_GUI_GLYPHS_LATIN1 = 0,   /**< U+0020..U+00FF (the default) */
    GRAPPLE_GUI_GLYPHS_PUNCTUATION,  /**< Latin-1 plus dashes, quotes,
                                            bullets, ellipsis, arrows and
                                            common currency — what UI text
                                            actually uses */
    GRAPPLE_GUI_GLYPHS_CYRILLIC,
    GRAPPLE_GUI_GLYPHS_CHINESE,
    GRAPPLE_GUI_GLYPHS_KOREAN
} Grapple_GuiGlyphRange;

/**
 * Create a GUI backend bound to `renderer`.
 * \param font_data  optional TTF bytes for the UI font (copied); NULL uses
 *                   Nuklear's embedded default font.
 * \param font_size  glyph height in pixels; <= 0 selects 13.
 * \returns a new backend (destroy with Grapple_DestroyGui), or NULL.
 *
 * Bakes GRAPPLE_GUI_GLYPHS_LATIN1; use Grapple_CreateGuiWithGlyphs for
 * anything else.
 */
extern Grapple_Gui *Grapple_CreateGui(SDL_Renderer *renderer, const void *font_data,
                                          size_t font_len, float font_size);

/** As Grapple_CreateGui, choosing which glyphs the atlas covers.
 *
 * Pair a wide range with a font that has the glyphs — pass the same TTF you
 * would give Grapple::TTF for that language. */
extern Grapple_Gui *Grapple_CreateGuiWithGlyphs(SDL_Renderer *renderer,
                                                    const void *font_data, size_t font_len,
                                                    float font_size,
                                                    Grapple_GuiGlyphRange range);

extern void Grapple_DestroyGui(Grapple_Gui *gui);

/** The Nuklear context — use the full nk_* API with it. */
extern struct nk_context *Grapple_GuiContext(Grapple_Gui *gui);

/** Begin collecting input for this frame. */
extern void Grapple_GuiInputBegin(Grapple_Gui *gui);

/**
 * Feed one SDL event (mouse, wheel, keyboard, text input). Call between
 * InputBegin and InputEnd. \returns true if the event type is one the GUI
 * consumes.
 */
extern bool Grapple_GuiProcessEvent(Grapple_Gui *gui, const SDL_Event *event);

/** Finish input collection. */
extern void Grapple_GuiInputEnd(Grapple_Gui *gui);

/**
 * True while the pointer is over / interacting with any Nuklear window —
 * lets a game skip its own input handling when the UI has focus.
 */
extern bool Grapple_GuiWantsInput(Grapple_Gui *gui);

/** Pump this frame's SDL events into the GUI in one call: wraps
 *  InputBegin / SDL_PollEvent loop / ProcessEvent / InputEnd.
 *  \returns false when the application should quit (SDL_EVENT_QUIT or a
 *  window-close request), true to keep running.
 *
 *  Games that need the raw events themselves should keep using the
 *  InputBegin/ProcessEvent/InputEnd trio instead. This entry point is what
 *  makes the GUI drivable from Lua and Ruby, where SDL_Event (a union)
 *  cannot cross the script boundary. */
extern bool Grapple_GuiPumpEvents(Grapple_Gui *gui);

/** True when `scancode` (an SDL_Scancode) went down during the most recent
 *  pump/input pass. Gives Lua and Ruby keyboard input, which SDL's own
 *  keyboard-state API cannot provide across the binding boundary — e.g.
 *  quitting on Escape. */
extern bool Grapple_GuiKeyPressed(Grapple_Gui *gui, int scancode);

/** A Button that opens a native "open file" picker when clicked.
 *
 *  Use this instead of a plain button plus Grapple_ShowOpenFileDialog if
 *  the program must work in a browser. Safari (and, in stricter modes,
 *  other browsers) only opens a file picker from inside the real click
 *  handler, but an SDL app processes clicks a frame later, by which time
 *  the browser has withdrawn permission. On web this keeps a transparent
 *  <input type="file"> positioned over the button so the user's click
 *  lands on a real DOM element; on desktop it is an ordinary button that
 *  starts the dialog. Collect the result with Grapple_DialogStatus, the
 *  same way in both cases.
 *
 *  One such button is supported at a time — enough for a File menu, and
 *  all a modal picker can be. Because the overlay swallows the click, the
 *  button does not show Nuklear's hover/press shading on web.
 *
 *  \returns true when a dialog was started (desktop only; on web the
 *  browser drives it). */
extern bool Grapple_GuiOpenFileButton(Grapple_Gui *gui, const char *label,
                                        const char *filter_name,
                                        const char *filter_pattern);

/** A "save file" button, the mirror image of Grapple_GuiOpenFileButton.
 *
 *  Browsers hand a file to the user by clicking a download link, and that
 *  click — like opening a picker — only counts inside the real gesture, so
 *  on web this parks a transparent <a download> over the button. The bytes
 *  therefore have to be ready *before* the click: pass the document's
 *  current contents every frame, not just when something changed. They are
 *  only re-blobbed when they actually differ.
 *
 *  Desktop shows the native save dialog and writes the file once the user
 *  has chosen a path, which takes a few frames; keep calling with the same
 *  arguments until it returns true.
 *
 *  \param filename the name to suggest, e.g. "untitled.txt".
 *  \param data,len the document's current bytes.
 *  \returns true on the frame the file was written (desktop) or handed to
 *  the browser (web). Grapple_GuiSavedPath then reports where. */
extern bool Grapple_GuiSaveFileButton(Grapple_Gui *gui, const char *label,
                                        const char *filename, const void *data, size_t len);

/** Where Grapple_GuiSaveFileButton last saved: an absolute path on
 *  desktop, the download's file name on web. NULL before the first save. */
extern const char *Grapple_GuiSavedPath(Grapple_Gui *gui);

/** Show `text` as a tooltip for the **next** widget, with desktop-style
 *  timing: it appears only after the pointer has rested on that widget for
 *  the tooltip delay, and disappears again the moment the pointer moves.
 *  (Nuklear's own nk_tooltip draws immediately and stays up for as long as
 *  the pointer is inside the widget, which is not how tooltips behave.)
 *
 *  Call it immediately before declaring the widget, like
 *  nk_widget_is_hovered:
 *
 *      Grapple_GuiTooltip(gui, "Create a new document");
 *      nk_button_label(ctx, "New");
 *
 *  \returns true on the frames where the tooltip is actually displayed. */
extern bool Grapple_GuiTooltip(Grapple_Gui *gui, const char *text);

/** Milliseconds the pointer must rest before Grapple_GuiTooltip shows
 *  (default 1000). 0 shows immediately. */
extern void Grapple_GuiSetTooltipDelay(Grapple_Gui *gui, int delay_ms);

/** The current tooltip delay in milliseconds. */
extern int Grapple_GuiTooltipDelay(Grapple_Gui *gui);

/* --- grid layout for scripts -------------------------------------------
 * <grapple/gui_grid.h> is the full helper, but it takes a caller-owned
 * Grapple_GuiGrid and a float array of column weights — neither of which
 * can cross a script boundary. These wrap one grid owned by the Gui object:
 * set the weights (optional; equal columns otherwise), begin, place cells,
 * end. C and C++ can use either API.
 */

/** Set the weight of one column for the next Grapple_GuiGridBeginOwned.
 *  Columns default to weight 1 (equal widths). */
extern bool Grapple_GuiGridWeight(Grapple_Gui *gui, int column, float weight);

/** Begin a grid of `columns` columns. row_height <= 0 auto-sizes to the
 *  font. Weights set by Grapple_GuiGridWeight apply, then reset. */
extern bool Grapple_GuiGridBeginOwned(Grapple_Gui *gui, int columns, float row_height);

/** Advance to the next cell (call before each widget). */
extern void Grapple_GuiGridCellOwned(Grapple_Gui *gui);

/** Advance to the next cell, spanning `span` columns. */
extern void Grapple_GuiGridCellSpanOwned(Grapple_Gui *gui, int span);

/** Finish the current row early. */
extern void Grapple_GuiGridNextRowOwned(Grapple_Gui *gui);

/** Finish the grid. */
extern void Grapple_GuiGridEndOwned(Grapple_Gui *gui);

/** Most overlay draws accepted in one frame; see Grapple_GuiDrawTextureOverlay. */
#define GRAPPLE_GUI_MAX_OVERLAYS 32

/** How Grapple_GuiImage fits a texture into its widget slot, mirroring
 *  the PictureBox sizing modes desktop toolkits offer. */
typedef enum Grapple_GuiImageMode
{
    GRAPPLE_GUI_IMAGE_STRETCH = 0, /**< fill the slot, ignoring aspect ratio */
    GRAPPLE_GUI_IMAGE_ZOOM,        /**< largest fit inside, aspect preserved */
    GRAPPLE_GUI_IMAGE_CENTER,      /**< native pixel size, centred */
    GRAPPLE_GUI_IMAGE_FILL         /**< cover the slot, aspect preserved, cropped */
} Grapple_GuiImageMode;

/** Draw an SDL texture in the next widget slot.
 *
 *  Nuklear's own nk_image takes a struct whose handle is a union, so it
 *  cannot cross a script boundary; this takes the SDL_Texture directly and
 *  works from C, C++, Lua and Ruby alike. Advances the layout exactly like
 *  any other widget. Returns false when the slot is clipped away or the
 *  arguments are invalid. */
extern bool Grapple_GuiImage(Grapple_Gui *gui, SDL_Texture *texture,
                               Grapple_GuiImageMode mode);

/** Draw a texture at an explicit rectangle instead of in a widget slot.
 *
 *  Game UI rarely wants one image per layout cell: an inventory slot draws
 *  its own background, icon and stack count inside a single rectangle it
 *  measured with nk_widget_bounds, and an editor viewport paints wherever
 *  it likes. This draws into the current window's canvas, so it is clipped
 *  by that window and layered with it, and it does **not** advance the
 *  layout — call it between widgets, not instead of one.
 *
 *  Must be called inside a window (between nk_begin and nk_end). */
extern bool Grapple_GuiDrawTexture(Grapple_Gui *gui, SDL_Texture *texture, SDL_FRect rect,
                                     Grapple_GuiImageMode mode);

/** Draw a texture above every panel, after the GUI has been rendered.
 *
 *  The case this exists for is drag-and-drop: the icon under the cursor
 *  has to float over whatever it is being dragged across, including
 *  windows declared after the one it came from. Nuklear's own overlay
 *  buffer is rebuilt for the cursor each frame, so these are queued by the
 *  GUI and flushed by Grapple_GuiRender in call order.
 *
 *  The queue is emptied every frame, so call it each frame the ghost
 *  should be visible. Up to GRAPPLE_GUI_MAX_OVERLAYS per frame. */
extern bool Grapple_GuiDrawTextureOverlay(Grapple_Gui *gui, SDL_Texture *texture,
                                            SDL_FRect rect, Grapple_GuiImageMode mode);

/** How many draw commands the last frame produced.
 *
 *  A cheap "is my UI getting expensive" number for a debug overlay: it
 *  counts what the GUI actually handed the renderer, so hiding a panel
 *  visibly drops it. Zero before the first Grapple_GuiRender. */
extern int Grapple_GuiDrawCommandCount(Grapple_Gui *gui);

/** Bytes of Nuklear's command buffer used by the last frame — the other
 *  half of the same picture, and an early warning that a panel is
 *  allocating more than it should. */
extern int Grapple_GuiMemoryUsed(Grapple_Gui *gui);

/** Font sizes baked at creation. Nuklear bakes glyphs into one atlas up
 *  front, so every size a program needs must exist before the first frame;
 *  these three are always available and are crisp (not resampled). Sizes
 *  are relative to the font size passed to Grapple_CreateGui. */
typedef enum Grapple_GuiFontSize
{
    GRAPPLE_GUI_FONT_SMALL = 0, /**< 0.75x */
    GRAPPLE_GUI_FONT_NORMAL,    /**< 1.0x, the default */
    GRAPPLE_GUI_FONT_LARGE      /**< 1.5x */
} Grapple_GuiFontSize;

/** Make `which` the font for subsequent widgets (persists across frames). */
extern bool Grapple_GuiSetFont(Grapple_Gui *gui, Grapple_GuiFontSize which);

/** Use `which` for the next widgets only; undo with Grapple_GuiPopFont.
 *  Pushes nest. */
extern bool Grapple_GuiPushFont(Grapple_Gui *gui, Grapple_GuiFontSize which);

/** Undo `count` Grapple_GuiPushFont calls. */
extern void Grapple_GuiPopFont(Grapple_Gui *gui, int count);

/** Height in pixels of the currently selected font. */
extern float Grapple_GuiFontHeight(Grapple_Gui *gui);

/** Themable colours (see Grapple_GuiPushStyleColor). Nuklear's own
 *  style stack takes union-typed style items, which cannot cross a script
 *  boundary — this is the theming entry point Lua and Ruby can use, and a
 *  convenience for C and C++. */
typedef enum Grapple_GuiStyleColor
{
    GRAPPLE_GUI_COLOR_WINDOW_BACKGROUND = 0,
    GRAPPLE_GUI_COLOR_TEXT,
    GRAPPLE_GUI_COLOR_BUTTON,
    GRAPPLE_GUI_COLOR_BUTTON_HOVER,
    GRAPPLE_GUI_COLOR_BUTTON_TEXT,
    GRAPPLE_GUI_COLOR_HEADER
} Grapple_GuiStyleColor;

/** Push one themed colour. Pushes nest; undo them with
 *  Grapple_GuiPopStyleColor in LIFO order (typically once per frame,
 *  after nk_end). Returns false on a bad argument or stack overflow. */
extern bool Grapple_GuiPushStyleColor(Grapple_Gui *gui, Grapple_GuiStyleColor which,
                                        SDL_Color color);

/** Undo `count` pushes made by Grapple_GuiPushStyleColor. */
extern void Grapple_GuiPopStyleColor(Grapple_Gui *gui, int count);

/** Pixel density the GUI is rendering at (1.0 on a normal display, 2.0 on
 *  a Retina window created with SDL_WINDOW_HIGH_PIXEL_DENSITY). The GUI
 *  lays out and hit-tests in pixels; multiply your own point-based sizes by
 *  this to stay density-independent. */
extern float Grapple_GuiScale(Grapple_Gui *gui);

/**
 * Convert this frame's draw list and render it through the bound renderer,
 * then clear Nuklear state for the next frame.
 */
extern bool Grapple_GuiRender(Grapple_Gui *gui);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_GUI_H */
