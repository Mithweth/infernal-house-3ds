// game.c
#include <3ds.h>
#include <stdlib.h>
#include "game.h"
#include "lang.h"
#include "inventory.h"
#include "gamestate.h"
#include "room_hall.h"
#include "hud.h"
#include "audio.h"
#include "title.h"
#include "simon.h"
#include "piano.h"
#include "measure.h"
#include "timeline.h"

static Room *current_room = NULL;
static GameMode game_mode = GAME_NORMAL;
static bool circle_ready = true;
static Hotspot *active_hotspot = NULL;
static Hotspot *target = NULL;
static const char *message_text = NULL;
static C2D_TextBuf text_buf;
static C2D_Text text;
static void (*game_busy_callback)(void) = NULL;
static int game_busy_sfx_channel = -1;
static size_t callback_count = 0;
static GameCallbackEntry callbacks[GAME_CALLBACK_MAX];
static uint8_t secret_code[4];


void game_secret_code(void) {
    static C2D_TextBuf secret_code_text_buf;
    char code[5];
    code[0] = '0' + secret_code[0];
    code[1] = '0' + secret_code[1];
    code[2] = '0' + secret_code[2];
    code[3] = '0' + secret_code[3];
    code[4] = '\0';
    if (!secret_code_text_buf) {
        secret_code_text_buf = C2D_TextBufNew(32);
    }
    C2D_TextBufClear(secret_code_text_buf);
    C2D_TextParse(&text, secret_code_text_buf, code);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 40.0f, 100.0f, 0.9f, 0.55f, 0.55f, C2D_Color32(192, 192, 192, 255));
}

static void game_generate_secret_code(void) {
    for (size_t i = 0; i < 4; i++) {
        secret_code[i] = rand() % 10;
    }
}

void game_callback_register(const char *name, void (*callback)(void)) {
    if (callback_count >= GAME_CALLBACK_MAX) {
        return;
    }

    callbacks[callback_count].name = name;
    callbacks[callback_count].callback = callback;
    callback_count++;
}

void (*game_callback_find(const char *name))(void) {
    for (size_t i = 0; i < callback_count; i++) {
        if (strcmp(callbacks[i].name, name) == 0) {
            return callbacks[i].callback;
        }
    }
    return NULL;
}

static bool path_is_available(const Path *path) {
    if (!path || !path->action) {
        return false;
    }
    if (!path->condition) {
        return true;
    }
    return path->condition();
}

static void path_execute(const Path *path) {
    if (path_is_available(path)) {
        path->action();
    }
}

void game_set_room(Room *room) {
    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }

    if (current_room && current_room->close) {
        current_room->close();
    }

    current_room = room;

    active_hotspot = NULL;
    target = NULL;
    game_mode = GAME_NORMAL;
    message_text = NULL;

    if (current_room && current_room->init) {
        current_room->init();
    }
    printf("entering Room: %d hotspots found\n", current_room->hotspot_count);
}

void game_close(void) {
    if (current_room && current_room->close) {
        current_room->close();
    }

    current_room = NULL;

    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    hud_close();
}

const char *game_target_name(void) {
    if (!target) {
        return NULL;
    }
    return target->id;
}

void game_over(const char *timeline) {
    char path[256];
    snprintf(path, sizeof(path), "romfs:/timelines/gameover_%s", timeline);
    if (timeline_init(path)) {
        game_mode = GAME_TIMELINE;
        return;
    }
    game_init();
}

void game_init(void) {
    timeline_close();
    music_stop();
    callback_count = 0;
    game_callback_register("secret_code", game_secret_code);
    hud_init();
    game_mode = GAME_TITLE;
    title_init();
}

void game_start(void) {
    title_close();
    inventory_reset();
    gamestate_reset();
    hud_reset();
    game_generate_secret_code();
    game_mode = GAME_NORMAL;
    message_text = NULL;
    active_hotspot = NULL;
    inventory_add("LIGHTBULB");
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

        if (hotspot->condition && !hotspot->condition()) {
            continue;
        }

        if (x >= hotspot->x && x <  hotspot->x + hotspot->width && y >= hotspot->y && y <  hotspot->y + hotspot->height) {
            return hotspot;
        }
    }

    return NULL;
}

static Hotspot *find_hotspot_by_id(const char *id) {
    for (size_t i = 0; i < current_room->hotspot_count; i++) {
        Hotspot *hotspot = &current_room->hotspots[i];

        if (strcmp(hotspot->id, id) != 0) {
            continue;
        }

        if (hotspot->condition && !hotspot->condition()) {
            continue;
        }

        return hotspot;
    }

    return NULL;
}

bool game_can_move_north(void) {
    return current_room && path_is_available(&current_room->north);
}

bool game_can_move_northeast(void) {
    return current_room && path_is_available(&current_room->northeast);
}

