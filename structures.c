#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUF_LEN 1024
#define MAX_GRPS 50

typedef struct entry{
	char name[BUF_LEN];
	char path[BUF_LEN];
	struct entry *next;
} ENTRY;

typedef struct group{
	char name[BUF_LEN];
	char path[BUF_LEN];
	struct entry *head;
	struct entry *tail;
	struct group *next;
} GROUP;

void check_line(char *buffer);
ENTRY *create_entry(char *new_path, char *new_group);
GROUP *create_group(char *new_path);
void group_add(char *gname, ENTRY *addme);

GROUP *groups_head;
GROUP *gp; //pointer to remember last group that was looked at

int main(){
	FILE *fp;
	char buffer[BUF_LEN];

	fp = fopen("config", "r");
	assert(fp != NULL);

	while(fgets(buffer, BUF_LEN, fp)){
		check_line(buffer);
	}

	//DEBUG: test to see if the list was added to properly
	GROUP *gtrav = groups_head;
	ENTRY *etrav;
	while(gtrav != NULL){
		etrav = gtrav->head;
		while(etrav != NULL){
			printf("%s, from group %s\n", etrav->name, gtrav->name);
			etrav = etrav->next;
		}
		gtrav = gtrav->next;
	}

	return 0;
}

void check_line(char *buffer){
	char *delims = " \t\n";
	char *tok = strtok(buffer, delims);
	DIR *dp;
	struct dirent *entry;
	ENTRY *new;

	//add a path to the entry list
	//TODO add potential dash functions
	//TODO account for spaces in file name
	if(!(strcmp(tok, "add"))){
		tok = strtok(NULL, delims);
		new = create_entry(tok, NULL);
		if(new != NULL) group_add(tok, new);
	}

	//recursively add entries from a directory
	if(!(strcmp(tok, "addDir"))){
		tok = strtok(NULL, delims);
		dp = opendir(tok);

		//check if the directory is valid
		if(dp == NULL) printf("Error: Invalid Directory \"%s\"\n", tok);

		else{
			while((entry = readdir(dp))){
				//only add if its a file (d_type = 8)
				if(entry->d_type == 8){
					new = create_entry(entry->d_name, tok);
					if(new != NULL) group_add(tok, new);
				}
			}
		}

		//TODO add option to also add files from directories inside directory (-r)

		closedir(dp);
	}

	return;
}

ENTRY *create_entry(char *new_path, char *new_group){
	ENTRY *new;
	char full_path[BUF_LEN] = "";

	if(new_group != NULL) strcat(full_path, new_group);
	strcat(full_path, new_path);

	//check if file exists
	if(access(full_path, F_OK) == -1){
		printf("Error: Invalid File Name \"%s\"\n", full_path);
		return NULL;
	}

	new = malloc(sizeof(ENTRY));

	strcpy(new->name, new_path);
	strcpy(new->path, full_path);
	new->next = NULL;

	return new;
}

GROUP *create_group(char *new_path){
	GROUP *new = malloc(sizeof(GROUP));

	strcpy(new->name, new_path); //by default, group name is equivalent to the path
	strcpy(new->path, new_path);
	new->head = NULL;
	new->tail = NULL;
	new->next = NULL;

	return new;
}

//FIXME maybe make this function part of a seperate file to handle a tree (AVL?)
//for now, simple linked list implementation
void group_add(char *gname, ENTRY *addme){
	int i;
	bool new_group = false; //do we need to create a new group?
	GROUP *new;
	GROUP *last = NULL; //last element in an existing group list (NULL to start)

	//The previous group is not the same as the new group to add to
	if(!(gp != NULL && (!(strcmp(gp->name, gname)) || !(strcmp(gp->path, gname))))){
		gp = groups_head;
		while(gp != NULL){
			//gname matches groups[i]'s name or path, add entry here
			if(!(strcmp(gp->name, gname)) || !(strcmp(gp->path, gname))) break;

			last = gp;
			gp = gp->next;
		}
	}

	//was unable to find a matching existing group
	if(gp == NULL) new_group = true;

	//need to create new group to insert the entry into
	if(new_group){
		new = create_group(gname);

		//first group
		if(last == NULL) groups_head = new;

		//add to the end of the groups
		else last->next = new;

		gp = new;
	}

	//add the entry to the list of entries in the group
	if(gp->head == NULL){
		gp->head = addme;
		gp->tail = addme;
	}

	else{
		gp->tail->next = addme;
		gp->tail = addme;
	}

	return;
}
