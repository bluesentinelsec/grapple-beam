#include "../project/project.hpp"
#include "launch.hpp"
#include "runner.h"

#include <CLI/CLI.hpp>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <grapple/bindings.h>
#include <grapple/engine_backend.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{
std::string Replacement(const std::string &option)
{
    if (option == "--fullscreen")
        return "use --window-mode fullscreen-borderless or fullscreen-exclusive";
    if (option == "--windowed")
        return "use --window-mode windowed";
    if (option == "--no-vsync")
        return "use --vsync off";
    if (option == "--with-safe-mode")
        return "use --safe-mode";
    if (option == "--with-default-settings")
        return "use --default-settings";
    if (option == "-e")
        return "use eval --language lua|ruby --code SOURCE";
    if (option == "--media-password")
        return "configure encrypted media through the embedding API";
    return {};
}

int ListBackends()
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        std::cerr << "error: " << SDL_GetError() << '\n';
        return 1;
    }
    std::cout << "SDL " << SDL_VERSIONNUM_MAJOR(SDL_GetVersion()) << '.'
              << SDL_VERSIONNUM_MINOR(SDL_GetVersion()) << '.'
              << SDL_VERSIONNUM_MICRO(SDL_GetVersion()) << '\n';
    for (int i = 0; i < Grapple_RenderBackendCount(); ++i)
    {
        Grapple_RenderBackendInfo info{};
        if (!Grapple_ProbeRenderBackend(Grapple_RenderBackendName(i), &info))
        {
            std::cerr << "error: " << SDL_GetError() << '\n';
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            return 1;
        }
        std::cout << info.name << " (" << info.label << "): "
                  << (info.available  ? "available"
                      : info.compiled ? "probe failed"
                                      : "not compiled")
                  << '\n';
        if (!info.available)
            std::cout << "  reason: " << info.error << '\n';
        else
        {
            const auto reported = [](const char *text) {
                return text[0] != '\0' ? text : "not reported";
            };
            std::cout << "  SDL renderer: " << info.renderer
                      << "\n  API version: " << reported(info.api_version)
                      << "\n  device: " << reported(info.device)
                      << "\n  driver/vendor: " << reported(info.driver)
                      << "\n  engine OpenGL effects: "
                      << (info.opengl_effects ? "compatible renderer" : "unavailable") << '\n';
        }
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return 0;
}

class ProcessArgs
{
  public:
    explicit ProcessArgs(std::vector<std::string> &values)
    {
        for (auto &value : values)
            pointers_.push_back(value.data());
        Grapple_SetScriptProcessArgs(static_cast<int>(pointers_.size()), pointers_.data());
    }
    ~ProcessArgs()
    {
        Grapple_SetScriptProcessArgs(0, nullptr);
    }
    ProcessArgs(const ProcessArgs &) = delete;
    ProcessArgs &operator=(const ProcessArgs &) = delete;

  private:
    std::vector<char *> pointers_;
};
} // namespace

namespace
{
int ListDisplays(const Grapple_Settings *settings, bool modes)
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        std::cerr << "error: " << SDL_GetError() << '\n';
        return 1;
    }
    int count = 0;
    SDL_DisplayID *ids = SDL_GetDisplays(&count);
    if (!ids)
    {
        std::cerr << "error: " << SDL_GetError() << '\n';
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return 1;
    }
    int selected = 0;
    std::string display = Grapple_SettingsGet(settings, "display");
    if (display == "primary")
    {
        for (int i = 0; i < count; ++i)
            if (ids[i] == SDL_GetPrimaryDisplay())
                selected = i;
    }
    else
        selected = SDL_atoi(display.c_str());
    if (selected < 0 || selected >= count)
    {
        std::cerr << "error: no display " << display << '\n';
        SDL_free(ids);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return 2;
    }
    for (int i = 0; i < count; ++i)
    {
        if (modes && i != selected)
            continue;
        const auto *desktop = SDL_GetDesktopDisplayMode(ids[i]);
        std::cout << i << ": " << SDL_GetDisplayName(ids[i])
                  << (ids[i] == SDL_GetPrimaryDisplay() ? " (primary)" : "");
        if (desktop)
            std::cout << " " << desktop->w << "x" << desktop->h << " @ " << desktop->refresh_rate
                      << " Hz";
        std::cout << '\n';
        if (modes)
        {
            int n = 0;
            SDL_DisplayMode **available = SDL_GetFullscreenDisplayModes(ids[i], &n);
            if (!available)
            {
                std::cerr << "error: " << SDL_GetError() << '\n';
                SDL_free(ids);
                SDL_QuitSubSystem(SDL_INIT_VIDEO);
                return 1;
            }
            for (int j = 0; j < n; ++j)
                std::cout << "  " << available[j]->w << "x" << available[j]->h << " @ "
                          << available[j]->refresh_rate << " Hz\n";
            if (n == 0)
                std::cout << "  no exclusive display modes reported\n";
            SDL_free(available);
        }
    }
    SDL_free(ids);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return 0;
}
class WorkingDirectory
{
  public:
    explicit WorkingDirectory(const std::filesystem::path &next)
        : previous_(std::filesystem::current_path())
    {
        std::filesystem::current_path(next);
    }
    ~WorkingDirectory()
    {
        std::error_code error;
        std::filesystem::current_path(previous_, error);
    }

