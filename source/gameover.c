// gameover.c

#include <citro2d.h>
#include "gameover.h"
#include "game.h"
#include "lang.h"
#include "gfx_gameover.h"

static GameOverId gameover_id;
static C2D_SpriteSheet gameover_assets = NULL;
static C2D_Image img_background;
static C2D_TextBuf text_buf;
static C2D_Text text;
static const char *message_id;

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
            break;
        case GAMEOVER_TIMEUP:
        	break;
    }
}

void gameover_draw_top(void) {
	C2D_DrawImageAt(img_background, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
}

void gameover_draw_bottom(void) {
	C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, lang_get(message_id));
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor | C2D_AlignCenter, 160.0f, 70.0f, 0.5f, 0.65f, 0.65f, C2D_Color32(164, 164, 0, 255));
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
