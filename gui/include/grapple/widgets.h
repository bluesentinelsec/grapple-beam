/**
 * @file widgets.h
 * @brief Widgets you declare once, in the shape Tk taught everyone.
 *
 * Original Grapple code (zlib), over the immediate-mode GUI in
 * <grapple/gui.h>.
 *
 * Named widgets.h rather than ui.h because <grapple/ui.h> is already the
 * C++ RAII wrapper around the immediate-mode GUI, and two headers with one
 * name resolve by include order — which is a bug that only appears in the
 * language whose include path happens to lose.
 *
 * The immediate-mode API underneath is the right tool for a debug overlay
 * that is different every frame. It is the wrong tool for a dialog that
 * never changes, because it makes you re-declare the dialog sixty times a
 * second, gives widgets no identity to hang a callback on, and leaves every
 * text field's buffer for you to own. This layer fixes those three things
 * and nothing else: it emits the same nk_* calls you would have written.
 *
 *   Grapple_Ui *ui = Grapple_CreateUi(gui);
 *   Grapple_UiWidget *panel = Grapple_UiPanel(ui, &(Grapple_UiPanelDef){
 *       .title = "Settings", .fill = true, .padding = 8 });
 *
 *   Grapple_UiLabel(panel, &(Grapple_UiLabelDef){ .text = "Volume" });
 *   Grapple_UiSlider(panel, &(Grapple_UiSliderDef){
 *       .value = 0.5f, .on_change = VolumeChanged, .user = app });
 *   Grapple_UiButton(panel, &(Grapple_UiButtonDef){
 *       .text = "Close", .width = GRAPPLE_UI_FIT,
 *       .align = GRAPPLE_UI_RIGHT, .on_click = Close, .user = app });
 *
 * Then once per frame, after the game has drawn:
 *
 *   Grapple_UiDraw(ui);
 *
 * That is the whole per-frame cost. Input arrives through the GUI's event
 * sink, which the engine already drives — see Grapple_GuiEventSink.
 *
 * ### Lengths
 *
 * Every width and height is a Grapple_UiLength, so a layout can be written
 * in the unit that makes it correct rather than the unit that happens to be
 * pixels today:
 *
 *   GRAPPLE_UI_PX(24)    24 pixels
 *   GRAPPLE_UI_EM(2.4f)  2.4 lines of the current font — survives a font change
 *   GRAPPLE_UI_PCT(0.25f) a quarter of the parent
 *   GRAPPLE_UI_FIT       exactly as wide as the widget's own content
 *   {0}                  stretch to share what is left (the default)
 *
 * GRAPPLE_UI_FIT is the one an immediate-mode API cannot offer: it measures
 * the text with the live font before asking for the space, which is only
 * possible because the widget was declared before the frame it appears in.
 *
 * ### What is not here
 *
 * Anything this layer has no widget for, reach through for it — a raw node
 * hands you the nk_context inside the layout, so the escape hatch costs one
 * call rather than a rewrite:
 *
 *   Grapple_UiRaw(panel, &(Grapple_UiRawDef){ .draw = DrawChart, .user = app });
 */
#ifndef GRAPPLE_WIDGETS_H
#define GRAPPLE_WIDGETS_H

#include <grapple/gui.h>
#include <grapple/nuklear.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_Ui Grapple_Ui;

/** One node of the tree: a container or a widget. Owned by its parent. */
typedef struct Grapple_UiWidget Grapple_UiWidget;

/** How a length is meant. */
typedef enum Grapple_UiUnit
{
    GRAPPLE_UI_UNIT_STRETCH = 0, /**< share what the fixed children left */
    GRAPPLE_UI_UNIT_PX,
    GRAPPLE_UI_UNIT_EM,      /**< multiples of the font's line height */
    GRAPPLE_UI_UNIT_PCT,     /**< fraction of the parent, 0..1 */
    GRAPPLE_UI_UNIT_FIT      /**< the widget's own content width */
} Grapple_UiUnit;

