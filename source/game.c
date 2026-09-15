// game.c
#include <3ds.h>
#include "game.h"

static Room *current_room = NULL;
static GameMode game_mode = GAME_NORMAL;
static Hotspot *last_hotspot = NULL;
static const char *examine_text = NULL;

void game_set_room(Room *room) {
    if (current_room && current_room->close)
        current_room->close();

    current_room = room;

    last_hotspot = NULL;
    game_mode = GAME_NORMAL;
    examine_text = NULL;

    if (current_room && current_room->init)
        current_room->init();
}

void game_close(void) {
    if (current_room && current_room->close)
        current_room->close();

    current_room = NULL;
}

void game_draw(void) {
    if (current_room && current_room->draw) {
        current_room->draw();
    }
}

static Hotspot *find_hotspot(int x, int y) {
    if (!current_room) {
        return NULL;
    }

    for (size_t i = 0; i < current_room->hotspot_count; i++) {
        Hotspot *hotspot = &current_room->hotspots[i];

        if (hotspot->is_active && !hotspot->is_active()) {
            continue;
        }

        if (x >= hotspot->x && x <  hotspot->x + hotspot->width && y >= hotspot->y && y <  hotspot->y + hotspot->height) {
            return hotspot;
        }
    }

    return NULL;
}

void game_update(u32 keys) {
    if (!(keys & KEY_TOUCH)) {
        return;
    }

    if (game_mode == GAME_EXAMINE) {
        game_mode = GAME_NORMAL;
        return;
    }

    touchPosition touch;
    hidTouchRead(&touch);

    Hotspot *hotspot = find_hotspot(touch.px, touch.py);

    if (!hotspot) {
        last_hotspot = NULL;
        return;
    }

    if (hotspot == last_hotspot) {
        if (hotspot->action)
            hotspot->action();

        last_hotspot = NULL;
        return;
    }

    last_hotspot = hotspot;
    examine_text = hotspot->description;
    game_mode = GAME_NORMAL;
}

// void game_draw(void)
// {
//     if (!current_room)
//         return;

//     if (current_room->draw)
//         current_room->draw();

//     /*
//      * Plus tard :
//      *
//      * if (game_mode == GAME_EXAMINE)
//      *     draw_examine_text(examine_text);
//      */
// }
