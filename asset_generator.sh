#!/bin/bash -eu

FORCE=false
while getopts "f" opt; do
	case "$opt" in
		f) FORCE=true;;
		*) echo "not implemented" >&2 && exit 1;;
	esac
done
shift $((OPTIND-1))

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
NAME=$1

mkdir -p $SCRIPT_DIR/gfx/$NAME 
echo "--atlas -f rgba8888 -z auto" > $SCRIPT_DIR/gfx/$NAME/gfx_$NAME.t3s

for i in $SCRIPT_DIR/gfx/$NAME/*.png; do
	basename "$i" >> $SCRIPT_DIR/gfx/$NAME/gfx_$NAME.t3s
done

if [ ! -f $SCRIPT_DIR/source/rooms/room_$NAME.c ] || $FORCE; then
	cat << EOF > "$SCRIPT_DIR/source/rooms/room_$NAME.c"
// room_$NAME.c

#include <citro2d.h>
#include "game.h"
#include "room_$NAME.h"
#include "gfx_$NAME.h"
#include "inventory.h"

static C2D_SpriteSheet room_scene;
EOF
	for i in $SCRIPT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "static C2D_Image img_$img;" >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"
	done
	cat << EOF >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"

static Hotspot hotspots[] = {};

static void room_init(void) {
    room_scene = C2D_SpriteSheetLoad("romfs:/gfx/gfx_$NAME.t3x");
EOF
	for i in $SCRIPT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "    img_${img} = C2D_SpriteSheetGetImage(room_scene, gfx_${NAME}_${img}_idx);" >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"
	done
	cat << "EOF" >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"
}

static void room_draw(void) {
EOF
	for i in $SCRIPT_DIR/gfx/$NAME/*.png; do
		img=$(basename $i .png)
		echo "    C2D_DrawImageAt(img_$img, 0.0f, 0.0f, 0.0f, NULL, 1.0f, 1.0f);" >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"
	done
	cat << EOF >> "$SCRIPT_DIR/source/rooms/room_$NAME.c"
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
	cat << EOF > "$SCRIPT_DIR/source/rooms/room_$NAME.h"
// room_$NAME.h

#pragma once

#include "game.h"

extern Room $NAME;
EOF
fi
