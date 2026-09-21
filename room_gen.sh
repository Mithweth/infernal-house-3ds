#!/bin/bash -eu

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
GFX=$1
mkdir -p $SCRIPT_DIR/gfx/$GFX 

echo "--atlas -f rgba8888 -z auto" > $SCRIPT_DIR/gfx/$GFX/gfx_$GFX.t3s
for i in $SCRIPT_DIR/gfx/$GFX/*.png; do basename "$i" >> $SCRIPT_DIR/gfx/$GFX/gfx_$GFX.t3s; done
cp $SCRIPT_DIR/source/rooms/room.c.sample $SCRIPT_DIR/source/rooms/room_$GFX.c
cp $SCRIPT_DIR/source/rooms/room.h.sample $SCRIPT_DIR/source/rooms/room_$GFX.h
sed -i "s/<room_name>/$GFX/g" $SCRIPT_DIR/source/rooms/room_$GFX.*
