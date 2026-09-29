// room_study.c

#include <citro2d.h>
#include "game.h"
#include "room_study.h"
#include "gfx_study.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_book_taken;
static C2D_Image img_bottom_drawer_opened;
static C2D_Image img_closet_open;
static C2D_Image img_dark_background;
static C2D_Image img_middle_drawer_opened;
static C2D_Image img_paper_taken;
static C2D_Image img_sliding_board;
static C2D_Image img_top_drawer_opened;

static Hotspot hotspots[] = {};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_study.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_study_background_idx);
    img_book_taken = C2D_SpriteSheetGetImage(room_scene, gfx_study_book_taken_idx);
    img_bottom_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_study_bottom_drawer_opened_idx);
    img_closet_open = C2D_SpriteSheetGetImage(room_scene, gfx_study_closet_open_idx);
    img_dark_background = C2D_SpriteSheetGetImage(room_scene, gfx_study_dark_background_idx);
    img_middle_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_study_middle_drawer_opened_idx);
    img_paper_taken = C2D_SpriteSheetGetImage(room_scene, gfx_study_paper_taken_idx);
    img_sliding_board = C2D_SpriteSheetGetImage(room_scene, gfx_study_sliding_board_idx);
    img_top_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_study_top_drawer_opened_idx);
}

// study_top_drawer_opened TOGGLE
// study_middle_drawer_opened TOGGLE
// study_bottom_drawer_opened TOGGLE
// study_closet_opened TOGGLE
// study_board_pulled TOGGLE
// study_lights_on KEEP
// ITEM PAPER ITEM_PAPER
// ITEM BOOK ITEM_BOOK
// ITEM LETTER ITEM_LETTER

static void room_draw(void) {
    if (!gamestate_get("study_lights_on")) {
        C2D_DrawImageAt(img_dark_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        return;
    }
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (inventory_has("BOOK")) {
        C2D_DrawImageAt(img_book_taken, 41.0f, 116.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("study_top_drawer_opened")) {
        C2D_DrawImageAt(img_top_drawer_opened, 3.0f, 140.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("study_middle_drawer_opened")) {
        C2D_DrawImageAt(img_middle_drawer_opened, 11.0f, 160.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("study_bottom_drawer_opened")) {
        C2D_DrawImageAt(img_bottom_drawer_opened, 4.0f, 179.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("study_closet_opened")) {
        C2D_DrawImageAt(img_closet_open, 199.0f, 1.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("study_board_pulled")) {
        C2D_DrawImageAt(img_sliding_board, 69.0f, 137.0f, 0.1f, NULL, 1.0f, 1.0f);
        if (inventory_has("PAPER")) {
            C2D_DrawImageAt(img_paper_taken, 98.0f, 142.0f, 0.2f, NULL, 1.0f, 1.0f);
        }
    }
}


static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

Room study = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = NULL,
    .northeast = NULL,
    .east = NULL,
    .southeast = NULL,
    .south = NULL,
    .southwest = NULL,
    .west = NULL,
    .northwest = NULL,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
