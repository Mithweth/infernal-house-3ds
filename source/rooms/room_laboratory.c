// room_laboratory.c

#include <citro2d.h>
#include "game.h"
#include "room_laboratory.h"
#include "room_secondfloor.h"
#include "gfx_laboratory.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_syringe_taken;

static void syringe_action(void) {
    gamestate_set("laboratory_syringe_taken");
    inventory_add("SYRINGE");
}

static bool syringe_is_here(void) {
    return !gamestate_get("laboratory_syringe_taken");
}
static Hotspot hotspots[] = {
    {
        .x = 23,
        .y = 178,
        .width = 64,
        .height = 30,
        .id = "LABORATORY_SYRINGE",
        .condition = syringe_is_here,
        .action = syringe_action
    },
    {
        .x = 170,
        .y = 71,
        .width = 95,
        .height = 103,
        .id = "LABORATORY_BLACKBOARD"
    },
    {
        .x = 170,
        .y = 71,
        .width = 95,
        .height = 103,
        .id = "LABORATORY_BLACKBOARD"
    },
    {
        .x = 3,
        .y = 47,
        .width = 32,
        .height = 52,
        .id = "LABORATORY_TOOLS"
    },
    {
        .x = 7,
        .y = 98,
        .width = 92,
        .height = 23,
        .id = "LABORATORY_TOOLS"
    },
    {
        .x = 33,
        .y = 130,
        .width = 49,
        .height = 42,
        .id = "LABORATORY_TEST_TUBE"
    },
    {
        .x = 113,
        .y = 181,
        .width = 16,
        .height = 13,
        .id = "LABORATORY_FLASK",
        .message_id = "LABORATORY_FLASK_LABEL"
    },
    {
        .x = 99,
        .y = 152,
        .width = 37,
        .height = 52,
        .id = "LABORATORY_FLASK"
    },
    {
        .x = 285,
        .y = 93,
        .width = 10,
        .height = 15,
        .id = "LABORATORY_LABCOAT",
        .message_id = "LABORATORY_LABCOAT_POCKET"
    },
    {
        .x = 272,
        .y = 68,
        .width = 30,
        .height = 108,
        .id = "LABORATORY_LABCOAT"
    },
    {
        .x = 57,
        .y = 27,
        .width = 32,
        .height = 20,
        .id = "LABORATORY_CONDITIONER"
    },
    {
        .x = 88,
        .y = 2,
        .width = 116,
        .height = 19,
        .id = "LABORATORY_NEON_LIGHT"
    }
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_laboratory.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_laboratory_background_idx);
    img_syringe_taken = C2D_SpriteSheetGetImage(room_scene, gfx_laboratory_syringe_taken_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_get("laboratory_syringe_taken")) {
        C2D_DrawImageAt(img_syringe_taken, 23.0f, 177.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_east(void) {
    game_set_room(&secondfloor);
}

static void east_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_east);
}

Room laboratory = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .east = {.action = east_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
