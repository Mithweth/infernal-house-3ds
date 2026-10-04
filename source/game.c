// game.c
// Game controller: the GameMode state machine, room input (circle pad
// movement, touch on hotspots, inventory keys), the message box, and the
// glue used by room actions and extensions to change mode. See game.h for the
// public API.
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "lang.h"
#include "inventory.h"
#include "gamestate.h"
#include "room.h"
#include "hud.h"
#include "audio.h"
#include "title.h"
#include "timeline.h"
#include "callbacks.h"

# define GAME_CALLBACK_MAX 8

static GameMode game_mode = GAME_NORMAL;
// Circle pad must go back to the dead zone before the next move is accepted,
// so holding the stick only moves one room.
static bool circle_ready = true;
// Last hotspot touched; a second touch on it runs its actions (see update_touch).
static Hotspot *active_hotspot = NULL;
// Hotspot shown in the HUD and used as the target of inventory items.
// Both pointers point into the current room and are reset by game_set_room.
static Hotspot *target = NULL;
// Message shown in GAME_MESSAGE; when examine_image is set, it is shown
// instead of the text.
static const char *message_text = NULL;
static C2D_Image examine_image;
static C2D_TextBuf text_buf;
static C2D_Text text;
// Called once the GAME_BUSY sound effect has finished playing.
static void (*game_busy_callback)(void) = NULL;
static MiniGame *active_minigame = NULL;
static int game_busy_sfx_channel = -1;

void game_minigame_start(const char *name) {
    active_minigame = callbacks_minigame_find(name);
    if (!active_minigame) {
        printf("Unknown mini-game: %s\n", name);
        return;
    }
    music_stop();
    if (active_minigame->init) {
        if (!active_minigame->init()) {
            printf("Fail loading mini-game\n");
            active_minigame = NULL;
            return;
        }
    }
    game_mode = GAME_MINIGAME;
}

void game_minigame_stop(void) {
    if (active_minigame && active_minigame->close) {
        active_minigame->close();
    }
    music_play("romfs:/audio/background.ogg");
    active_minigame = NULL;
    game_mode = GAME_NORMAL;
}

void game_set_room(const char *name) {
    // name usually comes from a ROOM action owned by the current room, which
    // room_close is about to free: copy it first.
    char room_name[64];
    snprintf(room_name, sizeof(room_name), "%s", name);
    room_close();
    active_hotspot = NULL;
    target = NULL;
    game_mode = GAME_NORMAL;
    message_text = NULL;
    if (!room_init(room_name)) {
        printf("Cannot enter room: %s\n", room_name);
    }
}

void game_close(void) {
    room_close();
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    hud_close();
}

// An action may have hidden the target since it was selected (its WHEN
// conditions no longer match). Fall back to an available hotspot with the same
// id, e.g. an open door replacing a closed one, or to no target at all.
// Called lazily wherever target is read, so every path is covered.
static void refresh_target(void) {
    if (target && !room_hotspot_is_available(target)) {
        target = room_find_hotspot_by_id(target->id);
    }
}

const char *game_target_name(void) {
    refresh_target();
    if (!target) {
        return NULL;
    }
    return target->id;
}

void game_timeline_start(const char *name) {
    char path[256];
    snprintf(path, sizeof(path), "romfs:/timelines/%s", name);
    if (timeline_init(path)) {
        game_mode = GAME_TIMELINE;
        return;
    }
    game_init();
}

void game_init(void) {
    timeline_close();
    music_stop();
    callbacks_init();
    game_mode = GAME_TITLE;
    examine_image = (C2D_Image){0};
    title_init();
}

void game_start(void) {
    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }
    title_close();
    inventory_reset();
    gamestate_reset();
    hud_reset();
    game_mode = GAME_NORMAL;
    message_text = NULL;
    active_hotspot = NULL;
    music_play("romfs:/audio/background.ogg");
    inventory_add("MEASURING_TAPE");
    game_set_room("hall");
}

bool game_wait_for_sfx(const char *sfx, void (*callback)(void)) {
    game_busy_sfx_channel = sfx_play(sfx);
    if (game_busy_sfx_channel < 0) {
        return false;
    }
    game_busy_callback = callback;
    game_mode = GAME_BUSY;
    return true;
}

void game_intro(void) {
    title_close();
    if (!timeline_init("romfs:/timelines/intro")) {
        game_mode = GAME_TITLE;
        title_init();
        return;
    }
    game_mode = GAME_TIMELINE;
}

// Turns the circle pad position into one of eight directions. A direction is
// a cardinal one when one axis is more than twice the other, otherwise a
// diagonal. Only one move is made per push of the stick.
static void update_movement(circlePosition analog) {
    const int DEADZONE = 60;

    int x = analog.dx;
    int y = analog.dy;

    if (abs(x) < DEADZONE && abs(y) < DEADZONE) {
        circle_ready = true;
        return;
    }

    if (!circle_ready) {
        return;
    }

    int ax = abs(x);
    int ay = abs(y);

    if (ay > ax * 2) {
        if (y > 0) {
            circle_ready = false;
            room_move_north();
            return;
        } else if (y < 0) {
            circle_ready = false;
            room_move_south();
            return;
        }
    } else if (ax > ay * 2) {
        if (x > 0) {
            circle_ready = false;
            room_move_east();
            return;
        } else if (x < 0) {
            circle_ready = false;
            room_move_west();
            return;
        }
    } else {
        if (x > 0 && y > 0) {
            circle_ready = false;
            room_move_northeast();
            return;
        } else if (x < 0 && y > 0) {
            circle_ready = false;
            room_move_northwest();
            return;
        } else if (x > 0 && y < 0) {
            circle_ready = false;
            room_move_southeast();
            return;
        } else if (x < 0 && y < 0) {
            circle_ready = false;
            room_move_southwest();
            return;
        }
    }
}

