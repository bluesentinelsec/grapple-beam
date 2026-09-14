#include "project.hpp"
#include "templates.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>

namespace grapple::project
{
namespace fs = std::filesystem;
namespace
{
const std::string kApi = "https://api.github.com/repos/bluesentinelsec/grapple-beam/";
std::string Check(const RunCommand &run, const Command &args, const fs::path &cwd = {})
{
    auto result = run(args, cwd, 120);
    if (result.status != 0)
        throw std::runtime_error(args.front() + " failed: " + result.output);
    return result.output;
}
class Stage
{
  public:
    explicit Stage(const fs::path &parent)
    {
        const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        for (int i = 0; i < 100; ++i)
        {
            path = parent / (".grapple-new-" + std::to_string(seed) + "-" + std::to_string(i));
            if (fs::create_directory(path))
                return;
        }
        throw std::runtime_error("Cannot reserve a staging directory");
    }
    ~Stage()
    {
        std::error_code error;
        fs::remove_all(path, error);
    }
    fs::path path;
};
} // namespace
std::string Identifier(const std::string &name)
{
    std::string result;
    for (char raw : name)
    {
        auto ch = static_cast<unsigned char>(raw);
        if (ch >= 'A' && ch <= 'Z')
            ch = static_cast<unsigned char>(ch + ('a' - 'A'));
        result +=
            ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) ? static_cast<char>(ch) : '_';
    }
    if (result.empty() || result.find_first_not_of('_') == std::string::npos)
        throw std::runtime_error(
            "Destination must contain an ASCII letter or digit for the game identifier");
    if (result.front() >= '0' && result.front() <= '9')
        result = "game_" + result;
    if (std::regex_match(result, std::regex("(con|prn|aux|nul|com[0-9]|lpt[0-9])")))
        result = "game_" + result;
    return result;
}
std::string Replace(std::string text, const std::map<std::string, std::string> &values)
{
    // Scan the original template once; inserted text is never interpreted as another token.
    std::string result;
    size_t pos = 0;
    while (pos < text.size())
    {
        size_t found = text.find('@', pos);
        if (found == std::string::npos)
        {
            result += text.substr(pos);
            break;
        }
        result += text.substr(pos, found - pos);
        const auto end = text.find('@', found + 1);
        if (end != std::string::npos)
        {
            const auto it = values.find(text.substr(found + 1, end - found - 1));
            if (it != values.end())
            {
                result += it->second;
                pos = end + 1;
                continue;
            }
        }
        result += '@';
        pos = found + 1;
    }
    return result;
}
void Write(const fs::path &path, const std::string &text)
{
    fs::create_directories(path.parent_path());
    std::ofstream stream(path, std::ios::binary);
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
    stream.close();
}
std::string Read(const fs::path &path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
        throw std::runtime_error("Cannot read " + path.string());
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
EngineRef Resolve(const NewOptions &options, const ReadUrl &get)
{
    if (!options.commit.empty() && !std::regex_match(options.commit, std::regex("[a-fA-F0-9]{40}")))
        throw std::runtime_error("--engine-commit requires a full 40-character commit SHA");
    if (!options.version.empty() &&
        !std::regex_match(options.version, std::regex("[A-Za-z0-9][A-Za-z0-9._-]{0,100}")))
        throw std::runtime_error("--engine-version requires an exact release tag");
    EngineRef ref;
    ref.requested = options.commit.empty() ? options.version : options.commit;
    if (ref.requested.empty())
    {
        const auto latest = nlohmann::json::parse(get(kApi + "releases/latest"));
        if (latest.value("prerelease", true) || latest.value("draft", true))
            throw std::runtime_error("GitHub did not return a stable published release");
        ref.requested = latest.at("tag_name").get<std::string>();
        if (!std::regex_match(ref.requested, std::regex("[A-Za-z0-9][A-Za-z0-9._-]{0,100}")))
            throw std::runtime_error("Invalid release tag returned by GitHub");
    }
    const auto commit = nlohmann::json::parse(get(kApi + "commits/" + ref.requested));
    ref.commit = commit.at("sha").get<std::string>();
    if (!std::regex_match(ref.commit, std::regex("[a-f0-9]{40}")))
        throw std::runtime_error("Invalid commit returned by GitHub");
    if (!options.commit.empty())
    {
        std::string expected = options.commit;
        std::transform(expected.begin(), expected.end(), expected.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (ref.commit != expected)
            throw std::runtime_error("Resolved commit differs from the requested commit");
    }
    const std::string raw =
        "https://raw.githubusercontent.com/bluesentinelsec/grapple-beam/" + ref.commit + "/";
    const auto version = get(raw + "VERSION");
    std::smatch parts;
    if (!std::regex_search(version, parts,
                           std::regex("^([0-9]{1,6})\\.([0-9]{1,6})\\.([0-9]{1,6})")) ||
        (std::stoul(parts[1].str()) == 0 && std::stoul(parts[2].str()) < 9))
        throw std::runtime_error(
            "Unsupported engine version; the C template requires v0.9.0 or a compatible later ref");
    const auto header = get(raw + "engine/include/grapple/engine.h");
    const auto graphics = get(raw + "engine/include/grapple/engine_graphics.h");
    for (const auto *symbol : {"Grapple_RunGame", "Grapple_EngineConfig", "media_path"})
        if (header.find(symbol) == std::string::npos)
            throw std::runtime_error("Unsupported engine ref: missing C API " +
                                     std::string(symbol) +
                                     "; use v0.9.0 or a compatible later ref");
    if (graphics.find("Grapple_GraphicsLoadTomlFile") == std::string::npos)
        throw std::runtime_error("Unsupported engine ref: missing graphics configuration API");
    ref.license = get(raw + "LICENSE");
    return ref;
}
void Create(const NewOptions &options, const Services &services)
{
    if (options.model != "c" || options.destination.empty())
        throw std::runtime_error("new requires a destination and --model=c");
    if ((!options.version.empty() && !options.commit.empty()) ||
        (options.no_git && !options.github.empty()))
        throw std::runtime_error("Conflicting creation options");
    if (options.license != "zlib" && options.license != "MIT")
        throw std::runtime_error("Supported licenses: zlib, MIT");
    if (options.visibility != "public" && options.visibility != "private")
        throw std::runtime_error("Supported visibility: private, public");
    const auto input_path = fs::absolute(options.destination).lexically_normal();
    if (fs::is_symlink(input_path))
        throw std::runtime_error("Destination must not be a symlink");
    const auto destination = fs::weakly_canonical(input_path);
    if (fs::is_symlink(destination) ||
        (fs::exists(destination) && (!fs::is_directory(destination) || !fs::is_empty(destination))))
        throw std::runtime_error("Destination must be a new or empty directory: " +
                                 destination.string());
    const auto id = Identifier(destination.filename().string());
    if (!options.no_git)
    {
        Check(services.run, {"git", "--version"});
        for (auto path = destination.parent_path(); !path.empty(); path = path.parent_path())
        {
            if (fs::exists(path / ".git"))
                throw std::runtime_error("Destination is inside a Git repository; choose another "
                                         "directory or use --no-git");
            if (path == path.root_path())
                break;
        }
    }
    if (!options.github.empty())
    {
        if (!std::regex_match(options.github,
                              std::regex("[A-Za-z0-9][A-Za-z0-9-]*/[A-Za-z0-9][A-Za-z0-9._-]*")))
            throw std::runtime_error("--github requires OWNER/REPO");
        auto auth = services.run({"gh", "auth", "status"}, {}, 30);
        if (auth.status != 0)
            throw std::runtime_error("GitHub authentication required; run gh auth login");
        Check(services.run, {"git", "var", "GIT_AUTHOR_IDENT"});
        auto existing =
            services.run({"gh", "repo", "view", options.github, "--json", "name"}, {}, 30);
        if (existing.status == 0)
            throw std::runtime_error("GitHub repository already exists: " + options.github);
    }
    const auto ref = Resolve(options, services.get);
    fs::create_directories(destination.parent_path());
    Stage stage(destination.parent_path());
    const std::map<std::string, std::string> values{{"GAME_ID", id},
                                                    {"ENGINE_COMMIT", ref.commit},
                                                    {"ENGINE_REF", ref.requested},
                                                    {"CLI_COMMIT", GRAPPLE_PROJECT_CLI_COMMIT},
                                                    {"LICENSE_NAME", options.license}};
    for (const auto &file : kTemplates)
        Write(stage.path / Replace(file.path, values), Replace(file.text, values));
    Write(stage.path / "LICENSE", options.license == "MIT" ? kMitLicense : kZlibLicense);
    Write(stage.path / "deps/licenses/grapple-beam.txt", ref.license);
    for (const auto *directory :
         {"sprites", "textures", "tiles", "ui", "fonts", "audio/music", "audio/sfx", "audio/speech",
          "audio/ambient", "maps", "lang", "data", "shaders"})
        Write(stage.path / "media" / directory / ".gitkeep", "");
    if (!options.no_git)
        Check(services.run, {"git", "init", "--initial-branch=main"}, stage.path);
    // Recheck immediately before publishing, so no existing project is overwritten.
    if (fs::exists(destination))
    {
        if (!fs::is_directory(destination) || !fs::is_empty(destination))
            throw std::runtime_error(
                "Destination changed during generation; no project was overwritten");
        fs::remove(destination);
    }
    fs::rename(stage.path, destination);
    std::cout << "Created " << destination.string() << " (C desktop)\nEngine: " << ref.requested
              << " @ " << ref.commit
              << "\nFirst build downloads and compiles source dependencies.\nNext: cd into the "
                 "project, then make (Windows: build.bat).\n";
    if (!options.github.empty())
    {
        try
        {
            Check(services.run, {"git", "add", "--all"}, destination);
            Check(services.run, {"git", "commit", "-m", "Create C desktop game project"},
                  destination);
            Check(services.run,
                  {"gh", "repo", "create", options.github, "--" + options.visibility, "--source",
                   ".", "--remote", "origin", "--push"},
                  destination);
            std::cout << "Created and pushed https://github.com/" << options.github
                      << "\nPages: select GitHub Actions in repository Settings > Pages if "
                         "available for your account.\n";
        }
        catch (const std::exception &error)
        {
            throw std::runtime_error(
                std::string(error.what()) + "\nLocal project preserved at " + destination.string() +
                ". Inspect git status and git remote -v; if origin exists, retry git push -u "
                "origin main. Otherwise inspect gh repo view " +
                options.github +
                " before retrying repository creation. No remote repository was deleted.");
        }
    }
    else
        std::cout << (options.no_git
                          ? "Git initialization skipped.\n"
                          : "Git initialized on main; review and commit the scaffold.\n");
}
} // namespace grapple::project
