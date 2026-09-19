// game.c
#include <3ds.h>
#include "game.h"
#include "lang.h"
#include "inventory.h"
#include "gamestate.h"
#include "room_hall.h"
#include "gameover.h"
#include "hud.h"
#include "audio.h"
#include "title.h"

const int THRESHOLD = 80;
static Room *current_room = NULL;
static GameMode game_mode = GAME_NORMAL;
static bool circle_ready = true;
static Hotspot *last_hotspot = NULL;
static const char *message_text = NULL;
static C2D_TextBuf text_buf;
static C2D_Text text;
static void (*game_busy_callback)(void) = NULL;
static int game_busy_sfx_channel = -1;


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
    message_text = NULL;

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

void game_over(GameOverId id) {
    game_mode = GAME_OVER;
    gameover_init(id);
}

void game_init(void) {
    game_mode = GAME_TITLE;
    title_init();
}

void game_start(void) {
    title_close();
    inventory_reset();
    gamestate_reset();
    gameover_close();
    hud_reset();
    game_mode = GAME_NORMAL;
    message_text = NULL;
    last_hotspot = NULL;
    music_play("romfs:/audio/background.ogg");
    game_set_room(&hall);
}

void game_wait_for_sfx(const char *sfx, void (*callback)(void)) {
    game_busy_sfx_channel = sfx_play(sfx);
    game_busy_callback = callback;
    game_mode = GAME_BUSY;
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

bool game_can_move_north(void) {
    return current_room && current_room->north;
}

bool game_can_move_northeast(void) {
    return current_room && current_room->northeast;
}

bool game_can_move_east(void) {
    return current_room && current_room->east;
}

bool game_can_move_southeast(void) {
    return current_room && current_room->southeast;
}

bool game_can_move_south(void) {
    return current_room && current_room->south;
}

bool game_can_move_southwest(void) {
    return current_room && current_room->southwest;
}

bool game_can_move_west(void) {
    return current_room && current_room->west;
}

bool game_can_move_northwest(void) {
    return current_room && current_room->northwest;
}

static void update_movement(circlePosition analog) {
    bool neutral = analog.dx > -THRESHOLD && analog.dx < THRESHOLD && analog.dy > -THRESHOLD && analog.dy < THRESHOLD;
    if (neutral) {
        circle_ready = true;
        return;
    }

    if (!circle_ready) {
        return;
    }

    if (analog.dx > THRESHOLD && analog.dy > THRESHOLD && current_room->northeast) {
        circle_ready = false;
        current_room->northeast();
    } else if (analog.dx > THRESHOLD && analog.dy < -THRESHOLD && current_room->southeast) {
        circle_ready = false;
        current_room->southeast();
    } else if (analog.dx < -THRESHOLD && analog.dy > THRESHOLD && current_room->northwest) {
        circle_ready = false;
        current_room->northwest();
    } else if (analog.dx < -THRESHOLD && analog.dy < -THRESHOLD && current_room->southwest) {
        circle_ready = false;
        current_room->southwest();
    } else if (analog.dy > THRESHOLD && current_room->north) {
        circle_ready = false;
        current_room->north();
    } else if (analog.dy < -THRESHOLD && current_room->south) {
        circle_ready = false;
        current_room->south();
    } else if (analog.dx < -THRESHOLD && current_room->west) {
        circle_ready = false;
        current_room->west();
    } else if (analog.dx > THRESHOLD && current_room->east) {
        circle_ready = false;
        current_room->east();
    }
}

static void update_touch(touchPosition touch) {
    if (game_mode == GAME_MESSAGE) {
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
    game_show_message(hotspot->text_id);
}

static void room_draw(void) {
    if (!current_room)
        return;

    if (current_room->draw)
        current_room->draw();

    if (game_mode == GAME_MESSAGE) {
        C2D_TextBufClear(text_buf);
        C2D_TextParse(&text, text_buf, message_text);
        C2D_TextOptimize(&text);
        float width, height;
        C2D_TextGetDimensions(&text, 0.5f, 0.5f, &width, &height);
        float box_height = height + 20.0f;
        float box_y = 240.0f - box_height - 10.0f;
        C2D_DrawRectSolid(10.0f, box_y, 0.8f, 300.0f, box_height, C2D_Color32(0, 0, 0, 180));
        C2D_DrawText(&text, C2D_WithColor, 20.0f, box_y + 10.0f, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    }
}

void game_use_item(ItemId item) {
    if (!last_hotspot) {
        return;
    }

    if (last_hotspot->use_item) {
        last_hotspot->use_item(item);
    }
}

void game_show_message(const char *message_id) {
    message_text = lang_get(message_id);
    game_mode = GAME_MESSAGE;
}

void game_update(u32 keys, circlePosition analog, touchPosition touch) {
    switch (game_mode) {
        case GAME_TITLE:
            title_update(keys);
            return;

        case GAME_INTRO:
            //intro_update(keys);
            return;

        case GAME_OVER:
            if (keys & KEY_A) {
                game_start();
            }
            return;

        case GAME_BUSY:
            hud_update();
            if (game_busy_sfx_channel > -1) {
                if (!sfx_is_playing(game_busy_sfx_channel)) {
                    game_mode = GAME_NORMAL;
                    if (game_busy_callback) {
                        void (*callback)(void) = game_busy_callback;
                        game_busy_callback = NULL;
                        game_busy_sfx_channel = -1;
                        callback();
                    }
                }
            }
            return;

        default:
            hud_update();
            break;
    }

    if ((inventory_is_active()) | (inventory_update(keys))) {
        return;
    }

    update_movement(analog);

    if (keys & KEY_TOUCH) {
        update_touch(touch);
    }
}

void game_draw(C3D_RenderTarget *top, C3D_RenderTarget *bottom) {
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
    switch (game_mode) {
        case GAME_TITLE:
            C2D_SceneBegin(top);
            title_draw_top();
            C2D_SceneBegin(bottom);
            title_draw_bottom();
            break;

        case GAME_INTRO:
            C2D_SceneBegin(top);
            //intro_draw_top();

            C2D_SceneBegin(bottom);
            //intro_draw_bottom();
            break;

        case GAME_OVER:
            C2D_SceneBegin(top);
            gameover_draw_top();
            C2D_SceneBegin(bottom);
            gameover_draw_bottom();
            break;

        default:
            C2D_SceneBegin(bottom);
            room_draw();
            C2D_SceneBegin(top);
            hud_draw();
            break;
    }
}
