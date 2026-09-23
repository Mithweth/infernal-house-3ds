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

static bool piano_is_opened(void) {
    return gamestate_is_livingroom_piano_opened();
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

static void fireplace_use_item(ItemId item) {
    if (item == ITEM_STATUE) {
        gamestate_set_livingroom_golden_statue_placed();
        inventory_remove(ITEM_STATUE);
        game_show_message("LIVINGROOM_PLACE_STATUE");
    }
}

static bool hearth_opened_is_active(void) {
    return gamestate_is_livingroom_secret_passage_opened() && !gamestate_is_rope_used_in_livingroom_hearth();
}

static void hearth_use_item(ItemId item) {
    if (item == ITEM_ROPE) {
        gamestate_bind_rope_used_in_livingroom_hearth();
        inventory_remove(ITEM_ROPE);
        game_show_message("LIVINGROOM_USE_ROPE");
    }
}

static Hotspot hotspots[] = {
    {
        .x = 146,
        .y = 107,
        .width = 68,
        .height = 47,
        .text_id = "LIVINGROOM_FIREPLACE_HEARTH_OPENED",
        .is_active = hearth_opened_is_active,
        .use_item = hearth_use_item
    },
    {
        .x = 2,
        .y = 111,
        .width = 91,
        .height = 30,
        .text_id = "LIVINGROOM_OPENED_PIANO",
        .is_active = piano_is_opened,
        .action = play_piano_action
    },
    {
        .x = 2,
        .y = 111,
        .width = 91,
        .height = 30,
        .text_id = "LIVINGROOM_PIANO",
        .action = piano_action
    },
    {
        .x = 140,
        .y = 178,
        .width = 32,
        .height = 20,
        .text_id = "LIVINGROOM_LIGHTER",
        .is_active = lighter_is_active,
        .action = lighter_action
    },
    {
        .x = 104,
        .y = 114,
        .width = 31,
        .height = 37,
        .text_id = "LIVINGROOM_WOOD"
    },
    {
        .x = 9,
        .y = 29,
        .width = 52,
        .height = 54,
        .text_id = "LIVINGROOM_PAINTING"
    },
    {
        .x = 163,
        .y = 72,
        .width = 26,
        .height = 17,
        .text_id = "LIVINGROOM_FIREPLACE",
        .use_item = fireplace_use_item
    },
    {
        .x = 146,
        .y = 107,
        .width = 68,
        .height = 47,
        .text_id = "LIVINGROOM_FIREPLACE_HEARTH"
    },
};

static void create_new_routes(void) {
    if (gamestate_is_livingroom_secret_passage_opened()) {
        livingroom.north = north_action;
    } else {
        livingroom.north = NULL;
    }
}

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_livingroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_bg_idx);
    img_lighter = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_lighter_idx);
    img_hearth_opened = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_hearth_opened_idx);
    img_hearth_prepared = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_hearth_prepared_idx);
    img_piano_opened = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_piano_opened_idx);
    img_statue = C2D_SpriteSheetGetImage(room_scene, gfx_livingroom_statue_idx);
    create_new_routes();
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
    create_new_routes();
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
    .south = south_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};