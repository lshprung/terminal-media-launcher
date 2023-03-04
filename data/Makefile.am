EXTRA_DIST = terminal-media-launcher.ico terminal-media-launcher.svg

# Build a .desktop entry
if DESKTOP_ENTRY

CLEANFILES = $(PACKAGE_NAME).desktop

edit = sed \
	   -e 's|@PACKAGE_NAME[@]|$(PACKAGE_NAME)|g' \
	   -e 's|@bindir[@]|$(bindir)|g' \
	   -e 's|@EXEEXT[@]|$(EXEEXT)|g'

$(PACKAGE_NAME).desktop: Makefile
	rm -f $@ $@.tmp
	srcdir=''; \
		   test -f ./$@.in || srcdir=$(srcdir)/; \
		   $(edit) $${srcdir}$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

$(PACKAGE_NAME).desktop: $(srcdir)/$(PACKAGE_NAME).desktop.in

desktopdir = $(datadir)/applications
icondir    = $(datadir)/pixmaps
desktop_DATA = $(PACKAGE_NAME).desktop
icon_DATA = $(PACKAGE_NAME).svg

endif
