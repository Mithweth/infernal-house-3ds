// game.c
// Game controller: the GameMode state machine, room input (circle pad
// movement, touch on hotspots, inventory keys), the message box, and the
// glue used by room actions and extensions to change mode. See game.h for the
// public API.
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "str_utils.h"

#define GAME_CALLBACK_MAX     8
#define GAME_CONFIG_MAX_ITEMS 16
#define QUIT_BOX_WIDTH      260.0f
#define QUIT_BOX_HEIGHT     110.0f
#define QUIT_TITLE_OFFSET    25.0f  // from the top of the box
#define QUIT_BUTTON_OFFSET   65.0f  // from the top of the box
#define QUIT_BUTTON_SPACING  40.0f  // from the center to each button
#define QUIT_BUTTON_PADDING   5.0f
#define QUIT_TEXT_SCALE       0.6f

typedef struct {
    char *open;
    char *music;
    char *items[GAME_CONFIG_MAX_ITEMS];
    size_t item_count;
} GameConfig;

static GameConfig game_config;
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
static bool quit_confirm;
static bool quit_selected;


static bool game_config_load(const char *filename) {
    FILE *file = fopen(filename, "r");
    char line[512];

    if (!file) {
        return false;
    }

    while (fgets(line, sizeof(line), file)) {
        char *key = str_trim(line);

        if (!*key || *key == '#' || *key == ';' || *key == '[') {
            continue;
        }

        char *value = strchr(key, '=');
        if (!value) {
            continue;
        }

        *value++ = '\0';
        key = str_trim(key);
        value = str_trim(value);

        if (strcmp(key, "open") == 0) {
            if (game_config.open) {
                printf("Duplicate key: open\n");
                fclose(file);
                return false;
            }
            game_config.open = strdup(value);
        } else if (strcmp(key, "music") == 0) {
            if (game_config.music) {
                printf("Duplicate key: music\n");
                fclose(file);
                return false;
            }
            game_config.music = strdup(value);
        } else if (strcmp(key, "items") == 0) {
            if (game_config.item_count) {
                printf("Duplicate key: items\n");
                fclose(file);
                return false;
            }
            char *item = strtok(value, ",");
            while (item && game_config.item_count < GAME_CONFIG_MAX_ITEMS) {
                item = str_trim(item);
                if (*item) {
                    game_config.items[game_config.item_count++] = strdup(item);
                }
                item = strtok(NULL, ",");
            }
        }
    }
    fclose(file);
    if (!game_config.open) {
        printf("Missing open in game configuration\n");
        return false;
    }
    return true;
}

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
            game_minigame_stop();
            return;
        }
    }
    game_mode = GAME_MINIGAME;
}

void game_minigame_stop(void) {
    if (active_minigame && active_minigame->close) {
        active_minigame->close();
    }
    if (game_config.music) {
        music_play(game_config.music);
    }
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
    free(game_config.open);
    free(game_config.music);
    for (size_t i = 0; i < game_config.item_count; i++) {
        free(game_config.items[i]);
    }
    memset(&game_config, 0, sizeof(game_config));
    hud_close();
    inventory_close();
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

bool game_title_start(void) {
    if (active_minigame && active_minigame->close) {
        active_minigame->close();
    }
    active_minigame = NULL;
    room_close();
    active_hotspot = NULL;
    target = NULL;
    message_text = NULL;
    game_busy_callback = NULL;
    title_close();
    timeline_close();
    music_stop();
    game_mode = GAME_TITLE;
    if (!title_init()) {
        printf("Cannot initialize title screen\n");
        return false;
    }
    return true;
}

bool game_timeline_start(const char *name) {
    char path[256];
    snprintf(path, sizeof(path), "romfs:/timelines/%s", name);
    if (timeline_init(path)) {
        game_mode = GAME_TIMELINE;
        return true;
    }
    game_title_start();
    return false;
}

bool game_init(void) {
    callbacks_init();
    if (!game_config_load("romfs:/game/autorun.inf")) {
        printf("Cannot load game configuration\n");
        return false;
    }
    if (!inventory_init()) {
        printf("Cannot initialize inventory\n");
        return false;
    }
    if (!hud_init()) {
        printf("Cannot initialize HUD\n");
        return false;
    }
    // Created here rather than in game_start: the quit confirmation also
    // draws its text on the title screen, before any game has started.
    text_buf = C2D_TextBufNew(4096);
    if (!text_buf) {
        printf("Cannot create game text buffer\n");
        return false;
    }
    return game_title_start();
}

void game_start(void) {
    title_close();
    callbacks_reset();
    inventory_reset();
    gamestate_reset();
    hud_reset();
    game_mode = GAME_NORMAL;
    message_text = NULL;
    active_hotspot = NULL;
    if (game_config.music) {
        music_play(game_config.music);
    }

    for (size_t i = 0; i < game_config.item_count; i++) {
        inventory_add(game_config.items[i]);
    }

    game_set_room(game_config.open);
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
        game_title_start();
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


// Draws the golden-framed dark box used by the message box and the quit
// confirmation, with a drop shadow. Uses depths z - 0.01 to z + 0.03.
static void draw_framed_box(float x, float y, float w, float h, float z) {
    C2D_DrawRectSolid(x + 3.0f, y + 3.0f, z - 0.01f, w, h, C2D_Color32(0, 0, 0, 150));
    C2D_DrawRectSolid(x, y, z, w, h, C2D_Color32(150, 120, 55, 255));
    C2D_DrawRectSolid(x + 2.0f, y + 2.0f, z + 0.01f, w - 4.0f, h - 4.0f, C2D_Color32(18, 24, 34, 235));
    C2D_DrawRectSolid(x + 5.0f, y + 5.0f, z + 0.02f, w - 10.0f, h - 10.0f, C2D_Color32(105, 82, 40, 255));
    C2D_DrawRectSolid(x + 6.0f, y + 6.0f, z + 0.03f, w - 12.0f, h - 12.0f, C2D_Color32(22, 28, 40, 245));
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
            draw_framed_box(10.0f, box_y, 300.0f, box_height, 0.80f);
            C2D_DrawText(&text, C2D_WithColor, 22.0f, box_y + 10.0f, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
        }
    }
}

// Draws a centered label at (x, y), highlighted when selected. Appends to
// text_buf without clearing it: the caller clears it first.
static void draw_button(const char *label_id, float x, float y, bool selected) {
    float width, height;
    C2D_TextParse(&text, text_buf, lang_get(label_id));
    C2D_TextOptimize(&text);
    C2D_TextGetDimensions(&text, QUIT_TEXT_SCALE, QUIT_TEXT_SCALE, &width, &height);
    if (selected) {
        C2D_DrawRectSolid(x - width / 2.0f - QUIT_BUTTON_PADDING, y - QUIT_BUTTON_PADDING, 0.95f, width + 2.0f * QUIT_BUTTON_PADDING, height + 2.0f * QUIT_BUTTON_PADDING, C2D_Color32(105, 82, 40, 255));
    }
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, x, y, 0.96f, QUIT_TEXT_SCALE, QUIT_TEXT_SCALE, C2D_Color32(255, 255, 255, 255));
}

