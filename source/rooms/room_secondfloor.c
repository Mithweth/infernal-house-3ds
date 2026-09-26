// room_secondfloor.c

#include <citro2d.h>
#include "game.h"
#include "room_secondfloor.h"
#include "room_firstfloor.h"
#include "gfx_secondfloor.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;


static bool door_use_item(ItemId item) {
    if (item == ITEM_KEY_ONE) {
        gamestate_unlock_secondfloor_door();
        inventory_remove(ITEM_KEY_ONE);
        game_show_message("SECONDFLOOR_DOOR_UNLOCK");
        return true;
    }
    return false;
}

static Hotspot hotspots[] = {
    {
        .x = 12,
        .y = 38,
        .width = 52,
        .height = 170,
        .message_id = "SECONDFLOOR_DOOR_UNLOCKED",
        .id = "SECONDFLOOR_DOOR",
        .is_active = gamestate_is_secondfloor_door_unlocked
    },
    {
        .x = 12,
        .y = 38,
        .width = 52,
        .height = 170,
        .message_id = "SECONDFLOOR_DOOR_LOCKED",
        .id = "SECONDFLOOR_DOOR",
        .use_item = door_use_item
    }
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_secondfloor.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_secondfloor_background_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_west(void) {
    //game_set_room(&firstbedroom);
}

static void west_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_west);
}

static void move_south(void) {
    game_set_room(&firstfloor);
}

static void south_action(void) {
    game_wait_for_sfx("romfs:/audio/wooden_stairs.raw", move_south);
}

Room secondfloor = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = south_action},
    .west = {.action = west_action, .condition = gamestate_is_secondfloor_door_unlocked},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
