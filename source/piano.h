// piano.h

#include <3ds.h>

void piano_init(void);
void piano_update(u32 keys, touchPosition touch);
void piano_draw_top(void);
void piano_draw_bottom(void);
void piano_close(void);
