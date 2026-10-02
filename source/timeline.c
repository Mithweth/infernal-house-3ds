// timeline.c

#include <citro2d.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "audio.h"
#include "lang.h"
#include "game.h"
#include "gfxmap.h"

#define TIMELINE_MAX_EVENTS       256
#define TIMELINE_MAX_SPRITES       16

#define TIMELINE_CHAR_DELAY      100
#define TIMELINE_CHAR_DELAY_FAST  33

typedef enum {
    TIMELINE_TEXT,
    TIMELINE_PAUSE,
    TIMELINE_MUSIC_START,
    TIMELINE_MUSIC_STOP,
    TIMELINE_PLAY_SOUND,
    TIMELINE_IMAGE_LEFT,
    TIMELINE_IMAGE_CENTER,
    TIMELINE_IMAGE_RIGHT,
    TIMELINE_END_SCENE,
    TIMELINE_FULL_SCREEN,
    TIMELINE_END
} TimelineEventType;

typedef enum {
    TIMELINE_COLOR_WHITE,
    TIMELINE_COLOR_RED,
    TIMELINE_COLOR_BLUE,
    TIMELINE_COLOR_YELLOW,
    TIMELINE_COLOR_GREEN
} TimelineTextColor;

typedef struct {
    C2D_Image image;
    float x;
    float y;
} TimelineSprite;

typedef struct {
    TimelineEventType type;
    char *text;
    u32 duration;
    TimelineTextColor color;
    C2D_Image image;
    TimelineSprite sprites[TIMELINE_MAX_SPRITES];
    size_t sprite_count;
    char *sound;
} TimelineEvent;

static size_t event_pos = 0;
static size_t text_position = 0;
static char current_str[2048];
static char previous_str[2048];
static C2D_Text text_current;
static C2D_Text text_previous;
static C2D_TextBuf text_buf;
static u32 current_color;

static size_t current_event;
static u64 next_char_time;
static u64 pause_start;
static const TimelineEvent *active_full_screen = NULL;
static C2D_Image image_left;
static C2D_Image image_center;
static C2D_Image image_right;
static C2D_SpriteSheet timeline_assets;
static TimelineEvent events[TIMELINE_MAX_EVENTS];
static size_t event_count = 0;


static char *trim(char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    char *end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    return str;
}

static TimelineTextColor parse_color(const char *str) {
    if (strcmp(str, "WHITE") == 0) {
        return TIMELINE_COLOR_WHITE;
    }
    if (strcmp(str, "RED") == 0) {
        return TIMELINE_COLOR_RED;
    }
    if (strcmp(str, "BLUE") == 0) {
        return TIMELINE_COLOR_BLUE;
    }
    if (strcmp(str, "GREEN") == 0) {
        return TIMELINE_COLOR_GREEN;
    }
    if (strcmp(str, "YELLOW") == 0) {
        return TIMELINE_COLOR_YELLOW;
    }
    return TIMELINE_COLOR_WHITE;
}

static TimelineEvent *add_event(TimelineEventType type) {
    if (event_count >= TIMELINE_MAX_EVENTS) {
        return NULL;
    }
    TimelineEvent *event = &events[event_count++];

    memset(event, 0, sizeof(*event));
    event->type = type;

    return event;
}

