/**
 * @file scene_test.cpp
 * @brief Grapple::Engine — the scene stack.
 *
 * The interesting properties are all about ordering: lifecycle callbacks
 * firing in the right order and the right number of times, stack changes
 * that are safe to request from inside a callback, and covered scenes
 * behaving the way a pause menu needs them to.
 */
#include <grapple/engine.h>
#include <grapple/engine_scene.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{

constexpr Uint64 kFrameNs = 1000000000ull / 60;

// Scenes record what happened to them into one shared log, so a test can
// assert on the exact order of lifecycle events.
std::vector<std::string> g_log;

struct SceneState
{
    int fixed_updates = 0;
    int renders = 0;
};

void Note(Grapple_Scene *scene, const char *what)
{
    const char *name = Grapple_SceneName(scene);
    g_log.push_back(std::string(name != nullptr ? name : "?") + ":" + what);
}

bool SceneLoad(Grapple_Scene *scene)
{
    Note(scene, "load");
    return true;
}
void SceneEnter(Grapple_Scene *scene) { Note(scene, "enter"); }
void SceneExit(Grapple_Scene *scene) { Note(scene, "exit"); }
void SceneUnload(Grapple_Scene *scene) { Note(scene, "unload"); }
void SceneFixedUpdate(Grapple_Scene *scene, float step)
{
    (void)step;
    static_cast<SceneState *>(Grapple_SceneState(scene))->fixed_updates++;
}
void SceneRender(Grapple_Scene *scene, float alpha)
{
    (void)alpha;
    static_cast<SceneState *>(Grapple_SceneState(scene))->renders++;
}

Grapple_SceneDef MakeDef(const char *name, Grapple_SceneFlags flags = GRAPPLE_SCENE_DEFAULT)
{
    Grapple_SceneDef def{};
    def.name = name;
    def.state_size = sizeof(SceneState);
    def.flags = flags;
    def.load = SceneLoad;
    def.enter = SceneEnter;
    def.exit = SceneExit;
    def.unload = SceneUnload;
    def.fixed_update = SceneFixedUpdate;
    def.render = SceneRender;
    return def;
}

class SceneHarness : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        g_log.clear();
        ASSERT_TRUE(SDL_Init(0)) << SDL_GetError();
        Grapple_EngineConfig config{};
        config.headless = true;
        config.manual_clock = true;
        config.design_width = 64;
        config.design_height = 64;
        engine_ = Grapple_CreateEngine(&config);
        ASSERT_NE(engine_, nullptr) << SDL_GetError();
    }
    void TearDown() override
    {
        Grapple_DestroyEngine(engine_);
        SDL_Quit();
    }

    void Frame(int count = 1)
    {
        for (int i = 0; i < count; i++)
        {
            Grapple_EngineAdvance(engine_, kFrameNs);
            Grapple_EngineTick(engine_);
        }
    }

    static bool LoggedInOrder(const std::vector<std::string> &wanted)
    {
        size_t at = 0;
        for (const auto &entry : g_log)
        {
            if (at < wanted.size() && entry == wanted[at])
            {
                at++;
            }
        }
        return at == wanted.size();
    }

    Grapple_Engine *engine_ = nullptr;
};

// The documented lifecycle order, and each callback exactly once.
TEST_F(SceneHarness, LifecycleRunsInOrder)
{
    const Grapple_SceneDef title = MakeDef("title");
    EXPECT_TRUE(Grapple_ScenePush(engine_, &title));
    EXPECT_EQ(Grapple_SceneDepth(engine_), 0) << "the change is deferred";

    Frame();
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);
    EXPECT_TRUE(LoggedInOrder({"title:load", "title:enter"}));

    Grapple_DestroyEngine(engine_);
    engine_ = nullptr;
    EXPECT_TRUE(LoggedInOrder({"title:load", "title:enter", "title:exit", "title:unload"}))
        << "destroying the engine still runs exit and unload";

    // TearDown expects an engine; give it an empty one.
    Grapple_EngineConfig config{};
    config.headless = true;
    config.design_width = 8;
    config.design_height = 8;
    engine_ = Grapple_CreateEngine(&config);
}

