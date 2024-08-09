#include <assert.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#if defined _WIN32 || defined _WIN64
#include "windows/read_cfg.h"
#else
#include "unix/read_cfg.h"
#endif
*/

#include "include/entry.h"
#include "include/group.h"
#include "include/read_cfg.h"
#define MAX_ARGS 5
#define OPTION_CNT 14

//private
void check_line(char *buffer, char **options, int ln);
int check_option(char *arg, char **options);
char *autoAlias(char *path);

//turn on or off sorting (A-Z); On by default
bool sort = true;

//set to true to automatically try to create a human readable name for an entry
bool hr = false;

//turn foldCase (insensitive case searching) on or off; On by default
bool fold_case = true;

//return false if invalid path
bool cfg_interp(char *path){
	FILE *fp;
	char buffer[BUF_LEN];
	GROUP **g;
	ENTRY **e;
	int count;
	int e_count;
	int i=0;
	int j;

	// check if file path exists
	fp = fopen(path, "r");
	if(fp == NULL){
		printf("Error: Invalid Configuration Path \"%s\"\n", path);
		return false;
	}
	fclose(fp);

	// load lua configuration
	lua_State *L = luaL_newstate();
	luaL_openlibs(L); // allow for standard library to be used
	int config_load_status = luaL_dofile(L, path);
	if(config_load_status != 0) {
		printf("Error: could not load configuration \"%s\"\n", path);
		lua_error(L);
		return false;
	}

	// set up base configuration variables
	// TODO set helper variables and functions (e.g., so that Groups table doesn't need to be manually created in the user's config)
	//lua_newtable(L);
	//lua_setglobal(L, "Groups");
	//lua_pcall(L, 0, 0, 0);

	// demo
	lua_getglobal(L, "Groups");
	i = lua_gettop(L);
	if(lua_type(L, i) != LUA_TTABLE) {
		printf("Error in config: 'Groups' should be Table, is actually %s\n", lua_typename(L, lua_type(L, i)));
		return 1;
	}
	// add each group
	const char *group_name;
	lua_pushnil(L);
	while(lua_next(L, i)) {
		// the key is index -2, value is -1
		if(lua_type(L, -2) != LUA_TSTRING) continue; // skip if invalid key
		group_name = lua_tostring(L, -2);
		group_add(group_name, NULL);
		// check for a launcher
		if(lua_type(L, -1) != LUA_TTABLE) continue;  // skip if invalid value
		lua_pushstring(L, "Launcher");
		lua_gettable(L, -2);
		if(lua_type(L, -1) == LUA_TSTRING) {
			// FIXME the groups '''API''' I built might need some improvements in order for this to not be a headache
			//set_gprog(GROUP *g, lua_tostring(L, -1))
		}
		lua_pop(L, 1);
		// add each entry
		lua_pushstring(L, "Entries");
		lua_gettable(L, -2);
		j = lua_gettop(L);
		lua_pushnil(L);
		while(lua_next(L, j)) {
			if(lua_type(L, -1) != LUA_TSTRING) continue; // skip if invalid value
			handle_fname(lua_tostring(L, -1), group_name, false, true, NULL, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}

	//lua_getfield(L, -1, "Pictures");
	//lua_getfield(L, -1, "Entries");
	//lua_rawgeti(L, -1, 1);
	//printf("message: %d\n", (int)lua_tonumber(L, -1));
	//lua_rawgeti(L, -2, 2);
	//printf("message: %d\n", (int)lua_tonumber(L, -1));
	//lua_rawgeti(L, -3, 3);
	//printf("message: %d\n", (int)lua_tonumber(L, -1));

	lua_close(L);
	return true;

	/* --- Old code --- */
	//build the options array
	char **options = malloc(sizeof(char *) * OPTION_CNT);
	options[0] = "add";
	options[1] = "addF";
	options[2] = "addGroup";
	options[3] = "addName";
	options[4] = "addNameF";
	options[5] = "addR";
	options[6] = "autoAlias";
	options[7] = "foldCase";
	options[8] = "hide";
	options[9] = "hideFile";
	options[10] = "setFlags";
	options[11] = "setLauncher";
	options[12] = "setLauncherRaw";
	options[13] = "sort";

	//Read each line of "config"
	while(fgets(buffer, BUF_LEN, fp)){
		printf("%d\n", i);
		i++;
		check_line(buffer, options, i);
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
	return true;
}

bool get_sort(){
	return sort;
}

bool get_case_sensitivity(){
	return fold_case;
}

void refer_to_doc(){
	printf("Refer to documentation on how to create terminal-media-launcher config file\n");
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

int search_ch(char *str, char c){
	int i = 0;

	while(str[i] != '\0'){
		if(str[i] == c) return i;
		i++;
	}

	return -1;
}

int search_last_ch(char *str, char c){
	int i = 0;
	int last_i = -1;

	while(str[i] != '\0'){
		if(str[i] == c) last_i = i;
		i++;
	}

	return last_i;
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

void error_mes(int ln, char *message){

	assert(message != NULL);

	printf("Configuration File Error:\nOn line %d: %s\n\n", ln, message);

	return;
}

//TODO add support for "addR" recursive adding (still needs work...)
//TODO add support for "alias" option
//TODO add support for "hide" option
void check_line(char *buffer, char **options, int ln){
	char *delims = " \t\n";
	char *tok = strtok(buffer, delims);
	char args[MAX_ARGS][BUF_LEN];
	GROUP **g;
	ENTRY **e;
	char *tok_p;
	char *arg_p;
	int g_count;
	int e_count;
	int search_res;
	int i, j;
	char *error_p; //helper for complex error messages

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
				error_mes(ln, "Too many arguments");
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
				handle_fname(args[1], args[2], 0, 0, NULL, ln);
				break;
				
			case 1: //addF
				//force add entry to a group: first arg is the file(s), second arg is the group to add to
				handle_fname(args[1], args[2], 0, 1, NULL, ln);
				break;

			case 2: //addGroup
				//create a new group
				group_add(strip_quotes(args[1]), NULL);
				break;

			case 3: //addName
				//add entry to a group: first arg is the name, second arg is the file, and third arg is the group to add to
				handle_fname(args[2], args[3], 0, 0, args[1], ln);
				break;

			case 4: //addNameF
				//same as addName, but with force on
				handle_fname(args[2], args[3], 0, 1, args[1], ln);
				break;

			case 5: //addR
				//recursively add: that is, also search directories in the given path
				//NOTE: experimental
				handle_fname(args[1], args[2], 1, 0, NULL, ln);
				break;
				
			case 6: //autoAlias
				if(!(strcmp(args[1], "on"))) hr = true;
				else if(!(strcmp(args[1], "off"))) hr = false;
				break;

			case 7: //foldCase (case insensitive)
				if(!(strcmp(args[1], "on"))) fold_case = true;
				else if(!(strcmp(args[1], "off"))) fold_case = false;
				break;

			//TODO consider having this call handle_fname instead so that '*' can be used
			case 8: //hide
			case 9: //hideFile
				//args[2] is referring to a group
				g = get_groups();
				g_count = get_gcount();

				//look for matching existing group
				for(i = 0; i < g_count; i++){
					if(!(strcmp(get_gname(g[i]), args[2]))) break;
				}

				if(i < g_count){
					e_count = get_ecount(g[i]);
					e = get_entries(get_ghead(g[i]), e_count);

					for(j = 0; j < e_count; j++){
						if(!strcmp((search_res == 8 ? get_ename(e[j]) : get_epath(e[j])), strip_quotes(args[1]))) break;
					}

					if(j < e_count){
						set_hide(e[j], true);
						set_ecount(g[i], get_ecount(g[i])-1);
					}
					else{
						error_p = malloc(sizeof(char) * 1024);
						sprintf(error_p, "Entry \"%s\" does not exist", args[1]);
						error_mes(ln, error_p);
						free(error_p);
					}
				}

				else{
					error_p = malloc(sizeof(char) * 1024);
					sprintf(error_p, "Group \"%s\" does not exist", args[2]);
					error_mes(ln, error_p);
					free(error_p);
				}
				break;

			case 10: //setFlags
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
				else{
					error_p = malloc(sizeof(char) * 1024);
					sprintf(error_p, "Group \"%s\" does not exist", args[1]);
					error_mes(ln, error_p);
					free(error_p);
				}
				break;

			case 11: //setLauncher
			case 12: //setLauncherRaw
				//args[1] is referring to a group
				g = get_groups();
				g_count = get_gcount();

				//look for matching existing group
				for(i = 0; i < g_count; i++){
					if(!(strcmp(get_gname(g[i]), args[1]))) break;
				}

				//set a group's launcher (this requires pulling down the existing groups and finding the one that args[1] mentions)
				//assert that a matching group was found
				if(i < g_count){
					set_gprog(g[i], strip_quotes(args[2]));
					if(search_res == 12) set_gquotes(g[i], false); //FIXME don't forget to change this line if adding more options!!!
				}
				else{
					error_p = malloc(sizeof(char) * 1024);
					sprintf(error_p, "Group \"%s\" does not exist", args[1]);
					error_mes(ln, error_p);
					free(error_p);
				}
				break;

			case 13: //sort
				if(!(strcmp(args[1], "on"))) sort = true;
				else if(!(strcmp(args[1], "off"))) sort = false;
				break;

			default:
				error_p = malloc(sizeof(char) * 1024);
				sprintf(error_p, "Unknown config option \"%s\"", args[0]);
				error_mes(ln, error_p);
				free(error_p);

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


char *autoAlias(char *path){
	char *hr_name = malloc(sizeof(char) * BUF_LEN);
	char *p = hr_name;
	char *rpath; //necessary so as not to touch the actual path
	char *last_dot = NULL; //used to trim the file extension (if there is one)
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

			case '.':
				last_dot = p;

			default:
				if(!stop){
					*p = *rpath;
					*p++;
				}
		}
		*rpath++;
	}
	
	//close the name
	if(last_dot != NULL) *last_dot = '\0';
	else if(*path == '"') *(p-1) = '\0'; //close early to avoid including closing quote
	else *p = '\0';

	return hr_name;
}



