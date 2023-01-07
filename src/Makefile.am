AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = terminal_media_launcher
terminal_media_launcher_SOURCES = cache.c draw.c read_cfg.c group.c entry.c unix/cache.c unix/draw.c unix/read_cfg.c
terminal_media_launcher_LDADD = @CURSES_LIBS@
terminal_media_launcher_CPPFLAGS = @CURSES_CFLAGS@
