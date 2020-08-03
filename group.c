#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "entry.h"
#include "group.h"
#define BUF_LEN 1024

typedef struct group{
	char name[BUF_LEN];
	char program[BUF_LEN];
	char flags[BUF_LEN];
	struct entry *head;
	struct entry *tail;
	struct group *next;
	int entry_count;
} GROUP;

GROUP *create_group(char *new_name);
void group_add(char *gname, ENTRY *addme);
void group_rm(GROUP *g);
void clean_groups(); //remove empty groups from linked list
GROUP **get_groups();
char *get_gname(GROUP *g);
char *get_gprog(GROUP *g);
void set_gprog(GROUP *g, char *p);
char *get_gflags(GROUP *g);
void set_gflags(GROUP *g, char *p);
ENTRY *get_ghead(GROUP *g);
int get_ecount(GROUP *g);
int get_gcount();
void group_debug(); //debug function to output all groups

GROUP *groups_head;
GROUP *gp; //pointer to remember last group that was looked at
int group_count = 0;
int total_count = 0;

GROUP *create_group(char *new_name){
	GROUP *new = malloc(sizeof(GROUP));

	strcpy(new->name, new_name); //by default, group name is equivalent to the path
	strcpy(new->program, "./");  //by default, launch an entry by executing it
	new->flags[0] = '\0';      //by default, no command line flags
	new->head = NULL;
	new->tail = NULL;
	new->next = NULL;
	new->entry_count = 0;

	group_count++;
	return new;
}

//add an entry to a group or add a new empty group
//FIXME maybe make this function part of a seperate file to handle a tree (AVL?)
//for now, simple linked list implementation
void group_add(char *gname, ENTRY *addme){
	int i;
	GROUP *new;
	GROUP *last = NULL; //last element in an existing group list (NULL to start)

	//only adding a new group
	if(addme == NULL){
		gp = groups_head;
		while(gp != NULL){
			if(!(strcmp(gp->name, gname))){
				printf("config error: %s is already a group!\n", gname);
				return;
			}

			last = gp;
			gp = gp->next;
		}
	}

	//The previous group is not the same as the new group to add to
	if(!(gp != NULL && (!(strcmp(gp->name, gname))))){
		gp = groups_head;
		while(gp != NULL){
			//gname matches groups[i]'s name, add entry here
			if(!(strcmp(gp->name, gname))) break;

			last = gp;
			gp = gp->next;
		}
	}

	//was unable to find a matching existing group
	//need to create new group to insert the entry into
	if(gp == NULL){
		new = create_group(gname);

		//first group
		if(last == NULL) groups_head = new;

		//add to the end of the groups
		else last->next = new;

		gp = new;
	}

	//add the entry to the list of entries in the group
	if(addme != NULL){
		i = entry_add(gp->head, gp->tail, addme);
		switch(i){
			case 1:
				gp->head = addme;
				break;

			case 2:
				gp->tail = addme;
				break;

			case 3:
				gp->head = addme;
				gp->tail = addme;
				break;

			}

		gp->entry_count++;
		total_count++;
	}

	return;
}

void group_rm(GROUP *g){

	clear_entries(g->head);

	free(g);
	group_count--;
	return;
}

void clean_groups(){
	GROUP *dummy_head;
	GROUP *trav;
	GROUP *hold;

	if(group_count == 0){
		printf("Error: no groups!\n");
		exit(0);
	}

	else{
		dummy_head = create_group("dummy");
		dummy_head->next = groups_head;
		trav = dummy_head;

		while(trav != NULL){
			//found empty group for removal
			if(trav->next != NULL && trav->next->entry_count < 1){
				printf("Omitting empty group \"%s\"\n", trav->next->name);
				hold = trav->next;
				trav->next = trav->next->next;
				group_rm(hold);
			}
			else trav = trav->next;
		}
	}

	//ensure groups->head is still correct
	groups_head = dummy_head->next;
	group_rm(dummy_head);
	return;

}


GROUP **get_groups(){
	GROUP **arr = malloc(sizeof(GROUP *) * group_count);
	GROUP *trav = groups_head;
	int i;

	for(i = 0; i < group_count; i++){
		arr[i] = trav;
		trav = trav->next;
	}

	return arr;
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

ENTRY *get_ghead(GROUP *g){
	assert(g != NULL);
	return g->head;
}

int get_ecount(GROUP *g){
	assert(g != NULL);
	return g->entry_count;
}

int get_gcount(){
	return group_count;
}

void group_debug(){
	GROUP *trav = groups_head;
	
	while(trav != NULL){
		entry_debug(trav->head);
		printf("\tfrom group %s\n", trav->name);
		trav = trav->next;
	}

	return;
}
