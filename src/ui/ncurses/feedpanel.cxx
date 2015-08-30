#include <ui/ncurses/feedpanel.hxx>

#include <database.hxx>
#include <ui/ncurses/ui.hxx>

#include <curses.h>


int NcursesFeedPanel::height() {
	return (expanded ? FEEDPANEL_HEIGHT_ACTIVE : FEEDPANEL_HEIGHT_MINIMIZED);
}

int NcursesFeedPanel::width() {
	return COLS - x();
}

int NcursesFeedPanel::x() {
	return (NcursesUI::getFocus() == Panel::DatabasePanel ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesFeedPanel::y() {
	return 0;
}


/*! \param db  the Database containing the desired article
 *  \param fID the article ID to search for
 */
void NcursesFeedPanel::loadFeed(const Database &db, const std::string &fID) {
	// Get feed title for tab name
	tabs.push_back(db.getColumns<Database::Table::Feed>(
				{ Database::Table::Feed::Title }, {{ Database::Table::Feed::ID, fID }}
			)[0][Database::Table::Feed::Title]);
	// We only need to display some of the data
	data.push_back(db.getColumns<Database::Table::Article>(
				{ Database::Table::Article::Title }, {{ Database::Table::Article::FeedID, fID }}));

	++activeTab;
}
