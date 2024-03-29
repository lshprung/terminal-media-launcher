#include <stdbool.h>
#include <windows.h>

#include "../include/draw.h"
#include "../include/entry.h"
#include "../include/group.h"

void launch(){
	char *program = get_gprog(g[g_hover]);
	char *flags = get_gflags(g[g_hover]);
	char *path = get_epath(e[e_hover[g_hover]]);
	bool quotes = get_gquotes(g[g_hover]);
	char file[BUF_LEN];
	char params[BUF_LEN];

	file[0] = '\0';

	if(!(strcmp(program, "./"))){
		strcat(file, "/C ");
		strcat(file, "\"");
		strcat(file, path);
		strcat(file, "\"");
		ShellExecute(NULL, NULL, "cmd.exe", file, NULL, SW_HIDE);
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
