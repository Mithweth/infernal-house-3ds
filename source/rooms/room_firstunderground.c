// room_firstunderground.c

#include <citro2d.h>
#include "game.h"
#include "room_firstunderground.h"
#include "room_secondunderground.h"
#include "room_cellar.h"
#include "gfx_firstunderground.h"
#include "inventory.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_bg;
static C2D_Image img_shovel_taken;
static C2D_Image img_sledgehammer_taken;

static void shovel_action(void) {
    inventory_add(ITEM_SHOVEL);
}

static bool shovel_is_active(void) {
    return !inventory_has(ITEM_SHOVEL);
}

static void sledgehammer_action(void) {
    inventory_add(ITEM_SLEDGEHAMMER);
}

static bool sledgehammer_is_active(void) {
    return !inventory_has(ITEM_SLEDGEHAMMER);
}

static Hotspot hotspots[] = {
    {
        .x = 132,
        .y = 32,
        .width = 104,
        .height = 176,
        .text_id = "UNDERGROUND_LADDER",
    },
    {
        .x = 18,
        .y = 89,
        .width = 37,
        .height = 100,
        .text_id = "UNDERGROUND_SHOVEL",
        .is_active = shovel_is_active,
        .action = shovel_action
    },
    {
        .x = 53,
        .y = 84,
        .width = 35,
        .height = 102,
        .text_id = "UNDERGROUND_SLEDGEHAMMER",
        .is_active = sledgehammer_is_active,
        .action = sledgehammer_action
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_firstunderground.t3x");
    img_bg = C2D_SpriteSheetGetImage(room_scene, gfx_firstunderground_bg_idx);
    img_shovel_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstunderground_shovel_taken_idx);
    img_sledgehammer_taken = C2D_SpriteSheetGetImage(room_scene, gfx_firstunderground_sledgehammer_taken_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_bg, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (inventory_has(ITEM_SHOVEL)) {
        C2D_DrawImageAt(img_shovel_taken, 15.0f, 85.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_SLEDGEHAMMER)) {
        C2D_DrawImageAt(img_sledgehammer_taken, 46.0f, 83.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_north(void) {
    game_set_room(&cellar);
}

static void east_action(void) {
    game_set_room(&secondunderground);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/metal_ladder.raw", move_north);
}

Room firstunderground = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = north_action,
    .east = east_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
