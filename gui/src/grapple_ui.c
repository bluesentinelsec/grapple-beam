/*
 * grapple_ui.c — a retained widget tree over the immediate-mode GUI.
 *
 * Original Grapple code (zlib). Contract in grapple/widgets.h.
 *
 * The whole layer is one idea: keep the tree, and walk it once a frame
 * emitting the nk_* calls the caller would otherwise have written by hand.
 * Nothing here can do anything Nuklear could not; what it does is remember,
 * which is what buys widget identity, owned state, and shrink-to-fit.
 *
 * Layout resolves in pixels at draw time, because that is the first moment
 * the panel's real width is known. A row asks each child how wide it wants
 * to be, hands the fixed answers to nk_layout_row_template_push_static and
 * lets the rest share what is left; a column gives each child a row of its
 * own. That is Tk's pack, and it is about forty lines of it.
 */
#include "grapple_gui_internal.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <grapple/widgets.h>

#define MAX_TEXT 1024

typedef enum NodeKind
{
    NODE_PANEL = 0,
    NODE_ROW,
    NODE_COLUMN,
    NODE_OVERLAY,
    NODE_LABEL,
    NODE_BUTTON,
    NODE_CHECK,
    NODE_SLIDER,
    NODE_ENTRY,
    NODE_IMAGE,
    NODE_IMAGE_ANNOTATION,
    NODE_SELECT,
    NODE_RADIO,
    NODE_LIST,
    NODE_PROGRESS,
    NODE_SPACER,
    NODE_RAW
} NodeKind;

struct Grapple_UiWidget
{
    NodeKind kind;
    Grapple_Ui *ui;
    Grapple_UiWidget *parent;
    Grapple_UiWidget *first_child;
    Grapple_UiWidget *last_child;
    Grapple_UiWidget *next_sibling;

    /* Every node carries the same handful of layout knobs; which ones mean
       anything depends on the kind, and the defaults are all "stretch". */
    Grapple_UiLength width;
    Grapple_UiLength height;
    Grapple_UiLength place_x;
    Grapple_UiLength place_y;
    Grapple_UiAlign align;
    float spacing;
    float padding;
    bool visible;
    bool disabled;

    /* Panel */
    char title[128];
    bool fill;
    bool movable;
    bool scrollable;
    bool no_border;
    bool has_colors;
    SDL_Color background_color;
    SDL_Color text_color;
    float x, y, w, h;

    /* Widget state. An entry's buffer lives here rather than in the
       caller's hands, which is most of the ceremony this layer removes. */
    char *text;
    char *entry_previous_text;
    int capacity;
    bool checked;
    float value, min, max, step;
    bool wrap;
    bool editable;

    /* A select's choices, owned and NULL-terminated like the array it was
       given, so the caller's array need not outlive the call. */
    char **options;
    int option_count;
    int selected;

    /* What the widget means, as opposed to what it shows. */
    char *value_text;

    /* A picture. `owns_texture` is the difference between one this widget
       loaded and one the caller handed over and still owns. */
    SDL_Texture *texture;
    bool owns_texture;
    Grapple_GuiImageMode image_mode;

    /* An image annotation's point is normalized to the rendered image, not
       the possibly letterboxed widget slot. */
    float annotation_x;
    float annotation_y;
    float annotation_gap;
    Grapple_UiImageAnnotationSide annotation_side;

    /* Where this node landed last frame, so something can be anchored to
       it. Nuklear knows only during the frame; the tree remembers. */
    struct nk_rect last_bounds;
    bool drawn;

    Grapple_UiCallback on_click;
    Grapple_UiCallback on_change;
    Grapple_UiDrawFn draw;
    void *user;
};

struct Grapple_Ui
{
    Grapple_Gui *gui;
    bool owns_gui;
    /* Drawing is the caller's to do, so forgetting it is a silent blank
       window. Counted so the UI can say something rather than sulk. */
    bool ever_drawn;
    bool warned_not_drawn;
    int frames_since_created;
    Grapple_UiWidget *first_panel;
    Grapple_UiWidget *last_panel;
    /* Nuklear identifies some widgets by call order, so every node needs a
       name that is stable across frames rather than an index that shifts
       when a sibling is hidden. */
    unsigned next_id;
};

/* --- little helpers ------------------------------------------------------ */

static float LineHeight(Grapple_Ui *ui)
{
    const float h = Grapple_GuiFontHeight(ui->gui);
    return (h > 0.0f) ? h : 16.0f;
}

/* What one string is worth in pixels, asked of the live font. This is the
   call an immediate-mode layout cannot make in time, and the reason
   GRAPPLE_UI_FIT can be honest where GuiGridCellPart's fraction could not. */
static float TextWidth(Grapple_Ui *ui, const char *text)
{
    struct nk_context *ctx = Grapple_GuiContext(ui->gui);
    if (ctx == NULL || ctx->style.font == NULL || ctx->style.font->width == NULL ||
        text == NULL)
    {
        return 0.0f;
    }
    const int len = (int)SDL_strlen(text);
    return ctx->style.font->width(ctx->style.font->userdata, ctx->style.font->height, text,
                                  len);
}

/* The horizontal padding a widget of this kind puts around its text. */
static float Chrome(Grapple_UiWidget *node)
{
    struct nk_context *ctx = Grapple_GuiContext(node->ui->gui);
    if (ctx == NULL)
    {
        return 0.0f;
    }
    switch (node->kind)
    {
    case NODE_BUTTON:
        return ctx->style.button.padding.x * 2.0f + ctx->style.button.border * 2.0f + 8.0f;
    case NODE_CHECK:
    case NODE_RADIO:
        return ctx->style.checkbox.padding.x * 2.0f + LineHeight(node->ui) + 8.0f;
    case NODE_SELECT:
        return ctx->style.combo.button_padding.x * 2.0f + 8.0f;
    case NODE_ENTRY:
        return ctx->style.edit.padding.x * 2.0f + 8.0f;
    default:
        return 4.0f;
    }
}

static float Resolve(Grapple_UiWidget *node, Grapple_UiLength length, float available,
                     float content)
{
    switch (length.unit)
    {
    case GRAPPLE_UI_UNIT_PX:
        return length.value;
    case GRAPPLE_UI_UNIT_EM:
        return length.value * LineHeight(node->ui);
    case GRAPPLE_UI_UNIT_PCT:
        return length.value * available;
    case GRAPPLE_UI_UNIT_FIT:
        return content;
    case GRAPPLE_UI_UNIT_STRETCH:
    default:
        return 0.0f; /* the caller shares the remainder out */
    }
}

