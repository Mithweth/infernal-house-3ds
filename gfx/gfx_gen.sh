#!/bin/bash -eu

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
GFX=$1
mkdir -p $SCRIPT_DIR/$GFX 

echo "--atlas -f rgba8888 -z auto" > $SCRIPT_DIR/$GFX/gfx_$GFX.t3s
for i in $SCRIPT_DIR/$GFX/*.png; do basename "$i" >> $SCRIPT_DIR/$GFX/gfx_$GFX.t3s; done