typedef struct Grapple_UiLength
{
    float value;
    Grapple_UiUnit unit;
} Grapple_UiLength;

/* Spelled twice because a compound literal is C only: C++ rejects it, and a
   header that only compiles in one of the two languages is not a header this
   project can ship. */
#ifdef __cplusplus
#define GRAPPLE_UI_LENGTH(v, u) (Grapple_UiLength{(float)(v), (u)})
#else
#define GRAPPLE_UI_LENGTH(v, u) ((Grapple_UiLength){(float)(v), (u)})
#endif

#define GRAPPLE_UI_PX(v) GRAPPLE_UI_LENGTH((v), GRAPPLE_UI_UNIT_PX)
#define GRAPPLE_UI_EM(v) GRAPPLE_UI_LENGTH((v), GRAPPLE_UI_UNIT_EM)
#define GRAPPLE_UI_PCT(v) GRAPPLE_UI_LENGTH((v), GRAPPLE_UI_UNIT_PCT)
#define GRAPPLE_UI_FIT GRAPPLE_UI_LENGTH(0.0f, GRAPPLE_UI_UNIT_FIT)
#define GRAPPLE_UI_STRETCH GRAPPLE_UI_LENGTH(0.0f, GRAPPLE_UI_UNIT_STRETCH)

/** Where a widget narrower than its slot sits in it. */
typedef enum Grapple_UiAlign
{
    GRAPPLE_UI_LEFT = 0,
    GRAPPLE_UI_CENTER,
    GRAPPLE_UI_RIGHT
} Grapple_UiAlign;

/** Which side of an image point an annotation occupies. */
typedef enum Grapple_UiImageAnnotationSide
{
    GRAPPLE_UI_ANNOTATION_RIGHT = 0,
    GRAPPLE_UI_ANNOTATION_LEFT,
    GRAPPLE_UI_ANNOTATION_ABOVE,
    GRAPPLE_UI_ANNOTATION_BELOW
} Grapple_UiImageAnnotationSide;

/** Something happened to `widget`. `user` is whatever the def carried. */
typedef void (*Grapple_UiCallback)(Grapple_UiWidget *widget, void *user);

/** Draw into `ctx` at the point the tree reached. See Grapple_UiRaw. */
typedef void (*Grapple_UiDrawFn)(struct nk_context *ctx, void *user);

/* --- the tree ----------------------------------------------------------- */

/**
 * Create a UI over `gui`. The GUI is borrowed and must outlive it.
 * \returns a new UI (destroy with Grapple_DestroyUi), or NULL.
 */
extern Grapple_Ui *Grapple_CreateUi(Grapple_Gui *gui);

/**
 * Create a UI and the GUI under it, with the platform's own interface font.
 *
 * The font search is the reason this exists. Nuklear's built-in face is a
 * 13-pixel bitmap that looks wrong the moment it is scaled onto a high-DPI
 * display, so every program was opening with a list of candidate paths and
 * a loop. This does that once: San Francisco on macOS, Segoe UI on Windows,
 * DejaVu or Liberation on Linux, and the built-in face if none of them are
 * there.
 *
 * `font_points` is a point size, scaled by the window's pixel density, so
 * 15 is 15pt on any display. Zero picks a sensible default.
 *
 * The GUI is owned: Grapple_DestroyUi destroys it too.
 */
extern Grapple_Ui *Grapple_OpenUi(SDL_Renderer *renderer, float font_points);

/** The GUI underneath, for the nk_* API or a second UI over the same one. */
extern Grapple_Gui *Grapple_UiGui(Grapple_Ui *ui);

/**
 * This UI packaged as an event sink.
 *
 *   Grapple_EngineSetEventSink(engine, &(Grapple_EventSink){0} ...)
 *
 * is the long way; hand this to Grapple_EngineSetEventSink and the engine
 * feeds the UI every frame with nothing further to write.
 */
