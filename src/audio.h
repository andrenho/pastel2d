#ifndef AUDIO_H
#define AUDIO_H

#include "res.h"

int ps_audio_init();
int ps_audio_finalize();

int ps_audio_step();

int ps_audio_choose_music(resource_idx_t idx);
int ps_audio_play_music(bool play);

#endif //AUDIO_H
