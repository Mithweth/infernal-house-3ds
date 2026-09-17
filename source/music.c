#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tremor/ivorbisfile.h>

#include "music.h"

#define MUSIC_CHANNEL       0
#define SAMPLES_PER_BUFFER  4096
#define BUFFER_COUNT        2

static OggVorbis_File ogg;
static FILE *ogg_file = NULL;

static ndspWaveBuf wavebuf[BUFFER_COUNT];
static s16 *audio_buffer = NULL;

static int channels;
static long sample_rate;

static bool initialized = false;


/*
 * Remplit un wavebuf avec du PCM décodé depuis l'OGG.
 *
 * Retourne false uniquement si on n'a vraiment rien pu décoder.
 */
static bool fill_buffer(ndspWaveBuf *buf)
{
    const size_t bytes_per_frame =
        channels * sizeof(s16);

    const size_t buffer_size =
        SAMPLES_PER_BUFFER * bytes_per_frame;

    char *dst = (char *)buf->data_vaddr;
    size_t total = 0;

    while (total < buffer_size) {

        int bitstream;

        long ret = ov_read(
            &ogg,
            dst + total,
            buffer_size - total,
            &bitstream
        );

        if (ret > 0) {
            total += ret;
            continue;
        }

        if (ret == 0) {
            /*
             * Fin du morceau :
             * retour au début pour boucler.
             */
            if (ov_raw_seek(&ogg, 0) != 0)
                break;

            continue;
        }

        /*
         * Erreur de décodage.
         */
        break;
    }

    if (total == 0)
        return false;

    /*
     * NDSP attend un nombre de frames audio.
     *
     * mono   : 1 frame = 1 x s16
     * stereo : 1 frame = 2 x s16
     */
    buf->nsamples = total / bytes_per_frame;

    DSP_FlushDataCache(
        buf->data_vaddr,
        total
    );

    return true;
}


void music_play(const char *filename)
{
    music_stop();
    if (initialized)
        return;

    if (ndspInit() != 0)
        return;

    ogg_file = fopen(
        filename,
        "rb"
    );

    if (!ogg_file) {
        ndspExit();
        return;
    }

    if (ov_open(ogg_file, &ogg, NULL, 0) < 0) {
        fclose(ogg_file);
        ogg_file = NULL;
        ndspExit();
        return;
    }

    /*
     * À partir d'ici, ov_clear() fermera également
     * le FILE donné à ov_open().
     */

    vorbis_info *info = ov_info(&ogg, -1);

    if (!info ||
        (info->channels != 1 && info->channels != 2)) {

        ov_clear(&ogg);
        ogg_file = NULL;
        ndspExit();
        return;
    }

    channels = info->channels;
    sample_rate = info->rate;

    const size_t bytes_per_frame =
        channels * sizeof(s16);

    const size_t buffer_size =
        SAMPLES_PER_BUFFER * bytes_per_frame;

    audio_buffer = linearAlloc(
        buffer_size * BUFFER_COUNT
    );

    if (!audio_buffer) {
        ov_clear(&ogg);
        ogg_file = NULL;
        ndspExit();
        return;
    }

    memset(wavebuf, 0, sizeof(wavebuf));

    for (int i = 0; i < BUFFER_COUNT; i++) {

        wavebuf[i].data_vaddr =
            (u8 *)audio_buffer +
            i * buffer_size;

        wavebuf[i].looping = false;
    }

    /*
     * Configuration du canal NDSP.
     */

    ndspChnReset(MUSIC_CHANNEL);

    ndspChnSetInterp(
        MUSIC_CHANNEL,
        NDSP_INTERP_LINEAR
    );

    ndspChnSetRate(
        MUSIC_CHANNEL,
        sample_rate
    );

    ndspChnSetFormat(
        MUSIC_CHANNEL,
        channels == 2
            ? NDSP_FORMAT_STEREO_PCM16
            : NDSP_FORMAT_MONO_PCM16
    );

    float mix[12] = {0};

    mix[0] = 1.0f;
    mix[1] = 1.0f;

    ndspChnSetMix(
        MUSIC_CHANNEL,
        mix
    );

    /*
     * On préremplit les deux buffers.
     */

    for (int i = 0; i < BUFFER_COUNT; i++) {

        if (fill_buffer(&wavebuf[i])) {
            ndspChnWaveBufAdd(
                MUSIC_CHANNEL,
                &wavebuf[i]
            );
        }
    }

    initialized = true;
}


void music_update(void)
{
    if (!initialized)
        return;

    for (int i = 0; i < BUFFER_COUNT; i++) {

        if (wavebuf[i].status != NDSP_WBUF_DONE)
            continue;

        if (fill_buffer(&wavebuf[i])) {

            ndspChnWaveBufAdd(
                MUSIC_CHANNEL,
                &wavebuf[i]
            );
        }
    }
}


void music_stop(void)
{
    if (!initialized)
        return;

    /*
     * Il faut d'abord empêcher NDSP d'utiliser
     * les buffers avant de libérer leur mémoire.
     */

    ndspChnWaveBufClear(MUSIC_CHANNEL);

    if (audio_buffer) {
        linearFree(audio_buffer);
        audio_buffer = NULL;
    }

    /*
     * ov_clear() libère l'état Vorbis et ferme
     * le FILE associé.
     */
    ov_clear(&ogg);
    ogg_file = NULL;

    ndspExit();

    initialized = false;
}
