#!/bin/bash -eu

clean() {
	docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make clean
}

build() {
	docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make
}

install() {
	if [ -n "${NITRO_IP:-}" ] && command -v 3dslink &>/dev/null; then
		3dslink -a "${NITRO_IP}" work.3dsx
	fi
}

if [ $# -eq 0 ]; then
	clean
	build
	install
fi

while [ $# -gt 0 ]; do
	case "$1" in
		clean) clean;;
		build) build;;
		install) install;;
	esac
	shift
done
