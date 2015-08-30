#include <ui/ncurses/ui.hxx>

#include <logger.hxx>
#include <ui/ncurses/articlepanel.hxx>
#include <ui/ncurses/databasepanel.hxx>
#include <ui/ncurses/feedpanel.hxx>

#include <curses.h>

using namespace std;


NcursesUI              NcursesUI::base(0);

std::vector<Database>  NcursesUI::db;

NcursesPanel::Panel    NcursesUI::focus        = NcursesPanel::Panel::DatabasePanel;
NcursesArticlePanel   *NcursesUI::articlePanel = NULL;
NcursesDatabasePanel  *NcursesUI::dbPanel      = NULL;
NcursesFeedPanel      *NcursesUI::feedPanel    = NULL;


/*! \param filename the database to load
 *  \todo Way too specialized; even beyond the tabs for testing, there's no
 *        reason to require a database, and it should certainly not be passed
 *        in the form of a filename rather than a Database object
 */
NcursesUI::NcursesUI(const std::string &filename) {
	Log << "Initializing UI with database '" << filename << "'" << Log.ENDL;

	// If any panel is non-null, the UI has already been initialized
	if (dbPanel) {
		Log << "Previous UI found; closing old UI";
		close();
	}

	// Initial ncurses parameter setup
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	// Create panels
	articlePanel = new NcursesArticlePanel();
	dbPanel = new NcursesDatabasePanel(true);
	feedPanel = new NcursesFeedPanel();

	// Open various tabs for testing
	openDatabase(filename);
	openFeed("http://www.bigfinish.com/");
	openFeed("http://what-if.xkcd.com/feed.atom");
	openArticle("http://what-if.xkcd.com/135/");

	draw();

	// Main control loop
	//! \todo This would be another good candidate to more deeply encapsulate
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


/*! \param f the panel to switch focus to
 *  \todo  Winds up recreating panels every time called; find way to optimize
 */
void NcursesUI::setFocus(NcursesPanel::Panel f) {
	if (focus != f) {
		focus = f;

		articlePanel->update();
		dbPanel->update();
		feedPanel->update();

		draw();
	}
}

/*! \param aID the ID of the article to open
 *  \todo  Rework to function with multiple Database tabs
 *         (the one currently open might not be the correct source)
 */
void NcursesUI::openArticle(const std::string &aID) {
	int index = dbPanel->getActiveTab();
	articlePanel->loadArticle(db[index], aID);
}

/*! \param filename the database to open
 *  \deprecated Really no reason to have this; better to encapsulate with other prototype
 */
void NcursesUI::openDatabase(const std::string &filename) {
	// Database destructor should keep object alive even after tmp leaves scope
	Database tmp(filename);
	openDatabase(tmp);
}
//! \param in the database to open
void NcursesUI::openDatabase(Database &in) {
	db.push_back(in);
	dbPanel->loadDatabase(in);
}

//! \param fID the ID of the feed to open
void NcursesUI::openFeed(const std::string &fID) {
	// As normal flow requires selecting the feed from the database panel,
	//   the one currently displayed should be the correct source
	//! \todo Allow more control over which database to use for behind-the-scenes control
	int index = dbPanel->getActiveTab();
	feedPanel->loadFeed(db[index], fID);
}


void NcursesUI::close() {
	// If any panel is non-null, the UI has already been initialized
	if (dbPanel) {
		Log << "Closing UI" << Log.ENDL;

		delete articlePanel;
		delete dbPanel;
		delete feedPanel;

		articlePanel = NULL;
		dbPanel = NULL;
		feedPanel = NULL;

		endwin();
	}
}

void NcursesUI::draw() {
	refresh();

	articlePanel->draw();
	dbPanel->draw();
	feedPanel->draw();
}
