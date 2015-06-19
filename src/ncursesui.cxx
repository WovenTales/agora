#include <ncursesui.hxx>

#include <logger.hxx>
#include <ncursesfeedlist.hxx>

#include <curses.h>

using namespace std;


NcursesUI         NcursesUI::base(0);

Database         *NcursesUI::db = NULL;

NcursesUI::Panel  NcursesUI::focus = NcursesUI::FeedList;
NcursesFeedList  *NcursesUI::feedlist = NULL;


NcursesUI::NcursesUI(const std::string &filename) {
	Log << "Initializing UI with database '" << filename << "'" << Log.ENDL;

	if (feedlist) {
		Log << "Previous UI found; closing old UI";
		close();
	}

	db = new Database(filename);

	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	feedlist = new NcursesFeedList(*db);

	draw();

	int ch;
	while (true) {
		ch = getch();
		if (ch == '\t') {
			toggleFocus();
		} else {
			break;
		}
	}
}

NcursesUI::~NcursesUI() {
	close();
}


//! \todo Winds up recreating panels every time called; find way to optimize.
void NcursesUI::toggleFocus() {
	switch (focus) {
		case FeedList:
			focus = None;
			break;
		case None:
			focus = FeedList;
			break;
	}
	Log << Log.ENDL;

	feedlist->update();

	draw();
}


void NcursesUI::close() {
	Log << "Closing UI" << Log.ENDL;

	if (feedlist) {
		delete feedlist;
		feedlist = NULL;
	}

	endwin();
}

void NcursesUI::draw() {
	clear();
	refresh();

	feedlist->draw();
}
