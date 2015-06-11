#include <ncursesui.hxx>

#include <curses.h>


NcursesUI NcursesUI::singleton;


NcursesUI::NcursesUI() {
	initscr();
	cbreak();
	noecho();

	getch();
}

NcursesUI::~NcursesUI() {
	close();
}


void NcursesUI::close() {
	endwin();
}
