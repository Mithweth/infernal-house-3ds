// measure.c
#include <3ds.h>
#include <math.h>
#include "game.h"
#include "lang.h"

#define DISTANCE 3.08f
#define TARGET_X 292.0f
#define TARGET_Y 184.0f

static C2D_TextBuf text_buf;
static C2D_Text text;
static touchPosition start;
static touchPosition current;

void measure_init(float x, float y) {
    if (!text_buf) {
        text_buf = C2D_TextBufNew(1024);
    }
    start.px = x;
    start.py = y;
    current.px = x;
    current.py = y;
}

void measure_update(u32 keys, touchPosition touch) {
	if ((keys & KEY_A) || (keys & KEY_B)) {
	    game_stop_measure();
	    return;
	}
    if ((touch.px == 0) && (touch.py == 0)) {
        return;
    }
	current = touch;
}

static float measure_distance(float x, float y) {
    float dx = x - start.px;
    float dy = y - start.py;

    float perspective = 1.0f + (start.py - y) * 0.01f;

    float len = sqrtf(dx * perspective * dx * perspective + dy * dy);

    const float target_dx = TARGET_X - start.px;
    const float target_dy = TARGET_Y - start.py;
    const float target_perspective = 1.0f + (start.py - TARGET_Y) * 0.01f;

    const float target_pixels = sqrtf(target_dx * target_perspective * target_dx * target_perspective + target_dy * target_dy);

    return len * DISTANCE / target_pixels;
}

void measure_draw(void) {
    float meters = measure_distance(current.px, current.py);
    char buf[64];
    snprintf(buf, sizeof(buf), "%s %.2f %s", lang_get("MEASURE_PREFIX"), meters, lang_get("MEASURE_SUFFIX"));
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawRectSolid(10.0f, 10.0f, 0.8f, 300.0f, 35, C2D_Color32(0, 0, 0, 180));
    C2D_DrawText(&text, C2D_WithColor, 20.0f, 20.0f, 0.9f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));

    float dx = current.px - start.px;
    float dy = current.py - start.py;

    float perspective = 1.0f + (start.py - current.py) * 0.01f;

    float len = sqrtf(dx * perspective * dx * perspective +dy * dy);
    
    if (len < 1.0f) {
        return;
    }

    float ux = dx / len;
    float uy = dy / len;

    float nx = -uy;
    float ny = ux;

    C2D_DrawLine(start.px, start.py, C2D_Color32(40, 30, 0, 255), current.px, current.py, C2D_Color32(40, 30, 0, 255), 7.0f, 0.6f);
    C2D_DrawLine(start.px, start.py, C2D_Color32(240, 200, 40, 255), current.px, current.py, C2D_Color32(240, 200, 40, 255), 5.0f, 0.61f);

    for (float d = 8.0f; d < len; d += 8.0f) {
        float x = start.px + ux * d;
        float y = start.py + uy * d;
        float tick = 2.0f;
        C2D_DrawLine(x - nx * tick, y - ny * tick, C2D_Color32(40, 30, 0, 255), x + nx * tick, y + ny * tick, C2D_Color32(40, 30, 0, 255), 1.0f, 0.62f);
    }
}

void measure_close(void) {
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
}

