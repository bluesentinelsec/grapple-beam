#include "project.hpp"

#include <CLI/CLI.hpp>
#include <iostream>

namespace grapple::project
{
int RunCli(int argc, char **argv)
{
    CLI::App app{"Create and package C desktop games", "grapple-beam"};
    app.require_subcommand(1);
    NewOptions options;
    std::string destination, directory = ".";
    auto *create = app.add_subcommand("new", "Create an editable desktop game project");
    create->add_option("destination", destination)->required();
    create->add_option("--model", options.model)->required()->check(CLI::IsMember({"c"}));
    auto *version = create->add_option("--engine-version", options.version,
                                       "Exact release tag; default latest stable");
    create->add_option("--engine-commit", options.commit, "Full engine commit SHA")
        ->excludes(version);
    auto *github = create->add_option("--github", options.github,
                                      "Create OWNER/REPO with authenticated gh, commit and push");
    create->add_flag("--no-git", options.no_git, "Skip Git initialization")->excludes(github);
    create->add_option("--license", options.license)
        ->check(CLI::IsMember({"zlib", "MIT"}))
        ->default_str("zlib");
    create->add_option("--visibility", options.visibility)
        ->check(CLI::IsMember({"private", "public"}))
        ->needs(github);
    auto *package =
        app.add_subcommand("package", "Build, stage, verify and archive a C desktop game");
    package->add_option("directory", directory)->default_str(".");
    try
    {
        app.parse(argc, argv);
        if (*create)
        {
            options.destination = destination;
            Create(options, {Run, Get});
        }
        else
            Package(directory, Run);
        return 0;
    }
    catch (const CLI::ParseError &error)
    {
        return app.exit(error);
    }
    catch (const std::exception &error)
    {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
} // namespace grapple::project
