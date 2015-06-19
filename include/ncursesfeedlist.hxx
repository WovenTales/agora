#ifndef NCURSESFEEDLIST_H
#define NCURSESFEEDLIST_H

class Database;

#include <curses.h>

class NcursesFeedList {
  private:
	WINDOW   *list;
	//! \todo Allow opening of multiple databases.
	Database *db;

	unsigned char &count;

	bool expanded;

	NcursesFeedList &operator--();

	void init(int, int, int, int);

  public:
	//! Copy constructor.
	NcursesFeedList(const NcursesFeedList&);
	NcursesFeedList(Database* = NULL, bool = false);
	virtual ~NcursesFeedList();

	//! Standard assignment operator.
	NcursesFeedList &operator=(const NcursesFeedList&);

	int getHeight();
	int getWidth();

	void draw();
	void update();
};

#endif
