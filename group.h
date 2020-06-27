#ifndef GROUP_H
#define GROUP_H

typedef struct group GROUP;

GROUP *create_group(char *new_name);

void group_add(char *gname, ENTRY *addme);

GROUP **get_groups();

char *get_gname(GROUP *g);

char *get_gprog(GROUP *g);

void set_gprog(GROUP *g, char *p);

ENTRY *get_ghead(GROUP *g);

int get_ecount(GROUP *g);

int get_gcount();

void group_debug();

#endif 
