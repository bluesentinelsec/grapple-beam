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
#include <mruby/variable.h>
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
    Grapple_ChipDiagnostic error;
    auto imported = grapple::ext::ChipSong::LoadChipSongEx(CHIP_XML_FIXTURE, nullptr, &error);
    ASSERT_TRUE(imported.ok());
    EXPECT_EQ(imported->GetChipDiagnosticCount(), 3);
    auto composer = grapple::ext::ChipComposer::CreateChipComposer(1, 480);
    ASSERT_TRUE(composer.ok());
    ASSERT_TRUE(composer->SetChipPart(0, "harmony", GRAPPLE_CHIP_PRESET_HARMONY, 1).ok());
    const Grapple_ChipNote notes[] = {
        {0, 48, 80, 0, 1920}, {0, 52, 80, 0, 1920}, {0, 55, 80, 0, 1920}};
    Grapple_ChipExpression expression;
    grapple::ext::GetChipExpressionDefaults(&expression);
    expression.vibrato_depth = 0.1f;
    for (const auto &note : notes)
        ASSERT_TRUE(composer->AddChipNoteEx(&note, &expression).ok());
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
    lua_pushstring(state.get(), CHIP_XML_FIXTURE);
    lua_setglobal(state.get(), "chip_xml_fixture");
    const char *script = R"lua(
local G = GrappleC
local ok, policy = G.GetChipImportDefaults()
assert(ok and policy.strict)
local imported, diagnostic = G.LoadChipSongEx(chip_xml_fixture, policy)
assert(imported and diagnostic.code == G.GRAPPLE_CHIP_DIAGNOSTIC_NONE)
assert(G.GetChipDiagnosticCount(imported) == 3)
local ok, diagnostic = G.ReadChipDiagnostic(imported, 0)
assert(ok and diagnostic.code == G.GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR)
assert(G.GetChipDiagnosticMessage(imported, 0):find('TAB'))
G.DestroyChipSong(imported)
local composer = assert(G.CreateChipComposer(1, 480))
assert(G.SetChipPart(composer, 0, 'harmony', G.GRAPPLE_CHIP_PRESET_HARMONY, 1))
local notes = {
    {track=0, note=48, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=52, velocity=80, start_tick=0, duration_ticks=1920},
    {track=0, note=55, velocity=80, start_tick=0, duration_ticks=1920},
}
local expression = G.GetChipExpressionDefaults()
assert(expression.gain == 1)
expression.vibrato_depth = 0.1
for _, note in ipairs(notes) do assert(G.AddChipNoteEx(composer, note, expression)) end
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
    mrb_gv_set(state.get(), mrb_intern_lit(state.get(), "$chip_xml_fixture"),
               mrb_str_new_cstr(state.get(), CHIP_XML_FIXTURE));
    const char *script = R"ruby(
g = GrappleC
ok, policy = g.GetChipImportDefaults()
raise 'policy' unless ok && policy[:strict]
imported, diagnostic = g.LoadChipSongEx($chip_xml_fixture, policy)
raise 'import' unless imported && diagnostic[:code] == g::GRAPPLE_CHIP_DIAGNOSTIC_NONE
raise 'diagnostics' unless g.GetChipDiagnosticCount(imported) == 3
ok, diagnostic = g.ReadChipDiagnostic(imported, 0)
raise 'diagnostic code' unless ok && diagnostic[:code] == g::GRAPPLE_CHIP_DIAGNOSTIC_STAFF_MIRROR
raise 'message' unless g.GetChipDiagnosticMessage(imported, 0).include?('TAB')
g.DestroyChipSong(imported)
composer = g.CreateChipComposer(1, 480)
raise 'composer' unless composer
raise 'part' unless g.SetChipPart(composer, 0, 'harmony', g::GRAPPLE_CHIP_PRESET_HARMONY, 1)
notes = [48, 52, 55].map { |pitch| {track: 0, note: pitch, velocity: 80, start_tick: 0, duration_ticks: 1920} }
expression = g.GetChipExpressionDefaults()
raise 'expression' unless expression[:gain] == 1
expression[:vibrato_depth] = 0.1
notes.each { |note| raise 'note' unless g.AddChipNoteEx(composer, note, expression) }
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