/* How wide this node would like to be if it got to choose. */
static float ContentWidth(Grapple_UiWidget *node)
{
    switch (node->kind)
    {
    case NODE_SELECT:
    case NODE_RADIO:
    case NODE_LIST: {
        /* Wide enough for the longest choice: a dropdown that fits only the
           option selected when it was built changes width as it is used. */
        float widest = 0.0f;
        for (int i = 0; i < node->option_count; ++i)
        {
            widest = SDL_max(widest, TextWidth(node->ui, node->options[i]));
        }
        return widest + Chrome(node) + LineHeight(node->ui);
    }
    case NODE_IMAGE:
    {
        /* "fit" for a picture means the picture's own width. */
        float w = 0.0f;
        if (node->texture != NULL)
        {
            SDL_GetTextureSize(node->texture, &w, NULL);
        }
        return w;
    }
    case NODE_LABEL:
    case NODE_IMAGE_ANNOTATION:
    case NODE_BUTTON:
    case NODE_CHECK:
    case NODE_ENTRY:
        return TextWidth(node->ui, node->text) + Chrome(node);
    default:
        return 0.0f;
    }
}

static float RowHeightOf(Grapple_UiWidget *node, float available_height)
{
    const float h = Resolve(node, node->height, available_height, LineHeight(node->ui));
    if (h > 0.0f)
    {
        return h;
    }
    /* One line plus the widget's own vertical chrome: a button that is
       exactly a line tall looks wrong in every toolkit. */
    switch (node->kind)
    {
    case NODE_RADIO:
    case NODE_LIST: {
        /* Every option is drawn, so the group needs the room for all of
           them rather than the one row a leaf usually gets. */
        const int count = (node->option_count > 0) ? node->option_count : 1;
        const struct nk_style_window *style = &Grapple_GuiContext(node->ui->gui)->style.window;
        return (LineHeight(node->ui) * 1.9f + style->spacing.y) * (float)count +
               2.0f * style->group_padding.y;
    }
    case NODE_IMAGE:
    {
        float h = 0.0f;
        if (node->texture != NULL)
        {
            SDL_GetTextureSize(node->texture, NULL, &h);
        }
        return (h > 0.0f) ? h : LineHeight(node->ui) * 4.0f;
    }
    case NODE_BUTTON:
    case NODE_ENTRY:
    case NODE_CHECK:
    case NODE_SELECT:
    case NODE_PROGRESS:
        return LineHeight(node->ui) * 1.9f;
    default:
        return LineHeight(node->ui) * 1.3f;
    }
}

static bool IsContainer(const Grapple_UiWidget *node)
{
    return node->kind == NODE_PANEL || node->kind == NODE_ROW || node->kind == NODE_COLUMN ||
           node->kind == NODE_OVERLAY;
}

static int VisibleChildren(Grapple_UiWidget *node)
{
    int n = 0;
    for (Grapple_UiWidget *c = node->first_child; c != NULL; c = c->next_sibling)
    {
        if (c->visible)
        {
            n++;
        }
    }
    return n;
}

/* --- construction -------------------------------------------------------- */

static Grapple_UiWidget *NewNode(Grapple_Ui *ui, Grapple_UiWidget *parent, NodeKind kind)
{
    Grapple_UiWidget *node = (Grapple_UiWidget *)SDL_calloc(1, sizeof(*node));
    if (node == NULL)
    {
        return NULL;
    }
    node->kind = kind;
    node->ui = ui;
    node->parent = parent;
    node->visible = true;
    node->spacing = -1.0f; /* inherit */
    node->max = 1.0f;
    ui->next_id++;

    if (parent != NULL)
    {
        if (parent->last_child == NULL)
        {
            parent->first_child = node;
        }
        else
        {
            parent->last_child->next_sibling = node;
        }
        parent->last_child = node;
    }
    return node;
}

static bool SetText(Grapple_UiWidget *node, const char *text, int capacity)
{
    const int want = (capacity > 0) ? capacity : MAX_TEXT;
    if (node->text == NULL || node->capacity < want)
    {
        char *grown = (char *)SDL_realloc(node->text, (size_t)want);
        if (grown == NULL)
        {
            return false;
        }
        node->text = grown;
        node->capacity = want;
    }
    if (text != NULL)
    {
        SDL_strlcpy(node->text, text, (size_t)node->capacity);
    }
    else
    {
        node->text[0] = '\0';
    }
    return true;
}

/* The interface font each platform actually uses, in the order worth
   trying. Loading the system font rather than embedding one keeps a
   megabyte of glyphs out of the library and gets each platform's own look;
   the built-in face is the fallback, not the plan. */
static void *LoadPlatformFont(size_t *length)
{
    static const char *kCandidates[] = {
#if defined(SDL_PLATFORM_MACOS)
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
#elif defined(SDL_PLATFORM_WIN32)
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
#elif defined(SDL_PLATFORM_ANDROID)
        "/system/fonts/Roboto-Regular.ttf",
        "/system/fonts/DroidSans.ttf",
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf",
#endif
    };
    for (size_t i = 0; i < SDL_arraysize(kCandidates); ++i)
    {
        void *bytes = SDL_LoadFile(kCandidates[i], length);
        if (bytes != NULL)
        {
            return bytes;
        }
    }
    *length = 0;
    return NULL;
}

Grapple_Ui *Grapple_OpenUi(SDL_Renderer *renderer, float font_points)
{
    size_t font_length = 0;
    void *font = LoadPlatformFont(&font_length);
    Grapple_Gui *gui = Grapple_CreateGui(renderer, font, font_length,
                                             (font_points > 0.0f) ? font_points : 15.0f);
    SDL_free(font); /* CreateGui copies what it needs */
    if (gui == NULL)
    {
        return NULL;
    }
    Grapple_Ui *ui = Grapple_CreateUi(gui);
    if (ui == NULL)
    {
        Grapple_DestroyGui(gui);
        return NULL;
    }
    ui->owns_gui = true;
    return ui;
}

Grapple_Gui *Grapple_UiGui(Grapple_Ui *ui)
{
    return (ui != NULL) ? ui->gui : NULL;
}

