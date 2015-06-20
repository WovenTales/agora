#include <ncursespanel.hxx>

#include <logger.hxx>
#include <string>

using namespace std;


NcursesPanel::NcursesPanel(bool max) : tabs(), data() {
	panel = NULL;
	activeTab = 0;
	expanded = !max;
}

NcursesPanel::~NcursesPanel() {
	if (panel) {
		delwin(panel);
		panel = NULL;
	}
}


void NcursesPanel::draw() {
	Log << "Drawing " << name() << " panel...";

	box(panel, 0, 0);

	int t = tabs.size();
	string titles("");
	for (int i = 0; i < t; ++i) {
		if (i > 0) {
			titles += '|';
		}

		//! \bug Doesn't actually set attributes.
		if (i == activeTab) {
			attron(A_UNDERLINE);
		} else {
			titles += (i != 0 ? ' ' : '\n');
		}

		titles += tabs[i];

		if (i == activeTab) {
			attroff(A_UNDERLINE);
		} else {
			titles += (i != t ? ' ' : '\n');
		}
	}

	//! \todo Implement scrollable range (with proper behavior when collapsing horizontally).
	int x = (activeTab == 0 ? 0 : 1);
	mvwaddnstr(panel, 0, x, titles.c_str(), (width() == 0 ? COLS - 1 : width() - 1) - x - 1);

	if (t > 0) {
		fill();
	}

	Log << "Completed" << Log.ENDL;
	wrefresh(panel);
}

void NcursesPanel::update() {
	Log << "Updating " << name() << " panel...";
	if ((!panel) || (expanded != (NcursesUI::getFocus() == indic()))) {
		if (panel) {
			delwin(panel);
		}

		expanded = !expanded;
		panel = newwin(height(), width(), y(), x());
		Log << "Completed" << Log.ENDL;
	} else {
		Log << "Nothing to be done" << Log.ENDL;
	}
}
