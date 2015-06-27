// Included in namespace NcursesListPanel

public:

NcursesListPanel(bool focus = false) : NcursesPanel(focus), data() {
	activeData = 0;
}


//! \bug Doesn't properly clear old render.
virtual bool scrollTab(bool down) {
	int d = data[activeTab].size() - 1;

	if (d <= 0) {
		return false;
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

	return true;
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
		if (i == activeData) {
			if (expanded) {
				wattron(panel, A_BOLD);
			}
			mvwaddch(panel, i + 1, 1, '*');
		}
		mvwaddnstr(panel, i + 1, 2, data[activeTab][i][line()].c_str(), width() - 3);
		if (expanded && (i == activeData)) {
			wattroff(panel, A_BOLD);
		}
	}
}
