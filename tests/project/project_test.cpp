#include "project.hpp"

#include <SDL3/SDL.h>
#include <chrono>
#include <gtest/gtest.h>
#include <stdexcept>

namespace
{
namespace fs = std::filesystem;
using namespace grapple::project;
const std::string kSha(40, 'a');
std::string Source(const std::string &url)
{
    if (url.ends_with("releases/latest"))
        return R"({"draft":false,"prerelease":false,"tag_name":"v0.9.0"})";
    if (url.find("commits/") != std::string::npos)
        return "{\"sha\":\"" + kSha + "\"}";
    if (url.ends_with("engine.h"))
        return "Grapple_RunGame Grapple_EngineConfig media_path";
    if (url.ends_with("engine_graphics.h"))
        return "Grapple_GraphicsLoadTomlFile";
    if (url.ends_with("LICENSE"))
        return "Engine license text\n";
    if (url.ends_with("VERSION"))
        return "0.9.0\n";
    throw std::runtime_error("Unexpected URL " + url);
}
class ProjectTest : public testing::Test
{
  protected:
    void SetUp() override
    {
        root = fs::temp_directory_path() /
               ("grapple-project-test-" + std::to_string(SDL_GetPerformanceCounter()));
        ASSERT_TRUE(fs::create_directory(root));
        options.destination = root / "Space Game";
        options.model = "c";
        services.get = Source;
        services.run = [&](const Command &args, const fs::path &, int) {
            commands.push_back(args);
            if (args.size() > 2 && args[0] == "gh" && args[1] == "repo" && args[2] == "view")
                return ProcessResult{1, "not found"};
            return ProcessResult{};
        };
    }
    void TearDown() override
    {
        std::error_code error;
        fs::remove_all(root, error);
    }
    fs::path root;
    NewOptions options;
    Services services;
    std::vector<Command> commands;
};
TEST_F(ProjectTest, LatestIsResolvedOnceAndStoredAsImmutableCommit)
{
    std::vector<std::string> urls;
    services.get = [&](const std::string &url) {
        urls.push_back(url);
        return Source(url);
    };
    Create(options, services);
    EXPECT_EQ(urls.size(), 6U);
    const auto pin = Read(options.destination / "cmake/GrappleDependency.cmake");
    EXPECT_NE(pin.find(kSha), std::string::npos);
    EXPECT_EQ(pin.find("GIT_TAG main"), std::string::npos);
    EXPECT_TRUE(fs::exists(options.destination / "src/space_game/game.c"));
    EXPECT_TRUE(fs::exists(options.destination / "include/space_game/game.h"));
    EXPECT_TRUE(fs::exists(options.destination / "media/audio/music/.gitkeep"));
    EXPECT_NE(
        Read(options.destination / "Makefile").find(".PHONY: build build-release test fmt clean"),
        std::string::npos);
    EXPECT_EQ(commands.back(), (Command{"git", "init", "--initial-branch=main"}));
}
TEST_F(ProjectTest, ExplicitVersionDoesNotQueryLatest)
{
    options.version = "v0.9.0";
    services.get = [](const std::string &url) {
        EXPECT_EQ(url.find("releases/latest"), std::string::npos);
        return Source(url);
    };
    Create(options, services);
}
TEST_F(ProjectTest, FullCommitMustMatchServerResponse)
{
    options.commit = std::string(40, 'b');
    EXPECT_THROW(Create(options, services), std::runtime_error);
    EXPECT_FALSE(fs::exists(options.destination));
}
TEST_F(ProjectTest, NetworkFailureLeavesNoProject)
{
    services.get = [](const std::string &) -> std::string { throw std::runtime_error("offline"); };
    EXPECT_THROW(Create(options, services), std::runtime_error);
    EXPECT_FALSE(fs::exists(options.destination));
    EXPECT_TRUE(fs::is_empty(root));
}
TEST_F(ProjectTest, ExistingContentIsNeverOverwritten)
{
    Write(options.destination / "precious.c", "keep");
    EXPECT_THROW(Create(options, services), std::runtime_error);
    EXPECT_EQ(Read(options.destination / "precious.c"), "keep");
    EXPECT_TRUE(commands.empty());
}
TEST_F(ProjectTest, EmptyDestinationIsAccepted)
{
    fs::create_directory(options.destination);
    Create(options, services);
    EXPECT_TRUE(fs::exists(options.destination / "README.md"));
}
TEST_F(ProjectTest, ParentRepositoryRequiresExplicitOptOut)
{
    fs::create_directory(root / ".git");
    EXPECT_THROW(Create(options, services), std::runtime_error);
    options.no_git = true;
    commands.clear();
    Create(options, services);
    EXPECT_TRUE(commands.empty());
}
TEST_F(ProjectTest, GitInitFailureDoesNotPublishPartialScaffold)
{
    services.run = [](const Command &args, const fs::path &, int) {
        return ProcessResult{args.size() > 1 && args[1] == "init" ? 1 : 0, "git failure"};
    };
    EXPECT_THROW(Create(options, services), std::runtime_error);
    EXPECT_TRUE(fs::is_empty(root));
}
TEST_F(ProjectTest, GithubFailurePreservesLocalProjectAndReportsRecovery)
{
    options.github = "owner/game";
    services.run = [&](const Command &args, const fs::path &, int) {
        commands.push_back(args);
        return ProcessResult{args[0] == "gh" && args[1] == "repo" ? 1 : 0, "service unavailable"};
    };
    try
    {
        Create(options, services);
        FAIL();
    }
    catch (const std::runtime_error &error)
    {
        EXPECT_NE(std::string(error.what()).find("Local project preserved"), std::string::npos);
    }
    EXPECT_TRUE(fs::exists(options.destination / "README.md"));
    EXPECT_EQ(commands.back(), (Command{"gh", "repo", "create", "owner/game", "--private",
                                        "--source", ".", "--remote", "origin", "--push"}));
}
TEST_F(ProjectTest, UnauthenticatedGithubFailsBeforeNetworkOrFilesystemChanges)
{
    options.github = "owner/game";
    services.run = [](const Command &args, const fs::path &, int) {
        return ProcessResult{args[0] == "gh" ? 1 : 0, "secret must not be printed"};
    };
    try
    {
        Create(options, services);
        FAIL();
    }
    catch (const std::runtime_error &error)
    {
        EXPECT_NE(std::string(error.what()).find("gh auth login"), std::string::npos);
        EXPECT_EQ(std::string(error.what()).find("secret"), std::string::npos);
    }
    EXPECT_TRUE(fs::is_empty(root));
}
TEST(ProjectUtilities, ReplacementDoesNotInterpretInjectedTokens)
{
    EXPECT_EQ(Replace("@A@ @B@", {{"A", "@B@"}, {"B", "ok"}}), "@B@ ok");
    EXPECT_EQ(Identifier("A Space-Game"), "a_space_game");
    EXPECT_EQ(Identifier("CON"), "game_con");
}
TEST(ProjectProcess, PreservesArgumentBoundariesAndWorkingDirectory)
{
    auto result = grapple::project::Run(
        {PROJECT_TEST_CMAKE, "-E", "echo", "hello world", "$(not-a-shell)", "`literal`"},
        fs::temp_directory_path(), 10);
    EXPECT_EQ(result.status, 0);
    EXPECT_NE(result.output.find("hello world $(not-a-shell) `literal`"), std::string::npos);
}
TEST(ProjectProcess, TimeoutTerminatesTheChild)
{
    const auto before = std::chrono::steady_clock::now();
    EXPECT_THROW(grapple::project::Run((Command{PROJECT_TEST_CMAKE, "-E", "sleep", "30"}), {}, 1),
                 std::runtime_error);
    EXPECT_LT(std::chrono::steady_clock::now() - before, std::chrono::seconds(10));
}

#ifndef _WIN32
TEST_F(ProjectTest, CompletedParentCannotLeaveBackgroundChildren)
{
    const auto marker = root / "unexpected-background-write";
    // All shell text is fixed; paths are passed as separate positional arguments.
    const auto result = grapple::project::Run(
        {"sh", "-c", "(sleep 2; touch \"$1\") &", "project-test", marker.string()}, root, 10);
    EXPECT_EQ(result.status, 0);
    SDL_Delay(2500);
    EXPECT_FALSE(fs::exists(marker));
}
#endif
TEST_F(ProjectTest, UnsupportedReleaseFailsBeforeWriting)
{
    services.get = [](const std::string &url) {
        return url.ends_with("VERSION") ? std::string("0.8.9\n") : Source(url);
    };
    EXPECT_THROW(Create(options, services), std::runtime_error);
    EXPECT_TRUE(fs::is_empty(root));
}
TEST_F(ProjectTest, ExplicitCommitAndMitWithoutGit)
{
    options.commit = kSha;
    options.license = "MIT";
    options.no_git = true;
    Create(options, services);
    EXPECT_TRUE(commands.empty());
    EXPECT_NE(Read(options.destination / "LICENSE").find("MIT License"), std::string::npos);
}
} // namespace
