#include <citro2d.h>

#include "game.h"
#include "audio.h"
#include "room_diningroom.h"

#include "gfx_diningroom.h"
#include "inventory.h"
#include "room_hall.h"
#include "room_corridor.h"
#include "gamestate.h"
#include "gameover.h"

// static bool left_closet_opened = false;
// static bool right_closet_opened = false;
// static bool tureen_opened = false;

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;
// static C2D_Image img_left_closet_opened;
// static C2D_Image img_right_closet_opened;
// static C2D_Image img_tureen_opened;
// static C2D_Image img_tureen_empty;
// static C2D_Image img_no_lasers;

static Hotspot hotspots[] = {};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_diningroom.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_diningroom_bg_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void move_right(void) {
    game_set_room(&hall);
}

static void right_action(void) {
    game_wait_for_sfx("romfs:/audio/door_open.raw", move_right);
}

static void left_action(void) {
    game_set_room(&corridor);
}

static void lasers_gameover(void) {
	game_over(GAMEOVER_LASERS);
}

static void up_action(void) {
    if (!gamestate_are_diningroom_lasers_disabled()) {
        game_wait_for_sfx("romfs:/audio/police_siren.raw", lasers_gameover);
    }
}

Room dining_room = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .up = up_action,
    .down = NULL,
    .left = left_action,
    .right = right_action,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};