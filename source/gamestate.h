// gamestate.h

#pragma once

#include <stdbool.h>

typedef enum {
    GAMESTATE_TOGGLE,
    GAMESTATE_KEEP
} GameStateType;

typedef struct {
    char *name;
    GameStateType type;
    bool value;
} GameState;

bool gamestate_init(const char *filename);
void gamestate_close(void);
bool gamestate_get(const char *name);
void gamestate_set(const char *name);
void gamestate_reset(void);
