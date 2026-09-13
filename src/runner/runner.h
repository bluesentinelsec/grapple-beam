/* Original Grapple code (zlib). Private interpreter runner entry. */
#ifndef GRAPPLE_RUNNER_H
#define GRAPPLE_RUNNER_H
#ifdef __cplusplus
extern "C"
{
#endif
    /** @brief Run the Lua/Ruby engine CLI. Arguments and version are borrowed for the call. */
    int GrappleRunner_Run(int argc, char **argv, const char *version);
    /** @brief Execute a validated invocation. All arguments are borrowed for the call. */
    int GrappleRunner_Execute(const char *language, const char *code, const char *script, int argc,
                              char **argv);
#ifdef __cplusplus
}
#endif
#endif
