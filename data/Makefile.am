EXTRA_DIST = terminal-media-launcher.ico terminal-media-launcher.svg

# Build a .desktop entry
if DESKTOP_ENTRY

BUILT_SOURCES = $(PACKAGE_NAME).desktop
CLEANFILES = $(PACKAGE_NAME).desktop
$(PACKAGE_NAME).desktop: Makefile
	echo "[Desktop Entry]" > $@
	echo "Type=Application" >> $@
	echo "Name=$(PACKAGE_NAME)" >> $@
	echo "Comment=Terminal Media Launcher" >> $@
	echo "Path=$(bindir)/$(PACKAGE_NAME)$(EXEEXT)" >> $@
	echo "Exec=$(PACKAGE_NAME)$(EXEEXT)" >> $@
	echo "Icon=$(PACKAGE_NAME)" >> $@
	echo "Terminal=true" >> $@
	echo "Categories=Utility" >> $@

desktopdir = $(datadir)/applications
icondir    = $(datadir)/pixmaps
desktop_DATA = $(PACKAGE_NAME).desktop
icon_DATA = $(PACKAGE_NAME).svg

endif
