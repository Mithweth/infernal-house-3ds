// main.c
#include <citro2d.h>
#include <stdlib.h>
#include <3ds.h>
#include <time.h>
#include "game.h"
#include "lang.h"
#include "hud.h"
#include "audio.h"
#include "gamestate.h"

#ifdef DEBUG

#include <malloc.h>
#include <unistd.h>

static u32 *soc_buffer;
static int debug_fd = -1;

static void debug_init(void) {
    soc_buffer = memalign(0x1000, 0x100000);
    if (!soc_buffer) {
        return;
    }
    Result rc = socInit(soc_buffer, 0x100000);
    if (R_SUCCEEDED(rc)) {
        debug_fd = link3dsStdio();
    }
}

static void debug_close(void) {
    fflush(stdout);
    fflush(stderr);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if (debug_fd >= 0) {
        close(debug_fd);
    }
    socExit();
    free(soc_buffer);
}

#else
static void debug_init(void) {}
static void debug_close(void) {}
#endif

static aptHookCookie apt_cookie;

static void apt_callback(APT_HookType hook, void *param) {
    if (hook == APTHOOK_ONRESTORE || hook == APTHOOK_ONWAKEUP) {
        timer_resume();
    }
}

int main(int argc, char **argv) {
    int ret = 0;
    gfxInitDefault();
    romfsInit();
    debug_init();
    if (!lang_init()) {
        return 1;
    }
    if (!gamestate_init("romfs:/states/game.state")) {
        return 1;
    }
    srand(time(NULL));
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget *bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    aptHook(&apt_cookie, apt_callback, NULL);
    audio_init();
    if (hud_init()) {
        game_init();
    } else {
        printf("Cannot initialize HUD\n");
        ret = 1;
    }
    while (ret == 0 && aptMainLoop()) {
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
    gamestate_close();
    audio_close();
    game_close();
    C2D_Fini();
    C3D_Fini();
    lang_close();
    romfsExit();
    gfxExit();
    debug_close();
    
    return ret;
}
