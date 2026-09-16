#include <3ds.h>
#include <citro2d.h>
#include "hud.h"
#include "game.h"
#include "inventory.h"

static void background_draw(void) {
    u32 background = C2D_Color32(8, 12, 30, 255);
    u32 border     = C2D_Color32(190, 145, 40, 255);
    u32 panel      = C2D_Color32(18, 28, 55, 255);

    C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, 400.0f, 240.0f, background);
    C2D_DrawRectSolid(5.0f, 5.0f, 0.1f, 390.0f, 160.0f, border);
    C2D_DrawRectSolid(7.0f, 7.0f, 0.2f, 386.0f, 156.0f, panel);
    C2D_DrawRectSolid(5.0f, 170.0f, 0.1f, 190.0f, 65.0f, border);
    C2D_DrawRectSolid(7.0f, 172.0f, 0.2f, 186.0f, 61.0f, panel);
    C2D_DrawRectSolid(205.0f, 170.0f, 0.1f, 190.0f, 65.0f, border);
    C2D_DrawRectSolid(207.0f, 172.0f, 0.2f, 186.0f, 61.0f, panel);
}

static void movement_draw(void) {
	u32 panel      = C2D_Color32(190, 145, 140, 255);
	if (game_can_move_up()) {
        C2D_DrawRectSolid(20.0f, 0.0f, 0.5f, 20.0f, 20.0f, panel);
    }

    if (game_can_move_down()) {
        C2D_DrawRectSolid(20.0f, 40.0f, 0.5f, 20.0f, 20.0f, panel);
    }

    if (game_can_move_left()) {
        C2D_DrawRectSolid(0.0f, 20.0f, 0.5f, 20.0f, 20.0f, panel);
    }

    if (game_can_move_right()) {
        C2D_DrawRectSolid(40.0f, 20.0f, 0.5f, 20.0f, 20.0f, panel);
    }
    printf("\n");
}

void hud_init(void) {
	inventory_init();
}

void hud_close(void) {
	inventory_close();
}

void hud_draw(void) {
	background_draw();
	inventory_draw();
	movement_draw();
	// timer_draw();
}
