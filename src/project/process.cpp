#include "project.hpp"

#include <SDL3/SDL.h>
#include <iostream>
#include <memory>
#include <mog/mog_c.h>
#include <stdexcept>

namespace grapple::project
{
std::string Get(const std::string &url)
{
    std::unique_ptr<mog_request, decltype(&mog_request_free)> request{
        mog_request_new("GET", url.c_str()), mog_request_free};
    if (!request)
        throw std::runtime_error("Cannot allocate HTTP request");
    mog_request_set_timeout_ms(request.get(), 30000);
    mog_request_set_connect_timeout_ms(request.get(), 10000);
    mog_request_set_max_response_bytes(request.get(), 2 * 1024 * 1024);
    mog_request_set_user_agent(request.get(), "grapple-beam-projects");
    mog_request_set_verify_tls(request.get(), 1);
    // Public use needs no login. CI can opt into authenticated metadata limits.
    // The token is never forwarded to redirects or raw/download hosts.
    if (url.starts_with("https://api.github.com/"))
    {
        const char *token = SDL_getenv("GITHUB_TOKEN");
        if (token && *token)
        {
            mog_request_set_bearer_token(request.get(), token);
            mog_request_set_allow_redirects(request.get(), 0);
        }
    }
    std::unique_ptr<mog_response, decltype(&mog_response_free)> response{mog_perform(request.get()),
                                                                         mog_response_free};
    if (!response || !mog_response_ok(response.get()))
        throw std::runtime_error(
            "Cannot fetch " + url + ": " +
            (response ? mog_response_error_message(response.get()) : "no response"));
    const int status = mog_response_status(response.get());
    if (status != 200)
        throw std::runtime_error("HTTP " + std::to_string(status) + " fetching " + url +
                                 (status == 403 || status == 429
                                      ? "; public API rate limit may be exhausted; retry later"
                                      : "; check the requested ref"));
    size_t size = 0;
    const char *body = mog_response_body(response.get(), &size);
    return body ? std::string(body, size) : std::string{};
}
} // namespace grapple::project
