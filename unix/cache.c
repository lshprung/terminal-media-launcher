#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/cache.h"
#include "../include/read_cfg.h"

void save_to_cache(int g_hover, int e_hover, int true_hover, char *cfg_name){
	FILE *fp;
	char path[BUF_LEN];

	char *home = getenv("HOME");

	if(home == NULL){
		printf("Failed to save cache data: HOME is not set\n");
		return;
	}

	sprintf(path, "%s%c.cache%c", home, sep, sep);
	mkdir(path, 0755);

	sprintf(path, "%s%c.cache%ctml%c", home, sep, sep, sep);
	mkdir(path, 0755);

	sprintf(path, "%s%c.cache%ctml%cdata.bin", home, sep, sep, sep);

	//open cache file for writing
	fp = fopen(path, "wb");
	if(fp == NULL){
		printf("Failed to save cache data: could not open \"%s\"\n", path);
		return;
	}

	//write to file
	fwrite(&g_hover, sizeof(int), 1, fp);
	fwrite(&e_hover, sizeof(int), 1, fp);
	fwrite(&true_hover, sizeof(int), 1, fp);
	fwrite(cfg_name, sizeof(char), BUF_LEN, fp);

	fclose(fp);
	return;
}

void load_cache(int *g_hover, int *e_hover, int *true_hover, char *new_cfg_name){
	FILE *fp;
	char path[BUF_LEN];
	char saved_cfg_name[BUF_LEN];

	char *home = getenv("HOME");

	if(home == NULL){
		printf("Failed to load cached data: HOME is not set\n");
		return;
	}

	sprintf(path, "%s%c.cache%ctml%cdata.bin", home, sep, sep, sep);

	//open cache file for reading
	fp = fopen(path, "rb");
	if(fp == NULL){
		printf("Failed to load cached data: could not open \"%s\"\n", path);
		return;
	}

	//check if cfg_name matches; if not, do not load from cache
	fseek(fp, sizeof(int) * 3, SEEK_SET);
	fread(saved_cfg_name, sizeof(char), BUF_LEN, fp);

	if(!(strcmp(saved_cfg_name, new_cfg_name))){
		fseek(fp, 0, SEEK_SET);
		fread(g_hover, sizeof(int), 1, fp);
		fread(e_hover, sizeof(int), 1, fp);
		fread(true_hover, sizeof(int), 1, fp);
	}

	else{
		*g_hover = 0;
		*e_hover = 0;
		*true_hover = 0;
	}

	fclose(fp);
	return;
}
