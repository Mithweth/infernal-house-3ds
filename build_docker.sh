#!/bin/bash -eu

docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make clean all

if [ -n "${NITRO_IP:-}" ] && command -v 3dslink &>/dev/null; then
	3dslink -a "${NITRO_IP}" work.3dsx
fi
