// audio.h
// Music (Ogg Vorbis, streamed) and sound effects (raw PCM) on top of NDSP.
// audio_init/audio_close are called from main(); audio_update every frame.
// If NDSP can't be initialized, every function below is a silent no-op.
#pragma once

#include <stdbool.h>

// Stops the current music and loops the given .ogg file (mono or stereo).
// Returns false if audio is unavailable or the file can't be opened/decoded.
bool music_play(const char *filename);

// Stops the music, if any.
void music_stop(void);

// Plays a raw sound effect (mono, signed 16-bit, 22050 Hz) on a free channel.
// Returns the channel number, or -1 if audio is unavailable, every effect
// channel is busy, or the file can't be loaded.
int sfx_play(const char *filename);

// Returns true while the effect started on this channel is still playing.
// Any value returned by sfx_play, including -1, may be passed.
bool sfx_is_playing(int channel);

// Stops the effect on this channel and frees its sample.
void sfx_stop(int channel);

// Initializes NDSP. On failure (typically a missing sdmc:/3ds/dspfirm.cdc)
// the game keeps running without sound.
void audio_init(void);

// Returns true if NDSP was initialized. Code that drives NDSP channels
// directly (e.g. the piano mini-game) must check it first.
bool audio_is_available(void);

// Refills the music buffers and frees finished effects. Call once per frame.
void audio_update(void);

// Stops everything and shuts NDSP down.
void audio_close(void);
