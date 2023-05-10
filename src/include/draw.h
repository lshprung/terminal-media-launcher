#ifndef DRAW_H
#define DRAW_H

//These functions are needed by either unix/draw.c or windows/draw.c and must be supplied via a header file

#define BUF_LEN 1024

extern int g_hover;
extern int *e_hover;
extern struct group **g;
extern struct entry **e;

char *get_launch();

//functions that differ between os
void launch();

#endif
