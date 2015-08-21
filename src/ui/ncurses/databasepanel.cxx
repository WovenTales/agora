#include <ui/ncurses/databasepanel.hxx>

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

	Database::FeedDataList tmp = db.getColumns<Database::Table::Feed>({ Database::Table::Feed::Title, Database::Table::Feed::ID });
	data.push_back(tmp);

	++activeTab;
}
