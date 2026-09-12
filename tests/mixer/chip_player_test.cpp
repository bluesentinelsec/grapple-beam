/** @file chip_player_test.cpp @brief Streaming synthesis and exact MIDI timing tests. */
#include "chip_effects.h"
#include "chip_internal.h"
#include "chip_voice.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <grapple/chiptune.h>
#include <gtest/gtest.h>
#include <limits>
#include <memory>
#include <vector>

namespace
{
using Bytes = std::vector<Uint8>;
using Song = std::unique_ptr<Grapple_ChipSong, decltype(&Grapple_DestroyChipSong)>;
using Player = std::unique_ptr<Grapple_ChipPlayer, decltype(&Grapple_DestroyChipPlayer)>;

void Big(Bytes &bytes, Uint32 value, int count)
{
    for (int i = count - 1; i >= 0; --i)
        bytes.push_back(static_cast<Uint8>((value >> (i * 8)) & 255u));
}

void Variable(Bytes &bytes, Uint32 value)
{
    Uint8 digits[4];
    int count = 0;
    digits[count++] = static_cast<Uint8>(value & 127u);
    while ((value >>= 7) != 0)
        digits[count++] = static_cast<Uint8>((value & 127u) | 128u);
    while (count)
        bytes.push_back(digits[--count]);
}

void Event(Bytes &track, Uint32 delta, std::initializer_list<Uint8> data)
{
    Variable(track, delta);
    track.insert(track.end(), data);
}

Bytes Midi(std::vector<Bytes> tracks, int ppqn = 480, int format = 1)
{
    Bytes bytes = {'M', 'T', 'h', 'd'};
    Big(bytes, 6, 4);
    Big(bytes, static_cast<Uint32>(format), 2);
    Big(bytes, static_cast<Uint32>(tracks.size()), 2);
    Big(bytes, static_cast<Uint32>(ppqn), 2);
    for (auto &track : tracks)
    {
        bytes.insert(bytes.end(), {'M', 'T', 'r', 'k'});
        Big(bytes, static_cast<Uint32>(track.size()), 4);
        bytes.insert(bytes.end(), track.begin(), track.end());
    }
    return bytes;
}

Song Load(const Bytes &bytes)
{
    return Song(Grapple_LoadChipSong_IO(SDL_IOFromConstMem(bytes.data(), bytes.size()), true),
                Grapple_DestroyChipSong);
}

Player MakePlayer(const Grapple_ChipSong *song, int voices = 64, bool loop = false,
                  int rate = 48000)
{
    Player player(Grapple_CreateChipPlayer(song, rate, voices, loop), Grapple_DestroyChipPlayer);
    if (player)
    {
        const Grapple_ChipEffects dry = {};
        for (int preset = GRAPPLE_CHIP_PRESET_LEAD; preset <= GRAPPLE_CHIP_PRESET_DRUMS; ++preset)
            EXPECT_TRUE(Grapple_SetChipPresetEffects(
                player.get(), static_cast<Grapple_ChipPreset>(preset), &dry));
    }
    return player;
}

Song Example()
{
    return Song(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition.mid"),
                Grapple_DestroyChipSong);
}

Song SustainedNote()
{
    Bytes track;
    Event(track, 0, {0x90, 60, 100});
    Event(track, 960, {0x80, 60, 0});
    Event(track, 0, {0xff, 0x2f, 0});
    return Load(Midi({track}, 480, 0));
}

std::vector<float> Render(Grapple_ChipPlayer *player, int frames)
{
    std::vector<float> pcm(static_cast<size_t>(frames) * 2);
    EXPECT_GE(Grapple_RenderChipPlayer(player, pcm.data(), frames), 0);
    return pcm;
}

double Energy(const std::vector<float> &pcm)
{
    double energy = 0;
    for (float sample : pcm)
        energy += static_cast<double>(sample) * sample;
    return energy;
}

class ChipPlayer : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        ASSERT_TRUE(SDL_Init(0));
        ASSERT_TRUE(MIX_Init());
    }
    static void TearDownTestSuite()
    {
        MIX_Quit();
        SDL_Quit();
    }
};

TEST_F(ChipPlayer, ExampleRetainsEveryInstrumentAndChordNote)
{
    const auto song = Example();
    ASSERT_TRUE(song) << SDL_GetError();
    const auto *info = Grapple_GetChipSongInfo(song.get());
    EXPECT_EQ(info->track_count, 5);
    EXPECT_EQ(info->ticks_per_quarter, 480);
    EXPECT_EQ(info->duration_ticks, 9600u);
    EXPECT_DOUBLE_EQ(info->duration_seconds, 10.0);
    const Uint32 notes[] = {0, 20, 15, 20, 32};
    const Uint16 channels[] = {0, 1, 4, 16, 512};
    for (int i = 0; i < 5; ++i)
    {
        const auto *track = Grapple_GetChipTrackInfo(song.get(), i);
        ASSERT_NE(track, nullptr);
        EXPECT_EQ(track->note_count, notes[i]);
        EXPECT_EQ(track->channels, channels[i]);
    }
    EXPECT_STREQ(Grapple_GetChipTrackInfo(song.get(), 2)->name, "Steel Guitar");
    auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    ASSERT_TRUE(Grapple_SetChipTrackPreset(player.get(), 2, GRAPPLE_CHIP_PRESET_RING, 1.0F));
    const auto pcm = Render(player.get(), 48000 * 11);
    EXPECT_GT(Energy(pcm), 100.0);
    EXPECT_GE(Grapple_GetChipPlayerPeakVoices(player.get()), 8);
    for (float sample : pcm)
    {
        ASSERT_TRUE(std::isfinite(sample));
        ASSERT_LT(std::abs(sample), 1.0F);
    }
    EXPECT_EQ(Energy(Render(player.get(), 1024)), 0.0);
    // Every part remains audible when isolated, including the three-note ring harmony.
    for (int solo = 1; solo < 5; ++solo)
    {
        Grapple_ResetChipPlayer(player.get());
        for (int i = 0; i < 5; ++i)
            ASSERT_TRUE(Grapple_SetChipTrackPreset(
                player.get(), i, i == 2 ? GRAPPLE_CHIP_PRESET_RING : GRAPPLE_CHIP_PRESET_AUTO,
                i == solo ? 1.0F : 0.0F));
        EXPECT_GT(Energy(Render(player.get(), 48000)), 1.0) << "part " << solo;
    }
}

