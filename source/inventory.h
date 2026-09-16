#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

typedef enum {
    ITEM_MESSAGE,
    ITEM_MAGNIFYING_GLASS,
    ITEM_SCREWDRIVER,
    ITEM_BINOCULARS,
    ITEM_FLASHLIGHT,
    ITEM_COUNT
} ItemId;

typedef struct Item {
    ItemId id;
    const char *name_id;
    C2D_Image image;
    void (*action)(struct Item *item);
} Item;

void inventory_init(void);
void inventory_close(void);

void inventory_add(ItemId item);
bool inventory_has(ItemId item);

void inventory_update(u32 keys);
void inventory_draw(void);
