// audio.c
// Implementation notes: the music streams on NDSP channel 0 through
// BUFFER_COUNT wave buffers of SAMPLES_PER_BUFFER frames, decoded with Tremor
// and refilled from audio_update() in the main loop. Effects use channels 1-4,
// each sample loaded entirely in linear memory. The piano mini-game drives
// channels 5-10 itself.
#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tremor/ivorbisfile.h>

#include "audio.h"

#define MUSIC_CHANNEL       0

#define SFX_CHANNEL_FIRST  1
#define SFX_CHANNEL_LAST   4

#define SAMPLES_PER_BUFFER  4096
#define BUFFER_COUNT        2

// One sound-effect channel. sample is non-NULL while the effect is playing;
// sfx_update frees it once the DSP has finished with the buffer.
typedef struct {
    s16 *sample;
    ndspWaveBuf wavebuf;
} SfxChannel;

static SfxChannel channels[SFX_CHANNEL_LAST - SFX_CHANNEL_FIRST + 1];

// Music decoder state. ov_clear() also closes ogg_file, which is why the file
// is never fclose'd directly once ov_open has succeeded.
static OggVorbis_File ogg;
static FILE *ogg_file = NULL;

static ndspWaveBuf wavebuf[BUFFER_COUNT];
static s16 *audio_buffer = NULL;

static int music_channels;
static long sample_rate;

static bool music_playing = false;
// False when ndspInit failed: public functions then return early, so the game
// runs silently and WAIT_SFX doesn't wait for a sound that never ends.
static bool audio_available = false;

// Decodes up to SAMPLES_PER_BUFFER frames into buf, seeking back to the start
// at end of stream so the music loops. Returns false if nothing was decoded.
static bool fill_buffer(ndspWaveBuf *buf) {
    const size_t bytes_per_frame = music_channels * sizeof(s16);
    const size_t buffer_size = SAMPLES_PER_BUFFER * bytes_per_frame;

    char *dst = (char *)buf->data_vaddr;
    size_t total = 0;

    while (total < buffer_size) {
        int bitstream;
        long ret = ov_read(&ogg, dst + total, buffer_size - total, &bitstream);
        if (ret > 0) {
            total += ret;
            continue;
        }
        if (ret == 0) {
            if (ov_raw_seek(&ogg, 0) != 0) {
                break;
            }
            continue;
        }
        break;
    }

    if (total == 0) {
        return false;
    }

    buf->nsamples = total / bytes_per_frame;
    DSP_FlushDataCache(buf->data_vaddr, total);
    return true;
}


bool music_play(const char *filename) {
    if (!audio_available) {
        return false;
    }
    music_stop();
    
    ogg_file = fopen(filename, "rb");

    if (!ogg_file) {
        printf("File not found: %s\n", filename);
        return false;
    }

    if (ov_open(ogg_file, &ogg, NULL, 0) < 0) {
        fclose(ogg_file);
        ogg_file = NULL;
        return false;
    }

    vorbis_info *info = ov_info(&ogg, -1);

    if (!info || (info->channels != 1 && info->channels != 2)) {
        ov_clear(&ogg);
        ogg_file = NULL;
        return false;
    }

    music_channels = info->channels;
    sample_rate = info->rate;

    const size_t bytes_per_frame = music_channels * sizeof(s16);

    const size_t buffer_size = SAMPLES_PER_BUFFER * bytes_per_frame;

// The DSP reads samples by DMA: buffers must live in linear memory and be
// flushed from the data cache once written (see fill_buffer).
    audio_buffer = linearAlloc(buffer_size * BUFFER_COUNT);

    if (!audio_buffer) {
        ov_clear(&ogg);
        ogg_file = NULL;
        return false;
    }

    memset(wavebuf, 0, sizeof(wavebuf));

    for (int i = 0; i < BUFFER_COUNT; i++) {
        wavebuf[i].data_vaddr = (u8 *)audio_buffer + i * buffer_size;
        wavebuf[i].looping = false;
    }

    ndspChnReset(MUSIC_CHANNEL);
    ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_LINEAR);

    ndspChnSetRate(MUSIC_CHANNEL, sample_rate);

    ndspChnSetFormat(MUSIC_CHANNEL, music_channels == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

    float mix[12] = {0};
    mix[0] = 1.0f;
    mix[1] = 1.0f;

    ndspChnSetMix(MUSIC_CHANNEL, mix);

    for (int i = 0; i < BUFFER_COUNT; i++) {
        if (fill_buffer(&wavebuf[i])) {
            ndspChnWaveBufAdd(MUSIC_CHANNEL, &wavebuf[i]);
        }
    }
    music_playing = true;
    return true;
}