TEST_F(ChipPlayer, RoleWordsMapMidiNamesBeforeChannelAndProgramDefaults)
{
    const struct
    {
        const char *name;
        Uint8 channel;
        Uint8 program;
        Grapple_ChipPreset expected;
    } cases[] = {{"Main MeLoDy", 0, 33, GRAPPLE_CHIP_PRESET_LEAD},
                 {"[LEAD] Guitar", 9, 0, GRAPPLE_CHIP_PRESET_LEAD},
                 {"Guitar: HaRmOnY", 0, 25, GRAPPLE_CHIP_PRESET_HARMONY},
                 {"CHORDS-2", 0, 89, GRAPPLE_CHIP_PRESET_HARMONY},
                 {"Bass_2", 0, 0, GRAPPLE_CHIP_PRESET_BASS},
                 {"Percussion", 0, 0, GRAPPLE_CHIP_PRESET_DRUMS},
                 {"PERCUSSIONS (aux)", 0, 0, GRAPPLE_CHIP_PRESET_DRUMS},
                 {"DrUmS", 0, 0, GRAPPLE_CHIP_PRESET_DRUMS},
                 {"Bass Harmony", 0, 0, GRAPPLE_CHIP_PRESET_BASS},
                 {"Harmony Bass", 0, 0, GRAPPLE_CHIP_PRESET_HARMONY},
                 {"Bassoon", 0, 0, GRAPPLE_CHIP_PRESET_LEAD},
                 {"Lead2", 0, 33, GRAPPLE_CHIP_PRESET_BASS},
                 {"Disharmony", 0, 89, GRAPPLE_CHIP_PRESET_CHORD},
                 {"Steel Guitar", 0, 25, GRAPPLE_CHIP_PRESET_LEAD},
                 {"Kit", 9, 0, GRAPPLE_CHIP_PRESET_DRUMS},
                 {"", 0, 0, GRAPPLE_CHIP_PRESET_LEAD}};
    for (const auto &entry : cases)
    {
        SCOPED_TRACE(entry.name);
        Bytes track;
        const auto on = static_cast<Uint8>(0x90 | entry.channel);
        Event(track, 0, {on, 60, 100});
        Event(track, 0, {0xff, 3});
        const auto length = SDL_strlen(entry.name);
        Variable(track, static_cast<Uint32>(length));
        track.insert(track.end(), entry.name, entry.name + length);
        Event(track, 0, {static_cast<Uint8>(0xc0 | entry.channel), entry.program});
        Event(track, 480, {static_cast<Uint8>(0x80 | entry.channel), 60, 0});
        Event(track, 0, {0xff, 0x2f, 0});
        const auto song = Load(Midi({track}));
        ASSERT_TRUE(song) << SDL_GetError();
        Player automatic(Grapple_CreateChipPlayer(song.get(), 8000, 8, false),
                         Grapple_DestroyChipPlayer);
        Player explicit_player(Grapple_CreateChipPlayer(song.get(), 8000, 8, false),
                               Grapple_DestroyChipPlayer);
        ASSERT_TRUE(automatic);
        ASSERT_TRUE(explicit_player);
        ASSERT_TRUE(Grapple_SetChipTrackPreset(explicit_player.get(), 0, entry.expected, 1));
        const auto pcm = Render(automatic.get(), 8000);
        EXPECT_GT(Energy(pcm), 0);
        EXPECT_EQ(pcm, Render(explicit_player.get(), 8000));
    }
}