Grapple_EventSink Grapple_UiEventSink(Grapple_Ui *ui)
{
    if (ui == NULL)
    {
        const Grapple_EventSink none = {0};
        return none;
    }
    return Grapple_GuiEventSink(ui->gui);
}

Grapple_Ui *Grapple_CreateUi(Grapple_Gui *gui)
{
    if (gui == NULL)
    {
        SDL_InvalidParamError("gui");
        return NULL;
    }
    Grapple_Ui *ui = (Grapple_Ui *)SDL_calloc(1, sizeof(*ui));
    if (ui == NULL)
    {
        return NULL;
    }
    ui->gui = gui;
    return ui;
}

static void DestroyNode(Grapple_UiWidget *node)
{
    Grapple_UiWidget *child = node->first_child;
    while (child != NULL)
    {
        Grapple_UiWidget *next = child->next_sibling;
        DestroyNode(child);
        child = next;
    }
    SDL_free(node->value_text);
    if (node->owns_texture && node->texture != NULL)
    {
        SDL_DestroyTexture(node->texture);
    }
    for (int i = 0; i < node->option_count; ++i)
    {
        SDL_free(node->options[i]);
    }
    SDL_free(node->options);
    SDL_free(node->text);
    SDL_free(node->entry_previous_text);
    SDL_free(node);
}

void Grapple_DestroyUi(Grapple_Ui *ui)
{
    if (ui == NULL)
    {
        return;
    }
    Grapple_UiWidget *panel = ui->first_panel;
    while (panel != NULL)
    {
        Grapple_UiWidget *next = panel->next_sibling;
        DestroyNode(panel);
        panel = next;
    }
    if (ui->owns_gui)
    {
        Grapple_DestroyGui(ui->gui);
    }
    SDL_free(ui);
}

Grapple_UiWidget *Grapple_UiPanel(Grapple_Ui *ui, const Grapple_UiPanelDef *def)
{
    if (ui == NULL || def == NULL)
    {
        SDL_InvalidParamError("ui/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(ui, NULL, NODE_PANEL);
    if (node == NULL)
    {
        return NULL;
    }
    /* The title doubles as Nuklear's window name, which must be unique and
       stable: two untitled panels would otherwise be the same window. */
    if (def->title != NULL && def->title[0] != '\0')
    {
        SDL_strlcpy(node->title, def->title, sizeof(node->title));
    }
    else
    {
        SDL_snprintf(node->title, sizeof(node->title), "##panel%u", ui->next_id);
    }
    node->fill = def->fill;
    node->x = def->x;
    node->y = def->y;
    node->w = def->width;
    node->h = def->height;
    node->padding = def->padding;
    node->spacing = (def->spacing > 0.0f) ? def->spacing : -1.0f;
    node->movable = def->movable;
    node->scrollable = def->scrollable;
    node->no_border = def->no_border;

    if (ui->last_panel == NULL)
    {
        ui->first_panel = node;
    }
    else
    {
        ui->last_panel->next_sibling = node;
    }
    ui->last_panel = node;
    return node;
}

static Grapple_UiWidget *NewStrip(Grapple_UiWidget *parent, const Grapple_UiStripDef *def,
                                  NodeKind kind)
{
    if (parent == NULL)
    {
        SDL_InvalidParamError("parent");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, kind);
    if (node == NULL)
    {
        return NULL;
    }
    if (def != NULL)
    {
        node->height = def->height;
        node->spacing = def->spacing;
        node->align = def->align;
    }
    return node;
}

Grapple_UiWidget *Grapple_UiRow(Grapple_UiWidget *parent, const Grapple_UiStripDef *def)
{
    return NewStrip(parent, def, NODE_ROW);
}

Grapple_UiWidget *Grapple_UiColumn(Grapple_UiWidget *parent, const Grapple_UiStripDef *def)
{
    return NewStrip(parent, def, NODE_COLUMN);
}

Grapple_UiWidget *Grapple_UiOverlay(Grapple_UiWidget *parent, const Grapple_UiOverlayDef *def)
{
    if (parent == NULL)
    {
        SDL_InvalidParamError("parent");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_OVERLAY);
    if (node != NULL && def != NULL)
    {
        node->height = def->height;
    }
    return node;
}

Grapple_UiWidget *Grapple_UiLabel(Grapple_UiWidget *parent, const Grapple_UiLabelDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_LABEL);
    if (node == NULL || !SetText(node, def->text, 0))
    {
        return NULL;
    }
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->wrap = def->wrap;
    return node;
}

Grapple_UiWidget *Grapple_UiButton(Grapple_UiWidget *parent, const Grapple_UiButtonDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_BUTTON);
    if (node == NULL || !SetText(node, def->text, 0))
    {
        return NULL;
    }
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->disabled = def->disabled;
    Grapple_UiSetValueText(node, def->value);
    node->on_click = def->on_click;
    node->user = def->user;
    return node;
}

Grapple_UiWidget *Grapple_UiCheck(Grapple_UiWidget *parent, const Grapple_UiCheckDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_CHECK);
    if (node == NULL || !SetText(node, def->text, 0))
    {
        return NULL;
    }
    node->checked = def->checked;
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_change = def->on_change;
    node->user = def->user;
    return node;
}

Grapple_UiWidget *Grapple_UiSlider(Grapple_UiWidget *parent, const Grapple_UiSliderDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_SLIDER);
    if (node == NULL)
    {
        return NULL;
    }
    node->min = def->min;
    node->max = (def->max > def->min) ? def->max : (def->min + 1.0f);
    node->step = (def->step > 0.0f) ? def->step : (node->max - node->min) / 100.0f;
    node->value = SDL_clamp(def->value, node->min, node->max);
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_change = def->on_change;
    node->user = def->user;
    return node;
}

Grapple_UiWidget *Grapple_UiEntry(Grapple_UiWidget *parent, const Grapple_UiEntryDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_ENTRY);
    if (node == NULL || !SetText(node, def->text, (def->capacity > 0) ? def->capacity : 256))
    {
        return NULL;
    }
    if (def->on_change != NULL)
    {
        node->entry_previous_text = (char *)SDL_malloc((size_t)node->capacity);
        if (node->entry_previous_text == NULL)
        {
            Grapple_UiRemove(node);
            return NULL;
        }
    }
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_change = def->on_change;
    node->user = def->user;
    return node;
}

