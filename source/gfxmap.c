// gfxmap.c
#include <citro2d.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfxmap.h"


#define GFX_MAX_IMAGES 128
static GfxImageIndex image_indexes[GFX_MAX_IMAGES];
static size_t image_index_count = 0;

int gfxmap_get_index(const char *name) {
	char index_name[256];
	snprintf(index_name, sizeof(index_name), "gfx_%s_idx", name);
    for (size_t i = 0; i < image_index_count; i++) {
        if (strcmp(image_indexes[i].name, index_name) == 0) {
            return image_indexes[i].index;
        }
    }
    return -1;
}

C2D_Image gfxmap_get_image(C2D_SpriteSheet assets, const char *name) {
    int index = gfxmap_get_index(name);

    if (index < 0) {
    	printf("Image not found: %s\n", name);
        return (C2D_Image){0};
    }
    return C2D_SpriteSheetGetImage(assets, index);
}

bool gfxmap_load(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Cannot open %s\n", filename);
        return false;
    }

    image_index_count = 0;

    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char directive[32];
        char name[GFX_NAME_MAX];
        char value[32];

        if (sscanf(line, "%31s %95s %31s", directive, name, value) != 3) {
            continue;
        }

        if (strcmp(directive, "#define") != 0) {
            continue;
        }

        size_t len = strlen(name);

        if (len < 4 || strcmp(name + len - 4, "_idx") != 0) {
            continue;
        }
        int index = atoi(value);

        if (image_index_count >= GFX_MAX_IMAGES) {
            printf("Too many images in %s\n", filename);
            fclose(f);
            return false;
        }

        GfxImageIndex *entry = &image_indexes[image_index_count++];

        strcpy(entry->name, name);
        entry->index = index;
    }

    fclose(f);
    return true;
}

bool gfxmap_load_assets(const char *path, C2D_SpriteSheet *assets) {
    char filename[256];

    snprintf(filename, sizeof(filename), "%s/gfx.t3x", path);

    *assets = C2D_SpriteSheetLoad(filename);
    if (!*assets) {
        printf("Cannot load spritesheet: %s\n", filename);
        return false;
    }

    snprintf(filename, sizeof(filename), "%s/gfx.h", path);

    if (!gfxmap_load(filename)) {
        printf("Cannot load gfx headers: %s\n", filename);
        C2D_SpriteSheetFree(*assets);
        *assets = NULL;
        return false;
    }

    return true;
}