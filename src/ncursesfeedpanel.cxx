#include <ncursesfeedpanel.hxx>

#include <database.hxx>
#include <ncursesui.hxx>

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


void NcursesFeedPanel::loadFeed(const Database &db, const std::string &fID) {
	Database::FeedDataList t = db.getColumns<Database::Table::Feed>({ Database::Table::Feed::Title }, {{ Database::Table::Feed::ID, fID }});
	Database::ArticleDataList a = db.getColumns<Database::Table::Article>({ Database::Table::Article::Title }, {{ Database::Table::Article::FeedID, fID }});

	tabs.push_back(t[0][Database::Table::Feed::Title]);
	data.push_back(a);

	++activeTab;
}
