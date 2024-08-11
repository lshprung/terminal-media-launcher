#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/entry.h"
#include "include/group.h"
#include "include/read_cfg.h"

typedef struct group{
	char name[BUF_LEN];
	char program[BUF_LEN];
	char flags[BUF_LEN];
	struct entry **entries;
	int entry_count;
	//bool launcher_quotes; //set by a group option whether or not the launcher should be wrapped by quotes
} GROUP;

int group_count = 0;
int total_count = 0;

GROUP *create_group(const char *new_name, const int entry_count){
	GROUP *new = malloc(sizeof(GROUP));

	strcpy(new->name, new_name); //by default, group name is equivalent to the path
	strcpy(new->program, "./");  //by default, launch an entry by executing it
	new->flags[0] = '\0';      //by default, no command line flags
	new->entries = malloc(sizeof(ENTRY *) * entry_count);
	new->entry_count = entry_count;

	group_count++;
	return new;
}

char *get_gname(GROUP *g){
	assert(g != NULL);
	return g->name;
}

char *get_gprog(GROUP *g){
	assert(g != NULL);
	return g->program;
}

void set_gprog(GROUP *g, char *p){
	assert(g != NULL);
	strcpy(g->program, p);
	return;
}

char *get_gflags(GROUP *g){
	assert(g != NULL);
	return g->flags;
}

void set_gflags(GROUP *g, char *p){
	assert(g != NULL);
	strcpy(g->flags, p);
}

ENTRY **get_gentries(GROUP *g) {
	assert(g != NULL);
	return g->entries;
}

void set_gentry(GROUP *g, int entry_index, ENTRY *new_entry) {
	assert(g != NULL);
	g->entries[entry_index] = new_entry;
}

int get_ecount(GROUP *g){
	assert(g != NULL);
	return g->entry_count;
}

void set_ecount(GROUP *g, int new_count){
	assert(g != NULL);
	g->entry_count = new_count;
}

/*
void group_debug(){
	GROUP *trav = groups_head;
	
	while(trav != NULL){
		entry_debug(trav->head);
		printf("\tfrom group %s\n", trav->name);
		trav = trav->next;
	}

	return;
}
*/
