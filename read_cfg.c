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
#define OPTION_CNT 11

//public
char *find_config();
void cfg_interp(char *path);
int get_compmode();
bool get_sort();

//private
void check_line(char *buffer, char **options);
int check_option(char *arg, char **options);
void handle_fname(char *path, char *group, bool recurs, bool force, char *name);
void addme(char *path, char *group, bool force, char *name);
char *autoAlias(char *path);
int search_ch(char *str, char c);
int wild_cmp(char *wild, char *literal);
char *strip_quotes(char *str);

//allow for compatability with compatability layers such as WSL
int compmode = 0;
//0 -> none
//1 -> WSL
//maybe more later?

//turn on or off sorting (A-Z); On by default
bool sort = 1;

//set to true to automatically try to create a human readable name for an entry
bool hr = false;

#if defined _WIN32 || defined _WIN64
//for Windows Compatability, this will be '\\' (otherwise '/')
char sep = '\\';
#else
char sep = '/';
#endif

char *find_config(){
	char *home = getenv("HOME");
	char *path = malloc(sizeof(char) * BUF_LEN);
	char choices[2][BUF_LEN];
	int check_count = 2;
	int i;

	sprintf(choices[0], "%s%c.config%ctml%cconfig", home, sep, sep, sep);
	sprintf(choices[1], "%s%c.tml%cconfig", home, sep, sep);

	//TODO where do we want the Windows config to be?

	for(i = 0; i < check_count; i++){
		path = choices[i];
		if(access(path, R_OK) == 0){
			printf("Using config \"%s\"\n", path);
			return path;
		}
	}

	return "config";
}

void cfg_interp(char *path){
	FILE *fp;
	char buffer[BUF_LEN];
	GROUP **g;
	ENTRY **e;
	int count;
	int e_count;
	int i;
	int j;

	fp = fopen(path, "r");
	assert(fp != NULL);

	//build the options array
	char **options = malloc(sizeof(char *) * OPTION_CNT);
	options[0] = "add";
	options[1] = "addF";
	options[2] = "addGroup";
	options[3] = "addName";
	options[4] = "addNameF";
	options[5] = "addR";
	options[6] = "autoAlias";
	options[7] = "compMode";
	options[8] = "setFlags";
	options[9] = "setLauncher";
	options[10] = "sort";

	//Read each line of "config"
	while(fgets(buffer, BUF_LEN, fp)){
		check_line(buffer, options);
	}

	//cleanup
	free(options);

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

int get_compmode(){
	return compmode;
}

bool get_sort(){
	return sort;
}

//TODO add support for "addR" recursive adding (still needs work...)
//TODO add support for "alias" option
//TODO add support for "hide" option
void check_line(char *buffer, char **options){
	char *delims = " \t\n";
	char *tok = strtok(buffer, delims);
	char args[MAX_ARGS][BUF_LEN];
	GROUP **g;
	char *tok_p;
	char *arg_p;
	int g_count;
	int search_res;
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
				arg_p = &args[i][0];
				tok_p = &tok[1];

				while(*tok_p != '"'){
					switch(*tok_p){


						case '\0':
							tok = strtok(NULL, delims);
							tok_p = &tok[0];
							*arg_p = ' ';
							arg_p++;
							break;

						case '\\':
							tok_p++;

						default:
							*arg_p = *tok_p;
							tok_p++;
							arg_p++;

					}
				}

				*arg_p = '\0';
					
			}

			tok = strtok(NULL, delims);
			i++;
		}

		//optimally check which option was specified
		search_res = check_option(args[0], options);

		switch(search_res){

			case 0: //add
				//add entry(ies) to a group: first arg is the file(s), second arg is the group to add to
				//TODO add sorting functionality
				handle_fname(args[1], args[2], 0, 0, NULL);
				break;
				
			case 1: //addF
				//force add entry to a group: first arg is the file(s), second arg is the group to add to
				handle_fname(args[1], args[2], 0, 1, NULL);
				break;

			case 2: //addGroup
				//create a new group
				group_add(strip_quotes(args[1]), NULL);
				break;

			case 3: //addName
				//add entry to a group: first arg is the name, second arg is the file, and third arg is the group to add to
				handle_fname(args[2], args[3], 0, 0, args[1]);
				break;

			case 4: //addNameF
				//same as addName, but with force on
				handle_fname(args[2], args[3], 0, 1, args[1]);
				break;

			case 5: //addR
				//recursively add: that is, also search directories in the given path
				//NOTE: experimental
				handle_fname(args[1], args[2], 1, 0, NULL);
				break;
				
			case 6: //autoAlias
				if(!(strcmp(args[1], "on"))) hr = true;
				else if(!(strcmp(args[1], "off"))) hr = false;
				break;

			case 7: //compMode
				if(!(strcmp(args[1], "WSL"))) compmode = 1;
				else printf("Error: Unknown Compatability Mode Argument \"%s\"\n", strip_quotes(args[1]));
				break;

			case 8: //setFlags
				//args[1] is referring to a group
				g = get_groups();
				g_count = get_gcount();

				//look for matching existing group
				for(i = 0; i < g_count; i++){
					if(!(strcmp(get_gname(g[i]), args[1]))) break;
				}

				//set a group's launcher flags (like ./program -f file for fullscreen)
				//assert that a matching group was found
				if(i < g_count) set_gflags(g[i], strip_quotes(args[2]));
				else printf("Error: Group \"%s\" does not exist\n", args[1]);
				break;

			case 9: //setLauncher
				//args[1] is referring to a group
				g = get_groups();
				g_count = get_gcount();

				//look for matching existing group
				for(i = 0; i < g_count; i++){
					if(!(strcmp(get_gname(g[i]), args[1]))) break;
				}

				//set a group's launcher (this requires pulling down the existing groups and finding the one that args[1] mentions)
				//assert that a matching group was found
				if(i < g_count) set_gprog(g[i], strip_quotes(args[2]));
				else printf("Error: Group \"%s\" does not exist\n", args[1]);
				break;

			case 10: //sort
				if(!(strcmp(args[1], "on"))) sort = true;
				else if(!(strcmp(args[1], "off"))) sort = false;
				break;

			default:
				printf("Error: Unknown config option \"%s\"\n", args[0]);

		}

	}

	return;
}

