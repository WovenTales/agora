#include <ncursesfeedlist.hxx>

#include <logger.hxx>

#include <iostream>

using namespace std;


NcursesFeedList::NcursesFeedList() {
	Logger::log("Initializing blank feed list");

	list = NULL;
}

NcursesFeedList::NcursesFeedList(const std::string &filename, int lines, int cols, int x, int y) {
	resize(lines, cols, x, y);
	init(filename);
}

NcursesFeedList::~NcursesFeedList() {
	Logger::log("Closing feed list");

	if (list) {
		delwin(list);
	}
}


void NcursesFeedList::init(const std::string &filename) {
	Logger::log("Initializing feed list with '", Logger::CONTINUE);
	Logger::log(filename, Logger::CONTINUE);
	Logger::log("'");

	waddstr(list, filename.c_str());

	wrefresh(list);
}


void NcursesFeedList::resize(int lines, int cols, int x, int y) {
	Logger::log("Resetting feed list to ", Logger::CONTINUE);
	Logger::log(lines, Logger::CONTINUE);
	Logger::log(", ", Logger::CONTINUE);
	Logger::log(cols, Logger::CONTINUE);
	Logger::log(", ", Logger::CONTINUE);
	Logger::log(x, Logger::CONTINUE);
	Logger::log(", ", Logger::CONTINUE);
	Logger::log(y);

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
	box(list, 0, 0);
}
