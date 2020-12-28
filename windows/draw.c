#include <stdbool.h>
#include <windows.h>

#include "../draw.h"
#include "../entry.h"
#include "../group.h"
#include "draw.h"

void launch(){
	char *program = get_gprog(g[g_hover]);
	char *flags = get_gflags(g[g_hover]);
	char *path = get_epath(e[e_hover]);
	bool quotes = get_gquotes(g[g_hover]);
	char file[BUF_LEN];
	char params[BUF_LEN];

	file[0] = '\0';

	if(!(strcmp(program, "./"))){
		strcat(file, "\"");
		strcat(file, path);
		strcat(file, "\"");
		ShellExecute(NULL, NULL, file, NULL, NULL, SW_SHOW);
	}

	else{
		if(quotes) strcat(file, "\"");
		strcat(file, program);
		if(quotes) strcat(file, "\"");

		params[0] = '\0';
		if(flags[0] != '\0'){
			strcat(params, flags);
			strcat(params, " ");
		}
		strcat(params, "\"");
		strcat(params, path);
		strcat(params, "\"");
		
		ShellExecute(NULL, NULL, file, params, NULL, SW_SHOW);
	}

	return;
}
