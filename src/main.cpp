/** @file main.cpp @brief Desktop engine runner entrypoint. */
#include "grapple/version.hpp"
#include "runner/runner.h"

#include <string>

/** @brief Run the engine CLI.
 * @param argc Number of process arguments.
 * @param argv Process argument vector.
 * @return Runner exit status. */
int main(int argc, char **argv)
{
    const std::string version{grapple::Version()};
    return GrappleRunner_Run(argc, argv, version.c_str());
}
