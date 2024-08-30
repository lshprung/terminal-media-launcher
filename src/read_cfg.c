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
void get_settings(lua_State *L); // gets settings from Settings global variable
int get_group_count(lua_State *L); // counts the number of valid groups
int get_entry_count(lua_State *L); // counts the number of valid entries for a group
void add_groups(lua_State *L, GROUP ***g);
void add_entries(lua_State *L, GROUP *g);
void stack_debug(lua_State *L);
void table_debug(lua_State *L, int print_depth);

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
	// new lua stack
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
	// stack now contains: -1 => Settings
	if(lua_type(L, -1) == LUA_TTABLE) {
		// parse settings
		get_settings(L);
	}
	lua_pop(L, 1);
	// empty stack

	// open Groups table
	lua_getglobal(L, "Groups");
	// stack now contains: -1 => Groups
	if(lua_type(L, -1) != LUA_TTABLE) {
		printf("Error in config: 'Groups' should be Table, is actually %s\n", lua_typename(L, lua_type(L, -1)));
		exit(1);
	}

	// create the group array
	*group_count = get_group_count(L);
	if(*group_count <= 0) {
		printf("Error: No Groups!\n");
		g = NULL;
	}
	else {
		g = malloc(sizeof(GROUP *) * (*group_count));

		// add each group (which also adds each entry to each group)
		add_groups(L, &g);
	}
	lua_close(L);
	return g;
}

void refer_to_doc(){
	printf("Refer to documentation on how to create terminal-media-launcher config file\n");
	return;
}

void get_settings(lua_State *L) {
	// stack now contains: -1 => table

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
		// stack now contains: -1 => setting_string; -2 => table
		lua_gettable(L, -2);
		// stack now contains: -1 => string_value; -2 => table
		if(lua_type(L, -1) == LUA_TBOOLEAN) {
			*(setting_vars[i]) = lua_toboolean(L, -1);
		}
		lua_pop(L, 1);
		// stack now contains: -1 => table
	}
}

int get_group_count(lua_State *L) {
	int output = 0;

	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		if(lua_type(L, -1) == LUA_TNUMBER && lua_type(L, -2) == LUA_TTABLE) {
			lua_pushstring(L, "name");
			// stack now contains: -1 => "name"; -2 => key; -3 => value; -4 => key; -5 => table
			lua_gettable(L, -3);
			// stack now contains: -1 => name; -2 => key; -3 => value; -4 => key; -5 => table
			if(lua_type(L, -1) == LUA_TSTRING) {
				// check that the Entries table for this group is not empty
				lua_pushstring(L, "Entries");
				// stack now contains: -1 => "Entries"; -2 => name; -3 => key; -4 => value; -5 => key; -6 => table
				lua_gettable(L, -4);
				// stack now contains: -1 => Entries; -2 => name; -3 => key; -4 => value; -5 => key; -6 => table
				if(lua_type(L, -1) == LUA_TTABLE && get_entry_count(L) > 0)
					++output;
				lua_pop(L, 1);
				// stack now contains: -1 => name; -2 => key; -3 => value; -4 => key; -5 => table
			}
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		}
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)

	return output;
}

int get_entry_count(lua_State *L) {
	int output = 0;

	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		if(lua_type(L, -1) == LUA_TNUMBER && lua_type(L, -2) == LUA_TTABLE) {
			// check that the entry has a valid name
			lua_pushstring(L, "name");
			// stack now contains: -1 => "name"; -2 => key; -3 => value; -4 => key; -5 => table
			lua_gettable(L, -3);
			// stack now contains: -1 => name; -2 => key; -3 => value; -4 => key; -5 => table
			if(lua_type(L, -1) == LUA_TSTRING) ++output;
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		}
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)

	return output;
}

