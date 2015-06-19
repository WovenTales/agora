#ifndef NCURSESUI_H
#define NCURSESUI_H

#include <database.hxx>
#include <ncursesfeedlist.hxx>

#include <string>


//! \todo Generates large amount of leaked memory.
class NcursesUI {
  public:
	enum Panel {
		FeedList,
		None
	};

  private:
	//! Allow static singleton without always initing blank window.
	NcursesUI(int) {};

	static NcursesUI        base;

	static Database        *db;
	//! \todo Try to get working without pointer?
	static NcursesFeedList *feedlist;

	static unsigned char    count;

	static Panel focus;

	static void close();

  public:
	NcursesUI(const std::string& = std::string(""));
	//! Copy constructor.
	NcursesUI(const NcursesUI&);
	virtual ~NcursesUI();

	//! Standard assignment operator.
	Database &operator=(const Database&);

	static Panel getFocus() { return focus; };

	static void toggleFocus();

	static void draw();
};

#endif
