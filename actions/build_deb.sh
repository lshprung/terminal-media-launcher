#!/usr/bin/env sh

CONTAINER_NAME="build_deb"
IMAGE_NAME="$1"

DEPENDENCY_LIST="autoconf-archive debmake devscripts ncurses-dev pkg-config"

if [ -z "$IMAGE_NAME" ]; then
	IMAGE_NAME="debian:unstable"
fi

podman pull docker.io/library/"$IMAGE_NAME"

podman run -itd --name "$CONTAINER_NAME" "$IMAGE_NAME"

make -C "$(dirname "$0")/.." dist
podman exec "$CONTAINER_NAME" mkdir -p /tmp/build
podman cp terminal-media-launcher*.tar.gz "$CONTAINER_NAME":/tmp/build

podman exec -w /tmp/build "$CONTAINER_NAME" apt-get -y update
podman exec -w /tmp/build "$CONTAINER_NAME" apt-get -y upgrade
podman exec -w /tmp/build "$CONTAINER_NAME" apt install -y $DEPENDENCY_LIST
podman exec -w /tmp/build "$CONTAINER_NAME" debmake -y -a terminal-media-launcher-0.1.tar.gz -i debuild

mkdir -p "$(dirname "$0")/build"

podman cp "$CONTAINER_NAME":/tmp/build/. "$(dirname "$0")/build/"

podman kill "$CONTAINER_NAME"
podman rm "$CONTAINER_NAME"
