// intro.c
#include <citro2d.h>
#include "intro.h"
#include "gfx_intro.h"
#include "game.h"
#include "lang.h"
#include "audio.h"

static size_t event_pos = 0;
static size_t text_position = 0;
static char current_str[2048];
static char previous_str[2048];

#define INTRO_CHAR_DELAY      100
#define INTRO_CHAR_DELAY_FAST  33
#define INTRO_MAX_SPRITES      5

typedef enum {
    INTRO_TEXT,
    INTRO_PAUSE,
    INTRO_MUSIC_START,
    INTRO_MUSIC_STOP,
    INTRO_PLAY_SOUND,
    INTRO_IMAGE_LEFT,
    INTRO_IMAGE_CENTER,
    INTRO_IMAGE_RIGHT,
    INTRO_END_SCENE,
    INTRO_FULL_SCREEN,
    INTRO_END
} IntroEventType;

static C2D_SpriteSheet intro_assets;
static size_t current_event;

static u64 next_char_time;
static u64 pause_start;

static bool full_screen = false;
static C2D_Image image_left;
static C2D_Image image_center;
static C2D_Image image_right;
static bool image_left_visible   = false;
static bool image_center_visible = false;
static bool image_right_visible  = false;

typedef enum {
    WHITE,
    RED,
    BLUE,
    GREEN
} TextColor;

typedef struct {
    float x;
    float y;
    int image;
} IntroSprite;

typedef struct {
    float x;
    float y;
    C2D_Image image;
    bool visible;
} Sprite;

typedef struct {
    IntroEventType type;
    const char *text;
    u32 duration;
    TextColor color;
    int image;
    IntroSprite sprites[INTRO_MAX_SPRITES];
    const char *sound;
} IntroEvent;

static Sprite sprites[INTRO_MAX_SPRITES];

static C2D_Text text_current;
static C2D_Text text_previous;
static C2D_TextBuf text_buf;
static u32 current_color;
static const IntroEvent timeline[] = {
    { .type = INTRO_MUSIC_START,  .sound = "romfs:/audio/intro.ogg"},
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_1", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_2", .color = RED },
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene1_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_3", .color = RED },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene4_idx },
    { .type = INTRO_PAUSE,        .duration = 100 },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene5_idx },
    { .type = INTRO_PAUSE,        .duration = 100 },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene6_idx },
    { .type = INTRO_PAUSE,        .duration = 100 },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene7_idx },
    { .type = INTRO_PAUSE,        .duration = 100 },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_4", .color = RED },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene1_idx },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene7_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_5", .color = RED },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene2_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_6", .color = RED },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene1_idx },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene2_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_7", .color = RED },
    { .type = INTRO_IMAGE_RIGHT, .image = gfx_intro_scene3_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_8", .color = RED },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene1_idx },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene8_idx },
    { .type = INTRO_IMAGE_RIGHT, .image = gfx_intro_scene9_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_9", .color = RED },
    { .type = INTRO_PAUSE,        .duration = 3000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_10", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene10_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_11", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene10_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_12", .color = BLUE },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene11_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_13", .color = BLUE },
    { .type = INTRO_IMAGE_RIGHT, .image = gfx_intro_scene12_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_14", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene10_idx },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene11_idx },
    { .type = INTRO_IMAGE_RIGHT, .image = gfx_intro_scene12_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_15", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_END_SCENE },
    { .type = INTRO_IMAGE_LEFT,   .image = gfx_intro_scene10_idx },
    { .type = INTRO_IMAGE_CENTER, .image = gfx_intro_scene11_idx },
    { .type = INTRO_IMAGE_RIGHT, .image = gfx_intro_scene12_idx },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_16", .color = BLUE },
    { .type = INTRO_IMAGE_CENTER, .image = -1 },
    { .type = INTRO_TEXT,         .text = "TITLE_INTRO_SCENE_17", .color = BLUE },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_MUSIC_STOP },
    { .type = INTRO_FULL_SCREEN,  .sprites = {
        {.x = 0.0f, .y = 240.0f, .image = gfx_intro_bottom_background_idx},
        {.x = 0.0f, .y = 0.0f, .image = gfx_intro_top_background_idx}
    }},
    { .type = INTRO_PLAY_SOUND,   .sound = "romfs:/audio/footsteps.raw" },
    { .type = INTRO_PAUSE,        .duration = 2000 },
    { .type = INTRO_FULL_SCREEN,  .sprites = {
        {.x = 0.0f, .y = 240.0f, .image = gfx_intro_bottom_background_idx},
        {.x = 0.0f, .y = 0.0f, .image = gfx_intro_top_background_idx},
        {.x = 94.0f, .y = 240.0f, .image = gfx_intro_far_man_idx},
    }},
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_FULL_SCREEN,  .sprites = {
        {.x = 0.0f, .y = 240.0f, .image = gfx_intro_bottom_background_idx},
        {.x = 0.0f, .y = 0.0f, .image = gfx_intro_top_background_idx},
        {.x = 141.0f, .y = 274.0f, .image = gfx_intro_not_very_far_man_idx}
    }},
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_FULL_SCREEN,  .sprites = {
        {.x = 0.0f, .y = 240.0f, .image = gfx_intro_bottom_background_idx},
        {.x = 0.0f, .y = 0.0f, .image = gfx_intro_top_background_idx},
        {.x = 144.0f, .y = 264.0f, .image = gfx_intro_close_man_idx}
    }},
    { .type = INTRO_PAUSE,        .duration = 1500 },
    { .type = INTRO_FULL_SCREEN,  .sprites = {
        {.x = 0.0f, .y = 240.0f, .image = gfx_intro_bottom_background_idx},
        {.x = 0.0f, .y = 0.0f, .image = gfx_intro_top_background_idx},
        {.x = 15.0f, .y = 62.0f, .image = gfx_intro_logo_infernal_idx},
        {.x = 57.0f, .y = 318.0f, .image = gfx_intro_logo_house_idx}
    }},
    { .type = INTRO_PAUSE,        .duration = 4500 },
    { .type = INTRO_END_SCENE },
    {. type = INTRO_END }
};

