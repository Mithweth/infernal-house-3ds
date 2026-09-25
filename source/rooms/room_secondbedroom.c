// room_secondbedroom.c

#include <citro2d.h>
#include "game.h"
#include "room_secondbedroom.h"
#include "room_firstfloor.h"
#include "gfx_secondbedroom.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_dark_background;
static C2D_Image img_dark_closet_drawer;
static C2D_Image img_dark_closet_left_door;
static C2D_Image img_dark_closet_right_door;
static C2D_Image img_dark_nightstand_drawer;
static C2D_Image img_dark_bedpost;
static C2D_Image img_dark_paint_empty;
static C2D_Image img_dark_paint_opened;
static C2D_Image img_light_background;
static C2D_Image img_light_closet_drawer;
static C2D_Image img_light_closet_left_door;
static C2D_Image img_light_closet_right_door;
static C2D_Image img_light_nightstand_drawer;
static C2D_Image img_light_bedpost;
static C2D_Image img_light_paint_empty;
static C2D_Image img_light_paint_opened;


static void lamp_action(void) {
    gamestate_turn_secondbedroom_lights_on(!gamestate_is_secondbedroom_lights_on());
    if (gamestate_is_secondbedroom_lights_on()) {
        game_show_message("SECONDBEDROOM_LAMP_TURNEDON");
    }
}

static void closet_drawer_action(void) {
    gamestate_open_secondbedroom_closet_drawer(!gamestate_is_secondbedroom_closet_drawer_opened());
}

static void right_closet_action(void) {
    gamestate_open_secondbedroom_right_closet_door(!gamestate_is_secondbedroom_right_closet_door_opened());
}

static void left_closet_action(void) {
    gamestate_open_secondbedroom_left_closet_door(!gamestate_is_secondbedroom_left_closet_door_opened());
}

static void pocket_action() {
    inventory_add(ITEM_BRACELET);
    inventory_add(ITEM_CIGARETTES);
}

static bool pocket_is_active(void) {
    return gamestate_is_secondbedroom_right_closet_door_opened() && !inventory_has(ITEM_CIGARETTES);
}

static void nightstand_drawer_action(void) {
    gamestate_open_secondbedroom_left_nightstand(!gamestate_is_secondbedroom_left_nightstand_opened());
}

static bool nightstand_drawer_is_active(void) {
    return gamestate_is_secondbedroom_left_nightstand_opened() && !inventory_has(ITEM_RUBBER);    
}

static void nightstand_objects_action(void) {
    inventory_add(ITEM_RUBBER);
    inventory_add(ITEM_LIGHTBULB);
    inventory_add(ITEM_MEASURING_TAPE);
}

static void move_left_painting(void) {
    game_show_message("SECONDBEDROOM_PAINTING_1_MOVE");
}

static void move_right_painting(void) {
    game_show_message("SECONDBEDROOM_PAINTING_2_MOVE");
}

static void pull_bedpost_action(void) {
    gamestate_pull_secondbedroom_bedpost(!gamestate_is_secondbedroom_bedpost_pulled());
    if (gamestate_is_secondbedroom_bedpost_pulled()) {
        game_show_message("SECONDBEDROOM_BED_FOOT_PULLED");
    }
}

static bool opened_painting_and_key_not_taken(void) {
    return gamestate_is_secondbedroom_bedpost_pulled() && !inventory_has(ITEM_KEY_ONE);
}

static void take_key_one_action(void) {
    inventory_add(ITEM_KEY_ONE);
}

