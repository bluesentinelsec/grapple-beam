/* Original Grapple code (zlib). Player mix controls and optional private part buses. */
#include "chip_player.h"

bool Grapple_SetChipPlayerGain(Grapple_ChipPlayer *p, float gain)
{
    if (!p || !(gain >= 0 && gain <= 2))
        return SDL_SetError("chiptune: master gain must be 0 through 2");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    p->master_gain = gain;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_SetChipTrackMix(Grapple_ChipPlayer *p, int track, const Grapple_ChipTrackMix *mix)
{
    if (!p || track < 0 || track >= p->song->info.track_count || !mix ||
        !(mix->gain >= 0 && mix->gain <= 2) || !(mix->pan >= -1 && mix->pan <= 1))
        return SDL_SetError("chiptune: invalid track mix");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    p->parts[track].gain = mix->gain;
    p->parts[track].pan = mix->pan;
    p->parts[track].muted = mix->muted;
    p->parts[track].solo = mix->solo;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_ReadChipTrackMix(Grapple_ChipPlayer *p, int track, Grapple_ChipTrackMix *mix)
{
    if (!p || track < 0 || track >= p->song->info.track_count || !mix)
        return SDL_SetError("chiptune: invalid track mix output");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    *mix = (Grapple_ChipTrackMix){p->parts[track].gain, p->parts[track].pan, p->parts[track].muted,
                                  p->parts[track].solo};
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_SetChipTrackEffects(Grapple_ChipPlayer *p, int track,
                                 const Grapple_ChipEffects *effects)
{
    if (!p || track < 0 || track >= p->song->info.track_count ||
        (effects && !Chip_EffectsValid(effects)))
        return SDL_SetError("chiptune: invalid track effect settings");
    ChipEffectBus *bus = effects ? SDL_calloc(1, sizeof(*bus)) : NULL;
    if (effects && (!bus || !Chip_EffectsInit(bus, GRAPPLE_CHIP_PRESET_LEAD, p->sample_rate)))
    {
        if (bus)
            Chip_EffectsDestroy(bus);
        SDL_free(bus);
        return false;
    }
    if (bus)
        bus->settings = *effects;
    if (!SDL_LockAudioStream(p->stream))
    {
        if (bus)
            Chip_EffectsDestroy(bus);
        SDL_free(bus);
        return false;
    }
    int overrides = 0;
    for (int i = 0; i < p->song->info.track_count; ++i)
        overrides += p->parts[i].effects != NULL;
    if (bus && !p->parts[track].effects && overrides >= 32)
    {
        SDL_UnlockAudioStream(p->stream);
        Chip_EffectsDestroy(bus);
        SDL_free(bus);
        return SDL_SetError("chiptune: at most 32 private part effect buses per player");
    }
    ChipEffectBus *old = p->parts[track].effects;
    p->parts[track].effects = bus;
    p->parts[track].effect_used = false;
    SDL_UnlockAudioStream(p->stream);
    if (old)
        Chip_EffectsDestroy(old);
    SDL_free(old);
    return true;
}

bool Grapple_ReadChipTrackEffects(Grapple_ChipPlayer *p, int track, Grapple_ChipPreset preset,
                                  Grapple_ChipEffects *effects)
{
    if (!p || track < 0 || track >= p->song->info.track_count || !effects ||
        preset < GRAPPLE_CHIP_PRESET_FIRST || preset > GRAPPLE_CHIP_PRESET_LAST)
        return SDL_SetError("chiptune: invalid track effect output");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    *effects =
        p->parts[track].effects ? p->parts[track].effects->settings : p->effects[preset].settings;
    SDL_UnlockAudioStream(p->stream);
    return true;
}

bool Grapple_ReadChipTrackMapping(Grapple_ChipPlayer *p, int track, int channel,
                                  Grapple_ChipMapping *mapping)
{
    if (!p || track < 0 || track >= p->song->info.track_count || channel < 0 || channel > 15 ||
        !mapping)
        return SDL_SetError("chiptune: invalid mapping output");
    if (!SDL_LockAudioStream(p->stream))
        return false;
    ChipEvent event = {0};
    event.track = (Uint16)track;
    event.status = (Uint8)(0x90 | channel);
    event.unpitched =
        p->channels[(p->song->independent_parts ? track * 16 : 0) + channel].percussion;
    mapping->preset = Chip_ResolvePreset(p, &event);
    mapping->program = p->channels[(p->song->independent_parts ? track * 16 : 0) + channel].program;
    mapping->channel = channel;
    mapping->reason =
        p->parts[track].preset != GRAPPLE_CHIP_PRESET_AUTO         ? GRAPPLE_CHIP_MAPPING_OVERRIDE
        : p->parts[track].named_preset != GRAPPLE_CHIP_PRESET_AUTO ? GRAPPLE_CHIP_MAPPING_NAME
        : (channel == 9 || event.unpitched)                        ? GRAPPLE_CHIP_MAPPING_PERCUSSION
        : mapping->preset != GRAPPLE_CHIP_PRESET_LEAD              ? GRAPPLE_CHIP_MAPPING_PROGRAM
                                                                   : GRAPPLE_CHIP_MAPPING_DEFAULT;
    SDL_UnlockAudioStream(p->stream);
    return true;
}
