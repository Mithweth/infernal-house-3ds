// secret_code.h
#pragma once

#include <stdint.h>

void secret_code_init(void);
void secret_code_draw(void);
const uint8_t *secret_code_get(void);