int sfx_play(const char *filename) {
    if (!audio_available) {
        return -1;
    }
    int channel = -1;
    SfxChannel *sfx = NULL;

    for (int i = SFX_CHANNEL_FIRST; i <= SFX_CHANNEL_LAST; i++) {
        SfxChannel *candidate = &channels[i - SFX_CHANNEL_FIRST];
        if (!candidate->sample) {
            channel = i;
            sfx = candidate;
            break;
        }
    }

    if (!sfx) {
        return -1;
    }

    FILE *f = fopen(filename, "rb");

    if (!f) {
        printf("File not found: %s\n", filename);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    sfx->sample = linearAlloc(size);

    if (!sfx->sample) {
        fclose(f);
        return -1;
    }

    if (fread(sfx->sample, 1, size, f) != size) {
        fclose(f);
        linearFree(sfx->sample);
        sfx->sample = NULL;
        return -1;
    }

    fclose(f);

    DSP_FlushDataCache(sfx->sample, size);

    memset(&sfx->wavebuf, 0, sizeof(sfx->wavebuf));

    sfx->wavebuf.data_pcm16 = sfx->sample;
    sfx->wavebuf.nsamples = size / sizeof(s16);
    sfx->wavebuf.looping = false;

    ndspChnReset(channel);
    ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
// Effects are headerless PCM: mono, signed 16-bit, 22050 Hz.
    ndspChnSetRate(channel, 22050.0f);
    ndspChnSetFormat(channel, NDSP_FORMAT_MONO_PCM16);

    float mix[12] = {0};
    mix[0] = 1.0f;
    mix[1] = 1.0f;

    ndspChnSetMix(channel, mix);
    ndspChnWaveBufAdd(channel, &sfx->wavebuf);

    return channel;
}

// Requeues every music buffer the DSP has finished playing. At 44.1 kHz a
// buffer lasts about 93 ms, so a frame that blocks longer than that (e.g. a
// room load) can make the music stutter.
static void music_update(void) {
    if (!music_playing) {
        return;
    }

    for (int i = 0; i < BUFFER_COUNT; i++) {
        if (wavebuf[i].status != NDSP_WBUF_DONE) {
            continue;
        }

        if (fill_buffer(&wavebuf[i])) {
            ndspChnWaveBufAdd(MUSIC_CHANNEL, &wavebuf[i]);
        }
    }
}

// Frees the sample of every finished effect, which makes its channel available
// to sfx_play and makes sfx_is_playing return false.
static void sfx_update(void) {
    for (int i = SFX_CHANNEL_FIRST; i <= SFX_CHANNEL_LAST; i++) {
        SfxChannel *sfx = &channels[i - SFX_CHANNEL_FIRST];

        if (!sfx->sample) {
            continue;
        }

        if (sfx->wavebuf.status != NDSP_WBUF_DONE) {
            continue;
        }

        linearFree(sfx->sample);
        sfx->sample = NULL;
        memset(&sfx->wavebuf, 0, sizeof(sfx->wavebuf));
    }
}

void music_stop(void) {
    if (!music_playing) {
        return;
    }

    ndspChnWaveBufClear(MUSIC_CHANNEL);

    if (audio_buffer) {
        linearFree(audio_buffer);
        audio_buffer = NULL;
    }

    ov_clear(&ogg);
    ogg_file = NULL;
    music_playing = false;
}

void sfx_stop(int channel) {
    if (!audio_available || channel < SFX_CHANNEL_FIRST || channel > SFX_CHANNEL_LAST) {
        return;
    }
    SfxChannel *sfx = &channels[channel - SFX_CHANNEL_FIRST];
    ndspChnWaveBufClear(channel);
    if (sfx->sample) {
        linearFree(sfx->sample);
        sfx->sample = NULL;
    }
    memset(&sfx->wavebuf, 0, sizeof(sfx->wavebuf));
}

bool sfx_is_playing(int channel) {
    if (channel < SFX_CHANNEL_FIRST || channel > SFX_CHANNEL_LAST) {
        return false;
    }

    SfxChannel *sfx = &channels[channel - SFX_CHANNEL_FIRST];
    return sfx->sample != NULL;
}

void audio_init(void) {
    if (R_FAILED(ndspInit())) {
        printf("Cannot initialize audio (missing dspfirm.cdc?)");
        return;
    }
    audio_available = true;
}

bool audio_is_available(void) {
    return audio_available;
}

void audio_update(void) {
    music_update();
    sfx_update();
}

void audio_close(void) {
    if (!audio_available) {
        return;
    }
    music_stop();
    for (int i = SFX_CHANNEL_FIRST; i <= SFX_CHANNEL_LAST; i++) {
        sfx_stop(i);
    }
    ndspExit();
    audio_available = false;
}
