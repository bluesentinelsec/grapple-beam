/** @file engine_settings.h @brief Typed layered engine settings and preference persistence. */
#ifndef GRAPPLE_ENGINE_SETTINGS_H
#define GRAPPLE_ENGINE_SETTINGS_H
#include <grapple/engine_graphics.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /** @brief Owned settings snapshot. Values, presence and sources are retained per field. */
    typedef struct Grapple_Settings Grapple_Settings;
    /** @brief Number of registered settings. @return Registry size. */
    extern int Grapple_SettingCount(void);
    /** @brief Registered canonical key. @param index Registry index. @return Static name or NULL.
     */
    extern const char *Grapple_SettingKey(int index);
    /** @brief CLI spelling without dashes. @param index Registry index. @return Static name, empty
     * for advanced keys, or NULL. */
    extern const char *Grapple_SettingOption(int index);
    /** @brief Accepted values/range. @param index Registry index. @return Static description or
     * NULL. */
    extern const char *Grapple_SettingChoices(int index);
    /** @brief Allocate opinionated engine defaults. @return Owned snapshot or NULL on OOM. */
    extern Grapple_Settings *Grapple_CreateSettings(void);
    /** @brief Copy a snapshot. @param settings Borrowed input. @return Owned copy or NULL. */
    extern Grapple_Settings *Grapple_CloneSettings(const Grapple_Settings *settings);
    /** @brief Release a snapshot. @param settings Owned snapshot; NULL is accepted. */
    extern void Grapple_DestroySettings(Grapple_Settings *settings);
    /** @brief Validate and overlay one field, preserving explicit false and zero.
     * @param settings Mutable snapshot. @param key Canonical key or CLI option name.
     * @param value String representation (on/off for booleans). @param source Diagnostic origin.
     * @return True on success, false with SDL_GetError(); failures leave the snapshot unchanged.
     * All strings are copied. Call on the owning thread. */
    extern bool Grapple_SettingsSet(Grapple_Settings *settings, const char *key, const char *value,
                                    const char *source);
    /** @brief Read a value. @param settings Snapshot. @param key Registered key or CLI name.
     * @return Borrowed string, valid until mutation/destruction, or NULL on invalid input. */
    extern const char *Grapple_SettingsGet(const Grapple_Settings *settings, const char *key);
    /** @brief Read a field's origin. @param settings Snapshot. @param key Registered key.
     * @return Borrowed origin string or NULL. Engine defaults are labeled "engine defaults". */
    extern const char *Grapple_SettingsSource(const Grapple_Settings *settings, const char *key);
    /** @brief Overlay fields explicitly provided by another snapshot.
     * @param settings Destination. @param overlay Borrowed overlay. @return Success; atomic on
     * failure. */
    extern bool Grapple_SettingsOverlay(Grapple_Settings *settings,
                                        const Grapple_Settings *overlay);
    /** @brief Apply a quality preset before individual overrides.
     * @param settings Snapshot. @param quality low|medium|high. @param source Origin.
     * @return Success; invalid input leaves the snapshot unchanged. */
    extern bool Grapple_SettingsQuality(Grapple_Settings *settings, const char *quality,
                                        const char *source);
    /** @brief Load a strict TOML overlay atomically.
     * @param settings Destination. @param toml Text. @param source File/origin for diagnostics.
     * @return Success, or false with SDL_GetError(). Unknown keys and types are errors. */
    extern bool Grapple_SettingsLoadToml(Grapple_Settings *settings, const char *toml,
                                         const char *source);
    /** @brief Load a TOML file. @param settings Destination. @param path Filename.
     * @return Success. Relative media paths are anchored to the configuration file's directory. */
    extern bool Grapple_SettingsLoadFile(Grapple_Settings *settings, const char *path);
    /** @brief Serialize settings as TOML.
     * @param settings Snapshot. @param overrides_only Omit engine-default fields if true.
     * @return SDL_malloc-owned text, or NULL. Free with SDL_free(). */
    extern char *Grapple_SettingsToToml(const Grapple_Settings *settings, bool overrides_only);
    /** @brief Set the destination for player changes; does not read/write files.
     * @param settings Snapshot. @param path Player TOML path (copied).
     * @return Success. The runner supplies this from the project identity. */
    extern bool Grapple_SettingsSetPlayerPath(Grapple_Settings *settings, const char *path);
    /** @brief Player preference destination.
     * @param settings Snapshot. @return Borrowed path or NULL for an unconfigured embedding. */
    extern const char *Grapple_SettingsPlayerPath(const Grapple_Settings *settings);
    /** @brief Persist explicitly changed player preferences without saving launch overrides.
     * @param changes Snapshot containing only player changes. @param path Player TOML filename.
     * @return Success. Existing player TOML fields are retained, then changes overlaid;
     * uses a temporary file and atomic replacement. Never serializes unrelated launch values.
     * Caller must provide a player-changes snapshot, not the effective launch snapshot. */
    extern bool Grapple_SettingsSaveChanges(const Grapple_Settings *changes, const char *path);
    /** @brief Apply settings to startup structs.
     * @param settings Snapshot. @param config Mutable startup config.
     * @param graphics Mutable graphics initialized by the caller.
     * @param overrides_only Apply only provided fields when true.
     * @return Success; pointers in config borrow snapshot strings until engine creation completes.
     */
    extern bool Grapple_SettingsApply(const Grapple_Settings *settings,
                                      Grapple_EngineConfig *config,
                                      Grapple_GraphicsSettings *graphics, bool overrides_only);
    /** @brief Capture runtime graphics and startup configuration as a settings snapshot.
     * @param config Startup fields. @param graphics Graphics fields.
     * @return Owned snapshot or NULL. Source is "game startup"; unset dimensions use engine
     * defaults. Use this before overlays when embedding the engine without the CLI. */
    extern Grapple_Settings *Grapple_SettingsCapture(const Grapple_EngineConfig *config,
                                                     const Grapple_GraphicsSettings *graphics);
    /** @brief Inspect requested settings retained by an engine.
     * @param engine Engine. @return Borrowed snapshot valid until engine destruction. */
    extern const Grapple_Settings *Grapple_EngineRequestedSettings(Grapple_Engine *engine);
    /** @brief Inspect applied runtime settings.
     * @param engine Engine. @return Owned snapshot; free with Grapple_DestroySettings().
     * Graphics reflect platform application and reduced-flashing constraints. Audio values are
     * launch requests; query Grapple_GetAudioBusGain/Grapple_AudioMuted for current mixer state.
     * Effect availability is reported separately by Grapple_EngineEffectsAvailable(). */
    extern Grapple_Settings *Grapple_EngineActualSettings(Grapple_Engine *engine);
    /** @brief Explain how a setting takes effect.
     * @param key Registered key or CLI option. @return Static policy string or NULL for unknown
     * keys. "restart" requires a new engine; "graphics" uses EngineSetGraphics; "audio" uses mixer
     * bus setters; "game" uses the camera/UI/quality helpers and the game's own systems. */
    extern const char *Grapple_SettingPolicy(const char *key);
    /** @brief Install a process launch snapshot for subsequent engine creation.
     * @param settings Borrowed until replaced with NULL; caller retains ownership.
     * @details Main-thread only. Explicit layers override inline startup config.
     * Defaults are used when constructing a launcher-managed engine. Clear before destroying.
     */
    extern void Grapple_SetLaunchSettings(const Grapple_Settings *settings);
    /** @brief Current launch snapshot. @return Borrowed snapshot or NULL. Main-thread only. */
    extern const Grapple_Settings *Grapple_GetLaunchSettings(void);
#ifdef __cplusplus
}
#endif
#endif
