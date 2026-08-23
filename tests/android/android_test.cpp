/**
 * @file android_test.cpp
 * @brief End-to-end Android tests consuming the grapple-beam Prefab package.
 */

#include <grapple/version.hpp>

#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_config.h>
#include <grapple/bindings.h>
#include <grapple/lua.h>

#include <SDL3/SDL.h>

#include <android/log.h>
#include <jni.h>
#include <string>
#include <string_view>

namespace
{

constexpr const char *kLogTag = "grapple-beam-android-test";

class TestRun
{
  public:
    void Check(bool condition, std::string_view message)
    {
        if (condition)
        {
            __android_log_print(ANDROID_LOG_INFO, kLogTag, "PASS: %.*s",
                                static_cast<int>(message.size()), message.data());
            return;
        }
        ++failures_;
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "FAIL: %.*s",
                            static_cast<int>(message.size()), message.data());
    }

    [[nodiscard]] int failures() const noexcept
    {
        return failures_;
    }

  private:
    int failures_ = 0;
};

int RunTests()
{
    TestRun run;
    const std::string_view version = grapple::Version();
    run.Check(!version.empty(), "Prefab package exports the generated version API");
    const std::string major_prefix = std::to_string(grapple::kVersionMajor) + ".";
    run.Check(version.substr(0, major_prefix.size()) == major_prefix,
              "Version() matches the compiled major version");
    run.Check(grapple::kVersionMajor >= 0, "major version is non-negative");

    // The engine's surface, called rather than merely linked. Checking the
    // version string alone is how this test passed for months against an
    // AAR that contained nothing else.
    //
    // What it deliberately does not do is create an engine. SDL's Android
    // backend expects to be driven by org.libsdl.app.SDLActivity — it owns
    // the surface, the looper and the main thread — and this harness is a
    // plain Activity calling in over JNI, where Grapple_CreateEngine
    // blocks waiting for plumbing that is not there. A game shipping this
    // AAR would subclass SDLActivity and be fine; proving that needs an
    // SDLActivity-based harness, which is its own piece of work.
    Grapple_EngineConfig *config = Grapple_ConfigCreate();
    run.Check(config != nullptr, "the engine's builders are in the package");
    if (config != nullptr)
    {
        Grapple_ConfigSetHeadless(config, true);
        Grapple_ConfigSetDesignSize(config, 640, 360);
        Grapple_ConfigSetTitle(config, "android");
        Grapple_ConfigDestroy(config);
    }

    Grapple_ActorDef *def = Grapple_ActorDefCreate();
    run.Check(def != nullptr, "actor definitions can be built");
    if (def != nullptr)
    {
        Grapple_ActorDefSetType(def, "android");
        Grapple_ActorDefDestroy(def);
    }

    // The script surface: Lua and its bindings are a large part of what the
    // package is for, and the largest part of what a link error would drop.
    // None of it needs a window.
    lua_State *lua = Grapple_CreateLuaState();
    run.Check(lua != nullptr, "a Lua state can be created");
    if (lua != nullptr)
    {
        run.Check(Grapple_OpenLuaBindings(lua), "the generated bindings load");
        lua_close(lua);
    }

    run.Check(SDL_GetPlatform() != nullptr, "SDL3 itself is in the package");
    return run.failures();
}

} // namespace

extern "C" JNIEXPORT jint JNICALL
Java_com_example_sdl3staticextensions_test_TestActivity_runNativeTests(JNIEnv *, jclass)
{
    return RunTests();
}
