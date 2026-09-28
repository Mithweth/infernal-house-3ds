#!/bin/bash

set -euo pipefail

DEBUG=0
IMAGE="devkitpro/devkitarm:latest"

docker_make() {
    docker run --rm \
        -v "$PWD:/work" \
        -w /work \
        "$IMAGE" \
        make "$@"
}

clean() {
    docker_make clean
}

build() {
    docker_make DEBUG="$DEBUG"
}

lint() {
    docker_make lint
}

install() {
    if [ -z "${NITRO_IP:-}" ]; then
        echo "NITRO_IP is not set, skipping install"
        return
    fi

    if ! command -v 3dslink &>/dev/null; then
        echo "3dslink not found, skipping install"
        return
    fi

    local args=(-a "$NITRO_IP")

    if [ "$DEBUG" = "1" ]; then
        args+=(-s)
    fi

    until 3dslink "${args[@]}" work.3dsx; do
        echo "3DS not reachable, retrying in 5 seconds..."
        sleep 5
    done
}

all() {
    clean
    build
    install
}

run_command() {
    case "$1" in
        clean)   clean ;;
        build)   build ;;
        lint)    lint ;;
        install) install ;;
        all)     all ;;
        *)
            echo "Unknown command: $1" >&2
            echo "Usage: $0 [-d] [clean|build|lint|install|all]..." >&2
            exit 1
            ;;
    esac
}

if [ "${1:-}" = "-d" ]; then
    DEBUG=1
    shift
fi

for command in "${@:-all}"; do
    run_command "$command"
done
