// room_firstbedroom.c
// <room_name.c

#include <citro2d.h>
#include "game.h"
#include "room_firstbedroom.h"
#include "room_firstfloor.h"
#include "gfx_firstbedroom.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_dark_background;
static C2D_Image img_dark_closet_opened;
static C2D_Image img_dark_invoice_taken;
static C2D_Image img_dark_left_nightstand_empty;
static C2D_Image img_dark_left_nightstand_opened;
static C2D_Image img_dark_right_nightstand_opened;
static C2D_Image img_dark_stain_remover_taken;
static C2D_Image img_light_background;
static C2D_Image img_light_closet_opened;
static C2D_Image img_light_invoice_taken;
static C2D_Image img_light_left_nightstand_empty;
static C2D_Image img_light_left_nightstand_opened;
static C2D_Image img_light_right_nightstand_opened;
static C2D_Image img_light_stain_remover_taken;


static void right_nightstand_action(void) {
    gamestate_open_firstbedroom_right_nightstand(!gamestate_is_firstbedroom_right_nightstand_opened());
}

static bool right_nightstand_is_active(void) {
    return gamestate_is_firstbedroom_right_nightstand_opened();
}

static void left_nightstand_action(void) {
    gamestate_open_firstbedroom_left_nightstand(!gamestate_is_firstbedroom_left_nightstand_opened());
}

static bool left_nightstand_is_active(void) {
    return gamestate_is_firstbedroom_left_nightstand_opened() && !inventory_has(ITEM_REVOLVER);
}

static void revolver_action(void) {
    inventory_add(ITEM_REVOLVER);
    inventory_add(ITEM_GLASSES);
}

static void closet_action(void) {
    gamestate_open_firstbedroom_closet(!gamestate_is_firstbedroom_closet_opened());
}

static void switch_action(void) {
    gamestate_turn_firstbedroom_lights_on(!gamestate_is_firstbedroom_lights_on());
}

static void invoice_action(void) {
    inventory_add(ITEM_INVOICE);
}

static bool invoice_is_active(void) {
    return gamestate_is_firstbedroom_closet_opened() && !inventory_has(ITEM_INVOICE);
}

static void stain_remover_action(void) {
    inventory_add(ITEM_STAIN_REMOVER);
}

static bool stain_remover_is_active(void) {
    return !inventory_has(ITEM_STAIN_REMOVER);
}

static void pillow_action(void) {
    game_show_message("FIRSTBEDROOM_PILLOW_MOVED");
}

static Hotspot hotspots[] = {
    {
        .x = 159,
        .y = 58,
        .width = 24,
        .height = 24,
        .id = "FIRSTBEDROOM_LAMP"
    },
    {
        .x = 217,
        .y = 88,
        .width = 16,
        .height = 16,
        .id = "FIRSTBEDROOM_LAMP_SWITCH",
        .action = switch_action
    },
    {
        .x = 285,
        .y = 43,
        .width = 20,
        .height = 26,
        .id = "FIRSTBEDROOM_STAIN_REMOVER",
        .is_active = stain_remover_is_active,
        .action = stain_remover_action
    },
    {
        .x = 249,
        .y = 45,
        .width = 28,
        .height = 23,
        .id = "FIRSTBEDROOM_BOOKS",
    },
    {
        .x = 125,
        .y = 112,
        .width = 81,
        .height = 14,
        .id = "FIRSTBEDROOM_PILLOW",
        .action = pillow_action
    },
    {
        .x = 119,
        .y = 125,
        .width = 111,
        .height = 60,
        .id = "FIRSTBEDROOM_BED",
    },
    {
        .x = 235,
        .y = 127,
        .width = 24,
        .height = 27,
        .id = "FIRSTBEDROOM_EMPTY_NIGHTSTAND_OPENED",
        .is_active = right_nightstand_is_active,
        .action = right_nightstand_action
    },
    {
        .x = 235,
        .y = 127,
        .width = 24,
        .height = 27,
        .id = "FIRSTBEDROOM_EMPTY_NIGHTSTAND_OPENED",
        .is_active = right_nightstand_is_active,
        .action = right_nightstand_action
    },
    {
        .x = 235,
        .y = 127,
        .width = 24,
        .height = 27,
        .id = "FIRSTBEDROOM_NIGHTSTAND",
        .action = right_nightstand_action
    },
    {
        .x = 78,
        .y = 127,
        .width = 24,
        .height = 27,
        .id = "FIRSTBEDROOM_LEFT_NIGHTSTAND_OPENED",
        .is_active = left_nightstand_is_active,
        .action = revolver_action
    },
    {
        .x = 78,
        .y = 127,
        .width = 24,
        .height = 27,
        .id = "FIRSTBEDROOM_NIGHTSTAND",
        .action = left_nightstand_action
    },
    {
        .x = 23,
        .y = 113,
        .width = 10,
        .height = 12,
        .id = "FIRSTBEDROOM_CLOSET_INVOICE",
        .is_active = invoice_is_active,
        .action = invoice_action
    },
    {
        .x = 20,
        .y = 74,
        .width = 41,
        .height = 72,
        .id = "FIRSTBEDROOM_CLOSET_OPENED",
        .is_active = gamestate_is_firstbedroom_closet_opened,
        .action = closet_action
    },
    {
        .x = 20,
        .y = 41,
        .width = 38,
        .height = 20,
        .id = "FIRSTBEDROOM_CLOSET_TISSUE",
        .is_active = gamestate_is_firstbedroom_closet_opened
    }, 
    {
        .x = 2,
        .y = 21,
        .width = 64,
        .height = 143,
        .id = "FIRSTBEDROOM_CLOSET",
        .action = closet_action
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_firstbedroom.t3x");
    img_dark_background = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_bg_idx);
    img_dark_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_closet_opened_idx);
    img_dark_invoice_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_invoice_taken_idx);
    img_dark_left_nightstand_empty = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_left_nightstand_empty_idx);
    img_dark_left_nightstand_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_left_nightstand_opened_idx);
    img_dark_right_nightstand_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_right_nightstand_opened_idx);
    img_dark_stain_remover_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_dark_stain_remover_taken_idx);
    img_light_background = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_bg_idx);
    img_light_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_closet_opened_idx);
    img_light_invoice_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_invoice_taken_idx);
    img_light_left_nightstand_empty = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_left_nightstand_empty_idx);
    img_light_left_nightstand_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_left_nightstand_opened_idx);
    img_light_right_nightstand_opened = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_right_nightstand_opened_idx);
    img_light_stain_remover_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstbedroom_light_stain_remover_taken_idx);
}

