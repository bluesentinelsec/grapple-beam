#ifndef GRAPPLE_RUNNER_LAUNCH_HPP
#define GRAPPLE_RUNNER_LAUNCH_HPP
#include <filesystem>
#include <grapple/engine_settings.h>
#include <memory>
#include <string>
#include <vector>
namespace grapple::runner
{
using SettingsPtr = std::unique_ptr<Grapple_Settings, decltype(&Grapple_DestroySettings)>;
struct LaunchRequest
{
    std::filesystem::path project;
    std::string language;
    std::vector<std::filesystem::path> configs;
    std::vector<std::filesystem::path> scripts;
    bool defaults = false, reset = false, safe = false, print = false;
};
struct Launch
{
    std::filesystem::path root, entry, preferences;
    std::string language;
    SettingsPtr settings{nullptr, Grapple_DestroySettings};
};
Launch ResolveLaunch(const LaunchRequest &request, const Grapple_Settings *cli);
void PrintSettings(const Grapple_Settings *settings);
void ApplyAudioSettings(const Grapple_Settings *settings);
} // namespace grapple::runner
#endif
