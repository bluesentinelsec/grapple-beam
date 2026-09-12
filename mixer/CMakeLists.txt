# Grapple::Mixer — static-only build of the vendored SDL_mixer sources
# (upstream release-3.2.4, the redesigned MIX_* API; provenance in
# deps/SDL3_mixer.md).
#
# Only static-link-friendly decoders are enabled; every one is vendored
# single-file or pure C, so the link-time dependency set is empty:
#   WAV, AIFF, AU, VOC, RAW, SINEWAVE           (pure C)
#   OGG Vorbis  via vendored stb_vorbis
#   MP3         via vendored dr_mp3
#   FLAC        via vendored dr_flac
#   MIDI        via vendored TiMidity (software synth; needs GUS patch data
#               at runtime to produce sound, links with zero dependencies)
#
# Deleted outright (no-stubs; heavy external libs): libFLAC, libvorbisfile,
# mpg123, opus/opusfile, wavpack, game-music-emu, libxmp (trackers are out of
# scope), fluidsynth. Their table entries were already #ifdef-gated upstream,
# so no dispatch surgery was required.

# The root project is CXX-only; the vendored SDL_mixer sources are C.
enable_language(C)

# Pinned in-tree source, never a system XML dependency (deps/expat.md).
set(EXPAT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_FUZZERS OFF CACHE BOOL "" FORCE)
set(EXPAT_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(EXPAT_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(EXPAT_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(EXPAT_DTD OFF CACHE BOOL "" FORCE)
set(EXPAT_GE ON CACHE BOOL "" FORCE)
add_subdirectory(src/expat)
set_target_properties(expat PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Grapple_Mixer STATIC
  src/SDL_mixer.c
  src/SDL_mixer_metadata_tags.c
  src/SDL_mixer_spatialization.c
  src/decoder_aiff.c
  src/decoder_au.c
  src/decoder_drflac.c
  src/decoder_drmp3.c
  src/decoder_raw.c
  src/decoder_sinewave.c
  src/decoder_stb_vorbis.c
  src/decoder_timidity.c
  src/decoder_voc.c
  src/decoder_wav.c
  src/timidity/common.c
  src/timidity/instrum.c
  src/timidity/mix.c
  src/timidity/output.c
  src/timidity/playmidi.c
  src/timidity/readmidi.c
  src/timidity/resample.c
  src/timidity/tables.c
  src/timidity/timidity.c
  # Original Grapple code (not vendored): 8-bit synthesis layer.
  src/grapple_chiptune.c
  src/chip_midi.c
  src/chip_song.c
  src/chip_effects.c
  src/chip_voice.c
  src/chip_player.c
  src/chip_xml.c
  src/chip_musicxml.c
  src/chip_score_order.c
  src/chip_archive.c
  src/chip_inflate.c
)
add_library(Grapple::Mixer ALIAS Grapple_Mixer)

if("c_std_99" IN_LIST CMAKE_C_COMPILE_FEATURES)
  target_compile_features(Grapple_Mixer PRIVATE c_std_99)
endif()

set_target_properties(Grapple_Mixer PROPERTIES
  OUTPUT_NAME Grapple_Mixer
  POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(Grapple_Mixer PRIVATE
  DECODER_WAV
  DECODER_AIFF
  DECODER_AU
  DECODER_VOC
  DECODER_OGGVORBIS_STB
  DECODER_FLAC_DRFLAC
  DECODER_MP3_DRMP3
  DECODER_MIDI_TIMIDITY
)

target_include_directories(Grapple_Mixer
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Grapple_Mixer PUBLIC SDL3::SDL3)
target_link_libraries(Grapple_Mixer PRIVATE expat)
# Only the existing standalone inflater header is reused; no VFS runtime dependency.
target_include_directories(Grapple_Mixer SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vfs/src/physfs)

# Vendored upstream C: build quietly rather than under the project's
# warnings-as-errors regime; our own code around it stays strict.

# Original streaming synth code follows the project's strict C warning policy.
if(MSVC)
  set_source_files_properties(src/chip_midi.c src/chip_voice.c src/chip_player.c src/chip_song.c src/chip_effects.c src/chip_xml.c src/chip_musicxml.c src/chip_archive.c src/chip_inflate.c src/chip_score_order.c
    PROPERTIES COMPILE_OPTIONS "/W4;/WX")
else()
  set_source_files_properties(src/chip_midi.c src/chip_voice.c src/chip_player.c src/chip_song.c src/chip_effects.c src/chip_xml.c src/chip_musicxml.c src/chip_archive.c src/chip_inflate.c src/chip_score_order.c
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Werror;-Wconversion;-Wshadow")
endif()
