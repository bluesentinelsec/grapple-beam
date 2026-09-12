#include <SDL3/SDL_opengl.h>
#include <grapple/engine_backend.h>

typedef struct Backend
{
    const char *name;
    const char *label;
    const char *renderer;
} Backend;

static const Backend kBackends[] = {
    {"directx11", "DirectX 11", "direct3d11"},
    {"directx12", "DirectX 12", "direct3d12"},
    {"vulkan", "Vulkan", "vulkan"},
    {"metal", "Metal", "metal"},
    {"opengl", "OpenGL", "opengl"},
    {"opengles2", "OpenGL ES", "opengles2"},
    {"software", "Software", "software"},
};

static const Backend *FindBackend(const char *name)
{
    if (name != NULL)
        for (size_t i = 0; i < SDL_arraysize(kBackends); ++i)
            if (SDL_strcmp(name, kBackends[i].name) == 0)
                return &kBackends[i];
    return NULL;
}

static bool Compiled(const Backend *backend)
{
    for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
        if (SDL_strcmp(SDL_GetRenderDriver(i), backend->renderer) == 0)
            return true;
    return false;
}

int Grapple_RenderBackendCount(void)
{
    return (int)SDL_arraysize(kBackends);
}

const char *Grapple_RenderBackendName(int index)
{
    return (index >= 0 && index < Grapple_RenderBackendCount()) ? kBackends[index].name : NULL;
}

bool Grapple_RenderBackendValid(const char *name)
{
    return name != NULL && (SDL_strcmp(name, "auto") == 0 || FindBackend(name) != NULL);
}

SDL_Renderer *Grapple_CreateBackendRenderer(SDL_Window *window, const char *name)
{
    if (window == NULL || !Grapple_RenderBackendValid(name))
    {
        SDL_SetError("invalid window or backend selection");
        return NULL;
    }
    if (SDL_strcmp(name, "auto") == 0)
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, "opengl,opengles2");
        if (renderer != NULL)
            return renderer;
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
        {
            const char *driver = SDL_GetRenderDriver(i);
            if (SDL_strcmp(driver, "opengl") == 0 || SDL_strcmp(driver, "opengles2") == 0)
                continue;
            renderer = SDL_CreateRenderer(window, driver);
            if (renderer != NULL)
                return renderer;
        }
        return NULL;
    }
    const Backend *backend = FindBackend(name);
    if (!Compiled(backend))
    {
        SDL_SetError("backend '%s' is not compiled in this SDL build", name);
        return NULL;
    }
    /* Select SDL's concrete Vulkan renderer, never the multi-API GPU driver. */
    return SDL_CreateRenderer(window, backend->renderer);
}

static void DescribeGL(Grapple_RenderBackendInfo *info)
{
    typedef const unsigned char *(APIENTRY * GetStringFn)(unsigned int);
    GetStringFn get_string = (GetStringFn)SDL_GL_GetProcAddress("glGetString");
    if (get_string == NULL || SDL_GL_GetCurrentContext() == NULL)
        return;
    const unsigned char *version = get_string(0x1F02); /* GL_VERSION */
    const unsigned char *device = get_string(0x1F01);  /* GL_RENDERER */
    const unsigned char *vendor = get_string(0x1F00);  /* GL_VENDOR */
    if (version != NULL)
        SDL_strlcpy(info->api_version, (const char *)version, sizeof(info->api_version));
    if (device != NULL)
        SDL_strlcpy(info->device, (const char *)device, sizeof(info->device));
    if (vendor != NULL)
        SDL_strlcpy(info->driver, (const char *)vendor, sizeof(info->driver));
}

bool Grapple_ProbeRenderBackend(const char *name, Grapple_RenderBackendInfo *info)
{
    const Backend *backend = FindBackend(name);
    if (backend == NULL || info == NULL)
        return SDL_SetError("invalid backend probe arguments");
    SDL_zero(*info);
    SDL_strlcpy(info->name, backend->name, sizeof(info->name));
    SDL_strlcpy(info->label, backend->label, sizeof(info->label));
    info->compiled = Compiled(backend);
    if (!info->compiled)
    {
        SDL_strlcpy(info->error, "not compiled in this SDL build", sizeof(info->error));
        return true;
    }
    SDL_Window *window = SDL_CreateWindow("grapple-beam backend probe", 64, 64, SDL_WINDOW_HIDDEN);
    SDL_Renderer *renderer = window != NULL ? Grapple_CreateBackendRenderer(window, name) : NULL;
    if (renderer == NULL)
    {
        SDL_strlcpy(info->error, SDL_GetError(), sizeof(info->error));
        SDL_DestroyWindow(window);
        return true;
    }
    Grapple_DescribeRenderBackend(renderer, info);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return true;
}

bool Grapple_DescribeRenderBackend(SDL_Renderer *renderer, Grapple_RenderBackendInfo *info)
{
    if (!renderer || !info)
        return SDL_InvalidParamError("renderer/info");
    SDL_zero(*info);
    const char *name = SDL_GetRendererName(renderer);
    SDL_strlcpy(info->name, name, sizeof(info->name));
    SDL_strlcpy(info->label, name, sizeof(info->label));
    for (int i = 0; i < Grapple_RenderBackendCount(); ++i)
        if (!SDL_strcmp(kBackends[i].renderer, name))
        {
            SDL_strlcpy(info->name, kBackends[i].name, sizeof(info->name));
            SDL_strlcpy(info->label, kBackends[i].label, sizeof(info->label));
            break;
        }
    info->compiled = true;
    info->available = true;
    SDL_strlcpy(info->renderer, SDL_GetRendererName(renderer), sizeof(info->renderer));
    info->opengl_effects =
        SDL_strcmp(info->renderer, "opengl") == 0 || SDL_strcmp(info->renderer, "opengles2") == 0;
    if (info->opengl_effects)
    {
        SDL_FlushRenderer(renderer);
        DescribeGL(info);
    }
    return true;
}
