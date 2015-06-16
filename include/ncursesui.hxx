#ifndef NCURSESUI_H
#define NCURSESUI_H

#include <ncursesfeedlist.hxx>

#include <string>


class NcursesUI {
  private:
	//! Allow static singleton without initing window
	NcursesUI();

	static NcursesUI        base;
	static NcursesFeedList *feedlist;

	static void init(const std::string&);
	static void null();

  public:
	NcursesUI(const std::string&);
	virtual ~NcursesUI();

	static void close();
};

#endif
