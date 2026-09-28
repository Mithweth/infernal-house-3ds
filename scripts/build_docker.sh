#!/bin/bash -e

DEBUG=0

clean() {
    docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make clean
}

build() {
    docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make DEBUG=$DEBUG
}

lint() {
    docker run --rm -v "$PWD:/work" -w /work devkitpro/devkitarm:latest make lint
}

install() {
    if [ -n "${NITRO_IP:-}" ] && command -v 3dslink &>/dev/null; then
        local args=("-a" "${NITRO_IP}")
        if [ "$DEBUG" = "1" ]; then
            args+=("-s")
        fi
        until 3dslink "${args[@]}" work.3dsx  ; do sleep 5 ; done
    fi
}

run_command() {
    case "$1" in
        clean) clean;;
        build) build;;
        lint) lint;;
        install) install;;
        *) clean && build && install;;
    esac
}

if [ "${1:-}" = "-d" ]; then
    DEBUG=1
    shift
fi

if [ $# -eq 0 ]; then
    run_command all
else
    while [ $# -gt 0 ]; do
        run_command "$1"
        shift
    done
fi
