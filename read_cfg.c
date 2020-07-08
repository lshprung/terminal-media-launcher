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
int get_compmode();

//private
void handle_fname(char *path, char *group);
char *autoAlias(char *path);
int search_ch(char *str, char c);
int wild_cmp(char *wild, char *literal);

//allow for compatability with compatability layers such as WSL
int compmode = 0;
//0 -> none
//1 -> WSL
//maybe more later?

//set to true to automatically try to create a human readable name for an entry
bool hr = false;

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

	//ensure line is not blank or commented out
	if(tok != NULL && tok[0] != '#' && tok[0] != '\0'){
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

		if(!(strcmp(args[0], "autoAlias"))){
			if(!(strcmp(args[1], "on"))) hr = true;
			else if(!(strcmp(args[1], "off"))) hr = false;
		}

		//add entry(ies) to a group: first arg is the file(s), second arg is the group to add to
		//TODO add potential dash functions
		//TODO add support for "-R" recursive adding
		//TODO add sorting functionality
		if(!(strcmp(args[0], "add"))) handle_fname(args[1], args[2]);

		//create a new group
		else if(!(strcmp(args[0], "addGroup"))) group_add(args[1], NULL);

		//set compatability mode
		else if(!(strcmp(args[0], "compMode"))){
			if(!(strcmp(args[1], "WSL"))) compmode = 1;
			else printf("Error: Unknown Compatability Mode Argument \"%s\"\n", args[1]);
		}

		//TODO fix this so it can give an error that it doesn't recognize args[0]
		else{
			//remaining possibilities involve args[1] being a char* referring to a group
			g = get_groups();
			g_count = get_gcount();

			//look for matching existing group
			for(i = 0; i < g_count; i++){
				if(!(strcmp(get_gname(g[i]), args[1]))) break;
			}

			//assert that a matching group was found
			if(i < g_count){
				//set a group's launcher (this requires pulling down the existing groups and finding the one that args[1] mentions)
				if(!(strcmp(args[0], "setLauncher"))) set_gprog(g[i], args[2]);

				//set a group's launcher flags (like ./program -f file for fullscreen)
				if(!(strcmp(args[0], "setFlags"))) set_gflags(g[i], args[2]);
			}

			//couldn't find a match
			else printf("Error: Group \"%s\" does not exist\n", args[1]);
		}
	}

	return;
}

int get_compmode(){
	return compmode;
}

void handle_fname(char *path, char *group){
	ENTRY *new;
	char *search; //pointer for traversing path
	char path_cpy[BUF_LEN];
	char arg_cpy[BUF_LEN];
	char auto_name[BUF_LEN];
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
	//TODO finish rewriting a more robust wildcard thingy
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

						//check if autoAlias is on. If it is, go to the autoAlias function
						if(hr){
							strcpy(auto_name, autoAlias(path_cpy));
							new = create_entry(auto_name, path_cpy);
						}
						else new = create_entry(path_cpy, path_cpy);
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
			if(hr){
				strcpy(auto_name, autoAlias(path));
				new = create_entry(auto_name, path);
			}
			else new = create_entry(path, path);
			if(new != NULL){
				group_add(group, new);
			}
		}
	}

	else printf("Error: \"%s\" bad path\n", path);

	return;
}

//TODO figure out how to trim the extensions of files off
char *autoAlias(char *path){
	char *hr_name = malloc(sizeof(char) * BUF_LEN);
	char *p = hr_name;
	char *rpath; //necessary so as not to touch the actual path
	bool stop = false; //stop when you don't want to add a series of chars to the output

	//get to the relative path name
	if(compmode) rpath = strrchr(path, '\\');
	else rpath = strrchr(path, '/');
	rpath++;

	while(*rpath != '\0'){
		switch(*rpath){
			case '(':
				stop = true;
				break;

			case ')':
				stop = false;
				break;

			case '-':
			case '_':
				if(*(p-1) != ' ' && !stop){
					*p = ' ';
					*p++;
				}
				break;

			case ' ':
				if(*(p-1) != ' ' && !stop){
					*p = *rpath;
					*p++;
				}
				break;

			default:
				if(!stop){
					*p = *rpath;
					*p++;
				}
		}
		*rpath++;
	}
	
	//close the name
	*p = '\0';

	return hr_name;
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
