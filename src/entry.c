#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/entry.h"
#include "include/read_cfg.h"

typedef struct entry{
	char name[BUF_LEN];
	char path[BUF_LEN];
	bool path_force;
} ENTRY;

ENTRY *create_entry(const char *new_name, const char *new_path, const bool force){
	ENTRY *new;

	new = malloc(sizeof(ENTRY));

	strcpy(new->name, new_name);
	strcpy(new->path, new_path);
	new->path_force = force;

	return new;
}

char *get_ename(ENTRY *e){
	assert(e != NULL);
	return e->name;
}

char *get_epath(ENTRY *e){
	assert(e != NULL);
	return e->path;
}

bool get_eforce(ENTRY *e){
	assert(e != NULL);
	return e->path_force;
}

/*
void entry_debug(ENTRY *trav){

	while(trav != NULL){
		printf("%s, \n", trav->name);
		trav = trav->next;
	}

	return;
}
*/
