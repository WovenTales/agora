#include <ncursesui.hxx>

#include <logger.hxx>
#include <ncursesarticlepanel.hxx>
#include <ncursesdatabasepanel.hxx>
#include <ncursesfeedpanel.hxx>

#include <curses.h>

using namespace std;


NcursesUI              NcursesUI::base(0);

std::vector<Database>  NcursesUI::db;

NcursesPanel::Panel    NcursesUI::focus        = NcursesPanel::Panel::DatabasePanel;
NcursesArticlePanel   *NcursesUI::articlePanel = NULL;
NcursesDatabasePanel  *NcursesUI::dbPanel      = NULL;
NcursesFeedPanel      *NcursesUI::feedPanel    = NULL;


NcursesUI::NcursesUI(const std::string &filename) {
	Log << "Initializing UI with database '" << filename << "'" << Log.ENDL;

	if (dbPanel) {
		Log << "Previous UI found; closing old UI";
		close();
	}

	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	articlePanel = new NcursesArticlePanel();
	dbPanel = new NcursesDatabasePanel(true);
	feedPanel = new NcursesFeedPanel();

	openDatabase(filename);
	openFeed("http://www.bigfinish.com/");
	openFeed("http://what-if.xkcd.com/feed.atom");
	openArticle("http://what-if.xkcd.com/135/");

	draw();

	bool loop = true;
	while (loop) {
		switch (getch()) {
			case 'a' : setFocus(NcursesPanel::Panel::ArticlePanel);  break;
			case 'd' : setFocus(NcursesPanel::Panel::DatabasePanel); break;
			case 'f' : setFocus(NcursesPanel::Panel::FeedPanel);     break;
			case 'h' : changeTab(false);                             break;
			case 'j' : scrollTab(true);                              break;
			case 'k' : scrollTab(false);                             break;
			case 'l' : changeTab(true);                              break;
			case 'q' : loop = false;                                 break;
		}
	}
}

NcursesUI::~NcursesUI() {
	close();
}


NcursesPanel *NcursesUI::activePanel() {
	NcursesPanel::Panel f = getFocus();
	     if (f == NcursesPanel::Panel::ArticlePanel)  return articlePanel;
	else if (f == NcursesPanel::Panel::DatabasePanel) return dbPanel;
	else if (f == NcursesPanel::Panel::FeedPanel)     return feedPanel;
}


//! \todo Winds up recreating panels every time called; find way to optimize.
void NcursesUI::setFocus(NcursesPanel::Panel f) {
	if (focus != f) {
		focus = f;

		articlePanel->update();
		dbPanel->update();
		feedPanel->update();
	}

	draw();
}

void NcursesUI::openArticle(const std::string &fID) {
	int index = dbPanel->getActiveTab();
	articlePanel->loadArticle(db[index], fID);
}

void NcursesUI::openDatabase(const std::string &filename) {
	Database tmp(filename);
	openDatabase(tmp);
}
void NcursesUI::openDatabase(Database &in) {
	db.push_back(in);
	dbPanel->loadDatabase(in);
}

void NcursesUI::openFeed(const std::string &fID) {
	int index = dbPanel->getActiveTab();
	feedPanel->loadFeed(db[index], fID);
}


void NcursesUI::close() {
	if (dbPanel) {
		Log << "Closing UI" << Log.ENDL;

		delete dbPanel;
		delete feedPanel;

		dbPanel = NULL;
		feedPanel = NULL;

		endwin();
	}
}

void NcursesUI::draw() {
	clear();
	refresh();

	articlePanel->draw();
	dbPanel->draw();
	feedPanel->draw();
}
