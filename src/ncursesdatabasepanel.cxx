#include <ncursesdatabasepanel.hxx>

#include <agora.hxx>
#include <database.hxx>
#include <logger.hxx>
#include <ncursesui.hxx>

#include <iostream>
#include <curses.h>
#include <string>
#include <vector>

using namespace agora;
using namespace std;


NcursesDatabasePanel::NcursesDatabasePanel(bool max) : NcursesPanel(max), dbs(), feeds() {
	index = 0;
	update();
}

NcursesDatabasePanel::NcursesDatabasePanel(Database &data, bool max) : NcursesPanel(max), dbs(), feeds() {
	index = 0;
	loadDatabase(data);
	update();
}


int NcursesDatabasePanel::height() {
	return 0;
}

int NcursesDatabasePanel::width() {
	if (expanded) {
		return 60;
	} else {
		return 20;
	}
}


void NcursesDatabasePanel::draw() {
	Log << "Initializing feed list" << Log.ENDL;

	box(panel, 0, 0);

	int w = (width() == 0 ? COLS : width());


	int d = dbs.size();
	string titles("");
	for (int i = 0; i < d; ++i) {
		if (i > 0) {
			titles += '|';
		}

		//! \bug Doesn't actually set attributes.
		if (i == index) {
			attron(A_UNDERLINE);
		} else {
			titles += (i != 0 ? ' ' : '\n');
		}

		titles += dbs[i];

		if (i == index) {
			attroff(A_UNDERLINE);
		} else {
			titles += (i != d ? ' ' : '\n');
		}
	}

	//! \todo Implement scrollable range (with proper behavior when collapsing).
	int x = (index == 0 ? 0 : 1);
	mvwaddnstr(panel, 0, x, titles.c_str(), w - x - 1);

	if (d > 0) {
		int s = feeds[index].size();
		int h = (height() == 0 ? LINES : height());

		//! \todo Implement scrollable list.
		if (s > h - 2) {
			s = h - 2;
			//! \todo Could definitely look better.
			mvwaddch(panel, h - 2, w - 1, ACS_UARROW);
			mvwaddch(panel, h - 1, w - 1, ACS_DARROW);
		}
		for (int i = 0; i < s; ++i) {
			mvwaddnstr(panel, i + 1, 2, replaceAll(feeds[index][i]["fTitle"], "''", "'").c_str(), w - 3);
		}
	}

	wrefresh(panel);
}


void NcursesDatabasePanel::loadDatabase(Database &data) {
	dbs.push_back(data.getTitle());
	feeds.push_back(*data.exec("SELECT fTitle FROM feeds;", 0));
}
