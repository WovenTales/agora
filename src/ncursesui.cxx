#include <ncursesui.hxx>

#include <logger.hxx>

#include <curses.h>

using namespace std;


NcursesUI         NcursesUI::base(0);
unsigned char     NcursesUI::count(1);

Database         *NcursesUI::db = NULL;

NcursesUI::Panel  NcursesUI::focus = NcursesUI::FeedList;
NcursesFeedList  *NcursesUI::feedlist = NULL;


NcursesUI::NcursesUI(const std::string &filename) {
	Log << "Initializing UI with database '" << filename << "'" << Log.ENDL;

	if (feedlist) {
		Log << "Previous UI found...";
		close();
	}

	db = new Database(filename);
	++count;

	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	feedlist = new NcursesFeedList(db);

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


void NcursesUI::toggleFocus() {
	clear();
	Log << "Toggling focus to ";

	switch (focus) {
		case FeedList:
			//focus = None;
			Log << "None";
			break;
		case None:
			focus = FeedList;
			Log << "FeedList";
			break;
	}
	Log << Log.ENDL;

	feedlist->update();

	draw();
}


void NcursesUI::close() {
	if (--count == 0) {
		Log << "Closing UI" << Log.ENDL;

		if (feedlist) {
			delete feedlist;
			feedlist = NULL;
		}

		endwin();
	}
}

void NcursesUI::draw() {
	refresh();

	feedlist->draw();
}
