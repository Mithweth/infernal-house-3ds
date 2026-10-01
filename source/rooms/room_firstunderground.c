// room_firstunderground.c

#include <citro2d.h>
#include "gfxmap.h"
#include "game.h"
#include "room_firstunderground.h"
#include "room_secondunderground.h"
#include "room_cellar.h"
#include "minigame_measure.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet assets;
static C2D_Image img_bg;
static C2D_Image img_shovel_taken;
static C2D_Image img_sledgehammer_taken;
static C2D_Image img_hole_dug;
static C2D_Image img_hole_empty;

static void shovel_action(void) {
    inventory_add("SHOVEL");
}

static bool shovel_is_active(void) {
    return !inventory_has("SHOVEL");
}

static void sledgehammer_action(void) {
    inventory_add("SLEDGEHAMMER");
}

static bool sledgehammer_is_active(void) {
    return !inventory_has("SLEDGEHAMMER");
}

static bool target_is_active(void) {
    return !gamestate_get("underground_dug");
}

static bool x_form_use_item(const char *id) {
    if (strcmp(id, "MEASURING_TAPE") == 0) {
        game_minigame_start(&minigame_measure);
        return true;
    }
    return false;
}

static bool target_use_item(const char *id) {
    if (strcmp(id, "SHOVEL") == 0) {
        gamestate_set("underground_dug");
        game_show_message("UNDERGROUND_DIG");
        return true;
    }
    return false;
}

static bool ground_use_item(const char *id) {
    if (strcmp(id, "SHOVEL") == 0) {
        game_show_message("UNDERGROUND_DIG_ANYWHERE");
        return true;
    }
    return false;
}

static bool magnetic_card_is_active(void) {
    return !gamestate_get("underground_card_taken") && gamestate_get("underground_dug");
}

static void magnetic_card_action(void) {
    gamestate_set("underground_card_taken");
    inventory_add("MAGNETIC_CARD");
}

static Hotspot hotspots[] = {
    {
        .x = 275,
        .y = 182,
        .width = 24,
        .height = 14,
        .id = "UNDERGROUND_MAGNETIC_CARD",
        .condition = magnetic_card_is_active,
        .action = magnetic_card_action
    },
    {
        .x = 284,
        .y = 176,
        .width = 12,
        .height = 12,
        .id = "UNDERGROUND_GROUND",
        .condition = target_is_active,
        .use_item = target_use_item
    },
    {
        .x = 141,
        .y = 190,
        .width = 17,
        .height = 16,
        .id = "UNDERGROUND_X_FORM",
        .message_id = "UNDERGROUND_X_FORM_EXAMINE",
        .use_item = x_form_use_item
    },
    {
        .x = 132,
        .y = 32,
        .width = 104,
        .height = 176,
        .id = "UNDERGROUND_LADDER",
    },
    {
        .x = 18,
        .y = 89,
        .width = 37,
        .height = 100,
        .id = "UNDERGROUND_SHOVEL",
        .condition = shovel_is_active,
        .action = shovel_action
    },
    {
        .x = 53,
        .y = 84,
        .width = 35,
        .height = 102,
        .id = "UNDERGROUND_SLEDGEHAMMER",
        .condition = sledgehammer_is_active,
        .action = sledgehammer_action
    },
    {
        .x = 45,
        .y = 171,
        .width = 275,
        .height = 68,
        .id = "UNDERGROUND_GROUND",
        .use_item = ground_use_item
    },
};

static void room_init(void) {
    if (!gfxmap_load_assets("romfs:/gfx/firstunderground", &assets)) {
        return;
    }
    img_bg = gfxmap_get_image(assets, "gfx_bg_idx");
    img_shovel_taken = gfxmap_get_image(assets, "gfx_shovel_taken_idx");
    img_sledgehammer_taken = gfxmap_get_image(assets, "gfx_sledgehammer_taken_idx");
    img_hole_dug = gfxmap_get_image(assets, "gfx_hole_dug_idx");
    img_hole_empty = gfxmap_get_image(assets, "gfx_hole_empty_idx");
}

static void room_draw(void) {
    C2D_DrawImageAt(img_bg, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (inventory_has("SHOVEL")) {
        C2D_DrawImageAt(img_shovel_taken, 15.0f, 85.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has("SLEDGEHAMMER")) {
        C2D_DrawImageAt(img_sledgehammer_taken, 46.0f, 83.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("underground_dug")) {
        if (gamestate_get("underground_card_taken")) {
            C2D_DrawImageAt(img_hole_empty, 265.0f, 175.0f, 0.3f, NULL, 1.0f, 1.0f);
        } else {
            C2D_DrawImageAt(img_hole_dug, 261.0f, 174.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
    }

}

static void room_close(void) {
    C2D_SpriteSheetFree(assets);
}

static void move_north(void) {
    game_set_room(&cellar);
}

static void east_action(void) {
    game_set_room(&secondunderground);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/metal_ladder.raw", move_north);
}

Room firstunderground = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = {.action = north_action},
    .east = {.action = east_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
