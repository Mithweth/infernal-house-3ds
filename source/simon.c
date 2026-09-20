// simon.c

#include <3ds.h>
#include <citro2d.h>
#include <stdlib.h>
#include "simon.h"
#include "game.h"
#include "audio.h"
#include "gfx_simon.h"

typedef enum {
    SIMON_OFF,
    SIMON_SHOW_SEQUENCE,
    SIMON_WAIT_INPUT,
    SIMON_PLAYER_FLASH,
    SIMON_FAILURE
} SimonMode;

typedef enum {
    SIMON_YELLOW,
    SIMON_RED,
    SIMON_GREEN,
    SIMON_BLUE
} SimonColor;

#define SIMON_MAX_ROUNDS 10
#define COLOR_WIDTH   85
#define COLOR_HEIGHT  75
#define SWITCH_WIDTH  36
#define SWITCH_HEIGHT 24
#define SIMON_LIGHT_TIME  500
#define SIMON_PAUSE_TIME  250
#define SIMON_PRESS_TIME 200

static C2D_SpriteSheet simon_assets;
static C2D_Image img_background;
static C2D_Image img_yellow;
static C2D_Image img_red;
static C2D_Image img_green;
static C2D_Image img_blue;
static SimonMode mode;
static SimonColor sequence[SIMON_MAX_ROUNDS];
static int game_round;
static int position;
static int show_position;
static u64 next_event_time;
static bool press_correct;
static bool round_complete;
static bool light_on;
static SimonColor lit_color;
static int fail_sfx_channel = -1;

static void simon_show_sequence(void) {
    mode = SIMON_SHOW_SEQUENCE;
    show_position = 0;
    light_on = false;
    next_event_time = osGetTime();
}

static void simon_update_sequence(void) {
    u64 now = osGetTime();

    if (now < next_event_time) {
        return;
    }

    if (!light_on) {
        lit_color = sequence[show_position];
        light_on = true;

        sfx_play("romfs:/audio/simon_blop.raw");

        next_event_time = now + SIMON_LIGHT_TIME;
    } else {
        light_on = false;
        show_position++;

        if (show_position >= game_round) {
            position = 0;
            mode = SIMON_WAIT_INPUT;
            return;
        }

        next_event_time = now + SIMON_PAUSE_TIME;
    }
}

static void simon_start(void) {
    game_round = 1;
    position = 0;

    for (int i = 0; i < SIMON_MAX_ROUNDS; i++) {
        sequence[i] = rand() % 4;
    }

    simon_show_sequence();
}

static bool touch_inside(touchPosition touch, int x, int y, int width, int height) {
    return touch.px >= x && touch.px < x + width && touch.py >= y && touch.py < y + height;
}

static void simon_press(SimonColor color) {
    if (mode != SIMON_WAIT_INPUT)
        return;

    lit_color = color;
    light_on = true;
    sfx_play("romfs:/audio/simon_blop.raw");
    press_correct = color == sequence[position];

    if (press_correct) {
        position++;
        round_complete = position >= game_round;
    } else {
        round_complete = false;
    }

    next_event_time = osGetTime() + SIMON_PRESS_TIME;
    mode = SIMON_PLAYER_FLASH;
}

static void simon_update_player_flash(void) {
    if (osGetTime() < next_event_time) {
        return;
    }

    light_on = false;

    if (!press_correct) {
        fail_sfx_channel = sfx_play("romfs:/audio/simon_fail.raw");
        mode = SIMON_FAILURE;
        return;
    }

    if (!round_complete) {
        mode = SIMON_WAIT_INPUT;
        return;
    }

    if (game_round >= SIMON_MAX_ROUNDS) {
        game_end_simon(true);
        return;
    }

    game_round++;
    position = 0;
    simon_show_sequence();
}

void simon_update(u32 keys, touchPosition touch) {
    if (keys & KEY_TOUCH) {
        if (touch_inside(touch, 161, 111, SWITCH_WIDTH, SWITCH_HEIGHT)) {
            game_end_simon(false);
            return;
        }
    }

    if (mode == SIMON_FAILURE) {
        if (!sfx_is_playing(fail_sfx_channel)) {
            fail_sfx_channel = -1;
            simon_start();
        }
        return;
    }

    if (mode == SIMON_SHOW_SEQUENCE) {
        simon_update_sequence();
        return;
    }

    if (mode == SIMON_PLAYER_FLASH) {
        simon_update_player_flash();
        return;
    }

    if (!(keys & KEY_TOUCH)) {
        return;
    }

    if (mode == SIMON_OFF) {
        if (touch_inside(touch, 125, 111, SWITCH_WIDTH, SWITCH_HEIGHT)) {
            simon_start();
        }
        return;
    }

    if (mode != SIMON_WAIT_INPUT) {
        return;
    }

    if (touch_inside(touch, 56, 21, COLOR_WIDTH, COLOR_HEIGHT)) {
        simon_press(SIMON_YELLOW);
        return;
    }

    if (touch_inside(touch, 182, 22, COLOR_WIDTH, COLOR_HEIGHT)) {
        simon_press(SIMON_RED);
        return;
    }

    if (touch_inside(touch, 56, 133, COLOR_WIDTH, COLOR_HEIGHT)) {
        simon_press(SIMON_GREEN);
        return;
    }

    if (touch_inside(touch, 179, 135, COLOR_WIDTH, COLOR_HEIGHT)) {
        simon_press(SIMON_BLUE);
        return;
    }
}

void simon_draw_bottom(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (light_on) {
        switch (lit_color) {
            case SIMON_YELLOW:
                C2D_DrawImageAt(img_yellow, 55.0f, 14.0f, 0.5f, NULL, 1.0f, 1.0f);
                break;
            case SIMON_RED:
                C2D_DrawImageAt(img_red, 166.0f, 14.0f, 0.5f, NULL, 1.0f, 1.0f);
                break;
            case SIMON_GREEN:
                C2D_DrawImageAt(img_green, 55.0f, 117.0f, 0.5f, NULL, 1.0f, 1.0f);
                break;
            case SIMON_BLUE:
                C2D_DrawImageAt(img_blue, 166.0f, 117.0f, 0.5f, NULL, 1.0f, 1.0f);
                break;
            default:
                break;
        }
    }
}

void simon_init(void) {
    simon_assets = C2D_SpriteSheetLoad("romfs:/gfx/gfx_simon.t3x");
    img_background = C2D_SpriteSheetGetImage(simon_assets, gfx_simon_background_idx);
    img_yellow = C2D_SpriteSheetGetImage(simon_assets, gfx_simon_yellow_idx);
    img_red = C2D_SpriteSheetGetImage(simon_assets, gfx_simon_red_idx);
    img_green = C2D_SpriteSheetGetImage(simon_assets, gfx_simon_green_idx);
    img_blue = C2D_SpriteSheetGetImage(simon_assets, gfx_simon_blue_idx);
    mode = SIMON_OFF;
}

void simon_close(void) {
    if (simon_assets) {
        C2D_SpriteSheetFree(simon_assets);
        simon_assets = NULL;
    }
}