// inventory.h
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

typedef struct {
    char *id;
    char *name_id;
    char *examine_text;
    C2D_Image image;
    C2D_Image detail_image;
    bool detail_fullscreen;
    float detail_x;
    float detail_y;
    void (*examine_callback)(void);
    void (*use_callback)(void);
} Item;

typedef enum {
    INVENTORY_NORMAL,
    INVENTORY_ACTION
} InventoryMode;

bool inventory_init(void);
void inventory_close(void);
void inventory_add(const char *id);
bool inventory_has(const char *id);
void inventory_remove(const char *id);
bool inventory_update(u32 keys);
void inventory_draw(void);
bool inventory_is_active(void);
void inventory_reset(void);
const Item *inventory_get_selected(void);
