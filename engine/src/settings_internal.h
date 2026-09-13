#ifndef GRAPPLE_SETTINGS_INTERNAL_H
#define GRAPPLE_SETTINGS_INTERNAL_H
#include <grapple/engine_settings.h>
#define SETTINGS_CAPACITY 64
struct Grapple_Settings
{
    char *player_path;
    char *values[SETTINGS_CAPACITY];
    char *sources[SETTINGS_CAPACITY];
    bool provided[SETTINGS_CAPACITY];
};
bool Settings_Capture(Grapple_Settings *settings, const Grapple_EngineConfig *config,
                      const Grapple_GraphicsSettings *graphics, const char *source);
void Settings_Swap(Grapple_Settings *a, Grapple_Settings *b);
#endif