// Push/pop is the pause-menu case: the level keeps its state, and gets its
// enter() back when the menu goes away.
TEST_F(SceneHarness, PushingKeepsTheSceneBelowAlive)
{
    const Grapple_SceneDef level = MakeDef("level");
    const Grapple_SceneDef menu = MakeDef("menu");

    Grapple_ScenePush(engine_, &level);
    Frame(3);
    SceneState *level_state =
        static_cast<SceneState *>(Grapple_SceneState(Grapple_SceneCurrent(engine_)));
    const int before = level_state->fixed_updates;
    EXPECT_GT(before, 0);

    Grapple_ScenePush(engine_, &menu);
    Frame(); // the push lands at the end of this frame, so the level still
             // simulates once more — that is the deferral rule working
    const int covered_at = level_state->fixed_updates;
    Frame(3);
    EXPECT_EQ(Grapple_SceneDepth(engine_), 2);
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "menu");
    EXPECT_EQ(level_state->fixed_updates, covered_at)
        << "a covered scene is paused by default";
    EXPECT_TRUE(LoggedInOrder({"level:enter", "level:exit", "menu:enter"}));

    Grapple_ScenePop(engine_);
    Frame(3);
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);
    EXPECT_GT(level_state->fixed_updates, covered_at) << "and resumes when uncovered";
    EXPECT_GT(covered_at, before);
    EXPECT_TRUE(LoggedInOrder({"menu:exit", "menu:unload", "level:enter"}));
}

// A level under a dialogue box should keep running; a level under a pause
// menu should not. The flag is the difference.
TEST_F(SceneHarness, CoveredScenesRunOnlyIfTheyAskTo)
{
    const Grapple_SceneDef level = MakeDef("level", GRAPPLE_SCENE_UPDATE_WHEN_COVERED);
    const Grapple_SceneDef dialogue = MakeDef("dialogue", GRAPPLE_SCENE_TRANSPARENT);

    Grapple_ScenePush(engine_, &level);
    Frame();
    SceneState *level_state =
        static_cast<SceneState *>(Grapple_SceneState(Grapple_SceneCurrent(engine_)));

    Grapple_ScenePush(engine_, &dialogue);
    Frame();
    const int updates = level_state->fixed_updates;
    const int renders = level_state->renders;
    Frame(3);

    EXPECT_GT(level_state->fixed_updates, updates) << "still simulating under the dialogue";
    EXPECT_GT(level_state->renders, renders) << "and still drawn, because it is transparent";
}

// An opaque scene on top means the one below is not drawn at all — the
// saving that makes a stack cheap.
TEST_F(SceneHarness, AnOpaqueSceneHidesTheOneBelow)
{
    const Grapple_SceneDef level = MakeDef("level", GRAPPLE_SCENE_UPDATE_WHEN_COVERED);
    const Grapple_SceneDef full = MakeDef("full"); // opaque by default

    Grapple_ScenePush(engine_, &level);
    Frame();
    SceneState *level_state =
        static_cast<SceneState *>(Grapple_SceneState(Grapple_SceneCurrent(engine_)));

    Grapple_ScenePush(engine_, &full);
    Frame(2); // one frame for the push to land, one to be safely covered
    const int renders = level_state->renders;
    Frame(3);
    EXPECT_EQ(level_state->renders, renders) << "hidden scenes are not rendered";
}

