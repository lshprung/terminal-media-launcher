#ifndef ENTRY_H
#define ENTRY_H

typedef struct entry ENTRY;

ENTRY *create_entry(char *new_name, char *new_path, bool force);

void entry_rm(ENTRY *e, ENTRY *prev);

void clear_entries(ENTRY *head);

ENTRY *entry_add_last(ENTRY *tail, ENTRY *add);

ENTRY **get_entries(ENTRY *head, int count);

char *get_ename(ENTRY *e);

char *get_epath(ENTRY *e);

bool get_eforce(ENTRY *e);

void entry_debug(ENTRY *trav);

#endif
