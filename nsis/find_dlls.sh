#!/usr/bin/env sh

###############################################################################
# Script to determine dll dependencies of a Windows executable file.          #
# This should help automate the process of locating dlls to include as part   #
# of an installer.                                                            #
#                                                                             #
# This script is licensed under the GPLv3+                                    #
# Written by Louie Shprung <lshprung@tutanota.com>                            #
###############################################################################

help() {
	echo "Usage: $0 [OPTION]... BINARY..."
	echo "Determine dlls required by BINARY (an .exe or .dll file)"
	echo
	echo "Options:"
	echo "  -h         display this help message and exit"
	echo "  -q         hide warnings"
	echo "  -s PATH    set a path to search for dlls on. Default is \$PATH"
}

get_dll_path() {
	(
	IFS=':'
	for path in $SEARCHPATH; do
		if [ -e "$path/$1" ]; then
			echo "$path/$1"
			return 0
		fi
	done
	echo "Warning: Could not locate '$1' on system"
	return 1
	)
}

print_dlls() {
	while [ -n "$1" ]; do
		DLL_NAMES=$(objdump -x "$1" | grep "DLL Name: " | sed 's/^[^D]*DLL Name: //')
		for file in $DLL_NAMES; do
			if DLL_PATH="$(get_dll_path "$file")"; then
				echo "$DLL_PATH"
				print_dlls "$DLL_PATH"
			elif [ "$WARNINGS" -eq 1 ]; then
				echo "$DLL_PATH"
			fi
		done
		shift
	done
}

SEARCHPATH="$PATH"
WARNINGS=1

# Check args
while getopts "hqs:" flag; do
	case "$flag" in
		h)
			help
			exit
			;;
		q)
			WARNINGS=0
			;;
		s)
			SEARCHPATH="${OPTARG}"
			;;
		*)
			break
	esac
done
shift $((OPTIND-1))

if [ -z "$1" ]; then
	help
	exit 1
fi

if [ ! -e "$1" ]; then
	>&2 echo "Error: '$1' does not exist - skipping"
fi

print_dlls "$@"
