// inventory.c
#include <citro2d.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inventory.h"
#include "game.h"
#include "lang.h"

#define ITEM_MAX       64
#define ITEM_NAME_MAX  96
#define INVENTORY_MAX_IMAGES 128
#define INVENTORY_COLUMNS  6
#define INVENTORY_ROWS     2
#define ITEM_SIZE       32.0f
#define ITEM_INNER_SPACING       4.0f
#define ITEM_OUTER_SPACING_X    14.0f
#define ITEM_OUTER_SPACING_Y    10.0f
#define ITEM_Y          75.0f
#define ITEM_X          20.0f

static Item items[ITEM_MAX];
static size_t item_count = 0;
static size_t selected = 0;

static Item *inventory[ITEM_MAX];
static size_t inventory_count = 0;

typedef struct {
    char name[ITEM_NAME_MAX];
    int index;
} InventoryImageIndex;

static InventoryImageIndex image_indexes[INVENTORY_MAX_IMAGES];
static size_t image_index_count = 0;
static C2D_SpriteSheet inventory_assets;
static C2D_TextBuf text_buf;
static C2D_Text text;
static C2D_Image img_selected;
static C2D_Image img_background;
static InventoryMode inventory_mode = INVENTORY_NORMAL;

static char *trim(char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    char *end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    return str;
}

static int get_image_index(const char *name) {
    for (size_t i = 0; i < image_index_count; i++) {
        if (strcmp(image_indexes[i].name, name) == 0) {
            return image_indexes[i].index;
        }
    }
    return -1;
}

static bool load_gfx_header(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Cannot open %s\n", filename);
        return false;
    }

    image_index_count = 0;

    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char directive[32];
        char name[ITEM_NAME_MAX];
        char value[32];

        if (sscanf(line, "%31s %95s %31s", directive, name, value) != 3) {
            continue;
        }

        if (strcmp(directive, "#define") != 0) {
            continue;
        }

        size_t len = strlen(name);

        if (len < 4 || strcmp(name + len - 4, "_idx") != 0) {
            continue;
        }
        int index = atoi(value);

        if (image_index_count >= INVENTORY_MAX_IMAGES) {
            printf("Too many images in %s\n", filename);
            fclose(f);
            return false;
        }

        InventoryImageIndex *entry = &image_indexes[image_index_count++];

        strcpy(entry->name, name);
        entry->index = index;
    }

    fclose(f);
    return true;
}

static C2D_Image get_image(const char *name) {
    int index = get_image_index(name);

    if (index < 0) {
        return (C2D_Image){0};
    }

    return C2D_SpriteSheetGetImage(inventory_assets, index);
}

static Item *inventory_find(const char *id) {
    for (size_t i = 0; i < item_count; i++) {
        if (strcmp(items[i].id, id) == 0) {
            return &items[i];
        }
    }

    return NULL;
}

static Item *add_item(const char *id, const char *name_id) {
    if (item_count >= ITEM_MAX) {
        return NULL;
    }
    Item *item = &items[item_count++];
    memset(item, 0, sizeof(*item));
    item->id = strdup(id);
    item->name_id = strdup(name_id);
    return item;
}

