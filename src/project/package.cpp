#include "project.hpp"

#include <SDL3/SDL.h>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>

namespace grapple::project
{
namespace fs = std::filesystem;
void Package(const fs::path &directory, const RunCommand &run)
{
    const auto root = fs::canonical(directory);
    const auto manifest = nlohmann::json::parse(Read(root / "grapple-project.json"));
    if (manifest.at("schema") != 1 || manifest.at("model") != "c")
        throw std::runtime_error("package requires a generated schema-1 C desktop project");
    const auto id = manifest.at("id").get<std::string>();
    if (!std::regex_match(id, std::regex("[a-z_][a-z0-9_]*")))
        throw std::runtime_error("Invalid project identifier");
    auto version = Read(root / "VERSION");
    while (!version.empty() && (version.back() == '\n' || version.back() == '\r'))
        version.pop_back();
    if (!std::regex_match(version, std::regex("[0-9]+\\.[0-9]+\\.[0-9]+")))
        throw std::runtime_error("VERSION must contain MAJOR.MINOR.PATCH");
    const std::string platform =
#if defined(__APPLE__)
        "macos";
#elif defined(_WIN32)
        "windows";
#else
        "linux";
#endif
    const std::string arch =
#if defined(__aarch64__) || defined(_M_ARM64)
        "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
        "x86_64";
#else
        "unknown";
#endif
    auto checked = [&](const Command &args, const fs::path &cwd, int seconds) {
        std::cout << "Running " << args.front() << "..." << std::endl;
        auto result = run(args, cwd, seconds);
        if (result.status != 0)
            throw std::runtime_error(args.front() + " failed:\n" + result.output);
        return result.output;
    };
    const auto build = root / "build/release";
    checked({"cmake", "-S", root.string(), "-B", build.string(), "-DCMAKE_BUILD_TYPE=Release"},
            root, 900);
    checked({"cmake", "--build", build.string(), "--config", "Release", "--parallel", "4"}, root,
            1800);
    checked({"ctest", "--test-dir", build.string(), "-C", "Release", "--output-on-failure",
             "--timeout", "30"},
            root, 300);
    const auto stem = id + "-" + version + "-" + platform + "-" + arch;
    const auto scratch =
        fs::temp_directory_path() /
        ("grapple-package-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (!fs::create_directory(scratch))
        throw std::runtime_error("Cannot reserve package staging directory");
    struct Cleanup
    {
        fs::path path;
        ~Cleanup()
        {
            std::error_code error;
            fs::remove_all(path, error);
        }
    } cleanup{scratch};
    const auto payload = scratch / stem;
    checked({"cmake", "--install", build.string(), "--config", "Release", "--component", "Game",
             "--prefix", payload.string()},
            root, 120);
    const auto executable =
#if defined(__APPLE__)
        payload / (id + ".app") / "Contents/MacOS" / id;
#elif defined(_WIN32)
        payload / (id + ".exe");
#else
        payload / id;
#endif
    if (!fs::is_regular_file(executable))
        throw std::runtime_error("CMake install did not produce the expected game executable");
    const auto symbols = scratch / (stem + "-symbols");
    fs::create_directory(symbols);
#if defined(__APPLE__)
    checked({"xcrun", "dsymutil", executable.string(), "-o", (symbols / (id + ".dSYM")).string()},
            scratch, 120);
#elif defined(_WIN32)
    const auto pdb = build / "bin" / (id + ".pdb");
    if (!fs::is_regular_file(pdb))
        throw std::runtime_error("Release build did not produce the expected PDB symbols");
    fs::copy_file(pdb, symbols / pdb.filename());
#else
    checked(
        {"objcopy", "--only-keep-debug", executable.string(), (symbols / (id + ".debug")).string()},
        scratch, 120);
    checked({"strip", "--strip-debug", executable.string()}, scratch, 120);
    checked({"objcopy", "--add-gnu-debuglink=" + (symbols / (id + ".debug")).string(),
             executable.string()},
            scratch, 120);
#endif
    const auto smoke = checked({executable.string(), "--self-test"}, scratch, 30);
    if (smoke.find("result=pass") == std::string::npos)
        throw std::runtime_error(
            "Packaged game did not report successful asset/render verification");
    checked(
        {"cmake", "-E", "tar", "cf", (scratch / (stem + ".zip")).string(), "--format=zip", stem},
        scratch, 120);
    const auto dist = root / "dist";
    if (fs::is_symlink(dist))
        throw std::runtime_error("dist must not be a symlink");
    fs::create_directories(dist);
    const auto archive = dist / (stem + ".zip");
    if (fs::is_symlink(archive))
        throw std::runtime_error("Package archive must not be a symlink");
    fs::copy_file(scratch / (stem + ".zip"), archive, fs::copy_options::overwrite_existing);
    Write(dist / (stem + ".zip.sha256"),
          checked({"cmake", "-E", "sha256sum", archive.filename().string()}, dist, 30));
    const auto symbol_archive = dist / (stem + "-symbols.zip");
    if (fs::is_symlink(symbol_archive))
        throw std::runtime_error("Symbol archive must not be a symlink");
    checked({"cmake", "-E", "tar", "cf", (scratch / symbol_archive.filename()).string(),
             "--format=zip", symbols.filename().string()},
            scratch, 120);
    fs::copy_file(scratch / symbol_archive.filename(), symbol_archive,
                  fs::copy_options::overwrite_existing);
    Write(dist / (symbol_archive.filename().string() + ".sha256"),
          checked({"cmake", "-E", "sha256sum", symbol_archive.filename().string()}, dist, 30));
    std::cout << "Verified desktop package: " << archive.string()
              << "\nUnsigned; see docs/developing/packaging.md before distribution.\n";
}
} // namespace grapple::project
