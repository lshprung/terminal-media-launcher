#include <stdlib.h>
#include <string.h>

#include "../include/draw.h"

void launch(){
	char full_command[BUF_LEN];

	strcpy(full_command, get_launch());
	strcat(full_command, " > /dev/null 2>&1 &");
	system(full_command);

	return;
}