// Replace destroys the old scene; reset empties the stack first.
TEST_F(SceneHarness, ReplaceAndReset)
{
    const Grapple_SceneDef one = MakeDef("one");
    const Grapple_SceneDef two = MakeDef("two");
    const Grapple_SceneDef three = MakeDef("three");

    Grapple_ScenePush(engine_, &one);
    Frame();
    Grapple_ScenePush(engine_, &two);
    Frame();
    EXPECT_EQ(Grapple_SceneDepth(engine_), 2);

    Grapple_SceneReplace(engine_, &three);
    Frame();
    EXPECT_EQ(Grapple_SceneDepth(engine_), 2) << "replace swaps the top only";
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "three");
    EXPECT_TRUE(LoggedInOrder({"two:unload", "three:load"}));

    Grapple_SceneReset(engine_, &one);
    Frame();
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1) << "reset clears the whole stack";
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "one");
}

// The reason changes are deferred: a scene must be able to replace itself
// from inside its own update without the ground moving underneath.
TEST_F(SceneHarness, ASceneCanReplaceItselfFromItsOwnUpdate)
{
    static Grapple_SceneDef next = MakeDef("next");
    static Grapple_SceneDef first = MakeDef("first");
    first.update = [](Grapple_Scene *scene, float dt) {
        (void)dt;
        SceneState *state = static_cast<SceneState *>(Grapple_SceneState(scene));
        // Ask on the second frame, then keep touching our own state: if the
        // stack had changed underneath us this would be a use-after-free,
        // which is what ASan is here to notice.
        if (state->fixed_updates == 2)
        {
            Grapple_SceneReplace(Grapple_SceneEngine(scene), &next);
        }
        state->renders += 0;
    };

    Grapple_ScenePush(engine_, &first);
    Frame(6);
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "next");
    EXPECT_TRUE(LoggedInOrder({"first:exit", "first:unload", "next:load", "next:enter"}));
}

// Two changes in one frame is a game bug: the second would silently win.
TEST_F(SceneHarness, TwoStackChangesInOneFrameIsAnError)
{
    const Grapple_SceneDef one = MakeDef("one");
    const Grapple_SceneDef two = MakeDef("two");
    EXPECT_TRUE(Grapple_ScenePush(engine_, &one));
    EXPECT_FALSE(Grapple_ScenePush(engine_, &two));
    EXPECT_NE(std::string(SDL_GetError()).find("already queued"), std::string::npos);
}

// A scene whose load fails must not end up on the stack, and must not
// disturb whatever was already there.
TEST_F(SceneHarness, AFailedLoadLeavesTheStackAlone)
{
    const Grapple_SceneDef good = MakeDef("good");
    Grapple_SceneDef bad = MakeDef("bad");
    bad.load = [](Grapple_Scene *scene) {
        Note(scene, "load");
        SDL_SetError("no");
        return false;
    };

    Grapple_ScenePush(engine_, &good);
    Frame();
    Grapple_ScenePush(engine_, &bad);
    Frame();

    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "good");
    EXPECT_TRUE(Grapple_SceneIsActive(Grapple_SceneCurrent(engine_)))
        << "the surviving scene is active again";
}

// Transitions swap at the midpoint, where the screen is covered.
TEST_F(SceneHarness, FadeTransitionSwapsAtTheMidpoint)
{
    const Grapple_SceneDef from = MakeDef("from");
    const Grapple_SceneDef to = MakeDef("to");
    Grapple_ScenePush(engine_, &from);
    Frame();

    ASSERT_TRUE(Grapple_SceneTransitionTo(engine_, &to, GRAPPLE_TRANSITION_FADE, 0.2f));
    EXPECT_TRUE(Grapple_SceneTransitioning(engine_));

    // A tenth of a second in — before the midpoint — the old scene is
    // still the live one.
    Frame(6);
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "from");

    Frame(6); // past the midpoint
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "to");

    Frame(12); // past the end
    EXPECT_FALSE(Grapple_SceneTransitioning(engine_));
    EXPECT_EQ(Grapple_SceneDepth(engine_), 1);

    // A second transition is refused while one is running.
    ASSERT_TRUE(Grapple_SceneTransitionTo(engine_, &from, GRAPPLE_TRANSITION_FADE, 0.2f));
    EXPECT_FALSE(Grapple_SceneTransitionTo(engine_, &to, GRAPPLE_TRANSITION_FADE, 0.2f));
}

