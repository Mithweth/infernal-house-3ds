// room_bathroom.c

#include <citro2d.h>
#include "game.h"
#include "room_bathroom.h"
#include "room_firstfloor.h"
#include "gfx_bathroom.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_carpet_moved;
static C2D_Image img_doors_opened;
static C2D_Image img_drawer_opened;
static C2D_Image img_perfume_taken;
static C2D_Image img_shower_opened;
static C2D_Image img_soap_taken;

static void move_carpet_action(void) {
    gamestate_move_bathroom_carpet(!gamestate_is_bathroom_carpet_moved());
    if (gamestate_is_bathroom_carpet_moved()) {
        game_show_message("BATHROOM_CARPET_EXAMINE");
    }
}

static void shower_action(void) {
    gamestate_open_bathroom_shower_curtain(!gamestate_is_bathroom_shower_curtain_opened());
    if (gamestate_is_bathroom_shower_curtain_opened()) {
        game_show_message("BATHROOM_SHOWER_EXAMINE");
    }
}

static bool perfume_is_active(void) {
    return !inventory_has(ITEM_PERFUME);
}

static void perfume_action(void) {
    inventory_add(ITEM_PERFUME);
}

static void use_paper_action(void) {
    game_show_message("BATHROOM_TOILET_PAPER_EXAMINE");
}

static void check_carpet_action(void) {
    game_show_message("BATHROOM_CARPET_EXAMINE");
}

static void closet_action(void) {
    gamestate_open_bathroom_closet_doors(!gamestate_are_bathroom_closet_doors_opened());
}

static void drawer_action(void) {
    gamestate_open_bathroom_closet_drawer(!gamestate_is_bathroom_closet_drawer_opened());
}

static void drawer_take(void) {
    inventory_add(ITEM_TOOTHPASTE);
    inventory_add(ITEM_COMB);
    inventory_add(ITEM_RAZOR);
}

static bool drawer_is_active(void) {
    return gamestate_is_bathroom_closet_drawer_opened() && !inventory_has(ITEM_TOOTHPASTE);
}

static void soap_action(void) {
    inventory_add(ITEM_SOAP);
}

static bool soap_is_active(void) {
    return !inventory_has(ITEM_SOAP);
}

static Hotspot hotspots[] = {
    {
        .x = 216,
        .y = 103,
        .width = 13,
        .height = 16,
        .id = "BATHROOM_PERFUME",
        .is_active = perfume_is_active,
        .action = perfume_action
    },
    {
        .x = 252,
        .y = 131,
        .width = 22,
        .height = 18,
        .id = "BATHROOM_TOILET_PAPER",
        .action = use_paper_action
    },
    {
        .x = 252,
        .y = 126,
        .width = 58,
        .height = 88,
        .id = "BATHROOM_TOILETS"
    },
    {
        .x = 181,
        .y = 119,
        .width = 52,
        .height = 18,
        .id = "BATHROOM_DRAWER",
        .message_id = "BATHROOM_DRAWER_EXAMINE",
        .is_active = drawer_is_active,
        .action = drawer_take
    },
    {
        .x = 181,
        .y = 119,
        .width = 52,
        .height = 18,
        .id = "BATHROOM_DRAWER",
        .action = drawer_action
    },
    {
        .x = 181,
        .y = 113,
        .width = 77,
        .height = 79,
        .id = "BATHROOM_CLOSET",
        .message_id = "BATHROOM_CLOSET_EXAMINE",
        .is_active = gamestate_are_bathroom_closet_doors_opened,
        .action = closet_action
    },
    {
        .x = 181,
        .y = 113,
        .width = 77,
        .height = 79,
        .id = "BATHROOM_CLOSET",
        .action = closet_action
    },
    {
        .x = 92,
        .y = 47,
        .width = 37,
        .height = 55,
        .id = "BATHROOM_MIRROR"
    },
    {
        .x = 86,
        .y = 106,
        .width = 52,
        .height = 27,
        .id = "BATHROOM_SINK"
    },
    {
        .x = 129,
        .y = 91,
        .width = 12,
        .height = 10,
        .id = "BATHROOM_SOAP",
        .is_active = soap_is_active,
        .action = soap_action
    },
    {
        .x = 85,
        .y = 174,
        .width = 57,
        .height = 17,
        .id = "BATHROOM_CARPET",
        .action = check_carpet_action
    },
    {
        .x = 230,
        .y = 208,
        .width = 72,
        .height = 25,
        .id = "BATHROOM_CARPET",
        .action = check_carpet_action
    },
    {
        .x = 1,
        .y = 193,
        .width = 74,
        .height = 25,
        .id = "BATHROOM_CARPET",
        .action = move_carpet_action
    },
    {
        .x = 4,
        .y = 28,
        .width = 74,
        .height = 163,
        .id = "BATHROOM_SHOWER",
        .action = shower_action
    },
    {
        .x = 146,
        .y = 103,
        .width = 32,
        .height = 48,
        .id = "BATHROOM_TOWEL"
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_bathroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_background_idx);
    img_carpet_moved = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_carpet_moved_idx);
    img_doors_opened = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_doors_opened_idx);
    img_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_drawer_opened_idx);
    img_perfume_taken = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_perfume_taken_idx);
    img_shower_opened = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_shower_opened_idx);
    img_soap_taken = C2D_SpriteSheetGetImage(room_scene, gfx_bathroom_soap_taken_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_bathroom_shower_curtain_opened()) {
        C2D_DrawImageAt(img_shower_opened, 5.0f, 30.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_bathroom_carpet_moved()) {
        C2D_DrawImageAt(img_carpet_moved, 0.0f, 186.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_are_bathroom_closet_doors_opened()) {
        C2D_DrawImageAt(img_doors_opened, 164.0f, 133.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_bathroom_closet_drawer_opened()) {
        C2D_DrawImageAt(img_drawer_opened, 172.0f, 120.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_PERFUME)) {
        C2D_DrawImageAt(img_perfume_taken, 217.0f, 103.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_SOAP)) {
        C2D_DrawImageAt(img_soap_taken, 129.0f, 94.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_west(void) {
    game_set_room(&bathroom);
}

static void west_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_west);
}


Room bathroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .west = {.action = west_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
