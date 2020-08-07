//Windows Compatability
#if defined _WIN32 || defined _WIN64
#include <ncurses/ncurses.h>
#include <windows.h>
#else
#include <ncurses.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "entry.h"
#include "group.h"
#include "read_cfg.h"
#define MAX_LEN 6
#define BUF_LEN 1024

void draw_title();
void draw_win(WINDOW *new, char *title);
void fill_groups();
void fill_entries();
char *trim_name(char *name, char *path, int max_len);
void update_col(int mode, int hl_where); //0 = last, 1 = first; 0 = GROUP, 1 = ENTRY, 2 = INFO
void switch_col();
void trav_col(int dir); //0 = down, 1 = up
char *get_launch();
char *compat_convert(char *path, int mode);
#if defined _WIN32 || defined _WIN64
void win_launch();
#endif

static int width;
static int height;
WINDOW *group_win;
WINDOW *entry_win;
WINDOW *info_win;
int g_hover = 0;
int e_hover = 0;
int true_hover = 0; //which column are we currently hovering on? (I think)
GROUP **g;
ENTRY **e;
int g_count;
int e_count;
int g_offset = 0;
int e_offset = 0;

//TODO consider figuring out where some refreshes are unecessary

int main(int argc, char **argv){
	char *cfg_path;
	bool tall = true; //is the window a certain height (tbd what the threshold should be TODO)
	bool wide = true; //is the window a certain width (tbd what the threshold should be TODO)
	int input;
	char full_command[BUF_LEN]; //what will be executed

	//if a config path was given as an argument, set it accordingly
	if(argc > 2 && (!strcmp(argv[1], "-c") || !strcmp(argv[1], "--cfg_path"))) cfg_path = argv[2];
	else cfg_path = find_config();

	//Fill Groups
	cfg_interp(cfg_path); //read the contents of the cfg file

	//Remove Empty Groups from the Array
	clean_groups();
	g = get_groups(); //retrieve results of cfg_interp
	g_count = get_gcount(g); //retrieve number of groups in g (only do this after removing empty groups)

	initscr();
	cbreak();
	keypad(stdscr, true);
	start_color();

	width = getmaxx(stdscr);
	height = getmaxy(stdscr);
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	attron(COLOR_PAIR(0));

	//title at the top (Terminal Media Launcher) (23 chars)
	draw_title();

	//TODO the search bar is a shelved feature for now, might consider writing it...
	//Draw Search Bar 2 spaces (3 spaces if window is big enough) under the title
	/*
	move(3, width/4);
	printw("[ Search: ");
	move(3, (width*3)/4);
	printw("]");
	move(3, (width/4)+10);
	*/

	//Draw Columns 
	//TODO create conditionals based on size of window)
	group_win = newwin(height-7, width/2, 4, 0);
	entry_win = newwin(height-7, width/2, 4, width/2);
	info_win = newwin(3, width, height-3, 0);
	refresh();
	draw_win(group_win, "GROUP");
	draw_win(entry_win, "ENTRY");
	draw_win(info_win, "INFO");
	update_col(0, 1);

	//start with hover on the first group, draw the entries from the selected group, true_hover is over the groups (rather than the entries)
	update_col(1, 1);
	update_col(2, 0);
	curs_set(0); //hide the cursor
	//move(3, (width/4)+10);

	//drawing is done, now run a while loop to receive input
	while(1){
		input = getch();

		switch(input){
			case 9: //tab key
			case KEY_LEFT:
			case KEY_RIGHT:
				//switch true_hover to look at the other column (TODO this code could use some polish)
				switch_col();
				break;

			case KEY_DOWN:
				trav_col(0);
				break;

			case KEY_UP:
				trav_col(1);
				break;

			case 10: //enter key

#if defined _WIN32 || defined _WIN64
				win_launch();
#else
				//TODO create option to redirect stdout and stderr to avoid writing over the ncurses instance
				strcpy(full_command, get_launch());
				strcat(full_command, " > /dev/null 2>&1");
				system(full_command);
#endif
				refresh();
				break;

			case 'q':
				endwin();
				return 0;

		}

		//move(3, (width/4)+10); //reset cursor location to the search bar after any action
	}
	
	endwin();
	return 0;
}

void draw_title(){
	WINDOW *title;

	title = newwin(2, width, 0, 0);
	refresh();
	attron(A_BOLD | A_UNDERLINE);
	move(0, (width-23)/2);
	printw("Terminal Media Launcher");
	attroff(A_BOLD | A_UNDERLINE);
	box(title, 0, 0);
	wrefresh(title);

	return;
}

