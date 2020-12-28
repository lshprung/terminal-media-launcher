CC = gcc
NAME = tml
LIBS = -lncurses
PREFIX = /usr/local


ifeq ($(OS),Windows_NT)

$(NAME): draw.o read_cfg.o group.o entry.o windows/draw.o windows/read_cfg.o
	$(CC) -o $(NAME) draw.o read_cfg.o group.o entry.o windows/draw.o windows/read_cfg.o $(LIBS)

draw.o: draw.c read_cfg.h group.h entry.h windows/draw.h windows/read_cfg.h
windows/draw.o: windows/draw.c windows/draw.h draw.h
read_cfg.o: read_cfg.c group.o entry.o windows/read_cfg.h
windows/read_cfg.o: windows/read_cfg.c windows/read_cfg.h read_cfg.h

else 

$(NAME): draw.o read_cfg.o group.o entry.o unix/draw.o unix/read_cfg.o
	$(CC) -o $(NAME) draw.o read_cfg.o group.o entry.o unix/draw.o unix/read_cfg.o $(LIBS)

draw.o: draw.c read_cfg.h group.h entry.h unix/draw.h unix/read_cfg.h
unix/draw.o: unix/draw.c unix/draw.h draw.h
read_cfg.o: read_cfg.c group.o entry.o unix/read_cfg.h
windows/read_cfg.o: unix/read_cfg.c unix/read_cfg.h read_cfg.h

endif

group.o: group.c group.h entry.h
entry.o: entry.c entry.h read_cfg.h group.h


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