/* Copy the caller's labels: a script's strings are garbage collected, and a
   widget that outlives the table it was built from must not point into it. */
static bool CopyOptions(Grapple_UiWidget *node, const char *const *options)
{
    int count = 0;
    while (options != NULL && options[count] != NULL)
    {
        count++;
    }
    if (count == 0)
    {
        return true;
    }
    node->options = (char **)SDL_calloc((size_t)count, sizeof(char *));
    if (node->options == NULL)
    {
        return false;
    }
    for (int i = 0; i < count; ++i)
    {
        node->options[i] = SDL_strdup(options[i]);
        if (node->options[i] == NULL)
        {
            node->option_count = i;
            return false;
        }
    }
    node->option_count = count;
    return true;
}

static Grapple_UiWidget *NewSelect(Grapple_UiWidget *parent, const Grapple_UiSelectDef *def,
                                   NodeKind kind)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, kind);
    if (node == NULL || !CopyOptions(node, def->options))
    {
        return NULL;
    }
    node->selected = (def->selected >= 0 && def->selected < node->option_count)
                         ? def->selected
                         : 0;
    SDL_snprintf(node->title, sizeof(node->title), "#choice_%u", parent->ui->next_id);
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_change = def->on_change;
    node->user = def->user;
    /* The chosen label is also the widget's text, so Grapple_UiText answers
       the question a caller usually has. */
    SetText(node, (node->option_count > 0) ? node->options[node->selected] : "", 0);
    return node;
}

Grapple_UiWidget *Grapple_UiSelect(Grapple_UiWidget *parent, const Grapple_UiSelectDef *def)
{
    return NewSelect(parent, def, NODE_SELECT);
}

Grapple_UiWidget *Grapple_UiRadio(Grapple_UiWidget *parent, const Grapple_UiSelectDef *def)
{
    return NewSelect(parent, def, NODE_RADIO);
}

Grapple_UiWidget *Grapple_UiList(Grapple_UiWidget *parent, const Grapple_UiSelectDef *def)
{
    return NewSelect(parent, def, NODE_LIST);
}

bool Grapple_UiSetPanelColors(Grapple_UiWidget *panel, SDL_Color background, SDL_Color text)
{
    if (panel == NULL || panel->kind != NODE_PANEL)
    {
        return SDL_InvalidParamError("panel");
    }
    panel->has_colors = true;
    panel->background_color = background;
    panel->text_color = text;
    return true;
}

Grapple_UiWidget *Grapple_UiProgress(Grapple_UiWidget *parent,
                                         const Grapple_UiProgressDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_PROGRESS);
    if (node == NULL)
    {
        return NULL;
    }
    node->max = (def->max > 0.0f) ? def->max : 1.0f;
    node->value = SDL_clamp(def->value, 0.0f, node->max);
    node->editable = def->editable;
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_change = def->on_change;
    node->user = def->user;
    return node;
}

int Grapple_UiSelected(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->selected : -1;
}

void Grapple_UiSetSelected(Grapple_UiWidget *widget, int index)
{
    if (widget == NULL || index < 0 || index >= widget->option_count)
    {
        return;
    }
    widget->selected = index;
    SetText(widget, widget->options[index], 0);
}

int Grapple_UiOptionCount(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->option_count : 0;
}

const char *Grapple_UiOption(Grapple_UiWidget *widget, int index)
{
    if (widget == NULL || index < 0 || index >= widget->option_count)
    {
        return "";
    }
    return widget->options[index];
}

/* Whoever can decode more than a bitmap, if anybody has said so. */
static Grapple_UiImageLoader g_image_loader = NULL;
static void *g_image_loader_user = NULL;

void Grapple_UiSetImageLoader(Grapple_UiImageLoader loader, void *user)
{
    g_image_loader = loader;
    g_image_loader_user = user;
}

static bool IsImageWidget(Grapple_UiWidget *widget)
{
    if (widget == NULL || widget->kind != NODE_IMAGE)
    {
        return SDL_InvalidParamError("image widget");
    }
    return true;
}

static void ReplaceImageTexture(Grapple_UiWidget *widget, SDL_Texture *texture,
                                bool owned)
{
    if (widget->texture == texture)
    {
        widget->owns_texture = owned;
        return;
    }
    if (widget->owns_texture && widget->texture != NULL)
    {
        SDL_DestroyTexture(widget->texture);
    }
    widget->texture = texture;
    widget->owns_texture = owned;
}

bool Grapple_UiSetImagePath(Grapple_UiWidget *widget, const char *path)
{
    if (!IsImageWidget(widget) || path == NULL || path[0] == '\0')
    {
        if (path == NULL || path[0] == '\0')
        {
            SDL_InvalidParamError("path");
        }
        return false;
    }

    SDL_Renderer *renderer = Grapple_GuiRenderer(widget->ui->gui);
    SDL_Texture *texture = NULL;
    if (g_image_loader != NULL)
    {
        texture = g_image_loader(renderer, path, g_image_loader_user);
    }
    else
    {
        texture = IMG_LoadTexture(renderer, path);
    }
    if (texture == NULL)
    {
        return false;
    }

    ReplaceImageTexture(widget, texture, true);
    return true;
}

bool Grapple_UiSetImageTexture(Grapple_UiWidget *widget, SDL_Texture *texture)
{
    if (!IsImageWidget(widget))
    {
        return false;
    }
    ReplaceImageTexture(widget, texture, false);
    return true;
}

Grapple_UiWidget *Grapple_UiImage(Grapple_UiWidget *parent, const Grapple_UiImageDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_IMAGE);
    if (node == NULL)
    {
        return NULL;
    }
    if (def->texture != NULL)
    {
        Grapple_UiSetImageTexture(node, def->texture);
    }
    else if (def->path != NULL)
    {
        if (!Grapple_UiSetImagePath(node, def->path))
        {
            Grapple_UiRemove(node);
            return NULL;
        }
    }
    Grapple_UiSetValueText(node, def->value);
    node->image_mode = def->mode;
    node->width = def->width;
    node->height = def->height;
    node->align = def->align;
    node->on_click = def->on_click;
    node->user = def->user;
    return node;
}

