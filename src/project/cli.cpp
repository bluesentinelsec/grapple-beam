#include "project.hpp"

#include <CLI/CLI.hpp>

namespace grapple::project
{
void AddCommands(CLI::App &app, Cli &cli)
{
    cli.create = app.add_subcommand("new", "Create an editable desktop game project");
    cli.create->add_option("destination", cli.destination)->required();
    cli.create->add_option("--model", cli.options.model)->required()->check(CLI::IsMember({"c"}));
    auto *version = cli.create->add_option("--engine-version", cli.options.version,
                                           "Exact release tag; default latest stable");
    cli.create->add_option("--engine-commit", cli.options.commit, "Full engine commit SHA")
        ->excludes(version);
    auto *github = cli.create->add_option(
        "--github", cli.options.github, "Create OWNER/REPO with authenticated gh, commit and push");
    cli.create->add_flag("--no-git", cli.options.no_git, "Skip Git initialization")
        ->excludes(github);
    cli.create->add_option("--license", cli.options.license)
        ->check(CLI::IsMember({"zlib", "MIT"}))
        ->default_str("zlib");
    cli.create->add_option("--visibility", cli.options.visibility)
        ->check(CLI::IsMember({"private", "public"}))
        ->needs(github);
    cli.package =
        app.add_subcommand("package", "Build, stage, verify and archive a C desktop game");
    cli.package->add_option("directory", cli.directory)->default_str(".");
}
} // namespace grapple::project
