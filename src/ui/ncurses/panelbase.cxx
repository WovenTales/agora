#include <ui/ncurses/panelbase.hxx>

#include <ui/ncurses/ui.hxx>


//! \param focus whether the panel should be created as active
NcursesPanel::NcursesPanel(bool focus) : tabs() {
	panel = NULL;
	activeTab = -1;
	expanded = focus;
}

NcursesPanel::~NcursesPanel() {
	if (panel) {
		delwin(panel);
	}
}


//! \param right direction of movement (rightward == true)
bool NcursesPanel::changeTab(bool right) {
	int t = tabs.size() - 1;

	// If there isn't another tab to switch to
	if (t <= 0) {
		return false;
	}

	if (right && (activeTab == t)) {  // Switching right && right-most active
		activeTab = 0;
	} else if (right) {               // Switching right
		++activeTab;
	} else if (activeTab == 0) {      // Switching left && left-most active
		activeTab = t;
	} else {                          // Switching left
		--activeTab;
	}

	return true;
}


void NcursesPanel::draw() {
	Log << "Drawing " << name() << " panel...";

	// Clear the old render
	//! \todo Make sure not unnecessarily deleting viable panels
	werase(panel);

	// Basic decoration
	//! \todo Differentiate active panel
	box(panel, 0, 0);

	// Setup for drawing tabs
	int x = 1;
	wmove(panel, 0, x);
	wattron(panel, A_UNDERLINE);

	// Draw tabbar
	int t = tabs.size();
	bool before = true;
	for (int i = 0; i < t; ++i) {
		// If the next tab will overflow the panel
		//! \todo Implement proper behavior when wider than panel
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

	// Only fill if there's content to fill with
	if (t > 0) {
		fill();
	}

	wrefresh(panel);
	Log << "Completed" << Log.ENDL;
}

//! \todo Implement quick return point to bypass unnecessary work
void NcursesPanel::update() {
	Log << "Updating " << name() << " panel" << Log.ENDL;
	if (panel) {
		delwin(panel);
	}

	expanded = (NcursesUI::getFocus() == indic());
	panel = newwin(height(), width(), y(), x());
}