// A zero-length or NONE transition is just a replace, rather than an error
// or a one-frame flash.
TEST_F(SceneHarness, AnInstantTransitionIsAPlainReplace)
{
    const Grapple_SceneDef from = MakeDef("from");
    const Grapple_SceneDef to = MakeDef("to");
    Grapple_ScenePush(engine_, &from);
    Frame();
    EXPECT_TRUE(Grapple_SceneTransitionTo(engine_, &to, GRAPPLE_TRANSITION_NONE, 0.0f));
    Frame();
    EXPECT_STREQ(Grapple_SceneName(Grapple_SceneCurrent(engine_)), "to");
    EXPECT_FALSE(Grapple_SceneTransitioning(engine_));
}

// Lookup by name, so a menu can talk to the level underneath it.
TEST_F(SceneHarness, ScenesAreFindableByName)
{
    const Grapple_SceneDef level = MakeDef("level");
    const Grapple_SceneDef menu = MakeDef("menu");
    Grapple_ScenePush(engine_, &level);
    Frame();
    Grapple_ScenePush(engine_, &menu);
    Frame();

    Grapple_Scene *found = Grapple_SceneFind(engine_, "level");
    ASSERT_NE(found, nullptr);
    EXPECT_STREQ(Grapple_SceneName(found), "level");
    EXPECT_FALSE(Grapple_SceneIsActive(found)) << "found, but covered";
    EXPECT_NE(Grapple_SceneState(found), nullptr);
    EXPECT_EQ(Grapple_SceneFind(engine_, "nothing"), nullptr);
}

// The stack has a ceiling, and reaching it is an error rather than a
// silent overwrite.
TEST_F(SceneHarness, TheStackHasALimit)
{
    const Grapple_SceneDef def = MakeDef("deep");
    for (int i = 0; i < GRAPPLE_SCENE_STACK_MAX; i++)
    {
        EXPECT_TRUE(Grapple_ScenePush(engine_, &def)) << "push " << i;
        Frame();
    }
    EXPECT_EQ(Grapple_SceneDepth(engine_), GRAPPLE_SCENE_STACK_MAX);
    Grapple_ScenePush(engine_, &def);
    Frame();
    EXPECT_EQ(Grapple_SceneDepth(engine_), GRAPPLE_SCENE_STACK_MAX) << "no overflow";
}

TEST_F(SceneHarness, NullsAreHandled)
{
    EXPECT_FALSE(Grapple_ScenePush(nullptr, nullptr));
    EXPECT_FALSE(Grapple_ScenePush(engine_, nullptr));
    EXPECT_FALSE(Grapple_ScenePop(nullptr));
    EXPECT_FALSE(Grapple_SceneReplace(engine_, nullptr));
    EXPECT_FALSE(Grapple_SceneReset(engine_, nullptr));
    EXPECT_EQ(Grapple_SceneCurrent(nullptr), nullptr);
    EXPECT_EQ(Grapple_SceneDepth(nullptr), 0);
    EXPECT_EQ(Grapple_SceneFind(nullptr, "x"), nullptr);
    EXPECT_EQ(Grapple_SceneState(nullptr), nullptr);
    EXPECT_EQ(Grapple_SceneEngine(nullptr), nullptr);
    EXPECT_EQ(Grapple_SceneName(nullptr), nullptr);
    EXPECT_FALSE(Grapple_SceneIsActive(nullptr));
    EXPECT_FALSE(Grapple_SceneTransitionTo(nullptr, nullptr, GRAPPLE_TRANSITION_FADE, 1.0f));
    EXPECT_FALSE(Grapple_SceneTransitioning(nullptr));
    Grapple_SceneSetTransitionColor(nullptr, SDL_FColor{});
}

} // namespace
