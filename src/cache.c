#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/cache.h"
#include "include/read_cfg.h"

void save_to_cache(int g_count, int g_hover, int *e_hover, int *e_offset, int true_hover, char *cfg_name){
	FILE *fp;
	struct stat cfg_stat; //for determining last modification time of file
	char *path = get_cache_path(true);
	int i;

	//ensure get_cache_path() did not return NULL
	if(path == NULL) return;

	//open cache file for writing
	fp = fopen(path, "wb");
	if(fp == NULL){
		printf("Failed to save cache data: could not open \"%s\"\n", path);
		return;
	}

	//determine last modification time for cfg file
	stat(cfg_name, &cfg_stat);

	//write to file
	fwrite(cfg_name, sizeof(char), BUF_LEN, fp);
	fwrite(&cfg_stat.st_mtime, sizeof(long int), 1, fp);
	fwrite(&g_count, sizeof(int), 1, fp);
	fwrite(&g_hover, sizeof(int), 1, fp);
	for(i = 0; i < g_count; ++i){
		fwrite(&(e_hover[i]), sizeof(int), 1, fp);
		fwrite(&(e_offset[i]), sizeof(int), 1, fp);
	}
	fwrite(&true_hover, sizeof(int), 1, fp);

	fclose(fp);
	return;
}

void load_cache(int g_count, int *g_hover, int **e_hover, int **e_offset, int *true_hover, char *new_cfg_name){
	FILE *fp;
	char *path = get_cache_path(false);
	char saved_cfg_name[BUF_LEN];
	long int saved_cfg_mtime;
	struct stat new_cfg_stat;
	int saved_g_count;
	int i;

	//ensure get_cache_path() did not return NULL
	if(path == NULL) return;

	//open cache file for reading
	fp = fopen(path, "rb");
	if(fp == NULL){
		printf("Failed to load cached data: could not open \"%s\"\n", path);
		return;
	}

	//check that cfg_name matches and is not newer than the cached cfg; if not, do not load from cache
	fread(saved_cfg_name, sizeof(char), BUF_LEN, fp);
	fread(&saved_cfg_mtime, sizeof(long int), 1, fp);
	stat(new_cfg_name, &new_cfg_stat);

	if(!(strcmp(saved_cfg_name, new_cfg_name)) && saved_cfg_mtime >= new_cfg_stat.st_mtime){
		fread(&saved_g_count, sizeof(int), 1, fp);
		if(saved_g_count == g_count){
			fread(g_hover, sizeof(int), 1, fp);
			for(i = 0; i < g_count; ++i){
				fread(&(*e_hover)[i], sizeof(int), 1, fp);
				fread(&(*e_offset)[i], sizeof(int), 1, fp);
			}
			fread(true_hover, sizeof(int), 1, fp);
		}
	}

	fclose(fp);
	return;
}
