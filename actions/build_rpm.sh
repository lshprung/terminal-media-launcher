#!/usr/bin/env sh

print_help() {
	echo "Usage: $0 IMAGE_NAME IMAGE_VERSION SPEC_FILE"
}

DEPENDENCY_LIST="gcc lua lua-devel make ncurses-devel rpmdevtools rpmlint"

# print help if not enough arguments were passed
if [ $# -lt 3 ]; then
	print_help
	exit 1
fi

IMAGE_NAME="$1"
shift
IMAGE_VERSION="$1"
shift
SPEC_FILE="$1"
shift

yum -y update

if [ "$IMAGE_NAME" = "rockylinux" ] && [ "$IMAGE_VERSION" = "8" ]; then
	yum -y install yum-utils
	yum -y install epel-release
	if [ "$IMAGE_VERSION" = "8" ]; then
		yum -y config-manager --set-enabled powertools
	else
		yum -y config-manager --set-enabled crb
	fi
	yum -y update
fi

yum -y makecache
yum install -y $DEPENDENCY_LIST
rpmdev-setuptree
rpmbuild -ba /root/rpmbuild/SPECS/"$SPEC_FILE"
