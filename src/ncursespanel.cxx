#include <ncursespanel.hxx>

#include <logger.hxx>
#include <ncursesui.hxx>
#include <string>

using namespace std;


NcursesPanel::NcursesPanel(bool max) : tabs(), data() {
	panel = NULL;
	activeTab = -1;
	activeData = 0;
	expanded = !max;
}

NcursesPanel::~NcursesPanel() {
	if (panel) {
		delwin(panel);
		panel = NULL;
	}
}


//! \bug Doesn't properly clear old render of previous tab.
void NcursesPanel::changeTab(bool right) {
	int t = tabs.size() - 1;

	if (!t) {
		return;
	}

	if (right && (activeTab == t)) {
		activeTab = 0;
	} else if (right) {
		++activeTab;
	} else if (!right && (activeTab == 0)) {
		activeTab = t;
	} else if (!right) {
		--activeTab;
	}

	NcursesUI::draw();
}

void NcursesPanel::scrollTab(bool down) {
	int d = data.size() - 1;

	if (!d) {
		return;
	}

	if (down && (activeData == d)) {
		activeData = 0;
	} else if (down) {
		++activeData;
	} else if (!down && (activeData == 0)) {
		activeData = d;
	} else if (!down) {
		--activeData;
	}

	NcursesUI::draw();
}


void NcursesPanel::draw() {
	Log << "Drawing " << name() << " panel...";

	box(panel, 0, 0);

	int x = 1;
	wmove(panel, 0, x);
	wattron(panel, A_UNDERLINE);

	//! \todo Implement proper behavior when wider than panel.
	int t = tabs.size();
	bool before = true;
	for (int i = 0; i < t; ++i) {
		if (x + tabs[i].size() + (i == activeTab ? 4 : 1) > width() - 2) {
			break;
		}

		if (i == activeTab) {
			wattroff(panel, A_UNDERLINE);
			waddstr(panel, "| ");
		} else if (before) {
			waddch(panel, '|');
		}

		waddstr(panel, tabs[i].c_str());

		if (i == activeTab) {
			before = false;
			waddstr(panel, " |");
			wattron(panel, A_UNDERLINE);
		} else if (!before) {
			waddch(panel, '|');
		}
	}
	wattroff(panel, A_UNDERLINE);

	if (t > 0) {
		fill();
	}

	Log << "Completed" << Log.ENDL;
	wrefresh(panel);
}

void NcursesPanel::fill() {
	int s = data[activeTab].size();

	//! \todo Implement scrollable list.
	if (s > height() - 2) {
		s = height() - 2;
		//! \todo Could definitely look better.
		mvwaddch(panel, height() - 2, width() - 1, ACS_UARROW);
		mvwaddch(panel, height() - 1, width() - 1, ACS_DARROW);
	}
	for (int i = 0; i < s; ++i) {
		mvwaddnstr(panel, i + 1, 2, data[activeTab][i][line()].c_str(), width() - 3);
	}
}

void NcursesPanel::update() {
	Log << "Updating " << name() << " panel" << Log.ENDL;
	if (panel) {
		delwin(panel);
	}

	expanded = (NcursesUI::getFocus() == indic());
	panel = newwin(height(), width(), y(), x());
}
