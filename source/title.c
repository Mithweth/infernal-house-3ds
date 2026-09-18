// title.c

#include <3ds.h>
#include <citro2d.h>
#include "lang.h"
#include "audio.h"
#include "gfx_title.h"
#include "game.h"

typedef enum {
    TITLE_INTRO,
    TITLE_GAME,
    TITLE_CONTROLS,
    TITLE_CREDITS,
    TITLE_COUNT
} TitleChoice;

typedef enum {
    OPTION_NONE,
    OPTION_CONTROLS,
    OPTION_CREDITS
} OptionChoice;

static TitleChoice selected;
static OptionChoice option = OPTION_NONE;

static C2D_SpriteSheet title_assets;
static C2D_Image img_background;

static const char *choices[] = {
    "TITLE_INTRO",
    "TITLE_GAME",
    "TITLE_CONTROLS",
    "TITLE_CREDITS"
};

static C2D_TextBuf text_buf;
static C2D_Text text[TITLE_COUNT];

void title_init(void) {
    title_assets = C2D_SpriteSheetLoad("romfs:/gfx/gfx_title.t3x");
    img_background = C2D_SpriteSheetGetImage(title_assets, gfx_title_background_idx);
    selected = TITLE_INTRO;
    text_buf = C2D_TextBufNew(256);
    for (int i = 0; i < TITLE_COUNT; i++) {
        C2D_TextParse(&text[i], text_buf, lang_get(choices[i]));
        C2D_TextOptimize(&text[i]);
    }
}

void title_update(u32 keys) {
    if (option == OPTION_NONE) {
        if (keys & KEY_UP) {
            if (selected == 0) {
                selected = TITLE_COUNT - 1;
            } else {
                selected--;
            }
            sfx_play("romfs:/audio/title_select.raw");
        }

        if (keys & KEY_DOWN) {
            selected++;
            if (selected >= TITLE_COUNT) {
                selected = 0;
            }
            sfx_play("romfs:/audio/title_select.raw");
        }
    }

    if (!(keys & KEY_A)) {
        return;
    }

    sfx_play("romfs:/audio/title_choice.raw");

    if (option != OPTION_NONE) {
        option = OPTION_NONE;
        selected = TITLE_INTRO;
        return;
    }
    switch (selected) {
        case TITLE_INTRO:
            //game_intro();
            break;

        case TITLE_GAME:
            game_start();
            break;

        case TITLE_CONTROLS:
            option = OPTION_CONTROLS;
            break;

        case TITLE_CREDITS:
            option = OPTION_CREDITS;
            break;

        default:
            break;
    }
}

void title_draw_top(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

void title_draw_bottom(void) {
    if (option == OPTION_NONE) {
        u32 color;
        for (int i = 0; i < TITLE_COUNT; i++) {
            if (selected == i) {
                color = C2D_Color32(146, 146, 146, 255);
            } else {
                color = C2D_Color32(64, 64, 64, 255);
            }
            C2D_DrawText(&text[i], C2D_WithColor | C2D_AlignCenter, 160.0f, (i * 30) + 70.0f, 0.5f, 0.65f, 0.65f, color);
        }
    } else {
        for (int i = 0; i < TITLE_COUNT; i++) {
            C2D_DrawText(&text[i], C2D_WithColor | C2D_AlignCenter, 160.0f, (i * 20) + 180.0f, 0.5f, 0.65f, 0.65f, C2D_Color32(i * 64, 255 - (i * 64), 146, 255));
        }
    }
}

void title_close(void) {
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    if (title_assets) {
        C2D_SpriteSheetFree(title_assets);
        title_assets = NULL;
    }
}