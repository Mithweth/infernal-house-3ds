// gamestate.c

#include <stdbool.h>
#include <string.h>

typedef struct {
    bool diningroom_lasers_disabled;
    bool diningroom_right_cabinet_door_opened;
    bool diningroom_left_cabinet_door_opened;
    bool diningroom_soup_opened;
    bool hall_left_closet_opened;
    bool hall_right_closet_opened;
    bool hall_carpet_moved;
} GameState;

static GameState game_state;

bool gamestate_are_diningroom_lasers_disabled(void) {
    return game_state.diningroom_lasers_disabled;
}

void gamestate_disable_diningroom_lasers(void) {
    game_state.diningroom_lasers_disabled = true;
}

void gamestate_open_hall_left_closet(bool value) {
    game_state.hall_left_closet_opened = value;
}

void gamestate_open_hall_right_closet(bool value) {
    game_state.hall_right_closet_opened = value;
}

void gamestate_move_hall_carpet(bool value) {
    game_state.hall_carpet_moved = value;
}

bool gamestate_is_hall_left_closet_opened(void) {
    return game_state.hall_left_closet_opened;
}

bool gamestate_is_hall_right_closet_opened(void) {
    return game_state.hall_right_closet_opened;
}

bool gamestate_is_hall_carpet_moved(void) {
    return game_state.hall_carpet_moved;
}


bool gamestate_is_diningroom_right_cabinet_door_opened(void) {
    return game_state.diningroom_right_cabinet_door_opened;
}

void gamestate_open_diningroom_right_cabinet_door(bool value) {
    game_state.diningroom_right_cabinet_door_opened = value;
}

bool gamestate_is_diningroom_left_cabinet_door_opened(void) {
    return game_state.diningroom_left_cabinet_door_opened;
}

void gamestate_open_diningroom_left_cabinet_door(bool value) {
    game_state.diningroom_left_cabinet_door_opened = value;
}

bool gamestate_is_diningroom_soup_opened(void) {
    return game_state.diningroom_soup_opened;
}

void gamestate_move_diningroom_soup(bool value) {
    game_state.diningroom_soup_opened = value;
}

void gamestate_reset(void) {
    memset(&game_state, 0, sizeof(game_state));
}