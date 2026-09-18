/*
 * grapple_gui_grid.c — weighted grid layout helper for Nuklear.
 *
 * Original Grapple code (zlib). Semantics in grapple/gui_grid.h: cells
 * advance left-to-right, wrap automatically, and every width is a weight
 * ratio of the window — no hard-coded coordinates anywhere.
 */
#include <grapple/gui_grid.h>

#include <SDL3/SDL.h>

bool Grapple_GuiGridBegin(struct nk_context *ctx, Grapple_GuiGrid *grid, int columns,
                            const float *weights, float row_height)
{
    if (ctx == NULL || grid == NULL || columns < 1 || columns > GRAPPLE_GUI_GRID_MAX_COLS)
    {
        return SDL_InvalidParamError("ctx/grid/columns");
    }
    grid->ctx = ctx;
    grid->columns = columns;
    grid->row_height = (row_height > 0.0f) ? row_height : 0.0f; /* 0 = font-based auto */
    grid->cursor = 0;
    grid->row_open = false;
    grid->total_weight = 0.0f;
    grid->next_row_height = 0.0f;
    grid->pending_tail = 0.0f;
    grid->styled = false;
    for (int i = 0; i < columns; ++i)
    {
        const float w = (weights != NULL && weights[i] > 0.0f) ? weights[i] : 1.0f;
        grid->weights[i] = w;
        grid->total_weight += w;
    }
    return true;
}

/* An aligned cell leaves blank space owed on its right, which cannot be
   emitted at the time: the caller's widget has to be drawn first. So it is
   settled by whatever grid call comes next. */
static void FlushPendingTail(Grapple_GuiGrid *grid)
{
    if (grid->pending_tail > 0.0f && grid->row_open)
    {
        nk_layout_row_push(grid->ctx, grid->pending_tail);
        nk_spacing(grid->ctx, 1);
    }
    grid->pending_tail = 0.0f;
}

static void CloseRow(Grapple_GuiGrid *grid)
{
    FlushPendingTail(grid);
    if (grid->row_open)
    {
        nk_layout_row_end(grid->ctx);
        grid->row_open = false;
    }
    grid->cursor = 0;
}

/* The share of the row a run of columns is worth, and the bookkeeping that
   goes with claiming it. Shared by the plain and the aligned cell. */
static float ClaimCells(Grapple_GuiGrid *grid, int *span)
{
    FlushPendingTail(grid);
    if (grid->cursor >= grid->columns)
    {
        CloseRow(grid);
    }
    if (!grid->row_open)
    {
        const float height =
            (grid->next_row_height > 0.0f) ? grid->next_row_height : grid->row_height;
        grid->next_row_height = 0.0f;
        /* Three slots per column: an aligned cell spends up to two extra on
           the blank space either side of its widget. Widths come from the
           pushes, so over-reserving costs nothing. */
        nk_layout_row_begin(grid->ctx, NK_DYNAMIC, height, grid->columns * 3);
        grid->row_open = true;
        grid->cursor = 0;
    }
    *span = SDL_clamp(*span, 1, grid->columns - grid->cursor);
    float weight = 0.0f;
    for (int i = 0; i < *span; ++i)
    {
        weight += grid->weights[grid->cursor + i];
    }
    grid->cursor += *span;
    return weight / grid->total_weight;
}

void Grapple_GuiGridCellSpan(Grapple_GuiGrid *grid, int span)
{
    if (grid == NULL || grid->ctx == NULL)
    {
        return;
    }
    nk_layout_row_push(grid->ctx, ClaimCells(grid, &span));
}

void Grapple_GuiGridCellPart(Grapple_GuiGrid *grid, int span, float fraction,
                               Grapple_GuiAlign align)
{
    if (grid == NULL || grid->ctx == NULL)
    {
        return;
    }
    fraction = SDL_clamp(fraction, 0.01f, 1.0f);
    const float share = ClaimCells(grid, &span);
    const float content = share * fraction;
    const float slack = share - content;

    switch (align)
    {
    case GRAPPLE_GUI_ALIGN_RIGHT:
        if (slack > 0.0f)
        {
            nk_layout_row_push(grid->ctx, slack);
            nk_spacing(grid->ctx, 1);
        }
        break;
    case GRAPPLE_GUI_ALIGN_CENTER:
        if (slack > 0.0f)
        {
            nk_layout_row_push(grid->ctx, slack * 0.5f);
            nk_spacing(grid->ctx, 1);
            grid->pending_tail = slack * 0.5f;
        }
        break;
    case GRAPPLE_GUI_ALIGN_LEFT:
    default:
        if (slack > 0.0f)
        {
            grid->pending_tail = slack;
        }
        break;
    }
    nk_layout_row_push(grid->ctx, content);
}

void Grapple_GuiGridRowHeight(Grapple_GuiGrid *grid, float height)
{
    if (grid != NULL)
    {
        grid->next_row_height = (height > 0.0f) ? height : 0.0f;
    }
}

void Grapple_GuiGridSpacing(Grapple_GuiGrid *grid, float x, float y)
{
    if (grid == NULL || grid->ctx == NULL || grid->styled)
    {
        return;
    }
    struct nk_vec2 spacing;
    spacing.x = (x >= 0.0f) ? x : 0.0f;
    spacing.y = (y >= 0.0f) ? y : 0.0f;
    nk_style_push_vec2(grid->ctx, &grid->ctx->style.window.spacing, spacing);
    grid->styled = true;
}

void Grapple_GuiGridCell(Grapple_GuiGrid *grid)
{
    Grapple_GuiGridCellSpan(grid, 1);
}

void Grapple_GuiGridNextRow(Grapple_GuiGrid *grid)
{
    if (grid != NULL && grid->ctx != NULL)
    {
        CloseRow(grid);
    }
}

void Grapple_GuiGridEnd(Grapple_GuiGrid *grid)
{
    if (grid != NULL && grid->ctx != NULL)
    {
        CloseRow(grid);
        if (grid->styled)
        {
            /* Pop what Grapple_GuiGridSpacing pushed: a style stack left
               unbalanced corrupts every window drawn after it. */
            nk_style_pop_vec2(grid->ctx);
            grid->styled = false;
        }
        grid->ctx = NULL;
    }
}

/* --- heap allocation, for callers without a stack ------------------------- */

/* C code declares a grid as a local; a script cannot. Without this every
   grid function was bound into Lua and Ruby and none of them was callable,
   because there was no way to produce the first argument. */
Grapple_GuiGrid *Grapple_GuiGridCreate(void)
{
    return (Grapple_GuiGrid *)SDL_calloc(1, sizeof(Grapple_GuiGrid));
}

void Grapple_GuiGridDestroy(Grapple_GuiGrid *grid)
{
    SDL_free(grid);
}