bool game_can_move_east(void) {
    return current_room && path_is_available(&current_room->east);
}

bool game_can_move_southeast(void) {
    return current_room && path_is_available(&current_room->southeast);
}

bool game_can_move_south(void) {
    return current_room && path_is_available(&current_room->south);
}

bool game_can_move_southwest(void) {
    return current_room && path_is_available(&current_room->southwest);
}

bool game_can_move_west(void) {
    return current_room && path_is_available(&current_room->west);
}

bool game_can_move_northwest(void) {
    return current_room && path_is_available(&current_room->northwest);
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
            path_execute(&current_room->north);
            return;
        } else if (y < 0) {
            circle_ready = false;
            path_execute(&current_room->south);
            return;
        }
    } else if (ax > ay * 2) {
        if (x > 0) {
            circle_ready = false;
            path_execute(&current_room->east);
            return;
        } else if (x < 0) {
            circle_ready = false;
            path_execute(&current_room->west);
            return;
        }
    } else {
        if (x > 0 && y > 0) {
            circle_ready = false;
            path_execute(&current_room->northeast);
            return;
        } else if (x < 0 && y > 0) {
            circle_ready = false;
            path_execute(&current_room->northwest);
            return;
        } else if (x > 0 && y < 0) {
            circle_ready = false;
            path_execute(&current_room->southeast);
            return;
        } else if (x < 0 && y < 0) {
            circle_ready = false;
            path_execute(&current_room->southwest);
            return;
        }
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

    printf("current hotspot: %s\n", hotspot->id);
    if (hotspot != active_hotspot) {
        active_hotspot = hotspot;
        target = hotspot;
        
        if (hotspot->message_id) {
            game_show_message(hotspot->message_id);
            return;
        }

        //return;
    }

    if (hotspot->action) {
        hotspot->action();
    }

    if (target && target->condition && !target->condition()) {
        target = find_hotspot_by_id(target->id);
    }

    if (active_hotspot && active_hotspot->condition && !active_hotspot->condition()) {
        active_hotspot = NULL;
    }
}

static void room_draw(void) {
    if (!current_room) {
        return;
    }

    if (current_room->draw) {
        current_room->draw();
    }

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

void game_start_simon(void) {
    simon_init();
    game_mode = GAME_SIMON;
}

void game_start_measure(void) {
    measure_init(151.0f, 197.0f);
    game_mode = GAME_MEASURE;
}

void game_stop_measure(void) {
    measure_close();
    game_mode = GAME_NORMAL;
}

void game_play_piano(void) {
    music_stop();
    piano_init();
    game_mode = GAME_PIANO;
}

void game_stop_piano(bool success) {
    if (success) {
        if (!gamestate_get("livingroom_golden_statue_placed")) {
            return;
        }
        gamestate_set("livingroom_secret_passage_opened");
        game_show_message("LIVINGROOM_SECRET_PASSAGE_OPEN");
    }
    piano_close();
    music_play("romfs:/audio/background.ogg");
    game_mode = GAME_NORMAL;
}

void game_end_simon(bool success) {
    simon_close();
    game_mode = GAME_NORMAL;
    if (success) {
        game_show_message("CELLAR_SIMON_WIN");
    }
}

bool game_use_item(const char *id) {
    if (!target) {
        return false;
    }
    printf("Use item: %s\n", id);
    if (target->use_item && target->use_item(id)) {
        return true;
    }
    game_show_message("GENERIC_USE");
    return false;
}

void game_show_message(const char *message_id) {
    message_text = lang_get(message_id);
    printf("Print: %s\n", message_id);
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

        case GAME_SIMON:
            hud_update();
            simon_update(keys, touch);
            return;

        case GAME_PIANO:
            hud_update();
            piano_update(keys, touch);
            return;

        case GAME_MEASURE:
            hud_update();
            measure_update(keys, touch);
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

    if ((keys & KEY_A) && (game_mode == GAME_MESSAGE)) {
        game_mode = GAME_NORMAL;
        return;
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

        case GAME_TIMELINE:
            C2D_SceneBegin(top);
            timeline_draw_top();
            C2D_SceneBegin(bottom);
            timeline_draw_bottom();
            break;

        case GAME_SIMON:
            C2D_SceneBegin(bottom);
            simon_draw_bottom();
            C2D_SceneBegin(top);
            hud_draw();
            break;

        case GAME_PIANO:
            C2D_SceneBegin(bottom);
            piano_draw_bottom();
            C2D_SceneBegin(top);
            hud_draw();
            break;

        case GAME_MEASURE:
            C2D_SceneBegin(bottom);
            room_draw();
            measure_draw();
            C2D_SceneBegin(top);
            hud_draw();
            break;

        default:
            C2D_SceneBegin(bottom);
            room_draw();
            C2D_SceneBegin(top);
            hud_draw();
            break;
    }
}
