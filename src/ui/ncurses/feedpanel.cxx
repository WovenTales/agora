#include <ui/ncurses/feedpanel.hxx>

#include <feed.hxx>
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
	tabs.push_back(db.getColumns({ Feed::columns["title"] }, {{ Feed::columns.getID(), fID }})[0][Feed::columns["title"]]);
	// We only need to display some of the data
	data.push_back(db.getColumns({ Article::columns["title"] }, {{ Feed::columns.getID(), fID }}));

	++activeTab;
}
