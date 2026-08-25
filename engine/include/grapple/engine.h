/**
 * @file engine.h
 * @brief Grapple::Engine — the frame loop, time, and the drawing surface.
 *
 * Original Grapple code (zlib). This is the core of an opinionated game
 * engine: it owns the loop, so it can do the things a library cannot.
 *
 * The loop is a **fixed simulation tick with interpolated rendering**, the
 * arrangement Godot and Unity both use, because a loop that simply
 * multiplies by a measured delta looks worse than it reads:
 *
 *   - Measured frame times are noisy (16.4, 17.1, 15.9 ms), so a steady
 *     motion advances by a different amount each refresh. The eye reads
 *     that as shimmer.
 *   - The display is quantised and a free-running simulation is not, so
 *     even perfectly delivered frames judder.
 *
 * So: the simulation advances in exact steps (60 Hz by default), the
 * measured delta is snapped to the display's cadence before it is used, and
 * rendering happens *between* two simulation states using an interpolation
 * factor. See docs/lighting.md's sibling, docs/engine.md, for the full
 * argument.
 *
 *     Grapple_GameHooks hooks = {0};
 *     hooks.fixed_update = FixedUpdate;   // simulation: 60 Hz, exact
 *     hooks.render = Render;              // drawing: display rate, smooth
 *
 *     Grapple_Engine *engine = Grapple_CreateEngine(NULL);
 *     Grapple_RunGame(engine, &hooks, &game);
 *     Grapple_DestroyEngine(engine);
 *
 * Coordinates are **design coordinates**, not pixels: the engine sets a
 * logical presentation size (1920x1080 by default) and SDL scales it to
 * whatever the display actually is, letterboxed. A game is written once, at
 * one resolution, and looks right on a 1080p laptop and a 4K monitor —
 * see Grapple_EnginePresentation for the other ways to fit it.
 */
#ifndef GRAPPLE_ENGINE_H
#define GRAPPLE_ENGINE_H

#include <SDL3/SDL.h>
#include <grapple/event_sink.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Grapple_Engine Grapple_Engine;

/**
 * How the design resolution is fitted to the window.
 *
 * All of these scale the *coordinates* — SDL applies a transform, it does
 * not render offscreen and resample — so a game authored at 4K costs a
 * 1080p machine nothing extra. What differs is what happens when the
 * window's aspect ratio is not the design's, and there are only three
 * possible answers: show bars, crop, or distort.
 *
 *     Grapple_EngineConfig config = {0};
 *     config.presentation = GRAPPLE_PRESENT_LETTERBOX;   // already the default
 *     Grapple_Engine *engine = Grapple_CreateEngine(&config);
 *
 * and to change it later, from an options menu:
 *
 *     Grapple_EngineSetPresentation(engine, GRAPPLE_PRESENT_INTEGER);
 *
 * See docs/engine.md for the full comparison.
 */
typedef enum Grapple_EnginePresentation
{
    /** Fit the whole design space, preserving aspect, with bars where the
     *  window is a different shape.
     *
     *  The default, and the right answer for most games: every player sees
     *  exactly the frame the game was composed in, at every window size and
     *  on every monitor, with no distortion and nothing cropped. The view
     *  rect never changes, so UI may be positioned against fixed
     *  coordinates. The cost is bars on a mismatched display. */
    GRAPPLE_PRESENT_LETTERBOX = 0,
    /** Keep the design's shorter axis and let the other grow or shrink with
     *  the window, so an ultrawide monitor **sees more world** instead of
     *  black bars. No bars, no cropping, no distortion either — the trade is
     *  that the visible area is not fixed, so UI must anchor to
     *  Grapple_EngineSafeRect, and a player on a wider monitor sees
     *  further. What Godot calls "expand". */
    GRAPPLE_PRESENT_EXPAND,
    /** Fill the window by cropping the overflow. Preserves aspect, no bars,
     *  but part of the design space is off-screen — the view and safe rects
     *  shrink to what survives. */
    GRAPPLE_PRESENT_OVERSCAN,
    /** Whole-number scale, for pixel art. Also switches texture filtering to
     *  nearest, because a fractional scale is what makes pixel art shimmer. */
    GRAPPLE_PRESENT_INTEGER,
    GRAPPLE_PRESENT_STRETCH, /**< fill, ignoring aspect ratio: distorts */
    GRAPPLE_PRESENT_NATIVE   /**< no scaling: coordinates are pixels */
} Grapple_EnginePresentation;

