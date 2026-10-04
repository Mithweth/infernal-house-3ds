// game.h
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>
#include "inventory.h"

typedef struct {
    const char *name;
    void (*callback)(void);
} GameCallbackEntry;

typedef struct {
    bool (*init)(void);
    void (*update)(u32 keys, touchPosition touch);
    void (*draw)(void);
    void (*close)(void);
} MiniGame;

typedef enum {
    GAME_NORMAL,
    GAME_MESSAGE,
    GAME_BUSY,
    GAME_MINIGAME,
    GAME_TITLE,
    GAME_TIMELINE,
    GAME_ENDING
} GameMode;

void game_update(u32 keys, circlePosition analog, touchPosition touch);
void game_draw(C3D_RenderTarget *top, C3D_RenderTarget *bottom);
void game_timeline_start(const char *name);
void game_close(void);
void game_init(void);
void game_intro(void);
void game_start(void);
void game_set_room(const char *name);
const char *game_target_name(void);
bool game_use_item(const char *id);
void game_show_message(const char *message_id);
void game_show_image(C2D_Image image);
void game_minigame_start(const char *name);
void game_minigame_stop(void);
bool game_wait_for_sfx(const char *sfx, void (*callback)(void));
void game_callback_register(const char *name, void (*callback)(void));
void (*game_callback_find(const char *name))(void);
void game_secret_code(void);
const uint8_t *game_get_secret_code(void);
void game_use_syringe(void);
