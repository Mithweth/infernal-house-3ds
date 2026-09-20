#include <citro2d.h>
#include "game.h"
#include "room_corridor.h"
#include "gfx_corridor.h"
#include "inventory.h"
#include "room_diningroom.h"
#include "room_livingroom.h"
#include "room_cellar.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;

static Hotspot hotspots[] = {
    {
        .x = 0,
        .y = 15,
        .width = 15,
        .height = 125,
        .text_id = "CORRIDOR_WINDOW",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 27,
        .y = 47,
        .width = 32,
        .height = 42,
        .text_id = "CORRIDOR_LAMP",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 168,
        .y = 68,
        .width = 50,
        .height = 45,
        .text_id = "CORRIDOR_SWORD",
        .is_active = NULL,
        .action = NULL
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_corridor.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_corridor_bg_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_north(void) {
    game_set_room(&cellar);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_north);
}

static void move_northeast(void) {
    game_set_room(&livingroom);
}

static void northeast_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_northeast);
}

static void east_action(void) {

}

static void south_action(void) {
    game_set_room(&dining_room);
}

static void west_action(void) {

}

Room corridor = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = north_action,
    .northeast = northeast_action,
    .east = east_action,
    .south = south_action,
    .west = west_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};