// title.h
// Title screen: menu (language, intro, new game, controls, credits) on the
// bottom screen, artwork on the top screen. Driven by game.c while in
// GAME_TITLE.
#pragma once

// Loads the title graphics (romfs:/gfx/title) and creates the text buffer.
// Called by game_init and when the intro fails to start.
bool title_init(void);

// Handles menu navigation for this frame. Choosing "intro" or "new game" calls
// game_intro / game_start, which close the title screen.
void title_update(u32 keys);

// Draws the top screen artwork.
void title_draw_top(void);

// Draws the menu, or the controls / credits page when one is open.
void title_draw_bottom(void);

// Releases the title graphics and text buffer. Safe to call more than once.
void title_close(void);