extern Grapple_EventSink Grapple_UiEventSink(Grapple_Ui *ui);

/** Destroy the UI and every widget in it. Safe on NULL. */
extern void Grapple_DestroyUi(Grapple_Ui *ui);

/**
 * Draw the whole tree. Call once a frame, after the game has drawn and
 * before presenting — the same place Grapple_GuiRender goes, which this
 * calls for you.
 */
extern void Grapple_UiDraw(Grapple_Ui *ui);

/** Grapple_UiDraw with the signature an engine overlay slot wants, so no
 *  function-pointer cast is needed to install it. */
extern void Grapple_UiDrawCallback(void *ui);

/**
 * Tell the UI a frame has passed, drawn or not.
 *
 * Drawing belongs to whoever owns the loop, which means forgetting it gives
 * a blank window and no explanation. Call this once a frame and a UI that
 * has widgets and has never been drawn will say so, once.
 */
extern void Grapple_UiNoteFrame(Grapple_Ui *ui);

/** Grapple_UiNoteFrame with the signature an engine overlay slot wants. */
extern void Grapple_UiNoteFrameCallback(void *ui);

/** True while the pointer is over the UI, so a game can ignore that click. */
extern bool Grapple_UiWantsInput(Grapple_Ui *ui);

/* --- containers --------------------------------------------------------- */

typedef struct Grapple_UiPanelDef
{
    const char *title;      /**< shown in the title bar; NULL for none */
    bool fill;              /**< cover the whole window, ignoring x/y/w/h */
    float x, y, width, height; /**< in pixels, when fill is false */
    float padding;          /**< inside the panel's edges */
    float spacing;          /**< between its children */
    bool movable;
    bool scrollable;
    bool no_border;
} Grapple_UiPanelDef;

/** A window. Every other widget lives inside one. */
extern Grapple_UiWidget *Grapple_UiPanel(Grapple_Ui *ui, const Grapple_UiPanelDef *def);

typedef struct Grapple_UiStripDef
{
    Grapple_UiLength height; /**< a row's height, or a column's per-child default */
    float spacing;           /**< between children; < 0 inherits the parent's */
    Grapple_UiAlign align;   /**< where narrow children sit */
} Grapple_UiStripDef;

/** Children side by side, sharing the width. Tk's `pack -side left`. */
extern Grapple_UiWidget *Grapple_UiRow(Grapple_UiWidget *parent,
                                           const Grapple_UiStripDef *def);

/** Children stacked, each on its own row. Tk's `pack -side top`. */
extern Grapple_UiWidget *Grapple_UiColumn(Grapple_UiWidget *parent,
                                              const Grapple_UiStripDef *def);

/** Settings for widgets that share one drawing area. */
typedef struct Grapple_UiOverlayDef
{
    Grapple_UiLength height; /**< Requested height; stretch uses one text row. */
} Grapple_UiOverlayDef;

/**
 * @brief Layer direct child widgets in one drawing area.
 *
 * Children are drawn in creation order, so later children appear over
 * earlier ones. Use Grapple_UiPlace to position each child within the area.
 * This is the retained equivalent of Tk's `place` geometry manager.
 *
 * @param parent Parent panel or column that owns the overlay.
 * @param def Height setting, or NULL for a one-line default.
 * @return The new overlay, or NULL on error.
 */
extern Grapple_UiWidget *Grapple_UiOverlay(Grapple_UiWidget *parent,
                                           const Grapple_UiOverlayDef *def);

/* --- widgets ------------------------------------------------------------ */

typedef struct Grapple_UiLabelDef
{
    const char *text;
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;   /**< of the text inside the widget */
    bool wrap;
} Grapple_UiLabelDef;

extern Grapple_UiWidget *Grapple_UiLabel(Grapple_UiWidget *parent,
                                             const Grapple_UiLabelDef *def);

