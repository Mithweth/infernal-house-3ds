// secret_code.c

#include <citro2d.h>
#include <stdlib.h>
#include "secret_code.h"

static uint8_t secret_code[4];

static C2D_Text text;
static C2D_TextBuf text_buf;


void secret_code_init(void) {
	if (!text_buf) {
		text_buf = C2D_TextBufNew(32);
	}
}

void secret_code_reset(void) {
	for (size_t i = 0; i < 4; i++) {
		secret_code[i] = rand() % 10;
	}
}

const uint8_t *secret_code_get(void) {
	return secret_code;
}

char *secret_code_serialize(void) {
	char *code = malloc(5);
	if (!code) {
		return NULL;
	}
	for (int i = 0; i < 4; i++) {
		code[i] = '0' + secret_code[i];
	}
	code[4] = '\0';
	return code;
}

void secret_code_deserialize(const char *data) {
	for (int i = 0; i < 4; i++) {
		secret_code[i] = data[i] - '0';
	}
}

void secret_code_draw(void) {
	char code[5];
	for (int i = 0; i < 4; i++) {
		code[i] = '0' + secret_code[i];
	}
	code[4] = '\0';
	C2D_TextBufClear(text_buf);
	C2D_TextParse(&text, text_buf, code);
	C2D_TextOptimize(&text);
	C2D_DrawText(&text, C2D_WithColor, 40.0f, 100.0f, 0.9f, 0.55f, 0.55f, C2D_Color32(192, 192, 192, 255));
}
