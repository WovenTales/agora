#ifndef NCURSESFEEDPANEL_H
#define NCURSESFEEDPANEL_H

class Database;
#include <ncursespanel.hxx>
#include <ncursesui.hxx>


class NcursesFeedPanel : public NcursesPanel {
  private:
	virtual std::string      name()  { return "feed"; };
	virtual NcursesUI::Panel indic() { return NcursesUI::FeedPanel; };

  public:
	NcursesFeedPanel(bool max = false) : NcursesPanel(max) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	virtual void fill();

	void loadFeed(const Database&, const std::string&);
};

#endif
