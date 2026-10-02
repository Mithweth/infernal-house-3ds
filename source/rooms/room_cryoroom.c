// room_cryoroom.c

#include <citro2d.h>
#include "gfxmap.h"
#include "game.h"
#include "room_cryoroom.h"
#include "room_livingroom.h"
#include "room_heliport.h"
#include "inventory.h"
#include "gamestate.h"
#include "audio.h"

static C2D_SpriteSheet assets;
static C2D_Image img_background;
static C2D_Image img_hangar;
static C2D_Image img_lever;
static C2D_Image img_rust_cleaned;
static C2D_Image img_chamber_opened;

static void lever_action(void) {
    return gamestate_set("cryoroom_lever_use");
}

static bool lever_is_pulled(void) {
    return gamestate_get("cryoroom_lever_use");
}

static bool lever_is_not_pulled(void) {
    return !gamestate_get("cryoroom_lever_use");
}

static bool sign_is_rusted(void) {
    return gamestate_get("cryoroom_rust_cleaned");
}

static bool sign_use_item(const char *id) {
    if (strcmp(id, "STAIN_REMOVER") == 0) {
        inventory_remove("STAIN_REMOVER");
        gamestate_set("cryoroom_rust_cleaned");
        return true;
    }
    return false;
}

static void digicode_action(void) {
    if (gamestate_get("cryoroom_digicode_enabled")) {
        game_minigame_start("digicode");
    } else {
        game_show_message("CRYOROOM_PANEL_NEED_CARD");
    }
}

static bool digicode_use_item(const char *id) {
    if (strcmp(id, "MAGNETIC_CARD") == 0) {
        inventory_remove("MAGNETIC_CARD");
        game_show_message("CRYOROOM_PANEL_USE_CARD");
        sfx_play("romfs:/audio/confirmation.raw");
        gamestate_set("cryoroom_digicode_enabled");
        return true;
    }
    return false;
}

static void open_cryo_chamber(void) {
    gamestate_set("cryoroom_chamber_opened");
}

static Hotspot hotspots[] = {
    {
        .x = 97,
        .y = 58,
        .width = 50,
        .height = 17,
        .id = "CRYOROOM_SIGN",
        .message_id = "CRYOROOM_RUST_SIGN_MESSAGE",
        .condition = sign_is_rusted,
        .use_item = sign_use_item
    },
    {
        .x = 97,
        .y = 58,
        .width = 50,
        .height = 17,
        .id = "CRYOROOM_SIGN",
        .message_id = "CRYOROOM_SIGN_MESSAGE"
    },
    {
        .x = 273,
        .y = 37,
        .width = 47,
        .height = 164,
        .id = "CRYOROOM_STORAGE",
        .condition = lever_is_not_pulled
    },
    {
        .x = 252,
        .y = 115,
        .width = 20,
        .height = 25,
        .id = "CRYOROOM_LEVER",
        .action = lever_action
    },
    {
        .x = 145,
        .y = 102,
        .width = 22,
        .height = 26,
        .id = "CRYOROOM_PANEL_CONTROL",
        .message_id = "CRYOROOM_PANEL_CONTROL_MESSAGE",
        .action = digicode_action,
        .use_item = digicode_use_item
    },
    {
        .x = 99,
        .y = 78,
        .width = 45,
        .height = 82,
        .id = "CRYOROOM_DOOR"
    },
    {
        .x = 37,
        .y = 78,
        .width = 22,
        .height = 22,
        .id = "CRYOROOM_TANK_MESSAGE"
    },
    {
        .x = 1,
        .y = 41,
        .width = 28,
        .height = 82,
        .id = "CRYOROOM_BOX"
    },
    {
        .x = 65,
        .y = 76,
        .width = 30,
        .height = 69,
        .id = "CRYOROOM_BOX"
    },
    {
        .x = 67,
        .y = 161,
        .width = 26,
        .height = 24,
        .id = "CRYOROOM_CRYOCHAMBER_CONSOLE",
        .action = open_cryo_chamber
    },
    {
        .x = 0,
        .y = 124,
        .width = 68,
        .height = 51,
        .id = "CRYOROOM_CRYOCHAMBER"
    },
    {
        .x = 170,
        .y = 72,
        .width = 62,
        .height = 79,
        .id = "CRYOROOM_ELEVATOR",
        .message_id = "CRYOROOM_ELEVATOR_MESSAGE"
    },
};

static void room_init(void) {
    if (!gfxmap_load_assets("romfs:/gfx/cryoroom", &assets)) {
        return;
    }
    img_background = gfxmap_get_image(assets, "gfx_background_idx");
    img_hangar = gfxmap_get_image(assets, "gfx_hangar_idx");
    img_lever = gfxmap_get_image(assets, "gfx_lever_idx");
    img_chamber_opened = gfxmap_get_image(assets, "gfx_cryo_opened_idx");
    img_rust_cleaned = gfxmap_get_image(assets, "gfx_rust_cleaned_idx");
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_get("cryoroom_lever_use")) {
        C2D_DrawImageAt(img_hangar, 271.0f, 12.0f, 0.1f, NULL, 1.0f, 1.0f);
        C2D_DrawImageAt(img_lever, 255.0f, 100.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("cryoroom_rust_cleaned")) {
        C2D_DrawImageAt(img_rust_cleaned, 97.0f, 58.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_get("cryoroom_chamber_opened")) {
        C2D_DrawImageAt(img_chamber_opened, 0.0f, 99.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(assets);
}

static void move_north(void) {
    game_set_room(&livingroom);
}

static void north_action(void) {
    game_wait_for_sfx("romfs:/audio/rope_climbing.raw", move_north);
}

static void east_action(void) {
    game_set_room(&heliport);
}

Room cryoroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = {.action = north_action},
    .east = {.action = east_action, .condition = lever_is_pulled},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