TEST_F(ChipPlayer, ExplicitPresetOverridesNameAndAutoRestoresIt)
{
    const std::unique_ptr<Grapple_ChipComposer, decltype(&Grapple_DestroyChipComposer)> composer(
        Grapple_CreateChipComposer(2, 480), Grapple_DestroyChipComposer);
    ASSERT_TRUE(composer);
    for (int i = 0; i < 2; ++i)
    {
        ASSERT_TRUE(Grapple_SetChipPart(composer.get(), i, "Harmony", GRAPPLE_CHIP_PRESET_AUTO, 1));
        const Grapple_ChipNote note = {i, 60 + i * 4, 100, 0, 960};
        ASSERT_TRUE(Grapple_AddChipNote(composer.get(), &note));
    }
    const Song song(Grapple_BuildChipSong(composer.get(), 960), Grapple_DestroyChipSong);
    ASSERT_TRUE(song);
    const auto player = MakePlayer(song.get());
    const auto reference = MakePlayer(song.get());
    ASSERT_TRUE(player);
    ASSERT_TRUE(reference);
    const auto harmony = Render(reference.get(), 4096);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(reference.get()), 2);
    ASSERT_TRUE(Grapple_SetChipTrackPreset(player.get(), 0, GRAPPLE_CHIP_PRESET_BASS, 1));
    ASSERT_TRUE(Grapple_SetChipTrackPreset(reference.get(), 0, GRAPPLE_CHIP_PRESET_BASS, 1));
    Grapple_ResetChipPlayer(reference.get());
    const auto bass = Render(player.get(), 4096);
    EXPECT_NE(bass, harmony);
    EXPECT_EQ(bass, Render(reference.get(), 4096));
    ASSERT_TRUE(Grapple_SetChipTrackPreset(player.get(), 0, GRAPPLE_CHIP_PRESET_AUTO, 1));
    Grapple_ResetChipPlayer(player.get());
    EXPECT_EQ(harmony, Render(player.get(), 4096));
}

TEST_F(ChipPlayer, NamedGuitarProExportMatchesExplicitMappingWithWetEffects)
{
    const Song song(Grapple_LoadChipSong(MIXER_TEST_ASSETS_DIR "/c64-composition-named.mid"),
                    Grapple_DestroyChipSong);
    ASSERT_TRUE(song) << SDL_GetError();
    const auto *info = Grapple_GetChipSongInfo(song.get());
    ASSERT_EQ(info->track_count, 5);
    const char *names[] = {"melody", "harmony", "bass", "percussion"};
    const Grapple_ChipPreset presets[] = {GRAPPLE_CHIP_PRESET_LEAD, GRAPPLE_CHIP_PRESET_HARMONY,
                                          GRAPPLE_CHIP_PRESET_BASS, GRAPPLE_CHIP_PRESET_DRUMS};
    Player automatic(Grapple_CreateChipPlayer(song.get(), 8000, 64, false),
                     Grapple_DestroyChipPlayer);
    Player explicit_player(Grapple_CreateChipPlayer(song.get(), 8000, 64, false),
                           Grapple_DestroyChipPlayer);
    ASSERT_TRUE(automatic);
    ASSERT_TRUE(explicit_player);
    Uint32 notes = 0;
    for (int i = 1; i < 5; ++i)
    {
        const auto *track = Grapple_GetChipTrackInfo(song.get(), i);
        EXPECT_STREQ(track->name, names[i - 1]);
        notes += track->note_count;
        ASSERT_TRUE(Grapple_SetChipTrackPreset(explicit_player.get(), i, presets[i - 1], 1));
    }
    EXPECT_EQ(notes, 87u);
    const auto pcm = Render(automatic.get(), 8000 * 14);
    EXPECT_GT(Energy(pcm), 1);
    EXPECT_EQ(pcm, Render(explicit_player.get(), 8000 * 14));
    EXPECT_GE(Grapple_GetChipPlayerPeakVoices(automatic.get()), 8);
}

TEST_F(ChipPlayer, TripletsTempoChangesAndLeadingRestsHaveExactSamplePositions)
{
    Bytes conductor;
    Event(conductor, 0, {0xff, 0x51, 3, 0x07, 0xa1, 0x20});   // 120 BPM
    Event(conductor, 480, {0xff, 0x51, 3, 0x0f, 0x42, 0x40}); // 60 BPM
    Event(conductor, 480, {0xff, 0x2f, 0});
    Bytes track;
    Event(track, 160, {0x90, 60, 100});
    Event(track, 160, {60, 0}); // Running status, note-on zero means note-off.
    Event(track, 160, {64, 100});
    Event(track, 160, {64, 0});
    Event(track, 160, {67, 100});
    Event(track, 160, {67, 0});
    Event(track, 0, {0xff, 0x2f, 0});
    const auto song = Load(Midi({conductor, track}));
    ASSERT_TRUE(song) << SDL_GetError();
    EXPECT_DOUBLE_EQ(song->info.duration_seconds, 1.5);
    std::vector<Uint64> frames;
    for (size_t i = 0; i < song->count; ++i)
        if (song->events[i].status == 0x90)
            frames.push_back(Chip_TimeToFrame(song.get(), song->events[i].time, 48000));
    EXPECT_EQ(frames, (std::vector<Uint64>{8000, 16000, 24000, 40000, 56000, 72000}));
    const auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    EXPECT_EQ(Energy(Render(player.get(), 8000)), 0.0);
    EXPECT_GT(Energy(Render(player.get(), 1000)), 0.1);
}

TEST_F(ChipPlayer, FractionalTempoSegmentsDoNotAccumulateRoundingError)
{
    Bytes track;
    for (int i = 0; i < 900; ++i)
        Event(track, 1, {0xff, 0x51, 3, 0x07, 0xa1, 0x21}); // 500001 us/quarter
    Event(track, 0, {0x90, 60, 100});
    Event(track, 100, {0x80, 60, 0});
    Event(track, 0, {0xff, 0x2f, 0});
    auto song = Load(Midi({track}, 960, 0));
    ASSERT_TRUE(song);
    // First tick at default tempo, remaining 899 ticks at 500001 us/quarter.
    const Uint64 expected = (500000u + 899u * 500001u) * 44100ull;
    EXPECT_EQ(Chip_TimeToFrame(song.get(), song->events[900].time, 44100),
              (expected + 480000000u) / 960000000u);
}

