// syringe.c

#include "inventory.h"
#include "game.h"
#include "gamestate.h"

void syringe_use(void) {
    inventory_remove("SYRINGE");
    gamestate_set("item_syringe_injected");
    game_show_message("ITEM_SYRINGE_USED");
}
