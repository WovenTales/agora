#include <ncursesui.hxx>

#include <logger.hxx>
#include <ncursesdatabasepanel.hxx>

#include <curses.h>

using namespace std;


NcursesUI              NcursesUI::base(0);

std::vector<Database>  NcursesUI::db;

NcursesUI::Panel       NcursesUI::focus     = NcursesUI::DatabasePanel;
NcursesDatabasePanel  *NcursesUI::dbPanel   = NULL;


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

	dbPanel = new NcursesDatabasePanel(true);

	openDatabase(filename);

	draw();

	int ch;
	while (true) {
		ch = getch();
		if (ch == '\t') {
			toggleFocus();
		} else {
			break;
		}
	}
}

NcursesUI::~NcursesUI() {
	close();
}


//! \todo Winds up recreating panels every time called; find way to optimize.
void NcursesUI::toggleFocus() {
	switch (focus) {
		case DatabasePanel:
			focus = None;
			break;
		case None:
			focus = DatabasePanel;
			break;
	}
	dbPanel->update();

	draw();
}

void NcursesUI::openDatabase(const std::string &filename) {
	Database tmp(filename);
	openDatabase(tmp);
}
void NcursesUI::openDatabase(Database &in) {
	db.push_back(in);
	dbPanel->loadDatabase(in);
}


void NcursesUI::close() {
	if (dbPanel) {
		Log << "Closing UI" << Log.ENDL;

		delete dbPanel;
		dbPanel = NULL;

		endwin();
	}
}

void NcursesUI::draw() {
	clear();
	refresh();

	dbPanel->draw();
}