/**
 * Which SDL renderer backend to run on.
 *
 * SDL ships several — on a Mac it offers metal, opengl, opengles2, vulkan,
 * gpu and software — and left alone it picks the platform's native one:
 * Metal on Apple, Direct3D on Windows, OpenGL elsewhere.
 *
 * This engine picks **OpenGL, everywhere, by default**, and that is a
 * deliberate trade:
 *
 *   One shader language. The post-processing chain and the lighting module
 *   are GLSL. Under a native backend they cannot run at all, so the same
 *   game looks different on macOS from how it looks on Linux — for no
 *   reason the player can see. Writing them again in MSL, HLSL and SPIR-V
 *   is three more implementations to keep in step.
 *
 *   One code path to test. GLSL 1.x runs on desktop GL, GLES on mobile and
 *   WebGL in a browser, which covers every platform this project targets.
 *
 * The cost is real and worth stating. Apple deprecated OpenGL in 2018: it
 * still works, it is capped at 4.1, and it will not improve. Metal has
 * lower CPU overhead. Some Windows OEM drivers have weaker GL than their
 * Direct3D. A 2D game is very unlikely to notice any of it — but a game
 * that measures a difference, or is shipping on a platform where GL is
 * genuinely worse, should say `GRAPPLE_BACKEND_NATIVE` and give up the
 * shader effects knowingly rather than by accident.
 */
typedef enum Grapple_EngineBackend
{
    /** OpenGL / OpenGL ES: the default, and the only one where the shader
     *  effects work. Falls back to whatever SDL can provide if the machine
     *  genuinely has no GL, rather than refusing to start. */
    GRAPPLE_BACKEND_OPENGL = 0,
    /** Whatever SDL would have chosen: Metal, Direct3D, Vulkan. Faster on
     *  paper; no post-processing. */
    GRAPPLE_BACKEND_NATIVE,
    /** The software renderer. For tools, and for a machine whose drivers
     *  are broken enough that nothing else starts. */
    GRAPPLE_BACKEND_SOFTWARE
} Grapple_EngineBackend;

/** How rendering positions things between two simulation steps. */
typedef enum Grapple_EngineInterpolation
{
    GRAPPLE_INTERPOLATE_LERP = 0, /**< between previous and current (default) */
    GRAPPLE_INTERPOLATE_NONE,     /**< snap to the latest step */
    GRAPPLE_INTERPOLATE_EXTRAPOLATE /**< predict past current: no render lag,
                                           at the cost of overshoot on sharp
                                           direction changes */
} Grapple_EngineInterpolation;

/**
 * Engine configuration. Zero-initialise and set what matters; every field
 * has a sensible default, so `Grapple_CreateEngine(NULL)` is a valid way
 * to start.
 */
