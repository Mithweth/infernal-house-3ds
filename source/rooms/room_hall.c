// room_hall.c
#include <citro2d.h>

#include "game.h"
#include "audio.h"
#include "room_hall.h"
#include "room_diningroom.h"
#include "gfx_hall.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_left_closet_opened;
static C2D_Image img_left_closet_empty;
static C2D_Image img_right_closet_opened;
static C2D_Image img_right_closet_empty;
static C2D_Image img_carpet_moved;
static C2D_Image img_message;


static void left_closet_action(void) {
    if (gamestate_is_hall_left_closet_opened()) {
        sfx_play("romfs:/audio/closet_close.raw");
    } else {
        sfx_play("romfs:/audio/closet_open.raw");
    }
    gamestate_open_hall_left_closet(!gamestate_is_hall_left_closet_opened());
}

static void right_closet_action(void) {
    if (gamestate_is_hall_right_closet_opened()) {
        sfx_play("romfs:/audio/closet_close.raw");
    } else {
        sfx_play("romfs:/audio/closet_open.raw");
    }
    gamestate_open_hall_right_closet(!gamestate_is_hall_right_closet_opened());
}

static void carpet_action(void) {
    gamestate_move_hall_carpet(!gamestate_is_hall_carpet_moved());
}

static void message_action(void) {
    inventory_add(ITEM_MESSAGE);
}

static bool message_is_active(void) {
    return gamestate_is_hall_carpet_moved() && !inventory_has(ITEM_MESSAGE);
}

static bool left_closet_contents_is_active(void) {
    return gamestate_is_hall_left_closet_opened() && !inventory_has(ITEM_SCREWDRIVER);
}

static bool right_closet_contents_is_active(void) {
    return gamestate_is_hall_right_closet_opened() && !inventory_has(ITEM_FLASHLIGHT);
}

static void left_closet_contents_action(void) {
    inventory_add(ITEM_SCREWDRIVER);
    inventory_add(ITEM_MAGNIFYING_GLASS);
}

static void right_closet_contents_action(void) {
    inventory_add(ITEM_FLASHLIGHT);
    inventory_add(ITEM_BINOCULARS);
}


static Hotspot hotspots[] = {
    {
        .x = 0,
        .y = 144,
        .width = 36,
        .height = 70,
        .text_id = "HALL_LEFT_CLOSET_CONTENTS",
        .is_active = left_closet_contents_is_active,
        .action = left_closet_contents_action
    },
    {
        .x = 37,
        .y = 135,
        .width = 36,
        .height = 70,
        .text_id = "HALL_RIGHT_CLOSET_CONTENTS",
        .is_active = right_closet_contents_is_active,
        .action = right_closet_contents_action
    },
    {
        .x = 78,
        .y = 50,
        .width = 25,
        .height = 40,
        .text_id = "HALL_CROSS",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 35,
        .y = 40,
        .width = 34,
        .height = 47,
        .text_id = "HALL_WALL",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 0,
        .y = 130,
        .width = 20,
        .height = 12,
        .text_id = "HALL_ASHTRAY",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 25,
        .y = 95,
        .width = 15,
        .height = 40,
        .text_id = "HALL_CANDLE",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 180,
        .y = 75,
        .width = 50,
        .height = 25,
        .text_id = "HALL_HANGER",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 250,
        .y = 30,
        .width = 70,
        .height = 100,
        .text_id = "HALL_PAINTING",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 0,
        .y = 154,
        .width = 36,
        .height = 70,
        .text_id = "HALL_CLOSET",
        .is_active = NULL,
        .action = left_closet_action
    },
    {
        .x = 37,
        .y = 135,
        .width = 36,
        .height = 60,
        .text_id = "HALL_CLOSET",
        .is_active = NULL,
        .action = right_closet_action
    },
    {
        .x = 124,
        .y = 200,
        .width = 28,
        .height = 20,
        .text_id = "HALL_MESSAGE",
        .is_active = message_is_active,
        .action = message_action
    },
    {
        .x = 46,
        .y = 175,
        .width = 200,
        .height = 65,
        .text_id = "HALL_CARPET",
        .is_active = NULL,
        .action = carpet_action
    }
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_hall.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_hall_bg_idx);
    img_right_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_hall_right_closet_opened_idx);
    img_right_closet_empty = C2D_SpriteSheetGetImage(room_scene, gfx_hall_right_closet_empty_idx);
    img_left_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_hall_left_closet_opened_idx);
    img_left_closet_empty = C2D_SpriteSheetGetImage(room_scene, gfx_hall_left_closet_empty_idx);
    img_carpet_moved = C2D_SpriteSheetGetImage(room_scene, gfx_hall_carpet_moved_idx);
    img_message = C2D_SpriteSheetGetImage(room_scene, gfx_hall_message_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);

    if (gamestate_is_hall_right_closet_opened()) {
        if (inventory_has(ITEM_FLASHLIGHT)) {
            C2D_DrawImageAt(img_right_closet_empty, 39.0f, 130.0f, 0.1f, NULL, 1.0f, 1.0f);
        } else {
            C2D_DrawImageAt(img_right_closet_opened, 39.0f, 129.0f, 0.1f, NULL, 1.0f, 1.0f);
        }
    }
    if (gamestate_is_hall_left_closet_opened()) {
        if (inventory_has(ITEM_SCREWDRIVER)) {
            C2D_DrawImageAt(img_left_closet_empty, 0.0f, 144.0f, 0.1f, NULL, 1.0f, 1.0f);
        } else {
            C2D_DrawImageAt(img_left_closet_opened, 0.0f, 144.0f, 0.1f, NULL, 1.0f, 1.0f);
        }
    }
    if (gamestate_is_hall_carpet_moved()) {
        C2D_DrawImageAt(img_carpet_moved, 22.0f, 169.0f, 0.2f, NULL, 1.0f, 1.0f);
        if (!inventory_has(ITEM_MESSAGE)) {
            C2D_DrawImageAt(img_message, 124.0f, 200.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_north(void) {
    game_set_room(&dining_room);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_north);
}

Room hall = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = {.action = north_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};