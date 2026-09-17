// game.h
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>
#include "gameover.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *text_id;
    bool (*is_active)(void);
    void (*action)(void);
} Hotspot;

typedef struct Room {
    C2D_Image background;
    Hotspot *hotspots;
    size_t hotspot_count;
    void (*up)(void);
    void (*down)(void);
    void (*left)(void);
    void (*right)(void);
    void (*init)(void);
    void (*draw)(void);
    void (*close)(void);
} Room;

typedef enum {
    GAME_NORMAL,
    GAME_EXAMINE,
    GAME_OVER
} GameMode;

void game_update(u32 keys, circlePosition analog, touchPosition touch);
void game_draw(void);
bool game_is_over(void);
void game_over(GameOverId id);
void game_close(void);
void game_reset(void);
void game_set_room(Room *room);
bool game_can_move_up(void);
bool game_can_move_down(void);
bool game_can_move_left(void);
bool game_can_move_right(void);