// game.h
// Central game controller: owns the current GameMode and routes input and
// drawing to the title screen, timelines, mini-games or the room view.
// Also the API that rooms, inventory and extensions use to change mode
// (messages, room changes, timelines, mini-games, waiting for a sound).
#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>
#include <stddef.h>
#include "inventory.h"

// Not used anywhere at the moment.
typedef struct {
    const char *name;
    void (*callback)(void);
} GameCallbackEntry;

// A mini-game plugged into the game loop (see extensions/ and
// callbacks_minigame_find). Every callback is optional.
//   init:   called by game_minigame_start; returning false aborts the start.
//   update: called every frame with the keys pressed this frame.
//   draw:   called on the bottom screen, after the room has been drawn.
//   close:  called by game_minigame_stop; must release what init allocated.
// A mini-game ends itself by calling game_minigame_stop() from update.
typedef struct {
    bool (*init)(void);
    void (*update)(u32 keys, touchPosition touch);
    void (*draw)(void);
    void (*close)(void);
} MiniGame;

typedef enum {
    GAME_NORMAL,    // exploring a room: movement, touch and inventory input
    GAME_MESSAGE,   // a message or examine image is shown; A, B or touch dismisses it
    GAME_BUSY,      // input blocked until a sound effect ends (see game_wait_for_sfx)
    GAME_MINIGAME,  // a MiniGame receives the input
    GAME_TITLE,     // title screen
    GAME_TIMELINE   // a scripted sequence (intro, game over, ending) is playing
} GameMode;

// Per-frame update, called by main. keys are the keys pressed this frame
// (hidKeysDown); dispatches on the current GameMode.
void game_update(u32 keys, circlePosition analog, touchPosition touch);

// Per-frame drawing on both screens, called by main between
// C3D_FrameBegin and C3D_FrameEnd.
void game_draw(C3D_RenderTarget *top, C3D_RenderTarget *bottom);

// Starts the timeline in romfs:/timelines/<name> and switches to
// GAME_TIMELINE. If it cannot be loaded, falls back to the title screen.
void game_timeline_start(const char *name);

// Releases the room, the message text buffer and the HUD. Called once by main
// at exit.
void game_close(void);

// Goes (back) to the title screen: closes any timeline, stops the music and
// regenerates the extensions' state (e.g. a new secret code). Called by main
// at start-up and whenever a timeline ends. The HUD must already be
// initialized (hud_init is called once by main).
void game_init(void);

// Leaves the title screen and plays the intro timeline. Returns to the title
// screen if the intro cannot be loaded.
void game_intro(void);

// Leaves the title screen and starts a new game: resets inventory, game states
// and timer, starts the music, gives the measuring tape and enters the hall.
void game_start(void);

// Leaves the current room and loads romfs:/rooms/<name>. Clears the target,
// any pending message and switches to GAME_NORMAL. name may point into the
// current room's data: it is copied before the room is freed.
void game_set_room(const char *name);

// Id of the hotspot currently targeted (shown in the HUD), or NULL.
// Revalidates the target first, since an action may have hidden it.
const char *game_target_name(void);

// Uses inventory item id on the current target by running the target's
// matching USE block. Returns true if a USE block ran; otherwise shows
// GAME_CANNOT_USE_MESSAGE (only when there is a target) and returns false.
bool game_use_item(const char *id);

// Shows the translated message for message_id and switches to GAME_MESSAGE.
// A later mode-changing action in the same frame (room change, timeline,
// mini-game, WAIT_SFX) replaces the message before it is ever displayed.
void game_show_message(const char *message_id);

// Shows an image centered over the room and switches to GAME_MESSAGE.
// The image must stay valid until the message is dismissed.
void game_show_image(C2D_Image image);

// Starts the mini-game registered under name (see callbacks.c), stops the
// music and switches to GAME_MINIGAME. Does nothing if the name is unknown or
// its init fails (the music then stays stopped).
void game_minigame_start(const char *name);

// Closes the active mini-game, restarts the background music and switches
// back to GAME_NORMAL. Usually called by the mini-game itself.
void game_minigame_stop(void);

// Plays sfx (a full romfs path) and blocks input in GAME_BUSY until it ends,
// then calls callback (may be NULL). Returns false without changing mode if
// the sound could not be started; the caller must then continue on its own.
bool game_wait_for_sfx(const char *sfx, void (*callback)(void));
