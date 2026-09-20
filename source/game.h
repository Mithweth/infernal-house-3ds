// game.h
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>
#include "gameover.h"
#include "inventory.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *text_id;
    bool (*is_active)(void);
    void (*action)(void);
    void (*use_item)(ItemId item);
} Hotspot;

typedef struct Room {
    C2D_Image background;
    Hotspot *hotspots;
    size_t hotspot_count;
    void (*north)(void);
    void (*northeast)(void);
    void (*east)(void);
    void (*southeast)(void);
    void (*south)(void);
    void (*southwest)(void);
    void (*west)(void);
    void (*northwest)(void);
    void (*init)(void);
    void (*draw)(void);
    void (*close)(void);
} Room;

typedef enum {
    GAME_NORMAL,
    GAME_MESSAGE,
    GAME_BUSY,
    GAME_OVER,
    GAME_SIMON,
    GAME_PIANO,
    GAME_TITLE,
    GAME_INTRO,
    GAME_ENDING
} GameMode;

void game_update(u32 keys, circlePosition analog, touchPosition touch);
void game_draw(C3D_RenderTarget *top, C3D_RenderTarget *bottom);
void game_over(GameOverId id);
void game_close(void);
void game_init(void);
void game_start(void);
void game_set_room(Room *room);
bool game_can_move_north(void);
bool game_can_move_northeast(void);
bool game_can_move_east(void);
bool game_can_move_southeast(void);
bool game_can_move_south(void);
bool game_can_move_southwest(void);
bool game_can_move_west(void);
bool game_can_move_northwest(void);
void game_start_simon(void);
void game_play_piano(void);
void game_end_simon(bool success);
void game_use_item(ItemId item);
void game_show_message(const char *message_id);
void game_wait_for_sfx(const char *sfx, void (*callback)(void));
