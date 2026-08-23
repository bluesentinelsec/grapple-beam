/*
 * light_internal.h — shared between the two rendering paths.
 *
 * Grapple::Light produces the same light map either way:
 *
 *   the shader path (grapple_light_gl.c) when SDL's renderer is on
 *   OpenGL or OpenGL ES — per-pixel falloff and shadows raymarched against
 *   an occluder mask, done by the GPU;
 *
 *   the geometry path (grapple_light.c) everywhere else — Metal, D3D, or
 *   a software renderer in a test — where the visibility polygon is built
 *   on the CPU and filled as triangles.
 *
 * Everything above the light map is shared: the ambient colour, the
 * camera, the day/night cycle, and the gameplay queries, which run on the
 * CPU in both cases so the answers never depend on the backend.
 */
#ifndef GRAPPLE_LIGHT_INTERNAL_H
#define GRAPPLE_LIGHT_INTERNAL_H

#include <grapple/light.h>

/* Uniform array size in the fragment shader; must match MAX_LIGHTS there. */
#define GRAPPLE_LIGHT_GL_MAX 16

/* GL's calling convention differs on Windows, and the function pointers
 * are declared here rather than pulled from a GL header we do not ship. */
#ifdef _WIN32
#define GLAPIENTRY_UNUSED __stdcall
#else
#define GLAPIENTRY_UNUSED
#endif
#define APIENTRY_UNUSED GLAPIENTRY_UNUSED

typedef struct Segment
{
    float x1, y1, x2, y2;
} Segment;

struct LightGl; /* opaque: only grapple_light_gl.c knows its shape */

struct Grapple_LightScene
{
    SDL_Renderer *renderer;
    SDL_Texture *map;  /* the light map, multiplied over the frame */
    SDL_Texture *mask; /* occluders as white on black, for the shader */
    int map_w, map_h;
    float map_scale; /* light map resolution relative to the output */

    SDL_FColor ambient;
    float camera_x, camera_y;
    float shadow_softness;
    bool map_swizzle; /* the light map's format needs R and B swapped */

    Grapple_Light lights[GRAPPLE_LIGHT_MAX];
    int light_count;
    Segment occluders[GRAPPLE_LIGHT_OCCLUDER_MAX];
    int occluder_count;
    SDL_FRect occluder_rects[GRAPPLE_LIGHT_OCCLUDER_MAX / 4];
    int occluder_rect_count;
    struct
    {
        SDL_FRect rect;
        SDL_FColor ambient;
    } zones[GRAPPLE_LIGHT_ZONE_MAX];
    int zone_count;

    int rays;
    int rings;
    bool debug_draw;
    bool prefer_shaders;

    struct LightGl *gl; /* NULL when the shader path is not in use */

    /* Scratch reused across lights so a frame does not allocate. */
    float *angles;
    int angle_capacity;
    SDL_Vertex *verts;
    int *indices;
    int vert_capacity;
    int index_capacity;
};

/** Flicker multiplier for a light at `seconds`; shared so the shader path,
 *  the geometry path and Grapple_SampleLight all agree on how bright a
 *  guttering torch is at any instant. */
extern float Grapple_LightFlickerScale(const Grapple_Light *light, float seconds);

/** Set up the shader path. Returns false — without setting an error worth
 *  reporting — when the renderer is not GL, which is a normal outcome. */
extern bool Grapple_LightGlInit(Grapple_LightScene *scene);

extern void Grapple_LightGlQuit(Grapple_LightScene *scene);

/** Draw the light map with the shader. The caller has already bound the
 *  light map as SDL's render target. */
extern bool Grapple_LightGlRenderMap(Grapple_LightScene *scene, float seconds);

#endif /* GRAPPLE_LIGHT_INTERNAL_H */
