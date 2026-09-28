// timeline.h
#pragma once

bool timeline_init(const char *directory);
void timeline_close();
void timeline_update(void);
void timeline_draw_bottom(void);
void timeline_draw_top(void);
