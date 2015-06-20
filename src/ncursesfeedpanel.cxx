#include <ncursesfeedpanel.hxx>

#include <agora.hxx>

#include <curses.h>

using namespace agora;


int NcursesFeedPanel::height() {
	if (expanded) {
		return 20;
	} else {
		return 10;
	}
}

int NcursesFeedPanel::width() {
	return COLS - (NcursesUI::getFocus() == NcursesUI::DatabasePanel ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesFeedPanel::x() {
	return (NcursesUI::getFocus() == NcursesUI::DatabasePanel ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesFeedPanel::y() {
	return 0;
}


void NcursesFeedPanel::fill() {
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
		mvwaddnstr(panel, i + 1, 2, replaceAll(data[activeTab][i]["aTitle"], "''", "'").c_str(), w - 3);
	}
}


void NcursesFeedPanel::loadFeed(Database &db, const std::string &fID) {
	Database::DataList *t = db.exec("SELECT fTitle FROM feeds WHERE fID = '" + replaceAll(fID, "'", "''") + "';", 0);
	Database::DataList *a = db.exec("SELECT aTitle FROM articles WHERE fID = '" + replaceAll(fID, "'", "''") + "';", 0);

	tabs.push_back((*t)[0]["fTitle"]);
	int s = a->size();
	for (int i = 0; i < s; ++i) {
		data.push_back(*a);
	}

	delete t;
	delete a;
}
