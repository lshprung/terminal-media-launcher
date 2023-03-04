AUTOMAKE_OPTIONS = subdir-objects
EXTRA_DIST = include unix windows 

bin_PROGRAMS = terminal-media-launcher
terminal_media_launcher_SOURCES = cache.c draw.c read_cfg.c group.c entry.c $(PLATFORM)/cache.c $(PLATFORM)/draw.c $(PLATFORM)/read_cfg.c
terminal_media_launcher_LDADD = @CURSES_LIBS@
terminal_media_launcher_CPPFLAGS = @CURSES_CFLAGS@

if WINDOWS
PLATFORM=windows

# Embed icon
if EMBED_ICON

terminal_media_launcher_SOURCES += icon/$(PACKAGE_NAME).rc icon/$(PACKAGE_NAME).ico
terminal_media_launcher_LDADD += icon/$(PACKAGE_NAME).res
BUILT_SOURCES = icon/$(PACKAGE_NAME).res
CLEANFILES = icon/$(PACKAGE_NAME).res

icon/$(PACKAGE_NAME).res: icon/$(PACKAGE_NAME).ico icon/$(PACKAGE_NAME).rc
	$(WINDRES) icon/$(PACKAGE_NAME).rc -O coff -o $@

endif
else
PLATFORM=unix
endif
