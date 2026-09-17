// gameover.c
#include "gameover.h"
#include "game.h"

static GameOverId gameover_id;

void gameover_set(GameOverId id) {
    gameover_id = id;
}

void gameover_draw(void) {
    switch (gameover_id) {
        case GAMEOVER_LASERS:
            break;
        case GAMEOVER_WHATEVER:
        	break;
    }
}

void gameover_init(void) {

}