static bool load_timeline(const char *filename) {
    FILE *f = fopen(filename, "r");

    if (!f) {
        printf("Cannot open timeline: %s\n", filename);
        return false;
    }

    event_count = 0;

    char line[512];
    size_t line_number = 0;

    TimelineEvent *full_screen = NULL;

    while (fgets(line, sizeof(line), f)) {
        line_number++;
        char *p = trim(line);

        if (*p == '\0' || *p == '#') {
            continue;
        }

        char *command = strtok(p, " ");

        if (!command) {
            continue;
        }

        if (full_screen) {
            if (strcmp(command, "SPRITE") == 0) {
                char *image_name = strtok(NULL, " ");
                char *x_str      = strtok(NULL, " ");
                char *y_str      = strtok(NULL, " ");

                if (!image_name || !x_str || !y_str) {
                    printf( "%s:%zu: invalid SPRITE\n", filename, line_number);
                    fclose(f);
                    event_count = 0;
                    return false;
                }

                if (full_screen->sprite_count >= TIMELINE_MAX_SPRITES) {

                    printf("%s:%zu: too many sprites\n", filename, line_number);
                    fclose(f);
                    event_count = 0;
                    return false;
                }

                TimelineSprite *sprite = &full_screen->sprites[full_screen->sprite_count];

                sprite->image = gfxmap_get_image(timeline_assets, image_name);
                if (!sprite->image.tex) {
                    printf("%s:%zu: unknown image %s\n", filename, line_number, image_name);
                    fclose(f);
                    event_count = 0;
                    return false;
                }

                sprite->x = atoi(x_str);
                sprite->y = atoi(y_str);
                full_screen->sprite_count++;
                continue;
            }

            if (strcmp(command, "END_FULL_SCREEN") == 0) {
                full_screen = NULL;
                continue;
            }

            fclose(f);
            event_count = 0;
            return false;
        }

        if (strcmp(command, "TEXT") == 0) {
            char *color = strtok(NULL, " ");
            char *text  = strtok(NULL, " ");
            if (!color || !text) {
                printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
                fclose(f);
                event_count = 0;
                return false;
            }

            TimelineEvent *event = add_event(TIMELINE_TEXT);

            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            event->color = parse_color(color);
            event->text = strdup(text);
            continue;
        }

        if (strcmp(command, "PAUSE") == 0) {
            char *duration = strtok(NULL, " ");
            if (!duration) {
                printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
                fclose(f);
                event_count = 0;
                return false;
            }

            TimelineEvent *event = add_event(TIMELINE_PAUSE);

            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            event->duration = (u32)atoi(duration);
            continue;
        }

        if (strcmp(command, "MUSIC_START") == 0) {
            char *sound = strtok(NULL, " ");
            if (!sound) {
                printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
                fclose(f);
                event_count = 0;
                return false;
            }

            TimelineEvent *event = add_event(TIMELINE_MUSIC_START);

            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            event->sound = strdup(sound);
            continue;
        }

        if (strcmp(command, "MUSIC_STOP") == 0) {
            TimelineEvent *event = add_event(TIMELINE_MUSIC_STOP);
            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            continue;
        }

        if (strcmp(command, "PLAY_SOUND") == 0) {
            char *sound = strtok(NULL, " ");
            if (!sound) {
                printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
                fclose(f);
                event_count = 0;
                return false;
            }

            TimelineEvent *event = add_event(TIMELINE_PLAY_SOUND);

            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }

            event->sound = strdup(sound);
            continue;
        }

        if ((strcmp(command, "IMAGE_LEFT") == 0) || (strcmp(command, "IMAGE_CENTER") == 0) || (strcmp(command, "IMAGE_RIGHT") == 0)) {
            TimelineEventType image_type;
            if (strcmp(command, "IMAGE_LEFT") == 0) {
                image_type = TIMELINE_IMAGE_LEFT;
            } else if (strcmp(command, "IMAGE_CENTER") == 0) {
                image_type = TIMELINE_IMAGE_CENTER;
            } else if (strcmp(command, "IMAGE_RIGHT") == 0) {
                image_type = TIMELINE_IMAGE_RIGHT;
            }
            char *image_name = strtok(NULL, " ");
            if (!image_name) {
                printf("%s:%zu: syntax error: %s\n", filename, line_number, command);
                fclose(f);
                event_count = 0;
                return false;
            }

            TimelineEvent *event = add_event(image_type);

            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }

            if (strcmp(image_name, "NONE") == 0) {
                event->image = (C2D_Image){0};
            } else {
                event->image = gfxmap_get_image(timeline_assets, image_name);
                if (!event->image.tex) {
                    printf("%s:%zu: unknown image %s\n", filename, line_number, image_name);
                    fclose(f);
                    event_count = 0;
                    return false;
                }
            }

            continue;
        }

        if (strcmp(command, "END_SCENE") == 0) {
            TimelineEvent *event = add_event(TIMELINE_END_SCENE);
            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            continue;
        }

        if (strcmp(command, "FULL_SCREEN") == 0) {
            full_screen = add_event(TIMELINE_FULL_SCREEN);
            if (!full_screen) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }

            continue;
        }
        if (strcmp(command, "END") == 0) {
            TimelineEvent *event = add_event(TIMELINE_END);
            if (!event) {
                printf("%s:%zu: too many timeline events\n", filename, line_number);
                fclose(f);
                event_count = 0;
                return false;
            }
            fclose(f);
            return true;
        }

        printf("%s:%zu: unknown command: %s\n", filename, line_number, command);
        fclose(f);
        event_count = 0;
        return false;
    }
    fclose(f);
    return false;
}