typedef struct Grapple_EngineConfig
{
    const char *title;   /**< window title; "Grapple" if NULL */
    int window_width;    /**< window size in pixels; 1280x720 if zero */
    int window_height;
    /** The coordinate space the game is written in — its *reference*
     *  resolution, not the resolution it renders at. 1920x1080 if zero.
     *
     *  Pick the aspect and the convenient number, not the target hardware:
     *  a game written at 1920x1080 renders natively at 4K, because the
     *  scale is applied to coordinates rather than to a rendered image. Art
     *  is a separate question — see Grapple_EngineAssetScale. */
    int design_width;
    int design_height;
    Grapple_EnginePresentation presentation;
    bool fullscreen;

    /* Opt-outs, not opt-ins: a zero-initialised config must give the
       documented defaults, so the fields that default to *on* are phrased
       as the thing you turn off. */
    bool no_vsync;    /**< vsync is on by default: free pacing, and it
                           quantises the frame delta for smoothing */
    bool fixed_size;  /**< windows are resizable by default */
    bool low_dpi;     /**< high-DPI is on by default */

    /** Upper limit on frames per second; 0 means "follow the display".
     *
     *  Vsync usually paces the loop for free, but not always — an occluded
     *  or off-screen window may not block on present, and some drivers
     *  ignore the request entirely. Left to itself the loop then free-runs
     *  at thousands of frames a second, burning battery to draw images
     *  nobody sees. The engine therefore sleeps out the remainder of each
     *  frame: by default to the display's refresh rate, or to this value
     *  when set. Use a negative value to disable the limiter outright. */
    int max_fps;

    int tick_rate;    /**< simulation steps per second; 60 if zero. 120 is
                           the other sensible value, and is worth it when the
                           engine is running physics. */
    int max_steps_per_frame; /**< catch-up cap; 5 if zero */
    float max_frame_seconds; /**< longer than this is a stall, not a slow
                                  frame, and the time is dropped; 0.25 if 0 */
    Grapple_EngineInterpolation interpolation;

    /** Graphics settings to start with — usually the result of
     *  Grapple_GraphicsResolve, which is how a config file and the command
     *  line reach the engine. NULL means Grapple_GraphicsDefaults().
     *
     *  These win over `presentation`, `no_vsync` and `max_fps` above, which
     *  remain for games that want nothing to do with settings files. */
    const struct Grapple_GraphicsSettings *graphics;

    /** Which of SDL's renderer backends to use. Zero means OpenGL, which is
     *  this engine's opinion — see Grapple_EngineBackend. */
    Grapple_EngineBackend backend;

    /** The program's command line, so the engine can honour the arguments
     *  it documents — `--media`, `--media-password`. Pass them straight
     *  through from main(); anything the engine does not recognise is left
     *  alone, because the game owns this command line.
     *
     *  Graphics settings are read from argv separately, by
     *  Grapple_GraphicsResolve, so that a game can inspect or override
     *  them before the engine exists. */
    int argc;
    char *const *argv;

    /** Where the game's assets are. NULL runs the default search — an
     *  embedded archive, then media.zip, media.dat and media/ — which is
     *  what almost every game should do. An explicit path here beats
     *  `--media` on the command line. See engine_media.h. */
    const char *media_path;
    /** Do not mount anything. For a game that manages its own VFS. */
    bool no_auto_mount;

    /** Headless: software renderer, no window. For tests and tools. */
    bool headless;
    /** Drive time with Grapple_EngineAdvance instead of the clock, so a
     *  test can simulate an hour of frames in a millisecond. */
    bool manual_clock;
} Grapple_EngineConfig;

/**
 * What a game implements. Every hook is optional; a game with only `render`
 * is valid (and is how you write a screensaver).
 *
 * The split between `fixed_update` and `update` is the important one:
 *
 *   fixed_update(step)  runs 0..max_steps times a frame, always with the
 *                       same `step`. Simulation goes here — movement,
 *                       physics, AI — because it must not depend on how
 *                       fast the machine happens to be.
 *   update(dt)          runs exactly once a frame with the real delta.
 *                       Cosmetics go here — camera smoothing, UI tweens.
 *   render(alpha)       runs once a frame. `alpha` is how far the frame
 *                       sits between the previous simulation step and the
 *                       current one, in [0, 1). Draw at
 *                       `lerp(previous, current, alpha)` and motion is
 *                       smooth even when the tick rate and the refresh rate
 *                       disagree.
 */
typedef struct Grapple_GameHooks
{
    bool (*load)(void *user);                      /**< once, before the loop */
    void (*fixed_update)(void *user, float step);  /**< simulation */
    void (*update)(void *user, float dt);          /**< per-frame cosmetics */
    void (*render)(void *user, float alpha);       /**< draw */
    /** Draw *after* the post-processing chain, over the finished frame and
     *  before it is presented.
     *
     *  Two things need this. A HUD usually should not be scanlined,
     *  pixelated or chromatically split along with the world — real CRT
     *  games had no UI layer, and applying the effect to one looks like a
     *  bug rather than a style. And a screenshot of what the player
     *  actually saw can only be taken here, because everything before this
     *  point is the frame *without* the effects.
     *
     *  Coordinates are design coordinates, the same as `render`. */
    void (*post_render)(void *user);
    void (*event)(void *user, const SDL_Event *event);
    void (*resize)(void *user, int width, int height);
    void (*unload)(void *user);                    /**< once, after the loop */
} Grapple_GameHooks;

/* --- lifecycle ---------------------------------------------------------- */

/** Create an engine (and its window and renderer). `config` may be NULL for
 *  the defaults. Returns NULL and sets SDL_GetError() on failure. */
extern Grapple_Engine *Grapple_CreateEngine(const Grapple_EngineConfig *config);

