#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

class Database;

#include <curses.h>

class NcursesFeedList {
  private:
	WINDOW   *list;
	Database *db;

	unsigned char &count;

	NcursesFeedList &operator--();

	void init(int, int, int, int);

  public:
	//! Copy constructor.
	NcursesFeedList(const NcursesFeedList&);
	NcursesFeedList(Database* = NULL, int = 0, int = 0, int = 0, int = 0);
	virtual ~NcursesFeedList();

	//! Standard assignment operator.
	NcursesFeedList &operator=(const NcursesFeedList&);
};

#endif
