#include <citro2d.h>

#include "game.h"
#include "room_diningroom.h"

#include "diningroom_gfx.h"
#include "diningroom_gfx_t3x.h"
#include "inventory.h"

static bool left_closet_opened = false;
static bool right_closet_opened = false;
static bool tureen_opened = false;

static C2D_SpriteSheet diningroom_scene;
static C2D_Image img_background;
static C2D_Image img_left_closet_opened;
static C2D_Image img_right_closet_opened;
static C2D_Image img_tureen_opened;
static C2D_Image img_tureen_empty;
static C2D_Image img_no_lasers;
