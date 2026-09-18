#include <grapple/audio_bus.h>
#define GAIN_SCALE 1000000
static SDL_AtomicInt g_gain[GRAPPLE_AUDIO_BUS_COUNT] = {
    {GAIN_SCALE}, {GAIN_SCALE}, {GAIN_SCALE}, {GAIN_SCALE}, {GAIN_SCALE}};
static SDL_AtomicInt g_muted;
static const char *kGroups[GRAPPLE_AUDIO_BUS_COUNT] = {"grapple.bus.master", "grapple.bus.music",
                                                       "grapple.bus.sfx", "grapple.bus.speech",
                                                       "grapple.bus.ambient"};
static bool ValidBus(Grapple_AudioBus bus)
{
    return bus >= GRAPPLE_AUDIO_MASTER && bus < GRAPPLE_AUDIO_BUS_COUNT;
}
bool Grapple_SetAudioBusGain(Grapple_AudioBus bus, float gain)
{
    if (!ValidBus(bus) || !(gain >= 0 && gain <= 1))
        return SDL_SetError("audio bus gain must be finite and within 0..1");
    SDL_SetAtomicInt(&g_gain[bus], (int)(gain * GAIN_SCALE + .5f));
    return true;
}
float Grapple_GetAudioBusGain(Grapple_AudioBus bus)
{
    return ValidBus(bus) ? (float)SDL_GetAtomicInt(&g_gain[bus]) / GAIN_SCALE : -1;
}
void Grapple_SetAudioMuted(bool muted)
{
    SDL_SetAtomicInt(&g_muted, muted ? 1 : 0);
}
bool Grapple_AudioMuted(void)
{
    return SDL_GetAtomicInt(&g_muted) != 0;
}
bool Grapple_ApplyAudioBus(Grapple_AudioBus bus, float *pcm, int samples)
{
    if (!ValidBus(bus) || samples < 0 || (!pcm && samples))
        return SDL_InvalidParamError("bus/pcm/samples");
    float gain = Grapple_AudioMuted() ? 0 : Grapple_GetAudioBusGain(GRAPPLE_AUDIO_MASTER);
    if (bus != GRAPPLE_AUDIO_MASTER)
        gain *= Grapple_GetAudioBusGain(bus);
    for (int i = 0; i < samples; ++i)
        pcm[i] *= gain;
    return true;
}
static void SDLCALL Master(void *user, MIX_Mixer *mixer, const SDL_AudioSpec *spec, float *pcm,
                           int samples)
{
    (void)user;
    (void)mixer;
    (void)spec;
    Grapple_ApplyAudioBus(GRAPPLE_AUDIO_MASTER, pcm, samples);
}
static void SDLCALL Category(void *user, MIX_Group *group, const SDL_AudioSpec *spec, float *pcm,
                             int samples)
{
    (void)group;
    (void)spec;
    float gain = (float)SDL_GetAtomicInt((SDL_AtomicInt *)user) / GAIN_SCALE;
    for (int i = 0; i < samples; ++i)
        pcm[i] *= gain;
}
bool Grapple_AttachAudioBuses(MIX_Mixer *mixer)
{
    if (!mixer)
        return SDL_InvalidParamError("mixer");
    SDL_PropertiesID props = MIX_GetMixerProperties(mixer);
    if (SDL_GetBooleanProperty(props, "grapple.buses.attached", false))
        return true;
    MIX_Group *groups[GRAPPLE_AUDIO_BUS_COUNT] = {0};
    bool ok = true;
    for (int i = 1; i < GRAPPLE_AUDIO_BUS_COUNT && ok; ++i)
    {
        groups[i] = MIX_CreateGroup(mixer);
        ok = groups[i] && MIX_SetGroupPostMixCallback(groups[i], Category, &g_gain[i]) &&
             SDL_SetPointerProperty(props, kGroups[i], groups[i]);
    }
    if (ok)
        ok = MIX_SetPostMixCallback(mixer, Master, NULL) &&
             SDL_SetBooleanProperty(props, "grapple.buses.attached", true);
    if (!ok)
        for (int i = 1; i < GRAPPLE_AUDIO_BUS_COUNT; ++i)
        {
            SDL_ClearProperty(props, kGroups[i]);
            MIX_DestroyGroup(groups[i]);
        }
    return ok;
}
MIX_Mixer *Grapple_CreateAudioMixer(SDL_AudioDeviceID device, const SDL_AudioSpec *spec)
{
    MIX_Mixer *mixer = MIX_CreateMixerDevice(device, spec);
    if (mixer && !Grapple_AttachAudioBuses(mixer))
    {
        MIX_DestroyMixer(mixer);
        return NULL;
    }
    return mixer;
}
bool Grapple_RouteAudioTrack(MIX_Track *track, Grapple_AudioBus bus)
{
    if (!track || !ValidBus(bus))
        return SDL_InvalidParamError("track/bus");
    MIX_Mixer *mixer = MIX_GetTrackMixer(track);
    SDL_PropertiesID props = MIX_GetMixerProperties(mixer);
    if (!SDL_GetBooleanProperty(props, "grapple.buses.attached", false))
        return SDL_SetError("mixer has no engine audio buses");
    MIX_Group *group =
        bus == GRAPPLE_AUDIO_MASTER ? NULL : SDL_GetPointerProperty(props, kGroups[bus], NULL);
    return MIX_SetTrackGroup(track, group);
}
