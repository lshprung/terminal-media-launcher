AC_INIT([terminal-media-launcher],[0.1])
AC_CONFIG_SRCDIR([src/draw.c])
#AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([ncurses], [initscr])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([malloc mkdir strrchr])

# Check for curses with color support
AX_WITH_CURSES
if test "x$ax_cv_curses" != xyes || test "x$ax_cv_curses_color" != xyes; then
	AC_MSG_ERROR([requires an X/Open-compatible Curses library with color])
fi

# Temporary fix for differentiating Unix and Windows builds
AC_CANONICAL_HOST

build_unix=no
build_windows=no

# Detect the target system
case "${host_os}" in
    cygwin*|mingw*)
        build_windows=yes
        ;;
    *)
        build_unix=yes
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([UNIX], [test "$build_unix" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])

AC_CONFIG_FILES([Makefile
                 src/Makefile
			     docs/Makefile
				 data/Makefile])
AC_OUTPUT

# Print helpful message
cat << EOF

** Configuration summary for ${PACKAGE_NAME} ${PACKAGE_VERSION}

      Prefix: ${prefix}
    Compiler: ${CC} ${CFLAGS} ${CPPFLAGS}
   Libraries: ${LIBS}
EOF
