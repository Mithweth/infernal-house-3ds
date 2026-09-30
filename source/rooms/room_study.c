// room_study.c

#include <citro2d.h>
#include "game.h"
#include "room_study.h"
#include "room_library.h"
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

static bool book_is_active(void) {
    return !inventory_has("BOOK");
}

static void book_action(void) {
    inventory_add("BOOK");
}

static bool dark_room_use_item(const char *id) {
    game_show_message("STUDY_USE_NO_LIGHT");
    return true;
}

static bool lamp_use_item(const char *id) {
    if (strcmp(id, "LIGHTBULB") == 0) {
        inventory_remove("LIGHTBULB");
        gamestate_set("study_lightbulb_screwed");
        game_show_message("STUDY_USE_LIGHTBULB");
        return true;
    }
    return false;
}

static bool dark_room_is_active(void) {
    return !gamestate_get("study_lights_on");
}

static void switch_action(void) {
    if (!gamestate_get("study_lightbulb_screwed")) {
        game_show_message("STUDY_NO_LIGHTBULB");
    } else {
        gamestate_set("study_lights_on");
        game_show_message("STUDY_LIGHT");
    }
}

static void top_drawer_action(void) {
    gamestate_set("study_top_drawer_opened");
}

static bool top_drawer_is_opened(void) {
    return gamestate_get("study_top_drawer_opened");
}

static void middle_drawer_action(void) {
    gamestate_set("study_middle_drawer_opened");
}

static bool middle_drawer_is_active(void) {
    return gamestate_get("study_middle_drawer_opened") && !inventory_has("SELLOTAPE");
}

static void middle_drawer_take(void) {
    inventory_add("SELLOTAPE");
    inventory_add("LETTER");
}

static void bottom_drawer_action(void) {
    gamestate_set("study_bottom_drawer_opened");
}

static bool bottom_drawer_is_opened(void) {
    return gamestate_get("study_bottom_drawer_opened");
}

static void board_action(void) {
    gamestate_set("study_board_pulled");
}

static bool board_is_pulled(void) {
    return gamestate_get("study_board_pulled");
}

static bool paper_is_active(void) {
    return gamestate_get("study_board_pulled") && !inventory_has("PAPER");
}

static void paper_action(void) {
    inventory_add("PAPER");
}

static bool closet_opened(void) {
    return gamestate_get("study_closet_opened");
}

static void closet_action(void) {
    gamestate_set("study_closet_opened");
}

static bool closet_use_item(const char *id) {
    if (strcmp(id, "KEY_ONE") == 0) {
        game_show_message("STUDY_CLOSET_WITH_KEY");
        return true;
    }
    return false;
}

static Hotspot hotspots[] = {
    {
        .x = 127,
        .y = 47,
        .width = 16,
        .height = 15,
        .id = "STUDY_SWITCH",
        .condition = dark_room_is_active,
        .action = switch_action
    },
    {
        .x = 120,
        .y = 61,
        .width = 29,
        .height = 20,
        .id = "STUDY_LAMP",
        .condition = dark_room_is_active,
        .use_item = lamp_use_item
    },
    {
        .x = 0,
        .y = 0,
        .width = 320,
        .height = 240,
        .id = "STUDY_DARK",
        .message_id = "STUDY_MESSAGE_NO_LIGHT",
        .condition = dark_room_is_active,
        .use_item = dark_room_use_item
    },
    {
        .x = 41,
        .y = 116,
        .width = 44,
        .height = 16,
        .id = "STUDY_BOOK",
        .message_id = "STUDY_BOOK_MESSAGE",
        .condition = book_is_active,
        .action = book_action
    },
    {
        .x = 22,
        .y = 140,
        .width = 50,
        .height = 21,
        .id = "STUDY_DRAWER",
        .message_id = "STUDY_DRAWER_TOP",
        .condition = top_drawer_is_opened,
        .action = top_drawer_action
    },
    {
        .x = 22,
        .y = 140,
        .width = 50,
        .height = 21,
        .id = "STUDY_DRAWER",
        .action = top_drawer_action
    },
    {
        .x = 21,
        .y = 160,
        .width = 50,
        .height = 21,
        .id = "STUDY_DRAWER",
        .message_id = "STUDY_DRAWER_MIDDLE",
        .condition = middle_drawer_is_active,
        .action = middle_drawer_take
    },
    {
        .x = 21,
        .y = 160,
        .width = 50,
        .height = 21,
        .id = "STUDY_DRAWER",
        .action = middle_drawer_action
    },
    {
        .x = 21,
        .y = 181,
        .width = 50,
        .height = 23,
        .id = "STUDY_DRAWER",
        .message_id = "STUDY_DRAWER_BOTTOM",
        .condition = bottom_drawer_is_opened,
        .action = bottom_drawer_action
    },
    {
        .x = 21,
        .y = 181,
        .width = 50,
        .height = 23,
        .id = "STUDY_DRAWER",
        .action = bottom_drawer_action
    },
    {
        .x = 100,
        .y = 141,
        .width = 37,
        .height = 12,
        .id = "STUDY_PAPER",
        .message_id = "STUDY_PAPER_EXAMINE",
        .condition = paper_is_active,
        .action = paper_action
    },
    {
        .x = 74,
        .y = 139,
        .width = 85,
        .height = 17,
        .id = "STUDY_BOARD",
        .condition = board_is_pulled,
        .action = board_action
    },
    {
        .x = 80,
        .y = 137,
        .width = 79,
        .height = 10,
        .id = "STUDY_BOARD",
        .action = board_action
    },
    {
        .x = 13,
        .y = 23,
        .width = 32,
        .height = 80,
        .id = "STUDY_CALENDAR"
    },
    {
        .x = 228,
        .y = 77,
        .width = 22,
        .height = 46,
        .id = "STUDY_FOLDER"
    },
    {
        .x = 250,
        .y = 79,
        .width = 64,
        .height = 44,
        .id = "STUDY_BOOK"
    },
    {
        .x = 185,
        .y = 166,
        .width = 25,
        .height = 38,
        .message_id = "STUDY_BIN_MESSAGE",
        .id = "STUDY_BIN"
    },
    {
        .x = 215,
        .y = 3,
        .width = 102,
        .height = 65,
        .message_id = "STUDY_CLOSET_OPENED",
        .id = "STUDY_CLOSET",
        .condition = closet_opened,
        .action = closet_action
    },
    {
        .x = 215,
        .y = 3,
        .width = 102,
        .height = 65,
        .id = "STUDY_CLOSET",
        .action = closet_action
    },
    {
        .x = 215,
        .y = 128,
        .width = 102,
        .height = 63,
        .message_id = "STUDY_CLOSET_BOTTOM",
        .id = "STUDY_CLOSET",
        .use_item = closet_use_item
    }
};

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

static void south_action(void) {
    game_set_room(&library);
}

Room study = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = south_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
