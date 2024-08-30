#ifndef READ_CFG_H
#define READ_CFG_H

#include <stdbool.h>

#include "group.h"

#define BUF_LEN 1024

GROUP **cfg_interp(char *path, int *group_count);
void refer_to_doc();

//functions that differ by os
extern char sep;
char *find_config();
void mkconfig_wizard(char *path);

#endif
