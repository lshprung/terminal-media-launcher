#include <stdbool.h>

#ifndef ENTRY_H
#define ENTRY_H

typedef struct entry ENTRY;

ENTRY *create_entry(const char *new_name, const char *new_path, const bool force);

char *get_ename(ENTRY *e);

char *get_epath(ENTRY *e);

bool get_eforce(ENTRY *e);

#endif
