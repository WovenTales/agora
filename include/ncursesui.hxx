#ifndef NCURSESUI_H
#define NCURSESUI_H

#include <database.hxx>
class NcursesDatabasePanel;
class NcursesFeedPanel;

#include <curses.h>
#include <math.h>
#include <string>
#include <vector>


//! \todo Smooth so don't take up more space than necessary (DVI shouldn't require resizing of panels).
#define DBPANEL_WIDTH_ACTIVE    60
#define DBPANEL_WIDTH_MINIMIZED std::max(20, std::min(COLS / 5, 60))


//! \todo Generates large amount of leaked memory.
class NcursesUI {
  public:
	enum Panel {
		DatabasePanel,
		FeedPanel
	};

  private:
	//! Allow static singleton without always initing blank window.
	NcursesUI(int) {};

	NcursesUI(const NcursesUI&);
	NcursesUI &operator=(const NcursesUI&);

	static NcursesUI              base;

	static std::vector<Database>  db;
	static NcursesDatabasePanel  *dbPanel;
	static NcursesFeedPanel      *feedPanel;

	static Panel focus;

	static void close();

  public:
  	//! \todo Allow startup without active database.
	NcursesUI(const std::string& = std::string(""));
	virtual ~NcursesUI();

	static Panel getFocus() { return focus; };

	static void toggleFocus();

	static void openDatabase(const std::string&);
	static void openDatabase(Database&);
	static void openFeed(const std::string&);

	static void draw();
};

#endif
