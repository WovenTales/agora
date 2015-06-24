#include <ncursespanel.hxx>

#include <logger.hxx>
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
	int t = tabs.size();

	if (!t) {
		return;
	}

	if (right && (activeTab < t - 1)) {
		++activeTab;
	} else if (right) {
		activeTab = 0;
	} else if (!right && (activeTab > 0)) {
		--activeTab;
	} else if (!right) {
		activeTab = t - 1;
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

void NcursesPanel::update() {
	Log << "Updating " << name() << " panel" << Log.ENDL;
	if (panel) {
		delwin(panel);
	}

	expanded = (NcursesUI::getFocus() == indic());
	panel = newwin(height(), width(), y(), x());
}
