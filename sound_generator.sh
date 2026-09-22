#!/bin/bash -eu

WAV=$1
RAW=$(basename "$WAV" .wav)

ffmpeg -i "$WAV" -ac 1 -ar 22050 -f s16le "romfs/audio/$RAW.raw"
