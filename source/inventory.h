// inventory.h
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
    ITEM_SCORE,
    ITEM_LIGHTER,
    ITEM_STATUE,
    ITEM_CUP,
    ITEM_CLOCK,
    ITEM_GLASSES,
    ITEM_INVOICE,
    ITEM_STAIN_REMOVER,
    ITEM_REVOLVER,
    ITEM_SHOVEL,
    ITEM_SLEDGEHAMMER,
    ITEM_RING,
    ITEM_ROPE,
    ITEM_RUBBER,
    ITEM_LIGHTBULB,
    ITEM_MEASURING_TAPE,
    ITEM_CIGARETTES,
    ITEM_BRACELET,
    ITEM_KEY_ONE,
    ITEM_COUNT
} ItemId;

typedef struct Item {
    ItemId id;
    const char *name_id;
    C2D_Image image;
    bool examinable;
    void (*draw_action)(struct Item *item);
} Item;

typedef enum {
    INVENTORY_NORMAL,
    INVENTORY_ACTION
} InventoryMode;

void inventory_init(void);
void inventory_close(void);
void inventory_add(ItemId id);
bool inventory_has(ItemId id);
void inventory_remove(ItemId id);
bool inventory_update(u32 keys);
void inventory_draw(void);
bool inventory_is_active(void);
void inventory_reset(void);
const Item *inventory_get_selected(void);
