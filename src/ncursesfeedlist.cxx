#include <ncursesfeedlist.hxx>

#include <curses.h>
#include <iostream>

using namespace std;


NcursesFeedList::NcursesFeedList() {
	list = newwin(0, 0, 0, 0);
}

NcursesFeedList::NcursesFeedList(int lines, int cols, int x, int y) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	
	if ((lines < 0) || (lines > (maxY - y))) {
		//! \todo Better error handling
		cerr << "lines out of bounds" << endl;
	}
	if ((cols < 0) || (cols > (maxX - x))) {
		//! \todo Better error handling
		cerr << "cols out of bounds" << endl;
	}
	if ((x < 0) || (x > (maxX - 1))) {
		//! \todo Better error handling
		cerr << "x out of bounds" << endl;
	}
	if ((y < 0) || (y > (maxY - 1))) {
		//! \todo Better error handling
		cerr << "y out of bounds" << endl;
	}

	list = newwin(lines, cols, x, y);

	wrefresh(list);
}

NcursesFeedList::~NcursesFeedList() {
	close();
}


void NcursesFeedList::close() {
	delwin(list);
}
