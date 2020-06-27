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
int search_ch(char *str, char c);
int wild_cmp(char *wild, char *literal);

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
	char args[MAX_ARGS][BUF_LEN];
	GROUP **g;
	int g_count;
	int i;

	//initialize args to 0
	for(i = 0; i < MAX_ARGS; i++){
		args[i][0] = '\0';
	}

	i = 0;
	//record all arguments in the line
	while(tok != NULL){
		if(i >= MAX_ARGS){
			printf("Error: too many arguments\n");
			return;
		}
		strcpy(args[i], tok);
		//handle if an argument has spaces and is wrapped in quotes
		if(tok[0] == '"'){
			while(tok[strlen(tok)-1] != '"'){
				tok = strtok(NULL, delims);
				strcat(args[i], " ");
				strcat(args[i], tok);
			}
		}

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

	//set a group's launcher (this requires pulling down the existing groups and finding the one that args[1] mentions)
	if(!(strcmp(args[0], "setLauncher"))){
		g = get_groups();
		g_count = get_gcount();

		//look for matching existing group
		for(i = 0; i < g_count; i++){
			if(!(strcmp(get_gname(g[i]), args[1]))){
				set_gprog(g[i], args[2]);
				return;
			}
		}

		//couldn't find a match
		printf("Error: Group \"%s\" does not exist\n", args[1]);
	}

	return;
}

void handle_fname(char *path, char *group){
	ENTRY *new;
	char *search; //pointer for traversing path
	char path_cpy[BUF_LEN];
	char arg_cpy[BUF_LEN];
	int plen = strlen(path);
	char *dirname;
	DIR *dp;
	struct dirent *fname;
	int i;

	if(path == NULL || group == NULL){
		printf("Error: too few arguments for \"add\"\n");
		return;
	}

	//file is not recognized, perhaps it has a wildcard?
	//TODO finish rewriting a more robust wildcard thingy, still doesn't work using regex
	if(access(path, F_OK) == -1){
		i = search_ch(path, '*');
		if(i > -1){
			//look for a directory
			while(path[i] != '/' && (i >= 0)){
				i--;
			}
			dirname = path;
			strcpy(arg_cpy, path);
			dirname[i+1] = '\0';
			dp = opendir(dirname);

			//the directory is real
			if(dp != NULL){ 
				while(fname = readdir(dp)){
					path_cpy[0] = '\0';
					strcat(path_cpy, dirname);
					strcat(path_cpy, fname->d_name);

					//check if path is a file (and not a directory/symlink/etc.) and regex matches
					if(fname->d_type == DT_REG && !(wild_cmp(&arg_cpy[i+1], fname->d_name))){

						new = create_entry(path_cpy, path_cpy);
						if(new != NULL) group_add(group, new);
					}
				}

				closedir(dp);
			}

			//directory is not real, report error to the user
			else printf("Error: \"%s\" bad path\n", dirname);
		}

		//file name is okay
		else{
			new = create_entry(path, path);
			if(new != NULL){
				group_add(group, new);
			}
		}
	}

	else printf("Error: \"%s\" bad path\n", path);

	return;
}

int search_ch(char *str, char c){
	int i = 0;

	while(str[i] != '\0'){
		if(str[i] == c) return i;
		i++;
	}

	return -1;
}

//return 0 if match, 1 if not
//TODO only supports one wildcard per entry
int wild_cmp(char *wild, char *literal){
	int i;
	
	while(*wild != '\0'){
		//traverse until wildcard
		if(*wild != '*'){
			if(*wild != *literal) return 1;
			wild++;
			literal++;
		}

		//found wildcard, find the end of both names and comapre from the back
		else{
			i = 0;
			wild++;
			while(*wild != '\0'){
				i++;
				wild++;
			}
			while(*literal != '\0'){
				literal++;
			}

			while(i > 0){
				wild--;
				literal--;
				if(*wild != *literal) return 1;
				i--;
			}

			return 0;
		}
	}

	return 0;
}
