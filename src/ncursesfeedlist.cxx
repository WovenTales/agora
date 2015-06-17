#include <ncursesfeedlist.hxx>

#include <database.hxx>
#include <logger.hxx>

#include <iostream>
#include <string>
#include <vector>

using namespace std;


NcursesFeedList::NcursesFeedList(Database *data, int lines, int cols, int x, int y) : count(*new unsigned char(1)), db(data) {
	if (data) {
		string title = db->getTitle();
		Log << "Initializing feed list with '" << title << "'" << Log.ENDL;
		refresh();

		init(lines, cols, x, y);

		box(list, 0, 0);
		waddstr(list, title.c_str());

		vector<Database::Data> *feeds = db->exec("SELECT fTitle FROM feeds;", 0);
		//! \todo Prevent overflow if more feeds than lines.
		int s = feeds->size();
		for (int i = 0; i < s; i++) {
			mvaddstr(i + 1, 2, (*feeds)[i]["fTitle"].c_str());
			Log << "Found feed '" << (*feeds)[i]["fTitle"] << "'" << Log.ENDL;
		}

		wrefresh(list);
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


void NcursesFeedList::init(int lines, int cols, int x, int y) {
	Log << "Resetting feed list to " << lines << ", " << cols << ", " << x << ", " << y << Log.ENDL;

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
}
