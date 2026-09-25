// room_library.c

#include <citro2d.h>
#include "game.h"
#include "room_library.h"
#include "room_corridor.h"
#include "gfx_library.h"
#include "inventory.h"
#include "gamestate.h"
#include "audio.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
static C2D_Image img_statue_taken;
static C2D_Image img_cup_taken;
static C2D_Image img_book_pushed;
static C2D_Image img_passage_opened;
static C2D_Image img_clock_taken;

static bool statue_is_active(void) {
    return !gamestate_is_library_statue_taken();
}

static void statue_action(void) {
    gamestate_take_library_statue();
    inventory_add(ITEM_STATUE);
}

static bool clock_is_active(void) {
    return !inventory_has(ITEM_CLOCK);
}

static void clock_action(void) {
    inventory_add(ITEM_CLOCK);
}

static bool cup_is_active(void) {
    return !inventory_has(ITEM_CUP);
}

static void cup_action(void) {
    inventory_add(ITEM_CUP);
}

static bool book_is_active(void) {
    return !gamestate_is_library_secret_passage_opened();
}

static void book_action(void) {
    gamestate_open_library_secret_passage();
    sfx_play("romfs:/audio/book_pushed.raw");
    game_show_message("LIBRARY_SECRET_PASSAGE");
}

static Hotspot hotspots[] = {
    {
        .x = 13,
        .y = 38,
        .width = 25,
        .height = 30,
        .id = "ITEM_STATUE",
        .is_active = statue_is_active,
        .action = statue_action
    },
    {
        .x = 10,
        .y = 71,
        .width = 31,
        .height = 30,
        .id = "ITEM_CLOCK",
        .is_active = clock_is_active,
        .action = clock_action
    },
    {
        .x = 103,
        .y = 61,
        .width = 21,
        .height = 25,
        .id = "ITEM_CUP",
        .is_active = cup_is_active,
        .action = cup_action
    },
    {
        .x = 73,
        .y = 108,
        .width = 16,
        .height = 24,
        .id = "LIBRARY_BOOK",
        .is_active = book_is_active,
        .action = book_action
    },
    {
        .x = 54,
        .y = 37,
        .width = 51,
        .height = 36,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 10,
        .y = 104,
        .width = 48,
        .height = 30,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 60,
        .y = 23,
        .width = 41,
        .height = 30,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 95,
        .y = 78,
        .width = 35,
        .height = 30,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 60,
        .y = 80,
        .width = 37,
        .height = 25,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 60,
        .y = 108,
        .width = 13,
        .height = 24,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 101,
        .y = 37,
        .width = 30,
        .height = 25,
        .id = "LIBRARY_BOOK"
    },
    {
        .x = 144,
        .y = 49,
        .width = 39,
        .height = 84,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_SCIENCE_BOOKS"
    },
    {
        .x = 191,
        .y = 45,
        .width = 28,
        .height = 86,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_RARE_BOOKS"
    },
    {
        .x = 227,
        .y = 40,
        .width = 33,
        .height = 70,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_NOVELS"
    },
    {
        .x = 226,
        .y = 114,
        .width = 33,
        .height = 18,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_EASTEREGGS"
    },
    {
        .x = 0,
        .y = 196,
        .width = 40,
        .height = 26,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_CLONE_BOOK"
    },
    {
        .x = 238,
        .y = 152,
        .width = 40,
        .height = 17,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_ROBOT_BOOK"
    },
    {
        .x = 296,
        .y = 40,
        .width = 24,
        .height = 57,
        .id = "LIBRARY_BOOK",
        .message_id = "LIBRARY_PAINTING"
    },
};

static void north_action(void) {
    //game_set_room(&corridor);
}

static void southeast_action(void) {
    game_set_room(&corridor);
}

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_library.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_library_bg_idx);
    img_statue_taken = C2D_SpriteSheetGetImage(room_scene, gfx_library_statue_taken_idx);
    img_cup_taken = C2D_SpriteSheetGetImage(room_scene, gfx_library_cup_taken_idx);
    img_book_pushed = C2D_SpriteSheetGetImage(room_scene, gfx_library_pushed_book_idx);
    img_clock_taken = C2D_SpriteSheetGetImage(room_scene, gfx_library_clock_taken_idx);
    img_passage_opened = C2D_SpriteSheetGetImage(room_scene, gfx_library_passage_opened_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    if (gamestate_is_library_statue_taken()) {
        C2D_DrawImageAt(img_statue_taken, 13.0f, 39.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_CLOCK)) {
        C2D_DrawImageAt(img_clock_taken, 11.0f, 71.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (inventory_has(ITEM_CUP)) {
        C2D_DrawImageAt(img_cup_taken, 103.0f, 62.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
    if (gamestate_is_library_secret_passage_opened()) {
        C2D_DrawImageAt(img_book_pushed, 75.0f, 108.0f, 0.3f, NULL, 1.0f, 1.0f);
        C2D_DrawImageAt(img_passage_opened, 127.0f, 29.0f, 0.3f, NULL, 1.0f, 1.0f);
    }
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

Room library = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .southeast = {.action = southeast_action},
    .north = {.action = north_action, .condition = gamestate_is_library_secret_passage_opened},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
