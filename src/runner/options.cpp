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
struct Setting
{
    const char *name;
    const char *values;
    const char *description;
};

constexpr Setting kChoices[] = {
    {"window-mode", "windowed|fullscreen-exclusive|fullscreen-borderless",
     "Window presentation mode"},
    {"vsync", "on|off", "Synchronize presentation with display refresh"},
    {"presentation", "letterbox|expand|overscan|integer|stretch|native",
     "Logical canvas presentation"},
    {"filter", "auto|nearest|linear", "Texture filtering"},
    {"particles", "off|low|medium|high", "Particle budget (consumed by game)"},
    {"lights", "off|low|medium|high", "Dynamic light budget (consumed by game)"},
    {"shadows", "off|low|medium|high", "Shadow budget (consumed by game)"},
    {"antialias", "off|fxaa", "Post-process anti-aliasing"},
    {"color-blind", "none|protanopia|deuteranopia|tritanopia", "Color-vision correction"},
    {"reduced-flashing", "on|off", "Constrain engine-controlled flashing effects"},
    {"backend", "auto|directx11|directx12|vulkan|metal|opengl|opengles2|software",
     "Concrete renderer selection"},
};

struct NumericSetting
{
    const char *name;
    double minimum;
    double maximum;
    bool integer;
};

constexpr NumericSetting kNumbers[] = {
    {"display", 0, 65535, true},   {"render-scale", 0.25, 2, false},
    {"bloom", 0, 1, false},        {"bloom-threshold", 0, 1, false},
    {"crt", 0, 1, false},          {"crt-curvature", 0, 1, false},
    {"pixelation", 1, 64, true},   {"chromatic-aberration", 0, 1, false},
    {"brightness", 0.5, 2, false}, {"contrast", 0.5, 2, false},
    {"saturation", 0, 2, false},   {"screen-shake", 0, 1, false},
    {"ui-scale", 0.5, 3, false},
};

std::string ValidateNumber(const std::string &value, double minimum, double maximum, bool integer)
{
    if (value.empty() || value.find_first_of(" \t\n\r") != std::string::npos)
        return "expected a number";
    if (integer && value.find_first_not_of("0123456789") != std::string::npos)
        return "expected an unsigned decimal integer";
    char *end = nullptr;
    errno = 0;
    const double number = std::strtod(value.c_str(), &end);
    if (end == value.c_str() || *end != '\0' || errno == ERANGE || !std::isfinite(number))
        return "expected a finite number";
    if (number < minimum || number > maximum)
        return "value is outside the documented range";
    return {};
}

std::vector<std::string> SplitChoices(const std::string &text)
{
    std::vector<std::string> result;
    std::size_t start = 0;
    for (;;)
    {
        const auto end = text.find('|', start);
        result.push_back(text.substr(start, end - start));
        if (end == std::string::npos)
            return result;
        start = end + 1;
    }
}

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
        return "use --default-settings (configuration recovery is pending)";
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