extern void Grapple_DestroyEngine(Grapple_Engine *engine);

/** Run until something calls Grapple_EngineQuit or the window closes.
 *
 *  On the web this hands the loop to the browser and returns immediately —
 *  which is why cleanup belongs in the `unload` hook rather than after this
 *  call. Returns false if `load` failed. */
extern bool Grapple_RunGame(Grapple_Engine *engine, const Grapple_GameHooks *hooks,
                              void *user);

/** One iteration of the loop, for a game that wants to own its own. Returns
 *  false once the engine has been asked to stop. */
extern bool Grapple_EngineTick(Grapple_Engine *engine);

/**
 * Echo every event of every frame to `sink`, bracketed by its begin/end.
 *
 * This exists so that an immediate-mode GUI does not have to be driven by
 * hand. The engine pumps events itself, before it calls any hook, so there
 * is no hook in which a caller could open Nuklear's input window — hence
 * the engine opening it:
 *
 *   const Grapple_EventSink sink = Grapple_GuiEventSink(gui);
 *   Grapple_EngineSetEventSink(engine, &sink);
 *
 * The sink is copied. Pass NULL to remove it. The `event` hook still fires
 * as before: a sink is an addition, not a replacement.
 */
extern void Grapple_EngineSetEventSink(Grapple_Engine *engine, const Grapple_EventSink *sink);

/**
 * Draw `draw` after everything else in the frame, including post_render.
 *
 * For a layer that draws itself — a retained UI, a console — so that using
 * one does not spend the game's own post_render hook. Pass NULL to remove.
 *
 *   Grapple_EngineSetOverlay(engine, Grapple_UiDrawCallback, ui);
 */
extern void Grapple_EngineSetOverlay(Grapple_Engine *engine, void (*draw)(void *user),
                                       void *user);

/** Install the hooks without handing over the loop.
 *
 *  Grapple_RunGame does this for you. A game driving the loop itself with
 *  Grapple_EngineTick needs it, because otherwise there is no way to be
 *  called back at all — which made the manual-loop path documented but not
 *  actually usable.
 *
 *  `hooks` is **not** copied: it must outlive the engine, which is what a
 *  static or a member gives you for free. Pass NULL to detach. */
extern void Grapple_EngineSetHooks(Grapple_Engine *engine,
                                     const Grapple_GameHooks *hooks, void *user);

/** Ask the loop to stop after the current frame. */
extern void Grapple_EngineQuit(Grapple_Engine *engine);

/* --- the frame ---------------------------------------------------------- */

/** Seconds of real time the last frame took, after clamping and smoothing. */
extern float Grapple_EngineDelta(Grapple_Engine *engine);

/** Interpolation factor for the current frame, in [0, 1). */
extern float Grapple_EngineAlpha(Grapple_Engine *engine);

/** Fixed step length in seconds — 1/tick_rate. Constant for the run. */
extern float Grapple_EngineStep(Grapple_Engine *engine);

/** Simulation steps run in the last frame; usually 1. Zero means the frame
 *  arrived early, two or more means it arrived late. */
extern int Grapple_EngineStepsLastFrame(Grapple_Engine *engine);

/** Frames in which the machine could not keep up and simulation time was
 *  dropped. A steadily rising number is the signal to lower quality. */
extern int Grapple_EngineOverloadFrames(Grapple_Engine *engine);

/** Frames rendered since the engine started. */
extern Uint64 Grapple_EngineFrameCount(Grapple_Engine *engine);

/** Smoothed frames per second, for a debug overlay. */
extern float Grapple_EngineFps(Grapple_Engine *engine);

/** Change the frame-rate limit at runtime — an options menu will want to.
 *  0 follows the display, negative disables the limiter. */
extern void Grapple_EngineSetMaxFps(Grapple_Engine *engine, int max_fps);
extern int Grapple_EngineMaxFps(Grapple_Engine *engine);

/** Scale simulation time: 0 pauses, 0.5 is slow motion, 2 is double speed.
 *  Rendering and `update` are unaffected, so menus still animate while the
 *  world is paused. */
extern void Grapple_EngineSetTimeScale(Grapple_Engine *engine, float scale);
extern float Grapple_EngineTimeScale(Grapple_Engine *engine);

