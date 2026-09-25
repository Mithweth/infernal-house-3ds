#!/bin/bash -eu

FORCE=false
while getopts "f" opt; do
	case "$opt" in
		f) FORCE=true;;
		*) echo "not implemented" >&2 && exit 1;;
	esac
done
shift $((OPTIND-1))
ROOT_DIR=$(dirname "$(cd "$(dirname "$0")" && pwd)")
NAME=$1

mkdir -p $ROOT_DIR/gfx/$NAME 
echo "--atlas -f rgba8888 -z auto" > $ROOT_DIR/gfx/$NAME/gfx_$NAME.t3s

for i in $ROOT_DIR/gfx/$NAME/*.png; do
	basename "$i" >> $ROOT_DIR/gfx/$NAME/gfx_$NAME.t3s
done

if [ ! -f $ROOT_DIR/source/rooms/room_$NAME.c ] || $FORCE; then
	cat << EOF > "$ROOT_DIR/source/rooms/room_$NAME.c"
// room_$NAME.c

#include <citro2d.h>
#include "game.h"
#include "room_$NAME.h"
#include "gfx_$NAME.h"
#include "inventory.h"
#include "gamestate.h"

static C2D_SpriteSheet room_scene;
EOF
	for i in $ROOT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "static C2D_Image img_$img;" >> "$ROOT_DIR/source/rooms/room_$NAME.c"
	done
	cat << EOF >> "$ROOT_DIR/source/rooms/room_$NAME.c"

static Hotspot hotspots[] = {};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_$NAME.t3x");
EOF
	for i in $ROOT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "    img_${img} = C2D_SpriteSheetGetImage(room_scene, gfx_${NAME}_${img}_idx);" >> "$ROOT_DIR/source/rooms/room_$NAME.c"
	done
	cat << "EOF" >> "$ROOT_DIR/source/rooms/room_$NAME.c"
}

static void room_draw(void) {
EOF
	for i in $ROOT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "    C2D_DrawImageAt(img_$img, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);" >> "$ROOT_DIR/source/rooms/room_$NAME.c"
	done
	cat << EOF >> "$ROOT_DIR/source/rooms/room_$NAME.c"
}

static void room_close(void) {
    C2D_SpriteSheetFree(room_scene);
}

Room $NAME = {
    .hotspots = hotspots,
    .hotspot_count = sizeof(hotspots) / sizeof(hotspots[0]),
    .north = NULL,
    .northeast = NULL,
    .east = NULL,
    .southeast = NULL,
    .south = NULL,
    .southwest = NULL,
    .west = NULL,
    .northwest = NULL,
    .init = room_init,
    .draw = room_draw,
    .close = room_close
};
EOF
	cat << EOF > "$ROOT_DIR/source/rooms/room_$NAME.h"
// room_$NAME.h

#pragma once

#include "game.h"

extern Room $NAME;
EOF
fi
