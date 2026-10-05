// inventory.h
// Item catalogue (loaded from romfs:/inventory/inventory) and the player's
// inventory, drawn in the top-screen HUD. Driven by hud.c: inventory_init and
// inventory_close are called from hud_init and hud_close.
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

// One catalogue entry. Strings are owned by the inventory module.
typedef struct {
    char *id;               // identifier used by room scripts (INVENTORY_ADD, USE...)
    char *name_id;          // lang key of the name shown in the HUD
    char *examine_text;     // lang key shown when examining (X), or NULL
    C2D_Image image;
    C2D_Image detail_image; // image shown when examining, or empty
    bool detail_fullscreen; // black out the whole top screen behind detail_image
    float detail_x;
    float detail_y;
    void (*examine_callback)(void); // extra drawing while examining, or NULL
    void (*use_callback)(void);     // replaces the "use on target" logic, or NULL
} Item;

typedef enum {
    INVENTORY_NORMAL,       // browsing items
    INVENTORY_ACTION        // examining the selected item (X pressed)
} InventoryMode;

// Loads the inventory spritesheet and the item catalogue. Returns false on any
// load or syntax error. Must be called once, after C2D is initialized.
bool inventory_init(void);

// Frees the catalogue and graphics. Safe to call more than once.
void inventory_close(void);

// Adds a catalogue item to the inventory and selects it. Unknown ids are
// logged and ignored; adding an item already held does nothing.
void inventory_add(const char *id);

// Returns true if the player currently holds the item.
bool inventory_has(const char *id);

// Removes the item from the inventory if held; the selection is kept in range.
void inventory_remove(const char *id);

// Handles D-pad navigation, X (examine) and A (use). Returns true when the
// keys were consumed, in which case the caller must not process them further.
bool inventory_update(u32 keys);

// Returns true while an item is being examined.
bool inventory_is_active(void);

// Empties the inventory and leaves examine mode (new game).
void inventory_reset(void);

// Returns the selected item, or NULL if the inventory is empty. The pointer
// stays owned by the inventory module.
const Item *inventory_get_selected_item(void);
const Item *inventory_get_item(int id);
int inventory_get_selected(void);
int inventory_get_count(void);
void inventory_set_columns(size_t value);