int GrappleRunner_Run(int argc, char **argv, const char *version)
{
    CLI::App app{"Run a Lua or Ruby game. Engine options precede the script; trailing arguments "
                 "belong to the game.",
                 "grapple-beam"};
    app.set_version_flag("-V,--version", std::string("grapple-beam ") + version);
    app.set_help_all_flag("--help-all", "Show all implemented options");
    std::string language;
    app.add_option("-l,--language", language, "Language; inferred from .lua or .rb")
        ->check(CLI::IsMember({"lua", "ruby"}))
        ->take_last();

    const bool repl = argc > 1 && std::string(argv[1]) == "repl";
    const bool eval = argc > 1 && std::string(argv[1]) == "eval";
    const bool developer = repl || eval;
    std::string code;
    bool list_backends = false;
    bool safe_mode = false;
    std::vector<std::string> engine_args{argv[0]};
    if (developer)
    {
        app.description(repl ? "Interactive Lua/Ruby shell" : "Evaluate Lua/Ruby source");
        app.get_option("--language")->required();
        if (eval)
            app.add_option("--code", code, "Source to evaluate")->required();
    }
    else
    {
        app.add_flag("--list-backends", list_backends,
                     "Probe concrete renderers and report API/device versions");
        app.add_flag("--safe-mode", safe_mode,
                     "Use conservative engine settings; do not save them");
        const auto add = [&](const std::string &name, const std::string &description) {
            return app
                .add_option_function<std::string>(
                    "--" + name,
                    [&, name](const std::string &value) {
                        engine_args.push_back("--" + name + "=" + value);
                    },
                    description)
                ->expected(1)
                ->trigger_on_parse();
        };
        for (const auto &setting : kChoices)
            add(setting.name, setting.description)
                ->check(CLI::IsMember(SplitChoices(setting.values)))
                ->type_name(setting.values);
        for (const auto &setting : kNumbers)
        {
            const std::string range =
                std::to_string(setting.minimum) + ".." + std::to_string(setting.maximum);
            add(setting.name, "Range " + range)
                ->check(CLI::Validator(
                    [setting](std::string &value) {
                        return ValidateNumber(value, setting.minimum, setting.maximum,
                                              setting.integer);
                    },
                    range));
        }
        add("max-fps", "Frame limiter: display, unlimited, or integer 10..1000")
            ->check(CLI::Validator(
                [](std::string &value) {
                    if (value == "display" || value == "unlimited")
                        return std::string{};
                    return ValidateNumber(value, 10, 1000, true);
                },
                "display|unlimited|10..1000"));
        add("window-size", "Windowed dimensions; does not change window mode")
            ->check(CLI::Validator(
                [](std::string &value) {
                    const auto x = value.find('x');
                    if (x == std::string::npos)
                        return std::string("expected WIDTHxHEIGHT");
                    auto error = ValidateNumber(value.substr(0, x), 320, 16384, true);
                    return error.empty() ? ValidateNumber(value.substr(x + 1), 240, 16384, true)
                                         : error;
                },
                "WIDTHxHEIGHT"));
        add("media", "Advanced asset directory/archive override")->check(CLI::ExistingPath);
    }

    std::vector<char *> option_args{argv[0]};
    const char *script = nullptr;
    int game_at = argc;
    for (int i = developer ? 2 : 1; i < argc; ++i)
    {
        const std::string token = argv[i];
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
            script = argv[i];
            game_at = i + 1;
            if (game_at < argc && std::string(argv[game_at]) == "--")
                ++game_at;
            break;
        }
        const std::string key = token.substr(0, token.find('='));
        const auto *option = app.get_option_no_throw(key);
        if (option == nullptr)
        {
            const auto replacement = Replacement(key);
            std::cerr << "error: " << key << ": "
                      << (replacement.empty() ? "unknown or not yet implemented option; see --help"
                                              : replacement)
                      << '\n';
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
        const int result = app.exit(error);
        return result == 0 ? 0 : 2;
    }
    if (list_backends)
    {
        if (script != nullptr || engine_args.size() != 1 || safe_mode)
        {
            std::cerr << "error: --list-backends does not take a game or launch settings\n";
            return 2;
        }
        return ListBackends();
    }
    if (!developer && script == nullptr)
    {
        std::cerr << "error: a .lua or .rb script is required; project discovery is not "
                     "implemented yet\n";
        return 2;
    }
    if (script != nullptr)
    {
        const auto extension = std::filesystem::path(script).extension().string();
        const std::string inferred = extension == ".lua" ? "lua" : extension == ".rb" ? "ruby" : "";
        if (inferred.empty() || (!language.empty() && language != inferred))
        {
            std::cerr << "error: language must agree with a .lua or .rb entrypoint\n";
            return 2;
        }
        language = inferred;
        std::error_code error;
        if (!std::filesystem::is_regular_file(script, error))
        {
            std::cerr << "error: script is not a readable regular file: " << script << '\n';
            return 2;
        }
    }
    if (safe_mode)
        engine_args.insert(engine_args.begin() + 1, "--with-safe-mode");
    ProcessArgs process_args(engine_args);
    return GrappleRunner_Execute(language.c_str(), eval ? code.c_str() : nullptr, script,
                                 argc - game_at, argv + game_at);
}
