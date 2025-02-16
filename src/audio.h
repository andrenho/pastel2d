#ifndef AUDIO_H
#define AUDIO_H

#include "res.h"

int ps_audio_init();
int ps_audio_finalize();

int ps_audio_choose_mod(resource_idx_t idx);
int ps_audio_play_mod(bool play);

int ps_audio_step();

#endif //AUDIO_H
