AUTOMAKE_OPTIONS = subdir-objects
EXTRA_DIST = include unix windows

if WINDOWS
PLATFORM=windows
else
PLATFORM=unix
endif

bin_PROGRAMS = terminal_media_launcher
terminal_media_launcher_SOURCES = cache.c draw.c read_cfg.c group.c entry.c $(PLATFORM)/cache.c $(PLATFORM)/draw.c $(PLATFORM)/read_cfg.c
terminal_media_launcher_LDADD = @CURSES_LIBS@
terminal_media_launcher_CPPFLAGS = @CURSES_CFLAGS@
