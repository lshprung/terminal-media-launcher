#ifndef GROUP_H
#define GROUP_H

#include <stdbool.h>

#include "entry.h"

typedef struct group GROUP;

GROUP *create_group(const char *new_name, const int entry_count);

char *get_gname(GROUP *g);

char *get_gprog(GROUP *g);

void set_gprog(GROUP *g, char *p);

char *get_gflags(GROUP *g);

void set_gflags(GROUP *g, char *p);

ENTRY **get_gentries(GROUP *g);

void set_gentry(GROUP *g, int entry_index, ENTRY *new_entry);

int get_ecount(GROUP *g);

void set_ecount(GROUP *g, int new_count); //for use in hiding entries

// print all group and entry information
void group_debug(GROUP *g);

#endif 