/** Change the simulation rate at runtime (30, 60, 120...). Clamped to
 *  [10, 480]. Existing accumulated time is rescaled, so this is safe to
 *  call from an options menu. */
extern bool Grapple_EngineSetTickRate(Grapple_Engine *engine, int ticks_per_second);
extern int Grapple_EngineTickRate(Grapple_Engine *engine);

/* --- the surface -------------------------------------------------------- */

extern SDL_Renderer *Grapple_EngineRenderer(Grapple_Engine *engine);
extern SDL_Window *Grapple_EngineWindow(Grapple_Engine *engine);

/** Change how the design space is fitted to the window, at runtime.
 *
 *  An options menu will want this — "letterbox" versus "expand" is a taste
 *  players have opinions about — and the view rect is recomputed
 *  immediately, so the next frame is already laid out for the new mode. */
extern bool Grapple_EngineSetPresentation(Grapple_Engine *engine,
                                            Grapple_EnginePresentation mode);

extern Grapple_EnginePresentation Grapple_EnginePresentation_(Grapple_Engine *engine);

/** The design (reference) space the game was configured with. */
extern void Grapple_EngineDesignSize(Grapple_Engine *engine, int *width, int *height);

/** The design-space rectangle actually visible in the window.
 *
 *  Equal to the design size for every mode except EXPAND, where the window's
 *  aspect ratio widens or heightens it — a 21:9 monitor showing a 16:9
 *  design gets a wider view rather than bars. Origin is 0,0 in EXPAND and
 *  LETTERBOX; OVERSCAN crops, so its origin is negative.
 *
 *  This is the rectangle to lay a camera or a full-screen backdrop out
 *  against. */
extern SDL_FRect Grapple_EngineViewRect(Grapple_Engine *engine);

/** The part of the view guaranteed to be visible on every aspect ratio —
 *  the design rectangle, centred in the view.
 *
 *  Anchor UI to this, not to the view: a button at the right edge of the
 *  *view* sits off in the periphery on an ultrawide, while the safe rect is
 *  where the game was actually composed. Consoles call the same idea a
 *  title-safe area. */
extern SDL_FRect Grapple_EngineSafeRect(Grapple_Engine *engine);

/** The window's real size in pixels — the framebuffer, not the design
 *  space, and not points on a high-DPI display. */
extern void Grapple_EnginePixelSize(Grapple_Engine *engine, int *width, int *height);

/** How many pixels one design unit covers right now.
 *
 *  1.0 when the window matches the design space, 2.0 for a 1920-wide design
 *  on a 4K display. Line widths and other things that should stay
 *  pixel-crisp can divide by it. */
extern float Grapple_EngineRenderScale(Grapple_Engine *engine);

/** Which art set to load: 1 below a 1.5x render scale, 2 below 3x, then 4.
 *
 *  The scaling above is free because it applies to coordinates, but *art* is
 *  not: a 1x sprite stretched onto a 4K display is soft, and a 4K sprite
 *  squeezed onto a laptop wastes memory and shimmers under minification.
 *  Games that ship more than one set of art choose with this. */
extern int Grapple_EngineAssetScale(Grapple_Engine *engine);

/** Colour the frame is cleared to before `render`. Defaults to near-black. */
extern void Grapple_EngineSetClearColor(Grapple_Engine *engine, SDL_FColor color);

/** Convert a window position (an SDL mouse or touch coordinate) into design
 *  coordinates. Needed because the game thinks in design space and SDL
 *  reports events in window space. */
extern void Grapple_EngineWindowToDesign(Grapple_Engine *engine, float window_x,
                                           float window_y, float *design_x,
                                           float *design_y);

/* --- testing ------------------------------------------------------------ */

/** Advance the manual clock by `nanoseconds`.
 *
 *  Only meaningful when the engine was created with `manual_clock`. This is
 *  what makes the loop testable: a test can feed exact, or deliberately
 *  awful, frame times and assert on how many simulation steps came out. */
extern void Grapple_EngineAdvance(Grapple_Engine *engine, Uint64 nanoseconds);

/** Pretend the display refreshes at this rate, so delta smoothing can be
 *  tested without a real monitor. Zero disables smoothing. */
extern void Grapple_EngineSetRefreshRate(Grapple_Engine *engine, float hz);

#ifdef __cplusplus
}
#endif

#endif /* GRAPPLE_ENGINE_H */
