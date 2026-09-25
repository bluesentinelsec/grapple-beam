/*
 * grapple_engine_target.c — the offscreen frame.
 * Original Grapple code (zlib).
 *
 * Render scale draws the frame at a fraction of the window's resolution and
 * lets the display scale it up, which needs somewhere to draw it — this
 * file is that somewhere.
 *
 * A game at render_scale 1.0 pays nothing: Begin and End do nothing at all
 * and the loop draws directly to the window exactly as it did before this
 * file existed. Post-processing is separate: it copies the finished frame
 * out of the framebuffer, so it works with or without a target here.
 *
 * The logical presentation is the subtle part. SDL applies it to whatever
 * the current render target is, so pointing the renderer at a texture of a
 * different size would silently change the scale the game draws at. The
 * target is therefore created at the *pixel* size the frame would have had,
 * scaled — and the logical presentation is re-applied against it, so design
 * coordinates keep meaning the same thing whatever the render scale is.
 */
#include "engine_internal.h"

/* Below this, the difference is not worth a texture and a blit. */
#define SCALE_EPSILON 0.001f

static bool IsPixel(const Grapple_Engine *engine)
{
    return engine->presentation == GRAPPLE_PRESENT_PIXEL ||
           engine->presentation == GRAPPLE_PRESENT_PIXEL_SNAP;
}

/* Screen pixels per design unit, as a whole number, for the window as it
   is now: the largest that fits. */
static int WholeScale(const Grapple_Engine *engine, int pixel_w, int pixel_h)
{
    const float scale = SDL_min((float)pixel_w / engine->view_width,
                                (float)pixel_h / engine->view_height);
    const int whole = (int)SDL_floorf(scale + SCALE_EPSILON);
    return (whole < 1) ? 1 : whole;
}

int Grapple_EngineFrameScale(Grapple_Engine *engine)
{
    if (engine == NULL || engine->renderer == NULL || !IsPixel(engine))
    {
        return 0;
    }
    if (engine->presentation == GRAPPLE_PRESENT_PIXEL_SNAP)
    {
        return 1;
    }
    int pixel_w = 0;
    int pixel_h = 0;
    if (!SDL_GetRenderOutputSize(engine->renderer, &pixel_w, &pixel_h) || pixel_w <= 0 ||
        pixel_h <= 0)
    {
        return 1;
    }
    return WholeScale(engine, pixel_w, pixel_h);
}

/* Render scale and PIXEL presentation need the offscreen frame.
   Post-processing reads the framebuffer directly, so it does not. */
static bool WantsTarget(const Grapple_Engine *engine)
{
    return IsPixel(engine) || SDL_fabsf(engine->graphics.render_scale - 1.0f) > SCALE_EPSILON;
}

/* (Re)make a render-target texture of the given size and filter. */
static bool EnsureTexture(Grapple_Engine *engine, SDL_Texture **texture, int *have_w,
                          int *have_h, int width, int height, SDL_ScaleMode filter)
{
    if (*texture != NULL && *have_w == width && *have_h == height)
    {
        return true;
    }
    if (*texture != NULL)
    {
        SDL_DestroyTexture(*texture);
        *texture = NULL;
    }
    /* An 8-bit target is enough for a 2D game and is the one format every
       backend supports as a render target. */
    *texture = SDL_CreateTexture(engine->renderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, width, height);
    if (*texture == NULL)
    {
        return false;
    }
    *have_w = width;
    *have_h = height;
    SDL_SetTextureScaleMode(*texture, filter);
    return true;
}

static bool EnsureTarget(Grapple_Engine *engine, int width, int height)
{
    /* Under PIXEL_SNAP the frame is enlarged by a whole number, and point
       sampling is what keeps every art pixel a square block. Under PIXEL it
       is already at that size and is only ever blitted 1:1 or stretched by
       a fraction, and under render scale a 0.75-scale frame must not come
       back blocky: linear for both. The game's own filter setting applies
       to its textures, not to this. */
    return EnsureTexture(engine, &engine->frame_target, &engine->frame_target_w,
                         &engine->frame_target_h, width, height,
                         (engine->presentation == GRAPPLE_PRESENT_PIXEL_SNAP)
                             ? SDL_SCALEMODE_NEAREST
                             : SDL_SCALEMODE_LINEAR);
}

