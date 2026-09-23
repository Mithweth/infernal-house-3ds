// room_secondunderground.c

#include <citro2d.h>
#include "game.h"
#include "room_secondunderground.h"
#include "room_firstunderground.h"
#include "room_deadroom.h"
#include "gfx_secondunderground.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_wall_open;

static bool broken_wall_is_active(void) {
    return gamestate_is_underground_wall_broken();
}

static bool wall_is_active(void) {
    return !gamestate_is_underground_wall_broken();
}

static void wall_use_item(ItemId item) {
    if ((item == ITEM_SLEDGEHAMMER) && (!gamestate_is_underground_wall_broken())) {
        gamestate_break_underground_wall();
        game_show_message("UNDERGROUND_BREAK_WALL");
    }
}

static Hotspot hotspots[] = {
    {
        .x = 13,
        .y = 46,
        .width = 78,
        .height = 27,
        .text_id = "UNDERGROUND_SHELF_1",
    },
    {
        .x = 13,
        .y = 83,
        .width = 78,
        .height = 30,
        .text_id = "UNDERGROUND_SHELF_2",
    },
    {
        .x = 13,
        .y = 121,
        .width = 78,
        .height = 26,
        .text_id = "UNDERGROUND_SHELF_3",
    },
    {
        .x = 267,
        .y = 38,
        .width = 54,
        .height = 135,
        .text_id = "UNDERGROUND_WATER_HEATER",
    },
    {
        .x = 124,
        .y = 39,
        .width = 96,
        .height = 139,
        .text_id = "UNDERGROUND_WALL",
        .is_active = wall_is_active,
        .use_item = wall_use_item
    },
    {
        .x = 124,
        .y = 39,
        .width = 96,
        .height = 139,
        .text_id = "UNDERGROUND_WALL_BROKEN",
        .is_active = broken_wall_is_active
    },
};


static void north_action(void) {
    game_set_room(&deadroom);
}

static void create_new_routes(void) {
    if (gamestate_is_underground_wall_broken()) {
        secondunderground.north = north_action;
    } else {
        secondunderground.north = NULL;
    }
}

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_secondunderground.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_secondunderground_background_idx);
    img_wall_open = C2D_SpriteSheetGetImage(room_scene, gfx_secondunderground_wall_open_idx);
    create_new_routes();
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_underground_wall_broken()) {
        C2D_DrawImageAt(img_wall_open, 103.0f, 37.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
    create_new_routes();
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void west_action(void) {
    game_set_room(&firstunderground);
}

Room secondunderground = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .west = west_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
