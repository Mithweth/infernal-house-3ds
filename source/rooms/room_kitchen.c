#include <citro2d.h>
#include "game.h"
#include "gamestate.h"
#include "room_kitchen.h"
#include "room_corridor.h"
#include "gfx_kitchen.h"
#include "inventory.h"
#include "audio.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_drawer_opened;
static C2D_Image img_top_closet_opened;
static C2D_Image img_bottom_closet_opened;
static C2D_Image img_closet_below_opened;
static C2D_Image img_fridge_opened;
static C2D_Image img_dishwasher_opened;
static C2D_Image img_oven_opened;


static void refrigerator_action(void) {
    gamestate_open_kitchen_refrigerator(!gamestate_is_kitchen_refrigerator_opened());
}

static bool refrigerator_is_active(void) {
    return gamestate_is_kitchen_refrigerator_opened();
}

static void below_closet_action(void) {
    gamestate_open_kitchen_below_closet(!gamestate_is_kitchen_below_closet_opened());
}

static bool below_closet_is_active(void) {
    return gamestate_is_kitchen_below_closet_opened();
}

static void oven_action(void) {
    gamestate_open_kitchen_oven(!gamestate_is_kitchen_oven_opened());
}

static void dishwasher_action(void) {
    gamestate_open_kitchen_dishwasher(!gamestate_is_kitchen_dishwasher_opened());
}

static bool dishwasher_is_active(void) {
    return gamestate_is_kitchen_dishwasher_opened();
}

static void top_closet_action(void) {
    gamestate_open_kitchen_top_closet(!gamestate_is_kitchen_top_closet_opened());
}

static bool top_closet_is_active(void) {
    return gamestate_is_kitchen_top_closet_opened();
}

static void bottom_closet_action(void) {
    gamestate_open_kitchen_bottom_closet(!gamestate_is_kitchen_bottom_closet_opened());
}

static bool bottom_closet_is_active(void) {
    return gamestate_is_kitchen_bottom_closet_opened();
}

static void drawer_action(void) {
    gamestate_open_kitchen_drawer(!gamestate_is_kitchen_drawer_opened());
}

static bool drawer_is_active(void) {
    return gamestate_is_kitchen_drawer_opened();
}