void add_groups(lua_State *L, GROUP ***g) {
	const char *group_name;
	int group_table_stack_index; // index of Groups.TABLE_NAME
	int entry_table_stack_index; // index of Groups.TABLE_NAME.Entries
	int entry_count;
	char sort_cmd_lua[BUF_LEN]; // used to store the lua call to table.sort
	int i = 1; // index in lua table
	int count = 0; // index in C struct

	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		if(lua_type(L, -1) == LUA_TNUMBER && lua_type(L, -2) == LUA_TTABLE) {
			lua_pushstring(L, "name");
			// stack now contains: -1 => "name"; -2 => key; -3 => value; -4 => key; -5 => table
			lua_gettable(L, -3);
			// stack now contains: -1 => name; -2 => key; -3 => value; -4 => key; -5 => table
			group_name = lua_tostring(L, -1);
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
			if(group_name != NULL) {
				// push the Entries table on the stack (to get entry information)
				lua_pushstring(L, "Entries");
				// stack now contains: -1 => "Entries"; -2 => key; -3 => value; -4 => key; -5 => table
				lua_gettable(L, -3);
				// stack now contains: -1 => Entries; -2 => key; -3 => value; -4 => key; -5 => table
				if(lua_type(L, -1) != LUA_TTABLE) {
					printf("Error in config: in group '%s': 'Entries' should be Table, is actually %s\n", group_name, lua_typename(L, lua_type(L, -1)));
					exit(1);
				}

				entry_count = get_entry_count(L);

				// check that the group has at least 1 entry
				if(entry_count <= 0) {
					printf("Skipping empty group '%s'\n", group_name);
					lua_pop(L, 1);
					// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
				}
				else {
					(*g)[count] = create_group(group_name, entry_count);

					// sort the entries if necessary
					if(sort) {
						sprintf(sort_cmd_lua, "table.sort(Groups[%d].Entries, function(a, b) return a.name < b.name end)", i);
						luaL_dostring(L, sort_cmd_lua);
					}

					// add entries to this group
					add_entries(L, (*g)[count]);
					lua_pop(L, 1);
					// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

					// set the launcher, if applicable
					lua_pushstring(L, "Launcher");
					// stack now contains: -1 => "Launcher"; -2 => key; -3 => value; -4 => key; -5 => table
					lua_gettable(L, -3);
					// stack now contains: -1 => Launcher; -2 => key; -3 => value; -4 => key; -5 => table
					if(lua_type(L, -1) == LUA_TSTRING) {
						set_gprog((*g)[count], lua_tostring(L, -1));
					}
					lua_pop(L, 1);
					// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

					// set the launcher flags, if applicable
					lua_pushstring(L, "Flags");
					// stack now contains: -1 => "Flags"; -2 => key; -3 => value; -4 => key; -5 => table
					lua_gettable(L, -3);
					// stack now contains: -1 => Flags; -2 => key; -3 => value; -4 => key; -5 => table
					if(lua_type(L, -1) == LUA_TSTRING) {
						set_gflags((*g)[count], lua_tostring(L, -1));
					}
					lua_pop(L, 1);
					// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

					++count;
				}
			}
		}
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
		++i;
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function	
}

void add_entries(lua_State *L, GROUP *g) {
	const char *entry_name;
	const char *entry_path;
	int entry_table_stack_index;
	int i = 1; // index in lua table
	int count = 0; // index in C struct

	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		if(lua_type(L, -1) == LUA_TNUMBER && lua_type(L, -2) == LUA_TTABLE) {
			// check that the entry has a valid name
			lua_pushstring(L, "name");
			// stack now contains: -1 => "name"; -2 => key; -3 => value; -4 => key; -5 => table
			lua_gettable(L, -3);
			// stack now contains: -1 => name; -2 => key; -3 => value; -4 => key; -5 => table
			entry_name = lua_tostring(L, -1);
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

			if(entry_name != NULL) {
				// get this entry's path, if applicable
				lua_pushstring(L, "path");
				// stack now contains: -1 => "path"; -2 => key; -3 => value; -4 => key; -5 => table
				lua_gettable(L, -3);
				// stack now contains: -1 => path; -2 => key; -3 => value; -4 => key; -5 => table
				if(lua_type(L, -1) == LUA_TSTRING) entry_path = lua_tostring(L, -1);
				else entry_path = entry_name;
				lua_pop(L, 1);
				// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

				set_gentry(g, count, create_entry(entry_name, entry_path, true));
				++count;
			}
		}
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
}

void stack_debug(lua_State *L) {
	int i;

	printf("DEBUGGING STACK:\n");
	for(i = 1; i <= lua_gettop(L); ++i) {
		printf("\t%d - %s", i, lua_typename(L, lua_type(L, i)));

		switch(lua_type(L, i)) {
			case LUA_TSTRING:
				printf(" - %s\n", lua_tostring(L, i));
				break;
				
			//case LUA_TTABLE:
			//	printf("\n");
			//	table_debug(L, 2);
			//	break;

			default:
				printf("\n");
		}
	}
}

// FIXME WIP debugging function
void table_debug(lua_State *L, int print_depth) {
	int i;

	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		const char *key = lua_tostring(L, -1);
		const char *value = lua_typename(L, lua_type(L, -2));
		for(i = 0; i < print_depth; ++i) {
			printf("\t");
		}
		printf("%s => %s", key, value);
		if(lua_type(L, -2) == LUA_TSTRING) {
			printf(" - %s", lua_tostring(L, -2));
		}
		printf("\n");
		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function	
}
