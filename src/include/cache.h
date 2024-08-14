#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>

void save_to_cache(int g_count, int g_hover, int *e_hover, int *e_offset, int true_hover, char *cfg_name);

void load_cache(int g_count, int *g_hover, int **e_hover, int **e_offset, int *true_hover, char *new_cfg_name);

char *get_cache_path(bool create);

#endif