static void room_draw(void) {
    if (!gamestate_is_firstbedroom_lights_on()) {
        C2D_DrawImageAt(img_dark_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        if (gamestate_is_firstbedroom_closet_opened()) {
            C2D_DrawImageAt(img_dark_closet_opened, 0.0f, 11.0f, 0.4f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_INVOICE)) {
                C2D_DrawImageAt(img_dark_invoice_taken, 23.0f, 112.0f, 0.5f, NULL, 1.0f, 1.0f);
            }
        }
        if (gamestate_is_firstbedroom_left_nightstand_opened()) {
            C2D_DrawImageAt(img_dark_left_nightstand_opened, 68.0f, 125.0f, 0.2f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_REVOLVER)) {
                C2D_DrawImageAt(img_dark_left_nightstand_empty, 80.0f, 129.0f, 0.3f, NULL, 1.0f, 1.0f);
            }
        }
        if (gamestate_is_firstbedroom_right_nightstand_opened()) {
            C2D_DrawImageAt(img_dark_right_nightstand_opened, 233.0f, 125.0f, 0.2f, NULL, 1.0f, 1.0f);
        }
        if (inventory_has(ITEM_STAIN_REMOVER)) {
            C2D_DrawImageAt(img_dark_stain_remover_taken, 286.0f, 41.0f, 0.2f, NULL, 1.0f, 1.0f);
        }
    } else {
        C2D_DrawImageAt(img_light_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        if (gamestate_is_firstbedroom_closet_opened()) {
            C2D_DrawImageAt(img_light_closet_opened, 0.0f, 11.0f, 0.4f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_INVOICE)) {
                C2D_DrawImageAt(img_light_invoice_taken, 23.0f, 112.0f, 0.5f, NULL, 1.0f, 1.0f);
            }
        }
        if (gamestate_is_firstbedroom_left_nightstand_opened()) {
            C2D_DrawImageAt(img_light_left_nightstand_opened, 68.0f, 124.0f, 0.2f, NULL, 1.0f, 1.0f);
            if (inventory_has(ITEM_REVOLVER)) {
                C2D_DrawImageAt(img_light_left_nightstand_empty, 80.0f, 127.0f, 0.3f, NULL, 1.0f, 1.0f);
            }
        }
        if (gamestate_is_firstbedroom_right_nightstand_opened()) {
            C2D_DrawImageAt(img_light_right_nightstand_opened, 237.0f, 127.0f, 0.2f, NULL, 1.0f, 1.0f);
        }
        if (inventory_has(ITEM_STAIN_REMOVER)) {
            C2D_DrawImageAt(img_light_stain_remover_taken, 287.0f, 44.0f, 0.2f, NULL, 1.0f, 1.0f);
        }      
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_south(void) {
    game_set_room(&firstfloor);
}

static void south_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_south);
}

Room firstbedroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = south_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};