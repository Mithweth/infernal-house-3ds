// callbacks.c
#include "callbacks.h"
#include "game.h"
#include "secret_code.h"
#include "syringe.h"
#include "simon.h"
#include "piano.h"
#include "measure.h"
#include "digicode.h"


typedef struct {
    const char *name;
    void (*init)(void);
    void (*callback)(void);
} InventoryCallback;

typedef struct {
    const char *name;
    MiniGame *minigame;
} MiniGameCallback;

static MiniGameCallback minigame_callbacks[] = {
    { "simon",    &simon },
    { "piano",    &piano },
    { "measure",  &measure },
    { "digicode", &digicode }
};

static InventoryCallback inventory_callbacks[] = {
    {
        .name = "secret_code",
        .init = secret_code_init,
        .callback = secret_code_draw
    },
    {
        .name = "inject_syringe",
        .callback = syringe_use
    }
};

static const size_t minigame_callback_count = sizeof(minigame_callbacks) / sizeof(minigame_callbacks[0]);
static const size_t inventory_callback_count = sizeof(inventory_callbacks) / sizeof(inventory_callbacks[0]);

void (*callbacks_inventory_find(const char *name))(void) {
    for (size_t i = 0; i < inventory_callback_count; i++) {
        InventoryCallback *cb = &inventory_callbacks[i];
        if (strcmp(cb->name, name) == 0) {
            return cb->callback;
        }
    }
    return NULL;
}

MiniGame *callbacks_minigame_find(const char *name) {
    for (size_t i = 0; i < minigame_callback_count; i++) {
        MiniGameCallback *cb = &minigame_callbacks[i];

        if (strcmp(cb->name, name) == 0) {
            return cb->minigame;
        }
    }

    return NULL;
}

void callbacks_init(void) {
    for (size_t i = 0; i < inventory_callback_count; i++) {
        InventoryCallback *cb = &inventory_callbacks[i];
        if (cb->init) {
            cb->init();
        }
    }
}
