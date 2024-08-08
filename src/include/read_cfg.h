#ifndef READ_CFG_H
#define READ_CFG_H

#include <stdbool.h>

#define BUF_LEN 1024

bool cfg_interp(char *path);
bool get_sort();
bool get_case_sensitivity();
void refer_to_doc();
void addme(char *path, char *group, bool force, char *name);
int search_ch(char *str, char c);
int search_last_ch(char *str, char c);
int wild_cmp(char *wild, char *literal);
char *strip_quotes(char *str);
void error_mes(int ln, char *message);

//functions that differ by os
extern char sep;
char *find_config();
void mkconfig_wizard(char *path);
void handle_fname(char *path, char *group, bool recurs, bool force, char *name, int ln);

#endif
