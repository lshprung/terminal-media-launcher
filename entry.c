#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/entry.h"
#include "include/group.h"
#include "include/read_cfg.h"

typedef struct entry{
	char name[BUF_LEN];
	char path[BUF_LEN];
	bool path_force;
	bool hidden;
	struct entry *next;
} ENTRY;

ENTRY *create_entry(char *new_name, char *new_path, bool force){
	ENTRY *new;

	new = malloc(sizeof(ENTRY));

	strcpy(new->name, new_name);
	strcpy(new->path, new_path);
	new->path_force = force;
	new->hidden = false;
	new->next = NULL;

	return new;
}

void entry_rm(ENTRY *e, ENTRY *prev){
	assert(e != NULL);
	if(prev != NULL) prev->next = e->next; //maintain linked structure
	free(e);
}

void clear_entries(ENTRY *head){
	ENTRY *temp;
	
	while(head != NULL){
		temp = head;
		head = head->next;
		free(temp);
	}

	return;
}

//returns 0 if in the middle, 1 if new head, 2 if new tail, or 3 if both new head and tail
//TODO this is kind of a stupid way of handling things
int entry_add(ENTRY *head, ENTRY *tail, ENTRY *add){
	assert(add != NULL);
	ENTRY *ahead;

	//Empty group (no need to sort)
	if(head == NULL) return 3;

	//add is the new tail
	if(!get_sort() || strcmp(tail->name, add->name) <= 0){
		tail->next = add;
		return 2;
	}

	//add is the new head
	if(strcmp(add->name, head->name) <= 0){
		add->next = head;
		return 1;
	}

	ahead = head->next;

	while(ahead != NULL){
		if(strcmp(head->name, add->name) <= 0 && strcmp(add->name, ahead->name) <= 0) break;
		head = head->next;
		ahead = ahead->next;
	}

	head->next = add;
	add->next = ahead;

	return 0;
}

ENTRY **get_entries(ENTRY *head, int count){
	ENTRY **arr = malloc(sizeof(ENTRY *) * count);
	ENTRY *trav = head;
	int i = 0;

	while(i < count){
		if(!trav->hidden){
			arr[i] = trav;
			i++;
		}
		trav = trav->next;
	}

	return arr;
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

void set_hide(ENTRY *e, bool status){
	assert(e != NULL);
	e->hidden = true;
}

void entry_debug(ENTRY *trav){

	while(trav != NULL){
		printf("%s, \n", trav->name);
		trav = trav->next;
	}

	return;
}
