/*
 * grapple_engine_script.c — the loop contract for scripts.
 * Original Grapple code (zlib).
 *
 * The generator can bind anything taking numbers and handles. It cannot
 * hand C a function pointer and a void* — which is what a hook is — so this
 * is the bridge, and it is the only hand-written part of the script
 * surface.
 *
 * It deliberately knows nothing about Lua or mruby. A language installs one
 * dispatch function and one release function; everything else here is an
 * opaque Sint64 the language chose to mean "this script function". Adding a
 * third language costs a dispatcher and no changes to this file or to the
 * engine.
 */
#include "engine_internal.h"

#include <grapple/engine_script.h>

/* One script-defined scene. */
typedef struct ScriptScene
{
    char name[64];
    Grapple_SceneFlags flags;
    size_t state_size;
    Sint64 handles[GRAPPLE_SCENE_HOOK_COUNT];
    bool registered[GRAPPLE_SCENE_HOOK_COUNT];
} ScriptScene;

struct Grapple_ScriptBridge
{
    void *language_state;
    Grapple_ScriptDispatch dispatch;
    Grapple_ScriptRelease release;

    Sint64 handles[GRAPPLE_HOOK_COUNT];
    bool registered[GRAPPLE_HOOK_COUNT];

    Grapple_GameHooks hooks;

    /* Scene definitions a script registered, keyed by name. Name rather
       than pointer because the engine copies a definition when it pushes
       one: a C game may pass a local and forget it, and a script has
       nowhere to keep a struct at all. A name outlives both, and it is
       what the trampolines have to work from — a callback is handed a
       scene, and Grapple_SceneName is the way back here. */
    Grapple_ScriptSceneDispatch scene_dispatch;
    ScriptScene *scenes;
    int scene_count, scene_capacity;
};

/* Defined with the rest of the scene half, below; unbind needs it, and a
   scene's handlers are released the same way a hook's are. */
static void ReleaseScene(struct Grapple_ScriptBridge *bridge, ScriptScene *scene);

static struct Grapple_ScriptBridge *Bridge(Grapple_Engine *engine, bool create)
{
    if (engine == NULL)
    {
        return NULL;
    }
    if (engine->script == NULL && create)
    {
        engine->script = (struct Grapple_ScriptBridge *)SDL_calloc(
            1, sizeof(struct Grapple_ScriptBridge));
    }
    return engine->script;
}

/* One trampoline per hook. Each is the C function pointer the engine wants,
   and each does the same thing: look up the script handle for its hook and
   ask the language to call it. */
static bool CallWith(Grapple_Engine *engine, Grapple_ScriptHook hook, float value,
                     const void *payload)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL || bridge->dispatch == NULL || !bridge->registered[hook])
    {
        return true;
    }
    return bridge->dispatch(bridge->language_state, bridge->handles[hook], hook, value, payload);
}

static bool Call(Grapple_Engine *engine, Grapple_ScriptHook hook, float value)
{
    return CallWith(engine, hook, value, NULL);
}

static bool OnLoad(void *user)
{
    return Call((Grapple_Engine *)user, GRAPPLE_HOOK_LOAD, 0.0f);
}
static void OnFixedUpdate(void *user, float step)
{
    Call((Grapple_Engine *)user, GRAPPLE_HOOK_FIXED_UPDATE, step);
}
static void OnUpdate(void *user, float dt)
{
    Call((Grapple_Engine *)user, GRAPPLE_HOOK_UPDATE, dt);
}
static void OnRender(void *user, float alpha)
{
    Call((Grapple_Engine *)user, GRAPPLE_HOOK_RENDER, alpha);
}
static void OnPostRender(void *user)
{
    Call((Grapple_Engine *)user, GRAPPLE_HOOK_POST_RENDER, 0.0f);
}
static void OnEvent(void *user, const SDL_Event *event)
{
    /* The event is borrowed for the length of the call: the language wraps
       it as a handle and must not keep it. */
    CallWith((Grapple_Engine *)user, GRAPPLE_HOOK_EVENT, 0.0f, event);
}
static void OnResize(void *user, int width, int height)
{
    const Grapple_ScriptSize size = {width, height};
    CallWith((Grapple_Engine *)user, GRAPPLE_HOOK_RESIZE, 0.0f, &size);
}
static void OnUnload(void *user)
{
    Call((Grapple_Engine *)user, GRAPPLE_HOOK_UNLOAD, 0.0f);
}

/* Publish only the hooks the script actually registered. A null C pointer
   means "no hook", so an unregistered one costs nothing per frame rather
   than a dispatch that finds nothing to call. */
