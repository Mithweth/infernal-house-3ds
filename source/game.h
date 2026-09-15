// game.h
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *text_id;
    bool (*is_active)(void);
    void (*action)(void);
} Hotspot;

typedef struct {
    C2D_Image background;
    Hotspot *hotspots;
    size_t hotspot_count;
    void (*init)(void);
    void (*draw)(void);
    void (*close)(void);
} Room;

typedef enum {
    GAME_NORMAL,
    GAME_EXAMINE
} GameMode;

void game_update(u32 keys);
void game_draw(void);
void game_close(void);
void game_set_room(Room *room);