static size_t utf8_char_size(const char *s) {
    unsigned char c = *s;

    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;

    return 1;
}

static void update_text(void) {
    C2D_TextBufClear(text_buf);
    C2D_TextParse(&text_current, text_buf, current_str);
    C2D_TextParse(&text_previous, text_buf, previous_str);
    C2D_TextOptimize(&text_current);
    C2D_TextOptimize(&text_previous);
}

void timeline_update(u32 keys) {
    if (keys & KEY_B) {
        current_event++;
        if (current_event < event_count && events[current_event].type == TIMELINE_PAUSE) {
            current_event++;
        }
        if (current_event >= event_count) {
            current_event = event_count - 1;
        }
        return;
    }
    const TimelineEvent *event = &events[current_event];
    u64 now = osGetTime();
    u32 held = hidKeysHeld();
    int pause_duration = (held & KEY_A) ? TIMELINE_CHAR_DELAY_FAST : TIMELINE_CHAR_DELAY;

    switch (event->type) {
    case TIMELINE_MUSIC_START:
        if (event->sound) {
            music_play(event->sound);
        }
        current_event++;
        break;
    case TIMELINE_MUSIC_STOP:
        music_stop();
        current_event++;
        break;
    case TIMELINE_PLAY_SOUND:
        if (event->sound) {
            sfx_play(event->sound);
        }
        current_event++;
        break;
    case TIMELINE_TEXT:
        if (now >= next_char_time) {
            const char *str = lang_get(event->text);
            switch(event->color) {
            case TIMELINE_COLOR_BLUE:
                current_color = C2D_Color32(0, 0, 164, 255);
                break;
            case TIMELINE_COLOR_RED:
                current_color = C2D_Color32(164, 0, 0, 255);
                break;
            case TIMELINE_COLOR_GREEN:
                current_color = C2D_Color32(0, 164, 0, 255);
                break;
            case TIMELINE_COLOR_YELLOW:
                current_color = C2D_Color32(164, 164, 0, 255);
                break;
            default:
                current_color = C2D_Color32(164, 164, 164, 255);
            }

            if (str[event_pos] != '\0') {
                size_t len = utf8_char_size(&str[event_pos]);
                memcpy(previous_str, current_str, text_position);
                previous_str[text_position] = '\0';
                memcpy(&current_str[text_position], &str[event_pos], len);
                text_position += len;
                current_str[text_position] = '\0';
                event_pos += len;
                next_char_time = now + pause_duration;
                update_text();
            } else {
                event_pos = 0;
                current_event++;
            }
        }
        break;

    case TIMELINE_PAUSE:
        if (event->duration == 0) {
            break;
        }
        if (pause_start == 0) {
            pause_start = now;
        }
        if (now - pause_start >= event->duration * pause_duration / TIMELINE_CHAR_DELAY) {
            pause_start = 0;
            current_event++;
        }
        break;

    case TIMELINE_IMAGE_LEFT:
        image_left = (event->image.tex) ? event->image : (C2D_Image){0};
        current_event++;
        break;

    case TIMELINE_IMAGE_CENTER:
        image_center = (event->image.tex) ? event->image : (C2D_Image){0};
        current_event++;
        break;

    case TIMELINE_IMAGE_RIGHT:
        image_right = (event->image.tex) ? event->image : (C2D_Image){0};
        current_event++;
        break;

    case TIMELINE_END_SCENE:
        image_left = (C2D_Image){0};
        image_center = (C2D_Image){0};
        image_right = (C2D_Image){0};
        active_full_screen = NULL;
        current_str[0] = '\0';
        previous_str[0] = '\0';
        text_position = 0;
        event_pos = 0;
        update_text();
        current_event++;
        break;

    case TIMELINE_FULL_SCREEN:
        active_full_screen = event;
        current_event++;
        break;

    case TIMELINE_END:
        game_init();
        return;
    }
}

