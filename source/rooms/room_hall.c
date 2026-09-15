#include <citro2d.h>

#include "game.h"
#include "room_hall.h"

#include "hall.h"
#include "hall_t3x.h"

static bool closet_open = false;
static bool carpet_moved = false;
static bool message_taken = false;

static C2D_SpriteSheet hall_scene;
static C2D_Image img_background;
static C2D_Image img_closet_opened;
static C2D_Image img_closet_empty;
static C2D_Image img_carpet_moved;
static C2D_Image img_message;

static void action_closet(void) {
    closet_open = !closet_open;
}

static void action_carpet(void) {
    carpet_moved = !carpet_moved;
}

static void action_message(void) {
    message_taken = !message_taken;
}

static Hotspot hotspots[] = {
    {
        .x = 25,
        .y = 108,
        .width = 65,
        .height = 90,
        .description = "C'est un vieux buffet en bois.",
        .is_active = NULL,
        .action = action_closet
    },
    {
        .x = 124,
        .y = 200,
        .width = 28,
        .height = 20,
        .description = "Un message caché.",
        .is_active = NULL,
        .action = action_message
    },
    {
        .x = 46,
        .y = 175,
        .width = 200,
        .height = 65,
        .description = "Un grand tapis.",
        .is_active = NULL,
        .action = action_carpet
    }
};


static void hall_init(void) {
    hall_scene = C2D_SpriteSheetLoadFromMem(hall_t3x, hall_t3x_size);
    img_background = C2D_SpriteSheetGetImage(hall_scene, hall_1_bg_idx);
    img_closet_opened = C2D_SpriteSheetGetImage(hall_scene, hall_1_closet_opened_idx);
    img_closet_empty = C2D_SpriteSheetGetImage(hall_scene, hall_1_closet_empty_idx);
    img_carpet_moved = C2D_SpriteSheetGetImage(hall_scene, hall_1_carpet_moved_idx);
    img_message = C2D_SpriteSheetGetImage(hall_scene, hall_1_message_idx);
}

static void hall_draw(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);

    if (closet_open) {
        C2D_DrawImageAt(img_closet_opened, 39.0f, 129.0f, 0.1f, NULL, 1.0f, 1.0f);
    }
    if (carpet_moved) {
        C2D_DrawImageAt(img_carpet_moved, 22.0f, 169.0f, 0.2f, NULL, 1.0f, 1.0f);
        if (!message_taken) {
            C2D_DrawImageAt(img_message, 124.0f, 200.0f, 0.3f, NULL, 1.0f, 1.0f);
        }
    }
}

static void hall_close(void)
{
    C2D_SpriteSheetFree(hall_scene);
}

Room hall = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .init = hall_init,
    .draw = hall_draw,
    .close = hall_close
};