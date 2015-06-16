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
	Log << "Closing UI" << Log.ENDL;

	if (feedlist) {
		delete feedlist;

		null();
	}

	endwin();
}

void NcursesUI::init(const std::string &filename) {
	Log << "Initializing UI with database '" << filename << "'" << Log.ENDL;

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
	Log << "Initializing blank UI" << Log.ENDL;

	feedlist = NULL;
}
