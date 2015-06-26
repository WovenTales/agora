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


void NcursesDatabasePanel::loadDatabase(const Database &db) {
	tabs.push_back(db.getTitle());

	Database::DataList tmp = db.getColumns({ Database::Column::FeedTitle, Database::Column::FeedID });
	data.push_back(tmp);

	++activeTab;
}
