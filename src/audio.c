#include "audio.h"

#include <stdio.h>

#include <SDL3/SDL_audio.h>
#include <pocketmod.h>
#include <stdlib.h>

#include "error.h"
extern char last_error[LAST_ERROR_SZ];

static SDL_AudioStream* stream = NULL;
static pocketmod_context const* mod_ctx = NULL;
static bool playing_music = false;

int ps_audio_init()
{
    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = 44100,
    };

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (stream == NULL) {
        snprintf(last_error, sizeof last_error, "Could not open audio device: %s", SDL_GetError());
        return -1;
    }

    SDL_Log("Sound device open: %s", SDL_GetAudioDeviceName(SDL_GetAudioStreamDevice(stream)));

    SDL_ResumeAudioStreamDevice(stream);

    return 0;
}

int ps_audio_finalize()
{
    SDL_DestroyAudioStream(stream);
    return 0;
}

int ps_audio_choose_music(resource_idx_t idx)
{
    if (idx == RES_ERROR)
        return -1;

    mod_ctx = ps_res_get(idx)->music;
    return 0;
}

int ps_audio_play_music(bool play)
{
    playing_music = play;
    return 0;
}

int ps_audio_step()
{
    if (mod_ctx != NULL && playing_music) {
        static float samples[44100];
        size_t sz = pocketmod_render((pocketmod_context *) mod_ctx, samples, sizeof samples);
        SDL_PutAudioStreamData(stream, samples, sz);
    }

    return 0;
}
