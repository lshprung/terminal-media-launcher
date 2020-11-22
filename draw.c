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
#define GAP_SIZE 1
#define WIDTH (getmaxx(stdscr)) //width of the entire term
#define HEIGHT (getmaxy(stdscr)) //height of the entire term

void update_display(bool resize);
void draw_title();
void draw_win(WINDOW *new, char *title);
void fill_col(int mode); //0 = GROUP, 1 = ENTRY
char *trim_name(char *name, char *path, int max_len);
void update_col(int mode, int hl_where, bool resize); //0 = last, 1 = first; 0 = GROUP, 1 = ENTRY, 2 = INFO
void switch_col();
void trav_col(int new_i);
int locateChar(char input);
char *get_launch();
char *compat_convert(char *path, int mode);
#if defined _WIN32 || defined _WIN64
void win_launch();
#endif

WINDOW *group_win = NULL;
WINDOW *entry_win = NULL;
WINDOW *info_win = NULL;
int g_hover = 0;
int e_hover = 0;
int true_hover = 0; //0 = hovering on groups, 1 = hovering on entries
GROUP **g;
ENTRY **e;
int g_count;
int e_count;
int g_offset = 0;
int e_offset = 0;

int main(int argc, char **argv){
	char cfg_path[BUF_LEN];
	int input;
	char full_command[BUF_LEN]; //what will be executed
	int prev_width; //used to check if the window was resized
	int prev_height; //used to check if the window was resized
	int i;

	//if a config path was given as an argument, set it accordingly
	if(argc > 2 && (!strcmp(argv[1], "-c") || !strcmp(argv[1], "--cfg_path"))) strcpy(cfg_path, argv[2]);
	else strcpy(cfg_path, find_config());

	//Fill Groups
	cfg_interp(cfg_path); //read the contents of the cfg file

	//Remove Empty Groups from the Array
	clean_groups();
	g = get_groups(); //retrieve results of cfg_interp
	g_count = get_gcount(g); //retrieve number of groups in g (only do this after removing empty groups)

	//check that there are is at least one valid group
	if(g_count == 0){
		printf("Error: No Entries!\n");
		exit(0);
	}

	initscr();
	cbreak();
	keypad(stdscr, true);
	noecho();
	set_escdelay(1); //used to avoid delay after ESC is pressed
	start_color();

	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	attron(COLOR_PAIR(0));

	prev_width = WIDTH;
	prev_height = HEIGHT;

	update_display(false);

	//drawing is done, now run a while loop to receive input
	while(1){
		input = getch();

		switch(input){
			case 9: //tab key
			case KEY_LEFT:
			case KEY_RIGHT:
				//switch true_hover to look at the other column
				switch_col();
				break;

			case KEY_DOWN:
				trav_col((true_hover ? e_hover : g_hover)+1);
				break;

			case KEY_UP:
				trav_col((true_hover ? e_hover : g_hover)-1);
				break;

			case KEY_PPAGE:
			//case KEY_SUP:
				trav_col(0);
				break;

			case KEY_NPAGE:
			//case KEY_SDOWN:
				trav_col((true_hover ? e_count : g_count)-1);
				break;

			case KEY_F(5):
			//manual refresh (for debug purposes)
				update_display(true);
				break;

			case 10: //enter key

#if defined _WIN32 || defined _WIN64
				win_launch();
#else
				strcpy(full_command, get_launch());
				strcat(full_command, " > /dev/null 2>&1 &");
				system(full_command);
#endif
				break;

			case 27: //escape key
				endwin();
				return 0;

			default: //a search char was entered, locate where to jump to
				trav_col(locateChar(input));
		}

		//redraw all windows if term is resized
		if(prev_width != WIDTH || prev_height != HEIGHT) update_display(true);

		//update prevs
		prev_width = WIDTH;
		prev_height = HEIGHT;
	}
	
	endwin();
	return 0;
}

void update_display(bool resize){
	if(WIDTH < 20 || HEIGHT < 6){
		endwin();
		printf("Unable to draw: Terminal Window is Too Small\n");
		exit(0);
	}

	//title at the top (Terminal Media Launcher) (23 chars)
	draw_title();

	if(group_win != NULL) delwin(group_win);
	if(entry_win != NULL) delwin(entry_win);
	if(info_win != NULL) delwin(info_win);

	//Draw Columns 
	group_win = newwin(HEIGHT-(5+GAP_SIZE), WIDTH/2, 2+GAP_SIZE, 0);
	entry_win = newwin(HEIGHT-(5+GAP_SIZE), WIDTH-WIDTH/2, 2+GAP_SIZE, WIDTH/2);
	info_win = newwin(3, WIDTH, HEIGHT-3, 0);
	draw_win(group_win, "GROUP");
	draw_win(entry_win, "ENTRY");
	draw_win(info_win, "INFO");
	update_col(0, 1, resize);

	//start with hover on the first group, draw the entries from the selected group, true_hover is over the groups (rather than the entries)
	update_col(1, 1, resize);
	update_col(2, 0, resize);
	curs_set(0); //hide the cursor
	//move(3, (WIDTH/4)+10);
	refresh();
}

