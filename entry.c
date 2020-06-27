#include <assert.h>
#include <dirent.h>
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
	struct entry *next;
} ENTRY;

ENTRY *create_entry(char *new_name, char *new_path);
ENTRY *entry_add_last(ENTRY *tail, ENTRY *add);
ENTRY **get_entries(ENTRY *head, int count);
char *get_ename(ENTRY *e);
char *get_epath(ENTRY *e);

ENTRY *create_entry(char *new_name, char *new_path){
	ENTRY *new;

	//double check if file exists
	if(access(new_path, F_OK) == -1){
		printf("Error: Invalid File Name \"%s\"\n", new_path);
		return NULL;
	}

	new = malloc(sizeof(ENTRY));

	strcpy(new->name, new_name);
	strcpy(new->path, new_path);
	new->next = NULL;

	return new;
}

ENTRY *entry_add_last(ENTRY *tail, ENTRY *add){
	assert(add != NULL);

	if(tail == NULL) tail = add;
	else{
		tail->next = add;
		tail = add;
	}

	return tail;
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
	return e->name;
	return e->path;
}

void entry_debug(ENTRY *trav){

	while(trav != NULL){
		printf("%s, \n", trav->name);
		trav = trav->next;
	}

	return;
}