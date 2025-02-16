#include "audio.h"

#include <stdio.h>

#include <SDL3/SDL_audio.h>
#include <pocketmod.h>
#include <stdlib.h>

#include "error.h"
extern char last_error[LAST_ERROR_SZ];

static SDL_AudioStream* music_stream = NULL;
static SDL_AudioDeviceID device;
static pocketmod_context const* mod_ctx = NULL;

int ps_audio_init()
{
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (device == 0) {
        snprintf(last_error, sizeof last_error, "Could not open audio device: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

int ps_audio_finalize()
{
    SDL_CloseAudioDevice(device);
    return 0;
}

SDL_AudioStream* ps_audio_create_stream(SDL_AudioSpec* spec)  // private in res.c: ps_res_add_sound
{
    SDL_AudioStream* stream = SDL_CreateAudioStream(spec, NULL);
    if (stream == NULL) {
        snprintf(last_error, sizeof last_error, "Could not create audio stream: %s", SDL_GetError());
        return NULL;
    }
    if (!SDL_BindAudioStream(device, stream)) {
        snprintf(last_error, sizeof last_error, "Could not bind audio stream: %s", SDL_GetError());
        return NULL;
    }
    return stream;
}


int ps_audio_choose_music(ps_res_idx_t idx)
{
    if (idx == RES_ERROR)
        return -1;

    mod_ctx = ps_res_get(idx, RT_MUSIC)->music;

    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = 44100,
    };
    music_stream = ps_audio_create_stream(&spec);
    if (music_stream == NULL)
        return -1;

    SDL_ClearAudioStream(music_stream);

    SDL_PauseAudioStreamDevice(music_stream);

    return 0;
}

int ps_audio_play_music(bool play)
{
    if (play)
        SDL_ResumeAudioStreamDevice(music_stream);
    else
        SDL_PauseAudioStreamDevice(music_stream);
    return 0;
}

int ps_audio_play_sound(ps_res_idx_t idx)
{
    if (idx == RES_ERROR)
        return -1;

    ps_SoundEffect sound = ps_res_get(idx, RT_SOUND)->sound;

    if (SDL_GetAudioStreamAvailable(sound.stream) < ((int) sound.sz)) {
        SDL_PutAudioStreamData(sound.stream, sound.data, sound.sz);
    }

    return 0;
}

int ps_audio_step()
{
    if (mod_ctx != NULL) {
        // keep 200k in audio queue at all times
        if (SDL_GetAudioStreamAvailable(music_stream) < 200000) {
            static float samples[8000];
            size_t sz = pocketmod_render((pocketmod_context *) mod_ctx, samples, sizeof samples);
            SDL_PutAudioStreamData(music_stream, samples, sz);
        }
    }

    return 0;
}
