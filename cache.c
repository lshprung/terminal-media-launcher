#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/cache.h"
#include "include/read_cfg.h"

void save_to_cache(int g_hover, int e_hover, int true_hover, char *cfg_name){
	FILE *fp;
	char *path = get_cache_path(true);

	//ensure get_cache_path() did not return NULL
	if(path == NULL) return;

	//open cache file for writing
	fp = fopen(path, "wb");
	if(fp == NULL){
		printf("Failed to save cache data: could not open \"%s\"\n", path);
		return;
	}

	//write to file
	fwrite(cfg_name, sizeof(char), BUF_LEN, fp);
	fwrite(&g_hover, sizeof(int), 1, fp);
	fwrite(&e_hover, sizeof(int), 1, fp);
	fwrite(&true_hover, sizeof(int), 1, fp);

	fclose(fp);
	return;
}

void load_cache(int *g_hover, int *e_hover, int *true_hover, char *new_cfg_name){
	FILE *fp;
	char *path = get_cache_path(false);
	char saved_cfg_name[BUF_LEN];

	//ensure get_cache_path() did not return NULL
	if(path == NULL) return;

	//open cache file for reading
	fp = fopen(path, "rb");
	if(fp == NULL){
		printf("Failed to load cached data: could not open \"%s\"\n", path);
		return;
	}

	//check if cfg_name matches; if not, do not load from cache
	fread(saved_cfg_name, sizeof(char), BUF_LEN, fp);

	if(!(strcmp(saved_cfg_name, new_cfg_name))){
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
