#include <ncursesfeedpanel.hxx>

#include <agora.hxx>
#include <database.hxx>

#include <curses.h>

using namespace agora;


int NcursesFeedPanel::height() {
	return (expanded ? FEEDPANEL_HEIGHT_ACTIVE : FEEDPANEL_HEIGHT_MINIMIZED);
}

int NcursesFeedPanel::width() {
	return COLS - x();
}

int NcursesFeedPanel::x() {
	return (NcursesUI::getFocus() == NcursesUI::DatabasePanel ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesFeedPanel::y() {
	return 0;
}


void NcursesFeedPanel::fill() {
	int s = data[activeTab].size();

	//! \todo Implement scrollable list.
	if (s > height() - 2) {
		s = height() - 2;
		//! \todo Could definitely look better.
		mvwaddch(panel, height() - 2, width() - 1, ACS_UARROW);
		mvwaddch(panel, height() - 1, width() - 1, ACS_DARROW);
	}
	for (int i = 0; i < s; ++i) {
		mvwaddnstr(panel, i + 1, 2, data[activeTab][i][Database::Column::ArticleTitle].c_str(), width() - 3);
	}
}


void NcursesFeedPanel::loadFeed(const Database &db, const std::string &fID) {
	Database::DataList t = db.getColumns({ Database::Column::FeedTitle }, {{ Database::Column::FeedID, fID }});
	Database::DataList a = db.getColumns({ Database::Column::ArticleTitle }, {{ Database::Column::FeedID, fID }});

	tabs.push_back(t[0][Database::Column::FeedTitle]);
	data.push_back(a);

	++activeTab;
}
