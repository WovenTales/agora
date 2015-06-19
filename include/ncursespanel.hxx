#ifndef NCURSESPANEL_H
#define NCURSESPANEL_H

#include <ncursesui.hxx>

#include <curses.h>
#include <vector>


class NcursesPanel {
  protected:
	NcursesPanel(const NcursesPanel&);
	NcursesPanel &operator=(const NcursesPanel&);

	WINDOW   *panel;

	unsigned char &count;

	bool expanded;

	virtual std::string      name() =0;
	virtual NcursesUI::Panel indic() =0;

	NcursesPanel &operator--();

  public:
	NcursesPanel(bool = false);
	virtual ~NcursesPanel();

	virtual int height() =0;
	virtual int width() =0;

	virtual void draw() =0;
	void update();
};

#endif
