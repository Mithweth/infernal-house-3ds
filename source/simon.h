// simon.h
#pragma once

#include <3ds.h>

void simon_init(void);
void simon_update(u32 keys, touchPosition touch);
void simon_draw_top(void);
void simon_draw_bottom(void);
void simon_close(void);