// secret_code.c

#include <citro2d.h>
#include <stdlib.h>
#include "secret_code.h"

static uint8_t secret_code[4];

static C2D_Text text;
static C2D_TextBuf text_buf;


void secret_code_init(void) {
    for (size_t i = 0; i < 4; i++) {
        secret_code[i] = rand() % 10;
    }
    if (!text_buf) {
        text_buf = C2D_TextBufNew(32);
    }
}

const uint8_t *secret_code_get(void) {
    return secret_code;
}

void secret_code_draw(void) {
    char code[5];
    code[0] = '0' + secret_code[0];
    code[1] = '0' + secret_code[1];
    code[2] = '0' + secret_code[2];
    code[3] = '0' + secret_code[3];
    code[4] = '\0';
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text, text_buf, code);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 40.0f, 100.0f, 0.9f, 0.55f, 0.55f, C2D_Color32(192, 192, 192, 255));
}