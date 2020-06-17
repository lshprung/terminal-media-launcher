#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include "entry.h"
#include "group.h"
#include "read_cfg.h"
#define MAX_LEN 6
#define BUF_LEN 1024

void draw_title(int width);
void draw_win(char *title, int x, int y, int width, int height);
void fill_groups(GROUP **group_arr, int count, int startx, int starty, int maxx, int maxy);
char *trim_group_name(GROUP *g, int max_len);

int main(){
	static int width;
	static int height;
	bool tall = true; //is the window a certain height (tbd what the threshold should be TODO)
	bool wide = true; //is the window a certain width (tbd what the threshold should be TODO)
	GROUP **g;
	int g_count;

	initscr();

	width = getmaxx(stdscr);
	height = getmaxy(stdscr);

	//title at the top (Terminal Media Launcher) (23 chars)
	draw_title(width);

	//Draw Search Bar 2 spaces (3 spaces if window is big enough) under the title
	move(3, width/4);
	printw("[ Search: ");
	move(3, (width*3)/4);
	printw("]");
	move(3, (width/4)+10);

	//Draw Columns 
	//TODO create conditionals based on size of window)
	draw_win("GROUP", 0, 4, width/3, height-4);
	draw_win("ENTRY", (width/3), 4, width/3, height-4);
	draw_win("INFO", (2*width/3), 4, width/3, height-4);

	//Fill Groups
	cfg_interp(); //read the contents of the cfg file
	g = get_groups(); //retrieve results of previous function
	g_count = get_gcount(g); //retrieve number of groups in g
	fill_groups(g, g_count, 1, 5, ((width/3)-1), height);

	/*
	//DEBUG
	move(5, 1);
	printw("This is test output to test the limits of a window");
	//END DEBUG
	*/

	getch();
	endwin();
	return 0;
}

void draw_title(int width){
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

void draw_win(char *title, int x, int y, int width, int height){
	WINDOW *new;
	int title_len = strlen(title);

	new = newwin(height, width, y, x);
	refresh();
	attron(A_UNDERLINE);
	move(y, x+(width-title_len)/2);
	printw("%s", title);
	attroff(A_UNDERLINE);
	box(new, 0, 0);
	wrefresh(new);

	return;
}

void fill_groups(GROUP **group_arr, int count, int startx, int starty, int maxx, int maxy){
	int i;
	int max_len = maxx - startx; //longest possible string length that can be displayed in the window
	char *name;

	for(i = 0; i < count; i++){
		name = get_gname(group_arr[i]);

		//the name is too long, take the group to the trimming function
		if(strlen(name) > max_len) name = trim_group_name(group_arr[i], max_len);
		move(starty, startx);
		printw("%s", name);
		starty++;
	}

	return;
}

char *trim_group_name(GROUP *g, int max_len){
	char *name = get_gname(g);
	char *tok; //for use in finding relative path name
	char *tok_ahead;
	char *delims = "/\t\n";

	//group name and path are equivalent: special procedure
	if(!(strcmp(name, get_gpath(g)))){
		//find relative path name
		tok_ahead = strtok(name, delims);
		while(tok_ahead != NULL){
			tok = tok_ahead;
			tok_ahead = strtok(NULL, delims);
		}
		name = tok;
		if(strlen(name) <= max_len) return name;
	}

	name[max_len] = '\0';
	return name;
}
