// gamestate.h
#include <stdbool.h>

bool gamestate_are_diningroom_lasers_disabled(void);
void gamestate_disable_diningroom_lasers(bool value);
void gamestate_open_hall_left_closet(bool value);
void gamestate_open_hall_right_closet(bool value);
void gamestate_move_hall_carpet(bool value);
bool gamestate_is_hall_left_closet_opened(void);
bool gamestate_is_hall_right_closet_opened(void);
bool gamestate_is_hall_carpet_moved(void);
void gamestate_reset(void);
bool gamestate_is_diningroom_right_cabinet_door_opened(void);
void gamestate_open_diningroom_right_cabinet_door(bool value);
bool gamestate_is_diningroom_left_cabinet_door_opened(void);
void gamestate_open_diningroom_left_cabinet_door(bool value);
void gamestate_move_diningroom_soup(bool value);
bool gamestate_is_diningroom_soup_opened(void);
void gamestate_unscrew_cellar_alarm_box(void);
bool gamestate_is_cellar_alarm_box_unscrewed(void);
void gamestate_open_cellar_alarm_box(void);
bool gamestate_is_cellar_alarm_box_opened(void);
void gamestate_disable_cellar_alarm_box(bool value);
bool gamestate_is_cellar_alarm_box_disabled(void);
