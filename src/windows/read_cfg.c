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

	sprintf(choices[0], "%s%cterminal-media-launcher%cconfig", appdata, sep, sep);

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

	sprintf(path, "%s%cterminal-media-launcher%cconfig", appdata, sep, sep);

	//open file for writing, make sure non-NULL
	fp = fopen(path, "w");
	if(fp == NULL){
		printf("Failed: \"%s\" could not be open for writing\n", path);
		exit(1);
	}

	fprintf(fp, "# This file was auto-generated by terminal-media-launcher. See docs\\terminal-media-launcher-config.md or terminal-media-launcher-config(5) for documentation\n"
	"# By default, no launcher is specified for any group. When no launcher is specified on the Windows build of terminal-media-launcher, media files will be opened with their default application.\n"
	"# It is generally recommended to specify a launcher for groups containing media files using the \"setLauncher\" command\n\n"
	"# Recursively add files from %s%cMusic%c to Music group\n"
	"addGroup Music\n", home, sep, sep);
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
	"addGroup Pictures\n", home, sep, sep);
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
	"addGroup Videos\n", home, sep, sep);
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

					//Windows cannot tell file types (TODO), so just add relatively indiscriminantly
					if(!(wild_cmp(&arg_cpy[i+1], fname->d_name))) addme(relative_path_cpy, group, force, name);

					//if the recursive option was specified, run handle_fname on this directory, but for security reasons, do not consider directories that start with a '.'
					else if(recurs && fname->d_name[0] != '.'){
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
			else if(!recurs){
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