void draw_win(WINDOW *new, char *title){
	int title_len = strlen(title);

	box(new, 0, 0);
	attron(A_UNDERLINE);
	wmove(new, 0, (new->_maxx - title_len)/2);
	wprintw(new, "%s", title);
	attroff(A_UNDERLINE);
	wrefresh(new);

	return;
}

void fill_groups(){
	int i;
	int max_len = group_win->_maxx-1; //longest possible string length that can be displayed in the window
	int ycoord = 1;
	int max_y = height-8; //last possible slot to draw a group
	char *name;

	for(i = 0+g_offset; i < g_count; i++){
		if(ycoord >= max_y) break; //already at bottom of terminal window, stop drawing
		//TODO account for empty groups with a removal function
		name = get_gname(g[i]);

		//the name is too long, take the group to the trimming function
		if(strlen(name) > max_len) name = trim_name(name, get_gname(g[i]), max_len);
		wmove(group_win, ycoord, 1);
		wprintw(group_win, "%s", name);
		ycoord++;
	}

	wrefresh(group_win);
	return;
}

//very similar to the previous function, perhaps they can be combined... (TODO)
void fill_entries(){
	int i;
	int max_len = entry_win->_maxx-1; //longest possible string length that can be displayed in the window
	int ycoord = 1;
	int max_y = height-8;
	char *name;

	for(i = 0+e_offset; i < e_count; i++){
		if(ycoord >= max_y) break; //reached the bottom of the terminal window, stop drawing
		name = get_ename(e[i]);

		//the name is too long, take the group to the trimming function
		if(strlen(name) > max_len) name = trim_name(name, get_epath(e[i]), max_len);
		wmove(entry_win, ycoord, 1);
		wprintw(entry_win, "%s", name);
		ycoord++;
	}

	wrefresh(entry_win);
	return;
}

char *trim_name(char *name, char *path, int max_len){
	char *relative;

	//group name and path are equivalent: special procedure
	if(!(strcmp(name, path))){
		//find relative path name
		relative = strrchr(name, '/');
		name = relative+1;
		if(strlen(name) <= max_len) return name;
	}

	name[max_len] = '\0';
	return name;
}

//FIXME info printing (mode = 2) is not uniformally in the box
void update_col(int mode, int hl_where){
	//mode 0 = group
	//mode 1 = entry
	//mode 2 = info

	WINDOW *col;
	char *name;
	int name_len;
	int y_hl;
	char *execution;

	switch(mode){
		case 0:
			col = group_win;
			name = "GROUP";
			name_len = 5;
			break;

		case 1:
			col = entry_win;
			name = "ENTRY";
			name_len = 5;
			break;

		case 2:
			col = info_win;
			name = "EXECUTION";
			name_len = 9;
			break;


		default:
			return;
	}

	y_hl = (hl_where ? 1 : col->_maxy-1);

	//reset the column window (including reboxing and redrawing the title)
	wclear(col);
	box(col, 0, 0);
	wmove(col, 0, (col->_maxx - name_len)/2);
	wprintw(col, name);
	wrefresh(col);

	switch(mode){
		case 0:
			fill_groups();
			mvwchgat(group_win, y_hl, 1, group_win->_maxx-1, A_DIM, 2, NULL);
			break;

		case 1:
			e_count = get_ecount(g[g_hover]);
			e = get_entries(get_ghead(g[g_hover]), e_count);
			fill_entries(e, e_count);
			mvwchgat(entry_win, y_hl, 1, entry_win->_maxx-1, A_DIM, 1, NULL);
			break;

		default:
			execution = get_launch();
			if(strlen(execution) >= info_win->_maxx){
				execution[info_win->_maxx - 1] = '\0';
				execution[info_win->_maxx - 2] = '.';
				execution[info_win->_maxx - 3] = '.';
				execution[info_win->_maxx - 4] = '.';
			}
			mvwprintw(info_win, 1, 1, execution);

	}

	wrefresh(col);
	return;
}

void switch_col(){
	true_hover = (true_hover+1) % 2;
	if(true_hover){
		mvwchgat(group_win, 1+g_hover, 1, group_win->_maxx-1, A_DIM, 1, NULL); //adjust group light
		mvwchgat(entry_win, 1+e_hover, 1, entry_win->_maxx-1, A_DIM, 2, NULL); //adjust entry light
	}
	else{
		mvwchgat(group_win, 1+g_hover, 1, group_win->_maxx-1, A_DIM, 2, NULL); //adjust group light
		mvwchgat(entry_win, 1+e_hover, 1, entry_win->_maxx-1, A_DIM, 1, NULL); //adjust entry light
	}
	move(3, (width/4)+10);

	wrefresh(group_win);
	wrefresh(entry_win);
	return;
}

