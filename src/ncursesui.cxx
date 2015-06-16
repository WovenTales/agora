#include <ncursesui.hxx>

#include <logger.hxx>

#include <curses.h>

using namespace std;


NcursesUI NcursesUI::base;

NcursesFeedList *NcursesUI::feedlist = new NcursesFeedList;


NcursesUI::NcursesUI() {
	null();
}

NcursesUI::NcursesUI(const std::string &filename) {
	init(filename);
}

NcursesUI::~NcursesUI() {
	close();
}


void NcursesUI::close() {
	Logger::log("Closing UI");

	if (feedlist) {
		delete feedlist;

		null();
	}

	endwin();
}

void NcursesUI::init(const std::string &filename) {
	Logger::log("Initializing UI with database ", Logger::CONTINUE);
	Logger::log(filename);

	base.close();

	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	feedlist = new NcursesFeedList(filename);

	refresh();

	getch();
}

void NcursesUI::null() {
	Logger::log("Initializing blank UI");

	feedlist = NULL;
}