void draw_title(){
	WINDOW *title;

	title = newwin(2, WIDTH, 0, 0);
	refresh();
	attron(A_BOLD | A_UNDERLINE);
	move(0, (WIDTH-23)/2);
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

void fill_col(int mode){
	//mode 0 = group
	//mode 1 = entry

	int i;
	WINDOW *col = (mode ? entry_win : group_win);
	int count = (mode ? e_count : g_count);
	int offset = (mode ? e_offset : g_offset);
	int max_len = col->_maxx-1; //longest possible string length that can be displayed in the window
	int ycoord = 1;
	int max_y = HEIGHT-(6+GAP_SIZE);
	char *name;

	for(i = 0+offset; i < count; i++){
		if(ycoord >= max_y) break; //reached the bottom of the terminal window, stop drawing
		name = (mode ? get_ename(e[i]) : get_gname(g[i]));

		//the name is too long, take the group to the trimming function
		if(strlen(name) > max_len) name = trim_name(name, (mode ? get_epath(e[i]) : get_gname(g[i])), max_len);
		wmove(col, ycoord, 1);
		wprintw(col, "%s", name);
		ycoord++;
	}

	wrefresh(col);
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

void update_col(int mode, int hl_where, bool resize){
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

	//update certain info in the col only if not a resizing-related call
	switch(mode){
		case 0:
			fill_col(0);
			if(!resize) mvwchgat(group_win, y_hl, 1, group_win->_maxx-1, A_DIM, 2, NULL);
			else mvwchgat(group_win, 1+g_hover-g_offset, 1, group_win->_maxx-1, A_DIM, (true_hover ? 1 : 2), NULL);
			break;

		case 1:
			e_count = get_ecount(g[g_hover]);
			e = get_entries(get_ghead(g[g_hover]), e_count);
			fill_col(1);
			if(!resize) mvwchgat(entry_win, y_hl, 1, entry_win->_maxx-1, A_DIM, 1, NULL);
			else mvwchgat(entry_win, 1+e_hover-e_offset, 1, entry_win->_maxx-1, A_DIM, (true_hover ? 2 : 1), NULL);
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
	move(3, (WIDTH/4)+10);

	wrefresh(group_win);
	wrefresh(entry_win);
	return;
}

void trav_col(int new_i){
	int *focus = (true_hover ? &e_hover : &g_hover); //make it easy to know which column we are looking at
	int *offset = (true_hover ? &e_offset : &g_offset);
	int count = (true_hover ? e_count : g_count);
	int max_hl = HEIGHT-(3+GAP_SIZE); //for some reason, this works
	int min_hl = 5;
	int oob_flag = 0; //0 = none, 1 = bottom, 2 = top

	//check if the traversal is valid (i.e. not at top/bottom), alter if not
	if(new_i < 0) return;
	if(new_i >= count) new_i = count-1;

	//reset previously highlighted entry and group, change focus
	mvwchgat(entry_win, 1+e_hover-e_offset, 1, entry_win->_maxx-1, A_NORMAL, 0, NULL);
	mvwchgat(group_win, 1+g_hover-g_offset, 1, group_win->_maxx-1, A_NORMAL, 0, NULL);
	*focus = new_i;


	//check offsets relating to new highlight, make sure highlight did not go oob
	while(*focus-*offset+5 > max_hl){
		(*offset)++;
		oob_flag = 1;
	}
	while(*focus-*offset+5 < min_hl){
		(*offset)--;
		oob_flag = 2;
	}

	if(oob_flag > 0) (true_hover ? update_col(1, oob_flag-1, false) : update_col(0, oob_flag-1, false));

	//highlight newly hovered upon entry/group
	mvwchgat(entry_win, 1+e_hover-e_offset, 1, entry_win->_maxx-1, A_DIM, (true_hover ? 2 : 1), NULL);
	mvwchgat(group_win, 1+g_hover-g_offset, 1, group_win->_maxx-1, A_DIM, (true_hover ? 1 : 2), NULL);
	if(!true_hover){ //a little extra work regarding group hover
		e_offset = 0;
		update_col(1, 1, false);
		e_hover = 0;
	}

	wrefresh(group_win);
	wrefresh(entry_win);
	update_col(2, 0, false);
	return;
}

int locateChar(char input){
	int location = (true_hover ? e_hover : g_hover);
	bool fold_case = get_case_sensitivity();
	char first_char;
	int i;

	if(fold_case && input >= 97 && input <= 122) input -= 32;

	if(true_hover){ //hovering on entries
		for(i = location+1; i < e_count; i++){
			first_char = get_ename(e[i])[0];
			if(fold_case && first_char >= 97 && first_char <= 122) first_char -= 32;
			if(input == first_char){
				location = i;
				break;
			}
		}
	}

	else{ //hovering on groups
		for(i = location+1; i < g_count; i++){
			first_char = get_gname(g[i])[0];
			if(fold_case && first_char >= 97 && first_char <= 122) first_char -= 32;
			if(input == get_gname(g[i])[0]){
				location = i;
				break;
			}
		}
	}

	return location;
}

char *get_launch(){
	char *program = get_gprog(g[g_hover]);
	char *flags = get_gflags(g[g_hover]);
	char *path = get_epath(e[e_hover]);
	bool force = get_eforce(e[e_hover]);
	char *full_command = malloc(sizeof(char) * BUF_LEN);

	full_command[0] = '\0';

	//if the entry is an executable file (doesn't have a launcher)
	if(!(strcmp(program, "./"))){
		strcat(full_command, "\"");
		strcat(full_command, path);
		strcat(full_command, "\"");
	}

	else{
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