// Draws the quit confirmation centered on the bottom screen: a dimmed
// background, the question, and the No (left) / Yes (right) buttons.
static void quit_confirm_draw(void) {
    float box_x = (320.0f - QUIT_BOX_WIDTH) / 2.0f;
    float box_y = (240.0f - QUIT_BOX_HEIGHT) / 2.0f;
    float center_x = box_x + QUIT_BOX_WIDTH / 2.0f;
    float button_y = box_y + QUIT_BUTTON_OFFSET;

    C2D_DrawRectSolid(0.0f, 0.0f, 0.85f, 320.0f, 240.0f, C2D_Color32(0, 0, 0, 128));
    draw_framed_box(box_x, box_y, QUIT_BOX_WIDTH, QUIT_BOX_HEIGHT, 0.90f);

    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get("GAME_QUIT_CONFIRM"));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, center_x, box_y + QUIT_TITLE_OFFSET, 0.96f, QUIT_TEXT_SCALE, QUIT_TEXT_SCALE, C2D_Color32(255, 255, 255, 255));
    draw_button("GAME_QUIT_NO", center_x - QUIT_BUTTON_SPACING, button_y, !quit_selected);
    draw_button("GAME_QUIT_YES", center_x + QUIT_BUTTON_SPACING, button_y, quit_selected);
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

bool game_update(u32 keys, circlePosition analog, touchPosition touch) {
    if (quit_confirm) {
        if (keys & KEY_LEFT) {
            quit_selected = false;
        }
        if (keys & KEY_RIGHT) {
            quit_selected = true;
        }
        if (keys & (KEY_B | KEY_START)) {
            quit_confirm = false;
            timer_resume();
            return true;
        }
        if (keys & KEY_A) {
            if (quit_selected) {
                return false;
            }
            quit_confirm = false;
            timer_resume();
        }

        return true;
    }

    if (keys & KEY_START) {
        quit_confirm = true;
        quit_selected = false;
        return true;
    }

    switch (game_mode) {
        case GAME_TITLE:
            title_update(keys, touch);
            return true;

        case GAME_TIMELINE:
            timeline_update(keys);
            return true;

        case GAME_MINIGAME:
            hud_update();
            if (active_minigame && active_minigame->update) {
                active_minigame->update(keys, touch);
            }
            return true;

        case GAME_MESSAGE:
            hud_update();
            if (keys & (KEY_A | KEY_B | KEY_TOUCH)) {
                game_mode = GAME_NORMAL;
                examine_image = (C2D_Image){0};
            }
            return true;

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
            return true;

        default:
            hud_update();
            break;
    }

    // GAME_NORMAL: the inventory gets the keys first (D-pad, A, X); when it
    // consumes them, no movement or touch is handled this frame.
    if ((inventory_is_active()) | (inventory_update(keys))) {
        return true;
    }

    update_movement(analog);

    if (keys & KEY_TOUCH) {
        update_touch(touch);
    }
    return true;
}

void game_draw(C3D_RenderTarget *top, C3D_RenderTarget *bottom) {
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
    switch (game_mode) {
        case GAME_TITLE:
            C2D_SceneBegin(bottom);
            title_draw_bottom();
            C2D_SceneBegin(top);
            title_draw_top();
            break;

        case GAME_TIMELINE:
            C2D_SceneBegin(bottom);
            timeline_draw_bottom();
            C2D_SceneBegin(top);
            timeline_draw_top();
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
    if (quit_confirm) {
        C2D_SceneBegin(bottom);
        quit_confirm_draw();
    }
}
