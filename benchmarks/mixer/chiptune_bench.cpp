/** @file chiptune_bench.cpp @brief Score import and steady polyphonic PCM rendering costs. */
#include <benchmark/benchmark.h>
#include <grapple/chiptune.h>
#include <memory>

namespace
{
using Song = std::unique_ptr<Grapple_ChipSong, decltype(&Grapple_DestroyChipSong)>;
using Player = std::unique_ptr<Grapple_ChipPlayer, decltype(&Grapple_DestroyChipPlayer)>;

void ImportMusicXml(benchmark::State &state)
{
    size_t size = 0;
    void *bytes = SDL_LoadFile(CHIP_BENCH_ASSETS "/c64-composition-named.xml", &size);
    if (!bytes)
    {
        state.SkipWithError(SDL_GetError());
        return;
    }
    for (auto _ : state)
    {
        Song song(Grapple_LoadChipSongMemory(bytes, size, nullptr, nullptr),
                  Grapple_DestroyChipSong);
        if (!song)
        {
            state.SkipWithError(SDL_GetError());
            break;
        }
        auto *value = song.get();
        benchmark::DoNotOptimize(value);
    }
    SDL_free(bytes);
}
BENCHMARK(ImportMusicXml);

void RenderPolyphony(benchmark::State &state)
{
    const int voices = static_cast<int>(state.range(0));
    auto *composer = Grapple_CreateChipComposer(4, 480);
    if (!composer)
    {
        state.SkipWithError(SDL_GetError());
        return;
    }
    const char *roles[] = {"melody", "harmony", "bass", "harmony"};
    for (int part = 0; part < 4; ++part)
        Grapple_SetChipPart(composer, part, roles[part], GRAPPLE_CHIP_PRESET_AUTO, 1);
    for (int i = 0; i < voices; ++i)
    {
        const Grapple_ChipNote note = {i % 4, 36 + i % 48, 80, 0, 1920};
        Grapple_AddChipNote(composer, &note);
    }
    Song song(Grapple_BuildChipSong(composer, 1920), Grapple_DestroyChipSong);
    Grapple_DestroyChipComposer(composer);
    Player player(Grapple_CreateChipPlayer(song.get(), 48000, voices, true),
                  Grapple_DestroyChipPlayer);
    if (!player)
    {
        state.SkipWithError(SDL_GetError());
        return;
    }
    float pcm[1024];
    for (auto _ : state)
    {
        int frames = Grapple_RenderChipPlayer(player.get(), pcm, 512);
        benchmark::DoNotOptimize(frames);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * 512);
}
BENCHMARK(RenderPolyphony)->Arg(8)->Arg(32)->Arg(64);
} // namespace
