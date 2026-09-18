#ifndef GRAPPLE_PROJECT_HPP
#define GRAPPLE_PROJECT_HPP
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace CLI
{
class App;
}

namespace grapple::project
{
struct ProcessResult
{
    int status = 0;
    std::string output;
};
using Command = std::vector<std::string>;
using RunCommand =
    std::function<ProcessResult(const Command &, const std::filesystem::path &, int)>;
using ReadUrl = std::function<std::string(const std::string &)>;
struct Services
{
    RunCommand run;
    ReadUrl get;
};
struct NewOptions
{
    std::filesystem::path destination;
    std::string model, version, commit, github;
    std::string license = "zlib";
    std::string visibility = "private";
    bool no_git = false;
};
struct EngineRef
{
    std::string requested, commit, license;
};
/** @brief Execute a child with separate argv entries and a bounded lifetime. */
ProcessResult Run(const Command &args, const std::filesystem::path &cwd, int timeout_seconds);
/** @brief Read a bounded, TLS-verified public HTTP response; throw on failure. */
std::string Get(const std::string &url);
/** @brief Resolve a release/commit and validate the supported C engine baseline. */
EngineRef Resolve(const NewOptions &options, const ReadUrl &get);
/** @brief Create an editable C project. External operations are injectable for tests. */
void Create(const NewOptions &options, const Services &services);
/** @brief Package a generated C desktop project with CMake and verify its payload. */
void Package(const std::filesystem::path &directory, const RunCommand &run);

/** @brief CLI11 subcommand state for new and package. */
struct Cli
{
    CLI::App *create = nullptr;
    CLI::App *package = nullptr;
    NewOptions options;
    std::string destination;
    std::string directory = ".";
};

/** @brief Register new and package on an existing CLI11 application.
 *  @param app Parent application that already requires a subcommand.
 *  @param cli Option storage and subcommand pointers; must outlive parse. */
void AddCommands(CLI::App &app, Cli &cli);
std::string Identifier(const std::string &name);
std::string Replace(std::string text, const std::map<std::string, std::string> &values);
void Write(const std::filesystem::path &path, const std::string &text);
std::string Read(const std::filesystem::path &path);
} // namespace grapple::project
#endif
