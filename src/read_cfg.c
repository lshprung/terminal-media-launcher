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
//void check_line(char *buffer, char **options, int ln);
//int check_option(char *arg, char **options);
int key_count(lua_State *L, int table_stack_index); // counts the number of keys in a table
void add_groups(lua_State *L, int table_stack_index, GROUP ***g);
void add_entries(lua_State *L, int table_stack_index, GROUP *g);

//turn on or off sorting (A-Z); On by default
bool sort = true;

//set to true to automatically try to create a human readable name for an entry
bool hr = false;

//turn foldCase (insensitive case searching) on or off; On by default
bool fold_case = true;

//return false if invalid path
GROUP **cfg_interp(char *path, int *group_count){
	FILE *fp;
	char buffer[BUF_LEN];
	GROUP **g;
	const char *group_name;
	//ENTRY **e;
	int i;
	int j;

	// check if file path exists
	fp = fopen(path, "r");
	if(fp == NULL){
		printf("Error: Invalid Configuration Path \"%s\"\n", path);
		return NULL;
	}
	fclose(fp);

	// load lua configuration
	lua_State *L = luaL_newstate();
	luaL_openlibs(L); // allow for standard library to be used
	int config_load_status = luaL_dofile(L, path);
	if(config_load_status != 0) {
		printf("Error: could not load configuration \"%s\"\n", path);
		lua_error(L);
		exit(1);
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
		exit(1);
	}

	// create the group array
	*group_count = key_count(L, i);
	g = malloc(sizeof(GROUP *) * (*group_count));

	// add each group (which also adds each entry to each group)
	add_groups(L, i, &g);
	lua_close(L);
	return g;
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

int key_count(lua_State *L, int table_stack_index) {
	int output = 0;

	lua_pushnil(L);
	while(lua_next(L, table_stack_index)) {
		// uses 'key' (at index -2) and 'value' (at index -1)
		if(lua_type(L, -2) == LUA_TSTRING) ++output;
		lua_pop(L, 1);
	}

	return output;
}

void add_groups(lua_State *L, int table_stack_index, GROUP ***g) {
	const char *group_name;
	int entry_table_stack_index;
	int i;

	lua_pushnil(L);
	i = 0;
	while(lua_next(L, table_stack_index)) {
		// uses 'key' (at index -2) and 'value' (at index -1)
		// looking at Groups.TABLE_NAME
		if(lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TTABLE) {
			group_name = lua_tostring(L, -2);
			if(group_name != NULL) {
				// push the Entries table on the stack (to get entry information)
				lua_pushstring(L, "Entries");
				// get table Groups.TABLE_NAME.Entries
				lua_gettable(L, -2);
				entry_table_stack_index = lua_gettop(L);
				// check that 'Entries' is a table
				if(lua_type(L, entry_table_stack_index) != LUA_TTABLE) {
					printf("Error in config: 'Entries' should be Table, is actually %s\n", lua_typename(L, lua_type(L, entry_table_stack_index)));
					exit(1);
				}
				(*g)[i] = create_group(group_name, key_count(L, entry_table_stack_index));
				// add entries to this group
				add_entries(L, entry_table_stack_index, (*g)[i]);
			}
		}
		lua_pop(L, 1);
		++i;
	}
}

void add_entries(lua_State *L, int table_stack_index, GROUP *g) {
	const char *entry_name;
	int i;

	lua_pushnil(L);
	i = 0;
	while(lua_next(L, table_stack_index)) {
		// uses 'key' (at index -2) and 'value' (at index -1)
		// looking at Groups.TABLE_NAME.Entries[i]
		if(lua_type(L, -1) == LUA_TSTRING) {
			entry_name = lua_tostring(L, -1);
			if(entry_name != NULL) {
				set_gentry(g, i, create_entry(entry_name, entry_name, true));
			}
		}
		lua_pop(L, 1);
		++i;
	}

	// one last pop to pop the key off
	lua_pop(L, 1);
}
