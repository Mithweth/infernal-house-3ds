#include <citro2d.h>

// Replace cellar

#include "game.h"
#include "room_cellar.h"
#include "room_corridor.h"
#include "gfx_cellar.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_alarm_box_opened;
static C2D_Image img_alarm_box_enabled;

static void alarm_box_use_item(ItemId item) {
    if ((item == ITEM_SCREWDRIVER) && (!gamestate_is_cellar_alarm_box_unscrewed())) {
        gamestate_unscrew_cellar_alarm_box();
        game_show_message("CELLAR_OPEN_ALARM_BOX");
    }
}

static void alarm_box_action(void) {
    if (gamestate_is_cellar_alarm_box_unscrewed()) {
        gamestate_open_cellar_alarm_box();
    }}

static void opened_alarm_box_action(void) {
    gamestate_disable_diningroom_lasers(!gamestate_are_diningroom_lasers_disabled());
    if (gamestate_are_diningroom_lasers_disabled()) {
        game_show_message("CELLAR_DISABLE_ALARM_BOX");
    } else {
        game_show_message("CELLAR_ENABLE_ALARM_BOX");
    }
}

static bool opened_alarm_box_is_active(void) {
    return gamestate_is_cellar_alarm_box_opened();
}

static void simon_action(void) {
    game_start_simon();
}

static Hotspot hotspots[] = {
    {
        .x = 129,
        .y = 24,
        .width = 40,
        .height = 45,
        .text_id = "CELLAR_OPENED_ALARM_BOX",
        .is_active = opened_alarm_box_is_active,
        .action = opened_alarm_box_action
    },
    {
        .x = 129,
        .y = 24,
        .width = 40,
        .height = 45,
        .text_id = "CELLAR_ALARM_BOX",
        .action = alarm_box_action,
        .use_item = alarm_box_use_item
    },
    {
        .x = 211,
        .y = 39,
        .width = 74,
        .height = 29,
        .text_id = "CELLAR_JAM",
    },
    {
        .x = 212,
        .y = 70,
        .width = 73,
        .height = 40,
        .text_id = "CELLAR_BOTTLES",
    },
    {
        .x = 216,
        .y = 111,
        .width = 38,
        .height = 36,
        .text_id = "CELLAR_TOOLS",
    },
    {
        .x = 217,
        .y = 199,
        .width = 103,
        .height = 41,
        .text_id = "CELLAR_CARPET",
    },
    {
        .x = 255,
        .y = 115,
        .width = 31,
        .height = 36,
        .text_id = "CELLAR_SIMON",
        .is_active = NULL,
        .action = simon_action
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_cellar.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_cellar_bg_idx);
    img_alarm_box_opened = C2D_SpriteSheetGetImage(room_scene, gfx_cellar_alarm_box_opened_idx);
    img_alarm_box_enabled = C2D_SpriteSheetGetImage(room_scene, gfx_cellar_alarm_box_enabled_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_are_diningroom_lasers_disabled()) {
        C2D_DrawImageAt(img_alarm_box_enabled, 135.0f, 20.0f, 0.3f, NULL, 1.0f, 1.0f);
    } else if (gamestate_is_cellar_alarm_box_opened()) {
        C2D_DrawImageAt(img_alarm_box_opened, 135.0f, 20.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
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

Room cellar = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .east = east_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};