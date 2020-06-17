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
	DIR *dp;
	struct dirent *d_entry;
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
			while((d_entry = readdir(dp))){
				//only add if its a file (d_type = 8)
				if(d_entry->d_type == 8){
					new = create_entry(d_entry->d_name, tok);
					if(new != NULL) group_add(tok, new);
				}
			}
		}

		//TODO add option to also add files from directories inside directory (-r)

		closedir(dp);
	}

	return;
}
