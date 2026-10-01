// room_heliport.c

#include <citro2d.h>
#include "game.h"
#include "room_heliport.h"
#include "room_cryoroom.h"
#include "inventory.h"
#include "gamestate.h"
#include "gfxmap.h"

static C2D_SpriteSheet assets;
static C2D_Image img_background;

static Hotspot hotspots[] = {};

static void room_init(void) {
    if (!gfxmap_load_assets("romfs:/gfx/heliport", &assets)) {
        return;
    }
    img_background = gfxmap_get_image(assets, "gfx_background_idx");
}

static void room_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

static void room_close(void) {
    C2D_SpriteSheetFree(assets);
}

static void move_south(void) {
    game_set_room(&cryoroom);
}

Room heliport = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .south = {.action = move_south},
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