static void Publish(Grapple_Engine *engine, struct Grapple_ScriptBridge *bridge)
{
    SDL_zero(bridge->hooks);
    if (bridge->registered[GRAPPLE_HOOK_LOAD])
    {
        bridge->hooks.load = OnLoad;
    }
    if (bridge->registered[GRAPPLE_HOOK_FIXED_UPDATE])
    {
        bridge->hooks.fixed_update = OnFixedUpdate;
    }
    if (bridge->registered[GRAPPLE_HOOK_UPDATE])
    {
        bridge->hooks.update = OnUpdate;
    }
    if (bridge->registered[GRAPPLE_HOOK_RENDER])
    {
        bridge->hooks.render = OnRender;
    }
    if (bridge->registered[GRAPPLE_HOOK_POST_RENDER])
    {
        bridge->hooks.post_render = OnPostRender;
    }
    if (bridge->registered[GRAPPLE_HOOK_EVENT])
    {
        bridge->hooks.event = OnEvent;
    }
    if (bridge->registered[GRAPPLE_HOOK_RESIZE])
    {
        bridge->hooks.resize = OnResize;
    }
    if (bridge->registered[GRAPPLE_HOOK_UNLOAD])
    {
        bridge->hooks.unload = OnUnload;
    }
    /* The engine is its own user pointer: the trampolines need to find the
       bridge, and the engine is the one thing they are guaranteed to be
       given. */
    Grapple_EngineSetHooks(engine, &bridge->hooks, engine);
}

bool Grapple_ScriptBind(Grapple_Engine *engine, void *language_state,
                          Grapple_ScriptDispatch dispatch, Grapple_ScriptRelease release)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, true);
    if (bridge == NULL || dispatch == NULL)
    {
        SDL_InvalidParamError("engine/dispatch");
        return false;
    }
    /* Binding the same state again is a no-op, not a reset. A language's
       registration helper calls this on every OnUpdate/OnRender, and
       unbinding each time would release the handler registered a moment
       ago — leaving only whichever hook was registered last, which is a
       bug that looks exactly like "hooks do not fire". */
    if (bridge->dispatch == dispatch && bridge->language_state == language_state)
    {
        return true;
    }

    /* A genuinely different language state does reset, so the outgoing one
       does not leak the references it was holding. */
    Grapple_ScriptUnbind(engine);
    bridge = Bridge(engine, true);
    if (bridge == NULL)
    {
        return false;
    }
    bridge->language_state = language_state;
    bridge->dispatch = dispatch;
    bridge->release = release;
    return true;
}

bool Grapple_ScriptSetHook(Grapple_Engine *engine, Grapple_ScriptHook hook, Sint64 handle)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL || bridge->dispatch == NULL || hook >= GRAPPLE_HOOK_COUNT)
    {
        SDL_SetError("no script bound to this engine");
        return false;
    }
    if (bridge->registered[hook] && bridge->release != NULL)
    {
        /* Replacing a handler releases the old handle, so re-registering in
           a loop does not pin every previous closure alive. */
        bridge->release(bridge->language_state, bridge->handles[hook]);
    }
    bridge->handles[hook] = handle;
    bridge->registered[hook] = true;
    Publish(engine, bridge);
    return true;
}

bool Grapple_ScriptHasHandlers(Grapple_Engine *engine)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL)
    {
        return false;
    }
    for (int i = 0; i < GRAPPLE_HOOK_COUNT; ++i)
    {
        if (bridge->registered[i])
        {
            return true;
        }
    }
    return false;
}

bool Grapple_ScriptRun(Grapple_Engine *engine)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL || !Grapple_ScriptHasHandlers(engine))
    {
        /* A script that calls Run with nothing registered has almost
           certainly forgotten to register, and a silent black window is a
           worse answer than an error. */
        SDL_SetError("no handlers registered: register one, or drive the loop "
                     "yourself with EngineTick");
        return false;
    }
    Publish(engine, bridge);
    return Grapple_RunGame(engine, &bridge->hooks, engine);
}

void Grapple_ScriptUnbind(Grapple_Engine *engine)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL)
    {
        return;
    }
    if (bridge->release != NULL)
    {
        for (int i = 0; i < GRAPPLE_HOOK_COUNT; ++i)
        {
            if (bridge->registered[i])
            {
                bridge->release(bridge->language_state, bridge->handles[i]);
            }
        }
    }
    for (int i = 0; i < bridge->scene_count; ++i)
    {
        ReleaseScene(bridge, &bridge->scenes[i]);
    }
    /* Detach from the engine before freeing: a hook still pointing at freed
       memory is exactly the crash this whole file exists to avoid. The
       scene trampolines reach the bridge through the engine, so clearing
       the scene stack matters for the same reason. */
    Grapple_EngineSetHooks(engine, NULL, NULL);
    SDL_free(bridge->scenes);
    SDL_free(bridge);
    engine->script = NULL;
}


