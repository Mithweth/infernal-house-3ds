#include <citro2d.h>
#include "game.h"
#include "room_corridor.h"
#include "gfx_corridor.h"
#include "inventory.h"
#include "room_diningroom.h"

static C2D_SpriteSheet room_scene;
static C2D_Image img_background;

static Hotspot hotspots[] = {};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_corridor.t3x");
    img_background = C2D_SpriteSheetGetImage(room_scene, gfx_corridor_bg_idx);
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

static void down_action(void) {
    game_set_room(&dining_room);
}

Room corridor = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .up = NULL,
    .down = down_action,
    .left = NULL,
    .right = NULL,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};