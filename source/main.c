#include <citro2d.h>
#include <3ds.h>
#include "hall.h"
#include "hall_t3x.h"

int main(int argc, char **argv)
{
    gfxInitDefault();

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    C2D_SpriteSheet hall_scene = C2D_SpriteSheetLoadFromMem(hall_t3x, hall_t3x_size);
    C2D_Image hall_bg = C2D_SpriteSheetGetImage(hall_scene, hall_1_bg_idx);
    C2D_Image hall_closet_opened =C2D_SpriteSheetGetImage(hall_scene, hall_1_closet_opened_idx);
	bool hall_closet_is_open = false;

    while (aptMainLoop())
    {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START) break;

        if (down & KEY_TOUCH) {
        	touchPosition touch;
			hidTouchRead(&touch);
		    if (touch.px >= 36 && touch.px <= 62 && touch.py >= 130 && touch.py <= 197) {
		        hall_closet_is_open ^= true;
		    }
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(bottom);
        C2D_DrawImageAt(hall_bg, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
        if (hall_closet_is_open) {
        	C2D_DrawImageAt(hall_closet_opened, 36.0f, 130.0f, 0.1f, NULL, 1.0f, 1.0f);
		}
        C3D_FrameEnd(0);
    }

    C2D_SpriteSheetFree(hall_scene);

    C2D_Fini();
    C3D_Fini();
    gfxExit();

    return 0;
}