static Hotspot hotspots[] = {
    {
        .x = 12,
        .y = 97,
        .width = 27,
        .height = 37,
        .id = "SECONDBEDROOM_BEDSIDE_LAMP",
        .action = lamp_action
    },
    {
        .x = 7,
        .y = 136,
        .width = 22,
        .height = 29,
        .id = "SECONDBEDROOM_NIGHTSTAND_DRAWER_OPENED",
        .is_active = nightstand_drawer_is_active,
        .action = nightstand_objects_action
    },
    {
        .x = 10,
        .y = 136,
        .width = 27,
        .height = 15,
        .id = "SECONDBEDROOM_NIGHTSTAND_DRAWER",
        .action = nightstand_drawer_action
    },
    {
        .x = 59,
        .y = 51,
        .width = 53,
        .height = 47,
        .id = "SECONDBEDROOM_PAINTING_1",
        .action = move_left_painting
    },
    {
        .x = 230,
        .y = 60,
        .width = 28,
        .height = 53,
        .id = "SECONDBEDROOM_PAINTING_2",
        .message_id = "SECONDBEDROOM_PAINTING_2_MOVED",
        .is_active = opened_painting_and_key_not_taken,
        .action = take_key_one_action
    },
    {
        .x = 230,
        .y = 60,
        .width = 28,
        .height = 53,
        .id = "SECONDBEDROOM_PAINTING_2",
        .message_id = "SECONDBEDROOM_PAINTING_2_EMPTY",
        .is_active = gamestate_is_secondbedroom_bedpost_pulled
    },
    {
        .x = 230,
        .y = 60,
        .width = 28,
        .height = 53,
        .id = "SECONDBEDROOM_PAINTING_2",
        .is_active = gamestate_is_secondbedroom_right_closet_door_opened,
        .action = move_right_painting
    },
    {
        .x = 222,
        .y = 60,
        .width = 39,
        .height = 53,
        .id = "SECONDBEDROOM_PAINTING_2",
        .action = move_right_painting
    },
    {
        .x = 143,
        .y = 51,
        .width = 35,
        .height = 26,
        .id = "SECONDBEDROOM_CLOSET",
        .message_id = "SECONDBEDROOM_LEFT_CLOSET_TOP",
        .is_active = gamestate_is_secondbedroom_left_closet_door_opened,
        .action = left_closet_action
    },
    {
        .x = 143,
        .y = 79,
        .width = 35,
        .height = 26,
        .id = "SECONDBEDROOM_CLOSET",
        .message_id = "SECONDBEDROOM_LEFT_CLOSET_MIDDLE",
        .is_active = gamestate_is_secondbedroom_left_closet_door_opened,
        .action = left_closet_action
    },
    {
        .x = 143,
        .y = 106,
        .width = 35,
        .height = 37,
        .id = "SECONDBEDROOM_CLOSET",
        .message_id = "SECONDBEDROOM_LEFT_CLOSET_BOTTOM",
        .is_active = gamestate_is_secondbedroom_left_closet_door_opened,
        .action = left_closet_action
    },
    {
        .x = 138,
        .y = 40,
        .width = 40,
        .height = 109,
        .id = "SECONDBEDROOM_CLOSET",
        .action = left_closet_action
    },
    {
        .x = 180,
        .y = 61,
        .width = 34,
        .height = 76,
        .id = "SECONDBEDROOM_RIGHT_CLOSET",
        .message_id = "SECONDBEDROOM_RIGHT_CLOSET_POCKET",
        .is_active = pocket_is_active,
        .action = pocket_action
    },
    {
        .x = 180,
        .y = 61,
        .width = 34,
        .height = 76,
        .id = "SECONDBEDROOM_RIGHT_CLOSET",
        .is_active = gamestate_is_secondbedroom_right_closet_door_opened
    },
    {
        .x = 178,
        .y = 40,
        .width = 40,
        .height = 109,
        .id = "SECONDBEDROOM_CLOSET",
        .action = right_closet_action
    },
    {
        .x = 142,
        .y = 150,
        .width = 79,
        .height = 26,
        .id = "SECONDBEDROOM_CLOSET_DRAWER_OPENED",
        .is_active = gamestate_is_secondbedroom_closet_drawer_opened,
        .action = closet_drawer_action
    },
    {
        .x = 142,
        .y = 150,
        .width = 72,
        .height = 20,
        .id = "SECONDBEDROOM_CLOSET_DRAWER",
        .action = closet_drawer_action
    },
    {
        .x = 3,
        .y = 177,
        .width = 52,
        .height = 48,
        .id = "SECONDBEDROOM_CARPET"
    },
    {
        .x = 8,
        .y = 132,
        .width = 34,
        .height = 44,
        .id = "SECONDBEDROOM_NIGHTSTAND"
    },
    {
        .x = 110,
        .y = 100,
        .width = 16,
        .height = 16,
        .id = "SECONDBEDROOM_BED",
        .message_id = "SECONDBEDROOM_BED_FOOT",
        .action = pull_bedpost_action
    },
    {
        .x = 49,
        .y = 113,
        .width = 92,
        .height = 91,
        .id = "SECONDBEDROOM_BED"
    }
};