TEST_F(ChipPlayer, RenderingIsIndependentOfBlockSizeAndResetReproducesAudio)
{
    const auto song = Example();
    ASSERT_TRUE(song);
    auto whole = MakePlayer(song.get());
    auto blocks = MakePlayer(song.get());
    ASSERT_TRUE(whole && blocks);
    const auto expected = Render(whole.get(), 100000);
    std::vector<float> actual;
    for (int done = 0; done < 100000;)
    {
        const int count = std::min(137, 100000 - done);
        const auto block = Render(blocks.get(), count);
        actual.insert(actual.end(), block.begin(), block.end());
        done += count;
    }
    EXPECT_EQ(actual, expected);
    Grapple_ResetChipPlayer(whole.get());
    EXPECT_EQ(Render(whole.get(), 100000), expected);
}

TEST_F(ChipPlayer, MoreThanFivePartsAndSixtyFourNotesAreRepresented)
{
    std::vector<Bytes> tracks;
    for (int channel = 0; channel < 8; ++channel)
    {
        Bytes track;
        for (int note = 48; note < 56; ++note)
            Event(track, 0, {static_cast<Uint8>(0x90 | channel), static_cast<Uint8>(note), 64});
        Event(track, 480, {0xff, 0x2f, 0});
        tracks.push_back(track);
    }
    const auto song = Load(Midi(tracks));
    ASSERT_TRUE(song);
    const auto player = MakePlayer(song.get(), 64);
    ASSERT_TRUE(player);
    EXPECT_GT(Energy(Render(player.get(), 4000)), 1.0);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(player.get()), 64);
    const auto limited = MakePlayer(song.get(), 5);
    ASSERT_TRUE(limited);
    EXPECT_GT(Energy(Render(limited.get(), 4000)), 1.0);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(limited.get()), 5);
}

TEST_F(ChipPlayer, RingModulationUsesPrivateModulatorsAndChangesSpectrum)
{
    const auto song = SustainedNote();
    ASSERT_TRUE(song);
    const auto ring = MakePlayer(song.get(), 1);
    const auto plain = MakePlayer(song.get(), 1);
    ASSERT_TRUE(ring && plain);
    ASSERT_TRUE(Grapple_SetChipTrackPreset(ring.get(), 0, GRAPPLE_CHIP_PRESET_RING, 1.0F));
    ASSERT_TRUE(Grapple_SetChipTrackPreset(plain.get(), 0, GRAPPLE_CHIP_PRESET_CHORD, 1.0F));
    const auto modulated = Render(ring.get(), 24000);
    const auto normal = Render(plain.get(), 24000);
    EXPECT_GT(Energy(modulated), 1.0);
    EXPECT_NE(modulated, normal);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(ring.get()), 1);
    // Polarity switching adds a strong third harmonic to the triangle carrier.
    ChipSynthVoice voice;
    Chip_VoiceStart(&voice, GRAPPLE_CHIP_PRESET_RING, 69, 127, 48000);
    double real1 = 0, imag1 = 0, real3 = 0, imag3 = 0;
    for (int i = 0; i < 48000; ++i)
    {
        const double value = Chip_VoiceSample(&voice, 1.0F, 0.0F, 48000);
        if (i < 24000)
            continue;
        const double phase = 2.0 * 3.14159265358979323846 * 440.0 * i / 48000.0;
        real1 += value * std::cos(phase);
        imag1 += value * std::sin(phase);
        real3 += value * std::cos(phase * 3.0);
        imag3 += value * std::sin(phase * 3.0);
    }
    EXPECT_GT(std::hypot(real3, imag3), std::hypot(real1, imag1) * 0.25);
}

TEST_F(ChipPlayer, LoopUsesMusicalDurationAndDoesNotLeakSustainOrVoices)
{
    Bytes track;
    Event(track, 0, {0xb0, 64, 127});
    Event(track, 0, {0x90, 60, 100});
    Event(track, 160, {0x80, 60, 0});
    Event(track, 320, {0xff, 0x2f, 0});
    const auto song = Load(Midi({track}, 480, 0));
    ASSERT_TRUE(song);
    const auto player = MakePlayer(song.get(), 32, true);
    ASSERT_TRUE(player);
    const auto first = Render(player.get(), 24000);
    const auto second = Render(player.get(), 24000);
    EXPECT_EQ(first, second);
    EXPECT_EQ(Grapple_GetChipPlayerPeakVoices(player.get()), 1);
}

