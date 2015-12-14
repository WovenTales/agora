#ifndef NCURSESFEEDPANEL_H
#define NCURSESFEEDPANEL_H

#include <article.hxx>
#include <database.hxx>
#include <ui/ncurses/panelbase.hxx>

#include <string>


//! Lists each Article contained in a Feed
class NcursesFeedPanel : public NcursesListPanel {
  private:
	virtual std::string             name()  { return "feed"; };
	virtual Panel                   indic() { return Panel::FeedPanel; };
	virtual Database::Table::Column line()  { return Article::columns["title"]; };

  public:
	//! \copydoc NcursesPanel(bool)
	NcursesFeedPanel(bool focus = false) : NcursesListPanel(focus) { update(); } ;

	virtual int height();
	virtual int width();
	virtual int x();
	virtual int y();

	//! Open a Feed in a new tab
	void loadFeed(const Database&, const std::string&);
};

#endif
