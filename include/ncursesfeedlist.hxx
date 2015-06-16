#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

#include <curses.h>
#include <string>

class NcursesFeedList {
  private:
	WINDOW *list;

	void init(const std::string&);

	void resize(int, int, int, int);

  public:
	NcursesFeedList();

	//! \bug Doesn't actually display anything
	NcursesFeedList(const std::string&, int = 0, int = 0, int = 0, int = 0);
	virtual ~NcursesFeedList();
};

#endif