Grapple_UiWidget *Grapple_UiImageAnnotation(Grapple_UiWidget *image,
                                            const Grapple_UiImageAnnotationDef *def)
{
    if (image == NULL || def == NULL || image->kind != NODE_IMAGE || image->parent == NULL ||
        image->parent->kind != NODE_OVERLAY || !(def->x >= 0.0f && def->x <= 1.0f) ||
        !(def->y >= 0.0f && def->y <= 1.0f) || !(def->gap >= 0.0f))
    {
        SDL_InvalidParamError("image/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(image->ui, image, NODE_IMAGE_ANNOTATION);
    if (node == NULL || !SetText(node, def->text, 0))
    {
        Grapple_UiRemove(node);
        return NULL;
    }
    node->width = GRAPPLE_UI_FIT;
    node->height = GRAPPLE_UI_FIT;
    node->annotation_x = def->x;
    node->annotation_y = def->y;
    node->annotation_gap = def->gap;
    node->annotation_side = def->side;
    return node;
}

void Grapple_UiMessage(Grapple_Ui *ui, const char *title, const char *text)
{
    SDL_Window *window = NULL;
    if (ui != NULL && ui->gui != NULL)
    {
        SDL_Renderer *renderer = Grapple_GuiRenderer(ui->gui);
        if (renderer != NULL)
        {
            window = SDL_GetRenderWindow(renderer);
        }
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, (title != NULL) ? title : "",
                             (text != NULL) ? text : "", window);
}

Grapple_UiWidget *Grapple_UiSpacer(Grapple_UiWidget *parent, const Grapple_UiSpacerDef *def)
{
    if (parent == NULL)
    {
        SDL_InvalidParamError("parent");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_SPACER);
    if (node != NULL && def != NULL)
    {
        node->width = def->width;
        node->height = def->height;
    }
    return node;
}

Grapple_UiWidget *Grapple_UiRaw(Grapple_UiWidget *parent, const Grapple_UiRawDef *def)
{
    if (parent == NULL || def == NULL)
    {
        SDL_InvalidParamError("parent/def");
        return NULL;
    }
    Grapple_UiWidget *node = NewNode(parent->ui, parent, NODE_RAW);
    if (node == NULL)
    {
        return NULL;
    }
    node->draw = def->draw;
    node->user = def->user;
    node->width = def->width;
    node->height = def->height;
    return node;
}

bool Grapple_UiPlace(Grapple_UiWidget *widget, Grapple_UiLength x, Grapple_UiLength y)
{
    if (widget == NULL || widget->parent == NULL || widget->parent->kind != NODE_OVERLAY)
    {
        return SDL_InvalidParamError("overlay child");
    }
    widget->place_x = x;
    widget->place_y = y;
    return true;
}

/* --- reading and changing ------------------------------------------------ */

void Grapple_UiSetText(Grapple_UiWidget *widget, const char *text)
{
    if (widget != NULL)
    {
        SetText(widget, text, widget->capacity);
    }
}

const char *Grapple_UiText(Grapple_UiWidget *widget)
{
    return (widget != NULL && widget->text != NULL) ? widget->text : "";
}

void Grapple_UiSetChecked(Grapple_UiWidget *widget, bool checked)
{
    if (widget != NULL)
    {
        widget->checked = checked;
    }
}

bool Grapple_UiChecked(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->checked : false;
}

void Grapple_UiSetValue(Grapple_UiWidget *widget, float value)
{
    if (widget != NULL)
    {
        widget->value = SDL_clamp(value, widget->min, widget->max);
    }
}

float Grapple_UiValue(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->value : 0.0f;
}

void Grapple_UiSetVisible(Grapple_UiWidget *widget, bool visible)
{
    if (widget != NULL)
    {
        widget->visible = visible;
    }
}

bool Grapple_UiVisible(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->visible : false;
}

void Grapple_UiSetDisabled(Grapple_UiWidget *widget, bool disabled)
{
    if (widget != NULL)
    {
        widget->disabled = disabled;
    }
}

bool Grapple_UiDisabled(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->disabled : false;
}

bool Grapple_UiBounds(Grapple_UiWidget *widget, float *x, float *y, float *width,
                        float *height)
{
    if (widget == NULL || !widget->drawn)
    {
        return false;
    }
    if (x != NULL)
    {
        *x = widget->last_bounds.x;
    }
    if (y != NULL)
    {
        *y = widget->last_bounds.y;
    }
    if (width != NULL)
    {
        *width = widget->last_bounds.w;
    }
    if (height != NULL)
    {
        *height = widget->last_bounds.h;
    }
    return true;
}

void Grapple_UiSetValueText(Grapple_UiWidget *widget, const char *value)
{
    if (widget == NULL)
    {
        return;
    }
    SDL_free(widget->value_text);
    widget->value_text = (value != NULL) ? SDL_strdup(value) : NULL;
}

const char *Grapple_UiValueText(Grapple_UiWidget *widget)
{
    if (widget == NULL || widget->value_text == NULL)
    {
        return "";
    }
    return widget->value_text;
}

void *Grapple_UiUser(Grapple_UiWidget *widget)
{
    return (widget != NULL) ? widget->user : NULL;
}

static void Unlink(Grapple_UiWidget *widget)
{
    Grapple_UiWidget **link = NULL;
    if (widget->parent != NULL)
    {
        link = &widget->parent->first_child;
    }
    else if (widget->ui != NULL)
    {
        link = &widget->ui->first_panel;
    }
    if (link == NULL)
    {
        return;
    }
    Grapple_UiWidget *prev = NULL;
    for (Grapple_UiWidget *c = *link; c != NULL; prev = c, c = c->next_sibling)
    {
        if (c != widget)
        {
            continue;
        }
        if (prev == NULL)
        {
            *link = c->next_sibling;
        }
        else
        {
            prev->next_sibling = c->next_sibling;
        }
        if (widget->parent != NULL && widget->parent->last_child == widget)
        {
            widget->parent->last_child = prev;
        }
        if (widget->parent == NULL && widget->ui != NULL && widget->ui->last_panel == widget)
        {
            widget->ui->last_panel = prev;
        }
        return;
    }
}

void Grapple_UiInvoke(Grapple_UiWidget *widget)
{
    if (widget == NULL)
    {
        return;
    }
    if (widget->on_click != NULL)
    {
        widget->on_click(widget, widget->user);
    }
    else if (widget->on_change != NULL)
    {
        widget->on_change(widget, widget->user);
    }
}

void Grapple_UiRemove(Grapple_UiWidget *widget)
{
    if (widget == NULL)
    {
        return;
    }
    Unlink(widget);
    DestroyNode(widget);
}

void Grapple_UiClear(Grapple_UiWidget *parent)
{
    if (parent == NULL)
    {
        return;
    }
    Grapple_UiWidget *child = parent->first_child;
    while (child != NULL)
    {
        Grapple_UiWidget *next = child->next_sibling;
        DestroyNode(child);
        child = next;
    }
    parent->first_child = NULL;
    parent->last_child = NULL;
}

/* --- drawing ------------------------------------------------------------- */

static void DrawNode(Grapple_UiWidget *node, float available_width);

/* One widget, once its slot has already been pushed by its parent. */
static void DrawLeaf(Grapple_UiWidget *node)
{
    struct nk_context *ctx = Grapple_GuiContext(node->ui->gui);
    node->last_bounds = nk_widget_bounds(ctx);
    node->drawn = true;
    const nk_flags text_align = (node->align == GRAPPLE_UI_CENTER)   ? NK_TEXT_CENTERED
                                : (node->align == GRAPPLE_UI_RIGHT) ? NK_TEXT_RIGHT
                                                                     : NK_TEXT_LEFT;
    switch (node->kind)
    {
    case NODE_LABEL:
    case NODE_IMAGE_ANNOTATION:
        if (node->wrap)
        {
            nk_label_wrap(ctx, node->text);
        }
        else
        {
            nk_label(ctx, node->text, text_align);
        }
        break;

    case NODE_BUTTON:
    {
        /* Nuklear has no disabled button, so it is drawn as one that cannot
           be pressed rather than one that merely looks pressable. */
        if (node->disabled)
        {
            struct nk_style_button dimmed = ctx->style.button;
            dimmed.normal = dimmed.hover = dimmed.active = ctx->style.button.normal;
            dimmed.text_normal = dimmed.text_hover = dimmed.text_active =
                nk_rgba(160, 160, 160, 160);
            nk_button_label_styled(ctx, &dimmed, node->text);
        }
        else if (nk_button_label(ctx, node->text) && node->on_click != NULL)
        {
            node->on_click(node, node->user);
        }
        break;
    }

    case NODE_CHECK:
    {
        nk_bool checked = node->checked ? 1 : 0;
        if (nk_checkbox_label(ctx, node->text, &checked))
        {
            node->checked = (checked != 0);
            if (node->on_change != NULL)
            {
                node->on_change(node, node->user);
            }
        }
        break;
    }

    case NODE_SLIDER:
    {
        float value = node->value;
        if (nk_slider_float(ctx, node->min, &value, node->max, node->step))
        {
            node->value = value;
            if (node->on_change != NULL)
            {
                node->on_change(node, node->user);
            }
        }
        break;
    }

    case NODE_ENTRY:
    {
        if (node->on_change != NULL)
        {
            SDL_strlcpy(node->entry_previous_text, node->text, (size_t)node->capacity);
        }
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, node->text, node->capacity,
                                       nk_filter_default);
        /* Nuklear reports focus and commit events, not changes to the buffer. */
        if (node->on_change != NULL && SDL_strcmp(node->entry_previous_text, node->text) != 0)
        {
            node->on_change(node, node->user);
        }
        break;
    }

    case NODE_SELECT:
    {
        if (node->option_count == 0)
        {
            nk_spacing(ctx, 1);
            break;
        }
        /* The popup is sized from the widget, so a dropdown does not open
           into a box narrower than the row that spawned it. */
        const struct nk_rect bounds = node->last_bounds;
        const float row = LineHeight(node->ui) * 1.9f;
        const float tall = row * (float)node->option_count + 8.0f;
        const struct nk_vec2 size =
            nk_vec2((bounds.w > 0.0f) ? bounds.w : 200.0f, SDL_min(tall, row * 8.0f));
        if (nk_combo_begin_label(ctx, node->options[node->selected], size))
        {
            nk_layout_row_dynamic(ctx, row, 1);
            for (int i = 0; i < node->option_count; ++i)
            {
                if (nk_combo_item_label(ctx, node->options[i], NK_TEXT_LEFT))
                {
                    if (i != node->selected)
                    {
                        Grapple_UiSetSelected(node, i);
                        if (node->on_change != NULL)
                        {
                            node->on_change(node, node->user);
                        }
                    }
                }
            }
            nk_combo_end(ctx);
        }
        break;
    }

    case NODE_RADIO:
    case NODE_LIST:
        /* A choice group must consume exactly one slot in its parent's layout. */
        if (nk_group_begin(ctx, node->title, node->kind == NODE_LIST ? NK_WINDOW_BORDER : 0))
        {
            nk_layout_row_dynamic(ctx, LineHeight(node->ui) * 1.9f, 1);
            for (int i = 0; i < node->option_count; ++i)
            {
                const bool chosen =
                    node->kind == NODE_RADIO
                        ? nk_option_label(ctx, node->options[i], node->selected == i)
                        : nk_select_label(ctx, node->options[i], NK_TEXT_LEFT, node->selected == i);
                if (chosen && node->selected != i)
                {
                    Grapple_UiSetSelected(node, i);
                    if (node->on_change != NULL)
                    {
                        node->on_change(node, node->user);
                    }
                }
            }
            nk_group_end(ctx);
        }
        break;

    case NODE_PROGRESS:
    {
        /* Nuklear counts in whole units; the widget's own scale is a float,
           so the value is carried at a fixed resolution across the call. */
        const nk_size steps = 1000;
        nk_size current = (nk_size)((node->value / node->max) * (float)steps);
        if (nk_progress(ctx, &current, steps, node->editable ? nk_true : nk_false))
        {
            node->value = ((float)current / (float)steps) * node->max;
            if (node->on_change != NULL)
            {
                node->on_change(node, node->user);
            }
        }
        break;
    }

    case NODE_IMAGE:
        if (node->texture == NULL)
        {
            nk_spacing(ctx, 1);
        }
        else if (node->on_click != NULL)
        {
            if (Grapple_GuiImageButton(node->ui->gui, node->texture, node->image_mode))
            {
                node->on_click(node, node->user);
            }
        }
        else
        {
            Grapple_GuiImage(node->ui->gui, node->texture, node->image_mode);
        }
        break;

    case NODE_SPACER:
        nk_spacing(ctx, 1);
        break;

    case NODE_RAW:
        if (node->draw != NULL)
        {
            node->draw(ctx, node->user);
        }
        break;

    default:
        break;
    }
}

/* A row: fixed-width children keep their width, the rest share what is left.
   This is nk_layout_row_template's whole purpose, so the mapping is direct. */
static void DrawRow(Grapple_UiWidget *row, float available_width)
{
    struct nk_context *ctx = Grapple_GuiContext(row->ui->gui);
    const int count = VisibleChildren(row);
    if (count == 0)
    {
        return;
    }

    float tallest = 0.0f;
    for (Grapple_UiWidget *c = row->first_child; c != NULL; c = c->next_sibling)
    {
        if (!c->visible)
        {
            continue;
        }
        const float h = IsContainer(c) ? LineHeight(row->ui) * 1.3f : RowHeightOf(c, 0.0f);
        tallest = SDL_max(tallest, h);
    }
    const float height = Resolve(row, row->height, 0.0f, tallest);

    nk_layout_row_template_begin(ctx, (height > 0.0f) ? height : tallest);
    for (Grapple_UiWidget *c = row->first_child; c != NULL; c = c->next_sibling)
    {
        if (!c->visible)
        {
            continue;
        }
        const float fixed = Resolve(c, c->width, available_width, ContentWidth(c));
        if (fixed > 0.0f)
        {
            nk_layout_row_template_push_static(ctx, fixed);
        }
        else
        {
            nk_layout_row_template_push_dynamic(ctx);
        }
    }
    nk_layout_row_template_end(ctx);

    for (Grapple_UiWidget *c = row->first_child; c != NULL; c = c->next_sibling)
    {
        if (c->visible)
        {
            DrawLeaf(c);
        }
    }
}

/* A column: one row per child, with the child's own height. A child that is
   narrower than the column is placed by its alignment, which needs the
   blank space either side to be real widgets — hence the spacers. */
static void DrawColumn(Grapple_UiWidget *column, float available_width)
{
    struct nk_context *ctx = Grapple_GuiContext(column->ui->gui);

    for (Grapple_UiWidget *c = column->first_child; c != NULL; c = c->next_sibling)
    {
        if (!c->visible)
        {
            continue;
        }
        if (c->kind == NODE_ROW || c->kind == NODE_COLUMN || c->kind == NODE_OVERLAY)
        {
            DrawNode(c, available_width);
            continue;
        }

        const float height = RowHeightOf(c, 0.0f);
        const float wanted = Resolve(c, c->width, available_width, ContentWidth(c));

        if (wanted <= 0.0f)
        {
            nk_layout_row_dynamic(ctx, height, 1);
            DrawLeaf(c);
            continue;
        }

        nk_layout_row_template_begin(ctx, height);
        if (c->align == GRAPPLE_UI_CENTER || c->align == GRAPPLE_UI_RIGHT)
        {
            nk_layout_row_template_push_dynamic(ctx);
        }
        nk_layout_row_template_push_static(ctx, wanted);
        if (c->align == GRAPPLE_UI_CENTER || c->align == GRAPPLE_UI_LEFT)
        {
            nk_layout_row_template_push_dynamic(ctx);
        }
        nk_layout_row_template_end(ctx);

        if (c->align == GRAPPLE_UI_CENTER || c->align == GRAPPLE_UI_RIGHT)
        {
            nk_spacing(ctx, 1);
        }
        DrawLeaf(c);
        if (c->align == GRAPPLE_UI_CENTER || c->align == GRAPPLE_UI_LEFT)
        {
            nk_spacing(ctx, 1);
        }
    }
}

/* An overlay is one Nuklear layout space. Direct children receive explicit
   rectangles, in creation order, so labels and controls can sit over an
   image without leaving the retained tree. */
static int OverlayWidgetCount(Grapple_UiWidget *overlay)
{
    int count = 0;
    for (Grapple_UiWidget *child = overlay->first_child; child != NULL; child = child->next_sibling)
    {
        if (!child->visible)
        {
            continue;
        }
        count++;
        if (child->kind == NODE_IMAGE)
        {
            count += VisibleChildren(child);
        }
    }
    return count;
}

static struct nk_rect ImageContentBounds(Grapple_UiWidget *image)
{
    struct nk_rect content;
    if (image->texture == NULL || image->on_click != NULL ||
        !Grapple_GuiFitTexture(image->texture, image->last_bounds, image->image_mode, &content))
    {
        return image->last_bounds;
    }
    return content;
}

static void DrawImageAnnotations(Grapple_UiWidget *image, Grapple_UiWidget *overlay)
{
    const struct nk_rect content = ImageContentBounds(image);
    struct nk_context *ctx = Grapple_GuiContext(overlay->ui->gui);
    const float image_x = Resolve(image, image->place_x, overlay->last_bounds.w, 0.0f);
    const float image_y = Resolve(image, image->place_y, overlay->last_bounds.h, 0.0f);
    const float space_x = image->last_bounds.x - image_x;
    const float space_y = image->last_bounds.y - image_y;
    for (Grapple_UiWidget *annotation = image->first_child; annotation != NULL;
         annotation = annotation->next_sibling)
    {
        if (!annotation->visible)
        {
            continue;
        }

        const float width = ContentWidth(annotation);
        const float height = RowHeightOf(annotation, 0.0f);
        const float point_x = content.x + content.w * annotation->annotation_x;
        const float point_y = content.y + content.h * annotation->annotation_y;
        float x = point_x + annotation->annotation_gap;
        float y = point_y - height * 0.5f;
        if (annotation->annotation_side == GRAPPLE_UI_ANNOTATION_LEFT)
        {
            x = point_x - annotation->annotation_gap - width;
        }
        else if (annotation->annotation_side == GRAPPLE_UI_ANNOTATION_ABOVE)
        {
            x = point_x - width * 0.5f;
            y = point_y - annotation->annotation_gap - height;
        }
        else if (annotation->annotation_side == GRAPPLE_UI_ANNOTATION_BELOW)
        {
            x = point_x - width * 0.5f;
            y = point_y + annotation->annotation_gap;
        }

        nk_layout_space_push(ctx, nk_rect(x - space_x, y - space_y, width, height));
        DrawLeaf(annotation);
    }
}

static void DrawOverlay(Grapple_UiWidget *overlay, float available_width)
{
    struct nk_context *ctx = Grapple_GuiContext(overlay->ui->gui);
    const float height = RowHeightOf(overlay, 0.0f);
    nk_layout_space_begin(ctx, NK_STATIC, height, OverlayWidgetCount(overlay));
    overlay->last_bounds = nk_layout_space_bounds(ctx);
    overlay->drawn = true;
    const float space_width =
        overlay->last_bounds.w > 0.0f ? overlay->last_bounds.w : available_width;
    const float space_height = overlay->last_bounds.h;

    for (Grapple_UiWidget *c = overlay->first_child; c != NULL; c = c->next_sibling)
    {
        if (!c->visible)
        {
            continue;
        }

        const float x = Resolve(c, c->place_x, space_width, 0.0f);
        const float y = Resolve(c, c->place_y, space_height, 0.0f);
        float width = Resolve(c, c->width, space_width, ContentWidth(c));
        float child_height = Resolve(c, c->height, space_height, LineHeight(c->ui));
        if (width <= 0.0f)
        {
            width = SDL_max(0.0f, space_width - x);
        }
        if (child_height <= 0.0f)
        {
            child_height = SDL_max(0.0f, space_height - y);
        }

        nk_layout_space_push(ctx, nk_rect(x, y, width, child_height));
        DrawLeaf(c);
        if (c->kind == NODE_IMAGE)
        {
            DrawImageAnnotations(c, overlay);
        }
    }
    nk_layout_space_end(ctx);
}

static void DrawNode(Grapple_UiWidget *node, float available_width)
{
    if (node->kind == NODE_ROW)
    {
        DrawRow(node, available_width);
    }
    else if (node->kind == NODE_OVERLAY)
    {
        DrawOverlay(node, available_width);
    }
    else
    {
        DrawColumn(node, available_width);
    }
}

/* Called once a frame by whoever owns the engine, whether or not this UI
   was drawn, so that a UI which is never drawn can notice and say so. */
void Grapple_UiNoteFrame(Grapple_Ui *ui)
{
    if (ui == NULL || ui->ever_drawn || ui->warned_not_drawn)
    {
        return;
    }
    ui->frames_since_created++;
    /* A second at 60Hz: long enough that a UI built during a loading screen
       is not accused, short enough to be seen. */
    if (ui->frames_since_created > 60 && ui->first_panel != NULL)
    {
        ui->warned_not_drawn = true;
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "a ui has widgets but has never been drawn — call ui:draw() "
                    "from a render callback");
    }
}

