#include <ncursesui.hxx>

#include <logger.hxx>
#include <ncursespanel.hxx>
#include <ncursesarticlepanel.hxx>
#include <ncursesdatabasepanel.hxx>
#include <ncursesfeedpanel.hxx>

#include <curses.h>

using namespace std;


NcursesUI              NcursesUI::base(0);

std::vector<Database>  NcursesUI::db;

NcursesUI::Panel       NcursesUI::focus        = NcursesUI::DatabasePanel;
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
			case 'a' : setFocus(ArticlePanel);
			           break;
			case 'd' : setFocus(DatabasePanel);
			           break;
			case 'f' : setFocus(FeedPanel);
			           break;
			case 'h' : changeTab(false);
			           break;
			case 'l' : changeTab(true);
			           break;
			case 'q' : loop = false;
			           break;
		}
	}
}

NcursesUI::~NcursesUI() {
	close();
}


void NcursesUI::changeTab(bool right) {
	Panel f = getFocus();

	NcursesPanel *p;
	if (f == ArticlePanel) {
		p = articlePanel;
	} else if (f == DatabasePanel) {
		p = dbPanel;
	} else if (f == FeedPanel) {
		p = feedPanel;
	}

	p->changeTab(right);
}

//! \todo Winds up recreating panels every time called; find way to optimize.
void NcursesUI::setFocus(Panel f) {
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
