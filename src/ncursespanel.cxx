#include <ncursespanel.hxx>


NcursesPanel::NcursesPanel(bool max) : count(*new unsigned char(1)) {
	panel = NULL;
	expanded = max;
}

NcursesPanel::~NcursesPanel() {
	if (--count == 0) {
		if (panel) {
			delwin(panel);
			panel = NULL;
		}

		delete &count;
	}
}


void NcursesPanel::update() {
	if ((!panel) || (expanded != (NcursesUI::getFocus() == indic()))) {
		if (panel) {
			delwin(panel);
		}

		expanded = !expanded;
		panel = newwin(height(), width(), 0, 0);
	}
}
