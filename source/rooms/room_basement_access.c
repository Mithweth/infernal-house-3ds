#include <citro2d.h>

// Replace basement_access

#include "game.h"
#include "room_basement_access.h"
#include "room_corridor.h"
#include "gfx_basement_access.h"
#include "inventory.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;

static Hotspot hotspots[] = {
    {
        .x = 129,
        .y = 24,
        .width = 40,
        .height = 45,
        .text_id = "BASEMENT_ACCESS_BOX",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 211,
        .y = 39,
        .width = 74,
        .height = 29,
        .text_id = "BASEMENT_ACCESS_JAM",
    },
    {
        .x = 212,
        .y = 70,
        .width = 73,
        .height = 40,
        .text_id = "BASEMENT_ACCESS_BOTTLES",
    },
    {
        .x = 216,
        .y = 111,
        .width = 38,
        .height = 36,
        .text_id = "BASEMENT_ACCESS_TOOLS",
    },
    {
        .x = 217,
        .y = 199,
        .width = 103,
        .height = 41,
        .text_id = "BASEMENT_ACCESS_CARPET",
    },
    {
        .x = 255,
        .y = 115,
        .width = 31,
        .height = 36,
        .text_id = "BASEMENT_ACCESS_GAME",
        .is_active = NULL,
        .action = NULL
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_basement_access.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_basement_access_bg_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_east(void) {
    game_set_room(&corridor);
}

static void east_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_east);
}

Room basement_access = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .east = east_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};