TEST_F(ChipPlayer, MixerDemandStreamMatchesDirectRendererAndStops)
{
    const auto song = Example();
    ASSERT_TRUE(song);
    const auto direct = MakePlayer(song.get());
    const auto streamed = MakePlayer(song.get());
    ASSERT_TRUE(direct && streamed);
    SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, 48000};
    std::unique_ptr<MIX_Mixer, decltype(&MIX_DestroyMixer)> mixer(MIX_CreateMixer(&spec),
                                                                  MIX_DestroyMixer);
    ASSERT_TRUE(mixer);
    MIX_Track *track = MIX_CreateTrack(mixer.get());
    ASSERT_NE(track, nullptr);
    ASSERT_TRUE(MIX_SetTrackAudioStream(track, Grapple_GetChipPlayerStream(streamed.get())));
    ASSERT_TRUE(MIX_PlayTrack(track, 0));
    const auto expected = Render(direct.get(), 48000 * 11);
    std::vector<float> actual(expected.size());
    const int got =
        MIX_Generate(mixer.get(), actual.data(), static_cast<int>(actual.size() * sizeof(float)));
    ASSERT_GT(got, 0);
    EXPECT_LE(got, static_cast<int>(actual.size() * sizeof(float)));
    EXPECT_EQ(actual, expected);
    EXPECT_FALSE(MIX_TrackPlaying(track));
    Grapple_ResetChipPlayer(streamed.get());
    ASSERT_TRUE(MIX_PlayTrack(track, 0));
    EXPECT_GT(MIX_Generate(mixer.get(), actual.data(), 8192), 0);
}

TEST_F(ChipPlayer, SustainVolumePanAndAllSoundOffAreApplied)
{
    Bytes track;
    Event(track, 0, {0xb0, 10, 0}); // Hard left.
    Event(track, 0, {0xb0, 64, 127});
    Event(track, 0, {0x90, 60, 100});
    Event(track, 160, {0x80, 60, 0});
    Event(track, 320, {0xb0, 64, 0});
    Event(track, 480, {0x90, 60, 100});
    Event(track, 160, {0xb0, 120, 0});
    Event(track, 320, {0xff, 0x2f, 0});
    const auto song = Load(Midi({track}, 480, 0));
    ASSERT_TRUE(song);
    const auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    auto first = Render(player.get(), 24000);
    EXPECT_GT(Energy(first), 1.0);
    for (size_t i = 1; i < first.size(); i += 2)
        EXPECT_EQ(first[i], 0.0F);
    const auto release = Render(player.get(), 24000);
    EXPECT_EQ(Energy(std::vector<float>(release.end() - 1000, release.end())), 0.0);
    EXPECT_GT(Energy(Render(player.get(), 8000)), 1.0);
    EXPECT_EQ(Energy(Render(player.get(), 16000)), 0.0);
}

TEST_F(ChipPlayer, GuitarProRpnPitchBendSensitivityIsHonored)
{
    Bytes track;
    Event(track, 0, {0xb0, 101, 0});
    Event(track, 0, {0xb0, 100, 0});
    Event(track, 0, {0xb0, 6, 12});
    Event(track, 0, {0xe0, 0, 96}); // Half upward bend: +6 semitones.
    Event(track, 0, {0x90, 60, 100});
    Event(track, 960, {0xff, 0x2f, 0});
    const auto bent_song = Load(Midi({track}, 480, 0));
    Bytes reference;
    Event(reference, 0, {0x90, 66, 100});
    Event(reference, 960, {0xff, 0x2f, 0});
    const auto reference_song = Load(Midi({reference}, 480, 0));
    ASSERT_TRUE(bent_song && reference_song);
    const auto bent = MakePlayer(bent_song.get());
    const auto plain = MakePlayer(reference_song.get());
    ASSERT_TRUE(bent && plain);
    const auto a = Render(bent.get(), 24000);
    const auto b = Render(plain.get(), 24000);
    const auto crossings = [](const std::vector<float> &pcm) {
        int count = 0;
        for (size_t i = 1002; i < pcm.size(); i += 2)
            if (pcm[i - 2] < 0 && pcm[i] >= 0)
                ++count;
        return count;
    };
    EXPECT_NEAR(crossings(a), crossings(b), 1);
    EXPECT_GT(crossings(a), 170);
}

TEST_F(ChipPlayer, RejectsMalformedFilesAndUnsupportedTiming)
{
    Bytes track;
    Event(track, 0, {0x90, 60, 100});
    Event(track, 480, {0x80, 60, 0});
    Event(track, 0, {0xff, 0x2f, 0});
    auto valid = Midi({track}, 480, 0);
    for (size_t size = 0; size < valid.size(); ++size)
        EXPECT_FALSE(Load(Bytes(valid.begin(), valid.begin() + static_cast<std::ptrdiff_t>(size))))
            << size;
    EXPECT_FALSE(Load(Midi({track}, 0, 0)));
    EXPECT_FALSE(Load(Midi({track}, 0xe728, 0)));
    EXPECT_FALSE(Load(Midi({track}, 480, 2)));
    EXPECT_FALSE(Load(Midi({{0, 60, 100, 0, 0xff, 0x2f, 0}}, 480, 0)));
    EXPECT_FALSE(Load(Midi({{0x81, 0x81, 0x81, 0x81, 0, 0xff, 0x2f, 0}}, 480, 0)));
    EXPECT_FALSE(Load(Midi({{0, 0xff, 0x51, 3, 0, 0, 0, 0, 0xff, 0x2f, 0}}, 480, 0)));
    EXPECT_FALSE(Load(Midi({{0, 0xff, 0x21, 1, 1, 0, 0xff, 0x2f, 0}}, 480, 0)));
    EXPECT_FALSE(Load(Midi({{0, 0x90, 200, 100, 0, 0xff, 0x2f, 0}}, 480, 0)));
    Bytes long_track;
    Event(long_track, 0x0fffffffu, {0xff, 0x2f, 0});
    EXPECT_FALSE(Load(Midi({long_track}, 1, 0)));
}

