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
void get_settings(lua_State *L, int table_stack_index); // gets settings from Settings global variable
int get_group_count(lua_State *L, int table_stack_index); // counts the number of valid groups
int get_entry_count(lua_State *L, int table_stack_index); // counts the number of valid entries for a group
void add_groups(lua_State *L, int table_stack_index, GROUP ***g);
void add_entries(lua_State *L, int table_stack_index, GROUP *g);
void stack_debug(lua_State *L);

//turn on or off sorting for entries (A-Z); On by default
// TODO allow specifying whether to sort groups or entries (or both, or none)
bool sort = true;

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

	// open Settings table
	lua_getglobal(L, "Settings");
	i = lua_gettop(L);
	if(lua_type(L, i) == LUA_TTABLE) {
		// parse settings
		get_settings(L, i);
	}
	lua_pop(L, lua_gettop(L));

	// open Groups table
	lua_getglobal(L, "Groups");
	i = lua_gettop(L);
	if(lua_type(L, i) != LUA_TTABLE) {
		printf("Error in config: 'Groups' should be Table, is actually %s\n", lua_typename(L, lua_type(L, i)));
		exit(1);
	}

	// create the group array
	*group_count = get_group_count(L, i);
	if(*group_count <= 0) {
		printf("Error: No Groups!\n");
		g = NULL;
	}
	else {
		g = malloc(sizeof(GROUP *) * (*group_count));

		// add each group (which also adds each entry to each group)
		add_groups(L, i, &g);
	}
	lua_close(L);
	return g;
}

void refer_to_doc(){
	printf("Refer to documentation on how to create terminal-media-launcher config file\n");
	return;
}

void get_settings(lua_State *L, int table_stack_index) {
	bool *setting_vars[] = {
		&sort
	};

	char *setting_strings[] = {
		"sort"
	};

	int count = 1;
	int i;

	// looking at table Settings

	// check if autoAlias is set
	for(i = 0; i < count; ++i) {
		lua_pushstring(L, setting_strings[i]);
		lua_gettable(L, table_stack_index);
		if(lua_type(L, -1) == LUA_TBOOLEAN) {
			*(setting_vars[i]) = lua_toboolean(L, -1);
		}
	}
}

int get_group_count(lua_State *L, int table_stack_index) {
	int i = 1;
	int output = 0;
	int group_table_stack_index;
	int entry_table_stack_index;
	const char *group_name;

	do {
		lua_rawgeti(L, table_stack_index, i);
		group_table_stack_index = lua_gettop(L);
		if(lua_type(L, group_table_stack_index) == LUA_TTABLE) {
			// check validity of this group
			lua_pushstring(L, "name");
			lua_gettable(L, group_table_stack_index);
			group_name = lua_tostring(L, -1);
			if(group_name != NULL) {
				// check that the Entries table for this group is not empty
				lua_pushstring(L, "Entries");
				lua_gettable(L, group_table_stack_index);
				entry_table_stack_index = lua_gettop(L);
				if(lua_type(L, entry_table_stack_index) == LUA_TTABLE
						&& get_entry_count(L, entry_table_stack_index) > 0)
					++output;
			}
		}
		++i;
	} while(lua_type(L, group_table_stack_index) != LUA_TNIL);

	lua_pop(L, lua_gettop(L)-table_stack_index-1);
	return output;
}

int get_entry_count(lua_State *L, int table_stack_index) {
	int i = 1;
	int output = 0;

	do {
		lua_rawgeti(L, table_stack_index, i);
		if(lua_type(L, -1) == LUA_TSTRING) ++output;
		++i;
	} while(lua_type(L, -1) != LUA_TNIL);

	lua_pop(L, lua_gettop(L)-table_stack_index-1);
	return output;
}

void add_groups(lua_State *L, int table_stack_index, GROUP ***g) {
	const char *group_name;
	int group_table_stack_index; // index of Groups.TABLE_NAME
	int entry_table_stack_index; // index of Groups.TABLE_NAME.Entries
	int entry_count;
	char sort_cmd_lua[BUF_LEN]; // used to store the lua call to table.sort
	int i = 1; // index in lua table
	int count = 0; // index in C struct

	// sort the groups, if necessary
	if(sort) {
		luaL_dostring(L, "table.sort(Groups)");
	}

	do {
		lua_rawgeti(L, table_stack_index, i);
		group_table_stack_index = lua_gettop(L);
		if(lua_type(L, group_table_stack_index) == LUA_TTABLE) {
			// check validity of this group
			lua_pushstring(L, "name");
			lua_gettable(L, group_table_stack_index);
			group_name = lua_tostring(L, -1);
			if(group_name != NULL) {
				// push the Entries table on the stack (to get entry information)
				lua_pushstring(L, "Entries");

				// get table Groups[i].Entries
				lua_gettable(L, group_table_stack_index);
				entry_table_stack_index = lua_gettop(L);

				// check that 'Entries' is a table
				if(lua_type(L, entry_table_stack_index) != LUA_TTABLE) {
					printf("Error in config: in group '%s': 'Entries' should be Table, is actually %s\n", group_name, lua_typename(L, lua_type(L, entry_table_stack_index)));
					exit(1);
				}

				entry_count = get_entry_count(L, entry_table_stack_index);
				// check that the group has at least 1 entry
				if(entry_count <= 0)
					printf("Skipping empty group '%s'\n", group_name);
					
				else {
					(*g)[count] = create_group(group_name, entry_count);

					// sort the entries if necessary
					if(sort) {
						sprintf(sort_cmd_lua, "table.sort(Groups[%d].Entries)", i);
						luaL_dostring(L, sort_cmd_lua);
					}

					// add entries to this group
					add_entries(L, entry_table_stack_index, (*g)[count]);

					// set the launcher, if applicable
					lua_pushstring(L, "Launcher");
					lua_gettable(L, group_table_stack_index);
					if(lua_type(L, -1) == LUA_TSTRING) {
						set_gprog((*g)[count], lua_tostring(L, -1));
					}

					// set the launcher flags, if applicable
					lua_pushstring(L, "Flags");
					lua_gettable(L, group_table_stack_index);
					if(lua_type(L, -1) == LUA_TSTRING) {
						set_gflags((*g)[count], lua_tostring(L, -1));
					}

					++count;
				}
			}
		}
		++i;
	} while(lua_type(L, group_table_stack_index) != LUA_TNIL);

	lua_pop(L, lua_gettop(L)-table_stack_index-1);
}

void add_entries(lua_State *L, int table_stack_index, GROUP *g) {
	const char *entry_name;
	int i = 1; // index in lua table
	int count = 0; // index in C struct

	do {
		lua_rawgeti(L, table_stack_index, i);
		if(lua_type(L, -1) == LUA_TSTRING) {
			entry_name = lua_tostring(L, -1);
			if(entry_name != NULL) {
				set_gentry(g, count, create_entry(entry_name, entry_name, true));
				++count;
			}
		}
		++i;
	} while(lua_type(L, -1) != LUA_TNIL);

	lua_pop(L, lua_gettop(L)-table_stack_index-1);
}

void stack_debug(lua_State *L) {
	int i;

	printf("DEBUGGING STACK:\n");
	for(i = 1; i <= lua_gettop(L); ++i) {
		printf("\t%d - %s\n", i, lua_typename(L, lua_type(L, i)));
	}
}