static bool load_inventory(const char *filename) {
    FILE *f = fopen(filename, "r");

    if (!f) {
        printf("Cannot open inventory: %s\n", filename);
        return false;
    }

    item_count = 0;

    char line[256];
    size_t line_number = 0;
    Item *item = NULL;

    while (fgets(line, sizeof(line), f)) {
        line_number++;
        char *p = trim(line);

        if (*p == '\0' || *p == '#') {
            continue;
        }

        char *command = strtok(p, " ");

        if (!command) {
            continue;
        }

        if (item) {
            if (strcmp(command, "IMAGE") == 0) {
                char *image_id = strtok(NULL, " ");
                if (!image_id) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                item->image = get_image(image_id);
                if (!item->image.tex) {
                    printf("%s:%zu: unknown image: %s\n", filename, line_number, image_id);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                continue;
            }
            if (strcmp(command, "EXAMINE") == 0) {
                char *text = strtok(NULL, " ");
                if (!text) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                item->examine_text = strdup(text);
                continue;
            }
            if (strcmp(command, "CALL") == 0) {
                char *cb = strtok(NULL, " ");
                if (!cb) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                item->callback = game_callback_find(cb);
                continue;
            }
            if (strcmp(command, "DETAIL") == 0) {
                char *image_id = strtok(NULL, " ");
                if (!image_id) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }

                char *x  = strtok(NULL, " ");
                char *y  = strtok(NULL, " ");
                char *fmt = strtok(NULL, " ");

                if ((!x) || (!y)) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                if ((fmt) && (strcmp(fmt, "FULLSCREEN") == 0)) {
                    item->detail_fullscreen = true;
                }
                item->detail_image = get_image(image_id);

                if (!item->detail_image.tex) {
                    printf("%s:%zu: unknown image: %s\n", filename, line_number, image_id);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                item->detail_x = atof(x);
                item->detail_y = atof(y);
                continue;
            }
            if (strcmp(command, "END_ITEM") == 0) {
                if (!item->image.tex) {
                    printf("%s:%zu: syntax error\n", filename, line_number);
                    fclose(f);
                    item_count = 0;
                    return false;
                }
                item = NULL;
                continue;
            }
            printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
            fclose(f);
            item_count = 0;
            return false;
        }
        if (strcmp(command, "ITEM") == 0) {
            char *item_id = strtok(NULL, " ");
            char *name_id = strtok(NULL, " ");
            if ((!item_id) || (!name_id)) {
                printf("%s:%zu: syntax error\n", filename, line_number);
                fclose(f);
                item_count = 0;
                return false;
            }
            if (inventory_find(item_id)) {
                printf("%s:%zu: already exists: %s\n", filename, line_number, item_id);
                fclose(f);
                item_count = 0;
                return false;
            }
            item = add_item(item_id, name_id);
            if (!item) {
                printf("%s:%zu: too many items\n", filename, line_number);
                fclose(f);
                item_count = 0;
                return false;
            }
            continue;
        }
        printf("%s:%zu: syntax error\n", filename, line_number);
        fclose(f);
        item_count = 0;
        return false;
    }

    if (item) {
        printf("%s:%zu: missing END_ITEM\n", filename, line_number);
        fclose(f);
        item_count = 0;
        return false;
    }

    fclose(f);
    printf("Loaded %zu inventory items\n", item_count);
    return true;
}

bool inventory_update(u32 keys) {
    if (inventory_mode == INVENTORY_ACTION) {
        if ((keys & KEY_X) || (keys & KEY_B)) {
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

    Item *item = inventory[selected];
    if (keys & KEY_X) {    
        if (item->detail_image.tex || item->examine_text || item->callback) {
            inventory_mode = INVENTORY_ACTION;
        }
        return true;
    }

    if (keys & KEY_A) {
        game_use_item(item->id);
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
        C2D_DrawImageAt(img_background, 7.0f, 49.0f, 0.4f, NULL, 1.0f, 1.0f);
        if (item->detail_image.tex) {
            if (item->detail_fullscreen) {
                C2D_DrawRectSolid(0.0f, 0.0f, 0.8f, 400.0f, 240.0f, C2D_Color32(0, 0, 0, 255));
            }
            C2D_DrawImageAt(item->detail_image, item->detail_x, item->detail_y, 0.9f, NULL, 1.0f, 1.0f);
        }
        if (item->examine_text) {
            C2D_TextBufClear(text_buf);
            C2D_TextParse(&text, text_buf, lang_get(item->examine_text));
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 20.0f, 62.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(192, 192, 192, 255));
        }
        if (item->callback) {
            item->callback();
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

        float x = ITEM_X + (visible % INVENTORY_COLUMNS) * (ITEM_SIZE + ITEM_OUTER_SPACING_X);
        float y = ITEM_Y + (visible / INVENTORY_COLUMNS) * (ITEM_SIZE + ITEM_OUTER_SPACING_Y);
        
        if (i == selected) {
            C2D_DrawImageAt(img_selected, x - ITEM_INNER_SPACING, y - ITEM_INNER_SPACING, 0.2f, NULL, 1.0f, 1.0f);
        }

        C2D_DrawImageAt(inventory[i]->image, x, y, 0.4f, NULL, ITEM_SIZE / 48, ITEM_SIZE / 48);
    }
}

const Item *inventory_get_selected(void) {
    return inventory[selected];
}


bool inventory_is_active(void) {
    return inventory_mode == INVENTORY_ACTION;
}

void inventory_reset(void) {
    inventory_count = 0;
    selected = 0;
    inventory_mode = INVENTORY_NORMAL;
}

bool inventory_has(const char *id) {
    Item *item = inventory_find(id);

    if (!item) {
        return false;
    }

    for (size_t i = 0; i < inventory_count; i++) {
        if (inventory[i] == item)
            return true;
    }

    return false;
}

void inventory_add(const char *id) {
    Item *item = inventory_find(id);

    if (!item) {
        printf("Unknown inventory item: %s\n", id);
        return;
    }

    if (inventory_count >= ITEM_MAX) {
        return;
    }

    if (inventory_has(id)) {
        return;
    }

    inventory[inventory_count++] = item;
    selected = inventory_count - 1;
}

void inventory_remove(const char *id) {
    Item *item = inventory_find(id);

    if (!item) {
        return;
    }

    for (size_t i = 0; i < inventory_count; i++) {
        if (inventory[i] != item) {
            continue;
        }

        for (size_t j = i; j < inventory_count - 1; j++)
            inventory[j] = inventory[j + 1];

        inventory_count--;

        if (inventory_count == 0) {
            selected = 0;
        } else if (selected >= inventory_count) {
            selected = inventory_count - 1;
        }
        return;
    }
}

bool inventory_init(void) {
    inventory_assets = C2D_SpriteSheetLoad("romfs:/inventory/gfx.t3x");
    if (!inventory_assets) {
        return false;
    }

    if (!load_gfx_header("romfs:/inventory/gfx.h")) {
        printf("Cannot load gfx headers\n");
        C2D_SpriteSheetFree(inventory_assets);
        inventory_assets = NULL;
        return false;
    }

    if (!load_inventory("romfs:/inventory/inventory")) {
        printf("Cannot load inventory\n");
        C2D_SpriteSheetFree(inventory_assets);
        inventory_assets = NULL;
        return false;
    }

    int img_idx = get_image_index("gfx_selected_idx");
    if (img_idx < 0) {
        printf("unknown image: gfx_selected_idx\n");
        C2D_SpriteSheetFree(inventory_assets);
        inventory_assets = NULL;
        return false;
    }
    img_selected = C2D_SpriteSheetGetImage(inventory_assets, img_idx);

    img_idx = get_image_index("gfx_background_idx");
    if (img_idx < 0) {
        printf("unknown image: gfx_background_idx\n");
        C2D_SpriteSheetFree(inventory_assets);
        inventory_assets = NULL;
        return false;
    }
    img_background = C2D_SpriteSheetGetImage(inventory_assets, img_idx);

    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }

    inventory_count = 0;
    selected = 0;

    return true;
}

void inventory_close(void) {
    for (size_t i = 0; i < item_count; i++) {
        free(items[i].id);
        free(items[i].name_id);
        free(items[i].examine_text);
    }
    if (inventory_assets) {
        C2D_SpriteSheetFree(inventory_assets);
        inventory_assets = NULL;
    }
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    inventory_count = 0;
    selected = 0;
}