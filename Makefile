CC = gcc
NAME = terminal-media-launcher
LIBS = -lncurses
PREFIX = /usr/local


ifeq ($(OS),Windows_NT)

$(NAME): cache.o draw.o read_cfg.o group.o entry.o windows/cache.o windows/draw.o windows/read_cfg.o windows/make_icon.res
	$(CC) -o $(NAME) cache.o draw.o read_cfg.o group.o entry.o windows/cache.o windows/draw.o windows/read_cfg.o windows/make_icon.res $(LIBS)

windows/make_icon.res: icon/icon.ico windows/make_icon.rc
	windres windows/make_icon.rc -O coff -o windows/make_icon.res

windows/draw.o: windows/draw.c include/draw.h 
windows/read_cfg.o: windows/read_cfg.c include/read_cfg.h 
windows/cache.o: windows/cache.c include/cache.h include/read_cfg.h

else 

$(NAME): cache.o draw.o read_cfg.o group.o entry.o unix/cache.o unix/draw.o unix/read_cfg.o
	$(CC) -o $(NAME) cache.o draw.o read_cfg.o group.o entry.o unix/cache.o unix/draw.o unix/read_cfg.o $(LIBS)

unix/draw.o: unix/draw.c include/draw.h
unix/read_cfg.o: unix/read_cfg.c include/read_cfg.h
unix/cache.o: unix/cache.c include/cache.h include/read_cfg.h

endif

cache.o: cache.c include/cache.h include/read_cfg.h
draw.o: draw.c include/cache.h include/draw.h include/entry.h include/group.h include/read_cfg.h 
read_cfg.o: read_cfg.c include/entry.h include/group.h include/read_cfg.h
group.o: group.c include/entry.h include/group.h include/read_cfg.h
entry.o: entry.c include/entry.h include/group.h include/read_cfg.h 

.PHONY: clean
clean:
	rm -f *.o
	rm -f unix/*.o
	rm -f windows/*.o
	rm -f windows/*.res
	rm -f $(NAME)
	rm -f $(NAME).exe

ifneq ($(OS),Windows_NT) 

.PHONY: desktop-entry
desktop-entry:
	echo "[Desktop Entry]" > icon/$(NAME).desktop
	echo "Type=Application" >> icon/$(NAME).desktop
	echo "Name=$(NAME)" >> icon/$(NAME).desktop
	echo "Comment=Terminal Media Launcher" >> icon/$(NAME).desktop
	echo "Path=$(DESTDIR)$(PREFIX)/bin" >> icon/$(NAME).desktop
	echo "Exec=$(NAME)" >> icon/$(NAME).desktop
	echo "Icon=$(NAME)" >> icon/$(NAME).desktop
	echo "Terminal=true" >> icon/$(NAME).desktop
	echo "Categories=Utility" >> icon/$(NAME).desktop
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps
	cp icon/icon.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/$(NAME).svg
	desktop-file-install --dir=$(DESTDIR)$(PREFIX)/share/applications icon/$(NAME).desktop
	update-desktop-database $(DESTDIR)$(PREFIX)/share/applications

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp docs/$(NAME).1.gz $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man5
	cp docs/$(NAME)-config.5.gz $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	rm -f $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz
	rm -f $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/$(NAME).svg
	update-desktop-database $(DESTDIR)$(PREFIX)/share/applications

endif