// Handles a tap on the room. The first tap on a hotspot selects it as target
// and, if it has a MESSAGE, only shows that message; tapping the same hotspot
// again runs its ACTION blocks. Hotspots without a MESSAGE run their actions
// on the first tap.
static void update_touch(touchPosition touch) {
    if (active_hotspot && !room_hotspot_is_available(active_hotspot)) {
        active_hotspot = NULL;
    }

    Hotspot *hotspot = room_find_hotspot(touch.px, touch.py);
    if (!hotspot) {
        return;
    }

    printf("current hotspot: %s\n", hotspot->id);

    if (hotspot != active_hotspot) {
        active_hotspot = hotspot;
        target = hotspot;

        if (hotspot->message_id) {
            game_show_message(hotspot->message_id);
            return;
        }
    }

    room_execute_hotspot_action(hotspot);
}


// Draws the room on the bottom screen, plus the examine image or the message
// box when in GAME_MESSAGE. The text is parsed again on every frame.
static void game_draw_room(void) {
    room_draw();
    if (game_mode == GAME_MESSAGE) {
        if (examine_image.tex) {
            float x = (320 - examine_image.subtex->width) / 2;
            float y = (240 - examine_image.subtex->height) / 2;
            C2D_DrawRectSolid(0.0f, 0.0f, 0.8f, 320, 240, C2D_Color32(0, 0, 0, 180));
            C2D_DrawImageAt(examine_image, x, y, 0.9f, NULL, 1.0f, 1.0f);
        } else {
            C2D_TextBufClear(text_buf);
            C2D_TextParse(&text, text_buf, message_text);
            C2D_TextOptimize(&text);
            float width, height;
            C2D_TextGetDimensions(&text, 0.5f, 0.5f, &width, &height);
            float box_height = height + 20.0f;
            float box_y = 240.0f - box_height - 10.0f;
            C2D_DrawRectSolid(13.0f, box_y + 3.0f, 0.79f, 300.0f, box_height, C2D_Color32(0, 0, 0, 150));
            C2D_DrawRectSolid(10.0f, box_y, 0.80f, 300.0f, box_height, C2D_Color32(150, 120, 55, 255));
            C2D_DrawRectSolid(12.0f, box_y + 2.0f, 0.81f, 296.0f, box_height - 4.0f, C2D_Color32(18, 24, 34, 235));
            C2D_DrawRectSolid(15.0f, box_y + 5.0f, 0.82f, 290.0f, box_height - 10.0f, C2D_Color32(105, 82, 40, 255));
            C2D_DrawRectSolid(16.0f, box_y + 6.0f, 0.83f, 288.0f, box_height - 12.0f, C2D_Color32(22, 28, 40, 245));
            C2D_DrawText(&text, C2D_WithColor, 22.0f, box_y + 10.0f, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
        }
    }
}

bool game_use_item(const char *id) {
    refresh_target();
    if (!target) {
        return false;
    }
    
    printf("Use item: %s\n", id);

    if (room_execute_hotspot_use(target, id)) {
        return true;
    }

    game_show_message("GAME_CANNOT_USE_MESSAGE");
    return false;
}

void game_show_message(const char *message_id) {
    message_text = lang_get(message_id);
    printf("Print: %s\n", message_id);
    game_mode = GAME_MESSAGE;
}

void game_show_image(C2D_Image image) {
    examine_image = image;
    game_mode = GAME_MESSAGE;
}

void game_update(u32 keys, circlePosition analog, touchPosition touch) {
    switch (game_mode) {
        case GAME_TITLE:
            title_update(keys);
            return;

        case GAME_TIMELINE:
            timeline_update(keys);
            return;

        case GAME_MINIGAME:
            hud_update();
            if (active_minigame && active_minigame->update) {
                active_minigame->update(keys, touch);
            }
            return;

        case GAME_MESSAGE:
            hud_update();
            if (keys & (KEY_A | KEY_B | KEY_TOUCH)) {
                game_mode = GAME_NORMAL;
                examine_image = (C2D_Image){0};
            }
            return;

        case GAME_BUSY:
            hud_update();
            // The callback is cleared before being called because it may
            // start a new wait (e.g. the next WAIT_SFX of a room action list).
            if (!sfx_is_playing(game_busy_sfx_channel)) {
                game_mode = GAME_NORMAL;
                if (game_busy_callback) {
                    void (*callback)(void) = game_busy_callback;
                    game_busy_callback = NULL;
                    game_busy_sfx_channel = -1;
                    callback();
                }
            }
            return;

        default:
            hud_update();
            break;
    }

    // GAME_NORMAL: the inventory gets the keys first (D-pad, A, X); when it
    // consumes them, no movement or touch is handled this frame.
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

        case GAME_TIMELINE:
            C2D_SceneBegin(top);
            timeline_draw_top();
            C2D_SceneBegin(bottom);
            timeline_draw_bottom();
            break;

        case GAME_MINIGAME:
            // The mini-game draws over the room, with a higher depth.
            C2D_SceneBegin(bottom);
            room_draw();
            if (active_minigame && active_minigame->draw) {
                active_minigame->draw();
            }
            C2D_SceneBegin(top);
            hud_draw();
            break;

        default:
            C2D_SceneBegin(bottom);
            game_draw_room();
            C2D_SceneBegin(top);
            hud_draw();
            break;
    }
}
