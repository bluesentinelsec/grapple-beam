#include "launch.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <grapple/audio_bus.h>
#include <grapple/bindings.h>
#include <iostream>
#include <physfs.h>
#include <stdexcept>
#include <toml.h>
namespace grapple::runner
{
namespace
{
void Check(bool success)
{
    if (!success)
        throw std::runtime_error(SDL_GetError());
}
bool Exists(const std::filesystem::path &path)
{
    std::error_code error;
    const bool exists = std::filesystem::exists(path, error);
    if (error)
        throw std::runtime_error(path.string() + ": " + error.message());
    return exists;
}
std::string Language(const std::filesystem::path &entry)
{
    if (entry.extension() == ".lua")
        return "lua";
    if (entry.extension() == ".rb")
        return "ruby";
    throw std::runtime_error("entrypoint must be a .lua or .rb script");
}
bool Identity(const std::string &text)
{
    return !text.empty() && text.size() <= 128 && text != "." && text != ".." &&
           text.find_first_not_of(
               "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-") ==
               std::string::npos;
}
std::string String(const toml_table_t *table, const char *key, const std::string &fallback)
{
    if (!toml_raw_in(table, key))
        return fallback;
    auto value = toml_string_in(table, key);
    if (!value.ok)
        throw std::runtime_error(std::string("manifest ") + key + " must be a string");
    std::string result = value.u.s;
    free(value.u.s);
    return result;
}
void Manifest(Launch &launch, std::string &org, std::string &id)
{
    auto path = launch.root / "grapple.toml";
    if (!Exists(path))
        return;
    if (std::filesystem::file_size(path) > 1024 * 1024)
        throw std::runtime_error("project manifest exceeds 1 MiB");
    std::ifstream input(path);
    std::string text((std::istreambuf_iterator<char>(input)), {});
    if (!input.eof() && input.fail())
        throw std::runtime_error("cannot read " + path.string());
    if (text.find('\0') != std::string::npos)
        throw std::runtime_error("project manifest contains NUL bytes");
    if (text.size() > 1024 * 1024)
        throw std::runtime_error("project manifest exceeds 1 MiB");
    char error[256];
    auto *raw = toml_parse(text.data(), error, sizeof(error));
    if (!raw)
        throw std::runtime_error(path.string() + ": " + error);
    std::unique_ptr<toml_table_t, decltype(&toml_free)> document(raw, toml_free);
    auto *game = toml_table_in(raw, "game");
    if (!game)
        throw std::runtime_error("grapple.toml requires a [game] table");
    for (int i = 0;; ++i)
    {
        const char *key = toml_key_in(raw, i);
        if (!key)
            break;
        if (std::string(key) != "game")
            throw std::runtime_error("unknown manifest table: " + std::string(key));
    }
    for (int i = 0;; ++i)
    {
        const char *key = toml_key_in(game, i);
        if (!key)
            break;
        if (std::string(key) != "id" && std::string(key) != "organization" &&
            std::string(key) != "entry" && std::string(key) != "language")
            throw std::runtime_error("unknown manifest key: " + std::string(key));
    }
    org = String(game, "organization", org);
    id = String(game, "id", id);
    std::string entry = String(game, "entry", "");
    std::string language = String(game, "language", "");
    if (!language.empty())
    {
        if (language != "lua" && language != "ruby")
            throw std::runtime_error("manifest language must be lua or ruby");
        if (!launch.language.empty() && launch.language != language)
            throw std::runtime_error("CLI language disagrees with manifest");
        launch.language = language;
    }
    if (!entry.empty() && launch.entry.empty())
    {
        std::filesystem::path relative(entry);
        if (relative.is_absolute())
            throw std::runtime_error("manifest entry must be relative to the project");
        launch.entry = std::filesystem::weakly_canonical(launch.root / relative);
        auto normalized = launch.entry.lexically_relative(launch.root);
        if (normalized.empty() || *normalized.begin() == "..")
            throw std::runtime_error("entrypoint must stay within the project");
    }
}
void MediaDefaults(Grapple_Settings *settings, const std::filesystem::path &root,
                   const Grapple_Settings *cli)
{
    if (std::string(Grapple_SettingsGet(cli, "auto-mount")) == "off")
        return;
    std::filesystem::path media = Grapple_SettingsGet(cli, "media");
    if (media.empty())
        for (const auto *candidate : {"media.zip", "media.dat", "media"})
            if (Exists(root / candidate))
            {
                media = root / candidate;
                break;
            }
    if (media.empty())
        return;
    if (std::filesystem::is_directory(media))
    {
        auto config = media / "config.toml";
        if (Exists(config))
            Check(Grapple_SettingsLoadFile(settings, config.string().c_str()));
        return;
    }
    if (!PHYSFS_isInit() && !PHYSFS_init(nullptr))
        throw std::runtime_error("cannot initialize media configuration reader");
    const auto filename = media.string();
    if (!PHYSFS_mount(filename.c_str(), "__grapple_launch_config", 0))
        throw std::runtime_error("cannot read media archive " + filename + ": " +
                                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    struct Mount
    {
        const std::string &path;
        ~Mount()
        {
            PHYSFS_unmount(path.c_str());
        }
    } mounted{filename};
    const char *entry = "__grapple_launch_config/config.toml";
    if (!PHYSFS_exists(entry))
        return;
    auto *file = PHYSFS_openRead(entry);
    if (!file)
        throw std::runtime_error("cannot open config.toml in " + filename);
    std::unique_ptr<PHYSFS_File, decltype(&PHYSFS_close)> owned(file, PHYSFS_close);
    auto length = PHYSFS_fileLength(file);
    if (length < 0 || length > 1024 * 1024)
        throw std::runtime_error("invalid configuration size in " + filename);
    std::string text(static_cast<std::size_t>(length), '\0');
    if (PHYSFS_readBytes(file, text.data(), static_cast<PHYSFS_uint64>(length)) != length ||
        text.find('\0') != std::string::npos)
        throw std::runtime_error("cannot read configuration in " + filename);
    const auto source = filename + "#config.toml";
    Check(Grapple_SettingsLoadToml(settings, text.c_str(), source.c_str()));
    if (std::string(Grapple_SettingsSource(settings, "media")) == source)
    {
        std::filesystem::path override_path = Grapple_SettingsGet(settings, "media");
        if (override_path.is_relative())
            Check(Grapple_SettingsSet(settings, "media",
                                      (media.parent_path() / override_path).string().c_str(),
                                      source.c_str()));
    }
}
void Reset(const std::filesystem::path &preferences)
{
    std::vector<std::filesystem::path> files;
    for (const auto *name : {"config.toml", "config.lua", "config.rb"})
        if (Exists(preferences / name))
        {
            if (!std::filesystem::is_regular_file(preferences / name) ||
                std::filesystem::is_symlink(preferences / name))
                throw std::runtime_error("refusing to reset a non-regular preferences file");
            files.emplace_back(name);
        }
    if (files.empty())
    {
        std::cout << "No player settings to reset\n";
        return;
    }
    auto backup =
        preferences / ("settings-backup-" +
                       std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    if (!std::filesystem::create_directory(backup))
        throw std::runtime_error("cannot create settings backup");
    std::size_t moved = 0;
    try
    {
        for (const auto &file : files)
        {
            std::filesystem::rename(preferences / file, backup / file);
            ++moved;
        }
    }
    catch (...)
    {
        for (std::size_t i = moved; i > 0; --i)
        {
            std::error_code error;
            std::filesystem::rename(backup / files[i - 1], preferences / files[i - 1], error);
        }
        throw std::runtime_error("settings reset failed; recoverable originals are in " +
                                 backup.string() + " or the preferences directory");
    }
    std::cout << "Reset " << files.size()
              << " player configuration files; backup: " << backup.string() << '\n';
}
void Script(Grapple_Settings *settings, const std::filesystem::path &path,
            const std::string &language)
{
    if (Language(path) != language)
        throw std::runtime_error("settings script language must match the game");
    const auto previous = std::filesystem::current_path();
    std::filesystem::current_path(path.parent_path());
    const bool ok = Grapple_LoadSettingsScript(settings, language.c_str(), path.string().c_str());
    const std::string error = ok ? "" : SDL_GetError();
    std::filesystem::current_path(previous);
    if (!ok)
        throw std::runtime_error(error);
}
} // namespace
Launch ResolveLaunch(const LaunchRequest &request, const Grapple_Settings *cli)
{
    Launch launch;
    launch.language = request.language;
    auto path = std::filesystem::weakly_canonical(
        request.project.empty() ? std::filesystem::current_path() : request.project);
    if (std::filesystem::is_directory(path))
        launch.root = path;
    else if (std::filesystem::is_regular_file(path))
    {
        launch.entry = path;
        launch.root = path.parent_path();
    }
    else
        throw std::runtime_error("project/script not found: " + path.string());
    std::uint64_t hash = 14695981039346656037ULL;
    for (char c : launch.root.generic_string())
    {
        hash ^= static_cast<unsigned char>(c);
        hash *= 1099511628211ULL;
    }
    std::string org = "grapple-beam", id = "project-" + std::to_string(hash);
    Manifest(launch, org, id);
    if (!Identity(org) || !Identity(id))
        throw std::runtime_error(
            "project id/organization must contain only letters, digits, '.', '_' or '-'");
    if (launch.entry.empty())
    {
        bool lua = Exists(launch.root / "main.lua"), ruby = Exists(launch.root / "main.rb");
        if (!launch.language.empty())
        {
            launch.entry = launch.root / (launch.language == "lua" ? "main.lua" : "main.rb");
        }
        else if (lua == ruby)
            throw std::runtime_error(lua ? "ambiguous project: declare entry in grapple.toml"
                                         : "no project entrypoint: expected main.lua or main.rb");
        else
            launch.entry = launch.root / (lua ? "main.lua" : "main.rb");
    }
    auto inferred = Language(launch.entry);
    if (!launch.language.empty() && launch.language != inferred)
        throw std::runtime_error("language must agree with entrypoint");
    launch.language = inferred;
    if (!std::filesystem::is_regular_file(launch.entry))
        throw std::runtime_error("entrypoint is not a regular file");
    char *pref = SDL_GetPrefPath(org.c_str(), id.c_str());
    if (!pref)
        throw std::runtime_error(SDL_GetError());
    launch.preferences = pref;
    SDL_free(pref);
    launch.settings.reset(Grapple_CreateSettings());
    if (!launch.settings)
        throw std::runtime_error(SDL_GetError());
    auto *settings = launch.settings.get();
    Check(Grapple_SettingsSetPlayerPath(settings,
                                        (launch.preferences / "config.toml").string().c_str()));
    const int recovery = static_cast<int>(request.safe) + static_cast<int>(request.defaults) +
                         static_cast<int>(request.reset);
    if (recovery > 1)
        throw std::runtime_error("recovery actions are mutually exclusive");
    if (recovery && (!request.configs.empty() || !request.scripts.empty()))
        throw std::runtime_error(
            "recovery actions cannot be combined with explicit configuration files");

    if (request.safe)
    {
        for (const auto &[key, value] :
             std::vector<std::pair<const char *, const char *>>{{"window-mode", "windowed"},
                                                                {"window-size", "1280x720"},
                                                                {"display", "primary"},
                                                                {"resizable", "on"},
                                                                {"backend", "software"},
                                                                {"effects", "off"},
                                                                {"max-fps", "60"},
                                                                {"vsync", "on"},
                                                                {"shadows", "off"},
                                                                {"lights", "low"},
                                                                {"particles", "low"}})
            Check(Grapple_SettingsSet(settings, key, value, "safe mode"));
    }
    else
    {
        const auto load = [&](const std::filesystem::path &root) {
            auto toml = root / "config.toml";
            if (Exists(toml))
                Check(Grapple_SettingsLoadFile(settings, toml.string().c_str()));
            auto script = root / (launch.language == "lua" ? "config.lua" : "config.rb");
            if (Exists(script))
                Script(settings, script, launch.language);
        };
        MediaDefaults(settings, launch.root, cli);
        load(launch.root);
        if (!request.defaults && !request.reset)
            load(launch.preferences);
        for (const auto &file : request.configs)
            Check(Grapple_SettingsLoadFile(settings,
                                           std::filesystem::absolute(file).string().c_str()));
        for (const auto &file : request.scripts)
            Script(settings, std::filesystem::absolute(file), launch.language);
    }
    Check(Grapple_SettingsOverlay(settings, cli));
    const auto get = [&](const char *key) {
        return std::string(Grapple_SettingsGet(settings, key));
    };
    if ((get("resolution") != "desktop" || get("refresh-rate") != "desktop") &&
        get("window-mode") != "fullscreen-exclusive")
        throw std::runtime_error("resolution/refresh-rate require fullscreen-exclusive");
    if (get("headless") == "on" && get("window-mode") != "windowed")
    {
        if (std::string(Grapple_SettingsSource(settings, "window-mode")) != "engine defaults")
            throw std::runtime_error("headless cannot request fullscreen");
        Check(Grapple_SettingsSet(settings, "window-mode", "windowed", "headless mode"));
    }
    if (get("auto-mount") == "off" && !get("media").empty())
        throw std::runtime_error("media cannot be combined with auto-mount off");
    if (request.reset && !request.print)
        Reset(launch.preferences);
    return launch;
}
void PrintSettings(const Grapple_Settings *settings)
{
    std::cout << "Requested startup settings (before game code and platform application):\n";
    for (int i = 0; i < Grapple_SettingCount(); ++i)
    {
        auto *key = Grapple_SettingKey(i);
        std::cout << key << " = " << Grapple_SettingsGet(settings, key) << "    # "
                  << Grapple_SettingsSource(settings, key) << '\n';
    }
}
void ApplyAudioSettings(const Grapple_Settings *settings)
{
    Grapple_SetAudioMuted(std::string(Grapple_SettingsGet(settings, "mute")) == "on");
    const char *keys[] = {"master-volume", "music-volume", "sfx-volume", "speech-volume",
                          "ambient-volume"};
    for (int i = 0; i < GRAPPLE_AUDIO_BUS_COUNT; ++i)
        Check(Grapple_SetAudioBusGain(
            static_cast<Grapple_AudioBus>(i),
            static_cast<float>(SDL_atof(Grapple_SettingsGet(settings, keys[i])))));
}
} // namespace grapple::runner