int check_option(char *arg, char **options){
	int min = 0;
	int max = OPTION_CNT-1;
	int hover;
	int comp_res;

	while(max - min > 1){
		hover = min + (max-min)/2;
		comp_res = strcmp(arg, options[hover]);

		if(comp_res > 0) min = hover;
		else if(comp_res < 0) max = hover;
		else return hover;
	}

	if(max == OPTION_CNT-1 && strcmp(arg, options[max]) == 0) return max;
	else if(min == 0 && strcmp(arg, options[min]) == 0) return min;

	return -1;
}

//TODO augment to involve recurs
//TODO could use some cleanup...
void handle_fname(char *path, char *group, bool recurs, bool force, char *name){
	ENTRY *new;
	char *search; //pointer for traversing path
	char full_path_cpy[BUF_LEN];
	char relative_path_cpy[BUF_LEN];
	char arg_cpy[BUF_LEN];
	char auto_name[BUF_LEN];
	int plen = strlen(path);
	char *dirname;
	DIR *dp;
	struct dirent *fname;
	int i;

	assert(path != NULL && group != NULL);

	if(path[0] == '\0' || group[0] == '\0'){
		printf("Error: too few arguments for \"add\"\n");
		return;
	}

	//address potential quotes
	strcpy(full_path_cpy, strip_quotes(path));

	//don't check that the path arg is valid when forced
	if(force) addme(full_path_cpy, group, force, name);

	//file is not recognized, perhaps it has a wildcard?
	//TODO finish rewriting a more robust wildcard thingy
	else if(access(full_path_cpy, F_OK) == -1){
		i = search_ch(full_path_cpy, '*');
		if(i > -1){
			//look for a directory
			while(full_path_cpy[i] != sep && (i >= 0)){
				i--;
			}
			dirname = full_path_cpy;
			strcpy(arg_cpy, full_path_cpy);
			dirname[i+1] = '\0';
			dp = opendir(dirname);

			//the directory is real
			if(dp != NULL){ 
				while(fname = readdir(dp)){
					relative_path_cpy[0] = '\0';
					strcat(relative_path_cpy, dirname);
					strcat(relative_path_cpy, fname->d_name);

#if defined _WIN32 || defined _WIN64
					//Windows cannot tell file types (TODO), so just add relatively indiscriminantly
					if(!(wild_cmp(&arg_cpy[i+1], fname->d_name))) addme(relative_path_cpy, group, force, name);

#else
					//check if path is a file (and not a directory/symlink/etc.) and regex matches
					if(fname->d_type == DT_REG && !(wild_cmp(&arg_cpy[i+1], fname->d_name))) addme(relative_path_cpy, group, force, name);

					//if the recursive option was specified and the path is a directory, run handle_fname on this directory, but for security reasons, do not consider directories that start with a '.'
					//FIXME this may still need some work...
					else if(recurs && fname->d_type == DT_DIR && fname->d_name[0] != '.'){
						strcat(relative_path_cpy, "/*");
						handle_fname(relative_path_cpy, group, 1, 0, NULL);
					}
#endif

				}

				closedir(dp);
			}

			//directory is not real, report error to the user
			else printf("Error: \"%s\" bad path\n", dirname);
		}

		//path is not real, report error to the user
		else printf("Error: \"%s\" bad path\n", dirname);
	}

	//file name is okay
	//FIXME does not take into account whether the argument is a file (could be a directory, symlink, etc.)
	else addme(full_path_cpy, group, force, name);

	return;
}

void addme(char *path, char *group, bool force, char *name){
	ENTRY *new;
	char auto_name[BUF_LEN];

	//check if a name was given as argument
	if(name != NULL){
		//strip quotes from the name
		name = strip_quotes(name);
		new = create_entry(name, path, force);
	}

	//check if autoAlias is on. If it is, go to the autoAlias function
	else if(hr){
		strcpy(auto_name, autoAlias(path));
		new = create_entry(auto_name, path, force);
	}

	else new = create_entry(path, path, force);
	if(new != NULL) group_add(group, new);

	return;
}


//TODO figure out how to trim the extensions of files off
char *autoAlias(char *path){
	char *hr_name = malloc(sizeof(char) * BUF_LEN);
	char *p = hr_name;
	char *rpath; //necessary so as not to touch the actual path
	bool stop = false; //stop when you don't want to add a series of chars to the output

	//get to the relative path name
	rpath = strrchr(path, sep);
	if(rpath == NULL) rpath = path;
	else rpath++;

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
	if(*path == '"') *(p-1) = '\0'; //close early to avoid including closing quote
	else *p = '\0';

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

char *strip_quotes(char *str){
	char *stripped_str = malloc(sizeof(char) * BUF_LEN);
	
	if(str[0] == '"'){
		stripped_str = &str[1];
		stripped_str[strlen(stripped_str) - 1] = '\0';
		return stripped_str;
	}

	return str;
}
