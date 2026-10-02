// minigame_piano.c

#include <3ds.h>
#include <citro2d.h>
#include "gfxmap.h"
#include "lang.h"
#include "game.h"
#include "gamestate.h"
#include "audio.h"

#define PIANO_CHANNEL_FIRST  5
#define PIANO_CHANNEL_LAST   10

typedef struct {
    s16 *sample;
    ndspWaveBuf wavebuf;
} PianoChannel;

#define PIANO_PATTERN_LENGTH 4
#define NOTE_C3   48
#define NOTE_CS3  49
#define NOTE_D3   50
#define NOTE_DS3  51
#define NOTE_E3   52
#define NOTE_F3   53
#define NOTE_FS3  54
#define NOTE_G3   55
#define NOTE_GS3  56
#define NOTE_A3   57
#define NOTE_AS3  58
#define NOTE_B3   59
#define NOTE_C4   60
#define NOTE_CS4  61
#define NOTE_D4   62
#define NOTE_DS4  63
#define NOTE_E4   64
#define NOTE_F4   65
#define NOTE_FS4  66
#define NOTE_G4   67
#define NOTE_GS4  68
#define NOTE_A4   69
#define NOTE_AS4  70
#define NOTE_B4   71
#define NOTE_C5   72

static ndspWaveBuf wavebuf[PIANO_CHANNEL_LAST - PIANO_CHANNEL_FIRST + 1];
static s16 *sample = NULL;
static size_t sample_size;
static C2D_SpriteSheet assets;
static C2D_Image img_background;
static C2D_TextBuf text_buf;
static C2D_Text text;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int note;
} PianoKey;

static int note_history[PIANO_PATTERN_LENGTH];
static int note_count = 0;

static const int pattern_octave3[] = {
    NOTE_C3, NOTE_DS3, NOTE_G3, NOTE_AS3
};

static const int pattern_octave4[] = {
    NOTE_C4, NOTE_DS4, NOTE_G4, NOTE_AS4
};

static PianoKey keys[] = {
    {.x = 2, .y = 122, .width = 20, .height = 48, .note = NOTE_C3},
    {.x = 2, .y = 45, .width = 9, .height = 77, .note = NOTE_C3},
    {.x = 17, .y = 45, .width = 14, .height = 77, .note = NOTE_CS3},
    {.x = 25, .y = 122, .width = 20, .height = 48, .note = NOTE_D3},
    {.x = 32, .y = 45, .width = 9, .height = 77, .note = NOTE_D3},
    {.x = 42, .y = 45, .width = 14, .height = 77, .note = NOTE_DS3},
    {.x = 47, .y = 122, .width = 20, .height = 48, .note = NOTE_E3},
    {.x = 57, .y = 45, .width = 9, .height = 77, .note = NOTE_E3},
    {.x = 69, .y = 122, .width = 20, .height = 48, .note = NOTE_F3},
    {.x = 69, .y = 45, .width = 9, .height = 77, .note = NOTE_F3},
    {.x = 79, .y = 45, .width = 14, .height = 77, .note = NOTE_FS3},
    {.x = 89, .y = 122, .width = 20, .height = 48, .note = NOTE_G3},
    {.x = 93, .y = 45, .width = 9, .height = 77, .note = NOTE_G3},
    {.x = 103, .y = 45, .width = 14, .height = 77, .note = NOTE_GS3},
    {.x = 110, .y = 122, .width = 20, .height = 48, .note = NOTE_A3},
    {.x = 118, .y = 45, .width = 9, .height = 77, .note = NOTE_A3},
    {.x = 127, .y = 45, .width = 14, .height = 77, .note = NOTE_AS3},
    {.x = 131, .y = 122, .width = 20, .height = 48, .note = NOTE_B3},
    {.x = 143, .y = 45, .width = 9, .height = 77, .note = NOTE_B3},
    {.x = 152, .y = 122, .width = 20, .height = 48, .note = NOTE_C4},
    {.x = 152, .y = 45, .width = 9, .height = 77, .note = NOTE_C4},
    {.x = 164, .y = 45, .width = 14, .height = 77, .note = NOTE_CS4},
    {.x = 172, .y = 122, .width = 20, .height = 48, .note = NOTE_D4},
    {.x = 179, .y = 45, .width = 9, .height = 77, .note = NOTE_D4},
    {.x = 189, .y = 45, .width = 14, .height = 77, .note = NOTE_DS4},
    {.x = 194, .y = 122, .width = 20, .height = 48, .note = NOTE_E4},
    {.x = 205, .y = 45, .width = 9, .height = 77, .note = NOTE_E4},
    {.x = 216, .y = 122, .width = 20, .height = 48, .note = NOTE_F4},
    {.x = 216, .y = 45, .width = 9, .height = 77, .note = NOTE_F4},
    {.x = 226, .y = 45, .width = 14, .height = 77, .note = NOTE_FS4},
    {.x = 237, .y = 122, .width = 20, .height = 48, .note = NOTE_G4},
    {.x = 240, .y = 45, .width = 9, .height = 77, .note = NOTE_G4},
    {.x = 251, .y = 45, .width = 14, .height = 77, .note = NOTE_GS4},
    {.x = 258, .y = 122, .width = 20, .height = 48, .note = NOTE_A4},
    {.x = 265, .y = 45, .width = 9, .height = 77, .note = NOTE_A4},
    {.x = 275, .y = 45, .width = 14, .height = 77, .note = NOTE_AS4},
    {.x = 279, .y = 122, .width = 20, .height = 48, .note = NOTE_B4},
    {.x = 290, .y = 45, .width = 9, .height = 77, .note = NOTE_B4},
    {.x = 299, .y = 122, .width = 20, .height = 125, .note = NOTE_C5}
};

