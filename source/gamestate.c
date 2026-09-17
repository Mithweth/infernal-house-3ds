// gamestate.c

#include <stdbool.h>
#include <string.h>

typedef struct {
    bool diningroom_lasers_disabled;
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

void gamestate_reset(void) {
    memset(&game_state, 0, sizeof(game_state));
}