typedef struct Grapple_UiButtonDef
{
    const char *text;
    const char *value;     /**< what it means, as opposed to what it says */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;   /**< where the button sits in its slot */
    bool disabled;
    Grapple_UiCallback on_click;
    void *user;
} Grapple_UiButtonDef;

extern Grapple_UiWidget *Grapple_UiButton(Grapple_UiWidget *parent,
                                              const Grapple_UiButtonDef *def);

typedef struct Grapple_UiCheckDef
{
    const char *text;
    bool checked;
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_change;
    void *user;
} Grapple_UiCheckDef;

extern Grapple_UiWidget *Grapple_UiCheck(Grapple_UiWidget *parent,
                                             const Grapple_UiCheckDef *def);

typedef struct Grapple_UiSliderDef
{
    float value;
    float min;
    float max;   /**< 1.0 if min and max are both left at zero */
    float step;  /**< 1/100th of the range if zero */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_change;
    void *user;
} Grapple_UiSliderDef;

extern Grapple_UiWidget *Grapple_UiSlider(Grapple_UiWidget *parent,
                                              const Grapple_UiSliderDef *def);

typedef struct Grapple_UiEntryDef
{
    const char *text;    /**< the starting contents */
    int capacity;        /**< 256 if zero */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_change; /**< Called once per draw when user input changes the text.
                                     Focus, Enter alone, and Grapple_UiSetText do not trigger it. */
    void *user;
} Grapple_UiEntryDef;

/** A text field. The buffer belongs to the widget, which is the point. */
extern Grapple_UiWidget *Grapple_UiEntry(Grapple_UiWidget *parent,
                                             const Grapple_UiEntryDef *def);

/**
 * A list of choices with one of them picked.
 *
 * `options` is a NULL-terminated array of labels, copied. The widget owns
 * the selection: `Grapple_UiValue` reads it back as an index, and
 * `Grapple_UiText` gives the chosen label.
 */
/**
 * A picture, optionally one you can click.
 *
 * Give it either a `path`, loaded through SDL_image and owned by the
 * widget, or a `texture` you loaded yourself and continue to own.
 *
 * With `on_click` set it is a button that happens to be a picture, which is
 * what a toolbar is made of.
 */
typedef struct Grapple_UiImageDef
{
    const char *path;      /**< any format SDL_image reads; owned by the widget */
    const char *value;     /**< what this picture means; see Grapple_UiValueText */
    SDL_Texture *texture;  /**< or a texture you own; wins over path */
    Grapple_GuiImageMode mode;
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_click;
    void *user;
} Grapple_UiImageDef;

extern Grapple_UiWidget *Grapple_UiImage(Grapple_UiWidget *parent,
                                             const Grapple_UiImageDef *def);

/** Settings for a short label attached to a point in an image. */
typedef struct Grapple_UiImageAnnotationDef
{
    const char *text;                   /**< Displayed text, copied. */
    float x;                            /**< Horizontal image coordinate, 0..1. */
    float y;                            /**< Vertical image coordinate, 0..1. */
    Grapple_UiImageAnnotationSide side; /**< Side of the point holding the label. */
    float gap;                          /**< Gap between point and label, in pixels. */
} Grapple_UiImageAnnotationDef;

/**
 * @brief Add a label that stays attached to a point in a zoomed image.
 *
 * The image must be a direct child of an overlay. Coordinates refer to the
 * image content rather than its widget slot, so the annotation follows the
 * same aspect-preserving resize and letterboxing as the image. The annotation
 * is owned by the image and may be shown or hidden like any other widget.
 *
 * @param image Image widget that owns and positions the annotation.
 * @param def Text, normalized coordinates, side, and pixel gap.
 * @return The new annotation, or NULL when the arguments or parent layout are
 *         invalid.
 * @pre `def->x` and `def->y` are each between 0 and 1 inclusive.
 */
extern Grapple_UiWidget *Grapple_UiImageAnnotation(Grapple_UiWidget *image,
                                                   const Grapple_UiImageAnnotationDef *def);