static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_secondbedroom.t3x");
    img_dark_background = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_background_idx);
    img_dark_closet_drawer = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_closet_drawer_idx);
    img_dark_closet_left_door = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_closet_left_door_idx);
    img_dark_closet_right_door = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_closet_right_door_idx);
    img_dark_nightstand_drawer = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_nightstand_drawer_idx);
    img_dark_bedpost = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_bedpost_idx);
    img_dark_paint_empty = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_paint_empty_idx);
    img_dark_paint_opened = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_dark_paint_opened_idx);
    img_light_background = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_background_idx);
    img_light_closet_drawer = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_closet_drawer_idx);
    img_light_closet_left_door = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_closet_left_door_idx);
    img_light_closet_right_door = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_closet_right_door_idx);
    img_light_nightstand_drawer = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_nightstand_drawer_idx);
    img_light_bedpost = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_bedpost_idx);
    img_light_paint_empty = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_paint_empty_idx);
    img_light_paint_opened = C2D_SpriteSheetGetImage(room_scene, gfx_secondbedroom_light_paint_opened_idx);
}

static void room_draw(void) {
    if (gamestate_is_secondbedroom_lights_on()) {
        C2D_DrawImageAt(img_light_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        if (gamestate_is_secondbedroom_left_nightstand_opened()) {
            C2D_DrawImageAt(img_light_nightstand_drawer, 6.0f, 138.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_left_closet_door_opened()) {
            C2D_DrawImageAt(img_light_closet_left_door, 127.0f, 40.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_right_closet_door_opened()) {
            C2D_DrawImageAt(img_light_closet_right_door, 177.0f, 40.0f, 0.6f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_closet_drawer_opened()) {
            C2D_DrawImageAt(img_light_closet_drawer, 144.0f, 151.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_bedpost_pulled()) {
            C2D_DrawImageAt(img_light_bedpost, 113.0f, 95.0f, 0.3f, NULL, 1.0f, 1.0f);
            C2D_DrawImageAt(img_light_paint_opened, 218.0f, 54.0f, 0.4f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_KEY_ONE)) {
                C2D_DrawImageAt(img_light_paint_empty, 231.0f, 78.0f, 0.5f, NULL, 1.0f, 1.0f);
            }
        }
    } else {
        C2D_DrawImageAt(img_dark_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        if (gamestate_is_secondbedroom_left_nightstand_opened()) {
            C2D_DrawImageAt(img_dark_nightstand_drawer, 6.0f, 139.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_left_closet_door_opened()) {
            C2D_DrawImageAt(img_dark_closet_left_door, 126.0f, 40.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_right_closet_door_opened()) {
            C2D_DrawImageAt(img_dark_closet_right_door, 179.0f, 40.0f, 0.6f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_closet_drawer_opened()) {
            C2D_DrawImageAt(img_dark_closet_drawer, 144.0f, 150.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
        if (gamestate_is_secondbedroom_bedpost_pulled()) {
            C2D_DrawImageAt(img_dark_bedpost, 113.0f, 94.0f, 0.3f, NULL, 1.0f, 1.0f);
            C2D_DrawImageAt(img_dark_paint_opened, 218.0f, 54.0f, 0.4f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_KEY_ONE)) {
                C2D_DrawImageAt(img_dark_paint_empty, 231.0f, 75.0f, 0.5f, NULL, 1.0f, 1.0f);
            }
        }
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_east(void) {
    game_set_room(&firstfloor);
}

static void east_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_east);
}

Room secondbedroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .east = {.action = east_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
