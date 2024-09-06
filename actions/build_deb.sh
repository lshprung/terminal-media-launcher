#!/usr/bin/env sh

print_help() {
	echo "Usage: $0 IMAGE_NAME IMAGE_VERSION ARCHIVE_FILE [DEBUILD_ARGS]..."
}

DEPENDENCY_LIST="autoconf-archive debmake devscripts liblua5.1-0-dev lua5.1 ncurses-dev pkg-config"

# print help if not enough arguments were passed
if [ $# -lt 3 ]; then
	print_help
	exit 1
fi

IMAGE_NAME="$1"
shift
IMAGE_VERSION="$1"
shift
ARCHIVE_FILE="$1"
shift

apt-get -y update
apt-get -y upgrade
apt install -y $DEPENDENCY_LIST
debmake -y -a "$ARCHIVE_FILE" -i debuild "$@"
