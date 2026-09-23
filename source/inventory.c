// inventory.c
#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

#include "lang.h"
#include "inventory.h"
#include "game.h"
#include "gfx_inventory.h"
#include "gfx_inventory_t3x.h"

#define INVENTORY_COLUMNS  6
#define INVENTORY_ROWS     2
#define ITEM_SIZE       48.0f
#define ITEM_SPACING    10.0f
#define ITEM_Y          30.0f
#define ITEM_X          20.0f
#define SELECT_RADIUS   27.0f


static Item items[ITEM_COUNT];
static Item *inventory[ITEM_COUNT];
static size_t inventory_count = 0;
static size_t selected = 0;
static C2D_TextBuf text_buf;
static C2D_Text text;
static C2D_SpriteSheet inventory_scene = NULL;
static InventoryMode inventory_mode = INVENTORY_NORMAL;
static C2D_Image object_details;
static C2D_Image arrow;

static void description_draw_action(Item *item) {
    char description_id[128];
    snprintf(description_id, sizeof(description_id), "%s_EXAMINE", item->name_id);

    u32 background = C2D_Color32(8, 12, 30, 255);
    u32 text_color = C2D_Color32(255, 255, 255, 255);

    C2D_DrawRectSolid(0.0f, 0.0f, 0.0f,400.0f, 240.0f,background);

    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get(description_id));
    C2D_TextOptimize(&text);

    C2D_DrawText(&text, C2D_WithColor, 20.0f, 30.0f, 0.5f, 0.55f, 0.55f, text_color);
}

static void score_draw_action(Item *item) {
    object_details = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_partition_details_idx);
    C2D_DrawImageAt(object_details, 50, 20, 0.5f, NULL, 1.0f, 1.0f);
}

static void invoice_draw_action(Item *item) {
    object_details = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_invoice_details_idx);
    C2D_DrawImageAt(object_details, 10, 10, 0.5f, NULL, 1.0f, 1.0f);
}

bool inventory_is_active(void) {
    return inventory_mode == INVENTORY_ACTION;
}

