#ifndef NCURSESPANEL_H
#define NCURSESPANEL_H

#include <database.hxx>
#include <logger.hxx>

#include <curses.h>
#include <string>
#include <vector>


//! \todo Smooth so don't take up more space than necessary (DVI shouldn't require resizing of panels).
#define DBPANEL_WIDTH_ACTIVE       60
#define DBPANEL_WIDTH_MINIMIZED    std::max(20, std::min(COLS / 5, 60))
#define FEEDPANEL_HEIGHT_ACTIVE    20
#define FEEDPANEL_HEIGHT_MINIMIZED 10


class NcursesPanel {
  public:
	enum struct Panel {
		ArticlePanel,
		DatabasePanel,
		FeedPanel
	};

	virtual int height() =0;
	virtual int width() =0;
	virtual int x() =0;
	virtual int y() =0;

	virtual unsigned char getActiveTab() =0;

	virtual void changeTab(bool) =0;
	virtual void scrollTab(bool) =0;

	virtual void draw() =0;
	virtual void update(bool) =0;
};

template <typename T>
class NcursesBasePanel : public NcursesPanel {
  protected:
	NcursesBasePanel(const NcursesPanel&);
	NcursesBasePanel &operator=(const NcursesPanel&);

	WINDOW                              *panel;
	std::vector<std::string>             tabs;
	std::vector<Database::DataList<T> >  data;

	unsigned char activeTab;
	unsigned char activeData;

	bool expanded;

	virtual std::string name() =0;
	virtual Panel       indic() =0;
	virtual T           line() =0;

	void update(const std::string&, Panel);

  public:
	virtual int height() =0;
	virtual int width() =0;
	virtual int x() =0;
	virtual int y() =0;

	unsigned char getActiveTab() { return activeTab; };

#include <ncursespanel.tcc>
};

#endif