/**
 * @brief Replace the file-backed image displayed by an image widget.
 *
 * The path is loaded through the installed image loader, or SDL_image when
 * no custom loader is installed. The widget owns the resulting texture. If
 * loading fails, the widget keeps displaying its previous image.
 *
 * @param widget Image widget to update.
 * @param path Path understood by the active image loader.
 * @return true when the new image was loaded and installed; false on error.
 */
extern bool Grapple_UiSetImagePath(Grapple_UiWidget *widget, const char *path);

/**
 * @brief Replace the texture displayed by an image widget.
 *
 * The texture remains owned by the caller and must outlive its use by the
 * widget. Passing NULL clears the image.
 *
 * @param widget Image widget to update.
 * @param texture Borrowed texture, or NULL to clear the image.
 * @return true when the widget was updated; false when widget is invalid or
 *         is not an image widget.
 */
extern bool Grapple_UiSetImageTexture(Grapple_UiWidget *widget, SDL_Texture *texture);

/**
 * How `path` turns into a texture.
 *
 * Left alone it is SDL_image, which reads everything a game is likely to
 * ship. Install a loader when the file is not where the name says it is —
 * an atlas, a pack file, a cache in front of the disk — and `path` becomes
 * whatever that loader decides it means.
 *
 * Passing NULL restores the default. It is global because it is a property
 * of the program rather than of one panel.
 */
typedef SDL_Texture *(*Grapple_UiImageLoader)(SDL_Renderer *renderer, const char *path,
                                                  void *user);

extern void Grapple_UiSetImageLoader(Grapple_UiImageLoader loader, void *user);

/**
 * A native message box, and the reason it is here rather than left to SDL:
 * the argument order of SDL_ShowSimpleMessageBox puts the window last and
 * the flags first, which is three things to remember for "say this".
 *
 * Blocks until dismissed, which is what a message box is for.
 */
extern void Grapple_UiMessage(Grapple_Ui *ui, const char *title, const char *text);

typedef struct Grapple_UiSelectDef
{
    const char *const *options;
    int selected;          /**< index into options; 0 if out of range */
    bool as_radio;         /**< draw every option, rather than a dropdown */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_change;
    void *user;
} Grapple_UiSelectDef;

/** A dropdown: one row, opening onto the list. */
extern Grapple_UiWidget *Grapple_UiSelect(Grapple_UiWidget *parent,
                                              const Grapple_UiSelectDef *def);

/** The same choice as a column of radio buttons, for a short list where
 *  seeing every option at once is worth the space. */
extern Grapple_UiWidget *Grapple_UiRadio(Grapple_UiWidget *parent,
                                             const Grapple_UiSelectDef *def);

typedef struct Grapple_UiProgressDef
{
    float value;
    float max;    /**< 1.0 if left at zero */
    bool editable; /**< draggable, like a slider without the handle */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiAlign align;
    Grapple_UiCallback on_change;
    void *user;
} Grapple_UiProgressDef;

extern Grapple_UiWidget *Grapple_UiProgress(Grapple_UiWidget *parent,
                                                const Grapple_UiProgressDef *def);

/** Which option a select or radio has: the index, and its label. */
extern int Grapple_UiSelected(Grapple_UiWidget *widget);
extern void Grapple_UiSetSelected(Grapple_UiWidget *widget, int index);
extern int Grapple_UiOptionCount(Grapple_UiWidget *widget);
extern const char *Grapple_UiOption(Grapple_UiWidget *widget, int index);

typedef struct Grapple_UiSpacerDef
{
    Grapple_UiLength width;
    Grapple_UiLength height;
} Grapple_UiSpacerDef;

/** Blank space, for when the answer really is "push the rest over". */
extern Grapple_UiWidget *Grapple_UiSpacer(Grapple_UiWidget *parent,
                                              const Grapple_UiSpacerDef *def);

