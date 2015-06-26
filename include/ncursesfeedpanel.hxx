#ifndef NCURSESFEEDPANEL_H
#define NCURSESFEEDPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>


class NcursesFeedPanel : public NcursesPanel {
  private:
	virtual std::string            name()  { return "feed"; };
	virtual Panel                  indic() { return Panel::FeedPanel; };
	virtual Database::Column::Name line()  { return Database::Column::ArticleTitle; };

  public:
	NcursesFeedPanel(bool max = false) : NcursesPanel(max) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	void loadFeed(const Database&, const std::string&);
};

#endif
