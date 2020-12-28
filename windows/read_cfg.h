#ifndef WINDOWS_READ_CFG_H
#define WINDOWS_READ_CFG_H

extern char sep;

char *find_config();

void mkconfig_wizard();

void handle_fname(char *path, char *group, bool recurs, bool force, char *name, int ln);

#endif