void Grapple_UiDraw(Grapple_Ui *ui)
{
    if (ui == NULL || ui->gui == NULL)
    {
        return;
    }
    ui->ever_drawn = true;
    struct nk_context *ctx = Grapple_GuiContext(ui->gui);
    if (ctx == NULL)
    {
        return;
    }

    for (Grapple_UiWidget *panel = ui->first_panel; panel != NULL;
         panel = panel->next_sibling)
    {
        if (!panel->visible)
        {
            continue;
        }

        struct nk_rect bounds = nk_rect(panel->x, panel->y, panel->w, panel->h);
        if (panel->fill)
        {
            /* Re-read every frame: this is what makes a resize reflow the
               layout instead of scaling it. */
            int pixel_w = 0;
            int pixel_h = 0;
            SDL_Renderer *renderer = Grapple_GuiRenderer(ui->gui);
            if (renderer != NULL && SDL_GetRenderOutputSize(renderer, &pixel_w, &pixel_h))
            {
                bounds = nk_rect(0, 0, (float)pixel_w, (float)pixel_h);
            }
        }

        nk_flags flags = 0;
        if (panel->title[0] != '\0' && panel->title[0] != '#')
        {
            flags |= NK_WINDOW_TITLE;
        }
        if (!panel->no_border)
        {
            flags |= NK_WINDOW_BORDER;
        }
        if (panel->movable)
        {
            flags |= NK_WINDOW_MOVABLE;
        }
        if (!panel->scrollable)
        {
            flags |= NK_WINDOW_NO_SCROLLBAR;
        }

        const bool padded = panel->padding > 0.0f;
        if (padded)
        {
            nk_style_push_vec2(ctx, &ctx->style.window.padding,
                               nk_vec2(panel->padding, panel->padding));
        }
        const bool spaced = panel->spacing > 0.0f;
        if (spaced)
        {
            nk_style_push_vec2(ctx, &ctx->style.window.spacing,
                               nk_vec2(panel->spacing, panel->spacing));
        }

        const struct nk_style_item background = ctx->style.window.fixed_background;
        const struct nk_color text_color = ctx->style.text.color;
        const struct nk_style_toggle option = ctx->style.option;
        const struct nk_style_toggle checkbox = ctx->style.checkbox;
        if (panel->has_colors)
        {
            const SDL_Color bg = panel->background_color;
            const SDL_Color fg = panel->text_color;
            const struct nk_color text = nk_rgba(fg.r, fg.g, fg.b, fg.a);
            ctx->style.window.fixed_background =
                nk_style_item_color(nk_rgba(bg.r, bg.g, bg.b, bg.a));
            ctx->style.text.color = text;
            ctx->style.option.text_normal = ctx->style.option.text_hover =
                ctx->style.option.text_active = text;
            ctx->style.checkbox.text_normal = ctx->style.checkbox.text_hover =
                ctx->style.checkbox.text_active = text;
        }
        if (nk_begin(ctx, panel->title, bounds, flags))
        {
            const struct nk_vec2 room = nk_window_get_content_region_size(ctx);
            DrawColumn(panel, room.x);
        }
        nk_end(ctx);
        ctx->style.window.fixed_background = background;
        ctx->style.text.color = text_color;
        ctx->style.option = option;
        ctx->style.checkbox = checkbox;

        /* Pop in the reverse order of the pushes: Nuklear's style stack is a
           stack, and unbalancing it corrupts every window after this one. */
        if (spaced)
        {
            nk_style_pop_vec2(ctx);
        }
        if (padded)
        {
            nk_style_pop_vec2(ctx);
        }
    }

    Grapple_GuiRender(ui->gui);
}

void Grapple_UiNoteFrameCallback(void *ui)
{
    Grapple_UiNoteFrame((Grapple_Ui *)ui);
}

void Grapple_UiDrawCallback(void *ui)
{
    Grapple_UiDraw((Grapple_Ui *)ui);
}

bool Grapple_UiWantsInput(Grapple_Ui *ui)
{
    return (ui != NULL) ? Grapple_GuiWantsInput(ui->gui) : false;
}
