// hud.h
#pragma once

#include <stdbool.h>

// Loads the HUD graphics (embedded in the executable) and initializes the
// inventory. Returns false on failure. Call once, after C2D is initialized.
bool hud_init(void);

// Restarts the countdown (new game).
void hud_reset(void);

// Updates the timer once per frame; when time runs out it plays footsteps and
// starts the "gameover_timeup" timeline.
void hud_update(void);

// Draws the whole HUD; call inside the top screen scene.
void hud_draw(void);

// Frees the HUD graphics and the inventory. Safe if hud_init failed.
void hud_close(void);

// Countdown helpers. timer_resume must be called after the application was
// suspended (home menu, sleep) so the suspended time isn't counted.
void timer_start(void);
void timer_update(void);
void timer_resume(void);
