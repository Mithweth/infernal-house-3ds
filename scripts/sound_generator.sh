#!/bin/bash -eu

WAV=$1
RAW=$(basename "$WAV" .wav)
ROOT_DIR=$(dirname "$(cd "$(dirname "$0")" && pwd)")

ffmpeg -i "$WAV" -ac 1 -ar 22050 -f s16le "$ROOT_DIR/romfs/audio/$RAW.raw"
