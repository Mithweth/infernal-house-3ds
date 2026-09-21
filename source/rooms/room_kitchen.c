#include <citro2d.h>
#include "game.h"
#include "gamestate.h"
#include "room_kitchen.h"
#include "room_corridor.h"
#include "gfx_kitchen.h"
#include "inventory.h"
#include "audio.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_drawer_opened;
static C2D_Image img_right_closet_opened;
static C2D_Image img_closet_below_opened;
static C2D_Image img_fridge_opened;
static C2D_Image img_oven_opened;


static void refrigerator_action(void) {
    gamestate_open_kitchen_refrigerator(!gamestate_is_kitchen_refrigerator_opened());
}

static Hotspot hotspots[] = {
    {
        .x = 0,
        .y = 34,
        .width = 48,
        .height = 175,
        .text_id = "KITCHEN_REFRIGERATOR",
        .is_active = NULL,
        .action = refrigerator_action
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_kitchen.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_bg_idx);
    img_drawer_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_drawer_opened_idx);
    img_right_closet_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_right_closet_opened_idx);
    img_closet_below_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_closet_below_opened_idx);
    img_fridge_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_fridge_opened_idx);
    img_oven_opened = C2D_SpriteSheetGetImage(room_scene, gfx_kitchen_oven_opened_idx);

}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_kitchen_refrigerator_opened()) {
        C2D_DrawImageAt(img_fridge_opened, 0.0f, 34.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_refrigerator_opened()) {
        C2D_DrawImageAt(img_fridge_opened, 0.0f, 34.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_below_closet_opened()) {
        C2D_DrawImageAt(img_closet_below_opened, 49.0f, 125.0f, 0.4f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_oven_opened()) {
        C2D_DrawImageAt(img_oven_opened, 118.0f, 136.0f, 0.5f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_right_closet_opened()) {
        C2D_DrawImageAt(img_right_closet_opened, 260.0f, 0.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_kitchen_drawer_opened()) {
        C2D_DrawImageAt(img_drawer_opened, 256.0f, 119.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_southwest(void) {
    game_set_room(&corridor);
}

static void southwest_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_southwest);
}

Room kitchen = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .southwest = southwest_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};