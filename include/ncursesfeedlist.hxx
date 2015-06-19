#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

class Database;
#include <ncursespanel.hxx>
#include <ncursesui.hxx>

#include <vector>


class NcursesFeedList : public NcursesPanel {
  private:
	std::vector<std::string>                  dbs;
	std::vector<std::vector<Database::Data> > feeds;

	unsigned char index;

	virtual std::string      name()  { return "feed list"; };
	virtual NcursesUI::Panel indic() { return NcursesUI::FeedList; };

	void loadDatabase(Database&);

  public:
	NcursesFeedList(bool = false);
	NcursesFeedList(Database&, bool = false);

	virtual int height();
	virtual int width();

	virtual void draw();
};

#endif
