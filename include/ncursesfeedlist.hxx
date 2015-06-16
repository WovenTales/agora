#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

#include <curses.h>
#include <string>

class NcursesFeedList {
  private:
	WINDOW *list;

	void init(int, int, int, int);

  public:
	NcursesFeedList();

	NcursesFeedList(const std::string&, int = 0, int = 0, int = 0, int = 0);
	virtual ~NcursesFeedList();
};

#endif
