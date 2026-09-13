/** @file engine_backend.h
 * @brief Concrete renderer selection and capability discovery.
 */
#ifndef GRAPPLE_ENGINE_BACKEND_H
#define GRAPPLE_ENGINE_BACKEND_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** @brief Result of probing one concrete renderer.
     * @details All strings are owned inline. Empty version strings mean that the
     * driver did not report a version; they must not be interpreted as version zero.
     */
    typedef struct Grapple_RenderBackendInfo
    {
        char name[32];         /**< Canonical CLI selection name. */
        char label[64];        /**< Human-readable API family. */
        char renderer[64];     /**< Actual SDL renderer name, if created. */
        char api_version[128]; /**< Queried API version, if available. */
        char device[256];      /**< Queried device name, if available. */
        char driver[256];      /**< Queried driver/version, if available. */
        char error[512];       /**< Reason creation failed, if unavailable. */
        bool compiled;         /**< This SDL build contains the renderer. */
        bool available;        /**< A hidden-window renderer probe succeeded. */
        bool opengl_effects;   /**< Renderer supports the engine's GL effect path. */
    } Grapple_RenderBackendInfo;

    /** @brief Number of concrete backend names recognized by the engine.
     * @return Count, including backends not compiled for the current platform.
     */
    extern int Grapple_RenderBackendCount(void);

    /** @brief Get a canonical backend name by index.
     * @param index Zero-based index below Grapple_RenderBackendCount().
     * @return Borrowed static string, or NULL for an invalid index.
     */
    extern const char *Grapple_RenderBackendName(int index);

    /** @brief Validate a backend selection without initializing video.
     * @param name Canonical concrete name, or "auto".
     * @return True for a recognized selection (not a hardware availability test).
     */
    extern bool Grapple_RenderBackendValid(const char *name);

    /** @brief Create a renderer using a concrete selection without fallback.
     * @param window Borrowed window; must outlive the returned renderer.
     * @param name Canonical backend name, or "auto" for engine preference.
     * @return Owned renderer to destroy with SDL_DestroyRenderer, or NULL with
     * SDL_GetError() set. Video must be initialized; call on the main thread.
     */
    extern SDL_Renderer *Grapple_CreateBackendRenderer(SDL_Window *window, const char *name);

    /** @brief Describe an existing renderer without creating another window.
     * @param renderer Borrowed renderer, accessed on the main thread. @param info Output.
     * @return Success. Empty version/device fields mean not reported by this query. */
    extern bool Grapple_DescribeRenderBackend(SDL_Renderer *renderer,
                                              Grapple_RenderBackendInfo *info);

    /** @brief Probe a concrete renderer using a temporary hidden window.
     * @param name Canonical concrete name ("auto" is not an enumeration entry).
     * @param info Output, copied in full; retained after probe resources are freed.
     * @return True when the query was valid, even if the backend is unavailable.
     * False indicates invalid arguments. Video must be initialized; call on the
     * main thread before creating the game's renderer. No resources are retained.
     */
    extern bool Grapple_ProbeRenderBackend(const char *name, Grapple_RenderBackendInfo *info);

#ifdef __cplusplus
}
#endif
#endif
