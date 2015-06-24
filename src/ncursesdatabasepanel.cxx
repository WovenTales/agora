#include <ncursesdatabasepanel.hxx>

#include <agora.hxx>
#include <database.hxx>

#include <curses.h>

using namespace agora;
using namespace std;


int NcursesDatabasePanel::height() {
	return LINES;
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

	//! \todo Implement scrollable list.
	if (s > height() - 2) {
		s = height() - 2;
		//! \todo Could definitely look better.
		mvwaddch(panel, height() - 2, width() - 1, ACS_UARROW);
		mvwaddch(panel, height() - 1, width() - 1, ACS_DARROW);
	}
	for (int i = 0; i < s; ++i) {
		mvwaddnstr(panel, i + 1, 2, data[activeTab][i][Database::Column::FeedTitle].c_str(), width() - 3);
	}
}


void NcursesDatabasePanel::loadDatabase(const Database &db) {
	tabs.push_back(db.getTitle());

	Database::DataList tmp = db.getColumns({ Database::Column::FeedTitle });
	data.push_back(tmp);

	++activeTab;
}
