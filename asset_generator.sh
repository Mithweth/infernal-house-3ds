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

for i in $SCRIPT_DIR/gfx/$NAME/*.png; do basename "$i" >> $SCRIPT_DIR/gfx/$NAME/gfx_$NAME.t3s; done

if [ ! -f $SCRIPT_DIR/source/rooms/room_$NAME.c ] || $FORCE; then
	cp $SCRIPT_DIR/source/rooms/room.c.sample $SCRIPT_DIR/source/rooms/room_$NAME.c
	cp $SCRIPT_DIR/source/rooms/room.h.sample $SCRIPT_DIR/source/rooms/room_$NAME.h
	sed -i "s/<room_name>/$NAME/g" $SCRIPT_DIR/source/rooms/room_$NAME.*
fi
