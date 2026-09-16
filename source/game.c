// game.c
#include <3ds.h>
#include "game.h"
#include "lang.h"
#include "inventory.h"

const int THRESHOLD = 80;
static Room *current_room = NULL;
static GameMode game_mode = GAME_NORMAL;
static bool circle_ready = true;
static Hotspot *last_hotspot = NULL;
static const char *examine_text = NULL;
static C2D_TextBuf text_buf;
static C2D_Text text;

void game_set_room(Room *room) {

	if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
	}

    if (current_room && current_room->close) {
        current_room->close();
    }

    current_room = room;

    last_hotspot = NULL;
    game_mode = GAME_NORMAL;
    examine_text = NULL;

    if (current_room && current_room->init) {
        current_room->init();
    }
}

void game_close(void) {
    if (current_room && current_room->close)
        current_room->close();

    current_room = NULL;

    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
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

bool game_can_move_up(void) {
    return current_room && current_room->up;
}

bool game_can_move_down(void) {
    return current_room && current_room->down;
}

bool game_can_move_left(void) {
    return current_room && current_room->left;
}

bool game_can_move_right(void) {
    return current_room && current_room->right;
}

static void update_movement(circlePosition analog) {
	bool neutral = analog.dx > -THRESHOLD && analog.dx < THRESHOLD && analog.dy > -THRESHOLD && analog.dy < THRESHOLD;

    printf("circle: dx=%d dy=%d neutral=%d\n", analog.dx, analog.dy, neutral);
    if (neutral) {
        circle_ready = true;
        return;
    }

    if (!circle_ready) {
        return;
    }

    if (analog.dy > THRESHOLD && current_room->up) {
        circle_ready = false;
        game_set_room(current_room->up);
    } else if (analog.dy < -THRESHOLD && current_room->down) {
        circle_ready = false;
        game_set_room(current_room->down);
    } else if (analog.dx < -THRESHOLD && current_room->left) {
        circle_ready = false;
        game_set_room(current_room->left);
    } else if (analog.dx > THRESHOLD && current_room->right) {
        circle_ready = false;
        game_set_room(current_room->right);
    }
}

static void update_touch(touchPosition touch) {
    if (game_mode == GAME_EXAMINE) {
        game_mode = GAME_NORMAL;
        return;
    }

    Hotspot *hotspot = find_hotspot(touch.px, touch.py);

    if (!hotspot) {
        return;
    }

    if (hotspot == last_hotspot) {
        if (hotspot->action)
            hotspot->action();
        return;
    }

    last_hotspot = hotspot;
    examine_text = lang_get(hotspot->text_id);
    game_mode = GAME_EXAMINE;
}

void game_update(u32 keys, circlePosition analog, touchPosition touch) {
    if (inventory_is_active()) {
        return;
    }

    update_movement(analog);

    if (keys & KEY_TOUCH) {
        update_touch(touch);
    }
}

void game_draw(void) {
    if (!current_room)
        return;

    if (current_room->draw)
        current_room->draw();

    if (game_mode == GAME_EXAMINE) {
	    C2D_TextBufClear(text_buf);
	    C2D_TextParse(&text, text_buf, examine_text);
	    C2D_TextOptimize(&text);
	    C2D_DrawRectSolid(10.0f, 185.0f, 0.8f, 300.0f, 45.0f, C2D_Color32(0, 0, 0, 180));
	    C2D_DrawText(&text, C2D_WithColor, 20.0f, 197.0f, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
	}
}
