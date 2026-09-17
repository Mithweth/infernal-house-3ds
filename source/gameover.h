// gameover.h
#pragma once

typedef enum {
    GAMEOVER_LASERS,
    GAMEOVER_TIMEUP
} GameOverId;

void gameover_init(GameOverId id);
void gameover_draw_top(void);
void gameover_draw_bottom(void);
void gameover_close(void);