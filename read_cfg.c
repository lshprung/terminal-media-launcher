#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "entry.h"
#include "group.h"
#define BUF_LEN 1024 //maybe move this line to the header file

void cfg_interp();
void check_line(char *buffer);

void cfg_interp(){
	FILE *fp;
	char buffer[BUF_LEN];
	GROUP **g;
	ENTRY **e;
	int count;
	int e_count;
	int i;
	int j;

	//TODO have this check in certain locations for a config file, give error message if "config" does not exist
	fp = fopen("config", "r");
	assert(fp != NULL);

	//Read each line of "config"
	while(fgets(buffer, BUF_LEN, fp)){
		check_line(buffer);
	}

	/*
	//DEBUG: test to see if the list was added to properly
	g = get_groups();
	count = get_gcount();
	for(i = 0; i < count; i++){
		printf("Looking at group %s\n", get_gname(g[i]));
		e_count = get_ecount(g[i]);
		e = get_entries(get_ghead(g[i]), e_count);
		for(j = 0; j < e_count; j++){
			printf("\t%s\n", get_ename(e[j]));
		}
	}
	//END DEBUG
	*/

	return;
}

void check_line(char *buffer){
	char *delims = " \t\n";
	char *tok = strtok(buffer, delims);
	ENTRY *new;

	//add entry(ies) to a group: first arg is the file(s), second arg is the group to add to
	//TODO add potential dash functions
	//TODO account for spaces in file name
	//TODO add regex support for adding patterns
	//TODO allow adding directories (might fall into regex support)
	if(!(strcmp(tok, "add"))){
		//look at first arg
		tok = strtok(NULL, delims); 
		new = create_entry(tok, tok);
		//look at second arg
		tok = strtok(NULL, delims); 
		if(new != NULL) group_add(tok, new);
	}

	//create a new group
	if(!(strcmp(tok, "addGroup"))){
		tok = strtok(NULL, delims);
		group_add(tok, NULL);
	}

	return;
}
