// lang.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang.h"

#define MAX_TRANSLATIONS 512
#define MAX_LINE_LENGTH  1024


typedef struct {
    char *key;
    char *value;
} Translation;


static Translation translations[MAX_TRANSLATIONS];
static size_t translation_count = 0;


static void unescape(char *str)
{
    char *src = str;
    char *dst = str;

    while (*src) {
        if (src[0] == '\\' && src[1] == 'n') {
            *dst++ = '\n';
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';
}


void lang_close(void) {
    for (size_t i = 0; i < translation_count; i++) {
        free(translations[i].key);
        free(translations[i].value);
    }

    translation_count = 0;
}

bool lang_init(const char *language) {
    char filename[64];
    snprintf(filename, sizeof(filename), "romfs:/lang/%s.txt", language);

    FILE *file = fopen(filename, "r");

    if (!file) {
        return false;
    }

    lang_close();

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char *separator = strchr(line, '=');

        if (!separator) {
            continue;
        }

        *separator = '\0';

        char *key = line;
        char *value = separator + 1;

        if (*key == '\0') {
            continue;
        }

        if (translation_count >= MAX_TRANSLATIONS) {
            break;
        }

        unescape(value);
        translations[translation_count].key = strdup(key);
        translations[translation_count].value = strdup(value);

        if (!translations[translation_count].key || !translations[translation_count].value) {
            free(translations[translation_count].key);
            free(translations[translation_count].value);
            fclose(file);
            lang_close();
            return false;
        }
        translation_count++;
    }

    fclose(file);
    return true;
}


const char *lang_get(const char *key) {
    for (size_t i = 0; i < translation_count; i++) {
        if (strcmp(translations[i].key, key) == 0)
            return translations[i].value;
    }
    return key;
}