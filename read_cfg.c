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
#define MAX_ARGS 5

//public
void cfg_interp();
void check_line(char *buffer);

//private
void handle_fname(char *path, char *group);

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

	fclose(fp);
	return;
}

void check_line(char *buffer){
	char *delims = " \t\n";
	char *tok = strtok(buffer, delims);
	char *args[MAX_ARGS];
	int i;

	//initialize args to NULL
	for(i = 0; i < MAX_ARGS; i++){
		args[i] = NULL;
	}

	i = 0;
	//record all arguments in the line
	while(tok != NULL){
		if(i >= MAX_ARGS){
			printf("Error: too many arguments\n");
			return;
		}
		args[i] = tok;
		tok = strtok(NULL, delims);
		i++;
	}

	//add entry(ies) to a group: first arg is the file(s), second arg is the group to add to
	//TODO add potential dash functions
	//TODO account for spaces in file name
	//TODO add support for "-R" recursive adding
	//TODO add sorting functionality
	if(!(strcmp(args[0], "add"))) handle_fname(args[1], args[2]);

	//create a new group
	if(!(strcmp(args[0], "addGroup"))) group_add(args[1], NULL);

	return;
}

void handle_fname(char *path, char *group){
	ENTRY *new;
	char path_cpy[BUF_LEN];
	int plen = strlen(path);
	char *dirname;
	DIR *dp;
	struct dirent *fname;
	int i = 1;

	if(path == NULL || group == NULL){
		printf("Error: too few arguments for \"add\"\n");
		return;
	}

	//file is not recognized, perhaps it has a wildcard?
	if(access(path, F_OK) == -1){
		if(path[plen-i] == '*'){
			//look for a directory
			while(path[plen-i] != '/' && (plen-i >= 0)){
				i++;
			}
			dirname = path;
			dirname[plen-i+1] = '\0';
			dp = opendir(dirname);

			//the directory is real
			if(dp != NULL){ 
				while(fname = readdir(dp)){
					path_cpy[0] = '\0';
					strcat(path_cpy, dirname);
					strcat(path_cpy, fname->d_name);

					//check if path is a file (and not a directory/symlink/etc.)
					if(fname->d_type == DT_REG){
						new = create_entry(path_cpy, path_cpy);
						if(new != NULL) group_add(group, new);
					}
				}

				//FIXME breaks if I close the dir (WHY?!?!?!?)
				closedir(dp);
			}

			//directory is not real, report error to the user
			else printf("Error: \"%s\" bad path\n", dirname);
		}
	}

	//file name is okay
	else{
		new = create_entry(path, path);
		if(new != NULL){
			group_add(group, new);
		}
	}

	return;
}
