#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/entry.h"
#include "../include/read_cfg.h"

char sep = '\\';

char *find_config(){
	char *appdata = getenv("APPDATA");
	char *path = malloc(sizeof(char) * BUF_LEN);
	int check_count = 1;
	char choices[check_count][BUF_LEN];
	int i;

	sprintf(choices[0], "%s%cterminal-media-launcher%cconfig.lua", appdata, sep, sep);

	for(i = 0; i < check_count; i++){
		strcpy(path, choices[i]);
		printf("Checking for config at %s: ", choices[i]);
		if(access(path, R_OK) == 0){
			printf("Using config \"%s\"\n\n", path);
			return path;
		}
		else printf("File does not exist\n");
	}

	//TODO no config exists, ask user if they want to autogenerate one
	mkconfig_wizard(choices[0]);
	strcpy(path, choices[0]);
	return path;
}

void mkconfig_wizard(char *path){
	char input;
	FILE *fp;

	char *home = getenv("USERPROFILE");
	char *appdata = getenv("APPDATA");

	printf("\nNo configuration file found. Auto-generate one now at \"%s\"? [Y/n] ", path);
	fflush(stdout);
	scanf(" %c", &input);

	if(input == 'n'){
		printf("Configuration will not be auto-generated\n");
		refer_to_doc();
		exit(0);
		
	}

	printf("Generating configuration file at \"%s\"...\n", path);

	//ensure directories have been created
	if(appdata == NULL){
		printf("Failed: \%APPDATA\% is NULL\n");
		exit(1);
	}

	if(home == NULL){
		printf("Failed: \%USERPROFILE\% is NULL\n");
		exit(1);
	}

	sprintf(path, "%s%cterminal-media-launcher%c", appdata, sep, sep);
	mkdir(path);

	sprintf(path, "%s%cterminal-media-launcher%cconfig.lua", appdata, sep, sep);

	//open file for writing, make sure non-NULL
	fp = fopen(path, "w");
	if(fp == NULL){
		printf("Failed: \"%s\" could not be open for writing\n", path);
		exit(1);
	}

	fprintf(fp, 
		"-- This file was auto-generated by terminal-media-launcher. See docs\\terminal-media-launcher-config.md or terminal-media-launcher-config(5) for documentation\n"
		"-- By default, no launcher is specified for any group. When no launcher is specified on the Windows build of terminal-media-launcher, media files will be opened with their default application.\n\n"
		"local lfs = require \"lfs\"\n"
		"\n"
		"local function addGroup(name, launcher, flags)\n"
		"	assert(type(name) == \"string\")\n"
		"\n"
		"    -- create Groups table if needed\n"
		"    if Groups == nil then\n"
		"        Groups = {}\n"
		"    end\n"
		"\n"
		"    local new_group = {}\n"
		"    new_group.name = name\n"
		"	new_group.Entries = {}\n"
		"    if launcher ~= nil then\n"
		"        new_group.Launcher = launcher\n"
		"    end\n"
		"    if flags ~= nil then\n"
		"        new_group.Flags = flags\n"
		"    end\n"
		"\n"
		"    table.insert(Groups, new_group)\n"
		"    return new_group\n"
		"end\n"
		"\n"
		"local function addEntries(parentGroup, startDir, filePattern, recursive)\n"
		"	-- recursive arg is a boolean for whether or not to descend into subdirectories (false by default)\n"
		"	assert(type(parentGroup) == \"table\")\n"
		"	assert(type(parentGroup.Entries) == \"table\")\n"
		"	assert(type(startDir) == \"string\")\n"
		"	assert(type(filePattern) == \"string\")\n"
		"\n"
		"	for file in lfs.dir(startDir) do\n"
		"		local fullFilePath = startDir .. \"\\\\\" .. file\n"
		"		if file ~= \".\" and file ~= \"..\" then\n"
		"			-- descend into subdirectory if recursive is set to true\n"
		"			if lfs.attributes(fullFilePath).mode == \"directory\" and recursive == true then\n"
		"				addEntries(parentGroup, fullFilePath, filePattern, recursive)\n"
		"			elseif lfs.attributes(fullFilePath).mode == \"file\" then\n"
		"				if string.match(file, filePattern) then\n"
		"					table.insert(parentGroup.Entries, {\n"
		"						name = file,\n"
		"						path = '\"' .. fullFilePath .. '\"'\n"
		"					})\n"
		"				end\n"
		"			end\n"
		"		end\n"
		"	end\n"
		"end\n"
		"\n"
		"local music = addGroup(\"Music\")\n"
		"addEntries(music, os.getenv(\"USERPROFILE\") .. \"\\\\Music\", \".*\", true)\n"
		"\n"
		"local pictures = addGroup(\"Pictures\")\n"
		"addEntries(pictures, os.getenv(\"USERPROFILE\") .. \"\\\\Pictures\", \".*\", true)\n"
		"\n"
		"local videos = addGroup(\"Videos\")\n"
		"addEntries(videos, os.getenv(\"USERPROFILE\") .. \"\\\\Videos\", \".*\", true)\n");

	fclose(fp);
	printf("done\nIt is highly recommended to further tweak the configuration file! [press any key to continue]");
	fflush(stdout);
	getchar();
	getchar();

	return;
}
