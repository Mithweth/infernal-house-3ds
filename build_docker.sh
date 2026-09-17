#!/bin/bash -e

clean() {
	docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make clean
}

debug() {
	docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make DEBUG=1
}

build() {
	docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make
}

install() {
	if [ -n "${NITRO_IP:-}" ] && command -v 3dslink &>/dev/null; then
		3dslink -a "${NITRO_IP}" work.3dsx
	fi
}

	case "$1" in
		clean) clean;;
		build) build;;
		install) install;;
                debug) clean && debug && install;;
                *) clean && build && install;;
	esac
	shift
