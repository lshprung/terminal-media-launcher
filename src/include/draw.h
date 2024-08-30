#ifndef DRAW_H
#define DRAW_H

//These functions are needed by either unix/draw.c or windows/draw.c and must be supplied via a header file

#define BUF_LEN 1024

// currently selected group
extern int g_hover;
// array of currently selected entries for each group
extern int *e_hover;
// array of groups (as loaded from the config)
extern struct group **g;

// returns the command to run for the current entry
char *get_launch();

//functions that differ between os
void launch();

#endif
