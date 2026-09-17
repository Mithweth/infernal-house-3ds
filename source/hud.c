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

static C2D_SpriteSheet hud_assets;
static C2D_Image arrow;
static C2D_Image timer;
static u64 elapsed_time;
static u64 last_time;

#ifdef DEBUG
static C2D_TextBuf debug_buf;
static C2D_Text debug_text;
static void debug_draw(void)
{
	if (!debug_buf) {
		debug_buf = C2D_TextBufNew(256);
	}
    char str[64];
    snprintf(str, sizeof(str), "elapsed: %llu", elapsed_time);
    C2D_TextBufClear(debug_buf);
    C2D_TextParse(&debug_text, debug_buf, str);
    C2D_TextOptimize(&debug_text);
    C2D_DrawText(&debug_text,C2D_WithColor,10, 10, 0.9f,0.5f, 0.5f,C2D_Color32(255, 0, 0, 255));
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

static void background_draw(void) {
    u32 background = C2D_Color32(8, 12, 30, 255);
    u32 border     = C2D_Color32(190, 145, 40, 255);
    u32 panel      = C2D_Color32(18, 28, 55, 255);

    C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, 400.0f, 240.0f, background);
    C2D_DrawRectSolid(5.0f, 5.0f, 0.1f, 390.0f, 160.0f, border);
    C2D_DrawRectSolid(7.0f, 7.0f, 0.2f, 386.0f, 156.0f, panel);
    C2D_DrawRectSolid(5.0f, 170.0f, 0.1f, 65.0f, 65.0f, border);
    C2D_DrawRectSolid(7.0f, 172.0f, 0.2f, 61.0f, 61.0f, panel);
    C2D_DrawRectSolid(75.0f, 170.0f, 0.1f, 250.0f, 65.0f, border);
    C2D_DrawRectSolid(77.0f, 172.0f, 0.2f, 246.0f, 61.0f, panel);
    C2D_DrawRectSolid(330.0f, 170.0f, 0.1f, 65.0f, 65.0f, border);
    C2D_DrawRectSolid(332.0f, 172.0f, 0.2f, 61.0f, 61.0f, panel);
}

static void draw_arrow(float x, float y, float degrees) {
    C2D_DrawImageAtRotated(arrow, x, y, 0.7f, C3D_AngleFromDegrees(degrees), NULL, 1.0f, 1.0f);
}

static void timer_draw() {
    const float cx = 363.0f;
    const float cy = 203.0f;
    const float radius = 17.0f;

    C2D_DrawImageAt(timer, cx - 28, cy - 28, 0.5f, NULL, 1.0f, 1.0f);
    int minutes = elapsed_time / 60000;
    float angle = (minutes / 60.0f) * 2.0f * M_PI;

    float x = cx + sinf(angle) * radius;
    float y = cy - cosf(angle) * radius;

    C2D_DrawLine(cx, cy, C2D_Color32(0, 0, 0, 255), x,  y,  C2D_Color32(0, 0, 0, 255), 1.5f, 0.7f);
}

static void movement_draw(void) {
	if (game_can_move_up()) {
        draw_arrow(37, 185, 0);
    }

    if (game_can_move_down()) {
        draw_arrow(37, 219, 180);
    }

    if (game_can_move_left()) {
        draw_arrow(20, 202, 270);
    }

    if (game_can_move_right()) {
        draw_arrow(54, 202, 90);
    }
}

void hud_init(void) {
	inventory_init();
	hud_assets = C2D_SpriteSheetLoadFromMem(gfx_hud_t3x, gfx_hud_t3x_size);
    arrow = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_arrow_idx);
    timer = C2D_SpriteSheetGetImage(hud_assets, gfx_hud_clock_idx);
    timer_start();
}

void hud_reset(void) {
	timer_start();
}

void hud_update(void) {
	timer_update();
	if (elapsed_time / 60000 > 60) {
		game_over(GAMEOVER_TIMEUP);
	}
}

void hud_close(void) {
	inventory_close();
	C2D_SpriteSheetFree(hud_assets);
}

void hud_draw(void) {
	background_draw();
	inventory_draw();
	movement_draw();
	timer_draw();
#ifdef DEBUG
    debug_draw();
#endif
}
