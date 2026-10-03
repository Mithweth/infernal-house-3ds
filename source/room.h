#pragma once

#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>

#define ROOM_MAX_IMAGES          64
#define ROOM_MAX_HOTSPOTS        64
#define ROOM_MAX_CONDITIONS       8
#define ROOM_MAX_ACTION_BLOCKS    8
#define ROOM_MAX_ACTIONS         16
#define ROOM_MAX_USES             8

typedef enum {
    ROOM_CONDITION_STATE_IS,
    ROOM_CONDITION_INVENTORY_HAS
} RoomConditionType;

typedef struct {
    RoomConditionType type;
    char *name;
    bool expected;
} RoomCondition;

typedef enum {
    ROOM_ACTION_SET,
    ROOM_ACTION_INVENTORY_ADD,
    ROOM_ACTION_INVENTORY_REMOVE,
    ROOM_ACTION_MESSAGE,
    ROOM_ACTION_SFX,
    ROOM_ACTION_WAIT_SFX,
    ROOM_ACTION_ROOM,
    ROOM_ACTION_TIMELINE,
    ROOM_ACTION_MINIGAME
} RoomActionType;

typedef struct {
    RoomActionType type;
    char *argument;
} RoomAction;

typedef struct {
    RoomCondition conditions[ROOM_MAX_CONDITIONS];
    size_t condition_count;
    RoomAction actions[ROOM_MAX_ACTIONS];
    size_t action_count;
} RoomActionBlock;

typedef struct {
    char *item;
    RoomCondition conditions[ROOM_MAX_CONDITIONS];
    size_t condition_count;
    RoomAction actions[ROOM_MAX_ACTIONS];
    size_t action_count;
} RoomUse;

typedef struct {
    C2D_Image image;
    float x;
    float y;
    float z;
    RoomCondition conditions[ROOM_MAX_CONDITIONS];
    size_t condition_count;
} RoomImage;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    char *id;
    char *message_id;
    RoomCondition conditions[ROOM_MAX_CONDITIONS];
    size_t condition_count;
    RoomActionBlock action_blocks[ROOM_MAX_ACTION_BLOCKS];
    size_t action_block_count;
    RoomUse uses[ROOM_MAX_USES];
    size_t use_count;
} Hotspot;

typedef struct {
    bool exists;
    RoomCondition conditions[ROOM_MAX_CONDITIONS];
    size_t condition_count;
    RoomAction actions[ROOM_MAX_ACTIONS];
    size_t action_count;
} Path;

typedef struct {
    char *path;
    C2D_SpriteSheet assets;
    RoomImage images[ROOM_MAX_IMAGES];
    size_t image_count;
    Hotspot hotspots[ROOM_MAX_HOTSPOTS];
    size_t hotspot_count;
    Path north;
    Path northwest;
    Path south;
    Path southwest;
    Path east;
    Path northeast;
    Path west;
    Path southeast;
} Room;

void room_move_north(void);
void room_move_northeast(void);
void room_move_east(void);
void room_move_southeast(void);
void room_move_south(void);
void room_move_southwest(void);
void room_move_west(void);
void room_move_northwest(void);
bool room_execute_hotspot_use(Hotspot *hotspot, const char *id);
void room_execute_hotspot_action(Hotspot *hotspot);
bool room_hotspot_is_available(Hotspot *hotspot);
bool room_can_move_north(void);
bool room_can_move_northeast(void);
bool room_can_move_east(void);
bool room_can_move_southeast(void);
bool room_can_move_south(void);
bool room_can_move_southwest(void);
bool room_can_move_west(void);
bool room_can_move_northwest(void);
bool room_can_move_north(void);
bool room_can_move_south(void);
bool room_can_move_east(void);
bool room_can_move_west(void);
bool room_can_move_northwest(void);
bool room_can_move_southwest(void);
bool room_can_move_northeast(void);
bool room_can_move_southeast(void);
bool room_init(const char *name);
void room_draw(void);
void room_close(void);
Hotspot *room_find_hotspot(int x, int y);
Hotspot *room_find_hotspot_by_id(const char *id);
