// gameover.h
#pragma once

typedef enum {
    GAMEOVER_LASERS,
    GAMEOVER_WHATEVER
} GameOverId;

void gameover_init(void);
void gameover_set(GameOverId id);
void gameover_draw(void);
void gameover_close(void);