// room_deadroom.c

#include <citro2d.h>
#include "game.h"
#include "room_deadroom.h"
#include "room_secondunderground.h"
#include "gfx_deadroom.h"
#include "inventory.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_ring_taken;
static C2D_Image img_rope_taken;

static void ring_action(void) {
    inventory_add(ITEM_RING);
}

static bool ring_is_active(void) {
    return !inventory_has(ITEM_RING);
}

static void rope_action(void) {
    inventory_add(ITEM_ROPE);
}

static bool rope_is_active(void) {
    return !inventory_has(ITEM_ROPE);
}

static Hotspot hotspots[] = {
    {
        .x = 6,
        .y = 188,
        .width = 131,
        .height = 44,
        .id = "DEADROOM_ROPE",
        .is_active = rope_is_active,
        .action = rope_action
    },
    {
        .x = 191,
        .y = 9,
        .width = 21,
        .height = 31,
        .id = "DEADROOM_RING",
        .is_active = ring_is_active,
        .action = ring_action
    },
    {
        .x = 104,
        .y = 0,
        .width = 30,
        .height = 40,
        .id = "DEADROOM_CHAINS",
        .message_id = "DEADROOM_CHAINS_EXAMINE"
    },
    {
        .x = 188,
        .y = 0,
        .width = 30,
        .height = 40,
        .id = "DEADROOM_CHAINS",
        .message_id = "DEADROOM_CHAINS_EXAMINE"
    },
    {
        .x = 131,
        .y = 41,
        .width = 53,
        .height = 168,
        .id = "DEADROOM_SKELETON",
        .message_id = "DEADROOM_SKELETON_EXAMINE"
    },
    {
        .x = 0,
        .y = 0,
        .width = 320,
        .height = 183,
        .id = "DEADROOM_WALL",
    },
    {
        .x = 18,
        .y = 193,
        .width = 282,
        .height = 49,
        .id = "DEADROOM_GROUND",
    },
};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_deadroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_deadroom_background_idx);
    img_ring_taken = C2D_SpriteSheetGetImage(room_scene, gfx_deadroom_ring_taken_idx);
    img_rope_taken = C2D_SpriteSheetGetImage(room_scene, gfx_deadroom_rope_taken_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (inventory_has(ITEM_RING)) {
        C2D_DrawImageAt(img_ring_taken, 191.0f, 8.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_ROPE)) {
        C2D_DrawImageAt(img_rope_taken, 0.0f, 186.0f, 0.2f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void south_action(void) {
    game_set_room(&secondunderground);
}

Room deadroom = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = south_action},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
