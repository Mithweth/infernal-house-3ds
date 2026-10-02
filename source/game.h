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
    int x;
    int y;
    int width;
    int height;
    const char *id;
    const char *message_id;
    bool (*condition)(void);
    void (*action)(void);
    bool (*use_item)(const char *id);
} Hotspot;

typedef struct {
    void (*action)(void);
    bool (*condition)(void);
} Path;

typedef struct Room {
    C2D_Image background;
    Hotspot *hotspots;
    size_t hotspot_count;
    Path north;
    Path northeast;
    Path east;
    Path southeast;
    Path south;
    Path southwest;
    Path west;
    Path northwest;
    void (*init)(void);
    void (*draw)(void);
    void (*close)(void);
} Room;

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
void game_over(const char *timeline);
void game_close(void);
void game_init(void);
void game_intro(void);
void game_start(void);
void game_set_room(Room *room);
const char *game_target_name(void);
bool game_can_move_north(void);
bool game_can_move_northeast(void);
bool game_can_move_east(void);
bool game_can_move_southeast(void);
bool game_can_move_south(void);
bool game_can_move_southwest(void);
bool game_can_move_west(void);
bool game_can_move_northwest(void);
bool game_use_item(const char *id);
void game_show_message(const char *message_id);
void game_show_image(C2D_Image image);
void game_ending(void);
void game_minigame_start(const char *name);
void game_minigame_stop(void);
void game_wait_for_sfx(const char *sfx, void (*callback)(void));
void game_callback_register(const char *name, void (*callback)(void));
void (*game_callback_find(const char *name))(void);
void game_secret_code(void);
const uint8_t *game_get_secret_code(void);
void game_use_syringe(void);
