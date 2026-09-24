// hud.c
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <math.h>
#include "hud.h"
#include "game.h"
#include "inventory.h"
#include "gfx_hud.h"
#include "gfx_hud_t3x.h"
#include "gameover.h"
#include "lang.h"

#define TIME_MAX_SECONDS 3600
static C2D_SpriteSheet hud_assets;
static C2D_Image img_background;
static C2D_Image img_inventory;
static C2D_Image img_directions;
static C2D_Image img_arrow_n;
static C2D_Image img_arrow_e;
static C2D_Image img_arrow_s;
static C2D_Image img_arrow_w;
static C2D_Image img_arrow_ne;
static C2D_Image img_arrow_nw;
static C2D_Image img_arrow_sw;
static C2D_Image img_arrow_se;
static C2D_TextBuf text_buf;
static C2D_Text text;
static u64 elapsed_time;
static u64 last_time;

#ifdef DEBUG
static C2D_TextBuf debug_buf;
static C2D_Text debug_text;
static void debug_draw(void) {
    if (!debug_buf) {
        debug_buf = C2D_TextBufNew(256);
    }
    char str[64];
    snprintf(str, sizeof(str), "elapsed: %llu", elapsed_time);
    C2D_TextBufClear(debug_buf);
    C2D_TextParse(&debug_text, debug_buf, str);
    C2D_TextOptimize(&debug_text);
    C2D_DrawText(&debug_text, C2D_WithColor, 10, 10, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 0, 0, 255));
}
#endif

void timer_start(void) {
    elapsed_time = 0;
    last_time = osGetTime();
}

void timer_update(void) {
    u64 now = osGetTime();
    elapsed_time += now - last_time;
    last_time = now;
}

void timer_resume(void) {
    last_time = osGetTime();
}

static void draw_bold_text(C2D_Text *text, float x, float y, float z, float sx, float sy, u32 color) {
    C2D_DrawText(text, C2D_WithColor, x, y, z, sx, sy, color);
    C2D_DrawText(text, C2D_WithColor, x + 1.0f, y, z, sx, sy, color);
}

static void selected_item_draw(void) {
    const Item *item = inventory_get_selected();
    if (item) {
        C2D_DrawImageAt(item->image, 20, 169, 0.2f, NULL, 0.5f, 0.5f);
        C2D_TextBufClear(text_buf);
        C2D_TextParse(&text, text_buf, lang_get(item->name_id));
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 100.0f, 172.0f, 0.5f, 0.5f, 0.5f, C2D_Color32(192, 192, 192, 255));
    }
}

static void target_draw(void) {
    const char* target_name = game_target_name();
    if (target_name) {
        C2D_TextParse(&text, text_buf, lang_get(target_name));
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 100.0f, 208.0f, 0.7f, 0.5f, 0.5f, C2D_Color32(192, 192, 192, 255));
    }
}

static void background_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    C2D_DrawImageAt(img_inventory, 7.0f, 49.0f, 0.0f, NULL, 1.0f, 1.0f);
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get("HUD_OBJECT"));
    C2D_TextOptimize(&text);
    draw_bold_text(&text, 60.0f, 172.0f, 0.3f, 0.5f, 0.5f, C2D_Color32(128, 128, 0, 255));
    C2D_TextParse(&text, text_buf, lang_get("HUD_TARGET"));
    C2D_TextOptimize(&text);
    draw_bold_text(&text, 60.0f, 208.0f, 0.3f, 0.5f, 0.5f, C2D_Color32(128, 128, 0, 255));
    C2D_TextParse(&text, text_buf, lang_get("HUD_INVENTORY"));
    C2D_TextOptimize(&text);
    draw_bold_text(&text, 115.0f, 52.0f, 0.3f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
}


static void timer_draw() {
    int total_seconds = TIME_MAX_SECONDS - elapsed_time / 1000;

    int hours   = (total_seconds / 3600) % 100;
    int minutes = (total_seconds / 60) % 60;
    int seconds = total_seconds % 60;
    char str[16];
    snprintf(str, sizeof(str), "%02d:%02d:%02d", hours, minutes, seconds);
    C2D_TextParse(&text, text_buf, str);
    C2D_TextOptimize(&text);
    draw_bold_text(&text, 335.0f, 23.0f, 0.7f, 0.4f, 0.4f, C2D_Color32(128, 128, 128, 255));
}

static void movement_draw(void) {
    C2D_DrawImageAt(img_directions, 303.0f, 51.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (game_can_move_north()) {
        C2D_DrawImageAt(img_arrow_n, 339.0f, 62.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_northeast()) {
        C2D_DrawImageAt(img_arrow_ne, 363.0f, 74.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_east()) {
        C2D_DrawImageAt(img_arrow_e, 369.0f, 98.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_southeast()) {
        C2D_DrawImageAt(img_arrow_se, 362.0f, 121.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_south()) {
        C2D_DrawImageAt(img_arrow_s, 338.0f, 129.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_southwest()) {
        C2D_DrawImageAt(img_arrow_sw, 314.0f, 120.0f, 0.2f, NULL, 1.0f, 1.0f);
    }

    if (game_can_move_west()) {
        C2D_DrawImageAt(img_arrow_w, 306.0f, 97.0f, 0.2f, NULL, 1.0f, 1.0f);
        
    }
    if (game_can_move_northwest()) {
        C2D_DrawImageAt(img_arrow_nw, 314.0f, 73.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
}

void hud_init(void) {
    inventory_init();
    hud_assets = C2D_SpriteSheetLoadFromMem(gfx_hud_t3x, gfx_hud_t3x_size);
    img_background = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_background_idx);
    img_inventory = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_inventory_idx);
    img_directions = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_directions_idx);
    img_arrow_n = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_n_idx);
    img_arrow_e = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_e_idx);
    img_arrow_s = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_s_idx);
    img_arrow_w = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_w_idx);
    img_arrow_ne = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_ne_idx);
    img_arrow_nw = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_nw_idx);
    img_arrow_se = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_se_idx);
    img_arrow_sw = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_sw_idx);
    if (!text_buf) {
        text_buf = C2D_TextBufNew(1024);
    }
    timer_start();
}

void hud_reset(void) {
    timer_start();
}

void hud_update(void) {
    timer_update();
    if (elapsed_time / 1000 > TIME_MAX_SECONDS) {
        game_over(GAMEOVER_TIMEUP);
    }
}

void hud_close(void) {
    inventory_close();
    if (text_buf) {
        text_buf = C2D_TextBufNew(1024);
    }
    C2D_SpriteSheetFree(hud_assets);
}

void hud_draw(void) {
    background_draw();
    target_draw();
    selected_item_draw();
    inventory_draw();
    movement_draw();
    timer_draw();
#ifdef DEBUG
    debug_draw();
#endif
}