/* The window rectangle the design frame lands in: the largest aspect-true
   fit, centred, and the integer part of its scale. */
static void PixelFit(const Grapple_Engine *engine, int pixel_w, int pixel_h, SDL_FRect *dst,
                     int *whole)
{
    const float design_w = engine->view_width;
    const float design_h = engine->view_height;
    const float scale = SDL_min((float)pixel_w / design_w, (float)pixel_h / design_h);
    *whole = WholeScale(engine, pixel_w, pixel_h);
    dst->w = design_w * scale;
    dst->h = design_h * scale;
    dst->x = ((float)pixel_w - dst->w) * 0.5f;
    dst->y = ((float)pixel_h - dst->h) * 0.5f;
}

/* Put the finished design frame on the window. Integer enlargement with point
   sampling first, then — only if the window is not an exact multiple — a
   linear stretch of the enlarged frame to fill the fit. The stretch is by
   less than one art pixel's worth, which reads as a faint softening rather
   than as pixels of uneven size. */
static void PresentPixelFrame(Grapple_Engine *engine)
{
    int pixel_w = 0;
    int pixel_h = 0;
    if (!SDL_GetRenderOutputSize(engine->renderer, &pixel_w, &pixel_h) || pixel_w <= 0 ||
        pixel_h <= 0)
    {
        return;
    }
    SDL_FRect dst;
    int whole = 1;
    PixelFit(engine, pixel_w, pixel_h, &dst, &whole);

    /* The bars. */
    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255);
    SDL_RenderClear(engine->renderer);

    const int enlarged_w = (int)(engine->view_width + 0.5f) * whole;
    const int enlarged_h = (int)(engine->view_height + 0.5f) * whole;
    const bool exact = SDL_fabsf(dst.w - (float)enlarged_w) < 0.5f &&
                       SDL_fabsf(dst.h - (float)enlarged_h) < 0.5f;
    if (exact)
    {
        /* A display the design divides: one blit, point sampled from a
           design-size frame, 1:1 from a frame already at this size. */
        dst.x = SDL_floorf(dst.x);
        dst.y = SDL_floorf(dst.y);
        dst.w = (float)enlarged_w;
        dst.h = (float)enlarged_h;
        SDL_RenderTexture(engine->renderer, engine->frame_target, NULL, &dst);
        return;
    }
    if (engine->frame_target_w == enlarged_w && engine->frame_target_h == enlarged_h)
    {
        /* PIXEL: the frame is already the enlarged size, and is linear. */
        SDL_RenderTexture(engine->renderer, engine->frame_target, NULL, &dst);
        return;
    }
    if (!EnsureTexture(engine, &engine->pixel_target, &engine->pixel_target_w,
                       &engine->pixel_target_h, enlarged_w, enlarged_h, SDL_SCALEMODE_LINEAR) ||
        !SDL_SetRenderTarget(engine->renderer, engine->pixel_target))
    {
        /* No memory for the middle step: a single linear fit is the next
           best thing, and still a whole frame rather than nothing. */
        SDL_RenderTexture(engine->renderer, engine->frame_target, NULL, &dst);
        return;
    }
    SDL_RenderTexture(engine->renderer, engine->frame_target, NULL, NULL); /* point sampled */
    SDL_SetRenderTarget(engine->renderer, NULL);
    SDL_SetRenderDrawBlendMode(engine->renderer, SDL_BLENDMODE_NONE);
    SDL_RenderTexture(engine->renderer, engine->pixel_target, NULL, &dst);       /* linear */
}

