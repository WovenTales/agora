#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

#include <curses.h>

class NcursesFeedList {
  private:
	WINDOW *list;

  public:
	NcursesFeedList();
	NcursesFeedList(int, int, int, int);
	virtual ~NcursesFeedList();

	void close();
};

#endif
