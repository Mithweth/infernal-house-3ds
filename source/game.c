// game.c
#include <3ds.h>
#include "game.h"
#include "lang.h"

static Room *current_room = NULL;
static GameMode game_mode = GAME_NORMAL;
static Hotspot *last_hotspot = NULL;
static const char *examine_text = NULL;
static C2D_TextBuf text_buf;
static C2D_Text text;

void game_set_room(Room *room) {

	if (!text_buf)
        text_buf = C2D_TextBufNew(4096);

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

void game_update(u32 keys) {
    if (!(keys & KEY_TOUCH))
        return;

    if (game_mode == GAME_EXAMINE) {
        game_mode = GAME_NORMAL;
        return;
    }

    touchPosition touch;
    hidTouchRead(&touch);

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
