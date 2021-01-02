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
	char *home = getenv("HOME");

	assert(path != NULL);

	if(home == NULL){
		printf("Failed to save cache data: HOME is not set\n");
		free(path);
		return NULL;
	}

	//if create is asserted, build the path to the file
	if(create){
		sprintf(path, "%s%c.cache%c", home, sep, sep);
		mkdir(path, 0755);

		sprintf(path, "%s%c.cache%ctml%c", home, sep, sep, sep);
		mkdir(path, 0755);
	}

	sprintf(path, "%s%c.cache%ctml%cdata.bin", home, sep, sep, sep);

	return path;
}
