#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/cache.h"
#include "../include/read_cfg.h"

char *get_cache_path(bool create){
	char *path = malloc(sizeof(char) * BUF_LEN);
	char *appdata = getenv("APPDATA");

	if(appdata == NULL){
		printf("Failed to save cache data: APPDATA is not set\n");
		free(path);
		return NULL;
	}

	//if create is asserted, build the path to the file
	if(create){
		sprintf(path, "%s%cterminal-media-launcher%c", appdata, sep, sep);
		mkdir(path);
	}

	sprintf(path, "%s%cterminal-media-launcher%ccache.bin", appdata, sep, sep);

	return path;
}
