#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "entry.h"
#include "group.h"
#define BUF_LEN 1024

typedef struct entry{
	char name[BUF_LEN];
	char path[BUF_LEN];
	bool path_force;
	struct entry *next;
} ENTRY;

ENTRY *create_entry(char *new_name, char *new_path, bool force);
void entry_rm(ENTRY *e, ENTRY *prev);
void clear_entries(ENTRY *head);
ENTRY *entry_add_last(ENTRY *tail, ENTRY *add);
int entry_add_sorted(ENTRY *head, ENTRY *tail, ENTRY *add);
ENTRY **get_entries(ENTRY *head, int count);
char *get_ename(ENTRY *e);
char *get_epath(ENTRY *e);
bool get_eforce(ENTRY *e);
void entry_debug(ENTRY *trav);

ENTRY *create_entry(char *new_name, char *new_path, bool force){
	ENTRY *new;

	new = malloc(sizeof(ENTRY));

	strcpy(new->name, new_name);
	strcpy(new->path, new_path);
	new->path_force = force;
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

//TODO consider joining entry_add_last and entry_add_sorted
ENTRY *entry_add_last(ENTRY *tail, ENTRY *add){
	assert(add != NULL);

	if(tail == NULL) tail = add;
	else{
		tail->next = add;
		tail = add;
	}

	return tail;
}

//returns 0 if in the middle, 1 if new head, 2 if new tail, or 3 if both new head and tail
//TODO this is kind of a stupid way of handling things
int entry_add_sorted(ENTRY *head, ENTRY *tail, ENTRY *add){
	assert(add != NULL);
	ENTRY *ahead;

	//Empty group (no need to sort)
	if(head == NULL) return 3;

	//add is the new tail
	if(strcmp(tail->name, add->name) <= 0){
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
	int i;

	for(i = 0; i < count; i++){
		arr[i] = trav;
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

void entry_debug(ENTRY *trav){

	while(trav != NULL){
		printf("%s, \n", trav->name);
		trav = trav->next;
	}

	return;
}
