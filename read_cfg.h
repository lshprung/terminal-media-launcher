#ifndef READ_CFG_H
#define READ_CFG_H

#if defined _WIN32 || defined _WIN64
char *find_config_win();
#else
char *find_config();
#endif

void cfg_interp();

bool get_sort();

bool get_case_sensitivity();

void refer_to_doc();

#endif
