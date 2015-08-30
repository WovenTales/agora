#include <ui/ncurses/databasepanel.hxx>

#include <agora.hxx>
#include <database.hxx>

#include <curses.h>


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


//! \param db the Database to load
void NcursesDatabasePanel::loadDatabase(const Database &db) {
	tabs.push_back(db.getTitle());

	// We only need to display some of the data
	data.push_back(db.getColumns<Database::Table::Feed>(
				{ Database::Table::Feed::Title, Database::Table::Feed::ID }));

	++activeTab;
}