TEST_F(ChipPlayer, MutatedInputsStayBoundedAndPlayableWhenAccepted)
{
    Bytes track;
    Event(track, 0, {0x90, 60, 100});
    Event(track, 480, {0x80, 60, 0});
    Event(track, 0, {0xff, 0x2f, 0});
    const auto valid = Midi({track}, 480, 0);
    for (size_t byte = 0; byte < valid.size(); ++byte)
        for (int bit = 0; bit < 8; ++bit)
        {
            auto mutated = valid;
            mutated[byte] ^= static_cast<Uint8>(1u << bit);
            const auto song = Load(mutated);
            if (!song)
                continue;
            const auto player = MakePlayer(song.get());
            ASSERT_TRUE(player) << SDL_GetError();
            for (float sample : Render(player.get(), 512))
                ASSERT_TRUE(std::isfinite(sample));
        }
}

TEST_F(ChipPlayer, EmptySongAndInvalidPlayerArgumentsAreHandled)
{
    const auto song = Load(Midi({{0, 0xff, 0x2f, 0}}, 480, 0));
    ASSERT_TRUE(song);
    EXPECT_FALSE(MakePlayer(song.get(), 64, true));
    EXPECT_FALSE(MakePlayer(song.get(), 0));
    EXPECT_FALSE(MakePlayer(song.get(), 1025));
    EXPECT_FALSE(MakePlayer(song.get(), 64, false, 0));
    const auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    EXPECT_EQ(Energy(Render(player.get(), 100)), 0.0);
    EXPECT_FALSE(Grapple_SetChipTrackPreset(player.get(), 1, GRAPPLE_CHIP_PRESET_LEAD, 1.0F));
    EXPECT_FALSE(Grapple_SetChipTrackPreset(player.get(), 0, GRAPPLE_CHIP_PRESET_LEAD,
                                            std::numeric_limits<float>::quiet_NaN()));
    EXPECT_EQ(Grapple_RenderChipPlayer(player.get(), nullptr, 2), -1);
    EXPECT_EQ(Grapple_RenderChipPlayer(player.get(), nullptr, 0), 0);
}

TEST_F(ChipPlayer, PresetsRemainFiniteAtSupportedRateExtremes)
{
    for (int rate : {8000, 44100, 48000, 192000})
        for (int note : {0, 60, 127})
            for (auto preset :
                 {GRAPPLE_CHIP_PRESET_LEAD, GRAPPLE_CHIP_PRESET_BASS, GRAPPLE_CHIP_PRESET_CHORD,
                  GRAPPLE_CHIP_PRESET_RING, GRAPPLE_CHIP_PRESET_DRUMS})
            {
                ChipSynthVoice voice;
                Chip_VoiceStart(&voice, preset, note, 127, rate);
                for (int i = 0; i < 4000; ++i)
                    ASSERT_TRUE(std::isfinite(Chip_VoiceSample(&voice, 4.0F, 1.0F, rate)));
            }
}

TEST_F(ChipPlayer, CompositionSnapshotsPreserveTripletsAndSurviveTheirBuilders)
{
    auto *composer = Grapple_CreateChipComposer(2, 480);
    ASSERT_NE(composer, nullptr);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 0, "lead", GRAPPLE_CHIP_PRESET_LEAD, 1));
    ASSERT_TRUE(Grapple_SetChipPart(composer, 1, "harmony", GRAPPLE_CHIP_PRESET_RING, 0.8F));
    const Grapple_ChipNote notes[] = {{0, 60, 90, 0, 160},   {0, 64, 90, 160, 160},
                                      {0, 67, 90, 320, 160}, {1, 48, 80, 0, 1920},
                                      {1, 52, 80, 0, 1920},  {1, 55, 80, 0, 1920}};
    for (const auto &note : notes)
        ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    ASSERT_TRUE(Grapple_AddChipTempo(composer, 480, 60));
    Song song(Grapple_BuildChipSong(composer, 1920), Grapple_DestroyChipSong);
    ASSERT_TRUE(song);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 1, "changed", GRAPPLE_CHIP_PRESET_BASS, 0));
    Grapple_DestroyChipComposer(composer);
    EXPECT_STREQ(Grapple_GetChipTrackInfo(song.get(), 1)->name, "harmony");
    EXPECT_DOUBLE_EQ(Grapple_GetChipSongInfo(song.get())->duration_seconds, 3.5);
    const auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    song.reset(); // The native player retains the immutable snapshot.
    EXPECT_GT(Energy(Render(player.get(), 48000)), 1.0);
    EXPECT_GE(Grapple_GetChipPlayerPeakVoices(player.get()), 4);
}

