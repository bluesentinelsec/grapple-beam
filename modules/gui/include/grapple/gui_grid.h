/**
 * @file gui_grid.h
 * @brief Weighted grid layout helper for Nuklear (Grapple GUI).
 *
 * Original Grapple code (zlib). Nuklear's native layout is row-at-a-time;
 * this helper adds a Tkinter-grid flavor on top so nothing hard-codes x/y
 * coordinates: declare column weights once, then place widgets cell by
 * cell. Cells advance left-to-right and wrap to a new row automatically;
 * spans merge adjacent columns. All widths are weight-proportional to the
 * window, so the layout scales with any display or window size.
 *
 *   static const float weights[] = {1.0f, 2.0f, 1.0f};   // like Tk column weight
 *   Grapple_GuiGrid grid;
 *   Grapple_GuiGridBegin(ctx, &grid, 3, weights, 0);   // 0 = auto row height
 *
 *   Grapple_GuiGridCell(&grid);          nk_label(ctx, "Name:", NK_TEXT_LEFT);
 *   Grapple_GuiGridCellSpan(&grid, 2);   nk_edit_string_zero_terminated(...);
 *
 *   Grapple_GuiGridCell(&grid);          nk_label(ctx, "HP:", NK_TEXT_LEFT);
 *   Grapple_GuiGridCellSpan(&grid, 2);   nk_slider_int(ctx, 0, &hp, 100, 1);
 *
 *   Grapple_GuiGridRowHeight(&grid, 44);            // this row only
 *   Grapple_GuiGridCellPart(&grid, 3, 0.3f,         // a third of the row,
 *                           GRAPPLE_GUI_ALIGN_RIGHT);  // hugging the right
 *   nk_button_label(ctx, "Save");
 *
 *   Grapple_GuiGridEnd(&grid);
 */
#ifndef GRAPPLE_GUI_GRID_H
#define GRAPPLE_GUI_GRID_H

#include <grapple/nuklear.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GRAPPLE_GUI_GRID_MAX_COLS 16

/** Where a part-width widget sits inside the cell it was given. */
typedef enum Grapple_GuiAlign
{
    GRAPPLE_GUI_ALIGN_LEFT = 0,
    GRAPPLE_GUI_ALIGN_CENTER,
    GRAPPLE_GUI_ALIGN_RIGHT
} Grapple_GuiAlign;

typedef struct Grapple_GuiGrid
{
    struct nk_context *ctx;
    float weights[GRAPPLE_GUI_GRID_MAX_COLS];
    float total_weight;
    float row_height;
    float next_row_height; /* one-shot override; 0 = use row_height */
    float pending_tail;    /* trailing spacer owed by an aligned cell */
    int columns;
    int cursor;   /* next column index in the current row */
    bool row_open;
    bool styled;  /* spacing was pushed and must be popped at End */
} Grapple_GuiGrid;

/**
 * Start a grid inside the current Nuklear window.
 * \param columns     1..GRAPPLE_GUI_GRID_MAX_COLS
 * \param weights     per-column weights (like Tkinter's column weight);
 *                    NULL means equal columns.
 * \param row_height  pixel height per row; <= 0 derives one widget row from
 *                    the current font (so even heights need no hard-coding).
 * \returns false on bad parameters.
 */
/** A grid on the heap, for callers that cannot declare one on the stack.
 *
 *  C code should keep using a local — it is cheaper and scoped. This exists
 *  for scripts, which have no stack to put a struct on, and without it every
 *  grid function was bound and uncallable. */
extern Grapple_GuiGrid *Grapple_GuiGridCreate(void);
extern void Grapple_GuiGridDestroy(Grapple_GuiGrid *grid);

extern bool Grapple_GuiGridBegin(struct nk_context *ctx, Grapple_GuiGrid *grid, int columns,
                                   const float *weights, float row_height);

/** Claim the next cell (auto-wraps to a new row when the row is full). Call
 *  exactly one widget after each claimed cell. */
extern void Grapple_GuiGridCell(Grapple_GuiGrid *grid);

/** Like Cell, but spanning `span` columns (clamped to the row remainder). */
extern void Grapple_GuiGridCellSpan(Grapple_GuiGrid *grid, int span);

/** Force the next cell onto a fresh row even if this one isn't full. */
extern void Grapple_GuiGridNextRow(Grapple_GuiGrid *grid);

/** Finish the grid (closes the last row). */
extern void Grapple_GuiGridEnd(Grapple_GuiGrid *grid);

/**
 * Height for the next row only, overriding the grid's own.
 *
 * A grid otherwise gives every row the same height, which is wrong the
 * moment a panel has a heading, a row of buttons and a status line — three
 * grids where one should do. Call this before the first cell of the row;
 * `height` <= 0 restores the grid default for that row.
 */
extern void Grapple_GuiGridRowHeight(Grapple_GuiGrid *grid, float height);

/**
 * Space between cells, in pixels, for the rest of this grid.
 *
 * Nuklear's spacing is a window-wide style value; this pushes it for the
 * grid and Grapple_GuiGridEnd pops it, so a dense grid and an airy one can
 * sit in the same panel. Call before the first cell.
 */
extern void Grapple_GuiGridSpacing(Grapple_GuiGrid *grid, float x, float y);

/**
 * Claim `span` columns but give the widget only `fraction` of that width,
 * placed by `align`; the remainder becomes blank space.
 *
 * This is what a full-width cell cannot express: a confirm button that hugs
 * the right edge, a title centred over a wide row. `fraction` is clamped to
 * (0, 1]; 1 is exactly Grapple_GuiGridCellSpan.
 *
 * Nuklear cannot measure a widget, so the fraction is the caller's estimate
 * rather than a shrink-to-fit — the honest limit of a layout that has to
 * emit widths before it has seen the widget.
 */
extern void Grapple_GuiGridCellPart(Grapple_GuiGrid *grid, int span, float fraction,
                                      Grapple_GuiAlign align);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_GUI_GRID_H */