static Hotspot hotspots[] = {
    {
        .x = 18,
        .y = 60,
        .width = 25,
        .height = 20,
        .text_id = "KITCHEN_REFRIGERATOR_OPENED_1",
        .is_active = refrigerator_is_active,
        .action = refrigerator_action
    },
    {
        .x = 18,
        .y = 80,
        .width = 25,
        .height = 19,
        .text_id = "KITCHEN_REFRIGERATOR_OPENED_2",
        .is_active = refrigerator_is_active,
        .action = refrigerator_action
    },
    {
        .x = 18,
        .y = 100,
        .width = 25,
        .height = 19,
        .text_id = "KITCHEN_REFRIGERATOR_OPENED_3",
        .is_active = refrigerator_is_active,
        .action = refrigerator_action
    },
    {
        .x = 18,
        .y = 118,
        .width = 25,
        .height = 21,
        .text_id = "KITCHEN_REFRIGERATOR_OPENED_4",
        .is_active = refrigerator_is_active,
        .action = refrigerator_action
    },
    {
        .x = 18,
        .y = 142,
        .width = 25,
        .height = 48,
        .text_id = "KITCHEN_REFRIGERATOR_OPENED_5",
        .is_active = refrigerator_is_active,
        .action = refrigerator_action
    },
    {
        .x = 0,
        .y = 40,
        .width = 48,
        .height = 173,
        .text_id = "KITCHEN_REFRIGERATOR",
        .action = refrigerator_action
    },
    {
        .x = 49,
        .y = 130,
        .width = 55,
        .height = 64,
        .text_id = "KITCHEN_CLOSET_BELOW_OPENED",
        .is_active = below_closet_is_active,
        .action = below_closet_action
    },
    {
        .x = 49,
        .y = 130,
        .width = 55,
        .height = 64,
        .text_id = "KITCHEN_CLOSET",
        .action = below_closet_action
    },
    {
        .x = 133,
        .y = 131,
        .width = 47,
        .height = 63,
        .text_id = "KITCHEN_OVEN",
        .action = oven_action
    },
    {
        .x = 183,
        .y = 131,
        .width = 60,
        .height = 70,
        .text_id = "KITCHEN_DISHWASHER_OPENED",
        .is_active = dishwasher_is_active,
        .action = dishwasher_action
    },
    {
        .x = 183,
        .y = 131,
        .width = 60,
        .height = 70,
        .text_id = "KITCHEN_DISHWASHER",
        .action = dishwasher_action
    },
    {
        .x = 260,
        .y = 2,
        .width = 53,
        .height = 119,
        .text_id = "KITCHEN_TOP_CLOSET_OPENED",
        .is_active = top_closet_is_active,
        .action = top_closet_action
    },
    {
        .x = 260,
        .y = 2,
        .width = 53,
        .height = 119,
        .text_id = "KITCHEN_CLOSET",
        .action = top_closet_action
    },
    {
        .x = 262,
        .y = 120,
        .width = 51,
        .height = 19,
        .text_id = "KITCHEN_DRAWER_OPENED",
        .is_active = drawer_is_active,
        .action = drawer_action
    },
    {
        .x = 262,
        .y = 120,
        .width = 51,
        .height = 19,
        .text_id = "KITCHEN_DRAWER",
        .action = drawer_action
    },
    {
        .x = 264,
        .y = 138,
        .width = 48,
        .height = 64,
        .text_id = "KITCHEN_BOTTOM_CLOSET_OPENED",
        .is_active = bottom_closet_is_active,
        .action = bottom_closet_action
    },
    {
        .x = 264,
        .y = 138,
        .width = 48,
        .height = 64,
        .text_id = "KITCHEN_CLOSET",
        .action = bottom_closet_action
    },
    {
        .x = 105,
        .y = 49,
        .width = 50,
        .height = 42,
        .text_id = "KITCHEN_TOWELS"
    },
    {
        .x = 176,
        .y = 47,
        .width = 74,
        .height = 46,
        .text_id = "KITCHEN_TOOLS"
    },
    {
        .x = 180,
        .y = 94,
        .width = 36,
        .height = 30,
        .text_id = "KITCHEN_TOOLS"
    }
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_kitchen.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_bg_idx);
    img_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_drawer_opened_idx);
    img_top_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_top_closet_opened_idx);
    img_bottom_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_bottom_closet_opened_idx);
    img_closet_below_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_closet_below_opened_idx);
    img_fridge_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_fridge_opened_idx);
    img_dishwasher_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_dishwasher_opened_idx);
    img_oven_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_oven_opened_idx);

}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_kitchen_refrigerator_opened()) {
        C2D_DrawImageAt(img_fridge_opened, 0.0f, 40.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_below_closet_opened()) {
        C2D_DrawImageAt(img_closet_below_opened, 49.0f, 128.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_oven_opened()) {
        C2D_DrawImageAt(img_oven_opened, 109.0f, 137.0f, 0.4f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_dishwasher_opened()) {
        C2D_DrawImageAt(img_dishwasher_opened, 166.0f, 128.0f, 0.5f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_top_closet_opened()) {
        C2D_DrawImageAt(img_top_closet_opened, 260.0f, 0.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_drawer_opened()) {
        C2D_DrawImageAt(img_drawer_opened, 252.0f, 120.0f, 0.4f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_bottom_closet_opened()) {
        C2D_DrawImageAt(img_bottom_closet_opened, 262.0f, 136.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_southwest(void) {
    game_set_room(&corridor);
}

static void southwest_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_southwest);
}

Room kitchen = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .southwest = southwest_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};