#ifndef NCURSESPANEL_H
#define NCURSESPANEL_H

#include <ncursesui.hxx>

#include <curses.h>
#include <vector>


class NcursesPanel {
  protected:
	NcursesPanel(const NcursesPanel&);
	NcursesPanel &operator=(const NcursesPanel&);

	WINDOW                          *panel;
	std::vector<std::string>         tabs;
	std::vector<Database::DataList>  data;

	unsigned char                    activeTab;
	unsigned char                    activeData;

	bool expanded;

	virtual std::string      name() =0;
	virtual NcursesUI::Panel indic() =0;

	void update(const std::string&, NcursesUI::Panel);

  public:
	NcursesPanel(bool = false);
	virtual ~NcursesPanel();

	virtual int height() =0;
	virtual int width() =0;
	virtual int x() =0;
	virtual int y() =0;

	unsigned char getActiveTab() { return activeTab; };

	void draw();
	virtual void fill() =0;
	void update();
};

#endif
