// Included in namespace NcursesBasePanel

public:

NcursesBasePanel(bool focus = false) : tabs(), data() {
	panel = NULL;
	activeTab = -1;
	activeData = 0;
	expanded = focus;
}

virtual ~NcursesBasePanel() {
	if (panel) {
		delwin(panel);
		panel = NULL;
	}
}


//! \bug Doesn't properly clear old render of previous tab.
void changeTab(bool right) {
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
}

//! \todo Integrate into fill().
virtual void scrollTab(bool down) {
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
}


void draw() {
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

virtual void fill() {
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

void update(bool focus) {
	Log << "Updating " << name() << " panel" << Log.ENDL;
	if (panel) {
		delwin(panel);
	}

	//! \todo Figure out how to restore this functionality here, rather than pushing test upstream.
	//expanded = (NcursesUI::getFocus() == indic());
	expanded = focus;
	panel = newwin(height(), width(), y(), x());
}
