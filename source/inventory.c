#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

#include "lang.h"
#include "inventory.h"
#include "inventory_gfx.h"
#include "inventory_gfx_t3x.h"

#define ITEM_SIZE       48.0f
#define ITEM_SPACING    15.0f
#define ITEM_Y          55.0f
#define SELECT_RADIUS   27.0f


static Item items[ITEM_COUNT];
static Item *inventory[ITEM_COUNT];
static size_t inventory_count = 0;
static size_t selected = 0;
static C2D_TextBuf text_buf;
static C2D_Text text;
static C2D_SpriteSheet inventory_scene = NULL;
static InventoryMode inventory_mode = INVENTORY_NORMAL;

static void message_draw_action(Item *item) {
    u32 background = C2D_Color32(8, 12, 30, 255);
    u32 text_color = C2D_Color32(255, 255, 255, 255);

    C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, 400.0f, 240.0f, background);
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get("ITEM_MESSAGE_CONTENT"));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 20.0f, 30.0f, 0.5f, 0.55f, 0.55f, text_color);
}

bool inventory_is_active(void) {
	return inventory_mode == INVENTORY_ACTION;
}

void inventory_init(void) {
    inventory_scene = C2D_SpriteSheetLoadFromMem(inventory_gfx_t3x, inventory_gfx_t3x_size);

    if (!inventory_scene) {
        return;
    }

    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
	}

    items[ITEM_MESSAGE] = (Item) {
        .id = ITEM_MESSAGE,
        .name_id = "ITEM_MESSAGE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, inventory_gfx_message_idx),
        .draw_action = message_draw_action
    };

    items[ITEM_MAGNIFYING_GLASS] = (Item) {
        .id = ITEM_MAGNIFYING_GLASS,
        .name_id = "ITEM_MAGNIFYING_GLASS",
        .image = C2D_SpriteSheetGetImage(inventory_scene, inventory_gfx_magnify_glass_idx),
        .action = NULL
    };

    items[ITEM_SCREWDRIVER] = (Item) {
        .id = ITEM_SCREWDRIVER,
        .name_id = "ITEM_SCREWDRIVER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, inventory_gfx_screwdriver_idx),
        .action = NULL
    };

    items[ITEM_BINOCULARS] = (Item) {
        .id = ITEM_BINOCULARS,
        .name_id = "ITEM_BINOCULARS",
        .image = C2D_SpriteSheetGetImage(inventory_scene, inventory_gfx_binoculars_idx),
        .action = NULL
    };

    items[ITEM_FLASHLIGHT] = (Item) {
        .id = ITEM_FLASHLIGHT,
        .name_id = "ITEM_FLASHLIGHT",
        .image = C2D_SpriteSheetGetImage(inventory_scene, inventory_gfx_flashlight_idx),
        .action = NULL
    };

    inventory_count = 0;
    selected = 0;
}

void inventory_close(void) {
    if (inventory_scene) {
        C2D_SpriteSheetFree(inventory_scene);
        inventory_scene = NULL;
    }
	if (text_buf) {
    	C2D_TextBufDelete(text_buf);
    	text_buf = NULL;
	}

    inventory_count = 0;
    selected = 0;
}


bool inventory_has(ItemId id) {
    for (size_t i = 0; i < inventory_count; i++) {
        if (inventory[i]->id == id)
            return true;
    }
    return false;
}

void inventory_add(ItemId id) {
    if (id < 0 || id >= ITEM_COUNT)
        return;

    if (inventory_count >= ITEM_COUNT)
        return;

    if (inventory_has(id))
        return;

    inventory[inventory_count++] = &items[id];
    selected = inventory_count - 1;
}


bool inventory_update(u32 keys) {
    if (inventory_mode == INVENTORY_ACTION) {
        if (keys & KEY_A) {
            inventory_mode = INVENTORY_NORMAL;
        }
        return true;
    }

    if (inventory_count == 0) {
        return false;
    }

    if (keys & KEY_LEFT) {
        if (selected == 0) {
            selected = inventory_count - 1;
        } else {
            selected--;
        }
        return true;
    }

    if (keys & KEY_RIGHT) {
        selected++;
        if (selected >= inventory_count) {
            selected = 0;
        }
        return true;
    }

    if (keys & KEY_A) {
        Item *item = inventory[selected];
        if (item->action) {
            item->action(item);
        }
        if (item->draw_action) {
            inventory_mode = INVENTORY_ACTION;
        }
        return true;
    }
    return false;
}

void inventory_draw(void) {
    if (inventory_count == 0) {
        return;
    }

    if (inventory_mode == INVENTORY_ACTION) {
        Item *item = inventory[selected];

        if (item->draw_action) {
            item->draw_action(item);
        }
        return;
    }

    float total_width = inventory_count * ITEM_SIZE + (inventory_count - 1) * ITEM_SPACING;
    float start_x = (400.0f - total_width) / 2.0f;

    for (size_t i = 0; i < inventory_count; i++) {
        float x = start_x + i * (ITEM_SIZE + ITEM_SPACING);
        if (i == selected) {
            C2D_DrawCircleSolid(x + ITEM_SIZE / 2.0f, ITEM_Y + ITEM_SIZE / 2.0f, 0.3f, SELECT_RADIUS, C2D_Color32(40, 65, 100, 255));
        }
        C2D_DrawImageAt(inventory[i]->image, x, ITEM_Y, 0.4f, NULL, 1.0f, 1.0f);
    }

	Item *item = inventory[selected];

	C2D_TextBufClear(text_buf);
	C2D_TextParse(&text, text_buf, lang_get(item->name_id));
	C2D_TextOptimize(&text);
	C2D_DrawRectSolid(100.0f, 120.0f, 0.4f, 200.0f, 20.0f, C2D_Color32(0, 0, 0, 180));
	C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, 200.0f, 120.0f, 0.5f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
}