static size_t utf8_char_size(const char *s) {
    unsigned char c = *s;

    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;

    return 1;
}

static void update_text(void) {
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text_current, text_buf, current_str);
    C2D_TextParse(&text_previous, text_buf, previous_str);
    C2D_TextOptimize(&text_current);
    C2D_TextOptimize(&text_previous);
}

void intro_update(void) {
    const IntroEvent *event = &timeline[current_event];
    u64 now = osGetTime();

    u32 held = hidKeysHeld();
    int pause_duration = (held & KEY_A) ? INTRO_CHAR_DELAY_FAST : INTRO_CHAR_DELAY;

    switch (event->type) {
    case INTRO_MUSIC_START:
        if (event->sound) {
            music_play(event->sound);
        }
        current_event++;
        break;
    case INTRO_MUSIC_STOP:
        music_stop();
        current_event++;
        break;
    case INTRO_PLAY_SOUND:
        if (event->sound) {
            sfx_play(event->sound);
        }
        current_event++;
        break;
    case INTRO_TEXT:
        if (now >= next_char_time) {
            const char *str = lang_get(event->text);
            switch(event->color) {
            case BLUE:
                current_color = C2D_Color32(0, 0, 164, 255);
                break;
            case RED:
                current_color = C2D_Color32(164, 0, 0, 255);
                break;
            case GREEN:
                current_color = C2D_Color32(0, 164, 0, 255);
                break;
            default:
                current_color = C2D_Color32(164, 164, 164, 255);
            }
            if (str[event_pos] != '\0') {
                size_t len = utf8_char_size(&str[event_pos]);
                memcpy(previous_str, current_str, text_position);
                previous_str[text_position] = '\0';
                memcpy(&current_str[text_position], &str[event_pos], len);
                text_position += len;
                current_str[text_position] = '\0';
                event_pos += len;
                next_char_time = now + pause_duration;
                update_text();
            } else {
                event_pos = 0;
                current_event++;
            }
        }
        break;

    case INTRO_PAUSE:
        if (pause_start == 0) {
            pause_start = now;
        }

        if (now - pause_start >= event->duration * pause_duration / INTRO_CHAR_DELAY) {
            pause_start = 0;
            current_event++;
        }
        break;

    case INTRO_IMAGE_LEFT:
        if (event->image >= 0) {
            image_left = C2D_SpriteSheetGetImage(intro_assets, event->image);
            image_left_visible = true;
        } else {
            image_left_visible = false;
        }
        current_event++;
        break;

    case INTRO_IMAGE_CENTER:
        if (event->image >= 0) {
            image_center = C2D_SpriteSheetGetImage(intro_assets, event->image);
            image_center_visible = true;
        } else {
            image_center_visible = false;
        }
        current_event++;
        break;

    case INTRO_IMAGE_RIGHT:
        if (event->image >= 0) {
            image_right = C2D_SpriteSheetGetImage(intro_assets, event->image);
            image_right_visible = true;
        } else {
            image_right_visible = false;
        }
        current_event++;
        break;

    case INTRO_END_SCENE:
        image_left_visible   = false;
        image_center_visible = false;
        image_right_visible  = false;
        full_screen = false;
        current_str[0] = '\0';
        previous_str[0] = '\0';
        text_position = 0;
        event_pos = 0;
        update_text();
        current_event++;
        break;

    case INTRO_FULL_SCREEN:
        full_screen = true;
        for (int i = 0; i < INTRO_MAX_SPRITES; i++) {
            if (event->sprites[i].image) {
                sprites[i].visible = true;
                sprites[i].image = C2D_SpriteSheetGetImage(intro_assets, event->sprites[i].image);
                sprites[i].x = event->sprites[i].x;
                sprites[i].y = event->sprites[i].y;
            } else {
                sprites[i].visible = false;
            }
        }
        current_event++;
        break;

    case INTRO_END:
        game_end_intro();
        return;
    }
}

