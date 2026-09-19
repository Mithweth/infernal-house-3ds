#include <citro2d.h>
#include <3ds.h>
#include "room_hall.h"
#include "game.h"
#include "gameover.h"
#include "lang.h"
#include "hud.h"
#include "audio.h"

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
    aptHook(&apt_cookie, apt_callback, NULL);
    audio_init();
    //game_set_room(&hall);
    game_init();
    while (aptMainLoop()) {
        hidScanInput();
        u32 keys = hidKeysDown();

        circlePosition analog;
        hidCircleRead(&analog);

        touchPosition touch;
        hidTouchRead(&touch);

        if (keys & KEY_START) {
            break;
        }

        audio_update();
        game_update(keys, analog, touch);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        game_draw(top, bottom);
        C3D_FrameEnd(0);
    }

    aptUnhook(&apt_cookie);
    audio_close();
    game_close();
    hud_close();
    C2D_Fini();
    C3D_Fini();
    lang_close();
    romfsExit();
    gfxExit();

    return 0;
}
