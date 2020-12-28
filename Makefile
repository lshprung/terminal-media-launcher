CC = gcc
NAME = tml
LIBS = -lncurses
PREFIX = /usr/local


ifeq ($(OS),Windows_NT)

$(NAME): draw.o read_cfg.o group.o entry.o windows/draw.o windows/read_cfg.o
	$(CC) -o $(NAME) draw.o read_cfg.o group.o entry.o windows/draw.o windows/read_cfg.o $(LIBS)

windows/draw.o: windows/draw.c include/draw.h include/draw_extend.h
windows/read_cfg.o: windows/read_cfg.c include/read_cfg.h include/read_cfg_extend.h 

else 

$(NAME): draw.o read_cfg.o group.o entry.o unix/draw.o unix/read_cfg.o
	$(CC) -o $(NAME) draw.o read_cfg.o group.o entry.o unix/draw.o unix/read_cfg.o $(LIBS)

unix/draw.o: unix/draw.c include/draw.h include/draw_extend.h
unix/read_cfg.o: unix/read_cfg.c include/read_cfg.h include/read_cfg_extend.h 

endif

draw.o: draw.c include/draw.h include/draw_extend.h include/entry.h include/group.h include/read_cfg.h include/read_cfg_extend.h
read_cfg.o: read_cfg.c include/entry.h include/group.h include/read_cfg_extend.h
group.o: group.c include/entry.h include/group.h include/read_cfg.h
entry.o: entry.c include/entry.h include/group.h include/read_cfg.h 


.PHONY: clean
clean:
	rm *.o $(NAME)

ifneq ($(OS),Windows_NT) 

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -i $(NAME) $(DESTDIR)$(PREFIX)/bin/$(NAME)
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -i docs/$(NAME).1.gz $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man5
	cp -i docs/$(NAME)-config.5.gz $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz

.PHONY: install
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/$(NAME).1.gz
	rm -f $(DESTDIR)$(PREFIX)/share/man/man5/$(NAME)-config.5.gz

endif
