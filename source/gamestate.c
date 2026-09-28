// gamestate.c

#include "gamestate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define GAMESTATE_MAX 128

static GameState states[GAMESTATE_MAX];
static size_t state_count = 0;


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

static GameState *gamestate_find(const char *name) {
    for (size_t i = 0; i < state_count; i++) {
        if (strcmp(states[i].name, name) == 0) {
            return &states[i];
        }
    }

    return NULL;
}

bool gamestate_init(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Cannot open %s\n", filename);
        return false;
    }

    char line[256];
    size_t line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        char *p = trim(line);

        if (*p == '\0' || *p == '#')
            continue;

        char *name = strtok(p, ";");
        char *type = strtok(NULL, ";");

        if (!name || !type) {
            printf("romfs:/game.state:%zu: syntax error\n", line_number);
            fclose(file);
            return false;
        }

        name = trim(name);
        type = trim(type);

        if (state_count >= GAMESTATE_MAX) {
            printf("romfs:/game.state:%zu: too many gamestates\n", line_number);
            fclose(file);
            return false;
        }

        GameStateType state_type;

        if (strcmp(type, "reversible") == 0) {
            state_type = GAMESTATE_REVERSIBLE;
        } else if (strcmp(type, "irreversible") == 0) {
            state_type = GAMESTATE_IRREVERSIBLE;
        } else {
            printf("romfs:/game.state:%zu: unknown type: %s\n", line_number, type);
            fclose(file);
            return false;
        }

        if (gamestate_find(name)) {
            printf("romfs:/game.state:%zu: duplicate gamestate: %s\n", line_number, name);
            fclose(file);
            return false;
        }

        GameState *state = &states[state_count++];

        state->name = strdup(name);
        state->type = state_type;
        state->value = false;
    }

    fclose(file);

    printf("Loaded %zu gamestates\n", state_count);

    return true;
}


bool gamestate_get(const char *name) {
    GameState *state = gamestate_find(name);

    if (!state) {
        printf("Unknown gamestate: %s\n", name);
        return false;
    }

    return state->value;
}


void gamestate_set(const char *name) {
    GameState *state = gamestate_find(name);

    if (!state) {
        printf("Unknown gamestate: %s\n", name);
        return;
    }

    if (state->type == GAMESTATE_IRREVERSIBLE && state->value) {
        return;
    }

    state->value = !state->value;
}


void gamestate_close(void) {
    for (size_t i = 0; i < state_count; i++) {
        free(states[i].name);
        states[i].name = NULL;
    }

    state_count = 0;
}

void gamestate_reset(void) {
    for (size_t i = 0; i < state_count; i++) {
        states[i].value = false;
    }
}
