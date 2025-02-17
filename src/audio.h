#ifndef AUDIO_H
#define AUDIO_H

#include "res.h"

int ps_audio_step();

int ps_audio_choose_music(ps_res_idx_t idx);
int ps_audio_play_music(bool play);

int ps_audio_play_sound(ps_res_idx_t idx);

#endif //AUDIO_H