static PianoKey *find_piano_key(int x, int y) {
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        PianoKey *key = &keys[i];
        if (x >= key->x && x <  key->x + key->width && y >= key->y && y <  key->y + key->height) {
            return key;
        }
    }
    return NULL;
}

static bool piano_pattern_matches(const int *pattern) {
    if (note_count < PIANO_PATTERN_LENGTH) {
        return false;
    }

    for (int i = 0; i < PIANO_PATTERN_LENGTH; i++) {
        if (note_history[i] != pattern[i]) {
            return false;
        }
    }

    return true;
}

static void piano_stop(bool success) {
    game_minigame_stop();
    if (success && gamestate_get("livingroom_golden_statue_placed")) {
        gamestate_set("livingroom_secret_passage_opened");
        game_show_message("LIVINGROOM_SECRET_PASSAGE_OPEN");
    }
}

static void piano_register_note(int note) {
    if (note_count < PIANO_PATTERN_LENGTH) {
        note_history[note_count++] = note;
    } else {
        for (int i = 0; i < PIANO_PATTERN_LENGTH - 1; i++) {
            note_history[i] = note_history[i + 1];
        }

        note_history[PIANO_PATTERN_LENGTH - 1] = note;
    }

    if (piano_pattern_matches(pattern_octave3) || piano_pattern_matches(pattern_octave4)) {
        piano_stop(true);
    }
}

static void piano_play_note(int note) {
    if (!sample) {
        return;
    }

    for (int i = 0; i < PIANO_CHANNEL_LAST - PIANO_CHANNEL_FIRST + 1; i++) {
        if (wavebuf[i].status != NDSP_WBUF_FREE && wavebuf[i].status != NDSP_WBUF_DONE) {
            continue;
        }

        int channel = PIANO_CHANNEL_FIRST + i;
        memset(&wavebuf[i], 0, sizeof(wavebuf[i]));

        wavebuf[i].data_pcm16 = sample;
        wavebuf[i].nsamples = sample_size / sizeof(s16);
        wavebuf[i].looping = false;

        float rate = 44100.0f * powf(2.0f, (note - NOTE_A4) / 12.0f);

        ndspChnSetRate(channel, rate);
        ndspChnWaveBufAdd(channel, &wavebuf[i]);
        return;
    }
}

static void piano_update(u32 keys, touchPosition touch) {
    if (!(keys & KEY_TOUCH)) {
        return;
    }
    if (touch.py > 200) {
        piano_stop(false);
        return;
    }
    PianoKey *pressed = find_piano_key(touch.px, touch.py);
    if (!pressed) {
        return;
    }
    piano_play_note(pressed->note);
    piano_register_note(pressed->note);
}

static void piano_draw_bottom(void) {
    C2D_DrawRectSolid(0.0f, 0.0f, 0.9f, 320, 240, C2D_Color32(0, 0, 0, 255));
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.91f, NULL, 1.0f, 1.0f);
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get("PIANO_BACK_TO_GAME"));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, 160.0f, 220.0f, 0.92f, 0.65f, 0.65f, C2D_Color32(164, 164, 164, 255));
}

static bool piano_init(void) {
    if (!gfxmap_load_assets("romfs:/minigames/piano", &assets)) {
        return false;
    }

    img_background = gfxmap_get_image(assets, "gfx_background_idx");

    if (!text_buf) {
        text_buf = C2D_TextBufNew(1024);
    }
    note_count = 0;
    FILE *f = fopen("romfs:/audio/piano_a4.raw", "rb");
    if (!f) {
        printf("cannot load sample romfs:/audio/piano_a4.raw\n");
        C2D_SpriteSheetFree(assets);
        assets = NULL;
        return false;
    }

    fseek(f, 0, SEEK_END);
    sample_size = ftell(f);
    rewind(f);
    sample = linearAlloc(sample_size);
    fread(sample, 1, sample_size, f);
    fclose(f);

    DSP_FlushDataCache(sample, sample_size);

    for (int i = 0; i < PIANO_CHANNEL_LAST - PIANO_CHANNEL_FIRST + 1; i++) {
        int channel = PIANO_CHANNEL_FIRST + i;

        memset(&wavebuf[i], 0, sizeof(wavebuf[i]));

        ndspChnReset(channel);
        ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
        ndspChnSetFormat(channel, NDSP_FORMAT_MONO_PCM16);

        float mix[12] = {0};
        mix[0] = 1.0f;
        mix[1] = 1.0f;

        ndspChnSetMix(channel, mix);
    }
    return true;
}

static void piano_close(void) {
    if (assets) {
        C2D_SpriteSheetFree(assets);
        assets = NULL;
    }
    for (int i = 0; i < PIANO_CHANNEL_LAST - PIANO_CHANNEL_FIRST + 1; i++) {
        ndspChnWaveBufClear(PIANO_CHANNEL_FIRST + i);
        memset(&wavebuf[i], 0, sizeof(wavebuf[i]));
    }

    if (sample) {
        linearFree(sample);
        sample = NULL;
    }
}


MiniGame minigame_piano = {
    .init = piano_init,
    .draw = piano_draw_bottom,
    .update = piano_update,
    .close = piano_close
};
