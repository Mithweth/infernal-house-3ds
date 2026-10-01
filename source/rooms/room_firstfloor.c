// room_firstfloor.c
// <room_name.c

#include <citro2d.h>
#include "gfxmap.h"
#include "game.h"
#include "room_firstfloor.h"
#include "room_diningroom.h"
#include "room_firstbedroom.h"
#include "room_secondfloor.h"
#include "room_secondbedroom.h"
#include "room_bathroom.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet assets;
static C2D_Image img_background;
static C2D_Image img_carpet_moved;

static void carpet_action(void) {
    gamestate_set("firstfloor_carpet_moved");
}

static Hotspot hotspots[] = {
    {
        .x = 46,
        .y = 34,
        .width = 49,
        .height = 51,
        .id = "FIRSTFLOOR_LARGE_PAINTING",
    },
    {
        .x = 68,
        .y = 160,
        .width = 25,
        .height = 29,
        .id = "FIRSTFLOOR_SMALL_PAINTING",
    },
    {
        .x = 229,
        .y = 165,
        .width = 82,
        .height = 46,
        .id = "FIRSTFLOOR_CARPET",
        .action = carpet_action
    },
};

static void room_init(void) {
    if (!gfxmap_load_assets("romfs:/gfx/firstfloor", &assets)) {
        return;
    }
    img_background = gfxmap_get_image(assets, "gfx_bg_idx");
    img_carpet_moved = gfxmap_get_image(assets, "gfx_carpet_moved_idx");
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_get("firstfloor_carpet_moved")) {
        C2D_DrawImageAt(img_carpet_moved, 181.0f, 151.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(assets);
}

static void move_east(void) {
    game_set_room(&bathroom);
}

static void east_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_east);
}

static void move_southeast(void) {
    game_set_room(&secondbedroom);
}

static void southeast_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_southeast);
}

static void move_southwest(void) {
    game_set_room(&firstbedroom);
}

static void southwest_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_southwest);
}

static void move_north(void) {
    game_set_room(&secondfloor);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/wooden_stairs.raw", move_north);
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
    .north = {.action = north_action},
    .east = {.action = east_action},
    .southeast = {.action = southeast_action},
    .south = {.action = south_action},
    .southwest = {.action = southwest_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};