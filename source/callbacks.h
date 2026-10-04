// callbacks.h
// Registry of the game-specific code in extensions/: inventory callbacks
// (referenced by EXAMINE_CALLBACK / USE_CALLBACK in the inventory file) and
// mini-games (started by the MINIGAME room action). Implemented in
// extensions/callbacks.c; add new entries to its tables.
#pragma once

#include "game.h"

// Runs the init function of every inventory callback that has one (e.g.
// generates a new secret code). Called by game_init on each return to the
// title screen.
void callbacks_init(void);

// Returns the inventory callback registered under name, or NULL (and logs it)
// if there is none.
void (*callbacks_inventory_find(const char *name))(void);

// Returns the mini-game registered under name, or NULL (and logs it) if there
// is none. The returned pointer refers to a static object: do not free it.
MiniGame *callbacks_minigame_find(const char *name);
