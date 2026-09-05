#ifndef GRAPPLE_GUI_INTERNAL_H
#define GRAPPLE_GUI_INTERNAL_H

#include <grapple/gui.h>
#include <grapple/nuklear.h>

/* Shared fitting geometry for retained widgets and immediate-mode drawing. */
bool Grapple_GuiFitTexture(SDL_Texture *texture, struct nk_rect slot, Grapple_GuiImageMode mode,
                           struct nk_rect *out);

#endif
