// inventory.h
#pragma once

#include <citro2d.h>
#include <stdbool.h>

#define GFX_NAME_MAX  96

typedef struct {
    char name[GFX_NAME_MAX];
    int index;
} GfxImageIndex;

int gfxmap_get_index(const char *name);
C2D_Image gfxmap_get(C2D_SpriteSheet assets, const char *name);
bool gfxmap_load(const char *filename);
