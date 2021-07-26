CC = gcc
NAME = tml
LIBS = -lncurses
PREFIX = /usr/local


ifeq ($(OS),Windows_NT)

$(NAME): cache.o draw.o read_cfg.o group.o entry.o windows/cache.o windows/draw.o windows/read_cfg.o
	$(CC) -o $(NAME) cache.o draw.o read_cfg.o group.o entry.o windows/cache.o windows/draw.o windows/read_cfg.o $(LIBS)

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
	rm -f $(NAME)
	rm -f $(NAME).exe

ifneq ($(OS),Windows_NT) 

.PHONY: desktop-entry
desktop-entry:
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	echo "[Desktop Entry]" > $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Type=Application" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Name=$(NAME)" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Comment=Terminal Media Launcher" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Path=$(DESTDIR)$(PREFIX)/bin" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Exec=$(NAME)" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Icon=$(NAME)" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Terminal=true" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	echo "Categories=Utility" >> $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps
	cp -i icon/icon.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/tml.svg
	desktop-file-install --dir=$(DESTDIR)$(PREFIX)/share/applications $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	update-desktop-database $(DESTDIR)$(PREFIX)/share/applications

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -i $(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -i docs/$(NAME).1.gz $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man5
	cp -i docs/$(NAME)-config.5.gz $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	rm -f $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz
	rm -f $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	rm -f $(DESTDIR)$(PREFIX)/share/applications/$(NAME).desktop
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/$(NAME).svg
	update-desktop-database $(DESTDIR)$(PREFIX)/share/applications

endif
