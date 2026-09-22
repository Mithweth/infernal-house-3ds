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

typedef struct {
    const char *role_id;
    const char *name;
    C2D_Text role;
    C2D_Text person;
} Credit;

static TitleChoice selected;
static OptionChoice option = OPTION_NONE;
static C2D_TextBuf text_buf;
static C2D_Text text[TITLE_COUNT];
static C2D_SpriteSheet title_assets;
static C2D_Image img_background;
static C2D_Image img_lankhor;
static C2D_Image img_abutton;
static C2D_Image img_analogpad;
static C2D_Image img_dpad;
static C2D_Image img_touch;

static const char *choices[] = {
    "TITLE_INTRO",
    "TITLE_GAME",
    "TITLE_CONTROLS",
    "TITLE_CREDITS"
};

static C2D_Text controls_text[5];

static Credit credits[] = {
    {
        .role_id = "TITLE_CREDITS_ORIGINAL_PROGRAM",
        .name = "Christophe Lajoux"
    },
    {
        .role_id = "TITLE_CREDITS_ORIGINAL_HELP",
        .name = "Laurent Hiriart"
    },
    {
        .role_id = "TITLE_CREDITS_ORIGINAL_GRAPHISM",
        .name = "Thierry Port & Momo"
    },
    {
        .role_id = "TITLE_CREDITS_SCENARIO",
        .name = "Jérome Marlier"
    },
    {
        .role_id = "TITLE_CREDITS_REMAKE_PROGRAMMER",
        .name = "Jean-Baptiste Langlois"
    },
    {
        .role_id = "TITLE_CREDITS_REMAKE_GRAPHISM",
        .name = "ChatGPT & GIMP"
    }
};

void title_init(void) {
    title_assets = C2D_SpriteSheetLoad("romfs:/gfx/gfx_title.t3x");
    img_background = C2D_SpriteSheetGetImage(title_assets, gfx_title_background_idx);
    img_lankhor = C2D_SpriteSheetGetImage(title_assets, gfx_title_lankhor_idx);
    img_abutton = C2D_SpriteSheetGetImage(title_assets, gfx_title_abutton_idx);
    img_analogpad = C2D_SpriteSheetGetImage(title_assets, gfx_title_analogpad_idx);
    img_dpad = C2D_SpriteSheetGetImage(title_assets, gfx_title_dpad_idx);
    img_touch = C2D_SpriteSheetGetImage(title_assets, gfx_title_touch_idx);
    selected = TITLE_INTRO;
    text_buf = C2D_TextBufNew(1024);
    for (int i = 0; i < TITLE_COUNT; i++) {
        C2D_TextParse(&text[i], text_buf, lang_get(choices[i]));
        C2D_TextOptimize(&text[i]);
    }
    for (int i = 0; i < (sizeof(credits) / sizeof(credits[0])); i++) {
        C2D_TextParse(&credits[i].role, text_buf, lang_get(credits[i].role_id));
        C2D_TextParse(&credits[i].person, text_buf, credits[i].name);
        C2D_TextOptimize(&credits[i].role);
        C2D_TextOptimize(&credits[i].person);
    }

    C2D_TextParse(&controls_text[0], text_buf, lang_get("TITLE_CONTROLS_MOVE"));
    C2D_TextParse(&controls_text[1], text_buf, lang_get("TITLE_CONTROLS_INVENTORY"));
    C2D_TextParse(&controls_text[2], text_buf, lang_get("TITLE_CONTROLS_OBJECT"));
    C2D_TextParse(&controls_text[3], text_buf, lang_get("TITLE_CONTROLS_EXAMINE"));
    C2D_TextParse(&controls_text[4], text_buf, lang_get("TITLE_CONTROLS_ACTION"));
    C2D_TextOptimize(&controls_text[0]);
    C2D_TextOptimize(&controls_text[1]);
    C2D_TextOptimize(&controls_text[2]);
    C2D_TextOptimize(&controls_text[3]);
    C2D_TextOptimize(&controls_text[4]);
}

void title_update(u32 keys) {
    if (option != OPTION_NONE) {
        if (keys & (KEY_A | KEY_B)) {
            sfx_play("romfs:/audio/title_choice.raw");
            option = OPTION_NONE;
        }
        return;
    }

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

    if (!(keys & KEY_A)) {
        return;
    }

    sfx_play("romfs:/audio/title_choice.raw");

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

static void title_draw_credits(void) {
    for (int i = 0; i < (sizeof(credits) / sizeof(credits[0])); i++) {
        float y = 20.0f + i * 25.0f;
        C2D_DrawText(&credits[i].role, C2D_WithColor, 20.0f, y, 0.5f, 0.4f, 0.4f, C2D_Color32(128, 128, 128, 255));
        C2D_DrawText(&credits[i].person, C2D_WithColor | C2D_AlignRight, 300.0f, y, 0.5f, 0.4f, 0.4f, C2D_Color32(164, 164, 164, 255));
    }
    C2D_DrawImageAt(img_lankhor, 85.0f, 160.0f, 0.5f, NULL, 1.0f, 1.0f);
}

static void title_draw_controls(void) {
    C2D_DrawImageAt(img_analogpad, 10.0f, 10.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_dpad, 10.0f, 70.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_abutton, 18.0f, 130.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_touch, 10.0f, 176.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawText(&controls_text[0], C2D_WithColor, 90.0f, 25.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_DrawText(&controls_text[1], C2D_WithColor, 90.0f, 85.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_DrawText(&controls_text[2], C2D_WithColor, 90.0f, 135.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_DrawText(&controls_text[3], C2D_WithColor, 90.0f, 190.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_DrawText(&controls_text[4], C2D_WithColor, 90.0f, 210.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
}

static void title_draw_menu(void) {
    u32 color;
    for (int i = 0; i < TITLE_COUNT; i++) {
        if (selected == i) {
            color = C2D_Color32(146, 146, 146, 255);
        } else {
            color = C2D_Color32(64, 64, 64, 255);
        }
        C2D_DrawText(&text[i], C2D_WithColor | C2D_AlignCenter, 160.0f, (i * 30) + 70.0f, 0.5f, 0.65f, 0.65f, color);
    }
}

void title_draw_bottom(void) {
    switch (option) {
        case OPTION_CONTROLS:
            title_draw_controls();
            break;

        case OPTION_CREDITS:
            title_draw_credits();
            break;

        default:
            title_draw_menu();
            break;
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