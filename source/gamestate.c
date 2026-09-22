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
    bool cellar_alarm_box_unscrewed;
    bool cellar_alarm_box_opened;
    bool livingroom_piano_opened;
    bool livingroom_fireplace_hearth_opened;
    bool livingroom_golden_statue_placed;
    bool livingroom_secret_passage;
    bool rope_used_in_livingroom_hearth;
    bool library_secret_passage_opened;
    bool kitchen_refrigerator_opened;
    bool kitchen_below_closet_opened;
    bool kitchen_oven_opened;
    bool kitchen_top_closet_opened;
    bool kitchen_bottom_closet_opened;
    bool kitchen_drawer_opened;
    bool kitchen_dishwasher_opened;
    bool firstfloor_carpet_moved;
} GameState;

static GameState game_state;

bool gamestate_are_diningroom_lasers_disabled(void) {
    return game_state.diningroom_lasers_disabled;
}

void gamestate_disable_diningroom_lasers(bool value) {
    game_state.diningroom_lasers_disabled = value;
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

void gamestate_unscrew_cellar_alarm_box(void) {
    game_state.cellar_alarm_box_unscrewed = true;
}

bool gamestate_is_cellar_alarm_box_unscrewed(void) {
    return game_state.cellar_alarm_box_unscrewed;
}

void gamestate_open_cellar_alarm_box(void) {
    game_state.cellar_alarm_box_opened = true;
}

bool gamestate_is_cellar_alarm_box_opened(void) {
    return game_state.cellar_alarm_box_opened;
}

void gamestate_open_livingroom_piano(bool value) {
    game_state.livingroom_piano_opened = value;
}

bool gamestate_is_livingroom_piano_opened(void) {
    return game_state.livingroom_piano_opened;
}

void gamestate_open_livingroom_fireplace_hearth(void) {
    game_state.livingroom_fireplace_hearth_opened = true;
}

bool gamestate_is_livingroom_fireplace_hearth_opened(void) {
    return game_state.livingroom_fireplace_hearth_opened;
}

void gamestate_set_livingroom_golden_statue_placed(void) {
    game_state.livingroom_golden_statue_placed = true;
}

bool gamestate_is_livingroom_golden_statue_placed(void) {
    return game_state.livingroom_golden_statue_placed;
}

void gamestate_open_livingroom_secret_passage(void) {
    game_state.livingroom_secret_passage = true;
}

bool gamestate_is_livingroom_secret_passage_opened(void) {
    return game_state.livingroom_secret_passage;
}

void gamestate_bind_rope_used_in_livingroom_hearth(void) {
    game_state.rope_used_in_livingroom_hearth = true;
}

bool gamestate_is_rope_used_in_livingroom_hearth(void) {
    return game_state.rope_used_in_livingroom_hearth;
}

void gamestate_open_library_secret_passage(void) {
    game_state.library_secret_passage_opened = true;
}

bool gamestate_is_library_secret_passage_opened(void) {
    return game_state.library_secret_passage_opened;
}

void gamestate_open_kitchen_refrigerator(bool value) {
    game_state.kitchen_refrigerator_opened = value;
}

bool gamestate_is_kitchen_refrigerator_opened(void) {
    return game_state.kitchen_refrigerator_opened;
}

void gamestate_open_kitchen_below_closet(bool value) {
    game_state.kitchen_below_closet_opened = value;
}

bool gamestate_is_kitchen_below_closet_opened(void) {
    return game_state.kitchen_below_closet_opened;
}

void gamestate_open_kitchen_oven(bool value) {
    game_state.kitchen_oven_opened = value;
}

bool gamestate_is_kitchen_oven_opened(void) {
    return game_state.kitchen_oven_opened;
}

void gamestate_open_kitchen_top_closet(bool value) {
    game_state.kitchen_top_closet_opened = value;
}

bool gamestate_is_kitchen_top_closet_opened(void) {
    return game_state.kitchen_top_closet_opened;
}

void gamestate_open_kitchen_bottom_closet(bool value) {
    game_state.kitchen_bottom_closet_opened = value;
}

bool gamestate_is_kitchen_bottom_closet_opened(void) {
    return game_state.kitchen_bottom_closet_opened;
}

void gamestate_open_kitchen_dishwasher(bool value) {
    game_state.kitchen_dishwasher_opened = value;
}

bool gamestate_is_kitchen_dishwasher_opened(void) {
    return game_state.kitchen_dishwasher_opened;
}

void gamestate_open_kitchen_drawer(bool value) {
    game_state.kitchen_drawer_opened = value;
}

bool gamestate_is_kitchen_drawer_opened(void) {
    return game_state.kitchen_drawer_opened;
}

void gamestate_move_firstfloor_carpet(bool value) {
    game_state.firstfloor_carpet_moved = value;
}

bool gamestate_is_firstfloor_carpet_moved(void) {
    return game_state.firstfloor_carpet_moved;
}

void gamestate_reset(void) {
    memset(&game_state, 0, sizeof(game_state));
}