typedef struct Grapple_UiRawDef
{
    Grapple_UiDrawFn draw;
    void *user;
    Grapple_UiLength width;
    Grapple_UiLength height;
} Grapple_UiRawDef;

/**
 * A hole in the layer, holding a real nk_context.
 *
 * Every wrapper over an immediate-mode library fails to cover something,
 * and the answer to that must never be "start again in the lower API".
 */
extern Grapple_UiWidget *Grapple_UiRaw(Grapple_UiWidget *parent,
                                           const Grapple_UiRawDef *def);

/**
 * @brief Position a widget inside its overlay parent.
 *
 * Pixel, em, and percentage coordinates are supported. Percentages are
 * relative to the overlay's width or height. The position is the widget's
 * top-left corner; its existing width and height determine its extent.
 *
 * @param widget Direct child of an overlay.
 * @param x Horizontal coordinate within the overlay.
 * @param y Vertical coordinate within the overlay.
 * @return true on success; false when widget is NULL or its parent is not
 *         an overlay.
 */
extern bool Grapple_UiPlace(Grapple_UiWidget *widget, Grapple_UiLength x, Grapple_UiLength y);

/* --- reading and changing a widget -------------------------------------- */

/** Replace a label's, a button's or an entry's text. Copied. */
extern void Grapple_UiSetText(Grapple_UiWidget *widget, const char *text);

/** The current text: an entry's contents, or whatever SetText last set. */
extern const char *Grapple_UiText(Grapple_UiWidget *widget);

extern void Grapple_UiSetChecked(Grapple_UiWidget *widget, bool checked);
extern bool Grapple_UiChecked(Grapple_UiWidget *widget);

extern void Grapple_UiSetValue(Grapple_UiWidget *widget, float value);
extern float Grapple_UiValue(Grapple_UiWidget *widget);

/** A hidden widget takes no space and receives no input. */
extern void Grapple_UiSetVisible(Grapple_UiWidget *widget, bool visible);
extern bool Grapple_UiVisible(Grapple_UiWidget *widget);

extern void Grapple_UiSetDisabled(Grapple_UiWidget *widget, bool disabled);
extern bool Grapple_UiDisabled(Grapple_UiWidget *widget);

/**
 * Where this widget was drawn last frame, in pixels. False before its first
 * frame, or while it is hidden.
 *
 * For anchoring something to a widget — a tooltip, a popup, a highlight —
 * and for a test that wants to click exactly where a button ended up rather
 * than where it guessed one would be.
 */
extern bool Grapple_UiBounds(Grapple_UiWidget *widget, float *x, float *y, float *width,
                               float *height);

/**
 * Somewhere to hang a value on a widget: Tk's -value, a tag, a card's name.
 *
 * Without it a handler shared by several widgets has to tell them apart by
 * their text, which reads well right up until the text is a label the user
 * sees and the value is not — an image has no text of its own at all, and a
 * button's is what it says rather than what it means.
 *
 * The string is copied. NULL clears it.
 */
extern void Grapple_UiSetValueText(Grapple_UiWidget *widget, const char *value);

/** What Grapple_UiSetValueText last set, or "" — never NULL. */
extern const char *Grapple_UiValueText(Grapple_UiWidget *widget);

/** Whatever the def carried, for a callback that needs its own context. */
extern void *Grapple_UiUser(Grapple_UiWidget *widget);

/**
 * Fire a widget's action as though it had been clicked or changed.
 *
 * Tk calls this invoke, and it is the same idea: a keyboard shortcut, a
 * menu item and a test all want to do what the button does without a mouse
 * being involved. Does nothing for a widget with no handler.
 */
extern void Grapple_UiInvoke(Grapple_UiWidget *widget);

/** Remove a widget and its children. The parent forgets it. */
extern void Grapple_UiRemove(Grapple_UiWidget *widget);

/** Remove every child, keeping the container — for a list that is rebuilt. */
extern void Grapple_UiClear(Grapple_UiWidget *parent);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_WIDGETS_H */