/* --- scenes ---------------------------------------------------------------- */

static ScriptScene *FindScene(struct Grapple_ScriptBridge *bridge, const char *name)
{
    if (bridge == NULL || name == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < bridge->scene_count; ++i)
    {
        if (SDL_strcmp(bridge->scenes[i].name, name) == 0)
        {
            return &bridge->scenes[i];
        }
    }
    return NULL;
}

/* Every scene callback lands here: find the definition this scene was made
   from, and ask the language to call the handler registered for the hook.
   The lookup is by name because that is all a callback is given — and the
   stack is eight deep at most, so a linear scan is not worth avoiding. */
static bool CallScene(Grapple_Scene *scene, Grapple_SceneHook hook, float value,
                      const SDL_Event *event)
{
    if (scene == NULL)
    {
        return true;
    }
    struct Grapple_ScriptBridge *bridge = Bridge(Grapple_SceneEngine(scene), false);
    if (bridge == NULL || bridge->scene_dispatch == NULL)
    {
        return true;
    }
    const ScriptScene *definition = FindScene(bridge, Grapple_SceneName(scene));
    if (definition == NULL || !definition->registered[hook])
    {
        return true;
    }
    return bridge->scene_dispatch(bridge->language_state, definition->handles[hook], hook,
                                  scene, value, event);
}

static bool SceneLoad(Grapple_Scene *scene)
{
    return CallScene(scene, GRAPPLE_SCENE_HOOK_LOAD, 0.0f, NULL);
}
static void SceneEnter(Grapple_Scene *scene)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_ENTER, 0.0f, NULL);
}
static void SceneFixedUpdate(Grapple_Scene *scene, float step)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_FIXED_UPDATE, step, NULL);
}
static void SceneUpdate(Grapple_Scene *scene, float dt)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_UPDATE, dt, NULL);
}
static void SceneRender(Grapple_Scene *scene, float alpha)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_RENDER, alpha, NULL);
}
static void SceneEvent(Grapple_Scene *scene, const SDL_Event *event)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_EVENT, 0.0f, event);
}
static void SceneExit(Grapple_Scene *scene)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_EXIT, 0.0f, NULL);
}
static void SceneUnload(Grapple_Scene *scene)
{
    CallScene(scene, GRAPPLE_SCENE_HOOK_UNLOAD, 0.0f, NULL);
}

/* Build the definition the engine wants from the one the script built.
   Only registered hooks get a C pointer: a null one means "no callback",
   so an unregistered hook costs nothing rather than a dispatch that finds
   nothing to call. */
static bool BuildDef(struct Grapple_ScriptBridge *bridge, const char *name,
                     Grapple_SceneDef *out)
{
    const ScriptScene *definition = FindScene(bridge, name);
    if (definition == NULL)
    {
        SDL_SetError("no scene defined as '%s'", (name != NULL) ? name : "(null)");
        return false;
    }
    SDL_zerop(out);
    out->name = definition->name;
    out->flags = definition->flags;
    out->state_size = definition->state_size;
    if (definition->registered[GRAPPLE_SCENE_HOOK_LOAD])
    {
        out->load = SceneLoad;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_ENTER])
    {
        out->enter = SceneEnter;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_FIXED_UPDATE])
    {
        out->fixed_update = SceneFixedUpdate;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_UPDATE])
    {
        out->update = SceneUpdate;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_RENDER])
    {
        out->render = SceneRender;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_EVENT])
    {
        out->event = SceneEvent;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_EXIT])
    {
        out->exit = SceneExit;
    }
    if (definition->registered[GRAPPLE_SCENE_HOOK_UNLOAD])
    {
        out->unload = SceneUnload;
    }
    return true;
}

bool Grapple_ScriptSetSceneDispatch(Grapple_Engine *engine,
                                      Grapple_ScriptSceneDispatch dispatch)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL || bridge->dispatch == NULL)
    {
        SDL_SetError("no script bound to this engine");
        return false;
    }
    bridge->scene_dispatch = dispatch;
    return true;
}

/* Release a definition's handlers. Used both when redefining a name and
   when the bridge goes away, so a reloaded script does not accumulate
   closures it can no longer reach. */
static void ReleaseScene(struct Grapple_ScriptBridge *bridge, ScriptScene *scene)
{
    if (bridge->release != NULL)
    {
        for (int i = 0; i < GRAPPLE_SCENE_HOOK_COUNT; ++i)
        {
            if (scene->registered[i])
            {
                bridge->release(bridge->language_state, scene->handles[i]);
            }
        }
    }
    SDL_zeroa(scene->handles);
    SDL_zeroa(scene->registered);
}