void timeline_draw_bottom(void) {
   if (active_full_screen) {
        for (int i = 0; i < active_full_screen->sprite_count; i++) {
            float z = i * 0.01f;
            const TimelineSprite *sprite = &active_full_screen->sprites[i];
            if ((sprite->image.tex) && (sprite->y >= 240.0f)) {
                C2D_DrawImageAt(sprite->image, sprite->x, sprite->y - 240, z, NULL, 1.0f, 1.0f);
            }
        }
        return;
    }
    C2D_DrawText(&text_current, C2D_WithColor, 7.0f, 70.0f, 0.0f, 0.6f, 0.65f, C2D_Color32(192, 192, 192, 255));
    C2D_DrawText(&text_previous, C2D_WithColor, 7.0f, 70.0f, 0.01f, 0.6f, 0.65f, current_color);
}

void timeline_draw_top(void) {
    if (active_full_screen) {
        for (int i = 0; i < active_full_screen->sprite_count; i++) {
            float z = i * 0.01f;
            const TimelineSprite *sprite = &active_full_screen->sprites[i];
            if ((sprite->image.tex) && (sprite->y < 240.0f)) {
                C2D_DrawImageAt(sprite->image, sprite->x + 40.0f, sprite->y, z, NULL, 1.0f, 1.0f);
            }
        }
        return;
    }
    if (image_left.tex) {
        C2D_DrawImageAt(image_left, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
    if (image_center.tex) {
        C2D_DrawImageAt(image_center, 133.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
    if (image_right.tex) {
        C2D_DrawImageAt(image_right, 266.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);
    }
}

bool timeline_init(const char *directory) {
    char script_path[256];
    char gfx_path[256];
    char header_path[256];

    snprintf(script_path, sizeof(script_path), "%s/timeline", directory);
    snprintf(gfx_path, sizeof(gfx_path), "%s/gfx.t3x", directory);
    snprintf(header_path, sizeof(header_path), "%s/gfx.h", directory);
    printf("starting timeline: %s\n", script_path);
    timeline_assets = C2D_SpriteSheetLoad(gfx_path);

    if (!text_buf) {
        text_buf = C2D_TextBufNew(4096);
    }
    if (!timeline_assets) {
        printf("Cannot load: %s\n", gfx_path);
        return false;
    }

    if (!gfxmap_load(header_path)) {
        printf("Cannot load gfx headers: %s\n", header_path);
        C2D_SpriteSheetFree(timeline_assets);
        timeline_assets = NULL;
        return false;
    }

    if (!load_timeline(script_path)) {
        printf("Cannot load timeline: %s\n", script_path);
        C2D_SpriteSheetFree(timeline_assets);
        timeline_assets = NULL;
        return false;
    }

    active_full_screen = NULL;
    current_event = 0;
    event_pos = 0;
    text_position = 0;
    current_str[0] = '\0';
    previous_str[0] = '\0';
    image_left = (C2D_Image){0};
    image_center = (C2D_Image){0};
    image_right = (C2D_Image){0};
    pause_start = 0;
    current_color = C2D_Color32(164, 164, 164, 255);
    next_char_time = osGetTime();
    update_text();
    return true;
}

void timeline_close(void) {
    for (size_t i = 0; i < event_count; i++) {
        free(events[i].text);
        free(events[i].sound);
    }

    event_count = 0;
    if (timeline_assets) {
        C2D_SpriteSheetFree(timeline_assets);
        timeline_assets = NULL;
    }
    if (text_buf) {
        C2D_TextBufDelete(text_buf);
        text_buf = NULL;
    }
    music_stop();
}