void trav_col(int dir){
	int *focus = (true_hover ? &e_hover : &g_hover); //make it easy to know which column we are looking at
	int *offset = (true_hover ? &e_offset : &g_offset); //this variable is broken
	int count = (true_hover ? e_count : g_count);
	int max_hl = height-5+*offset; //for some reason, this works
	int min_hl = 5+*offset;
	bool oob_flag = false;

	//check if the traversal is valid (i.e. not at top/bottom), exit if not
	if((dir && !(*focus)) || (!dir && (*focus == count-1))) return;

	//reset previously highlighted entry and group, change focus
	mvwchgat(entry_win, 1+e_hover-e_offset, 1, entry_win->_maxx-1, A_NORMAL, 0, NULL);
	mvwchgat(group_win, 1+g_hover-g_offset, 1, group_win->_maxx-1, A_NORMAL, 0, NULL);
	(dir ? (*focus)-- : (*focus)++);

	//check offsets relating to new highlight, make sure highlight did not go oob
	if(*focus+5 > max_hl){
		*focus--;
		(*offset)++;
		oob_flag = true;
	}
	else if(*focus+5 < min_hl){
		*focus++;
		(*offset)--;
		oob_flag = true;
	}

	if(oob_flag) (true_hover ? update_col(1, dir) : update_col(0, dir));

	//highlight newly hovered upon entry/group
	mvwchgat(entry_win, 1+e_hover-e_offset, 1, entry_win->_maxx-1, A_DIM, (true_hover ? 2 : 1), NULL);
	mvwchgat(group_win, 1+g_hover-g_offset, 1, group_win->_maxx-1, A_DIM, (true_hover ? 1 : 2), NULL);
	if(!true_hover){ //a little extra work regarding group hover
		e_offset = 0;
		update_col(1, 1);
		e_hover = 0;
	}

	wrefresh(group_win);
	wrefresh(entry_win);
	update_col(2, 0);
	return;
}

//FIXME account for how Windows does things: ""program.exe" [flags] "file""
	//There is an issue with flags: needs to be able to have quotation marks
char *get_launch(){
	char *program = get_gprog(g[g_hover]);
	char *flags = get_gflags(g[g_hover]);
	char *path = get_epath(e[e_hover]);
	bool force = get_eforce(e[e_hover]);
	int mode = get_compmode();
	char *full_command = malloc(sizeof(char) * BUF_LEN);

	full_command[0] = '\0';

	//if the entry is an executable file (doesn't have a launcher)
	if(!(strcmp(program, "./"))){
		strcat(full_command, "\"");
		strcat(full_command, path);
		strcat(full_command, "\"");
	}

	else{
		//if the entry is not forced and compatability mode is on, run it through the converter function
		if(mode != 0 && !force) path = compat_convert(path, mode);
		strcat(full_command, "\"");
		strcat(full_command, program);
		strcat(full_command, "\"");
		if(flags[0] !='\0'){
			strcat(full_command, " ");
			strcat(full_command, flags);
		}
		strcat(full_command, " ");
		strcat(full_command, "\"");
		strcat(full_command, path);
		strcat(full_command, "\"");
	}

	return full_command;

}

char *compat_convert(char *path, int mode){
	char *new = malloc(sizeof(char) * BUF_LEN);
	char *trav = new;

	//1 -> WSL: letter is in /mnt/, convert slashes to backslashes
	if(mode == 1){
		path = path+5;
		*trav = *path - 32; //point at letter, make it uppercase
		trav++;
		*trav = ':';
		path++;
		trav++;

		//convert each character
		while(*path != '\0'){
			if(*path == '/') *trav = '\\';
			else *trav = *path;
			path++;
			trav++;
		}
		*trav = '\0';
	}

	else printf("Error: mode should not be %d\n", mode);

	return new;
}

#if defined _WIN32 || defined _WIN64
void win_launch(){
	char *program = get_gprog(g[g_hover]);
	char *flags = get_gflags(g[g_hover]);
	char *path = get_epath(e[e_hover]);
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
		strcat(file, "\"");
		strcat(file, program);
		strcat(file, "\"");

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
#endif
