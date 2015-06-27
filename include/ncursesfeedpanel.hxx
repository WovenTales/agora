#ifndef NCURSESFEEDPANEL_H
#define NCURSESFEEDPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>


class NcursesFeedPanel : public NcursesBasePanel<Database::Table::Article> {
  private:
	virtual std::string              name()  { return "feed"; };
	virtual Panel                    indic() { return Panel::FeedPanel; };
	virtual Database::Table::Article line()  { return Database::Table::Article::Title; };

  public:
	NcursesFeedPanel(bool focus = false) : NcursesBasePanel(focus) { update(focus); } ;

	virtual int height();
	virtual int width();
	virtual int x();
	virtual int y();

	void loadFeed(const Database&, const std::string&);
};

#endif
