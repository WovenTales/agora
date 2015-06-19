#include <ncursesfeedlist.hxx>

#include <database.hxx>
#include <logger.hxx>
#include <ncursesui.hxx>

#include <iostream>
#include <string>
#include <vector>

using namespace std;


NcursesFeedList::NcursesFeedList(Database *data, bool max) : count(*new unsigned char(1)), db(data) {
	if (data) {
		expanded = max;
		update();
	} else {
		Log << "Initializing blank feed list" << Log.ENDL;

		list = NULL;
	}
}

/*! \param d the NcursesFeedList to copy
 */
NcursesFeedList::NcursesFeedList(const NcursesFeedList &l) : count(l.count) {
	Log << "Copying feed list" << Log.ENDL;

	list = l.list;

	++count;
}

NcursesFeedList::~NcursesFeedList() {
	Log << "Closing feed list" << Log.ENDL;

	if (--count == 0) {
		if (list) {
			// Potentially unnecessary, but doesn't hurt to include
			wborder(list, ' ', ' ', ' ',' ',' ',' ',' ',' ');

			wrefresh(list);

			delwin(list);
			list = NULL;
		}

		delete &count;
	}
}


NcursesFeedList &NcursesFeedList::operator=(const NcursesFeedList &d) {
	Log << "Assigning feed list" << Log.ENDL;

	count = d.count;

	++count;
}


int NcursesFeedList::getHeight() {
	return 0;
}

int NcursesFeedList::getWidth() {
	if (expanded) {
		return 60;
	} else {
		return 20;
	}
}


void NcursesFeedList::draw() {
	string title = db->getTitle();
	Log << "Initializing feed list with '" << title << "'" << Log.ENDL;

	box(list, 0, 0);
	mvwaddstr(list, 0, 0, title.c_str());

	vector<Database::Data> *feeds = db->exec("SELECT fTitle FROM feeds;", 0);

	int s = feeds->size();
	int h = (getHeight() == 0 ? LINES : getHeight());
	int w = (getWidth() == 0 ? COLS : getWidth());

	//! \todo Implement scrollable list.
	if (s > h - 2) {
		s = h - 2;
		mvwaddch(list, h - 2, w - 1, ACS_UARROW);
		mvwaddch(list, h - 1, w - 1, ACS_DARROW);
	}
	for (int i = 0; i < s; i++) {
		mvwaddnstr(list, i + 1, 2, (*feeds)[i]["fTitle"].c_str(), w - 3);
		Log << "Found feed '" << (*feeds)[i]["fTitle"] << "'" << Log.ENDL;
	}

	wrefresh(list);
}

void NcursesFeedList::update() {
	if ((!list) || (expanded != (NcursesUI::getFocus() == NcursesUI::FeedList))) {
		if (list) {
			delwin(list);
		}

		/*
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
		*/

		expanded = !expanded;
		list = newwin(getHeight(), getWidth(), 0, 0);
	}
}
