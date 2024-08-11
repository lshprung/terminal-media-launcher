#ifndef READ_CFG_H
#define READ_CFG_H

#include <stdbool.h>

#include "group.h"

#define BUF_LEN 1024

GROUP **cfg_interp(char *path, int *group_count);
bool get_sort();
bool get_case_sensitivity();
void refer_to_doc();
//void addme(char *path, char *group, bool force, char *name);
//int search_ch(char *str, char c);
//int search_last_ch(char *str, char c);
//int wild_cmp(char *wild, char *literal);
//char *strip_quotes(char *str);
//void error_mes(int ln, char *message);

//functions that differ by os
extern char sep;
char *find_config();
void mkconfig_wizard(char *path);

#endif
