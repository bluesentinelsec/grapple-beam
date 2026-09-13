/** @file audio_bus.h @brief Shared master, music, effects, speech and ambient audio controls. */
#ifndef GRAPPLE_AUDIO_BUS_H
#define GRAPPLE_AUDIO_BUS_H
#include <SDL3_mixer/SDL_mixer.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /** @brief Engine audio categories; master multiplies every category. */
    typedef enum Grapple_AudioBus
    {
        GRAPPLE_AUDIO_MASTER,
        GRAPPLE_AUDIO_MUSIC,
        GRAPPLE_AUDIO_SFX,
        GRAPPLE_AUDIO_SPEECH,
        GRAPPLE_AUDIO_AMBIENT,
        GRAPPLE_AUDIO_BUS_COUNT
    } Grapple_AudioBus;
    /** @brief Set a process-wide bus gain, including already playing tracks.
     * @param bus Category. @param gain Linear gain 0..1, copied atomically.
     * @return Success or false with SDL_GetError(). Thread-safe. */
    extern bool Grapple_SetAudioBusGain(Grapple_AudioBus bus, float gain);
    /** @brief Read a bus gain. @param bus Category. @return Gain or -1 for invalid bus.
     * Thread-safe. */
    extern float Grapple_GetAudioBusGain(Grapple_AudioBus bus);
    /** @brief Mute all engine buses without changing gains. @param muted New mute state.
     * Thread-safe. */
    extern void Grapple_SetAudioMuted(bool muted);
    /** @brief Read master mute. @return True when muted. Thread-safe. */
    extern bool Grapple_AudioMuted(void);
    /** @brief Apply one category and master gain to float PCM.
     * @param bus Category. @param pcm Interleaved samples, modified in place.
     * @param samples Sample count (not frames). @return Success or invalid-input error.
     * This is the integration point for custom audio pipelines; apply exactly once,
     * after their category effects. Real-time safe for valid arguments. */
    extern bool Grapple_ApplyAudioBus(Grapple_AudioBus bus, float *pcm, int samples);
    /** @brief Attach engine bus groups to a mixer. @param mixer Borrowed mixer.
     * @return Success. Idempotent. Mixer destruction releases groups automatically.
     * Reserves the mixer's post-mix callback for master gain; call before playback.
     * Existing ungrouped tracks receive master gain; route category tracks explicitly. */
    extern bool Grapple_AttachAudioBuses(MIX_Mixer *mixer);
    /** @brief Create a playback mixer with engine buses.
     * @param device SDL device selection. @param spec Optional desired format.
     * @return Owned mixer; destroy with MIX_DestroyMixer. Requires SDL audio and MIX_Init. */
    extern MIX_Mixer *Grapple_CreateAudioMixer(SDL_AudioDeviceID device, const SDL_AudioSpec *spec);
    /** @brief Assign a track to an engine bus.
     * @param track Borrowed track on a mixer with attached engine buses.
     * @param bus Music/SFX/speech/ambient, or master for uncategorized audio.
     * @return Success; existing track gain and effects are retained. */
    extern bool Grapple_RouteAudioTrack(MIX_Track *track, Grapple_AudioBus bus);
#ifdef __cplusplus
}
#endif
#endif
