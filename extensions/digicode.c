// digicode.c

#include <3ds.h>
#include <citro2d.h>
#include <stdlib.h>
#include <string.h>
#include "digicode.h"
#include "game.h"
#include "audio.h"
#include "gfxmap.h"
#include "secret_code.h"
#include "gamestate.h"

#define KEY_SPACING_X 8.0f
#define KEY_SPACING_Y 8.0f
#define KEY_WIDTH 30.0f
#define KEY_HEIGHT 26.0f
#define PANEL_LEFT 76.0f
#define PANEL_TOP 90.0f


static C2D_SpriteSheet assets;
static C2D_Image img_background;
static uint8_t entered_code[4];
static size_t position;
static C2D_TextBuf text_buf;
static C2D_Text text;

static int key_pressed(touchPosition touch) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            if (touch.px >= PANEL_LEFT + x * (KEY_WIDTH + KEY_SPACING_X) &&
                touch.px <  PANEL_LEFT + x * (KEY_WIDTH + KEY_SPACING_X) + KEY_WIDTH &&
                touch.py >= PANEL_TOP  + y * (KEY_HEIGHT + KEY_SPACING_Y) &&
                touch.py <  PANEL_TOP  + y * (KEY_HEIGHT + KEY_SPACING_Y) + KEY_HEIGHT) {
                return x + 1 + y * 3;
            }
        }
    }

    return -1;
}

static void digicode_stop(bool success) {
    game_minigame_stop();
    if (success) {
        if (gamestate_get("item_syringe_injected")) {
            game_timeline_start("ending");
        } else {
            game_timeline_start("bacteria");
        }
    }

}

static void digicode_update(u32 keys, touchPosition touch) {
    if (keys & KEY_B) {
        digicode_stop(false);
        return;
    }
    if (!(keys & KEY_TOUCH)) {
        return;
    }
    
    int pressed = key_pressed(touch);
    if (pressed == -1) {
        return;
    }
    if (pressed == 10) {
        digicode_stop(false);
        return;
    }
    sfx_play("romfs:/minigames/digicode/beep.raw");
    if (pressed == 12) {
        if (position < 4) {
            return;
        }

        if (memcmp(entered_code, secret_code_get(), sizeof(entered_code)) == 0) {
            sfx_play("romfs:/minigames/digicode/confirm_beep.raw");
            digicode_stop(true);
        } else {
            sfx_play("romfs:/minigames/digicode/error_beep.raw");
            position = 0;
        }
        return;
    }
    if (position >= 4) {
        return;
    } 
    if (pressed == 11) {
        pressed = 0;
    }
    entered_code[position++] = pressed;
}

static void digicode_draw(void) {
    C2D_DrawImageAt(img_background, 35.0f, 0.0f, 0.9f, NULL, 1.0f, 1.0f);
    C2D_TextBufClear(text_buf);
    char buf[2];
    for (int i = 0; i < 4; i++) {
        buf[0] = (i < position) ? '0' + entered_code[i] : '_';
        buf[1] = '\0';
        C2D_TextParse(&text, text_buf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 90.0f + i * 20.0f, 54.0f, 0.91f, 0.8f, 0.8f, C2D_Color32(0, 104, 0, 255));
    }
}

static bool digicode_init(void) {
    if (!gfxmap_load_assets("romfs:/minigames/digicode", &assets)) {
        return false;
    }
    img_background = gfxmap_get_image(assets, "gfx_background_idx");
    if (!text_buf) {
        text_buf = C2D_TextBufNew(256);
    }
    position = 0;
    memset(entered_code, 0, sizeof(entered_code));
    return true;
}

static void digicode_close(void) {
    if (assets) {
        C2D_SpriteSheetFree(assets);
        assets = NULL;
    }
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
}

MiniGame digicode = {
    .init = digicode_init,
    .draw = digicode_draw,
    .update = digicode_update,
    .close = digicode_close
};
