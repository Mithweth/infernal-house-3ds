#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "room.h"
#include "gfxmap.h"

static Room *room = NULL;

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


static bool parse_bool(const char *str) {
    return strcmp(str, "true") == 0;
}


static RoomCondition condition_init(const char *type, const char *name, const char *value) {
    RoomCondition condition = {0};

    if (strcmp(type, "STATE_IS") == 0) {
        condition.type = ROOM_CONDITION_STATE_IS;
    } else if (strcmp(type, "INVENTORY_HAS") == 0) {
        condition.type = ROOM_CONDITION_INVENTORY_HAS;
    } else {
        printf("Unknown condition: %s\n", type);
        return condition;
    }

    condition.name = strdup(name);
    condition.expected = parse_bool(value);
    return condition;
}

static void condition_close(RoomCondition *condition) {
    free(condition->name);
}

static RoomAction action_init(const char *command,const char *argument) {
    RoomAction action = {0};

    if (!argument) {
        printf("Missing argument for %s\n", command);
        return action;
    }

    if (strcmp(command, "SET") == 0) {
        action.type = ROOM_ACTION_SET;
    } else if (strcmp(command, "INVENTORY_ADD") == 0) {
        action.type = ROOM_ACTION_INVENTORY_ADD;
    } else if (strcmp(command, "INVENTORY_REMOVE") == 0) {
        action.type = ROOM_ACTION_INVENTORY_REMOVE;
    } else if (strcmp(command, "MESSAGE") == 0) {
        action.type = ROOM_ACTION_MESSAGE;
    } else if (strcmp(command, "SFX") == 0) {
        action.type = ROOM_ACTION_SFX;
    } else if (strcmp(command, "WAIT_SFX") == 0) {
        action.type = ROOM_ACTION_WAIT_SFX;
    } else if (strcmp(command, "ROOM") == 0) {
        action.type = ROOM_ACTION_ROOM;
    } else if (strcmp(command, "TIMELINE") == 0) {
        action.type = ROOM_ACTION_TIMELINE;
    } else if (strcmp(command, "MINIGAME") == 0) {
        action.type = ROOM_ACTION_MINIGAME;
    } else {
        printf("Unknown action: %s\n", command);
        return action;
    }
    action.argument = strdup(argument);
    return action;
}

static void action_close(RoomAction *action) {
    free(action->argument);
}

static Path *get_path(Room *room, const char *direction) {
    if (strcmp(direction, "NORTH") == 0) {
        return &room->north;
    }

    if (strcmp(direction, "SOUTH") == 0) {
        return &room->south;
    }

    if (strcmp(direction, "EAST") == 0) {
        return &room->east;
    }

    if (strcmp(direction, "WEST") == 0) {
        return &room->west;
    }

    if (strcmp(direction, "NORTHWEST") == 0) {
        return &room->northwest;
    }

    if (strcmp(direction, "SOUTHWEST") == 0) {
        return &room->southwest;
    }

    if (strcmp(direction, "NORTHEAST") == 0) {
        return &room->northeast;
    }

    if (strcmp(direction, "SOUTHEAST") == 0) {
        return &room->southeast;
    }

    return NULL;
}