void Grapple_EngineBeginFrameTarget(Grapple_Engine *engine)
{
    engine->frame_target_active = false;
    if (engine->renderer == NULL || !WantsTarget(engine))
    {
        return;
    }

    int pixel_w = 0;
    int pixel_h = 0;
    if (!SDL_GetRenderOutputSize(engine->renderer, &pixel_w, &pixel_h) || pixel_w <= 0 ||
        pixel_h <= 0)
    {
        return;
    }

    int target_w;
    int target_h;
    if (IsPixel(engine))
    {
        /* The frame is the design times a whole number of screen pixels
           per unit — one under PIXEL_SNAP. Render scale does not apply:
           there is no resolution to trade. */
        const int whole = (engine->presentation == GRAPPLE_PRESENT_PIXEL_SNAP)
                              ? 1
                              : WholeScale(engine, pixel_w, pixel_h);
        target_w = (int)(engine->view_width + 0.5f) * whole;
        target_h = (int)(engine->view_height + 0.5f) * whole;
    }
    else
    {
        const float scale = engine->graphics.render_scale;
        target_w = (int)((float)pixel_w * scale + 0.5f);
        target_h = (int)((float)pixel_h * scale + 0.5f);
    }
    if (target_w < 1)
    {
        target_w = 1;
    }
    if (target_h < 1)
    {
        target_h = 1;
    }

    if (!EnsureTarget(engine, target_w, target_h))
    {
        return; /* out of texture memory: draw to the window and carry on */
    }

    if (!SDL_SetRenderTarget(engine->renderer, engine->frame_target))
    {
        return;
    }
    engine->frame_target_active = true;

    /* Re-apply the logical presentation against the target. Without this
       the game would draw into a 1440x810 texture at the scale worked out
       for a 1920x1080 window, and everything would be a quarter too big. */
    /* Design units onto the target: 1:1 for a design-size frame, and an
       exact whole-number letterbox — the target has that aspect, so no
       bars — for a frame drawn at a multiple of it. */
    const bool one_to_one = engine->presentation == GRAPPLE_PRESENT_NATIVE ||
                            engine->presentation == GRAPPLE_PRESENT_PIXEL_SNAP;
    SDL_SetRenderLogicalPresentation(engine->renderer, (int)(engine->view_width + 0.5f),
                                     (int)(engine->view_height + 0.5f),
                                     one_to_one ? SDL_LOGICAL_PRESENTATION_DISABLED
                                                : SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void Grapple_EngineEndFrameTarget(Grapple_Engine *engine)
{
    if (engine == NULL || engine->renderer == NULL)
    {
        return;
    }

    if (engine->frame_target_active && engine->frame_target != NULL)
    {
        engine->frame_target_active = false;
        SDL_SetRenderTarget(engine->renderer, NULL);
        /* Blit in window pixels, not design coordinates: this is the one
           draw in the frame that is about the display rather than the
           game. */
        SDL_SetRenderLogicalPresentation(engine->renderer, 0, 0,
                                         SDL_LOGICAL_PRESENTATION_DISABLED);
        SDL_SetRenderDrawBlendMode(engine->renderer, SDL_BLENDMODE_NONE);
        if (IsPixel(engine))
        {
            PresentPixelFrame(engine);
        }
        else
        {
            SDL_RenderTexture(engine->renderer, engine->frame_target, NULL, NULL);
        }

        /* Put the presentation back, so anything drawn after this — a debug
           overlay, the next frame — sees the coordinates it expects. */
        Grapple_EngineSetPresentation(engine, engine->presentation);
    }

    /* The chain runs over whatever is now in the framebuffer, which is the
       upscaled frame if there was a target and the frame itself if not. */
    Grapple_EnginePostFXPresent(engine);
}

void Grapple_EngineDestroyFrameTarget(Grapple_Engine *engine)
{
    if (engine == NULL)
    {
        return;
    }
    if (engine->frame_target != NULL)
    {
        SDL_DestroyTexture(engine->frame_target);
        engine->frame_target = NULL;
    }
    if (engine->pixel_target != NULL)
    {
        SDL_DestroyTexture(engine->pixel_target);
        engine->pixel_target = NULL;
    }
    engine->frame_target_w = 0;
    engine->frame_target_h = 0;
}
