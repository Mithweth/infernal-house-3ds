#include <citro2d.h>
#include <3ds.h>
#include "room_hall.h"
#include "game.h"
#include "lang.h"
#include "hud.h"

static aptHookCookie apt_cookie;

static void apt_callback(APT_HookType hook, void *param) {
    if (hook == APTHOOK_ONRESTORE) {
        timer_resume();
    }
}

int main(int argc, char **argv)
{
    gfxInitDefault();
    romfsInit();
    if (!lang_init("fr")) {
    	return 1;
    }
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    hud_init();
    gameover_init();
    aptHook(&apt_cookie, apt_callback, NULL);
    game_set_room(&hall);

	while (aptMainLoop())
	{
	    hidScanInput();
	    u32 keys = hidKeysDown();

		circlePosition analog;
		hidCircleRead(&analog);

    	touchPosition touch;
    	hidTouchRead(&touch);

	    if (keys & KEY_START) {
	        break;
	    }

	    hud_update();
    	game_update(keys, analog, touch);
	    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
	    C2D_SceneBegin(bottom);
	    game_draw();

	    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
	    C2D_SceneBegin(top);
	    hud_draw();

	    C3D_FrameEnd(0);
	}

	aptUnhook(&apt_cookie);
    game_close();
    hud_close();
    C2D_Fini();
    C3D_Fini();
    lang_close();
	romfsExit();
    gfxExit();

    return 0;
}
