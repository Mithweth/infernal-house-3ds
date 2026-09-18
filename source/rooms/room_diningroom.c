#include <citro2d.h>

#include "game.h"
#include "audio.h"
#include "room_diningroom.h"

#include "gfx_diningroom.h"
#include "inventory.h"
#include "room_hall.h"
#include "room_corridor.h"
#include "gamestate.h"
#include "gameover.h"

// static bool left_closet_opened = false;
// static bool right_closet_opened = false;
// static bool tureen_opened = false;

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_left_closet_opened;
static C2D_Image img_right_closet_opened;
static C2D_Image img_laser_disabled;
static C2D_Image img_soup_empty;
static C2D_Image img_soup_opened;

static void right_cabinet_door_action(void) {
    if (gamestate_is_diningroom_right_cabinet_door_opened()) {
        sfx_play("romfs:/audio/closet_close.raw");
    } else {
        sfx_play("romfs:/audio/closet_open.raw");
    }
    gamestate_open_diningroom_right_cabinet_door(!gamestate_is_diningroom_right_cabinet_door_opened());
}

static void left_cabinet_door_action(void) {
    if (gamestate_is_diningroom_left_cabinet_door_opened()) {
        sfx_play("romfs:/audio/closet_close.raw");
    } else {
        sfx_play("romfs:/audio/closet_open.raw");
    }
    gamestate_open_diningroom_left_cabinet_door(!gamestate_is_diningroom_left_cabinet_door_opened());
}

static void soup_cover_action(void) {
    gamestate_move_diningroom_soup(!gamestate_is_diningroom_soup_opened());
}

static bool soup_message_is_active(void) {
    return gamestate_is_diningroom_soup_opened() && !inventory_has(ITEM_SCORE);
}

static void soup_contents_action(void) {
    inventory_add(ITEM_SCORE);
}

static Hotspot hotspots[] = {
    {
        .x = 130,
        .y = 24,
        .width = 100,
        .height = 168,
        .text_id = "DINING_LASERS",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 36,
        .y = 103,
        .width = 36,
        .height = 30,
        .text_id = "DINING_SOUP_CONTENTS",
        .is_active = soup_message_is_active,
        .action = soup_contents_action,
    },
    {
        .x = 36,
        .y = 103,
        .width = 36,
        .height = 30,
        .text_id = "DINING_SOUP",
        .is_active = NULL,
        .action = soup_cover_action,
    },
    {
        .x = 5,
        .y = 135,
        .width = 60,
        .height = 75,
        .text_id = "DINING_LEFT_CABINET_CONTENTS",
        .is_active = gamestate_is_diningroom_left_cabinet_door_opened,
        .action = left_cabinet_door_action
    },
    {
        .x = 66,
        .y = 134,
        .width = 55,
        .height = 60,
        .text_id = "DINING_RIGHT_CABINET_CONTENTS",
        .is_active = gamestate_is_diningroom_right_cabinet_door_opened,
        .action = right_cabinet_door_action
    },
    {
        .x = 5,
        .y = 135,
        .width = 60,
        .height = 75,
        .text_id = "DINING_CABINET",
        .is_active = NULL,
        .action = left_cabinet_door_action
    },
    {
        .x = 66,
        .y = 134,
        .width = 55,
        .height = 60,
        .text_id = "DINING_CABINET",
        .is_active = NULL,
        .action = right_cabinet_door_action
    },
    {
        .x = 3,
        .y = 101,
        .width = 111,
        .height = 109,
        .text_id = "DINING_CABINET",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 3,
        .y = 22,
        .width = 101,
        .height = 79,
        .text_id = "DINING_UP_CABINET",
        .is_active = NULL,
        .action = NULL
    },
    {
        .x = 265,
        .y = 105,
        .width = 25,
        .height = 25,
        .text_id = "DINING_SWITCH",
        .is_active = NULL,
        .action = NULL
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_diningroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_bg_idx);
    img_laser_disabled = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_laser_disabled_idx);
    img_left_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_left_closet_opened_idx);
    img_right_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_right_closet_opened_idx);
    img_soup_empty = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_soup_empty_idx);
    img_soup_opened = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_soup_opened_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_are_diningroom_lasers_disabled()) {
        C2D_DrawImageAt(img_laser_disabled, 132.0f, 22.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_diningroom_left_cabinet_door_opened()) {
       C2D_DrawImageAt(img_left_closet_opened, 5.0f, 135.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_diningroom_right_cabinet_door_opened()) {
       C2D_DrawImageAt(img_right_closet_opened, 68.0f, 133.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_diningroom_soup_opened()) {
        if (!inventory_has(ITEM_SCORE)) {
            C2D_DrawImageAt(img_soup_opened, 26.0f, 106.0f, 0.2f, NULL, 1.0f, 1.0f);
        } else {
            C2D_DrawImageAt(img_soup_empty, 26.0f, 106.0f, 0.2f, NULL, 1.0f, 1.0f);
        }
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_right(void) {
    game_set_room(&hall);
}

static void right_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_right);
}

static void left_action(void) {
    game_set_room(&corridor);
}

static void lasers_gameover(void) {
	game_over(GAMEOVER_LASERS);
}

static void up_action(void) {
    if (!gamestate_are_diningroom_lasers_disabled()) {
        game_wait_for_sfx("romfs:/audio/police_siren.raw", lasers_gameover);
    }
}

Room dining_room = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .up = up_action,
    .down = NULL,
    .left = left_action,
    .right = right_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};