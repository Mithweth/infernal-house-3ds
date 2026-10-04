// title.c
// Title screen implementation. The menu entries are translation keys looked
// up on every frame, so switching language (lang_next) takes effect at once.
// Assets are loaded by title_init and released by title_close (called when
// a game or the intro starts).

#include <3ds.h>
#include <citro2d.h>
#include "lang.h"
#include "audio.h"
#include "game.h"
#include "gfxmap.h"

typedef enum {
    TITLE_LANG,
    TITLE_INTRO,
    TITLE_GAME,
    TITLE_CONTROLS,
    TITLE_CREDITS,
    TITLE_COUNT
} TitleChoice;

// Sub-page currently shown instead of the menu.
typedef enum {
    OPTION_NONE,
    OPTION_CONTROLS,
    OPTION_CREDITS
} OptionChoice;

// One credits line: a translated role and an untranslated name.
typedef struct {
    const char *role_id;
    const char *name;
} Credit;

static TitleChoice selected;
static OptionChoice option = OPTION_NONE;
static C2D_TextBuf text_buf;
static C2D_Text text[TITLE_COUNT];
static C2D_SpriteSheet assets;
static C2D_Image img_background;
static C2D_Image img_lankhor;
static C2D_Image img_abutton;
static C2D_Image img_xbutton;
static C2D_Image img_analogpad;
static C2D_Image img_dpad;
static C2D_Image img_touch;
static C2D_Text version_text;

// Translation keys of the menu entries, in TitleChoice order.
static const char *choices[] = {
    "LANG_NAME",
    "TITLE_INTRO",
    "TITLE_GAME",
    "TITLE_CONTROLS",
    "TITLE_CREDITS"
};

static C2D_Text controls_text;

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
        .role_id = "TITLE_CREDITS_REMAKE_TESTER",
        .name = "Akira Langlois"
    },
    {
        .role_id = "TITLE_CREDITS_REMAKE_GRAPHISM",
        .name = "ChatGPT & GIMP"
    }
};

void title_init(void) {
    if (!gfxmap_load_assets("romfs:/gfx/title", &assets)) {
        return;
    }
    img_background = gfxmap_get_image(assets, "background");
    img_lankhor = gfxmap_get_image(assets, "lankhor");
    img_abutton = gfxmap_get_image(assets, "abutton");
    img_xbutton = gfxmap_get_image(assets, "xbutton");
    img_analogpad = gfxmap_get_image(assets, "analogpad");
    img_dpad = gfxmap_get_image(assets, "dpad");
    img_touch = gfxmap_get_image(assets, "touch");
    selected = TITLE_GAME;
    text_buf = C2D_TextBufNew(4096);
}

void title_update(u32 keys) {
    // While the controls or credits page is open, A or B only closes it.
    if (option != OPTION_NONE) {
        if (keys & (KEY_A | KEY_B)) {
            sfx_play("romfs:/audio/title_choice.raw");
            option = OPTION_NONE;
        }
        return;
    }

    // KEY_UP / KEY_DOWN match both the D-pad and the circle pad.
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
        case TITLE_LANG:
            lang_next();
            break;
        case TITLE_INTRO:
            game_intro();
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
    C2D_TextBufClear(text_buf);
    C2D_Text role;
    C2D_Text person;
    for (int i = 0; i < (sizeof(credits) / sizeof(credits[0])); i++) {
        float y = 20.0f + i * 20.0f;
        C2D_TextParse(&role, text_buf, lang_get(credits[i].role_id));
        C2D_TextParse(&person, text_buf, credits[i].name);
        C2D_TextOptimize(&role);
        C2D_TextOptimize(&person);
        C2D_DrawText(&role, C2D_WithColor, 20.0f, y, 0.5f, 0.4f, 0.4f, C2D_Color32(128, 128, 128, 255));
        C2D_DrawText(&person, C2D_WithColor | C2D_AlignRight, 300.0f, y, 0.5f, 0.4f, 0.4f, C2D_Color32(164, 164, 164, 255));
    }
    C2D_DrawImageAt(img_lankhor, 85.0f, 160.0f, 0.5f, NULL, 1.0f, 1.0f);
}

static void title_draw_controls(void) {
    C2D_DrawImageAt(img_analogpad, 10.0f, 0.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_dpad, 10.0f, 55.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_xbutton, 18.0f, 110.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_abutton, 18.0f, 150.0f, 0.3f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_touch, 10.0f, 185.0f, 0.3f, NULL, 0.9f, 0.9f);
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&controls_text, text_buf, lang_get("TITLE_CONTROLS_MOVE"));
    C2D_TextOptimize(&controls_text);
    C2D_DrawText(&controls_text, C2D_WithColor, 90.0f, 15.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_TextParse(&controls_text, text_buf, lang_get("TITLE_CONTROLS_INVENTORY"));
    C2D_TextOptimize(&controls_text);
    C2D_DrawText(&controls_text, C2D_WithColor, 90.0f, 70.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_TextParse(&controls_text, text_buf, lang_get("TITLE_CONTROLS_EXAMINE"));
    C2D_TextOptimize(&controls_text);
    C2D_DrawText(&controls_text, C2D_WithColor, 90.0f, 115.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_TextParse(&controls_text, text_buf, lang_get("TITLE_CONTROLS_USE"));
    C2D_TextOptimize(&controls_text);
    C2D_DrawText(&controls_text, C2D_WithColor, 90.0f, 155.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
    C2D_TextParse(&controls_text, text_buf, lang_get("TITLE_CONTROLS_ACTION"));
    C2D_TextOptimize(&controls_text);
    C2D_DrawText(&controls_text, C2D_WithColor, 90.0f, 200.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(224, 224, 224, 255));
}

static void title_draw_menu(void) {
    u32 color;
    for (int i = 0; i < TITLE_COUNT; i++) {
        if (selected == i) {
            color = C2D_Color32(146, 146, 146, 255);
        } else {
            color = C2D_Color32(64, 64, 64, 255);
        }
        // Each entry is drawn right after being parsed, so the buffer can be
        // cleared and reused for the next one.
        C2D_TextBufClear(text_buf);
        C2D_TextParse(&text[i], text_buf, lang_get(choices[i]));
        C2D_TextOptimize(&text[i]);
        C2D_DrawText(&text[i], C2D_WithColor | C2D_AlignCenter, 160.0f, (i * 30) + 70.0f, 0.5f, 0.65f, 0.65f, color);
    }
    C2D_TextParse(&version_text, text_buf, VERSION);
    C2D_DrawText(&version_text, C2D_WithColor | C2D_AlignRight, 320.0f, 230.0f, 0.5f, 0.35f, 0.35f, C2D_Color32(64, 64, 64, 255));
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
    if (assets) {
        C2D_SpriteSheetFree(assets);
        assets = NULL;
    }
}