void inventory_init(void) {
    inventory_scene = C2D_SpriteSheetLoadFromMem(gfx_inventory_t3x, gfx_inventory_t3x_size);
    arrow = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_arrow_idx);

    if (!inventory_scene) {
        return;
    }

    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }

    items[ITEM_MESSAGE] = (Item) {
        .id = ITEM_MESSAGE,
        .name_id = "ITEM_MESSAGE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_message_idx),
        .examinable = true
    };

    items[ITEM_MAGNIFYING_GLASS] = (Item) {
        .id = ITEM_MAGNIFYING_GLASS,
        .name_id = "ITEM_MAGNIFYING_GLASS",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_magnify_glass_idx),
    };

    items[ITEM_SCREWDRIVER] = (Item) {
        .id = ITEM_SCREWDRIVER,
        .name_id = "ITEM_SCREWDRIVER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_screwdriver_idx),
    };

    items[ITEM_BINOCULARS] = (Item) {
        .id = ITEM_BINOCULARS,
        .name_id = "ITEM_BINOCULARS",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_binoculars_idx),
    };

    items[ITEM_FLASHLIGHT] = (Item) {
        .id = ITEM_FLASHLIGHT,
        .name_id = "ITEM_FLASHLIGHT",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_flashlight_idx),
    };

    items[ITEM_SCORE] = (Item) {
        .id = ITEM_SCORE,
        .name_id = "ITEM_SCORE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_partition_idx),
        .draw_action = score_draw_action
    };

    items[ITEM_LIGHTER] = (Item) {
        .id = ITEM_LIGHTER,
        .name_id = "ITEM_LIGHTER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_lighter_idx),
    };

    items[ITEM_STATUE] = (Item) {
        .id = ITEM_STATUE,
        .name_id = "ITEM_STATUE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_statue_idx),
        .examinable = true
    };

    items[ITEM_CUP] = (Item) {
        .id = ITEM_CUP,
        .name_id = "ITEM_CUP",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_cup_idx),
        .examinable = true
    };
    items[ITEM_CLOCK] = (Item) {
        .id = ITEM_CLOCK,
        .name_id = "ITEM_CLOCK",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_clock_idx),
    };
    items[ITEM_GLASSES] = (Item) {
        .id = ITEM_GLASSES,
        .name_id = "ITEM_GLASSES",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_glasses_idx),
    };
    items[ITEM_STAIN_REMOVER] = (Item) {
        .id = ITEM_STAIN_REMOVER,
        .name_id = "ITEM_STAIN_REMOVER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_stain_remover_idx),
    };

    items[ITEM_REVOLVER] = (Item) {
        .id = ITEM_REVOLVER,
        .name_id = "ITEM_REVOLVER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_revolver_idx),
    };

    items[ITEM_INVOICE] = (Item) {
        .id = ITEM_INVOICE,
        .name_id = "ITEM_INVOICE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_invoice_idx),
        .draw_action = invoice_draw_action
    };

    items[ITEM_SHOVEL] = (Item) {
        .id = ITEM_SHOVEL,
        .name_id = "ITEM_SHOVEL",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_shovel_idx),
    };

    items[ITEM_SLEDGEHAMMER] = (Item) {
        .id = ITEM_SLEDGEHAMMER,
        .name_id = "ITEM_SLEDGEHAMMER",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_sledgehammer_idx),
    };

    items[ITEM_ROPE] = (Item) {
        .id = ITEM_ROPE,
        .name_id = "ITEM_ROPE",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_rope_idx),
    };

    items[ITEM_RING] = (Item) {
        .id = ITEM_RING,
        .name_id = "ITEM_RING",
        .image = C2D_SpriteSheetGetImage(inventory_scene, gfx_inventory_ring_idx),
        .examinable = true
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

void inventory_reset(void) {
    inventory_count = 0;
    selected = 0;
    inventory_mode = INVENTORY_NORMAL;
}


bool inventory_has(ItemId id) {
    for (size_t i = 0; i < inventory_count; i++) {
        if (inventory[i]->id == id)
            return true;
    }
    return false;
}

void inventory_add(ItemId id) {
    if (id < 0 || id >= ITEM_COUNT) {
        return;
    }

    if (inventory_count >= ITEM_COUNT) {
        return;
    }

    if (inventory_has(id)) {
        return;
    }

    inventory[inventory_count++] = &items[id];
    selected = inventory_count - 1;
}

void inventory_remove(ItemId id) {
    for (size_t i = 0; i < inventory_count; i++) {
        if (inventory[i]->id != id) {
            continue;
        }

        for (size_t j = i; j < inventory_count - 1; j++) {
            inventory[j] = inventory[j + 1];
        }

        inventory_count--;

        if (inventory_count == 0) {
            selected = 0;
        } else if (selected >= inventory_count) {
            selected = inventory_count - 1;
        }

        return;
    }
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

    if (keys & KEY_DLEFT) {
        if (selected == 0) {
            selected = inventory_count - 1;
        } else {
            selected--;
        }
        return true;
    }

    if (keys & KEY_DRIGHT) {
        selected++;
        if (selected >= inventory_count) {
            selected = 0;
        }
        return true;
    }


    if (keys & KEY_DDOWN) {
        selected += INVENTORY_COLUMNS;

        if (selected >= inventory_count) {
            selected = inventory_count - 1;
        }
        return true;
    }

    if (keys & KEY_DUP) {
        if (selected < INVENTORY_COLUMNS) {
            selected = 0;
        } else {
            selected -= INVENTORY_COLUMNS;
        }
        return true;
    }

    if (keys & KEY_A) {
        Item *item = inventory[selected];
        if (!game_use_item(item->id)) {
            if (item->draw_action || item->examinable) {
                inventory_mode = INVENTORY_ACTION;
            }
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
        } else {
            description_draw_action(item);
        }

        return;
    }

    int selected_row = selected / INVENTORY_COLUMNS;
    int first_row = selected_row > 0 ? selected_row - 1 : 0;
    size_t first = first_row * INVENTORY_COLUMNS;
    size_t last = first + INVENTORY_COLUMNS * INVENTORY_ROWS;

    if (last > inventory_count) {
        last = inventory_count;
    }
    for (size_t i = first; i < last; i++) {
        size_t visible = i - first;

        float x = ITEM_X + (visible % INVENTORY_COLUMNS) * (ITEM_SIZE + ITEM_SPACING);
        float y = ITEM_Y + (visible / INVENTORY_COLUMNS) * (ITEM_SIZE + ITEM_SPACING);
        
        if (i == selected) {
            C2D_DrawCircleSolid(x + ITEM_SIZE / 2.0f, y + ITEM_SIZE / 2.0f, 0.3f, SELECT_RADIUS, C2D_Color32(40, 65, 100, 255));
        }

        C2D_DrawImageAt(inventory[i]->image, x, y, 0.4f, NULL, 1.0f, 1.0f);
    }

    if (first > 0) {
        C2D_DrawImageAt(arrow, 365, 25, 0.4f, NULL, 1.0f, 1.0f);
    }
    if (first + INVENTORY_COLUMNS * INVENTORY_ROWS < inventory_count) {
        C2D_DrawImageAtRotated(arrow, 375, 130, 0.4f, C3D_AngleFromDegrees(180), NULL, 1.0f, 1.0f);
    }

    Item *item = inventory[selected];

    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get(item->name_id));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, 200.0f, 195.0f, 0.5f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
}