bool Grapple_ScriptSceneDefine(Grapple_Engine *engine, const char *name)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    if (bridge == NULL || bridge->dispatch == NULL)
    {
        SDL_SetError("no script bound to this engine");
        return false;
    }
    if (name == NULL || name[0] == '\0')
    {
        SDL_InvalidParamError("name");
        return false;
    }

    /* Redefining a name starts it over rather than adding a second entry:
       a script reloaded during development should replace its scenes, not
       shadow them with handlers the old definition still holds. */
    ScriptScene *existing = FindScene(bridge, name);
    if (existing != NULL)
    {
        ReleaseScene(bridge, existing);
        existing->flags = GRAPPLE_SCENE_DEFAULT;
        existing->state_size = 0;
        return true;
    }

    if (bridge->scene_count >= bridge->scene_capacity)
    {
        const int next = (bridge->scene_capacity > 0) ? bridge->scene_capacity * 2 : 4;
        ScriptScene *grown =
            (ScriptScene *)SDL_realloc(bridge->scenes, (size_t)next * sizeof(ScriptScene));
        if (grown == NULL)
        {
            return false;
        }
        bridge->scenes = grown;
        bridge->scene_capacity = next;
    }
    ScriptScene *scene = &bridge->scenes[bridge->scene_count++];
    SDL_zerop(scene);
    SDL_strlcpy(scene->name, name, sizeof(scene->name));
    return true;
}

bool Grapple_ScriptSceneSetHook(Grapple_Engine *engine, const char *name,
                                  Grapple_SceneHook hook, Sint64 handle)
{
    struct Grapple_ScriptBridge *bridge = Bridge(engine, false);
    ScriptScene *scene = FindScene(bridge, name);
    if (scene == NULL || hook >= GRAPPLE_SCENE_HOOK_COUNT)
    {
        SDL_SetError("no scene defined as '%s'", (name != NULL) ? name : "(null)");
        return false;
    }
    if (scene->registered[hook] && bridge->release != NULL)
    {
        bridge->release(bridge->language_state, scene->handles[hook]);
    }
    scene->handles[hook] = handle;
    scene->registered[hook] = true;
    return true;
}

bool Grapple_ScriptSceneSetFlags(Grapple_Engine *engine, const char *name,
                                   Grapple_SceneFlags flags)
{
    ScriptScene *scene = FindScene(Bridge(engine, false), name);
    if (scene == NULL)
    {
        SDL_SetError("no scene defined as '%s'", (name != NULL) ? name : "(null)");
        return false;
    }
    scene->flags = flags;
    return true;
}

bool Grapple_ScriptSceneSetStateSize(Grapple_Engine *engine, const char *name,
                                       int state_size)
{
    ScriptScene *scene = FindScene(Bridge(engine, false), name);
    if (scene == NULL || state_size < 0)
    {
        SDL_SetError("no scene defined as '%s'", (name != NULL) ? name : "(null)");
        return false;
    }
    scene->state_size = (size_t)state_size;
    return true;
}

/* The scene's address, narrowed to something a script can hold as a table
   key. A handle is boxed fresh every time it crosses into a language, so
   the scene itself is a different key each frame — a per-scene table keyed
   that way grows forever and never finds what it stored. */
Sint64 Grapple_SceneKey(Grapple_Scene *scene)
{
    return (Sint64)(uintptr_t)scene;
}

bool Grapple_ScriptSceneDefined(Grapple_Engine *engine, const char *name)
{
    return FindScene(Bridge(engine, false), name) != NULL;
}

bool Grapple_ScriptScenePush(Grapple_Engine *engine, const char *name)
{
    Grapple_SceneDef def;
    if (!BuildDef(Bridge(engine, false), name, &def))
    {
        return false;
    }
    /* The engine copies the definition, so this local going out of scope
       at the end of the call is exactly the case that is safe. */
    return Grapple_ScenePush(engine, &def);
}

bool Grapple_ScriptSceneReplace(Grapple_Engine *engine, const char *name)
{
    Grapple_SceneDef def;
    if (!BuildDef(Bridge(engine, false), name, &def))
    {
        return false;
    }
    return Grapple_SceneReplace(engine, &def);
}

bool Grapple_ScriptSceneReset(Grapple_Engine *engine, const char *name)
{
    Grapple_SceneDef def;
    if (!BuildDef(Bridge(engine, false), name, &def))
    {
        return false;
    }
    return Grapple_SceneReset(engine, &def);
}

bool Grapple_ScriptSceneTransitionTo(Grapple_Engine *engine, const char *name,
                                       Grapple_SceneTransition transition, float seconds)
{
    Grapple_SceneDef def;
    if (!BuildDef(Bridge(engine, false), name, &def))
    {
        return false;
    }
    return Grapple_SceneTransitionTo(engine, &def, transition, seconds);
}
