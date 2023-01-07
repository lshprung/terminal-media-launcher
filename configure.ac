AC_INIT([terminal-media-launcher], [0.1])
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
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir strrchr])

# Check for curses with color support
AX_WITH_CURSES
if test "x$ax_cv_curses" != xyes || test "x$ax_cv_curses_color" != xyes; then
	AC_MSG_ERROR([requires an X/Open-compatible Curses library with color])
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
