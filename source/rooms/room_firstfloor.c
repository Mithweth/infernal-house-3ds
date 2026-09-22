// <room_name.c

#include <citro2d.h>
#include "game.h"
#include "room_firstfloor.h"
#include "room_diningroom.h"
#include "room_firstbedroom.h"
#include "gfx_firstfloor.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_carpet_moved;

static void carpet_action(void) {
    gamestate_move_firstfloor_carpet(!gamestate_is_firstfloor_carpet_moved());
}

static bool carpet_is_active(void) {
    return gamestate_is_firstfloor_carpet_moved();
}

static Hotspot hotspots[] = {
    {
        .x = 46,
        .y = 34,
        .width = 49,
        .height = 51,
        .text_id = "FIRSTFLOOR_LARGE_PAINTING",
    },
    {
        .x = 68,
        .y = 160,
        .width = 25,
        .height = 29,
        .text_id = "FIRSTFLOOR_SMALL_PAINTING",
    },
    {
        .x = 191,
        .y = 158,
        .width = 71,
        .height = 44,
        .text_id = "FIRSTFLOOR_CARPET_MOVED",
        .is_active = carpet_is_active,
        .action = carpet_action
    },
    {
        .x = 229,
        .y = 165,
        .width = 82,
        .height = 46,
        .text_id = "FIRSTFLOOR_CARPET",
        .action = carpet_action
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_firstfloor.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_firstfloor_bg_idx);
    img_carpet_moved = C2D_SpriteSheetGetImage(room_scene, gfx_firstfloor_carpet_moved_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_firstfloor_carpet_moved()) {
        C2D_DrawImageAt(img_carpet_moved, 181.0f, 151.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void east_action(void) {

}

static void southeast_action(void) {

}

static void move_southwest(void) {
    game_set_room(&firstbedroom);
}

static void southwest_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_southwest);
}

static void north_action(void) {

}

static void move_south(void) {
    game_set_room(&dining_room);
}

static void south_action(void) {
    game_wait_for_sfx("romfs:/audio/wooden_stairs.raw", move_south);
}

Room firstfloor = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = north_action,
    .east = east_action,
    .southeast = southeast_action,
    .south = south_action,
    .southwest = southwest_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};