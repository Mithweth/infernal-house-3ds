// room_livingroom.c
#include <citro2d.h>
#include "game.h"
#include "room_livingroom.h"
#include "room_corridor.h"
#include "gfx_livingroom.h"
#include "inventory.h"
#include "gamestate.h"
#include "audio.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_lighter;
static C2D_Image img_hearth_opened;
static C2D_Image img_piano_opened;
static C2D_Image img_hearth_prepared;
static C2D_Image img_statue;

static void play_piano_action(void) {
    game_play_piano();
}

static void piano_action(void) {
    if (gamestate_is_livingroom_piano_opened()) {
        sfx_play("romfs:/audio/closet_close.raw");
    } else {
        sfx_play("romfs:/audio/closet_open.raw");
    }
    gamestate_open_livingroom_piano(!gamestate_is_livingroom_piano_opened());
}

static void lighter_action(void) {
    inventory_add(ITEM_LIGHTER);
}

static bool lighter_is_active(void) {
    return !inventory_has(ITEM_LIGHTER);
}

static void fall_gameover(void) {
    game_over(GAMEOVER_FALL);
}

static void north_action(void) {
    if (!gamestate_is_rope_used_in_livingroom_hearth()) {
        game_wait_for_sfx("romfs:/audio/falling_down.raw", fall_gameover);
    } else {
        //game_set_room(&laboratory);
    }
}

static bool fireplace_use_item(ItemId item) {
    if (item == ITEM_STATUE) {
        gamestate_set_livingroom_golden_statue_placed();
        inventory_remove(ITEM_STATUE);
        game_show_message("LIVINGROOM_PLACE_STATUE");
        return true;
    }
    return false;
}

static bool hearth_opened_is_active(void) {
    return gamestate_is_livingroom_secret_passage_opened() && !gamestate_is_rope_used_in_livingroom_hearth();
}

static bool hearth_use_item(ItemId item) {
    if (item == ITEM_ROPE) {
        gamestate_bind_rope_used_in_livingroom_hearth();
        inventory_remove(ITEM_ROPE);
        game_show_message("LIVINGROOM_USE_ROPE");
        return true;
    }
    return false;
}

static bool wood_use_item(ItemId item) {
    if (item == ITEM_LIGHTER) {
        game_show_message("LIVINGROOM_USE_LIGHTER");
        return true;
    }
    return false;
}

static bool lighter_use_item(ItemId item) {
    if (item == ITEM_MAGNIFYING_GLASS) {
        game_show_message("LIVINGROOM_USE_MAGNIFYING_GLASS");
        return true;
    }
    return false;
}

static Hotspot hotspots[] = {
    {
        .x = 146,
        .y = 107,
        .width = 68,
        .height = 47,
        .id = "LIVINGROOM_FIREPLACE_HEARTH_OPENED",
        .is_active = hearth_opened_is_active,
        .use_item = hearth_use_item
    },
    {
        .x = 2,
        .y = 111,
        .width = 91,
        .height = 30,
        .id = "LIVINGROOM_PIANO",
        .is_active = gamestate_is_livingroom_piano_opened,
        .action = play_piano_action
    },
    {
        .x = 2,
        .y = 111,
        .width = 91,
        .height = 30,
        .id = "LIVINGROOM_PIANO",
        .action = piano_action
    },
    {
        .x = 140,
        .y = 178,
        .width = 32,
        .height = 20,
        .id = "LIVINGROOM_LIGHTER",
        .is_active = lighter_is_active,
        .action = lighter_action,
        .use_item = lighter_use_item
    },
    {
        .x = 104,
        .y = 114,
        .width = 31,
        .height = 37,
        .id = "LIVINGROOM_WOOD",
        .use_item = wood_use_item
    },
    {
        .x = 9,
        .y = 29,
        .width = 52,
        .height = 54,
        .id = "LIVINGROOM_PAINTING"
    },
    {
        .x = 163,
        .y = 72,
        .width = 26,
        .height = 17,
        .id = "LIVINGROOM_FIREPLACE",
        .message_id = "LIVINGROOM_FIREPLACE_EXAMINE",
        .use_item = fireplace_use_item
    },
    {
        .x = 146,
        .y = 107,
        .width = 68,
        .height = 47,
        .id = "LIVINGROOM_FIREPLACE",
        .message_id = "LIVINGROOM_FIREPLACE_HEARTH"
    },
    {
        .x = 138,
        .y = 50,
        .width = 92,
        .height = 104,
        .id = "LIVINGROOM_FIREPLACE"
    }
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_livingroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_bg_idx);
    img_lighter = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_lighter_idx);
    img_hearth_opened = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_hearth_opened_idx);
    img_hearth_prepared = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_hearth_prepared_idx);
    img_piano_opened = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_piano_opened_idx);
    img_statue = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_statue_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (!inventory_has(ITEM_LIGHTER)) {
        C2D_DrawImageAt(img_lighter, 146.0f, 181.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_livingroom_piano_opened()) {
        C2D_DrawImageAt(img_piano_opened, 2.0f, 102.0f, 0.1f, NULL, 1.0f, 1.0f); 
    }
    if (gamestate_is_livingroom_secret_passage_opened()) {
        C2D_DrawImageAt(img_hearth_opened, 150.0f, 108.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_rope_used_in_livingroom_hearth()) {
        C2D_DrawImageAt(img_hearth_prepared, 152.0f, 110.0f, 0.1f, NULL, 1.0f, 1.0f); 
    }
    if (gamestate_is_livingroom_golden_statue_placed()) {
        C2D_DrawImageAt(img_statue, 162.0f, 54.0f, 0.1f, NULL, 1.0f, 1.0f); 
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_south(void) {
    game_set_room(&corridor);
}

static void south_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_south);
}

Room livingroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = south_action},
    .north = {.action = north_action, .condition = gamestate_is_livingroom_secret_passage_opened},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};