static bool load_room(const char *filename) {
    if (!room) {
        return NULL;
    }

    FILE *f = fopen(filename, "r");

    if (!f) {
        printf("Cannot open room: %s\n", filename);
        return false;
    }

    char line[512];

    RoomImage *image = NULL;
    Hotspot *hotspot = NULL;
    Path *path = NULL;
    RoomActionBlock *action_block = NULL;
    RoomUse *use = NULL;

    bool path_action = false;

    size_t line_number = 0;

    while (fgets(line, sizeof(line), f)) {
        line_number++;

        char *p = trim(line);

        if (!*p || *p == '#') {
            continue;
        }

        char *command = strtok(p, " ");

        if (!command) {
            continue;
        }

        if (strcmp(command, "IMAGE") == 0) {
            char *image_name = strtok(NULL, " ");
            char *x = strtok(NULL, " ");
            char *y = strtok(NULL, " ");
            char *z = strtok(NULL, " ");

            if (!image_name || !x || !y || !z) {
                printf("%s:%zu: invalid IMAGE\n", filename, line_number);
                fclose(f);
                return false;
            }

            if (room->image_count >= ROOM_MAX_IMAGES) {
                printf("%s:%zu: too many images\n", filename, line_number);
                fclose(f);
                return false;
            }

            image = &room->images[room->image_count++];
            memset(image, 0, sizeof(*image));

            image->image = gfxmap_get_image(room->assets, image_name);
            image->x = atof(x);
            image->y = atof(y);
            image->z = atof(z);

            continue;
        }

        if (strcmp(command, "END_IMAGE") == 0) {
            image = NULL;
            continue;
        }


        if (strcmp(command, "HOTSPOT") == 0) {
            char *id = strtok(NULL, " ");
            char *x = strtok(NULL, " ");
            char *y = strtok(NULL, " ");
            char *width = strtok(NULL, " ");
            char *height = strtok(NULL, " ");

            if (!id || !x || !y || !width || !height) {
                printf("%s:%zu: invalid HOTSPOT\n", filename, line_number);
                fclose(f);
                return false;
            }

            if (room->hotspot_count >= ROOM_MAX_HOTSPOTS) {
                printf("%s:%zu: too many hotspots\n", filename, line_number);
                fclose(f);
                return false;
            }

            hotspot = &room->hotspots[room->hotspot_count++];
            memset(hotspot, 0, sizeof(*hotspot));

            hotspot->id = strdup(id);
            hotspot->x = atoi(x);
            hotspot->y = atoi(y);
            hotspot->width = atoi(width);
            hotspot->height = atoi(height);
            continue;
        }

        if (strcmp(command, "END_HOTSPOT") == 0) {
            hotspot = NULL;
            action_block = NULL;
            use = NULL;
            continue;
        }

        if (strcmp(command, "PATH") == 0) {
            char *direction = strtok(NULL, " ");

            if (!direction) {
                printf("%s:%zu: missing PATH direction\n", filename, line_number);
                fclose(f);
                return false;
            }
            path = get_path(room, direction);
            path->exists = true;
            if (!path) {
                printf("%s:%zu: invalid PATH direction: %s\n", filename, line_number, direction);
                fclose(f);
                return false;
            }
            continue;
        }

        if (strcmp(command, "END_PATH") == 0) {
            path = NULL;
            path_action = false;
            continue;
        }

        if (strcmp(command, "ACTION") == 0) {
            if (hotspot) {
                if (hotspot->action_block_count >= ROOM_MAX_ACTION_BLOCKS) {
                    printf("%s:%zu: too many ACTION blocks\n",filename, line_number);
                    fclose(f);
                    return false;
                }

                action_block = &hotspot->action_blocks[hotspot->action_block_count++];

                memset(action_block, 0, sizeof(*action_block));

            } else if (path) {
                path_action = true;
            } else {
                printf("%s:%zu: ACTION outside HOTSPOT/PATH\n", filename, line_number);
                fclose(f);
                return false;
            }
            continue;
        }

        if (strcmp(command, "END_ACTION") == 0) {
            action_block = NULL;
            path_action = false;
            continue;
        }

        if (strcmp(command, "USE") == 0) {
            char *item = strtok(NULL, " ");

            if (!hotspot || !item) {
                printf("%s:%zu: invalid USE\n", filename, line_number);
                fclose(f);
                return false;
            }

            if (hotspot->use_count >= ROOM_MAX_USES) {
                printf("%s:%zu: too many USE blocks\n", filename, line_number);
                fclose(f);
                return false;
            }

            use = &hotspot->uses[hotspot->use_count++];
            memset(use, 0, sizeof(*use));

            use->item = strdup(item);

            continue;
        }

        if (strcmp(command, "END_USE") == 0) {
            use = NULL;
            continue;
        }

        if (strcmp(command, "WHEN") == 0) {
            char *type = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *value = strtok(NULL, " ");

            if (!type || !name || !value) {
                printf("%s:%zu: invalid WHEN\n", filename, line_number);
                fclose(f);
                return false;
            }

            RoomCondition condition = condition_init(type, name, value);

            if (!condition.name) {
                fclose(f);
                return false;
            }

            if (action_block) {
                if (action_block->condition_count >= ROOM_MAX_CONDITIONS) {
                    printf("%s:%zu: too many ACTION conditions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                action_block->conditions[action_block->condition_count++] = condition;

            } else if (use) {
                if (use->condition_count >= ROOM_MAX_CONDITIONS) {
                    printf("%s:%zu: too many USE conditions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                use->conditions[use->condition_count++] = condition;

            } else if (image) {
                if (image->condition_count >= ROOM_MAX_CONDITIONS) {
                    printf("%s:%zu: too many IMAGE conditions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                image->conditions[
                    image->condition_count++
                ] = condition;

            } else if (hotspot) {
                if (hotspot->condition_count >= ROOM_MAX_CONDITIONS) {
                    printf("%s:%zu: too many HOTSPOT conditions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                hotspot->conditions[hotspot->condition_count++] = condition;

            } else if (path && !path_action) {
                if (path->condition_count >= ROOM_MAX_CONDITIONS) {
                    printf("%s:%zu: too many PATH conditions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                path->conditions[path->condition_count++] = condition;

            } else {
                printf("%s:%zu: WHEN outside condition block\n", filename, line_number);
                fclose(f);
                return false;
            }
            continue;
        }

        if (strcmp(command, "MESSAGE") == 0 && hotspot && !action_block && !use) {

            char *message = strtok(NULL, " ");

            if (!message) {
                printf("%s:%zu: invalid MESSAGE\n", filename, line_number);
                fclose(f);
                return false;
            }
            hotspot->message_id = strdup(message);
            continue;
        }

        if (action_block || use || path_action) {
            char *argument = strtok(NULL, " ");

            RoomAction action = action_init(command, argument);

            if (!action.argument) {
                printf("%s:%zu: invalid action\n", filename, line_number);
                fclose(f);
                return false;
            }

            if (action_block) {
                if (action_block->action_count >= ROOM_MAX_ACTIONS) {
                    printf("%s:%zu: too many ACTION actions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                action_block->actions[action_block->action_count++] = action;

            } else if (use) {
                if (use->action_count >= ROOM_MAX_ACTIONS) {
                    printf("%s:%zu: too many USE actions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                use->actions[use->action_count++] = action;

            } else {
                if (path->action_count >= ROOM_MAX_ACTIONS) {
                    printf("%s:%zu: too many PATH actions\n", filename, line_number);
                    fclose(f);
                    return false;
                }

                path->actions[path->action_count++] = action;
            }

            continue;
        }


        printf("%s:%zu: unexpected directive: %s\n", filename, line_number, command);

        fclose(f);
        return false;
    }

    fclose(f);

    printf("Loaded room: %zu images, %zu hotspots\n", room->image_count, room->hotspot_count);

    return true;
}

static void execute_actions(RoomAction *action, size_t count) {
    for (size_t c = 0; c < count; c++) {
        RoomAction *action = &actions[c];
        if (action->type == ROOM_ACTION_SET) {
            gamestate_set(action->argument);
        } else if (action->type == ROOM_ACTION_INVENTORY_ADD) {
            inventory_add(action->argument);
        } else if (action->type == ROOM_ACTION_INVENTORY_REMOVE) {
            inventory_remove(action->argument);
        } else if (action->type == ROOM_ACTION_MESSAGE) {
            game_show_message(action->argument);
        } else if (action->type == ROOM_ACTION_SFX) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s.raw", room->path, action->argument);
            sfx_play(path);
        } else if (action->type == ROOM_ACTION_ROOM) {
            game_set_room(action->argument);
        } else if (action->type == ROOM_ACTION_TIMELINE) {
            game_timeline_start(action->argument);
        } else if (action->type == ROOM_ACTION_MINIGAME) {
            game_minigame_start(action->argument);
        }
    }
}

static bool match_conditions(RoomCondition *conditions, size_t count) {
    for (size_t c = 0; c < count; c++) {
        RoomCondition *condition = &conditions[c];
        bool value;
        if (condition->type == ROOM_CONDITION_STATE_IS) {
            value = gamestate_get(condition->name);
        } else {
            value = inventory_has(condition->name);
        }

        if (value != condition->expected) {
            return false;
        }
    }
    return true;
}

static bool path_is_available(Path *path) {
    if (!path->exists) {
        return false;
    }
    return match_conditions(path->conditions, path->condition_count);
}

static void room_path_execute(Path *path) {
    if (path_is_available(path)) {
        path->action();
    }
}


bool room_can_move_north(void) {
    return room && path_is_available(&room->north);
}

bool room_can_move_south(void) {
    return room && path_is_available(&room->south);
}

bool room_can_move_east(void) {
    return room && path_is_available(&room->east);
}

bool room_can_move_west(void) {
    return room && path_is_available(&room->west);
}

bool room_can_move_northwest(void) {
    return room && path_is_available(&room->northwest);
}

bool room_can_move_southwest(void) {
    return room && path_is_available(&room->southwest);
}

bool room_can_move_northeast(void) {
    return room && path_is_available(&room->northeast);
}

bool room_can_move_southeast(void) {
    return room && path_is_available(&room->southeast);
}

bool room_init(const char *name) {
    char path[256];
    if (room) {
        return false;
    }

    room = calloc(1, sizeof(Room));
    if (!room) {
        return false;
    }

    snprintf(path, sizeof(path), "romfs:/rooms/%s", name);
    room->path = strdup(path);
    if (!gfxmap_load_assets(path, &room->assets)) {
        free(room);
        return false;
    }
    snprintf(path, sizeof(path), "romfs:/rooms/%s/room", name);
    if (!load_room(path)) {
        C2D_SpriteSheetFree(room->assets);
        free(room);
        return false;
    }

    return true;
}

void room_draw(void) {
    if (!room) {
        return;
    }
    for (size_t i = 0; i < room->image_count; i++) {
        RoomImage *image = &room->images[i];
        if (match_conditions(image->conditions, image->condition_count)) {
            C2D_DrawImageAt(image->image, image->x, image->y, image->z, NULL, 1.0f, 1.0f);
        }
    }
}

void room_close(void) {
    if (!room) {
        return;
    }

    for (size_t i = 0; i < room->image_count; i++) {
        RoomImage *image = &room->images[i];
        for (size_t j = 0; j < image->condition_count; j++) {
            condition_close(&image->conditions[j]);
        }
    }

    for (size_t i = 0; i < room->hotspot_count; i++) {
        Hotspot *hotspot = &room->hotspots[i];

        free(hotspot->id);
        free(hotspot->message_id);

        for (size_t j = 0; j < hotspot->condition_count; j++) {
            condition_close(&hotspot->conditions[j]);
        }

        for (size_t j = 0; j < hotspot->action_block_count; j++) {
            RoomActionBlock *block = &hotspot->action_blocks[j];
            for (size_t k = 0; k < block->condition_count; k++) {
                condition_close(&block->conditions[k]);
            }
            for (size_t k = 0; k < block->action_count; k++) {
                action_close(&block->actions[k]);
            }
        }

        for (size_t j = 0; j < hotspot->use_count; j++) {
            RoomUse *use = &hotspot->uses[j];
            free(use->item);
            for (size_t k = 0; k < use->condition_count; k++) {
                condition_close(&use->conditions[k]);
            }
            for (size_t k = 0; k < use->action_count; k++) {
                action_close(&use->actions[k]);
            }
        }
    }

    Path *paths[] = {
        &room->north,
        &room->south,
        &room->east,
        &room->west,
        &room->northeast,
        &room->southeast,
        &room->northwest,
        &room->southwest,
    };

    for (size_t i = 0; i < 4; i++) {
        Path *path = paths[i];

        for (size_t j = 0; j < path->condition_count; j++) {
            condition_close(&path->conditions[j]);
        }

        for (size_t j = 0; j < path->action_count; j++) {
            action_close(&path->actions[j]);
        }
    }
    C2D_SpriteSheetFree(room->assets);
    free(room);
}