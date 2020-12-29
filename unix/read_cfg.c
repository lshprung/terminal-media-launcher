#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/read_cfg.h"
#include "../include/entry.h"

char sep = '/';

char *find_config(){
	char *home = getenv("HOME");
	char *path = malloc(sizeof(char) * BUF_LEN);
	char choices[2][BUF_LEN];
	int check_count = 2;
	int i;

	sprintf(choices[0], "%s%c.config%ctml%cconfig", home, sep, sep, sep);
	sprintf(choices[1], "%s%c.tml%cconfig", home, sep, sep);

	for(i = 0; i < check_count; i++){
		path = choices[i];
		printf("Checking for config at %s: ", choices[i]);
		if(access(path, R_OK) == 0){
			printf("Using config \"%s\"\n\n", path);
			return path;
		}
		else printf("File does not exist\n");
	}

	//TODO no config exists, ask user if they want to autogenerate one
	mkconfig_wizard(choices[0]);
	path = choices[0];
	return path;
}

void mkconfig_wizard(char *path){
	char input;
	FILE *fp;

	char *home = getenv("HOME");

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
	if(home == NULL){
		printf("Failed: HOME is NULL\n");
		exit(1);
	}

	sprintf(path, "%s%c.config%c", home, sep, sep);
	mkdir(path, 0755);

	sprintf(path, "%s%c.config%ctml%c", home, sep, sep, sep);
	mkdir(path, 0755);

	sprintf(path, "%s%c.config%ctml%cconfig", home, sep, sep, sep);

	//open file for writing, make sure non-NULL
	fp = fopen(path, "w");
	if(fp == NULL){
		printf("Failed: \"%s\" could not be open for writing\n", path);
		exit(1);
	}

	//write to file
	fprintf(fp, "# This file was auto-generated by tml. See docs/tml-config.md or tml-config(5) for documentation\n"
	"# The default launcher is set to \"xdg-open\" which will open files based on the relevant default application set through xdg\n\n"
	"# Recursively add files from %s%cMusic%c to Music group\n"
	"addGroup Music\n"
	"setLauncher Music xdg-open\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.aac Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.aiff Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.alac Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.au Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.flac Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.m4a Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.mp3 Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.ogg Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.pcm Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.wav Music\n", home, sep, sep);
	fprintf(fp, "addR %s%cMusic%c*.wma Music\n\n", home, sep, sep);
	fprintf(fp, "# Recursively add files from %s%cPictures%c to Pictures group\n"
	"addGroup Pictures\n"
	"setLauncher Pictures xdg-open\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.epi Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.eps Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.eps2 Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.eps3 Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.epsf Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.epsi Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.ept Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.gif Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.gfa Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.giff Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.jpeg Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.jpg Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.png Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.svg Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.svgz Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.tif Pictures\n", home, sep, sep);
	fprintf(fp, "addR %s%cPictures%c*.tiff Pictures\n\n", home, sep, sep);
	fprintf(fp, "# Recursively add files from %s%cVideos%c to Videos group\n"
	"addGroup Videos\n"
	"setLauncher Videos xdg-open\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.asf Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.avi Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.flv Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.mk3d Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.mkv Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.mov Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.mp4 Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.qt Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.webm Videos\n", home, sep, sep);
	fprintf(fp, "addR %s%cVideos%c*.wmv Videos\n", home, sep, sep);

	fclose(fp);
	printf("done\nIt is highly recommended to further tweak the configuration file! [press any key to continue]");
	fflush(stdout);
	getchar();
	getchar();

	return;
}

//TODO augment to involve recurs
//TODO could use some cleanup...
void handle_fname(char *path, char *group, bool recurs, bool force, char *name, int ln){
	ENTRY *new;
	char *search; //pointer for traversing path
	char full_path_cpy[BUF_LEN];
	char relative_path_cpy[BUF_LEN];
	char arg_cpy[BUF_LEN];
	char auto_name[BUF_LEN];
	int plen = strlen(path);
	char *dirname;
	char *local_arg; //for use in addR
	DIR *dp;
	struct dirent *fname;
	int i;
	char *error_p; //helper for complex error messages

	assert(path != NULL && group != NULL);

	if(path[0] == '\0' || group[0] == '\0'){
		error_mes(ln, "Too few arguments for \"add\"");
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

					//check if path is a file (and not a directory/symlink/etc.) and regex matches
					if(fname->d_type == DT_REG && !(wild_cmp(&arg_cpy[i+1], fname->d_name))) addme(relative_path_cpy, group, force, name);

					//if the recursive option was specified and the path is a directory, run handle_fname on this directory, but for security reasons, do not consider directories that start with a '.'
					else if(recurs && fname->d_type == DT_DIR && fname->d_name[0] != '.'){
						i = search_last_ch(arg_cpy, sep);
						local_arg = &arg_cpy[i+1];
						strcat(relative_path_cpy, &sep);
						strcat(relative_path_cpy, local_arg);
						handle_fname(relative_path_cpy, group, 1, 0, NULL, ln);
					}

				}

				closedir(dp);
			}

			//directory is not real, report error to the user
			else{
				error_p = malloc(sizeof(char) * 1024);
				sprintf(error_p, "\"%s\" bad path", dirname);
				error_mes(ln, error_p);
				free(error_p);
				//printf("Error: \"%s\" bad path\n", dirname);
			}
		}

		//path is not real, report error to the user
		else{
			error_p = malloc(sizeof(char) * 1024);
			sprintf(error_p, "\"%s\" bad path", full_path_cpy);
			error_mes(ln, error_p);
			free(error_p);
		}
	}

	//file name is okay
	//FIXME does not take into account whether the argument is a file (could be a directory, symlink, etc.)
	else addme(full_path_cpy, group, force, name);

	return;
}
