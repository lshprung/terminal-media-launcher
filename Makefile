CC     = gcc
PROGS  = tml

all: $(PROGS)

clean:; $(RM) $(PROGS) *.o core

tml:		draw.o read_cfg.o entry.o group.o
		$(CC) -o tml draw.o read_cfg.o entry.o group.o -lncurses