TEST_F(ChipPlayer, InvalidCompositionEditsDoNotChangeTheSong)
{
    auto *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_NE(composer, nullptr);
    Grapple_ChipNote note = {0, 60, 100, 0, 480};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    note.duration_ticks = 0;
    EXPECT_FALSE(Grapple_AddChipNote(composer, &note));
    note.start_tick = std::numeric_limits<Uint64>::max();
    note.duration_ticks = 1;
    EXPECT_FALSE(Grapple_AddChipNote(composer, &note));
    EXPECT_FALSE(Grapple_AddChipTempo(composer, 0, std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(Grapple_BuildChipSong(composer, 100), nullptr);
    Song song(Grapple_BuildChipSong(composer, 0), Grapple_DestroyChipSong);
    ASSERT_TRUE(song);
    EXPECT_EQ(Grapple_GetChipTrackInfo(song.get(), 0)->note_count, 1u);
    EXPECT_DOUBLE_EQ(Grapple_GetChipSongInfo(song.get())->duration_seconds, 0.5);
    Grapple_DestroyChipComposer(composer);
}

TEST_F(ChipPlayer, HarmonyPulsesEveryEighthAndFollowsTempoChanges)
{
    auto *composer = Grapple_CreateChipComposer(1, 480);
    ASSERT_NE(composer, nullptr);
    ASSERT_TRUE(Grapple_SetChipPart(composer, 0, "harmony", GRAPPLE_CHIP_PRESET_RING, 1));
    const Grapple_ChipNote note = {0, 60, 100, 0, 3840};
    ASSERT_TRUE(Grapple_AddChipNote(composer, &note));
    ASSERT_TRUE(Grapple_AddChipTempo(composer, 1920, 60));
    Song song(Grapple_BuildChipSong(composer, 0), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 48000, 64, false),
                  Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    Grapple_ChipEffects settings = {};
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_RING, &settings));
    EXPECT_DOUBLE_EQ(settings.pulse_beats, 0.5);
    settings.reverb = 0;
    ASSERT_TRUE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_RING, &settings));
    const auto pcm = Render(player.get(), 48000 * 6);
    const auto window = [&pcm](int frame) {
        double sum = 0;
        for (int i = frame; i < frame + 600; ++i)
        {
            const double sample = pcm[static_cast<size_t>(i) * 2];
            sum += sample * sample;
        }
        return sum;
    };
    for (int pulse = 0; pulse < 8; ++pulse)
    {
        const int start = pulse * 12000;
        EXPECT_GT(window(start + 800), window(start + 6000) * 2) << "120 BPM pulse " << pulse;
    }
    for (int pulse = 0; pulse < 8; ++pulse)
    {
        const int start = 96000 + pulse * 24000;
        EXPECT_GT(window(start + 800), window(start + 12000) * 2) << "60 BPM pulse " << pulse;
    }
    settings.pulse_beats = 1;
    settings.motion = 0;
    ASSERT_TRUE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_RING, &settings));
    Grapple_ResetChipPlayer(player.get());
    const auto quarter = Render(player.get(), 48000);
    EXPECT_NE(std::vector<float>(pcm.begin(), pcm.begin() + 96000), quarter);
}

TEST_F(ChipPlayer, DelayIsTempoBasedAndDrumsAreDry)
{
    for (float seconds_per_beat : {0.5F, 1.0F})
    {
        ChipEffectBus bus;
        ASSERT_TRUE(Chip_EffectsInit(&bus, GRAPPLE_CHIP_PRESET_LEAD, 8000));
        bus.settings = {};
        bus.settings.delay = 1;
        bus.settings.delay_beats = 0.5F;
        const int expected = static_cast<int>(seconds_per_beat * 4000);
        for (int i = 0; i <= expected; ++i)
        {
            float left = i == 0 ? 1.0F : 0.0F;
            float right = 0;
            Chip_EffectsProcess(&bus, 8000, seconds_per_beat, &left, &right);
            EXPECT_FLOAT_EQ(left, (i == 0 || i == expected) ? 1.0F : 0.0F);
        }
        Chip_EffectsDestroy(&bus);
    }
    ChipEffectBus drums;
    ASSERT_TRUE(Chip_EffectsInit(&drums, GRAPPLE_CHIP_PRESET_DRUMS, 8000));
    for (int i = 0; i < 1000; ++i)
    {
        float left = 0.25F, right = -0.3F;
        Chip_EffectsProcess(&drums, 8000, 0.5F, &left, &right);
        EXPECT_FLOAT_EQ(left, 0.25F);
        EXPECT_FLOAT_EQ(right, -0.3F);
    }
    Chip_EffectsDestroy(&drums);
}

TEST_F(ChipPlayer, ChorusCreatesStereoMovementAndReverbDecays)
{
    ChipEffectBus bass;
    ASSERT_TRUE(Chip_EffectsInit(&bass, GRAPPLE_CHIP_PRESET_BASS, 8000));
    double difference = 0;
    for (int i = 0; i < 8000; ++i)
    {
        float left = std::sin(static_cast<float>(i) * 0.1F), right = left;
        Chip_EffectsProcess(&bass, 8000, 0.5F, &left, &right);
        difference += std::abs(left - right);
    }
    EXPECT_GT(difference, 1.0);
    Chip_EffectsDestroy(&bass);
    ChipEffectBus room;
    ASSERT_TRUE(Chip_EffectsInit(&room, GRAPPLE_CHIP_PRESET_LEAD, 8000));
    room.settings = {};
    room.settings.reverb = 1;
    double early = 0, late = 0;
    for (int i = 0; i < 24000; ++i)
    {
        float left = i == 0 ? 1.0F : 0.0F, right = left;
        Chip_EffectsProcess(&room, 8000, 0.5F, &left, &right);
        ASSERT_TRUE(std::isfinite(left) && std::isfinite(right));
        if (i > 100 && i < 8000)
            early += left * left + right * right;
        if (i > 16000)
            late += left * left + right * right;
    }
    EXPECT_GT(early, 0.01);
    EXPECT_LT(late, early * 0.01);
    Chip_EffectsDestroy(&room);
}

