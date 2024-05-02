#!/usr/bin/env sh

# Proof-of-concept to have a script determine dlls that need to be shipped with the installer

help() {
	echo "Usage: [OPTION]... $0 BINARY..."
	echo "Determine dlls required by BINARY (an .exe or .dll file)"
	echo
	echo "Options:"
	echo "  -h         display this help message and exit"
	echo "  -s PATH    set a path to search for dlls on. Default is \$PATH"
}

print_dlls() {
	while [ -e "$1" ]; do
		objdump -x "$1" | grep "DLL Name: " | sed 's/^[^D]*DLL Name: //'
		shift
	done
}

SEARCHPATH="$PATH"
echo "$SEARCHPATH"

# Check args
while getopts "hs:" flag; do
	case "$flag" in
		h)
			help
			exit
			;;
		s)
			SEARCHPATH="${OPTARG}"
			;;
		*)
			break
	esac
done

if [ -z "$1" ]; then
	help
	exit 1
fi

if [ ! -e "$1" ]; then
	>&2 echo "Error: '$1' does not exist" > STDERR
	exit 1
fi

print_dlls "$@"

