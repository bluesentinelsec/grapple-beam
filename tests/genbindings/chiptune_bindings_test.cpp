/** @file chiptune_bindings_test.cpp @brief Composition and player ownership across language
 * boundaries. */
#include <SDL3/SDL.h>
#include <grapple/bindings.h>
#include <grapple/chiptune.h>
#include <grapple/gen/grapple.h>
#include <grapple/lua.h>
#include <grapple/ruby.h>
#include <gtest/gtest.h>
#include <memory>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <vector>

namespace
{
class ChipBindings : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_AUDIO));
        ASSERT_TRUE(MIX_Init());
    }
    static void TearDownTestSuite()
    {
        MIX_Quit();
        SDL_Quit();
        SDL_ResetHint(SDL_HINT_AUDIO_DRIVER);
    }
};

TEST_F(ChipBindings, CppComposesDeclarativeNotesWithRaii)
{
    auto composer = grapple::ext::ChipComposer::CreateChipComposer(1, 480);
    ASSERT_TRUE(composer.ok());
    ASSERT_TRUE(composer->SetChipPart(0, "harmony", GRAPPLE_CHIP_PRESET_HARMONY, 1).ok());
    const Grapple_ChipNote notes[] = {
        {0, 48, 80, 0, 1920}, {0, 52, 80, 0, 1920}, {0, 55, 80, 0, 1920}};
    for (const auto &note : notes)
        ASSERT_TRUE(composer->AddChipNote(&note).ok());
    auto song = grapple::ext::ChipSong::BuildChipSong(composer->get(), 1920);
    ASSERT_TRUE(song.ok());
    auto player = grapple::ext::ChipPlayer::CreateChipPlayer(song->get(), 8000, 64, false);
    ASSERT_TRUE(player.ok());
    composer->reset();
    song->reset();
    std::vector<float> pcm(16000);
    EXPECT_EQ(player->RenderChipPlayer(pcm.data(), 8000), 8000);
    EXPECT_EQ(player->GetChipPlayerPeakVoices(), 3);
}

TEST_F(ChipBindings, LuaOwnsComposerSongAndManagedPlayback)
{
    std::unique_ptr<lua_State, decltype(&lua_close)> state(Grapple_CreateLuaState(), lua_close);
    ASSERT_TRUE(state);
    ASSERT_TRUE(Grapple_OpenLuaBindings(state.get()));
    const char *script = R"lua(
local G = GrappleC
local composer = assert(G.CreateChipComposer(1, 480))
assert(G.SetChipPart(composer, 0, 'harmony', G.GRAPPLE_CHIP_PRESET_HARMONY, 1))
local notes = {
    {track=0, note=48, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=52, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=55, velocity=80, start_tick=0, duration_ticks=1920},
}
for _, note in ipairs(notes) do assert(G.AddChipNote(composer, note)) end
local song = assert(G.BuildChipSong(composer, 1920))
local ok, info = G.ReadChipSongInfo(song)
assert(ok and info.duration_seconds == 2)
local player = assert(G.CreateChipPlayer(song, 8000, 64, true))
G.DestroyChipComposer(composer)
G.DestroyChipSong(song)
composer, song = nil, nil
collectgarbage('collect')
local ok, effects = G.GetChipPresetEffects(G.GRAPPLE_CHIP_PRESET_HARMONY)
assert(ok and effects.pulse_beats == 0.5)
effects.pulse_beats = 1
assert(G.SetChipPresetEffects(player, G.GRAPPLE_CHIP_PRESET_HARMONY, effects))
assert(G.PlayChipPlayer(player))
assert(G.ChipPlayerPlaying(player))
G.PauseChipPlayer(player)
assert(not G.ChipPlayerPlaying(player))
assert(G.PlayChipPlayer(player))
G.StopChipPlayer(player)
assert(not G.ChipPlayerPlaying(player))
assert(G.PlayChipPlayer(player))
player = nil
collectgarbage('collect')
assert(G.RenderChipPlayer == nil and G.GetChipPlayerStream == nil)
)lua";
    ASSERT_EQ(luaL_dostring(state.get(), script), LUA_OK) << lua_tostring(state.get(), -1);
}

TEST_F(ChipBindings, RubyOwnsComposerSongAndManagedPlayback)
{
    std::unique_ptr<mrb_state, decltype(&mrb_close)> state(Grapple_CreateRubyState(), mrb_close);
    ASSERT_TRUE(state);
    ASSERT_TRUE(Grapple_OpenRubyBindings(state.get()));
    const char *script = R"ruby(
g = GrappleC
composer = g.CreateChipComposer(1, 480)
raise 'composer' unless composer
raise 'part' unless g.SetChipPart(composer, 0, 'harmony', g::GRAPPLE_CHIP_PRESET_HARMONY, 1)
notes = [48, 52, 55].map { |pitch| {track: 0, note: pitch, velocity: 80, start_tick: 0, duration_ticks: 1920} }
notes.each { |note| raise 'note' unless g.AddChipNote(composer, note) }
song = g.BuildChipSong(composer, 1920)
ok, info = g.ReadChipSongInfo(song)
raise 'duration' unless ok && info[:duration_seconds] == 2
player = g.CreateChipPlayer(song, 8000, 64, true)
g.DestroyChipComposer(composer)
g.DestroyChipSong(song)
composer = song = nil
GC.start
ok, effects = g.GetChipPresetEffects(g::GRAPPLE_CHIP_PRESET_HARMONY)
raise 'defaults' unless ok && effects[:pulse_beats] == 0.5
effects[:pulse_beats] = 1
raise 'effects' unless g.SetChipPresetEffects(player, g::GRAPPLE_CHIP_PRESET_HARMONY, effects)
raise 'play' unless g.PlayChipPlayer(player)
raise 'playing' unless g.ChipPlayerPlaying(player)
g.PauseChipPlayer(player)
raise 'pause' if g.ChipPlayerPlaying(player)
raise 'resume' unless g.PlayChipPlayer(player)
g.StopChipPlayer(player)
raise 'stop' if g.ChipPlayerPlaying(player)
raise 'restart' unless g.PlayChipPlayer(player)
player = nil
GC.start
)ruby";
    mrb_load_string(state.get(), script);
    if (state->exc)
    {
        const mrb_value message = mrb_inspect(state.get(), mrb_obj_value(state->exc));
        FAIL() << RSTRING_CSTR(state.get(), message);
    }
}
} // namespace
