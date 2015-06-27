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

  protected:
	WINDOW                              *panel;
	std::vector<std::string>             tabs;

	unsigned char activeTab;
	bool expanded;

	virtual std::string name() =0;
	virtual Panel       indic() =0;

  public:
	NcursesPanel(bool = false);
	virtual ~NcursesPanel();

	virtual int height() =0;
	virtual int width() =0;
	virtual int x() =0;
	virtual int y() =0;

	unsigned char getActiveTab() { return activeTab; };

	bool changeTab(bool);
	virtual bool scrollTab(bool) =0;

	virtual void draw();
	virtual void fill() =0;
	void update();
};

template <typename T>
class NcursesListPanel : public NcursesPanel {
  protected:
	std::vector<Database::DataList<T> >  data;

	unsigned char activeData;

	virtual T           line() =0;

	//void update(const std::string&, Panel);

/* public:
 * NcursesListPanel(bool = false);
 * virtual bool scrollTab(bool);
 * virtual void fill();
 */
#include <ncursespanel.tcc>
};

#endif
