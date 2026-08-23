/**
 * @file test_main.mm
 * @brief End-to-end iOS tests consuming the packaged grapple-beam XCFramework.
 */

#import <UIKit/UIKit.h>
#import <os/log.h>

#include <grapple/version.hpp>

#include <grapple/engine.h>
#include <grapple/engine_actor.h>
#include <grapple/engine_config.h>
#include <grapple/bindings.h>
#include <grapple/lua.h>

#include <SDL3/SDL.h>

#include <string>
#include <string_view>

namespace
{

os_log_t TestLog()
{
    static os_log_t log = os_log_create("com.example.sdl3staticextensions.test", "tests");
    return log;
}

class TestRun
{
  public:
    void Check(bool condition, std::string_view message)
    {
        const std::string text{message};
        if (condition)
        {
            os_log_info(TestLog(), "PASS: %{public}s", text.c_str());
            return;
        }
        ++failures_;
        os_log_error(TestLog(), "FAIL: %{public}s", text.c_str());
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
    run.Check(version == GRAPPLE_BEAM_EXPECTED_VERSION,
              "XCFramework exports the VERSION-derived API");
    const std::string major_prefix = std::to_string(grapple::kVersionMajor) + ".";
    run.Check(version.substr(0, major_prefix.size()) == major_prefix,
              "Version() matches the compiled major version");
    run.Check(grapple::kVersionMajor >= 0, "major version is non-negative");

    // The engine, not just the version string. The XCFramework passed its
    // slice, architecture and version checks for months while containing
    // nothing but version.cpp — all three were true of an empty library.
    Grapple_EngineConfig *config = Grapple_ConfigCreate();
    run.Check(config != nullptr, "the engine's builders are in the framework");
    if (config != nullptr)
    {
        // Headless with a manual clock: a simulator test has no window to
        // wait on and no real clock worth waiting for.
        Grapple_ConfigSetHeadless(config, true);
        Grapple_ConfigSetManualClock(config, true);
        Grapple_ConfigSetAutoMount(config, false);

        Grapple_Engine *engine = Grapple_CreateEngine(config);
        Grapple_ConfigDestroy(config);
        run.Check(engine != nullptr, "an engine can be created on iOS");
        if (engine != nullptr)
        {
            Grapple_ActorDef *def = Grapple_ActorDefCreate();
            Grapple_ActorDefSetType(def, "ios");
            const Grapple_ActorId actor = Grapple_ActorSpawn(engine, def);
            Grapple_ActorDefDestroy(def);
            run.Check(actor != GRAPPLE_ACTOR_NONE, "an actor can be spawned");

            for (int i = 0; i < 5; ++i)
            {
                Grapple_EngineAdvance(engine, 16666667);
                Grapple_EngineTick(engine);
            }
            run.Check(Grapple_EngineFrameCount(engine) >= 5, "the loop runs frames");
            run.Check(Grapple_ActorCount(engine) == 1, "the actor survived the frames");
            Grapple_DestroyEngine(engine);
        }
    }

    // Lua, which is the component iOS nearly could not have: os.execute calls
    // system(), and iOS marks it unavailable. Upstream's LUA_USE_IOS turns it
    // into a stub, so the rest of the language is here.
    lua_State *lua = Grapple_CreateLuaState();
    run.Check(lua != nullptr, "a Lua state can be created");
    if (lua != nullptr)
    {
        run.Check(Grapple_OpenLuaBindings(lua), "the generated bindings load");
        lua_close(lua);
    }

    run.Check(SDL_GetPlatform() != nullptr, "SDL3 itself is in the framework");
    return run.failures();
}

} // namespace

@interface TestAppDelegate : UIResponder <UIApplicationDelegate>
@property(nonatomic, strong) UIWindow *window;
@end

@implementation TestAppDelegate

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    (void)application;
    (void)launchOptions;
    self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    self.window.rootViewController = [[UIViewController alloc] init];
    self.window.rootViewController.view.backgroundColor = UIColor.systemBackgroundColor;
    [self.window makeKeyAndVisible];

    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
      const int failures = RunTests();
      os_log_info(TestLog(), "GRAPPLE_BEAM_IOS_TEST_RESULT: %{public}d", failures);
    });
    return YES;
}

@end

int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass(TestAppDelegate.class));
    }
}
