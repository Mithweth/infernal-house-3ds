// gameover.c

#include <citro2d.h>
#include "gameover.h"
#include "game.h"
#include "lang.h"
#include "gfx_gameover.h"
#include "audio.h"

static GameOverId gameover_id;
static C2D_SpriteSheet gameover_assets = NULL;
static C2D_Image img_background;
static C2D_TextBuf text_buf;
static C2D_Text text;
static const char *message_id;
static u64 init_time;
static u32 text_color;

void gameover_init(GameOverId id) {
    gameover_id = id;
    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }
    gameover_assets = C2D_SpriteSheetLoad("romfs:/gfx/gfx_gameover.t3x");
    switch (gameover_id) {
        case GAMEOVER_LASERS:
            img_background = C2D_SpriteSheetGetImage(gameover_assets, gfx_gameover_prison_idx);
            message_id = "GAMEOVER_LASERS";
            text_color = C2D_Color32(164, 164, 0, 255);
            break;
        case GAMEOVER_FALL:
            img_background = C2D_SpriteSheetGetImage(gameover_assets, gfx_gameover_fallendown_idx);
            message_id = "GAMEOVER_FALLENDOWN";
            text_color = C2D_Color32(164, 164, 0, 255);
            break;
        case GAMEOVER_TIMEUP:
            img_background = C2D_SpriteSheetGetImage(gameover_assets, gfx_gameover_timeup_idx);
            message_id = "GAMEOVER_TIMEUP";
            text_color = C2D_Color32(0, 164, 164, 255);
            break;
    }
    music_play("romfs:/audio/gameover_sad.ogg");
    init_time = osGetTime();
}

void gameover_draw_top(void) {
    C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

void gameover_draw_bottom(void) {
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get(message_id));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, 160.0f, 70.0f, 0.5f, 0.65f, 0.65f, text_color);
    u64 elapsed_time = osGetTime() - init_time;
    float y = 70 + elapsed_time / 100;
    if (y < 240) {
        C2D_DrawRectSolid(0.0f, y, 0.9f, 320.0f, 240.0f, C2D_Color32(0, 0, 0, 255));
    }
}

void gameover_close(void) {
    if (gameover_assets) {
        C2D_SpriteSheetFree(gameover_assets);
        gameover_assets = NULL;
    }
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
}