TEST_F(ChipPlayer, WetRenderingIsBlockIndependentAndDrainsTails)
{
    const auto song = Example();
    ASSERT_TRUE(song);
    Player a(Grapple_CreateChipPlayer(song.get(), 8000, 64, false), Grapple_DestroyChipPlayer);
    Player b(Grapple_CreateChipPlayer(song.get(), 8000, 64, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(a && b);
    const auto whole = Render(a.get(), 8000 * 23);
    std::vector<float> blocks;
    for (int done = 0; done < 8000 * 23;)
    {
        const int count = std::min(137, 8000 * 23 - done);
        const auto pcm = Render(b.get(), count);
        blocks.insert(blocks.end(), pcm.begin(), pcm.end());
        done += count;
    }
    EXPECT_EQ(whole, blocks);
    EXPECT_GT(
        Energy(std::vector<float>(whole.begin() + 8000 * 2 * 10, whole.begin() + 8000 * 2 * 11)),
        0.001);
    EXPECT_EQ(Energy(Render(a.get(), 1000)), 0.0);
    Grapple_ResetChipPlayer(b.get());
    EXPECT_EQ(Render(b.get(), 8000 * 23), whole);
}

TEST_F(ChipPlayer, ManagedPlaybackOwnsMixerAndRetainsReleasedSong)
{
    ASSERT_TRUE(SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy"));
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_AUDIO));
    auto song = SustainedNote();
    ASSERT_TRUE(song);
    Player player(Grapple_CreateChipPlayer(song.get(), 8000, 64, true), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(player);
    song.reset();
    ASSERT_TRUE(Grapple_PlayChipPlayer(player.get())) << SDL_GetError();
    SDL_Delay(40);
    EXPECT_TRUE(Grapple_ChipPlayerPlaying(player.get()));
    Grapple_PauseChipPlayer(player.get());
    EXPECT_FALSE(Grapple_ChipPlayerPlaying(player.get()));
    ASSERT_TRUE(Grapple_PlayChipPlayer(player.get()));
    Grapple_StopChipPlayer(player.get());
    EXPECT_FALSE(Grapple_ChipPlayerPlaying(player.get()));
    ASSERT_TRUE(Grapple_PlayChipPlayer(player.get()));
    SDL_Delay(40);
    player.reset();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_ResetHint(SDL_HINT_AUDIO_DRIVER);
}

TEST_F(ChipPlayer, EffectControlsRejectNonfiniteAndOutOfRangeSettings)
{
    const auto song = SustainedNote();
    ASSERT_TRUE(song);
    const auto player = MakePlayer(song.get());
    ASSERT_TRUE(player);
    Grapple_ChipEffects effects = {};
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_RING, &effects));
    effects.pulse_beats = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_RING, &effects));
    effects.pulse_beats = 0.01;
    EXPECT_FALSE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_RING, &effects));
    effects.pulse_beats = 0.5;
    effects.delay_feedback = 1;
    EXPECT_FALSE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_RING, &effects));
    effects = {};
    effects.delay = 1;
    EXPECT_FALSE(Grapple_SetChipPresetEffects(player.get(), GRAPPLE_CHIP_PRESET_LEAD, &effects));
}

TEST_F(ChipPlayer, HarmonyDefaultsCanBeAppliedToAnotherInstrument)
{
    const auto song = SustainedNote();
    ASSERT_TRUE(song);
    Player normal(Grapple_CreateChipPlayer(song.get(), 8000, 16, false), Grapple_DestroyChipPlayer);
    Player pulsed(Grapple_CreateChipPlayer(song.get(), 8000, 16, false), Grapple_DestroyChipPlayer);
    ASSERT_TRUE(normal && pulsed);
    ASSERT_TRUE(Grapple_SetChipTrackPreset(normal.get(), 0, GRAPPLE_CHIP_PRESET_BASS, 1));
    ASSERT_TRUE(Grapple_SetChipTrackPreset(pulsed.get(), 0, GRAPPLE_CHIP_PRESET_BASS, 1));
    Grapple_ChipEffects bass = {}, harmony = {};
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_BASS, &bass));
    ASSERT_TRUE(Grapple_GetChipPresetEffects(GRAPPLE_CHIP_PRESET_HARMONY, &harmony));
    EXPECT_DOUBLE_EQ(harmony.pulse_beats, 0.5);
    EXPECT_GT(harmony.pulse_depth, 0);
    EXPECT_GT(harmony.motion, 0);
    bass.pulse_beats = harmony.pulse_beats;
    bass.pulse_depth = harmony.pulse_depth;
    bass.motion = harmony.motion;
    ASSERT_TRUE(Grapple_SetChipPresetEffects(pulsed.get(), GRAPPLE_CHIP_PRESET_BASS, &bass));
    const auto plain = Render(normal.get(), 8000);
    const auto moving = Render(pulsed.get(), 8000);
    EXPECT_GT(Energy(plain), 1.0);
    EXPECT_GT(Energy(moving), 1.0);
    EXPECT_NE(plain, moving);
}

} // namespace