void intro_draw_bottom(void) {
    if (full_screen) {
        for (int i = 0; i < INTRO_MAX_SPRITES; i++) {
            float z = i * 0.01f;
            Sprite *sprite = &sprites[i];
            if (sprite->visible && sprite->y >= 240.0f) {
                C2D_DrawImageAt(sprite->image, sprite->x, sprite->y - 240, z, NULL, 1.0f, 1.0f);
            }
        }
        return;
    }
    C2D_DrawText(&text_current, C2D_WithColor, 7.0f, 70.0f, 0.0f, 0.6f, 0.65f, C2D_Color32(192, 192, 192, 255));
    C2D_DrawText(&text_previous, C2D_WithColor, 7.0f, 70.0f, 0.01f, 0.6f, 0.65f, current_color);
}


void intro_draw_top(void) {
    if (full_screen) {
        for (int i = 0; i < INTRO_MAX_SPRITES; i++) {
            float z = i * 0.01f;
            Sprite *sprite = &sprites[i];
            if (sprite->visible && sprite->y < 240.0f) {
                C2D_DrawImageAt(sprite->image, sprite->x + 40.0f, sprite->y, z, NULL, 1.0f, 1.0f);
            }
        }
        return;
    }
    if (image_left_visible) {
        C2D_DrawImageAt(image_left, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
    if (image_center_visible) {
        C2D_DrawImageAt(image_center, 133.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
    if (image_right_visible) {
        C2D_DrawImageAt(image_right, 266.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
}

void intro_init(void) {
    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }
    full_screen = false;
    current_event = 0;
    event_pos = 0;
    text_position = 0;
    current_str[0] = '\0';
    previous_str[0] = '\0';
    image_left_visible = false;
    image_center_visible = false;
    image_right_visible = false;
    pause_start = 0;
    current_color = C2D_Color32(164, 164, 164, 255);;
    next_char_time = osGetTime();
    intro_assets = C2D_SpriteSheetLoad("romfs:/gfx/gfx_intro.t3x");
    update_text();
}

void intro_close(void) {
    if (intro_assets) {
        C2D_SpriteSheetFree(intro_assets);
        intro_assets = NULL;
    }
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    music_stop();
}
