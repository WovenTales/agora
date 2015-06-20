#include <ncursesdatabasepanel.hxx>

#include <agora.hxx>

#include <curses.h>

using namespace agora;
using namespace std;


int NcursesDatabasePanel::height() {
	return 0;
}

int NcursesDatabasePanel::width() {
	return (expanded ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesDatabasePanel::x() {
	return 0;
}

int NcursesDatabasePanel::y() {
	return 0;
}


void NcursesDatabasePanel::fill() {
	int s = data[activeTab].size();
	int h = (height() == 0 ? LINES : height());
	int w = (width() == 0 ? COLS : width());

	//! \todo Implement scrollable list.
	if (s > h - 2) {
		s = h - 2;
		//! \todo Could definitely look better.
		mvwaddch(panel, h - 2, w - 1, ACS_UARROW);
		mvwaddch(panel, h - 1, w - 1, ACS_DARROW);
	}
	for (int i = 0; i < s; ++i) {
		mvwaddnstr(panel, i + 1, 2, replaceAll(data[activeTab][i]["fTitle"], "''", "'").c_str(), w - 3);
	}
}


void NcursesDatabasePanel::loadDatabase(Database &db) {
	tabs.push_back(db.getTitle());

	Database::DataList *tmp = db.exec("SELECT fTitle FROM feeds;", 0);
	data.push_back(*tmp);
	delete tmp;
}
