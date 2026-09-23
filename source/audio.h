// audio.h
#pragma once

#include <stdbool.h>

bool music_play(const char *filename);
void music_stop(void);
int sfx_play(const char *filename);
bool sfx_is_playing(int channel);
void sfx_stop(int channel);
void audio_init(void);
void audio_update(void);
void audio_close(void);