  private:
    std::filesystem::path previous_;
};
class LaunchSettings
{
  public:
    explicit LaunchSettings(const Grapple_Settings *s) : previous_(Grapple_GetLaunchSettings())
    {
        Grapple_SetLaunchSettings(s);
    }
    ~LaunchSettings()
    {
        Grapple_SetLaunchSettings(previous_);
    }

  private:
    const Grapple_Settings *previous_;
};
} // namespace
int GrappleRunner_Run(int argc, char **argv, const char *version)
{
    if (argc > 1 && (std::string(argv[1]) == "new" || std::string(argv[1]) == "package"))
        return grapple::project::RunCli(argc, argv);
    using namespace grapple::runner;
    CLI::App app{"Create C games with new, ship with package, or run a Lua/Ruby project or script. "
                 "Engine options precede the project; -- "
                 "introduces game arguments.",
                 "grapple-beam"};
    app.set_version_flag("-V,--version", std::string("grapple-beam ") + version);
    app.set_help_all_flag("--help-all", "Include registered settings and advanced options");
    LaunchRequest request;
    app.add_option("-l,--language", request.language,
                   "Language inferred from project or entrypoint")
        ->check(CLI::IsMember({"lua", "ruby"}))
        ->take_last();
    const bool repl = argc > 1 && std::string(argv[1]) == "repl",
               eval = argc > 1 && std::string(argv[1]) == "eval";
    const bool developer = repl || eval;
    std::string code, quality;
    bool backends = false, displays = false, modes = false;
    SettingsPtr cli{Grapple_CreateSettings(), Grapple_DestroySettings};
    if (!cli)
    {
        std::cerr << SDL_GetError() << '\n';
        return 1;
    }
    auto set = [&](const std::string &key, const std::string &raw) {
        std::string value = raw;
        if (!value.empty() && (key == "media" || key == "engine.media"))
            value = std::filesystem::absolute(raw).lexically_normal().string();
        if (!Grapple_SettingsSet(cli.get(), key.c_str(), value.c_str(), "CLI"))
            throw CLI::ValidationError(key, SDL_GetError());
    };
    if (developer)
    {
        app.description(repl ? "Interactive Lua/Ruby shell" : "Evaluate Lua/Ruby source");
        app.get_option("--language")->required();
        if (eval)
            app.add_option("--code", code, "Source to evaluate")->required();
    }
    else
    {
        app.add_flag("--list-backends", backends, "Probe concrete renderers and report versions");
        app.add_flag("--list-displays", displays, "List displays without launching a project");
        app.add_flag("--list-display-modes", modes, "List exclusive modes for --display");
        app.add_flag("--print-settings", request.print,
                     "Resolve requested settings and sources, then exit");
        app.add_flag("--safe-mode", request.safe,
                     "Bypass configuration scripts/files; use recovery defaults");
        app.add_flag("--default-settings", request.defaults,
                     "Ignore player settings for this launch");
        app.add_flag("--reset-settings", request.reset,
                     "Back up player settings and restore game defaults");
        app.add_option_function<std::string>(
               "--config",
               [&](const std::string &v) {
                   request.configs.push_back(std::filesystem::absolute(v));
               },
               "Explicit TOML overlay (repeatable)")
            ->expected(1)
            ->trigger_on_parse();
        app.add_option_function<std::string>(
               "--config-script",
               [&](const std::string &v) {
                   request.scripts.push_back(std::filesystem::absolute(v));
               },
               "Explicit Lua/Ruby settings overlay (repeatable)")
            ->expected(1)
            ->trigger_on_parse();
        app.add_option("--quality", quality,
                       "Preset: low=.75 scale/no FXAA; medium/high=1 scale/FXAA; matching budgets")
            ->check(CLI::IsMember({"low", "medium", "high"}))
            ->take_last();
        app.add_option_function<std::string>(
               "--set",
               [&](const std::string &v) {
                   auto eq = v.find('=');
                   if (eq == std::string::npos)
                       throw CLI::ValidationError("--set", "expected REGISTERED.KEY=VALUE");
                   set(v.substr(0, eq), v.substr(eq + 1));
               },
               "Advanced typed override; keys listed below")
            ->expected(1)
            ->trigger_on_parse();
        std::string footer = "Advanced keys (use --set KEY=VALUE):\n";
        for (int i = 0; i < Grapple_SettingCount(); ++i)
        {
            std::string key = Grapple_SettingKey(i), name = Grapple_SettingOption(i);
            if (name.empty())
            {
                footer += "  " + key + " (" + Grapple_SettingChoices(i) + ")\n";
                continue;
            }
            app.add_option_function<std::string>(
                   "--" + name, [&, key](const std::string &v) { set(key, v); },
                   key + "; inherited default " + Grapple_SettingsGet(cli.get(), key.c_str()))
                ->expected(1)
                ->trigger_on_parse()
                ->type_name(Grapple_SettingChoices(i));
        }
        app.footer(footer + "\nExamples:\n  grapple-beam --window-mode windowed .\n  grapple-beam "
                            "--config player.toml ./game -- --level forest\n");
    }
    std::vector<char *> option_args{argv[0]};
    int game_at = argc;
    for (int i = developer ? 2 : 1; i < argc; ++i)
    {
        std::string token = argv[i];
        if (token == "--")
        {
            game_at = i + 1;
            break;
        }
        if (token.empty() || token[0] != '-')
        {
            if (developer)
            {
                std::cerr << "error: use -- before evaluation arguments\n";
                return 2;
            }
            request.project = argv[i];
            game_at = i + 1;
            if (game_at < argc && std::string(argv[game_at]) == "--")
                ++game_at;
            break;
        }
        std::string key = token.substr(0, token.find('='));
        const auto *option = app.get_option_no_throw(key);
        if (!option)
        {
            auto replacement = Replacement(key);
            std::cerr << "error: " << key << ": "
                      << (replacement.empty() ? "unknown option; see --help" : replacement) << '\n';
            return 2;
        }
        option_args.push_back(argv[i]);
        if (option->get_expected_min() > 0 && token.find('=') == std::string::npos)
        {
            if (i + 1 == argc)
            {
                std::cerr << "error: " << key << " requires a value\n";
                return 2;
            }
            option_args.push_back(argv[++i]);
        }
    }
    try
    {
        app.parse(static_cast<int>(option_args.size()), option_args.data());
    }
    catch (const CLI::ParseError &error)
    {
        return app.exit(error) == 0 ? 0 : 2;
    }
    catch (const std::exception &error)
    {
        std::cerr << "error: " << error.what() << '\n';
        return 2;
    }
    try
    {
        if (developer)
            return GrappleRunner_Execute(request.language.c_str(), eval ? code.c_str() : nullptr,
                                         nullptr, argc - game_at, argv + game_at);
        if (static_cast<int>(backends) + static_cast<int>(displays) + static_cast<int>(modes) > 1)
            throw std::runtime_error("enumeration actions are mutually exclusive");
        if (backends || displays || modes)
        {
            if (!request.project.empty() || request.safe || request.defaults || request.reset ||
                request.print || !request.configs.empty() || !request.scripts.empty() ||
                !quality.empty())
                throw std::runtime_error(
                    "enumeration actions do not launch or configure a project");
            for (int i = 0; i < Grapple_SettingCount(); ++i)
            {
                const auto *key = Grapple_SettingKey(i);
                if (std::string(Grapple_SettingsSource(cli.get(), key)) != "engine defaults" &&
                    !(modes && std::string(key) == "display.display"))
                    throw std::runtime_error(
                        "launch settings are not valid with this enumeration action");
            }
            return backends ? ListBackends() : ListDisplays(cli.get(), modes);
        }
        if (!quality.empty())
        {
            SettingsPtr preset{Grapple_CreateSettings(), Grapple_DestroySettings};
            if (!preset || !Grapple_SettingsQuality(preset.get(), quality.c_str(), "CLI quality") ||
                !Grapple_SettingsOverlay(preset.get(), cli.get()))
                throw std::runtime_error(SDL_GetError());
            cli.swap(preset);
        }
        auto launch = ResolveLaunch(request, cli.get());
        if (request.print)
        {
            PrintSettings(launch.settings.get());
            std::cout << "Player configuration directory: " << launch.preferences.string() << '\n';
            return 0;
        }
        WorkingDirectory directory(launch.root);
        ApplyAudioSettings(launch.settings.get());
        LaunchSettings installed(launch.settings.get());
        std::vector<std::string> no_args{argv[0]};
        ProcessArgs process_args(no_args);
        auto script = launch.entry.string();
        return GrappleRunner_Execute(launch.language.c_str(), nullptr, script.c_str(),
                                     argc - game_at, argv + game_at);
    }
    catch (const std::exception &error)
    {
        std::cerr << "error: " << error.what() << '\n';
        return 2;
    }
}
