#include <citro2d.h>

#include "game.h"
#include "room_diningroom.h"

#include "diningroom_gfx.h"
#include "diningroom_gfx_t3x.h"
#include "inventory.h"
#include "room_hall.h"

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
    room_scene = C2D_SpriteSheetLoadFromMem(diningroom_gfx_t3x, diningroom_gfx_t3x_size);
    img_background = C2D_SpriteSheetGetImage(room_scene, diningroom_gfx_bg_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

Room dining_room = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .up = NULL,
    .down = NULL,
    .left = NULL,
    .right = &hall,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};