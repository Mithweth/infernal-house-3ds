// callbacks.h
#pragma once

#include "game.h"

void callbacks_init(void);
void (*callbacks_inventory_find(const char *name))(void);
MiniGame *callbacks_minigame_find(const char *name);